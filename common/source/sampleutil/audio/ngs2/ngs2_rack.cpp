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

namespace ssa =sce::SampleUtil::Audio;
namespace ssai =sce::SampleUtil::Audio::Internal;

ssai::Rack::Rack( void ) :
	m_handle(SCE_NGS2_HANDLE_INVALID)
{
}

ssai::Rack::~Rack(void)
{
}

int ssai::Rack::initialize(SceNgs2Handle systemHandle, uint32_t rackId,  const SceNgs2RackOption *option )
{
	int ret = SCE_OK;

	if( m_handle != SCE_NGS2_HANDLE_INVALID ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	memset( &m_rackInfo, 0, sizeof(m_rackInfo) );
	SceNgs2ContextBufferInfo	bufferInfo;
	memset( &bufferInfo, 0, sizeof( SceNgs2ContextBufferInfo ) );
	ret = sceNgs2RackQueryBufferSize( rackId, option, &bufferInfo );
	if( ret != SCE_OK ){
		return ret;
	}

	bufferInfo.hostBuffer = malloc( bufferInfo.hostBufferSize );
	if( bufferInfo.hostBuffer == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_NOT_INITIALIZED;
	}

	ret = sceNgs2RackCreate( systemHandle, rackId, option, &bufferInfo, &m_handle );
	if( ret != SCE_OK ){
		m_handle = SCE_NGS2_HANDLE_INVALID;
		return ret;
	}

	ret = sceNgs2RackGetInfo( m_handle, &m_rackInfo, sizeof(m_rackInfo) );
	if( ret != SCE_OK ){
		finalize();
	}

	for(uint32_t i=0; i<m_rackInfo.maxVoices; i++){
		SceNgs2Handle voiceHandle;

		ret = sceNgs2RackGetVoiceHandle( m_handle, i, &voiceHandle );
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_availableHandles.push_back(voiceHandle);
	}

	return ret;
}

int ssai::Rack::finalize(void)
{
	int ret = SCE_OK;

	if( m_handle == SCE_NGS2_HANDLE_INVALID ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}


	m_availableHandles.clear();

	SceNgs2ContextBufferInfo	bufferInfo;
	ret = sceNgs2RackDestroy( m_handle, &bufferInfo );
	if( ret != SCE_OK ){
		return ret;
	}
	free( bufferInfo.hostBuffer );

	memset( &m_rackInfo, 0, sizeof(m_rackInfo) );
	m_handle = SCE_NGS2_HANDLE_INVALID;
	return ret;
}

SceNgs2Handle ssai::Rack::reserveHandle(void)
{
	std::vector<SceNgs2Handle>::iterator it = m_availableHandles.begin();
	if(it == m_availableHandles.end()){
		return SCE_NGS2_HANDLE_INVALID;
	}
	SceNgs2Handle voiceHandle = *it;
	m_availableHandles.erase(it);
	return voiceHandle;

}
void ssai::Rack::releaseHandle(SceNgs2Handle handle)
{
	m_availableHandles.push_back(handle);
}


/* --------------------------------------------------------------------- */

int ssai::MasterBussRack::initialize(SceNgs2Handle systemHandle,
	void (*onSpeakerBussClosed)(SceUserServiceUserId userId, void* userarg),
	void *userarg
	)
{

	SceNgs2MasteringRackOption masteringOption;
	sceNgs2MasteringRackResetOption( &masteringOption );
	masteringOption.rackOption.maxVoices = 6;
	
	int ret = Rack::initialize(systemHandle, SCE_NGS2_RACK_ID_MASTERING, &masteringOption.rackOption);
	if(ret != SCE_OK){
		return ret;
	}

	SceNgs2Handle masterHandle;
	ret = sceNgs2RackGetVoiceHandle( m_handle, 0, &masterHandle );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SCE_SAMPLE_UTIL_ASSERT(m_availableHandles.at(0) == masterHandle);
	m_mainBuss.initialize(m_availableHandles.at(0), NULL/*no callback is needed */);
	ret = sceNgs2MasteringVoiceSetup( m_mainBuss.getHandle(), SCE_NGS2_CHANNELS_2_0CH , 0);
	if( ret != SCE_OK ){
		finalize();
		return ret;
	}
	ret = sceNgs2MasteringVoiceSetOutput(m_mainBuss.getHandle(),0);
	if( ret != SCE_OK ){
		finalize();
		return ret;
	}

	ret = sceNgs2VoiceKickEvent( m_mainBuss.getHandle(), SCE_NGS2_VOICE_EVENT_PLAY );
	if( ret != SCE_OK ){
		finalize();
		return ret;
	}


	SceNgs2Handle bgmHandle;
	ret = sceNgs2RackGetVoiceHandle( m_handle, 1, &bgmHandle );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SCE_SAMPLE_UTIL_ASSERT(m_availableHandles.at(1) == bgmHandle);
	m_bgmBuss.initialize(m_availableHandles.at(1), NULL/*no callback is needed */);
	ret = sceNgs2MasteringVoiceSetup( m_bgmBuss.getHandle(), SCE_NGS2_CHANNELS_2_0CH , 0);
	if( ret != SCE_OK ){
		finalize();
		return ret;
	}
	ret = sceNgs2MasteringVoiceSetOutput(m_bgmBuss.getHandle(),1);
	if( ret != SCE_OK ){
		finalize();
		return ret;
	}

	ret = sceNgs2VoiceKickEvent( m_bgmBuss.getHandle(), SCE_NGS2_VOICE_EVENT_PLAY );
	if( ret != SCE_OK ){
		finalize();
		return ret;
	}

	for(uint32_t i=0; i<4; i++){
		m_speakerOutBuss[i].userId = SCE_USER_SERVICE_USER_ID_INVALID;
		m_speakerOutBuss[i].handle = m_availableHandles.at(2+i);
		
		ret = sceNgs2MasteringVoiceSetup( m_speakerOutBuss[i].handle, SCE_NGS2_CHANNELS_1_0CH , 0);
		if( ret != SCE_OK ){
			finalize();
			return ret;
		}
		ret = sceNgs2MasteringVoiceSetOutput(m_speakerOutBuss[i].handle,2+i);
		if( ret != SCE_OK ){
			finalize();
			return ret;
		}

		ret = sceNgs2VoiceKickEvent( m_speakerOutBuss[i].handle, SCE_NGS2_VOICE_EVENT_PLAY );
		if( ret != SCE_OK ){
			finalize();
			return ret;
		}

	}
/*
	printf("--- master rack ---\n");
	for(uint32_t i=0; i<6; i++){
		printf("%#x\n", m_availableHandles.at(i));
	}
*/
	m_onSpeakerBussClosed = onSpeakerBussClosed;
	m_userarg = userarg;
	return SCE_OK;
}


ssa::Buss *ssai::MasterBussRack::getMainBuss(void) 
{
	return &m_mainBuss;
}

ssa::Buss *ssai::MasterBussRack::getBgmBuss(void) 
{ 
	return &m_bgmBuss;
}


int ssai::MasterBussRack::createPadSpeakerBuss(ssa::Buss **outBuss, SceUserServiceUserId userId, uint32_t &outIndex)
{
	if(outBuss == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	int32_t index=-1;
	for(uint32_t i=0; i<4; i++){
		if(m_speakerOutBuss[i].userId == SCE_USER_SERVICE_USER_ID_INVALID){
			m_speakerOutBuss[i].userId = userId;
			index = i;
			break;
		}
	}
	if(index <0){
		return -1;
	}

	BussImpl *bi = new BussImpl;
	bi->initialize(m_speakerOutBuss[index].handle,this);
	*outBuss = bi;
	outIndex = index;
	return SCE_OK;
}


void ssai::MasterBussRack::onHandleReleased(SceNgs2Handle handle)
{
	for(uint32_t i=0; i<4; i++){
		if(m_speakerOutBuss[i].handle == handle){

			m_onSpeakerBussClosed(m_speakerOutBuss[i].userId, m_userarg);
			m_speakerOutBuss[i].userId = SCE_USER_SERVICE_USER_ID_INVALID;
			break;
		}
	}

}


/* --------------------------------------------------------------------- */

int ssai::VoiceRack::createVoice(VoiceInternal **voice, const VoiceData *voiceData)
{
	SceNgs2Handle h = reserveHandle();
	if(h == SCE_NGS2_HANDLE_INVALID){
		return SCE_SAMPLE_UTIL_ERROR_BUSY;
	}

	VoiceInternal *v = new VoiceInternal();
	int ret = v->initialize(h, this, voiceData);
	if(ret != SCE_OK){
		releaseHandle(h);
		delete v;
		return ret;
	}
	*voice = v;
	m_voices.push_back(v);
	return SCE_OK;
}
int ssai::VoiceRack::createRawVoice(VoiceInternal** outVoice, uint32_t bufferSize,
	uint32_t waveformType, uint32_t numChannels, uint32_t sampleRate)
{
	SceNgs2Handle h = reserveHandle();
	if(h == SCE_NGS2_HANDLE_INVALID){
		return SCE_SAMPLE_UTIL_ERROR_BUSY;
	}

	VoiceInternal *v = new VoiceInternal();
	int ret = v->initializeAsRawVoice(h, this, bufferSize, waveformType, numChannels, sampleRate);
	if(ret != SCE_OK){
		releaseHandle(h);
		delete v;
		return ret;
	}
	*outVoice = v;
	m_voices.push_back(v);
	return SCE_OK;
}


void ssai::VoiceRack::notifyVoiceDestroyed(VoiceInternal* voice)
{
	releaseHandle(voice->getHandle());

	std::vector<VoiceInternal*>::iterator it = std::find(m_voices.begin(), m_voices.end(), voice);
	if(it != m_voices.end()){
		m_voices.erase(it);
	}

}


int ssai::VoiceRack::finalize(void)
{
	std::vector<VoiceInternal*>::iterator it;
	const std::vector<VoiceInternal*>::const_iterator end = m_voices.end();
	for(it= m_voices.begin(); it != end; it++){
		(*it)->kill();
		delete (*it);
	}
	m_voices.clear();

	return Rack::finalize();

}

void ssai::VoiceRack::update(void)
{
	for(uint32_t i=0; i<m_voices.size(); i++){
		m_voices.at(i)->update();
	}
}

#endif // defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS