/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include "ngs_internal.h"
#include <libdbg.h>

#define SOUND_UTIL_LOG_TRACE(a) 

namespace ssai = sce::SampleUtil::Audio::Internal;

SceNgsHVoice  ssai::VoiceInternal::getNgsVoice(void) const
{
	return m_voiceHandler;
}


int ssai::VoiceInternal::initialize(VoiceRack *rack, SceNgsHVoice voiceHandler, const VoiceData *_voiceData)
{
	int returnCode;
	m_voiceHandler = voiceHandler;
	m_rack         = rack;

	const VoiceDataImpl *voiceData = dynamic_cast<const VoiceDataImpl *>(_voiceData);

	if(voiceData == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if((voiceData->getNumChannels() < 1) ||  (voiceData->getNumChannels() > 2)){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	this->m_state = kVoiceStateAvailable;

	returnCode = sceNgsVoiceInit(this->m_voiceHandler, NULL, SCE_NGS_VOICE_INIT_ALL);
	if(returnCode != SCE_OK){
		return returnCode;
	}

	m_voiceData = voiceData;

	// Initialize Module
	switch(m_voiceData->getCodecType())
	{
	case Data::CODEC_TYPE_PCM:
		{
			returnCode = m_voiceModule.initializeAsPcm(m_voiceHandler, SCE_NGS_PLAYER_TYPE_PCM, m_voiceData->getSampleRate(), m_voiceData->getNumChannels());
			break;
		}
	case Data::CODEC_TYPE_ADPCM:
		{
			returnCode = m_voiceModule.initializeAsPcm(m_voiceHandler, SCE_NGS_PLAYER_TYPE_ADPCM, m_voiceData->getSampleRate(), m_voiceData->getNumChannels());
			break;
		}
	case Data::CODEC_TYPE_AT9:
		{			
			returnCode = m_voiceModule.initializeAsAt9(m_voiceHandler, m_voiceData->getSampleRate(), 
				m_voiceData->getNumChannels(), 
				m_voiceData->getAudioFileInfo()->m_loopStartOffset,
				m_voiceData->getAudioFileInfo()->m_loopEndOffset,
				m_voiceData->getAudioFileInfo()->m_at9configData);
			break;
		}
	case Data::CODEC_TYPE_GENERATOR:
		{
			returnCode = m_voiceModule.initializeAsGenerator(m_voiceHandler, &m_voiceData->m_generatorInfo);
			break;
		}
	default:
		{
			SCE_SAMPLE_UTIL_ASSERT(0);
			break;
		}
	}

	if(returnCode != SCE_OK){
		return returnCode;
	}

	m_voiceModule.setCallback(VoiceInternal::onSwappedBuffer, VoiceInternal::onLoopedBuffer, this);

	m_stream = NULL;
	if(voiceData->getDataType() == VoiceDataImpl::TYPE_STREAMING){
		m_stream = new sce::SampleUtil::Internal::Util::Stream();

		int align = m_voiceData->getBlockAlign();

		VoiceDataImpl::Offset offset;
		m_voiceData->getOffset(&offset);

		returnCode = m_stream->open(m_voiceData->getFilePath(), 0, align,
			offset.dataOffset,
			offset.dataSize,
			offset.alignedLoopStartOffset,
			offset.alignedLoopEndOffset);
	}

	return returnCode;
}



int ssai::VoiceInternal::finalize(void)
{
	if(m_voiceHandler == SCE_NGS_INVALID_HANDLE){
		return SCE_OK;
	}

	if(this->m_state == kVoiceStateInvalid){
		//SCE_DBG_LOG_ERROR("closeVoiceCommon: this voice has not been used\n");
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	sceNgsVoiceKill(this->m_voiceHandler);

	if(m_stream != NULL){
		delete m_stream;
		m_stream = NULL;
	}

	this->m_state = kVoiceStateInvalid;

	m_rack->onClose(this);

	m_voiceHandler = SCE_NGS_INVALID_HANDLE;
	return SCE_OK;

}






void ssai::VoiceInternal::onSwappedBuffer(void *userArg)
{
	VoiceInternal *voice = (VoiceInternal *)userArg;
	if(voice->m_voiceData->getDataType() == VoiceDataImpl::TYPE_STREAMING)
	{
		int ret = loadNextStreamingData(&voice->m_voiceModule, voice->m_stream, voice->m_voiceData);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	}
}

void ssai::VoiceInternal::onLoopedBuffer(int finishedLoopNum, const  void *buffer, void *userArg)
{
	VoiceInternal *voice = (VoiceInternal *)userArg;

	voice->m_voiceModule.notifyLoopedBuffer(finishedLoopNum, buffer);
}


int ssai::VoiceInternal::loadNextStreamingData(ssai::VoiceModule *voiceModule, sce::SampleUtil::Internal::Util::Stream	*stream, const ssai::VoiceDataImpl *data)
{
	void *streamBuffer = NULL;
	int dataFileOffset;
	char bufferState;
	int gotDataSize = stream->getStreamingData(&streamBuffer, &dataFileOffset, &bufferState);

	const ssai::Data::AudioFile *fileInfo = data->getAudioFileInfo();
	if(fileInfo == NULL){
		return -1;
	}
	VoiceDataImpl::Offset offset;
	data->getOffset(&offset);

	bool isFileStart = dataFileOffset == offset.dataOffset;
	bool isFileEnd   = (dataFileOffset + gotDataSize) == (offset.dataOffset + offset.dataSize);
	bool isLoopStart = dataFileOffset == offset.alignedLoopStartOffset;
	bool isLoopEnd   = (dataFileOffset + gotDataSize) == offset.alignedLoopEndOffset;

	if(gotDataSize == 0){
		return voiceModule->pushEndBuffer();
	}else{
		return voiceModule->pushBuffer(streamBuffer,
			gotDataSize, 
			0, // loopCount
			isFileStart || isLoopStart,
			isFileEnd   || isLoopEnd);
	}
}



int ssai::VoiceInternal::play(int loopNum)
{
	int returnCode = SCE_OK;

	if(this->m_state == kVoiceStatePlaying){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	switch(m_voiceData->getDataType())
	{
	case VoiceDataImpl::TYPE_ON_MEMORY:
		{
			m_voiceModule.resetBuffer();

			VoiceDataImpl::Offset offset;
			memset(&offset, 0, sizeof(offset));
			m_voiceData->getOffset(&offset);

			uint32_t dataEnd = offset.dataOffset + offset.dataSize;
			bool hasPrologue = offset.dataOffset != offset.alignedLoopStartOffset;
			bool hasEpilogue = dataEnd != offset.alignedLoopEndOffset;


			uint32_t prologueStart = hasPrologue ? offset.dataOffset : 0;
			uint32_t prologueSize  = hasPrologue ? (offset.alignedLoopStartOffset - offset.dataOffset) : 0;
			uint32_t epilogueStart = hasEpilogue ? offset.alignedLoopEndOffset : 0;
			uint32_t epilogueSize  = hasEpilogue ? (dataEnd - offset.alignedLoopEndOffset) : 0;
			uint32_t mainStart     = hasPrologue ? offset.alignedLoopStartOffset : offset.dataOffset;
			uint32_t mainEnd       = hasEpilogue ? offset.alignedLoopEndOffset : dataEnd;
			uint32_t mainSize      = mainEnd - mainStart;

			uintptr_t fileImage = (uintptr_t)m_voiceData->getFileImage();

			if(hasPrologue){
				m_voiceModule.pushBuffer((void*)(fileImage + prologueStart), prologueSize, 0);
			}
			m_voiceModule.pushBuffer( (void*)(fileImage + mainStart), mainSize, loopNum);
			
			if(hasEpilogue){
				m_voiceModule.pushBuffer( (void*)(fileImage + epilogueStart), epilogueSize,  0);
			}

			m_voiceModule.pushEndBuffer();
			break;
		}
	case VoiceDataImpl::TYPE_STREAMING:
		{
			m_stream->reset();
			m_stream->setLoop(loopNum);

			m_voiceModule.resetBuffer();
			//E Copy the stream data
			//J ストリームデータのコピー
			for(int i = 0; i < 2; i++) {
				returnCode = loadNextStreamingData(&m_voiceModule, m_stream, m_voiceData);
				if(returnCode != SCE_OK){
					return returnCode;
				}
			}
			break;
		}
	default:
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	returnCode = sceNgsVoicePlay(this->m_voiceHandler);
	if(returnCode != SCE_NGS_OK){
		return returnCode;
	}

	this->m_state = kVoiceStatePlaying;

	return SCE_OK;
}


int ssai::VoiceInternal::connectOutput(
								 unsigned int nSourceOutputIndex, 
								 SceNgsHVoice hVoiceDestination, 
								 unsigned int nTargetInputIndex)
{
	if(nSourceOutputIndex >= NUM_OUTPUT_INDEX){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	return connectRacks(this->m_voiceHandler, 
		nSourceOutputIndex, 
		hVoiceDestination, nTargetInputIndex, 
		&this->m_patchHandler[nSourceOutputIndex]);
}

int ssai::VoiceInternal::setOutput(uint32_t nSourceOutputIndex, Buss *_buss)
{
	BussImpl *buss = dynamic_cast<BussImpl *>(_buss);
	if(buss != NULL){
		if(isConnectedOutput(nSourceOutputIndex)){
			disConnectOutput(nSourceOutputIndex);
		}
		return connectOutput(nSourceOutputIndex, buss->m_voiceHandler, 0);
	}else{
		return disConnectOutput(nSourceOutputIndex);
	}
}


int ssai::VoiceInternal::disConnectOutput(uint32_t nSourceOutputIndex)
{
	if(nSourceOutputIndex >= NUM_OUTPUT_INDEX){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	if(m_patchHandler[nSourceOutputIndex] == NULL){
		return SCE_OK;
	}
	int returnCode = sceNgsPatchRemoveRouting(this->m_patchHandler[nSourceOutputIndex]);
	if(returnCode < 0){
		return returnCode;
	}
	this->m_patchHandler[nSourceOutputIndex] = NULL;
	return SCE_OK;

}

int ssai::VoiceInternal::checkFinishedVoice(void)
{
	int returnCode;
	SceNgsVoiceInfo	voiceInfo;

	returnCode = sceNgsVoiceGetInfo(this->getNgsVoice(), &voiceInfo);
	if(returnCode < SCE_OK)	{
		//SCE_DBG_LOG_ERROR("soundUtilProcess : sceNgsVoiceGetInfo failed 0x%08x\n", returnCode);
		return returnCode;
	}

	if(voiceInfo.uVoiceState == SCE_NGS_VOICE_STATE_AVAILABLE){
		if(m_state == kVoiceStateKeyoff){
			m_state = kVoiceStateAvailable;
		}
		if(m_state == kVoiceStatePlaying){
			m_state = kVoiceStateAvailable;
		}
	}


	if(voiceInfo.uVoiceState == SCE_NGS_VOICE_STATE_KEY_OFF)	{
		//SOUND_UTIL_LOG_TRACE("voice is keyoff\n");
	}else if(voiceInfo.uVoiceState == SCE_NGS_VOICE_STATE_FINALIZING){
		//SOUND_UTIL_LOG_TRACE("voice is finalizing\n");
	}else if(voiceInfo.uVoiceState == SCE_NGS_VOICE_STATE_AVAILABLE){	
		//SOUND_UTIL_LOG_TRACE("voice is available\n");
	}else if(voiceInfo.uVoiceState == SCE_NGS_VOICE_STATE_ACTIVE){
		//SOUND_UTIL_LOG_TRACE("voice is active\n");
	}else if(voiceInfo.uVoiceState == SCE_NGS_VOICE_STATE_UNLOADING){
		//SOUND_UTIL_LOG_TRACE("voice %d is unloading\n");
	}else if(voiceInfo.uVoiceState == SCE_NGS_VOICE_STATE_PENDING){
		//SOUND_UTIL_LOG_TRACE("voice %d is pending\n");
	}

	return SCE_OK;
}


int  ssai::VoiceInternal::keyOff(void)
{
	int returnCode;
	//J 状態のチェック

	if((m_state != kVoiceStatePlaying) && (m_state != kVoiceStatePause)){
		//SCE_DBG_LOG_ERROR("soundUtilVoiceKeyOff: This voice is not active!\n");
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	returnCode = sceNgsVoiceKeyOff(this->m_voiceHandler);
	if(returnCode < SCE_OK)	{
		//SCE_DBG_LOG_ERROR("soundUtilVoiceKeyOff: sceNgsVoiceKeyOff() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	this->m_state = kVoiceStateKeyoff;
	return SCE_OK;
}

int  ssai::VoiceInternal::kill(void)
{
	int returnCode;
	//J 状態のチェック

	if((m_state != kVoiceStatePlaying) && (m_state != kVoiceStatePause)){
		//SCE_DBG_LOG_ERROR("soundUtilVoiceKeyOff: This voice is not active!\n");
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	returnCode = sceNgsVoiceKill(this->m_voiceHandler);
	if(returnCode < SCE_OK)	{
		//SCE_DBG_LOG_ERROR("soundUtilVoiceKeyOff: sceNgsVoiceKeyOff() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	this->m_state = kVoiceStateAvailable;
	return SCE_OK;
}


int ssai::VoiceInternal::pause(void)
{
	int returnCode;
	//J 状態のチェック
	if(m_state != kVoiceStatePlaying){
		//SCE_DBG_LOG_ERROR("soundUtilVoicePause: This voice is not active!\n");
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	returnCode = sceNgsVoicePause(this->m_voiceHandler);
	if(returnCode < SCE_OK)	{
		//SCE_DBG_LOG_ERROR("soundUtilVoicePause: sceNgsVoicePause() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	this->m_state = kVoiceStatePause;

	return SCE_OK;
}

int ssai::VoiceInternal::resume(void)
{
	int returnCode;
	//J 状態のチェック
	if(m_state != kVoiceStatePause){
		//SCE_DBG_LOG_ERROR("soundUtilVoiceResume: This voice is not active!\n");
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	returnCode = sceNgsVoiceResume(this->m_voiceHandler);
	if(returnCode < SCE_OK)	{
		//SCE_DBG_LOG_ERROR("soundUtilVoiceResume: sceNgsVoiceResume() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	this->m_state = kVoiceStatePlaying;

	return SCE_OK;
}


int ssai::VoiceInternal::setVolume(uint32_t outIndex, const Volume *volInfo)
{
	Volume vol = { 1.0f, 1.0f };
	if(volInfo == NULL){
		volInfo = &vol;
	}
	if(outIndex >= NUM_OUTPUT_INDEX){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	SceNgsHPatch patch = m_patchHandler[outIndex];

	return setPatchVolume(patch, volInfo);

}

int ssai::VoiceInternal::setPitch(float pitch)
{
	return m_voiceModule.setPitch(pitch);
}

int ssai::VoiceInternal::setEnvelope(const Envelope* envelopeInfo)
{
	if(envelopeInfo == NULL) {
		//E Cancel the bypass of the envelope module
		//J Envelopeモジュールのバイパスを解除
		return sceNgsVoiceBypassModule( this->getNgsVoice(), SCE_NGS_VOICE_T1_ENVELOPE, SCE_NGS_MODULE_FLAG_BYPASSED );
	}


	int returnCode;
	SceNgsBufferInfo		bufferInfo;
	SceNgsEnvelopeParams	*pEnvelopeParams;

	//Get Envelope parameters
	returnCode = sceNgsVoiceLockParams( this->getNgsVoice(), SCE_NGS_VOICE_T1_ENVELOPE, SCE_NGS_ENVELOPE_PARAMS_STRUCT_ID, &bufferInfo );
	if(returnCode != SCE_NGS_OK){
		//SCE_DBG_LOG_ERROR("setEnvelopefromVoiceStruct: sceNgsVoiceLockParams() failed : 0x%08X\n", returnCode);
		goto End;
	}

	// Set Envelope parameters
	memset( bufferInfo.data, 0, bufferInfo.size);
	pEnvelopeParams              = (SceNgsEnvelopeParams*)bufferInfo.data;
	pEnvelopeParams->desc.id     = SCE_NGS_ENVELOPE_PARAMS_STRUCT_ID;
	pEnvelopeParams->desc.size   = sizeof(SceNgsEnvelopeParams);
	pEnvelopeParams->uNumPoints  = envelopeInfo->numPoints;
	memcpy(pEnvelopeParams->envelopePoints, envelopeInfo->envelopePoints, sizeof(SceNgsEnvelopePoint) * envelopeInfo->numPoints);
	pEnvelopeParams->nLoopEnd    = envelopeInfo->loopEnd;
	pEnvelopeParams->uLoopStart  = envelopeInfo->loopStart;
	pEnvelopeParams->uReleaseMsecs = envelopeInfo->releaseMsecs;

	// update Envelope parameters
	returnCode = sceNgsVoiceUnlockParams( this->getNgsVoice(), SCE_NGS_VOICE_T1_ENVELOPE);
	if(returnCode != SCE_NGS_OK){
		SCE_DBG_LOG_ERROR("setEnvelopefromVoiceStruct: sceNgsVoiceUnlockParams() failed : 0x%08X\n", returnCode);
		goto End;
	}

	//E Cancel the bypass of the envelope module
	//J Envelopeモジュールのバイパスを解除
	returnCode = sceNgsVoiceBypassModule( this->getNgsVoice(), SCE_NGS_VOICE_T1_ENVELOPE, SCE_NGS_MODULE_FLAG_NOT_BYPASSED );
	if(returnCode != SCE_NGS_OK){
		//SCE_DBG_LOG_ERROR("setEnvelopefromVoiceStruct: sceNgsVoiceBypassModule() failed : 0x%08X\n", returnCode);
		goto End;
	}

	returnCode = SCE_OK;
End:
	return returnCode;
}



/* ----------------------------------------------------------------------------------------- */
//  Voice Rack
/* ----------------------------------------------------------------------------------------- */

int ssai::VoiceRack::initialize(
	SceNgsHSynSystem			 sysHandle,
	const SceNgsRackDescription &rackDesc, 
	int32_t						 idStart)
{
	SceNgsBufferInfo      bufferInfo;
	int ret = sceNgsRackGetRequiredMemorySize( sysHandle, &rackDesc, &bufferInfo.size);
	if( ret != SCE_NGS_OK ){
		//SCE_DBG_LOG_ERROR( "createRacks: sceNgsRackGetRequiredMemorySize(mono-player) failed: 0x%08X\n", ret );
		return ret;
	} 

	//E Allocate the memory for the monaural player rack
	//J モノラルプレイヤーラックのメモリ確保
	m_rackMem = ::memalign(16, bufferInfo.size);
	if( m_rackMem == NULL){
		//SCE_DBG_LOG_ERROR( "createRacks: malloc(mono-player) failed for %d bytes\n", bufferInfo.size );
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}

	//E Initialize the monaural player rack
	//J モノラルプレイヤーラックの初期化
	bufferInfo.data = m_rackMem;
	ret = sceNgsRackInit( sysHandle, &bufferInfo, &rackDesc, &m_rack );
	if( ret != SCE_NGS_OK ){
		//SCE_DBG_LOG_ERROR( "createRacks: sceNgsRackInit(mono-player) failed: 0x%08X\n", ret );
		return ret;
	}

	for(uint32_t i=0; i<rackDesc.nVoices; i++){
		SceNgsHVoice voice;
		ret = sceNgsRackGetVoiceHandle( m_rack, i, &voice );
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

		m_availableVoiceHandle.push_back(voice);
	}
	return SCE_OK;
}

int ssai::VoiceRack::finalize(void)
{
	for(uint32_t i=0; i<m_voices.size(); i++){
		VoiceInternal *v = m_voices.at(i);
		delete v;
	}
	m_voices.clear();

	if(m_rackMem != NULL){
		int ret = sceNgsRackRelease(m_rack, NULL);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		::free(m_rackMem);
		m_rackMem = NULL;
	}
	return SCE_OK;
}

ssai::VoiceInternal *ssai::VoiceRack::createVoice(const VoiceData *voiceData)
{
	if(m_availableVoiceHandle.size() == 0){
		return NULL;
	}
	std::vector<SceNgsHVoice>::iterator it = m_availableVoiceHandle.begin();
	SceNgsHVoice voiceHandler = *it;
	m_availableVoiceHandle.erase(it);

	VoiceInternal *v = new VoiceInternal();
	int ret = v->initialize(this, voiceHandler, voiceData);
	if(ret != SCE_OK){
		delete v;
		return NULL;
	}
	m_voices.push_back(v);
	return v;
}

void ssai::VoiceRack::onClose(VoiceInternal *voice)
{
	std::vector<VoiceInternal*>::iterator it;
	for(it = m_voices.begin(); it != m_voices.end(); it++){
		VoiceInternal *v = *it;
		if(v == voice){
			m_voices.erase(it);
			break;
		}
	}

	m_availableVoiceHandle.push_back(voice->getNgsVoice());

}


#endif /* _SCE_TARGET_OS_PSP2 */