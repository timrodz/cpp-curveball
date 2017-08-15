/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */
#include "stdafx.h" 

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>
#include <audioout.h>
#include <math.h>

#include <libdbg.h>
#include <assert.h>
#include "ngs_internal.h"

namespace ssai = sce::SampleUtil::Audio::Internal;

/*E Update thread */

int32_t ssai::MainOutWorkerThread::updateThread(uint32_t args, void *argc)
{
	
	int32_t err;
	
	MainOutWorkerThread *work;
	work = *(MainOutWorkerThread**)argc;
	void	*userArg = work->m_userArg;

	/*E Open audio out port */
	int32_t mainPortId = sceAudioOutOpenPort(
		SCE_AUDIO_OUT_PORT_TYPE_MAIN,
		work->m_numGrain,
		48000,
		SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);

	if(mainPortId < 0) {
		printf("Error: sceAudioOutOpenPort() %0x\n", mainPortId);
		return -1;
	}

	int32_t bgmPortId = sceAudioOutOpenPort(
		SCE_AUDIO_OUT_PORT_TYPE_BGM,	
		work->m_numGrain,
		48000,
		SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);
	if (bgmPortId < 0) {
		printf("Error: sceAudioOutOpenPort() %0x\n", bgmPortId);
		return -1;
	}

	/*E Set volume */
	int32_t aVolume[2];
	aVolume[0] = SCE_AUDIO_VOLUME_0dB;
	aVolume[1] = SCE_AUDIO_VOLUME_0dB;
	sceAudioOutSetVolume(mainPortId, (SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), aVolume);
	sceAudioOutSetVolume(bgmPortId,  (SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), aVolume);

	/*E Output loop */
	uint32_t bufferId = 0;
	int16_t  mainBuffer[2][GRAIN_MAX];
	int16_t  bgmBuffer[2][GRAIN_MAX];

	while (work->m_isAborted == 0)
	{
		/*E Call rendering handler */
		(*work->m_renderFunction)(mainBuffer[bufferId], sizeof(int16_t) * GRAIN_MAX, 
			                      bgmBuffer[bufferId],  sizeof(int16_t) * GRAIN_MAX, 
			                      userArg);

		/*E Output audio */
		err = sceAudioOutOutput(mainPortId, mainBuffer[bufferId]);
		if (err < 0) {
			printf("Error: sceAudioOutOutput() %0x\n", err);
			break;
		}
		err = sceAudioOutOutput(bgmPortId, bgmBuffer[bufferId]);
		if (err < 0) {
			printf("Error: sceAudioOutOutput() %0x\n", err);
			break;
		}

		/*E Swap buffer */
		bufferId ^= 1;
	}

	/*E Flush buffer */
	sceAudioOutOutput(mainPortId, NULL);
	sceAudioOutOutput(bgmPortId, NULL);

	/*E Release audio output port */
	err = sceAudioOutReleasePort(mainPortId);
	if (err != SCE_OK) {
		printf("Error: sceAudioOutReleasePort() %0x\n", err);
	}
	err = sceAudioOutReleasePort(bgmPortId);
	if (err != SCE_OK) {
		printf("Error: sceAudioOutReleasePort() %0x\n", err);
	}
	return 0;
}


/*E Start audio out service */

int32_t ssai::MainOutWorkerThread::start(AudioOutRenderFunction renderFunction,
					               void					*userArg,
								   uint32_t numGrain, AudioOutThreadInfo threadInfo)
{

	int32_t err;

	/*E Clear work */
	m_updateThreadId = SCE_UID_INVALID_UID;			// Update thread ID
	m_isAborted = false;	// Is aborted ?



	/*E Check parameter */

	if (GRAIN_MAX < numGrain) {
		printf("Error: Over flow number of output grain %d\n", numGrain);
		return -1;
	}

	m_numGrain = numGrain;


	if(renderFunction == NULL){
		//if (renderHandler == NULL) {
		printf("Error: Invalid rendering handler %p\n", renderFunction);
		return -1;
	}

	m_renderFunction = renderFunction;
	m_userArg      = userArg;

	/*E Create update thread */

	m_updateThreadId = sceKernelCreateThread(
		"audio_out_thread",
		updateThread,
		threadInfo.priority,
		threadInfo.stackSize,
		0,
		threadInfo.cpuAffinityMask, 
		SCE_NULL);
	if(m_updateThreadId < 0){
		return m_updateThreadId;
	}

	{
		/*E Start update thread */
		MainOutWorkerThread* pWork = this;
		err = sceKernelStartThread(m_updateThreadId, sizeof(MainOutWorkerThread*), &pWork);
		if (err < 0) {
			sceKernelDeleteThread(m_updateThreadId);
			m_updateThreadId = SCE_UID_INVALID_UID;
			return err;
		}
	}

	return SCE_OK;
}

/*E Stop audio out service */

int32_t ssai::MainOutWorkerThread::stop(void)
{
	/*E Shutdown update thread */

	SceInt32 exitStatus;

	if(m_updateThreadId < 0){
		return SCE_OK;
	}

	m_isAborted = 1;

	sceKernelWaitThreadEnd(m_updateThreadId, &exitStatus, SCE_NULL);
	sceKernelDeleteThread(m_updateThreadId);
	m_updateThreadId = SCE_UID_INVALID_UID;

	/* Clear work */
	m_updateThreadId = SCE_UID_INVALID_UID;			// Update thread ID
	m_isAborted = false;	// Is aborted ?
	m_numGrain = 0;				// Nubmer of grain
	m_renderFunction = NULL;

	m_userArg = NULL;

	return SCE_OK;
}



#endif /*_SCE_TARGET_OS_PSP2 */