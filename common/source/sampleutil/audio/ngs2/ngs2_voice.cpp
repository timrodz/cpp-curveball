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
#include <algorithm>
#include "ngs2_internal.h"

namespace ssa =sce::SampleUtil::Audio;
namespace ssai =sce::SampleUtil::Audio::Internal;


/* ------------------------------------------------------------------ */


/* ------------------------------------------------------------------ */

int ssai::VoiceInternal::initialize(SceNgs2Handle voiceHandle, VoiceRack *rack, const VoiceData *voiceData)
{
	int ret;
	SCE_SAMPLE_UTIL_ASSERT(voiceData != NULL);

	m_voiceHandle = voiceHandle;
	m_rack = rack;

	m_stream = NULL;
	m_rawVoiceInfo = NULL;
	m_voiceData  = NULL;
	m_state = kVoiceStateAvailable;
	m_addedDataSize = 0;
			
	m_voiceData  = dynamic_cast<const ssai::VoiceDataImpl*>(voiceData);


	switch(m_voiceData->m_type)
	{
	case ssai::VoiceDataImpl::TYPE_ON_MEMORY:
		{
			ret = sceNgs2SamplerVoiceSetup( voiceHandle, &m_voiceData->m_fileInfo.waveformInfo.format , 0);
			if( ret != SCE_OK ){
				return ret;
			}

			ret = sceNgs2SamplerVoiceAddWaveformBlocks( voiceHandle,
				m_voiceData->m_onMemorySoundInfo.m_data,
				m_voiceData->m_fileInfo.waveformInfo.aBlock,
				m_voiceData->m_fileInfo.waveformInfo.numBlocks,
				0);


			if( ret != SCE_OK ){
				return ret;
			}
			return SCE_OK;
		}
	case ssai::VoiceDataImpl::TYPE_STREAMING:
		{
			ret = sceNgs2SamplerVoiceSetup( voiceHandle, &m_voiceData->m_fileInfo.waveformInfo.format, 0);
			if( ret != SCE_OK ){
				return ret;
			}

			m_stream = new Stream;
			ret = m_stream->initialize(m_voiceData->m_streamingSoundInfo.m_filePath.c_str(),
								 m_voiceData->m_fileInfo.waveformInfo.aBlock,
								 m_voiceData->m_fileInfo.waveformInfo.numBlocks);
			if(ret != SCE_OK){
				delete m_stream;
				m_stream = NULL;
				return ret;
			}
			return SCE_OK;
		}
	case ssai::VoiceDataImpl::TYPE_GENERATOR:
	default:
		{
			abort();
		}

	}

	return SCE_OK;
}

int ssai::VoiceInternal::initializeAsRawVoice(SceNgs2Handle voiceHandle, VoiceRack *rack, uint32_t streamBufferSize, 
	uint32_t waveformType, uint32_t numChannels, uint32_t sampleRate)
{
	int ret;

	m_voiceHandle = voiceHandle;
	m_rack = rack;
	m_stream = NULL;
	m_rawVoiceInfo = NULL;
	m_voiceData  = NULL;
	m_state = kVoiceStateAvailable;
	m_addedDataSize = 0;

	SceNgs2WaveformFormat format;
	memset(&format, 0, sizeof(format));
	format.waveformType = waveformType; //SCE_NGS2_WAVEFORM_TYPE_PCM_I16L;
	format.numChannels  = numChannels; //2;
	format.sampleRate  = sampleRate; //48000;
	format.configData  = 0;
	format.frameOffset = 0;
	format.frameMargin = 0;
	
	ret = sceNgs2SamplerVoiceSetup( voiceHandle, &format , 0);
	if( ret != SCE_OK ){
		return ret;
	}
	m_rawVoiceInfo = new RawVoiceInfo;
	m_rawVoiceInfo->m_streamBufferSize  = streamBufferSize;
	m_rawVoiceInfo->m_streamBuffer = new uint8_t[streamBufferSize];
	m_rawVoiceInfo->m_streamBufferOffset = 0;
	m_rawVoiceInfo->m_waveformType = waveformType;
	m_rawVoiceInfo->m_numChannels  = numChannels;
	m_rawVoiceInfo->m_sampleRate   = sampleRate;
	
	return SCE_OK;
}

int ssai::VoiceInternal::finalize(void)
{
	if(m_rawVoiceInfo != NULL){
		if(m_rawVoiceInfo->m_streamBuffer != NULL){
			delete [] m_rawVoiceInfo->m_streamBuffer;
			m_rawVoiceInfo->m_streamBuffer = NULL;
		}
		delete m_rawVoiceInfo;
		m_rawVoiceInfo = NULL;
	}
	if(m_voiceHandle != SCE_NGS2_HANDLE_INVALID){
		kill();
		m_rack->notifyVoiceDestroyed(dynamic_cast<VoiceInternal*>(this));
		m_voiceHandle = SCE_NGS2_HANDLE_INVALID;
		m_rack = NULL;
		if(m_stream){
			m_stream->finalize();
			delete m_stream;
		}
	}
	return SCE_OK;
}

void ssai::VoiceInternal::loadNextStreamData(bool clear)
{
	if(m_voiceData->m_type != ssai::VoiceDataImpl::TYPE_STREAMING){
		return;
	}
	SCE_SAMPLE_UTIL_ASSERT(m_stream != NULL);
	if(m_stream->isFinished()){
		return;
	}
	SceNgs2SamplerVoiceState voiceState;
	int ret = sceNgs2SamplerVoiceGetState(m_voiceHandle, &voiceState);
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT(ret >= SCE_OK);
	uint64_t remainDataSize = m_addedDataSize - voiceState.decodedDataSize;
	if(remainDataSize < Stream::BUFFER_SIZE){
		Block block = m_stream->readNext();
		int ret = sceNgs2SamplerVoiceAddWaveformBlocks( m_voiceHandle,
			block.m_data,
			&block.waveformBlock, 1, 
			block.waveformBlockFlag | (clear? SCE_NGS2_WAVEFORM_BLOCK_FLAG_CLEAR:0));
		(void)ret;
		SCE_SAMPLE_UTIL_ASSERT(ret >= SCE_OK);
		m_addedDataSize += block.waveformBlock.dataSize;
	}


}

void ssai::VoiceInternal::update(void)
{
	SceNgs2SamplerVoiceState samplerVoiceState;
	int ret = sceNgs2SamplerVoiceGetState(m_voiceHandle, &samplerVoiceState);
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT(ret >= SCE_OK);
	//printf("(%#lx) voiceState.stateFlags=%#x\n", (uintptr_t)this, voiceState.stateFlags);
	
	//SCE_SAMPLE_UTIL_ASSERT((voiceState.stateFlags & SCE_NGS2_VOICE_STATE_FLAG_EMPTY) == 0);
	if((m_state == kVoiceStateKeyoff) || (m_state == kVoiceStatePlaying)){
		if((samplerVoiceState.voiceState.stateFlags & SCE_NGS2_VOICE_STATE_FLAG_PLAYING) == 0){
			//printf("voice seems to be stopped\n");
			m_state = kVoiceStateAvailable;
		}
	}
	if(m_voiceData == NULL){
		return;
	}
	switch(m_voiceData->m_type)
	{
	case ssai::VoiceDataImpl::TYPE_STREAMING:
		{
			if(m_state == kVoiceStatePlaying){
				loadNextStreamData(false);
			}			
			break;
		}
	default:
		break;

	}


}

int ssai::VoiceInternal::play( int loopCount )
{
	if(m_state != kVoiceStateAvailable){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	if(m_voiceData != NULL){
		switch(m_voiceData->m_type)
		{
		case ssai::VoiceDataImpl::TYPE_ON_MEMORY:
			{
				break;
			}
		case ssai::VoiceDataImpl::TYPE_STREAMING:
			{
				m_stream->reset();
				m_addedDataSize = 0;
				loadNextStreamData(true);
				break;
			}
		case ssai::VoiceDataImpl::TYPE_GENERATOR:
		default:
			{
				abort();
			}
		}
	}else{
	}


	int ret = sceNgs2VoiceKickEvent( m_voiceHandle, SCE_NGS2_VOICE_EVENT_PLAY );
	if(ret == SCE_OK){
		m_state = kVoiceStatePlaying;
	}
	return ret;
}

int ssai::VoiceInternal::kill(void)
{
	int ret = sceNgs2VoiceKickEvent( m_voiceHandle, SCE_NGS2_VOICE_EVENT_KILL );
	if(ret == SCE_OK){
		m_state = kVoiceStateAvailable;
	}
	return ret;
}

int ssai::VoiceInternal::pause(void)
{
	int ret = sceNgs2VoiceKickEvent( m_voiceHandle, SCE_NGS2_VOICE_EVENT_PAUSE );
	if(ret == SCE_OK){
		m_state = kVoiceStatePause;
	}
	return ret;
}
int ssai::VoiceInternal::resume(void)
{
	int ret = sceNgs2VoiceKickEvent( m_voiceHandle, SCE_NGS2_VOICE_EVENT_PAUSE );
	if(ret == SCE_OK){
		m_state = kVoiceStatePlaying;
	}
	return ret;
}
int ssai::VoiceInternal::keyOff(void)
{
	int ret = sceNgs2VoiceKickEvent( m_voiceHandle, SCE_NGS2_VOICE_EVENT_STOP );
	if(ret == SCE_OK){
		m_state = kVoiceStateKeyoff;
	}
	return ret;
}

int ssai::VoiceInternal::setOutput(uint32_t port, ssa::Buss *buss)
{
	if( buss == NULL ) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssai::BussImpl *bussImpl = dynamic_cast<ssai::BussImpl*>(buss);
	//printf("VOICE PATCH %#x => %#x\n", m_voiceHandle, bussImpl->getHandle());
	return sceNgs2VoicePatch( m_voiceHandle, port, bussImpl->getHandle(), 0 );
}

ssa::VoiceState ssai::VoiceInternal::getState(void) const
{
	return m_state;
}

int ssai::VoiceInternal::setVolume(uint32_t outIndex, const Volume *volume)
{
	if(volume == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	return sceNgs2VoiceSetPortVolume(m_voiceHandle, outIndex, volume->left);
}

uint32_t ssai::VoiceInternal::getBufferEmptySize(void)
{
	
	if(m_voiceData != NULL){
		return -1;
	}
	SCE_SAMPLE_UTIL_ASSERT(m_rawVoiceInfo != NULL);
	SceNgs2SamplerVoiceState voiceState;
	int ret = sceNgs2SamplerVoiceGetState(m_voiceHandle, &voiceState);
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT(ret >= SCE_OK);
	
	uint64_t remainDataSize = m_addedDataSize - voiceState.decodedDataSize;
	if(m_rawVoiceInfo->m_streamBufferSize < remainDataSize){
		//printf("%ld, %ld, %ld\n", m_rawVoiceInfo->m_streamBufferSize, m_addedDataSize, voiceState.decodedDataSize);
	}
	SCE_SAMPLE_UTIL_ASSERT(m_rawVoiceInfo->m_streamBufferSize >= remainDataSize);

	uint64_t emptySize = m_rawVoiceInfo->m_streamBufferSize - remainDataSize;
	SCE_SAMPLE_UTIL_ASSERT(m_rawVoiceInfo->m_streamBufferSize >= emptySize);
	return emptySize;
}

int ssai::VoiceInternal::_addData(const void* data, uint32_t dataSize)
{
	SCE_SAMPLE_UTIL_ASSERT(m_rawVoiceInfo != NULL);
	if(dataSize == 0){
		return SCE_OK;
	}
	SCE_SAMPLE_UTIL_ASSERT(0 <= m_rawVoiceInfo->m_streamBufferOffset);
	SCE_SAMPLE_UTIL_ASSERT((m_rawVoiceInfo->m_streamBufferOffset+dataSize) <= m_rawVoiceInfo->m_streamBufferSize);
	memcpy(&m_rawVoiceInfo->m_streamBuffer[m_rawVoiceInfo->m_streamBufferOffset], data, dataSize);


	SceNgs2WaveformBlock aBlock;
	aBlock.dataOffset = m_rawVoiceInfo->m_streamBufferOffset;
	aBlock.dataSize   = dataSize;
	aBlock.numRepeats = 0;
	aBlock.numSkipSamples = 0;
	aBlock.numSamples = dataSize/sizeof(uint16_t); // todo
	aBlock.reserved = 0;
	aBlock.userData = 0;

	//printf("size = %d\n", dataSize);
	int ret = sceNgs2SamplerVoiceAddWaveformBlocks( m_voiceHandle,
		m_rawVoiceInfo->m_streamBuffer, &aBlock, 1, SCE_NGS2_WAVEFORM_BLOCK_FLAG_CONTINUOUS);
	if(ret != SCE_OK){
		SceNgs2SamplerVoiceState state;
		sceNgs2SamplerVoiceGetState(m_voiceHandle, &state);
		//printf("sceNgs2SamplerVoiceAddWaveformBlocks ret=%#x empty=%d(%d), state=%#x\n", ret, getBufferEmptySize(), m_rawVoiceInfo->m_streamBufferSize, state.voiceState.stateFlags);
		return ret;
	}
	m_addedDataSize += dataSize;
	m_rawVoiceInfo->m_streamBufferOffset = (m_rawVoiceInfo->m_streamBufferOffset + dataSize) % m_rawVoiceInfo->m_streamBufferSize;
	return ret;
}

int ssai::VoiceInternal::addData(const void* data, uint32_t dataSize)
{
	SceNgs2WaveformBlock aBlock;
	aBlock.dataOffset = 0;
	aBlock.dataSize   = dataSize;
	aBlock.numRepeats = 0;
	aBlock.numSkipSamples = 0;
	aBlock.numSamples = dataSize/sizeof(uint16_t); // todo
	aBlock.reserved = 0;
	aBlock.userData = 0;

	//printf("size = %d\n", dataSize);
	return sceNgs2SamplerVoiceAddWaveformBlocks( m_voiceHandle,
		data, &aBlock, 1, SCE_NGS2_WAVEFORM_BLOCK_FLAG_CONTINUOUS);

#if 0
	//printf("addData   %d, %d\n", getBufferEmptySize(), dataSize);
	if(m_voiceData != NULL){
		return -1;
	}
	SCE_SAMPLE_UTIL_ASSERT(m_rawVoiceInfo != NULL);
	if(getBufferEmptySize() <  dataSize){
		//printf("empty size = %d\n", getBufferEmptySize());
		return -1;
	}
	
	if(m_rawVoiceInfo->m_streamBufferOffset > m_rawVoiceInfo->m_streamBufferSize){
		printf("error %d, %d\n", m_rawVoiceInfo->m_streamBufferOffset,  m_rawVoiceInfo->m_streamBufferSize);
	}
	SCE_SAMPLE_UTIL_ASSERT(m_rawVoiceInfo->m_streamBufferOffset <= m_rawVoiceInfo->m_streamBufferSize);
	uint32_t size1 = m_rawVoiceInfo->m_streamBufferSize - m_rawVoiceInfo->m_streamBufferOffset;
	uint32_t dataSize1 = (dataSize < size1) ? dataSize : size1;
	uint32_t dataSize2 = dataSize - dataSize1;

	int ret = _addData(data, dataSize1);
	if(ret != SCE_OK){
		return ret;
	}
	const void* data2 = (const void*)(((uintptr_t)data) + dataSize1);
	return _addData(data2, dataSize2);
#endif
}

int ssai::VoiceInternal::clearBuffer(void)
{
	return sceNgs2SamplerVoiceAddWaveformBlocks( m_voiceHandle, NULL, NULL, 0, SCE_NGS2_WAVEFORM_BLOCK_FLAG_CLEAR);
}


/* ------------------------------------------------------------------------------------------------*/
/* ------------------------------------------------------------------------------------------------*/


int ssai::BussImpl::setOutput(uint32_t nSourceOutputIndex, ssa::Buss *buss)
{
	ssai::BussImpl *bussImpl = dynamic_cast<ssai::BussImpl*>(buss);
	//printf("VOICE PATCH %#x => %#x\n", this->m_handle, bussImpl->m_handle);
	return sceNgs2VoicePatch( this->m_handle, nSourceOutputIndex, bussImpl->m_handle, 0);
}

int ssai::BussImpl::initialize(SceNgs2Handle handle, MasterBussRack *owner)
{
	m_handle = handle;
	m_owner = owner;
	return SCE_OK;
}
int ssai::BussImpl::finalize(void)
{
	if((m_handle != SCE_NGS2_HANDLE_INVALID) && (m_owner != NULL)){
		m_owner->onHandleReleased(m_handle);
	}
	m_owner = NULL;
	m_handle = -1;
	return SCE_OK;
}



#endif // defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS