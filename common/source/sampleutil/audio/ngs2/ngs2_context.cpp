/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#include <sampleutil/audio.h>
#include <sampleutil/sampleutil_common.h>
#include <sampleutil/sampleutil_error.h>
#include <libsysmodule.h>
#include "ngs2_internal.h"

#pragma comment(lib,"libSceSysmodule_stub_weak.a")
#pragma comment(lib,"libSceNgs2_stub_weak.a")

namespace ssa = sce::SampleUtil::Audio;
namespace ssai = sce::SampleUtil::Audio::Internal;


/* ------------------------------------------------------------------------------------------------*/

ssai::AudioContextInternal::AudioContextInternal(void)
{
	m_ngs2systemHandle = SCE_NGS2_HANDLE_INVALID;
}

ssai::AudioContextInternal::~AudioContextInternal(void)
{
	finalize();
}

int ssai::AudioContextInternal::initialize( const AudioContextOption *pOption )
{
	int ret = SCE_OK;

	ret = sceSysmoduleLoadModule( SCE_SYSMODULE_NGS2 );
	if( ret != SCE_OK ){
		return ret;
	}

	AudioContextOption option;
	if(pOption == NULL){
		pOption = &option;
	}
	m_option = *pOption;
	m_option.stereoVoiceNum = (m_option.stereoVoiceNum) < 1 ? 1 : m_option.stereoVoiceNum;
	m_option.monoVoiceNum   = (m_option.monoVoiceNum)   < 1 ? 1 : m_option.monoVoiceNum;
	m_option.at9VoiceNum    = (m_option.at9VoiceNum)    < 1 ? 1 : m_option.at9VoiceNum;
	m_option.reverbBussNum  = (m_option.reverbBussNum)  < 1 ? 1 : m_option.reverbBussNum;
	m_option.mixerBussNum   = (m_option.mixerBussNum)  < 1 ? 1 : m_option.mixerBussNum;


	if( m_ngs2systemHandle != SCE_NGS2_HANDLE_INVALID ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}


	SceNgs2ContextBufferInfo	bufferInfo;
	memset( &bufferInfo, 0, sizeof( SceNgs2ContextBufferInfo ) );

	ret = sceNgs2SystemQueryBufferSize( NULL, &bufferInfo );
	if( ret != SCE_OK ){
		return ret;
	}

	m_systemBufer = malloc( bufferInfo.hostBufferSize );
	if( m_systemBufer == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_NOT_INITIALIZED;
	}
	bufferInfo.hostBuffer = m_systemBufer;

	ret = sceNgs2SystemCreate( NULL, &bufferInfo, &m_ngs2systemHandle );
	if( ret != SCE_OK ){
		m_ngs2systemHandle = SCE_NGS2_HANDLE_INVALID;
		free(m_systemBufer);
		m_systemBufer = NULL;
		return ret;
	}


	ret = m_masteringRack.initialize(m_ngs2systemHandle, onSpeakerBussClosed, this); 
	if( ret != SCE_OK ){
		finalize();
		return ret;
	}



	SceNgs2SamplerRackOption voiceRackOption;
	ret = sceNgs2SamplerRackResetOption(&voiceRackOption);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	voiceRackOption.rackOption.maxMatrices = 16;

	ret = m_voiceRack.initialize( m_ngs2systemHandle, SCE_NGS2_RACK_ID_SAMPLER, &voiceRackOption.rackOption); 
	if( ret != SCE_OK ){
		finalize();
		return ret;
	}

	ret = m_audioOutThread.initialize( updateCallback, this);  
	if( ret != SCE_OK ){
		finalize();
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}

	return ret;
}

int ssai::AudioContextInternal::finalize(void)
{
	int ret = SCE_OK;

	if( m_ngs2systemHandle == SCE_NGS2_HANDLE_INVALID ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	m_audioOutThread.finalize();

	m_masteringRack.finalize();
	m_voiceRack.finalize();

	SceNgs2ContextBufferInfo	bufferInfo;
	ret = sceNgs2SystemDestroy( m_ngs2systemHandle, &bufferInfo );
	if( ret != SCE_OK ){
		return ret;
	}

	if(m_systemBufer != NULL){
		free(m_systemBufer);
		m_systemBufer = NULL;
	}
	m_ngs2systemHandle = SCE_NGS2_HANDLE_INVALID;

	ret = sceSysmoduleUnloadModule( SCE_SYSMODULE_NGS2 );
	if( ret != SCE_OK ){
		return ret;
	}

	return ret;
}

void ssai::AudioContextInternal::updateCallback( 
	SceNgs2RenderBufferInfo *buffers, uint32_t numBuffers,
	void* userAttr )
{
	AudioContextInternal* self = (AudioContextInternal*)userAttr;
	if(self->m_ngs2systemHandle == SCE_NGS2_HANDLE_INVALID ){
		return;
	}


	int ret = sceNgs2SystemRender( self->m_ngs2systemHandle, buffers, numBuffers );
	if(ret != SCE_OK){
		printf(__FILE__"(%d) ret = %#x\n" , __LINE__, ret);
	}

	if( self->m_option.contextUpdate ){
		self->m_option.contextUpdate( 
			buffers[0].buffer,
			buffers[0].bufferSize,
			buffers[1].buffer,
			buffers[1].bufferSize,
			self->m_option.userArg );
	}

	self->m_voiceRack.update();
}

int ssai::AudioContextInternal::createReverbBuss(ssa::Buss **outBuss, const int reverbMode)
{
	*outBuss = NULL;
	return -1;
}

int ssai::AudioContextInternal::createVoice(Voice **outVoice, const VoiceData *voiceData )
{
	if( voiceData == NULL || outVoice == NULL ) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if( m_ngs2systemHandle == SCE_NGS2_HANDLE_INVALID ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	int ret = SCE_OK;
	
	VoiceInternal *voice;
	ret = m_voiceRack.createVoice(&voice, voiceData);
	if( ret != SCE_OK ){
		return ret;
	}

	ret = voice->setOutput(0, m_masteringRack.getMainBuss());
	if( ret != SCE_OK ){
		delete voice;
		return ret;
	}

	*outVoice = voice;
	return SCE_OK;
}

int ssai::AudioContextInternal::createRawVoice(Voice** outVoice, uint32_t bufferSize, uint32_t waveformType, uint32_t numChannels, uint32_t sampleRate)
{
	if( bufferSize == 0 || outVoice == NULL ) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if( m_ngs2systemHandle == SCE_NGS2_HANDLE_INVALID ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	int ret = SCE_OK;
	
	VoiceInternal *voice;
	ret = m_voiceRack.createRawVoice(&voice, bufferSize, waveformType, numChannels, sampleRate);
	if( ret != SCE_OK ){
		return ret;
	}

	ret = voice->setOutput(0, m_masteringRack.getMainBuss());
	if( ret != SCE_OK ){
		delete voice;
		return ret;
	}

	*outVoice = voice;
	return SCE_OK;
}

ssa::Buss *ssai::AudioContextInternal::getMainBuss(void) 
{
	return m_masteringRack.getMainBuss();
}
ssa::Buss *ssai::AudioContextInternal::getBgmBuss(void) 
{
	return m_masteringRack.getBgmBuss();
}


int ssa::createAudioContext(ssa::AudioContext **outAudioContext, const ssa::AudioContextOption *option )
{
	if(outAudioContext == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssai::AudioContextInternal *context = new ssai::AudioContextInternal;
	int ret = context->initialize(option);
	if(ret != SCE_OK){
		delete context;
		return ret;
	}
	*outAudioContext = context;
	return SCE_OK;
}

#endif /*  defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS */