/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <ngs.h>
#include "ngs_internal.h"
#include <libdbg.h>

namespace ssai = sce::SampleUtil::Audio::Internal;


#pragma region PcmModule



int ssai::PcmModule::initialize(SceNgsHVoice voiceHandler, int codecType, int sampleRate, int channelNum)
{
	SCE_SAMPLE_UTIL_ASSERT((codecType == SCE_NGS_PLAYER_TYPE_PCM) || (codecType == SCE_NGS_PLAYER_TYPE_ADPCM));
	m_voiceHandler = voiceHandler;
	SceNgsBufferInfo	bufferInfo;

	int returnCode;
	// Get player parameters
	returnCode = sceNgsVoiceLockParams( this->m_voiceHandler, 
		SCE_NGS_VOICE_T1_PCM_PLAYER, 
		SCE_NGS_PLAYER_PARAMS_STRUCT_ID, 
		&bufferInfo );
	if( returnCode != SCE_NGS_OK ){
		SCE_DBG_LOG_ERROR( "setParamsToPcmModule: sceNgsVoiceLockParams() failed: 0x%08X \n", returnCode );
		return returnCode;
	}

	// Set player parameters
	memset( bufferInfo.data, 0, bufferInfo.size );
	SceNgsPlayerParams *pPcmParams = (SceNgsPlayerParams *)bufferInfo.data;
	pPcmParams->desc.id            = SCE_NGS_PLAYER_PARAMS_STRUCT_ID;
	pPcmParams->desc.size          = sizeof(SceNgsPlayerParams);
	pPcmParams->fPlaybackFrequency = sampleRate;
	pPcmParams->fPlaybackScalar    = 1.0f;
	pPcmParams->nLeadInSamples     = 0;
	pPcmParams->nChannels          = channelNum;
	pPcmParams->nStartBuffer = 0;

	if( channelNum == 1 ) {
		pPcmParams->nChannelMap[0] = SCE_NGS_PLAYER_LEFT_CHANNEL;
		pPcmParams->nChannelMap[1] = SCE_NGS_PLAYER_LEFT_CHANNEL;
	}else {
		pPcmParams->nChannelMap[0] = SCE_NGS_PLAYER_LEFT_CHANNEL;
		pPcmParams->nChannelMap[1] = SCE_NGS_PLAYER_RIGHT_CHANNEL;
	}

	pPcmParams->nType            = codecType; // SCE_NGS_PLAYER_TYPE_PCM (0)  or SCE_NGS_PLAYER_TYPE_ADPCM (1)  

	//E Update the player parameter
	//J Playerパラメータの更新
	returnCode = sceNgsVoiceUnlockParams( this->m_voiceHandler, SCE_NGS_VOICE_T1_PCM_PLAYER );
	if( returnCode != SCE_NGS_OK )
	{
		SCE_DBG_LOG_ERROR( "setParamsToPcmModule: sceNgsVoiceUnlockParams() failed: 0x%08X\n", returnCode );
		if( returnCode == SCE_NGS_ERROR_PARAM_OUT_OF_RANGE ) {
			char str[128];
			sceNgsVoiceGetParamsOutOfRange( this->m_voiceHandler, SCE_NGS_VOICE_T1_PCM_PLAYER, str );
			printf("PARAM ERROR is \"%s\"\n", str );
		}
		return returnCode;
	}
	m_param = NULL;


	//E Cancel the bypass of the player module
	//J player module のバイパスを解除
	returnCode = sceNgsVoiceBypassModule( this->m_voiceHandler, SCE_NGS_VOICE_T1_PCM_PLAYER, SCE_NGS_MODULE_FLAG_NOT_BYPASSED );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_OK);

	return SCE_OK;

}



SceNgsPlayerParams *ssai::PcmModule::lock(void)
{
	SceNgsBufferInfo	bufferInfo;
	// Get player parameters
	int returnCode = sceNgsVoiceLockParams( this->m_voiceHandler, 
		SCE_NGS_VOICE_T1_PCM_PLAYER, 
		SCE_NGS_PLAYER_PARAMS_STRUCT_ID, 
		&bufferInfo );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_OK);

	m_param = (SceNgsPlayerParams *)bufferInfo.data;
	return m_param;
}

void ssai::PcmModule::unlock(void)
{
	int returnCode = sceNgsVoiceUnlockParams( this->m_voiceHandler, SCE_NGS_VOICE_T1_PCM_PLAYER );
	if( returnCode != SCE_NGS_OK )
	{
		printf( "setParamsToPcmModule: sceNgsVoiceUnlockParams() failed: 0x%08X\n", returnCode );
		if( returnCode == SCE_NGS_ERROR_PARAM_OUT_OF_RANGE ) {
			char str[128];
			sceNgsVoiceGetParamsOutOfRange( this->m_voiceHandler, SCE_NGS_VOICE_T1_PCM_PLAYER, str );
			SCE_DBG_LOG_INFO( "PARAM ERROR is \"%s\"\n", str );
		}
	}
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_OK );
	m_param = NULL;
}

void ssai::PcmModule::setBuffer(uint32_t buffIndex, 
										 const void* pBuffer,
										 SceInt32 nNumBytes,
										 SceInt16 nLoopCount,
										 SceInt16 nNextBuff)
{
	m_param->buffs[buffIndex].pBuffer              = pBuffer;
	m_param->buffs[buffIndex].nNumBytes            = nNumBytes;
	m_param->buffs[buffIndex].nLoopCount           = (nLoopCount<0) ? SCE_NGS_PLAYER_LOOP_CONTINUOUS : nLoopCount;
	m_param->buffs[buffIndex].nNextBuff            = nNextBuff;
}

void ssai::PcmModule::setStartBuffer(int index)
{
	m_param->nStartBuffer = index;
}

void ssai::PcmModule::setCallback(const SceNgsModuleCallbackFunc callbackFuncPtr, void* arg)
{
	int returnCode = sceNgsVoiceSetModuleCallback(this->m_voiceHandler, SCE_NGS_VOICE_T1_PCM_PLAYER, callbackFuncPtr, arg);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_OK);
}




#pragma endregion 

#pragma region At9Module


SceNgsAT9Params *ssai::At9Module::lock(void)
{
	SceNgsBufferInfo	bufferInfo;
	int returnCode  = sceNgsVoiceLockParams( m_voiceHandler, 
		SCE_NGS_VOICE_AT9_PLAYER, 
		SCE_NGS_AT9_PARAMS_STRUCT_ID, &bufferInfo );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_NGS_OK);

	m_param = (SceNgsAT9Params *)bufferInfo.data;
	return m_param;
}

void ssai::At9Module::setBuffer(uint32_t buffIndex, 
	const void* pBuffer,
	SceInt32 nNumBytes,
	SceInt16 nLoopCount,
	SceInt16 nNextBuff,
	bool isStart,
	bool isEnd)
{
	lock();
	m_param->buffs[buffIndex].pBuffer              = pBuffer;
	m_param->buffs[buffIndex].nNumBytes            = (nNumBytes<0) ? 0 : nNumBytes;
	m_param->buffs[buffIndex].nLoopCount           = nLoopCount ;
	m_param->buffs[buffIndex].nNextBuff            = nNextBuff;
	m_param->buffs[buffIndex].nSamplesDiscardStart = isStart ? m_skipBufInfo.nStartSkip : 0;
	m_param->buffs[buffIndex].nSamplesDiscardEnd   = isEnd   ? m_skipBufInfo.nEndSkip   : 0;
	unlock();
}


void ssai::At9Module::unlock(void)
{
	int returnCode = sceNgsVoiceUnlockParams( m_voiceHandler, SCE_NGS_VOICE_AT9_PLAYER );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_NGS_OK);
	m_param = NULL;
}

int ssai::At9Module::initialize(SceNgsHVoice voiceHandler,  
	                             int sampleRate, 
								 int channelNum, 
								 uint32_t loopStartOffset,
								 uint32_t loopEndOffset,
								 int32_t  at9configData)
{
	int returnCode;
	SceNgsBufferInfo	bufferInfo;

	returnCode = sceNgsAT9GetSectionDetails(
		loopStartOffset | SCE_NGS_SAMPLE_OFFSET_FROM_AT9_HEADER,
		loopEndOffset - loopStartOffset + 1,
		at9configData,
		&m_skipBufInfo);

	m_voiceHandler = voiceHandler;
	returnCode  = sceNgsVoiceLockParams( m_voiceHandler, 
		SCE_NGS_VOICE_AT9_PLAYER, 
		SCE_NGS_AT9_PARAMS_STRUCT_ID, &bufferInfo );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_NGS_OK);

	// Set player parameters
	memset( bufferInfo.data, 0, bufferInfo.size );
	SceNgsAT9Params	*pAt9Params = (SceNgsAT9Params *)bufferInfo.data;
	pAt9Params->desc.id     = SCE_NGS_AT9_PARAMS_STRUCT_ID;
	pAt9Params->desc.size   = sizeof(SceNgsAT9Params);

	pAt9Params->fPlaybackFrequency = sampleRate;
	pAt9Params->fPlaybackScalar    = 1.0f;
	pAt9Params->nLeadInSamples     = 0;
	pAt9Params->nChannels          = channelNum;

	pAt9Params->configData         = at9configData;

	if( channelNum == 1 ) {
		pAt9Params->nChannelMap[0] = SCE_NGS_PLAYER_LEFT_CHANNEL;
		pAt9Params->nChannelMap[1] = SCE_NGS_PLAYER_LEFT_CHANNEL;
	} else	{
		pAt9Params->nChannelMap[0] = SCE_NGS_PLAYER_LEFT_CHANNEL;
		pAt9Params->nChannelMap[1] = SCE_NGS_PLAYER_RIGHT_CHANNEL;
	}

	returnCode = sceNgsVoiceUnlockParams( m_voiceHandler, SCE_NGS_VOICE_AT9_PLAYER );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_NGS_OK);

	//E Cancel the bypass of the player module
	//J at9 module のバイパスを解除
	returnCode = sceNgsVoiceBypassModule( m_voiceHandler, SCE_NGS_VOICE_AT9_PLAYER, SCE_NGS_MODULE_FLAG_NOT_BYPASSED );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_NGS_OK);

	m_param = NULL;
	(void)returnCode;
	return SCE_OK;
}

void ssai::At9Module::setCallback(const SceNgsModuleCallbackFunc callbackFuncPtr, void* arg)
{
	int returnCode = sceNgsVoiceSetModuleCallback(this->m_voiceHandler, SCE_NGS_VOICE_AT9_PLAYER, callbackFuncPtr, arg);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_OK);
}

void ssai::At9Module::notifyLoopedBuffer(int finishedLoopNum, const void *buffer)
{
	SceNgsAT9Params *params = lock();

	for(uint32_t i=0; i<SCE_NGS_AT9_MAX_BUFFERS; i++){
		bool isLoopedBuffer = (((uintptr_t)params->buffs[i].pBuffer) <= (uintptr_t)buffer)
			&& (((uintptr_t)params->buffs[i].pBuffer) + params->buffs[i].nNumBytes > (uintptr_t)buffer);

		if(isLoopedBuffer){
			params->buffs[i].nSamplesDiscardStart = m_skipBufInfo.nStartSkip;
		}else{
			params->buffs[i].nSamplesDiscardStart = 0;
		}

		if(params->buffs[i].nLoopCount > 0){
			params->buffs[i].nSamplesDiscardEnd = m_skipBufInfo.nEndSkip;
		}else{
			params->buffs[i].nSamplesDiscardEnd = 0;
		}

		if(isLoopedBuffer && (params->buffs[i].nLoopCount <= finishedLoopNum)){
			params->buffs[i].nSamplesDiscardEnd = 0;
		}
	}

	unlock();
}

void ssai::At9Module::resetBuffer(void)
{
	SceNgsAT9Params *param = lock();
	memset(param->buffs, 0, sizeof(param->buffs));
	param->nLeadInSamples = 0;
	unlock();
}
void ssai::At9Module::setPitch(float pitch)
{
	SceNgsAT9Params *param = lock();
	param->fPlaybackScalar = pitch;
	unlock();
}



#pragma endregion 


int ssai::VoiceModule::initializeAsAt9(SceNgsHVoice voiceHandler,
	int sampleRate,
	int channelNum,
	uint32_t loopStartOffset,
	uint32_t loopEndOffset,
	int32_t  at9configData)
{
	m_onSwappedBufferCallback = NULL;
	m_onLoopedBufferCallback  = NULL;
	m_userArg                 = NULL;

	m_nextBufferIndex = 0;
	SCE_SAMPLE_UTIL_ASSERT(NUM_BUFFERS <= SCE_NGS_AT9_MAX_BUFFERS);
	m_type = MODULE_AT9;
	int ret = m_at9.initialize(voiceHandler, 
		sampleRate,  channelNum,   loopStartOffset,    loopEndOffset,	  at9configData);
	if(ret != SCE_OK){
		return ret;
	}

	m_at9.setCallback(at9ModuleCallback, this);

	return initializeMixerModule(voiceHandler, SCE_NGS_VOICE_AT9_MIXER);
}

int ssai::VoiceModule::initializeAsPcm(SceNgsHVoice voiceHandler, int codecType, int sampleRate, int channelNum)
{
	m_onSwappedBufferCallback = NULL;
	m_onLoopedBufferCallback  = NULL;
	m_userArg                 = NULL;

	m_nextBufferIndex = 0;
	SCE_SAMPLE_UTIL_ASSERT(NUM_BUFFERS <= SCE_NGS_PLAYER_MAX_BUFFERS);
	m_type = MODULE_PCM;
	int ret = m_pcm.initialize(voiceHandler, codecType, sampleRate, channelNum);
	if(ret != SCE_OK){
		return ret;
	}

	m_pcm.setCallback(pcmModuleCallback, this);

	return initializeMixerModule(voiceHandler, SCE_NGS_VOICE_T1_MIXER);
}


int ssai::VoiceModule::initializeAsGenerator(SceNgsHVoice voiceHandler, const sce::SampleUtil::Audio::GeneratorSettings  *pNoise)
{
	m_onSwappedBufferCallback = NULL;
	m_onLoopedBufferCallback  = NULL;
	m_userArg                 = NULL;

	m_type = MODULE_GENERATOR;
	//set SIGNAL_GENERATOR parameter 

	SceNgsBufferInfo	bufferInfo;	
	int ret = sceNgsVoiceLockParams( voiceHandler, 
		SCE_NGS_VOICE_T1_SIGNAL_GENERATOR, 
		SCE_NGS_GENERATOR_PARAMS_STRUCT_ID, 
		&bufferInfo );
	if( ret != SCE_NGS_OK )	{
		SCE_DBG_LOG_ERROR( "setParamsToNoiseModule: sceNgsVoiceLockParams() failed: 0x%08X\n", ret );
		return ret;
	}

	memset( bufferInfo.data, 0, bufferInfo.size);
	SceNgsGeneratorParams* pGeneratorParams    = (SceNgsGeneratorParams*)bufferInfo.data;
	pGeneratorParams->desc.id                  = SCE_NGS_GENERATOR_PARAMS_STRUCT_ID;
	pGeneratorParams->desc.size                = sizeof(SceNgsGeneratorParams);
	pGeneratorParams->generator.eGeneratorMode = pNoise->generatorMode;
	pGeneratorParams->generator.nFrequency     = pNoise->frequency;
	pGeneratorParams->generator.fAmplitude     = pNoise->amplitude;
	pGeneratorParams->generator.fPulseWidth    = pNoise->pulseWidth;
	pGeneratorParams->generator.uSampleOffset  = pNoise->sampleOffset;
	pGeneratorParams->generator.uPhaseAngle    = pNoise->phaseAngle;

	ret = sceNgsVoiceUnlockParams( voiceHandler, SCE_NGS_VOICE_T1_SIGNAL_GENERATOR );
	if( ret != SCE_NGS_OK ){
		SCE_DBG_LOG_ERROR( "setParamsToNoiseModule: sceNgsVoiceUnlockParams() failed: 0x%08X\n", ret );
		return ret;
	}

	//E Cancel the bypass of the signal generator
	//J signal generator のバイパスを解除
	ret = sceNgsVoiceBypassModule( voiceHandler, SCE_NGS_VOICE_T1_SIGNAL_GENERATOR, SCE_NGS_MODULE_FLAG_NOT_BYPASSED );
	if( ret != SCE_NGS_OK )	{
		SCE_DBG_LOG_ERROR( "setParamsToNoiseModule: sceNgsVoiceBypassModule() failed: 0x%08X\n", ret );
		return ret;
	}

	return initializeMixerModule(voiceHandler, SCE_NGS_VOICE_T1_MIXER);

}


int ssai::VoiceModule::resetBuffer(void)
{
	switch(m_type)
	{
	case MODULE_AT9:
		{
			m_nextBufferIndex = 0;
			m_at9.resetBuffer();
			return SCE_OK;
		}
	case MODULE_PCM:
		{
			m_nextBufferIndex = 0;
			SceNgsPlayerParams *param = m_pcm.lock();
			memset(param->buffs, 0, sizeof(param->buffs));
			param->nStartBuffer = 0;
			param->nStartByte = 0;
			m_pcm.unlock();
			return SCE_OK;
		}
	case MODULE_GENERATOR:
		return -1;
	default:
		return -1;
	}
}


int ssai::VoiceModule::pushBuffer(const void *buffer, uint32_t numBytes, int loopCount,
								  bool isStart,
								  bool isEnd
								  )
{
	switch(m_type)
	{
	case MODULE_AT9:
		{
			uint32_t bufIndex = m_nextBufferIndex;
			m_nextBufferIndex = (m_nextBufferIndex + 1) % NUM_BUFFERS;
			m_at9.setBuffer(bufIndex, buffer, numBytes, loopCount, m_nextBufferIndex, isStart, isEnd);
			return SCE_OK;
		}
	case MODULE_PCM:
		{
			uint32_t bufIndex = m_nextBufferIndex;
			m_nextBufferIndex = (m_nextBufferIndex + 1) % NUM_BUFFERS;
			m_pcm.lock();
			m_pcm.setBuffer(bufIndex, buffer, numBytes, loopCount, m_nextBufferIndex);
			m_pcm.unlock();
			return SCE_OK;
		}
	case MODULE_GENERATOR:
		return -1;
	default:
		return -1;
	}
}

int ssai::VoiceModule::pushEndBuffer(void)
{
	switch(m_type)
	{
	case MODULE_AT9:
		{
			uint32_t bufIndex = m_nextBufferIndex;
			m_nextBufferIndex = (m_nextBufferIndex + 1) % NUM_BUFFERS;
			m_at9.setBuffer(bufIndex, NULL, 0, 0, SCE_NGS_PLAYER_NO_NEXT_BUFFER);
			return SCE_OK;
		}
	case MODULE_PCM:
		{
			uint32_t bufIndex = m_nextBufferIndex;
			m_nextBufferIndex = (m_nextBufferIndex + 1) % NUM_BUFFERS;
			m_pcm.lock();
			m_pcm.setBuffer(bufIndex, NULL, 0, 0, SCE_NGS_PLAYER_NO_NEXT_BUFFER);
			m_pcm.unlock();
			return SCE_OK;
		}
	case MODULE_GENERATOR:
		return -1;
	default:
		return -1;
	}
}

int ssai::VoiceModule::initializeMixerModule(SceNgsHVoice voiceHandle, SceUInt32 mixerModuleId)
{
	SceNgsMixerParams*		pMixerParams;
	SceNgsBufferInfo	bufferInfo;

	// Get mixer parameters
	int returnCode = sceNgsVoiceLockParams( voiceHandle, mixerModuleId, SCE_NGS_MIXER_PARAMS_STRUCT_ID, &bufferInfo );
	if( returnCode != SCE_NGS_OK ){
		SCE_DBG_LOG_ERROR( "soundUtilVoiceSetDataInfo: mixer sceNgsVoiceLockParams() failed: 0x%08X\n", returnCode );
		return returnCode;
	}
	// Set mixer parameters
	memset( bufferInfo.data, 0, bufferInfo.size);
	pMixerParams = (SceNgsMixerParams *)bufferInfo.data;
	pMixerParams->desc.id     = SCE_NGS_MIXER_PARAMS_STRUCT_ID;
	pMixerParams->desc.size   = sizeof(SceNgsMixerParams);

	pMixerParams->fGainIn[0]    = 1.0f;
	pMixerParams->fGainIn[1]    = 1.0f;
	pMixerParams->uPauseLeadIn  = 0;
	pMixerParams->uPauseLeadOut = 0;

	// Update mixer parameters
	returnCode = sceNgsVoiceUnlockParams( voiceHandle, mixerModuleId );
	if( returnCode != SCE_NGS_OK ){
		SCE_DBG_LOG_ERROR( "soundUtilVoiceSetDataInfo: mixer sceNgsVoiceUnlockParams() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	//E Cancel the bypass of the input mixer
	//J input mixer のバイパスを解除
	returnCode = sceNgsVoiceBypassModule( voiceHandle, mixerModuleId, SCE_NGS_MODULE_FLAG_NOT_BYPASSED );
	if( returnCode != SCE_NGS_OK )
	{
		SCE_DBG_LOG_ERROR( "soundUtilVoiceSetDataInfo: mixer sceNgsVoiceBypassModule() failed: 0x%08X\n", returnCode );
		return returnCode;
	}
	return SCE_OK;


}


void ssai::VoiceModule::notifyLoopedBuffer(int finishedLoopNum, const void *buffer)
{
	if(m_type != MODULE_AT9){
		return;
	}

	m_at9.notifyLoopedBuffer(finishedLoopNum, buffer);
}



void ssai::VoiceModule::at9ModuleCallback( const SceNgsCallbackInfo *pCallbackInfo )
{
	VoiceModule *module = (VoiceModule *)pCallbackInfo->pUserData;

	switch(pCallbackInfo->nCallbackData)
	{
	case SCE_NGS_AT9_SWAPPED_BUFFER:
		{
			if(module->m_onSwappedBufferCallback != NULL){
				module->m_onSwappedBufferCallback(module->m_userArg);
			}
			break;
		}
	case SCE_NGS_AT9_LOOPED_BUFFER:
		{
			int finishedLoopNum = pCallbackInfo->nCallbackData2;
			const void* buffer =  (const void*)pCallbackInfo->nCallbackData;
			if(module->m_onLoopedBufferCallback != NULL){
				module->m_onLoopedBufferCallback(finishedLoopNum, buffer,  module->m_userArg);
			}
			break;
		}
	}
}

void ssai::VoiceModule::pcmModuleCallback( const SceNgsCallbackInfo *pCallbackInfo )
{
	VoiceModule *module = (VoiceModule *)pCallbackInfo->pUserData;

	switch(pCallbackInfo->nCallbackData){
	case SCE_NGS_PLAYER_END_OF_DATA:
		// Do Nothing
		break;

	case SCE_NGS_PLAYER_SWAPPED_BUFFER:
		{
			if(module->m_onSwappedBufferCallback != NULL){
				module->m_onSwappedBufferCallback(module->m_userArg);
			}
			break;
		}
	default:

		break;
	}

}


void ssai::VoiceModule::setCallback(OnSwappedBufferCallback onSwappedBufferCallback,
									OnLoopedBufferCallback  onLoopedBufferCallback,
									void *userArg)
{
	m_onSwappedBufferCallback = onSwappedBufferCallback;
	m_onLoopedBufferCallback  = onLoopedBufferCallback;
	m_userArg                 = userArg;
}


int ssai::VoiceModule::setPitch(float pitch)
{
	switch(m_type)
	{
	case MODULE_AT9:
		{
			m_at9.setPitch(pitch);
			return SCE_OK;
		}
	case MODULE_PCM:
		{
			SceNgsPlayerParams *param = m_pcm.lock();
			param->fPlaybackScalar = pitch;
			m_pcm.unlock();
			return SCE_OK;
		}
	case MODULE_GENERATOR:
		return -1;
	default:
		return -1;
	}

}

#endif /* _SCE_TARGET_OS_PSP2 */