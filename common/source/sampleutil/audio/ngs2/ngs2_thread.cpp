/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#include <sampleutil/sampleutil_common.h>
#include <sampleutil/sampleutil_error.h>
#include "ngs2_internal.h"
#include <audioout.h>
#include <user_service.h>

#pragma comment(lib,"libSceAudioOut_stub_weak.a")

namespace ssa = sce::SampleUtil::Audio;
namespace ssai = sce::SampleUtil::Audio::Internal;
namespace ssiu = sce::SampleUtil::Internal::Util;

ssai::AudioPortManager::AudioPortManager(void) : 
	m_state(STATE_STOP)
{
}

ssai::AudioPortManager::~AudioPortManager(void)
{
	finalize();
}

void* ssai::AudioPortManager::updateThread(void *argc)
{
	
	int32_t err;
	
	AudioPortManager *self;
	self = (AudioPortManager*)argc;
	void	*userArg = self->m_userArg;



	/*E Output loop */
	uint32_t bufferId = 0;

	while (self->m_isAborted == 0)
	{

		{
			SceNgs2RenderBufferInfo aInfo[6];
			
			aInfo[0].buffer       = self->m_mainBuffer[bufferId];
			aInfo[0].bufferSize   = sizeof(int16_t)*2*GRAIN_MAX;
			aInfo[0].numChannels  = SCE_NGS2_CHANNELS_2_0CH;
			aInfo[0].waveformType = SCE_NGS2_WAVEFORM_TYPE_PCM_I16L;
			
			aInfo[1].buffer       = self->m_bgmBuffer[bufferId];
			aInfo[1].bufferSize   = sizeof(int16_t)*2*GRAIN_MAX;
			aInfo[1].numChannels  = SCE_NGS2_CHANNELS_2_0CH;
			aInfo[1].waveformType = SCE_NGS2_WAVEFORM_TYPE_PCM_I16L;

			for(uint32_t i=0; i<4; i++){
				aInfo[2+i].buffer       = self->m_padSpeakerBuffer[i][bufferId];
				aInfo[2+i].bufferSize   = sizeof(int16_t)*GRAIN_MAX;
				aInfo[2+i].numChannels  = SCE_NGS2_CHANNELS_1_0CH;
				aInfo[2+i].waveformType = SCE_NGS2_WAVEFORM_TYPE_PCM_I16L;
			}

			/*E Call rendering handler */
			(*self->m_renderFunction)(aInfo,6, userArg);

/*
			int16_t a0 = *(int16_t*)aInfo[0].buffer;
			int16_t a1 = *(int16_t*)aInfo[1].buffer;

			int16_t a2 = *(int16_t*)aInfo[2].buffer;
			int16_t a3 = *(int16_t*)aInfo[3].buffer;
			int16_t a4 = *(int16_t*)aInfo[4].buffer;
			int16_t a5 = *(int16_t*)aInfo[5].buffer;
			*/
/*
			bool iszero = (a0==0) && (a1==0) && (a2==0)
				&& (a3==0) && (a4==0) && (a5==0);
			if(!iszero){
				printf("%#4x %#4x, %#4x %#4x, %#4x, %#4x\n", a0, a1, a2, a3, a4, a5);
			}
*/
/*
			bool iszero2 = (a2==0)	&& (a3==0) && (a4==0) && (a5==0);
			if(!iszero2){
				printf("%#4x %#4x, %#4x %#4x, %#4x, %#4x\n", a0, a1, a2, a3, a4, a5);
			}
*/

			self->m_mutex.lock();
			/*E Output audio */
			uint32_t numOutputs = 2;
			SceAudioOutOutputParam aAudioOutParam[6];
			memset(aAudioOutParam, 0, sizeof(aAudioOutParam));

			aAudioOutParam[0].handle = self->m_mainPortId;
			aAudioOutParam[0].ptr    = aInfo[0].buffer;

			aAudioOutParam[1].handle = self->m_bgmPortId;
			aAudioOutParam[1].ptr    = aInfo[1].buffer;

			for(uint32_t i = 0; i < 4; i++) {
				if(self->m_padSpeakerPorts[i].portId >= 0){
					aAudioOutParam[numOutputs].handle = self->m_padSpeakerPorts[i].portId;
					aAudioOutParam[numOutputs].ptr    = aInfo[2+i].buffer;
					numOutputs++;
				}
			}

			err = sceAudioOutOutputs(aAudioOutParam, numOutputs);
			if (err < 0) {
				printf("Error: sceAudioOutOutputs() %0x\n", err);
			}
		}
		self->m_mutex.unlock();
		/*E Swap buffer */
		bufferId = (bufferId==0) ? 1 : 0;
	}
		
	return NULL;
}

