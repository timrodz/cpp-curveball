/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <scebase_common.h>
#include <sampleutil/audio.h>
#include "audio_internal.h"

using namespace sce::SampleUtil::Audio;
using namespace sce::Vectormath::Simd::Aos;

namespace ssa = sce::SampleUtil::Audio;
namespace ssaid = ssa::Internal::Data;
namespace ssiu = sce::SampleUtil::Internal::Util;

#if _SCE_TARGET_OS_WINDOWS

/* temporarlily disable Xaudio2, because it is included in D3DX11, but D3DX11 is deprecated, 
and Xaudio2 is not moved to standard windows library yet at windows 7 */
#define DISABLE_XAUDIO


#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <math.h>

//#include "SDKwavefile.h"

#include <windows.h>
#include <xaudio2.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits.h>
#include <stdint.h>
#include "sampleutil_internal.h"

class VoiceDataImpl : public VoiceData, public sce::SampleUtil::Impl::ResourceImpl
{
public:
	WAVEFORMATEXTENSIBLE m_formatex;
	XAUDIO2_BUFFER m_buffer;
	uint8_t *data;
	bool m_isValid;
public:
	VoiceDataImpl(void)
	{
		m_isValid = false;
	}
	int initializeFromFile(const char* filename)
	{
		memset(&m_formatex, 0, sizeof(m_formatex));
		memset(&m_buffer, 0, sizeof(m_buffer));

		int ret;

		ssiu::Reader reader;
		ret = reader.initializeFromFile(filename);
		if(ret != SCE_OK){
			return ret;
		}
		
		ssaid::AudioFile audioFile;
		ret = audioFile.parse(&reader);
		if(ret != SCE_OK){
			return ret;
		}
		reader.finalize();

		uint32_t size;
		data = (uint8_t*)ssiu::malloc_and_load(filename, &size);
		
		if(audioFile.m_hasExFormat){
			m_formatex.Format.wFormatTag   = WAVE_FORMAT_EXTENSIBLE;
		}else{
			if((audioFile.m_codec == ssaid::kCodecAtrac9) || (audioFile.m_codec == ssaid::kCodecPcm)){
				m_formatex.Format.wFormatTag        = WAVE_FORMAT_PCM;
			}else if(audioFile.m_codec == ssaid::kCodecAdpcm){
				m_formatex.Format.wFormatTag        = WAVE_FORMAT_ADPCM;
			}else{
				return SCE_OK;
			}
		}

		m_formatex.Format.nChannels         = audioFile.m_numChannels;
		m_formatex.Format.wBitsPerSample    = audioFile.m_bitsPerSample;
		m_formatex.Format.nSamplesPerSec    = audioFile.m_sampleRate;
		m_formatex.Format.nBlockAlign       = audioFile.m_blockAlign;
		m_formatex.Format.nAvgBytesPerSec   = audioFile.m_averageBytesPerSec;
		if(audioFile.m_hasExFormat){
			m_formatex.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);

			m_formatex.dwChannelMask = audioFile.m_channelMask;
			m_formatex.Samples.wSamplesPerBlock = audioFile.m_samplesPerBlock;
			memcpy(&m_formatex.SubFormat, audioFile.m_subFormat, 16);
		}else{
			m_formatex.Format.cbSize = 0;
		}
		
		
		m_buffer.AudioBytes = audioFile.m_dataSize;
		m_buffer.pAudioData = &data[ audioFile.m_dataOffset ];         //バッファの先頭アドレス
		m_buffer.Flags = XAUDIO2_END_OF_STREAM;   // tell the source voice not to expect any data after this buffer
		m_isValid = true;

		return SCE_OK;
	}

	bool isAt9(void) const
	{
		return (m_formatex.Format.wFormatTag   == WAVE_FORMAT_EXTENSIBLE)
			&& (memcmp(&m_formatex.SubFormat, ssaid::WavFileInfo::kSubformatAtrac9, 16) == 0);
	}
	bool isAdpcm(void) const
	{
		return m_formatex.Format.wFormatTag     == WAVE_FORMAT_ADPCM;

	}


	int finalize(void)
	{
		if(data != NULL){
			free(data);
			data = NULL;
		}
		m_isValid = false;
		return 0;
	}

	void dump(void) const
	{
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("[Format]\n"));
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  wFormatTag      %#x\n"), m_formatex.Format.wFormatTag);
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  nChannels       %#x\n"), m_formatex.Format.nChannels);
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  wBitsPerSample  %#x\n"), m_formatex.Format.wBitsPerSample);
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  nSamplesPerSec  %#x\n"), m_formatex.Format.nSamplesPerSec);
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  nBlockAlign     %#x\n"), m_formatex.Format.nBlockAlign);
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  nAvgBytesPerSec %#x\n"), m_formatex.Format.nAvgBytesPerSec);
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  cbSize          %#x\n"), m_formatex.Format.cbSize);
		if(m_formatex.Format.cbSize > 0){
			sce::SampleUtil::Impl::debugPrintfln(_TEXT("  dwChannelMask =%#x\n"), m_formatex.dwChannelMask);
			sce::SampleUtil::Impl::debugPrintfln(_TEXT("  wValidBitsPerSample=%#x\n"), m_formatex.Samples.wValidBitsPerSample);
			uint8_t *guid = (uint8_t*)&m_formatex.SubFormat;
			for(uint32_t i=0; i<16; i++){
				sce::SampleUtil::Impl::debugPrintfln(_TEXT("%#2x "), guid[i]);
			}
			sce::SampleUtil::Impl::debugPrintfln(_TEXT("\n"));
		}
		
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("[Buffer]\n"));
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  AudioBytes      %#x\n"), m_buffer.AudioBytes);
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  pAudioData      %#x\n"), m_buffer.pAudioData);
		sce::SampleUtil::Impl::debugPrintfln(_TEXT("  Flags           %#x\n"), m_buffer.Flags);
	}


	std::string getResourceType(void) const
	{
		return "Audio::VoiceData";
	}
};

class VoiceInternalWin : public Voice, public sce::SampleUtil::Impl::ResourceImpl
{
public:
	IXAudio2SourceVoice* m_source_voice;
	const VoiceDataImpl *m_voiceDataImpl;
	VoiceInternalWin(void)
	{
		m_source_voice = NULL;
	}
	int initialize(IXAudio2* xaudio, const VoiceData *voiceData)
	{

		const VoiceDataImpl *voiceDataImpl = dynamic_cast<const VoiceDataImpl *>(voiceData);
		if(voiceDataImpl == NULL){
			return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
		}
		if(! voiceDataImpl->m_isValid){
			return SCE_OK;
		}

		HRESULT hr;
		//voiceDataImpl->dump();
		if(voiceDataImpl->isAt9() || voiceDataImpl->isAdpcm()){
			m_source_voice = NULL; // not supported yet.
			return SCE_OK;
		}

		voiceDataImpl->dump();
#ifndef DISABLE_XAUDIO
		hr = xaudio->CreateSourceVoice( &m_source_voice, &voiceDataImpl->m_formatex.Format );
		if(FAILED(hr)){
			return -1;
		}
#endif
		m_voiceDataImpl = voiceDataImpl;
		return 0;
	}

	int play(int loopNum)
	{
#ifndef DISABLE_XAUDIO
		if(m_source_voice == NULL){
			return SCE_OK;
		}

		HRESULT hr = m_source_voice->SubmitSourceBuffer( &m_voiceDataImpl->m_buffer );
		if(FAILED(hr)){
			return -1;
		}

		hr = m_source_voice->Start();
		if(FAILED(hr)){
			return -1;
		}
#endif
		return 0;
	}
	int kill(void)
	{
#ifndef DISABLE_XAUDIO
		if(m_source_voice == NULL){
			return SCE_OK;
		}

		HRESULT hr = m_source_voice->Stop();
		if(FAILED(hr)){
			return -1;
		}
		m_source_voice->Discontinuity();
		m_source_voice->FlushSourceBuffers();
#endif
		return 0;
	}

	int finalize(void)
	{
#ifndef DISABLE_XAUDIO
		if(m_source_voice == NULL){
			return SCE_OK;
		}

		m_source_voice->DestroyVoice();
		m_source_voice = NULL;
#endif
		return 0;
	}


	int keyOff(void)
	{
		return SCE_OK;
	}

	int pause(void)
	{
		return SCE_OK;
	}

	int resume(void)
	{
		return SCE_OK;
	}

	int setOutput( uint32_t outIndex, Buss *buss )
	{
		return SCE_OK;
	}

	VoiceState getState(void) const
	{
		return kVoiceStateAvailable;
	}

	int setVolume(uint32_t outIndex, const Volume *volInfo)
	{
		return SCE_OK;
	}

	std::string getResourceType(void) const
	{
		return "Audio::Voice";
	}


};


class BussImplWin : public ssa::Buss, public sce::SampleUtil::Impl::ResourceImpl
{
public:
	int setOutput(uint32_t nSourceOutputIndex, Buss *buss)
	{
		return SCE_OK;
	}

	std::string getResourceType(void) const
	{
		return "Audio::Buss";
	}

};

class AudioContextImplWin : public ssa::AudioContext, public sce::SampleUtil::Impl::ResourceImpl
{
private:
	IXAudio2* m_xaudio;
	IXAudio2MasteringVoice* m_mastering_voice;

	BussImplWin m_dummyBuss;
public:
	AudioContextImplWin(void)
	{
		m_xaudio = NULL;
		m_mastering_voice = NULL;
	}
	int initialize(void)
	{
#ifndef DISABLE_XAUDIO
		HRESULT hr;
		hr = CoInitializeEx( NULL, COINIT_MULTITHREADED);
		if( FAILED( hr ) ) {
			return -1;
		}
		
		UINT32 flags = 0;
		hr = XAudio2Create( &m_xaudio, flags );
		if( FAILED(hr)){
			return -1;
		}
		
		//  Create a mastering voice
		hr = m_xaudio->CreateMasteringVoice( &m_mastering_voice );
		if(FAILED(hr)){
			return -1;
		}
#endif
		return 0;

	}
	int finalize(void)
	{
#ifndef DISABLE_XAUDIO
		if(m_xaudio != NULL){
			// Cleanup XAudio2
			if( m_mastering_voice != NULL )  {
				m_mastering_voice->DestroyVoice();
				m_mastering_voice = NULL;
			}
			if( m_xaudio != NULL )  {
				m_xaudio->Release();
				m_xaudio = NULL;
			}
			CoUninitialize();
		}
#endif
		return 0;
	}
	~AudioContextImplWin(void)
	{
		finalize();
	}
	

	virtual Buss *getMainBuss(void)
	{
		return &m_dummyBuss;
	}
	virtual Buss *getBgmBuss(void)
	{
		return &m_dummyBuss;
	}

	virtual int createReverbBuss(Buss **outBuss, const int reverbMode)
	{
		if(outBuss == NULL){
			return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
		}
		BussImplWin *buss = new BussImplWin;
		*outBuss = buss;
		return SCE_OK;
	}


	virtual int createVoice(Voice** outVoice, const VoiceData *voiceData )
	{
		if( (outVoice == NULL) || (voiceData == NULL) ) {
			return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
		}
		VoiceInternalWin *v = new VoiceInternalWin;
		int ret = v->initialize(m_xaudio, voiceData);
		if(ret != SCE_OK){
			delete v;
			return ret;
		}
		*outVoice = v;
		return SCE_OK;
	}

	std::string getResourceType(void) const
	{
		return "Audio::AudioContext";
	}

};

int ssa::createAudioContext(AudioContext **outContext, const AudioContextOption *option)
{
	if(outContext == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	AudioContextImplWin *context = new AudioContextImplWin();
	int ret = context->initialize();
	if(ret != SCE_OK){
		delete context;
		return ret;
	}
	*outContext = context;
	
	return SCE_OK;
}


/* ------------------------------------------------------------------------------------------------*/


int ssa::createVoiceDataFromFile(class sce::SampleUtil::Audio::VoiceData **outData, char const *path, bool isStreaming)
{
	if( outData == NULL || path == NULL ) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	
	VoiceDataImpl *voiceData = new VoiceDataImpl;
	int ret = voiceData->initializeFromFile(path);
	sce::SampleUtil::Impl::debugPrintfln(_TEXT("loading %s => %#x\n"), path, ret);
	if(ret != SCE_OK){
		delete voiceData;
		return ret;
	}
	*outData = voiceData;
	return SCE_OK;
}


/* ------------------------------------------------------------------------------------------- */


#endif /* _SCE_TARGET_OS_WINDOWS */
