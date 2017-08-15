/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_AUDIO_NGS2_INTERNAL_H
#define _SCE_SAMPLE_UTIL_AUDIO_NGS2_INTERNAL_H

#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#include "sampleutil_internal.h"
#include <sampleutil/audio.h>

#include <ngs2.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <fios2.h>
#include "ngs2_stream.h"
#include <util/util.h>
#include <audioout.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Audio
		{

			namespace Data
			{
				struct FileInfo 
				{
					SceNgs2WaveformInfo		waveformInfo;

					int initFromPath(const char *path);
				};


				struct OnMemorySoundInfo 
				{
				private:
					FileInfo        *m_fileInfo;
				public:
					void setFileInfo(FileInfo *fileInfo)
					{
						m_fileInfo = fileInfo;
					}
					void			*m_data;

					bool hasPrologue(void) const
					{
						return m_fileInfo->waveformInfo.loopBeginPosition > m_fileInfo->waveformInfo.dataOffset;
					}
					bool hasEpilogue(void) const
					{
						return m_fileInfo->waveformInfo.loopEndPosition < (m_fileInfo->waveformInfo.dataOffset + m_fileInfo->waveformInfo.dataSize);
					}

					void *getPrologue(void) const
					{
						if(hasPrologue()){
							return m_data;
						}else{
							return NULL;
						}
					}
					void *getMainLoop(void) const
					{
						if(hasPrologue()){
							return (void*)(((uintptr_t)m_data) + m_fileInfo->waveformInfo.loopBeginPosition - m_fileInfo->waveformInfo.dataOffset);
						}else{
							return m_data;
						}
					}
					void *getEpilogue(void) const
					{
						if(hasEpilogue()){
							return (void*)(((uintptr_t)m_data) + m_fileInfo->waveformInfo.loopEndPosition - m_fileInfo->waveformInfo.dataOffset);
						}else{
							return NULL;
						}
					}
					uint32_t getPrologueSize(void) const
					{
						if(hasPrologue()){
							return m_fileInfo->waveformInfo.loopBeginPosition - m_fileInfo->waveformInfo.dataOffset;
						}else{
							return 0;
						}
					}
					uint32_t getEpilogueSize(void) const
					{
						if(hasEpilogue()){
							return (m_fileInfo->waveformInfo.dataOffset + m_fileInfo->waveformInfo.dataSize) - m_fileInfo->waveformInfo.loopEndPosition;
						}else{
							return 0;
						}
					}
					uint32_t getMainLoopSize(void) const
					{
						return m_fileInfo->waveformInfo.dataSize - getPrologueSize() - getEpilogueSize();
					}
				};



				struct StreamingInfo  
				{
					FileInfo        m_fileInfo;
					std::string     m_filePath;
				};
			};

			namespace Internal
			{
				class VoiceRack;


				class VoiceDataImpl : public VoiceData, public sce::SampleUtil::Impl::ResourceImpl
				{
					friend class AudioContextInternal;
					friend class VoiceInternal;
				private:
					enum Type
					{
						TYPE_ON_MEMORY,
						TYPE_STREAMING,
						TYPE_GENERATOR,
						TYPE_UNKNOWN
					};
					Type m_type;

					Data::FileInfo m_fileInfo;

					Data::OnMemorySoundInfo m_onMemorySoundInfo;
					Data::StreamingInfo     m_streamingSoundInfo;
					GeneratorSettings     m_generatorInfo;
				public:
					VoiceDataImpl(void)
					{
						m_type = TYPE_UNKNOWN;
					}
					virtual ~VoiceDataImpl(void)
					{
						finalize();
					}

					int initializeAsStreaming(const char *path);

					int initializeAsOnMemoryData(const char *path);

					int initializeAsGenerator(const GeneratorSettings *setting);

					int finalize(void);

					virtual std::string getResourceType(void) const
					{
						return "Audio::VoiceData";
					}
				};

				struct RawVoiceInfo
				{
					uint32_t m_streamBufferSize;
					uint8_t *m_streamBuffer;
					uint32_t m_streamBufferOffset;

					uint32_t m_waveformType;
					uint32_t m_numChannels;
					uint32_t m_sampleRate;
				};

				class VoiceInternal : public Voice, public sce::SampleUtil::Impl::ResourceImpl
				{
				protected:
					SceNgs2Handle m_voiceHandle;
					VoiceRack *m_rack;
					const Internal::VoiceDataImpl *m_voiceData;
					Stream *m_stream;
					uint64_t m_addedDataSize;
					VoiceState m_state;

					void loadNextStreamData(bool clear);

					RawVoiceInfo *m_rawVoiceInfo;
				public:
					VoiceInternal(void)
					{
						m_voiceHandle = SCE_NGS2_HANDLE_INVALID;
						m_state = kVoiceStateAvailable;
					}			

					virtual ~VoiceInternal(void)
					{
						finalize();
					}
					int initialize(SceNgs2Handle voiceHandle, VoiceRack *rack, const VoiceData *voiceData);
					int initializeAsRawVoice(SceNgs2Handle voiceHandle, VoiceRack *rack, uint32_t bufferSize, uint32_t waveformType, uint32_t numChannels, uint32_t sampleRate);
					int finalize(void);

					SceNgs2Handle getHandle(void) const
					{
						return m_voiceHandle;
					}

					int play( int loopCount=0 );
					int kill(void);
					int pause(void);
					int resume(void);
					int keyOff(void);

					int setOutput(uint32_t port, Buss *buss);

					VoiceState getState(void) const;

					void update(void);
					virtual std::string getResourceType(void) const
					{
						return "Audio::Voice";
					}

					int setVolume(uint32_t outIndex, const Volume *volume);

					int clearBuffer(void);
					uint32_t getBufferEmptySize(void);
					int addData(const void* data, uint32_t dataSize);
				private:
					int _addData(const void* data, uint32_t dataSize);
				};


				class Rack
				{
				public:
					Rack( void );
					~Rack(void);
					int initialize(SceNgs2Handle systemHandle, uint32_t rackId, const SceNgs2RackOption *option = NULL );
					int finalize(void);

					SceNgs2Handle reserveHandle(void);
					void releaseHandle(SceNgs2Handle handle);

				protected:
					SceNgs2Handle						m_handle;
					SceNgs2RackInfo						m_rackInfo;
					std::vector<SceNgs2Handle> m_availableHandles;
				};

				class VoiceRack : public Rack
				{
				public:
					int createVoice(VoiceInternal **voice, const VoiceData *voiceData);
					int createRawVoice(VoiceInternal** outVoice, uint32_t bufferSize, 
						uint32_t waveformType, uint32_t numChannels, uint32_t sampleRate);
					void notifyVoiceDestroyed(VoiceInternal* voice);
					int finalize(void);
					void update(void);
				private:
					std::vector<VoiceInternal*> m_voices;
				};

				class MasterBussRack;

				class BussImpl : public Buss, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:

					SceNgs2Handle m_handle;
					MasterBussRack *m_owner;
				public:
					BussImpl(void)
					{
						m_handle = SCE_NGS2_HANDLE_INVALID;
						m_owner = NULL;
					}
					~BussImpl(void)
					{
						finalize();
					}

					int initialize(SceNgs2Handle handle, MasterBussRack *owner);
					int finalize(void);

					int setOutput(uint32_t nSourceOutputIndex, Buss *buss);

					SceNgs2Handle getHandle(void) const { return m_handle; }
					virtual std::string getResourceType(void) const
					{
						return "Audio::Buss";
					}
				};
				
				class MasterBussRack : public Rack
				{
				private:
					BussImpl m_mainBuss;
					BussImpl m_bgmBuss;
					struct SpeakerOutInfo
					{
						SceUserServiceUserId userId;
						SceNgs2Handle handle;
					};
					SpeakerOutInfo m_speakerOutBuss[4];

					void (*m_onSpeakerBussClosed)(SceUserServiceUserId userId, void* userarg);
					void *m_userarg;
				public:

					int initialize(SceNgs2Handle systemHandle,
						void (*onSpeakerBussClosed)(SceUserServiceUserId userId, void* userarg),
						void *userarg
						);

					Buss *getMainBuss(void);
					Buss *getBgmBuss(void);
					int createPadSpeakerBuss(Buss **outBuss, SceUserServiceUserId userId, uint32_t &outIndex);

					void onHandleReleased(SceNgs2Handle handle);
				};

				typedef void (*OnRenderCallback)( SceNgs2RenderBufferInfo *buffers, uint32_t numBuffers,  void *userArg );


				class AudioPortManager
				{
				public:
					static const uint32_t GRAIN_MAX = 256;

					AudioPortManager(void);
					~AudioPortManager(void);
					int initialize( OnRenderCallback renderFunction,	void *  userArg);
					int finalize(void);

					int openPadSpeakerPort(SceUserServiceUserId userId, uint32_t index);
					int closePadSpeakerPort(SceUserServiceUserId userId);
				private:
					enum {
						STATE_STOP,
						STATE_START
					};
					sce::SampleUtil::Internal::Util::Mutex m_mutex;


					ScePthread					m_thread;
					volatile uint32_t			m_state;
					volatile uint32_t			m_isAborted;		// Is aborted ?
					uint32_t					m_numGrain;			// Nubmer of grain
					OnRenderCallback		m_renderFunction;	// Rendering function
					void						*m_userArg;
					static void* updateThread(void *argc);
					
					int16_t  m_mainBuffer[2][2*GRAIN_MAX];
					int16_t  m_bgmBuffer[2][2*GRAIN_MAX];
					int16_t  m_padSpeakerBuffer[4][2][GRAIN_MAX];

					int32_t m_mainPortId;
					int32_t m_bgmPortId;
					struct PadSpeakerPortInfo
					{
						SceUserServiceUserId userId;
						int32_t portId;
					};
					PadSpeakerPortInfo m_padSpeakerPorts[4];
				};


				class AudioContextInternal : public AudioContext, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					AudioContextInternal(void);
					~AudioContextInternal(void);

					int initialize( const AudioContextOption *option );
					int finalize(void);
					int createReverbBuss(Buss **outBuss, const int reverbMode);
					int createVoice( Voice **outVoice, const VoiceData *voiceData);
					int createRawVoice(Voice** outVoice, uint32_t bufferSize, uint32_t waveformType, uint32_t numChannels, uint32_t sampleRate);
					Buss *getMainBuss(void);
					Buss *getBgmBuss(void) ;

					int createPadSpeakerBuss(Buss **outBuss, SceUserServiceUserId userId)
					{
						if(outBuss == NULL){
							return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
						}

						uint32_t index;
						int ret = m_masteringRack.createPadSpeakerBuss(outBuss, userId, index);
						if(ret != SCE_OK){
							return ret;
						}

						ret = m_audioOutThread.openPadSpeakerPort(userId, index);
						if(ret != SCE_OK){
							sce::SampleUtil::destroy(*outBuss);
							return ret;
						}

						return SCE_OK;
					}

					static void onSpeakerBussClosed(SceUserServiceUserId userId, void* userarg)
					{
						AudioContextInternal *context = (AudioContextInternal *)userarg;
						context->m_audioOutThread.closePadSpeakerPort(userId);
					}

					virtual std::string getResourceType(void) const
					{
						return "Audio::AudioContext";
					}

				private:
					AudioContextOption		m_option;
					SceNgs2Handle           m_ngs2systemHandle;
					void*                  m_systemBufer;
					VoiceRack				m_voiceRack;
					MasterBussRack			m_masteringRack;
					AudioPortManager		m_audioOutThread;
					static void updateCallback( 
						SceNgs2RenderBufferInfo *buffers, uint32_t numBuffers,
						void* userAttr );	/* for internal (callback function) */
				};
			}

		}
	}
}

#endif /* #if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS */

#endif /* _SCE_SAMPLE_UTIL_AUDIO_NGS2_INTERNAL_H */