int32_t ssai::AudioPortManager::initialize( OnRenderCallback renderFunction, void *userArg)
{
	int ret = sceAudioOutInit();		// TODO: sceAudioOutInit 呼出しは起動時（初回）のみ必要
	if(ret != SCE_OK){
		// Ignore.
		//printf("sceAudioOutInit() failed : %#x\n", ret);
	}

	uint32_t numGrain = GRAIN_MAX;
	size_t stackSize = 128*1024;

	int32_t err;

	if( m_state == STATE_START ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	/*E Clear work */
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

	// ------------------------------------------------
	/* Initializing main and bgm port. */
	// ------------------------------------------------

	/*E Open audio out port */
	m_mainPortId = sceAudioOutOpen(
		SCE_USER_SERVICE_USER_ID_SYSTEM,
		SCE_AUDIO_OUT_PORT_TYPE_MAIN,
		0,
		m_numGrain,
		48000,
		SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);
	if(m_mainPortId < 0) {
		printf("Error: sceAudioOutOpen() %0x\n", m_mainPortId);
		return m_mainPortId;
	}

	m_bgmPortId = sceAudioOutOpen(
		SCE_USER_SERVICE_USER_ID_SYSTEM,
		SCE_AUDIO_OUT_PORT_TYPE_BGM,
		0,
		m_numGrain,
		48000,
		SCE_AUDIO_OUT_PARAM_FORMAT_S16_STEREO);
	if (m_bgmPortId < 0) {
		printf("Error: sceAudioOutOpen() %0x\n", m_bgmPortId);
		return m_bgmPortId;
	}

	/*E Set volume */
	int32_t aVolume[2];
	aVolume[0] = SCE_AUDIO_OUT_VOLUME_0dB;
	aVolume[1] = SCE_AUDIO_OUT_VOLUME_0dB;
	sceAudioOutSetVolume(m_mainPortId, (SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), aVolume);
	sceAudioOutSetVolume(m_bgmPortId,  (SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), aVolume);

	for(uint32_t i=0; i<4; i++){
		m_padSpeakerPorts[i].userId = SCE_USER_SERVICE_USER_ID_INVALID;
		m_padSpeakerPorts[i].portId = -1;
	}

	// ------------------------------------------------
	/*E Create update thread */
	// ------------------------------------------------

	ScePthreadAttr threadAttr;
	err = scePthreadAttrInit(&threadAttr);
	if (err < 0) {
		return err;
	}

	err = scePthreadAttrSetdetachstate(&threadAttr, SCE_PTHREAD_CREATE_JOINABLE);
	if (err < 0) {
		return err;
	}

// TODO: priority, cpuAffinityMask
/*
	int32_t priority = 0; //TODO
	SceKernelSchedParam schedParam;
	schedParam.sched_priority = threadInfo.priority;
	err = scePthreadAttrSetschedparam(&threadAttr, &schedParam);
	if (err < 0) {
		return err;
	}

	err = scePthreadAttrSetaffinity(&threadAttr, threadInfo.cpuAffinityMask);
	if (err < 0) {
		return err;
	}
*/

	err = scePthreadAttrSetstacksize(&threadAttr, stackSize);
	if (err < 0) {
		return err;
	}

	err = scePthreadCreate(&m_thread, &threadAttr, updateThread, (void*)this, "audio_out_thread");
	scePthreadAttrDestroy(&threadAttr);
	if (err < 0) {
		return err;
	}

	m_state = STATE_START;
	return SCE_OK;
}

int32_t ssai::AudioPortManager::finalize(void)
{
	int ret;
	/*E Shutdown update thread */

	if ( m_state == STATE_STOP ) {
		return SCE_OK;
	}

	m_isAborted = 1;

	scePthreadJoin(m_thread, NULL);
	m_state = STATE_STOP;

	for(uint32_t i=0; i<4; i++){
		if(m_padSpeakerPorts[i].portId > 0){
			closePadSpeakerPort(m_padSpeakerPorts[i].userId);
		}
	}


	/*E Flush buffer */
	sceAudioOutOutput(m_mainPortId, NULL);
	sceAudioOutOutput(m_bgmPortId, NULL);

	/*E Release audio output port */
	ret = sceAudioOutClose(m_mainPortId);
	if (ret != SCE_OK) {
		printf("Error: sceAudioOutClose() %0x\n", ret);
	}
	ret = sceAudioOutClose(m_bgmPortId);
	if (ret != SCE_OK) {
		printf("Error: sceAudioOutClose() %0x\n", ret);
	}

	/* Clear work */
	m_isAborted = false;	// Is aborted ?
	m_numGrain = 0;			// Nubmer of grain
	m_renderFunction = NULL;
	m_userArg = NULL;

	return SCE_OK;
}

int ssai::AudioPortManager::openPadSpeakerPort(SceUserServiceUserId userId, uint32_t index)
{
	if(index > 4){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}


	ssiu::FocusLock focusLock(&m_mutex);
	if(m_padSpeakerPorts[index].portId >= 0){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	int portId = sceAudioOutOpen(
		userId,
		SCE_AUDIO_OUT_PORT_TYPE_PADSPK,
		0,
		m_numGrain,
		48000,
		SCE_AUDIO_OUT_PARAM_FORMAT_S16_MONO);
	if (portId < 0) {
		printf("Error: sceAudioOutOpen() %0x\n", portId);
		return portId;
	}

	//int ret = sceAudioOutSetMixLevelPadSpk( portId, SCE_AUDIO_OUT_MIXLEVEL_PADSPK_0dB );
	//SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	/*E Set volume */
	int32_t aVolume[2];
	aVolume[0] = SCE_AUDIO_OUT_VOLUME_0dB;
	aVolume[1] = SCE_AUDIO_OUT_VOLUME_0dB;
	sceAudioOutSetVolume(portId,  SCE_AUDIO_VOLUME_FLAG_L_CH  , aVolume);

	m_padSpeakerPorts[index].portId = portId;
	m_padSpeakerPorts[index].userId = userId;

	return SCE_OK;
}

int ssai::AudioPortManager::closePadSpeakerPort(SceUserServiceUserId userId)
{
	ssiu::FocusLock focusLock(&m_mutex);
	int32_t index = -1;
	for(uint32_t i=0; i<4; i++){
		if(m_padSpeakerPorts[i].userId == userId){
			index = i;
			break;
		}
	}
	if(index < 0){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	sceAudioOutOutput(m_padSpeakerPorts[index].portId, NULL);

	int ret = sceAudioOutClose(m_padSpeakerPorts[index].portId);
	if (ret != SCE_OK) {
		printf("Error: sceAudioOutClose() %0x\n", ret);
	}

	m_padSpeakerPorts[index].portId = -1;
	m_padSpeakerPorts[index].userId = SCE_USER_SERVICE_USER_ID_INVALID;
	return SCE_OK;
}


#endif // defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS