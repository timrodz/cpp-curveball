/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#ifndef _SCE_SAMPLE_UTIL_CPP_AUDIO_NGS_INTERNAL_H
#define _SCE_SAMPLE_UTIL_CPP_AUDIO_NGS_INTERNAL_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/audio.h>

#include <kernel.h>
#include <ngs.h>
#include <fios2.h>
#include <util/util.h>
#include "audio/audio_internal.h"
#include "ngs_module.h"

namespace sce
{
	namespace SampleUtil
	{
		namespace Audio
		{
			namespace Internal
			{
				int setPatchVolume(SceNgsHPatch patch, const Volume *volInfo);

				int connectRacks(   SceNgsHVoice hVoiceSource, 
					unsigned int nSourceOutputIndex, 
					SceNgsHVoice hVoiceDestination, 
					unsigned int nTargetInputIndex, 
					SceNgsHPatch* pPatch);




				class VoiceDataImpl : public VoiceData
				{
				public:
					enum Type
					{
						TYPE_ON_MEMORY,
						TYPE_STREAMING,
						TYPE_GENERATOR
					};

					Internal::Data::CodecType getCodecType(void) const
					{
						switch(m_type)
						{
						case TYPE_ON_MEMORY:
						case TYPE_STREAMING: 
							{
								switch(m_audioFile.m_codec)
								{
								case Data::kCodecAdpcm    : return Internal::Data::CODEC_TYPE_ADPCM;
								case Data::kCodecPcm      : return Internal::Data::CODEC_TYPE_PCM;
								case Data::kCodecAtrac9   : return Internal::Data::CODEC_TYPE_AT9;
								default: return Internal::Data::CODEC_TYPE_GENERATOR;
								}
							}
						default:             return Internal::Data::CODEC_TYPE_GENERATOR;
						};
					}

					uint32_t getNumChannels(void) const { return m_audioFile.m_numChannels; }
					int getSampleRate(void) const { return m_audioFile.m_sampleRate; }
					Type getDataType(void) const{	return m_type;	}

					const char* getFilePath(void) const
					{
						if(m_type != TYPE_STREAMING){
							return NULL;
						}
						return m_streamingFilePath.c_str();
					}
					const void* getFileImage(void) const
					{
						if(m_type != TYPE_ON_MEMORY){
							return NULL;
						}
						return m_fileImage;
					}


					int getBlockAlign(void) const
					{
						int align = 0;
						if(m_audioFile.m_codec == Data::kCodecAtrac9){
							if(m_audioFile.m_blockAlign > 0){
								align = m_audioFile.m_blockAlign;
							}
						}
						return align;
					}

					const Internal::Data::AudioFile* getAudioFileInfo(void) const
					{
						switch(m_type)
						{
						case TYPE_ON_MEMORY: 
						case TYPE_STREAMING: return &m_audioFile;
						default:             return NULL;
						};
					}

					struct Offset
					{
						uint32_t dataOffset;
						uint32_t dataSize;
						uint32_t alignedLoopStartOffset;
						uint32_t alignedLoopEndOffset;
					};
					
					void getOffset(Offset *offset) const
					{
						memset(offset, 0, sizeof(Offset));
						offset->dataOffset = m_audioFile.m_dataOffset;
						offset->dataSize = m_audioFile.m_dataSize;
						if((m_audioFile.m_loopStartOffset == 0) && (m_audioFile.m_loopEndOffset == 0)){
							offset->alignedLoopStartOffset = m_audioFile.m_dataOffset;
							offset->alignedLoopEndOffset = m_audioFile.m_dataOffset + m_audioFile.m_dataSize;
							return;
						}

						offset->alignedLoopStartOffset = m_audioFile.m_loopStartOffset;
						offset->alignedLoopEndOffset = m_audioFile.m_loopEndOffset;
					}

					VoiceDataImpl(void)
					{
						m_type = TYPE_GENERATOR;
					}
					virtual ~VoiceDataImpl(void)
					{
						finalize();
					}
					int initializeAsStreaming(const char *path);
					int initializeAsOnMemoryData(const char *path);
					int initializeAsGenerator(const GeneratorSettings *setting);
					int finalize(void);

				private:
					Type m_type;
					Internal::Data::AudioFile m_audioFile;

					std::string m_streamingFilePath;
					void *m_fileImage;
				public:
					GeneratorSettings     m_generatorInfo;

				};

				/* ------------------------------------------------------------------------------------ */



				class BussImpl;
				class VoiceRack;

				class VoiceInternal : public Voice
				{
					friend class VoiceRack;
				public:
					VoiceInternal(void)
					{
						m_voiceHandler = SCE_NGS_INVALID_HANDLE;
					}
					~VoiceInternal(void)
					{
						finalize();
					}

					int initialize(VoiceRack *rack, SceNgsHVoice voiceHandler, const VoiceData *voiceData);

					int play(int loopNum);

					int finalize(void);

					int  keyOff(void);
					int  kill(void);
					int pause(void);

					int resume(void);

					VoiceState getState(void) const
					{
						return m_state;
					}


					int setOutput(uint32_t nSourceOutputIndex, Buss *buss);


					bool isConnectedOutput(unsigned int nSourceOutputIndex)
					{
						return m_patchHandler[nSourceOutputIndex] != NULL;
					}

					int checkFinishedVoice(void);

					int setEnvelope(const Envelope* envelopeInfo);

					int setVolume(uint32_t outIndex, const Volume *volumeInfo);

					int setPitch(float pitch);
				private:

					SceNgsHVoice  getNgsVoice(void) const;

					int connectOutput(
						unsigned int nSourceOutputIndex, 
						SceNgsHVoice hVoiceDestination, 
						unsigned int nTargetInputIndex);

					int disConnectOutput(unsigned int nSourceOutputIndex);

					static void onSwappedBuffer(void *userArg);
					static void onLoopedBuffer(int finishedLoopNum, const  void *buffer, void* userArg);
					static int loadNextStreamingData(VoiceModule *voiceModule, sce::SampleUtil::Internal::Util::Stream	*stream, const VoiceDataImpl *data);

					VoiceModule m_voiceModule;

					sce::SampleUtil::Internal::Util::Stream	*m_stream;

					SceNgsHVoice	 m_voiceHandler;
					SceNgsHPatch	 m_patchHandler[NUM_OUTPUT_INDEX];

					const VoiceDataImpl *m_voiceData;



					VoiceState		 m_state;

					VoiceRack *m_rack;
				};

				class VoiceRack
				{
					friend class VoiceInternal;
				private:
					void				*m_rackMem;

					SceNgsHRack			 m_rack;

					std::vector<SceNgsHVoice> m_availableVoiceHandle;
					std::vector<VoiceInternal*> m_voices;

					void onClose(VoiceInternal *voice);
				public:
					VoiceRack(void)
					{
						m_rackMem = NULL;
					}
					~VoiceRack(void)
					{
						finalize();
					}
					int initialize(SceNgsHSynSystem sysHandle,const SceNgsRackDescription &rackDesc, int32_t idStart);
					int finalize(void);

					VoiceInternal *createVoice(const VoiceData *voiceData);

					int checkFinishedVoice(void)
					{
						int ret = SCE_OK;
						for(int i = 0; i < m_voices.size(); i++)	{
							ret = m_voices.at(i)->checkFinishedVoice();
							if(ret < SCE_OK){
								break;
							}
						}

						return ret;
					}
				};





				class BussRack;

				class BussImpl : public Buss
				{
					friend class VoiceInternal;
				private:
					SceNgsHPatch	 patchHandler[NUM_OUTPUT_INDEX];
					VoiceState		 m_state;
					SceNgsHVoice	 m_voiceHandler;

					int disConnectOutput(uint32_t nSourceOutputIndex);

					int connectOutput(
						unsigned int nSourceOutputIndex, 
						SceNgsHVoice hVoiceDestination, 
						unsigned int nTargetInputIndex=0);


					BussRack *m_bussRack;

				public:
					int initialize(BussRack *bussRack, SceNgsHVoice	 voiceHandler);

					int close(void);


					int setOutput(uint32_t nSourceOutputIndex, Buss *buss);

					int getData(void *buffer, uint32_t size);

					int play(void);

					int pause(void);

					int resume(void);

					VoiceState getState(void) const
					{
						return m_state;
					}

					int setReverbMode(SceNgsHSynSystem sysHandle, int reverbMode);

					BussImpl(void)
					{
						m_bussRack = NULL;
					}
					~BussImpl(void)
					{
						close();
					}
				};

				class BussRack
				{
					friend class BussImpl;
				private:
					SceNgsHRack				m_rack;

					void					*m_rackMem;

					std::vector<SceNgsHVoice> m_freeBussHandles;

				public:
					int initialize(SceNgsHSynSystem sysHandle, const SceNgsRackDescription &rackDesc);
					int finalize(void);

					Buss *createBuss(void);
					int releaseBuss(Buss *_buss);
				};


				class MasterBuss
				{
				private:
					BussRack m_masterRack;
					BussImpl		*m_mainBuss;
					BussImpl		*m_bgmBuss;
				public:

					int initialize(SceNgsHSynSystem		sysHandle, uint32_t voiceTotal, uint32_t bussTotal, int idStart);
					int finalize(void);

					int enableBgmPort(bool enable);

					Buss *getMainBuss(void);
					Buss *getBgmBuss(void);

					int start(void);
				};



					struct VoiceManager
					{
						static const int	MIXER_BUSS_MODULE_NUM		=		1;
						static const int 	REVERB_BUSS_MODULE_NUM		=		2;

					private:
						const AudioContextOption *m_option;
						int						masterBussMaxNum;
					public:
						VoiceRack m_monoRack;
						VoiceRack m_stereoRack;
						VoiceRack m_at9Rack;

						BussRack m_mixerRack;
						BussRack m_reverbRack;

						MasterBuss m_masterBuss;

						int init(SceNgsHSynSystem		sysHandle, 
							const AudioContextOption *option
							);

						int finalize(void);

						VoiceRack *getVoiceRack(bool isAt9, int channels);

						int checkFinishedVoices(void);


						bool isMonoVoice(int voiceId)
						{
							return (voiceId >= 0) && (voiceId < m_option->monoVoiceNum);
						}
						bool isStereoVoice(int voiceId)
						{
							return (voiceId >= m_option->monoVoiceNum)
								&& (voiceId < (m_option->monoVoiceNum + m_option->stereoVoiceNum));
						}
						bool isAt9Voice(int voiceId)
						{
							return (voiceId >= (m_option->monoVoiceNum + m_option->stereoVoiceNum))
								&& (voiceId < (m_option->monoVoiceNum + m_option->stereoVoiceNum + m_option->at9VoiceNum));
						}

						Buss *getMainBuss(void)
						{
							return m_masterBuss.getMainBuss();
						}

						Buss *getBgmBuss(void)
						{
							return m_masterBuss.getBgmBuss();
						}

					};


				typedef void (*AudioOutRenderFunction)(void* mainBuffer, uint32_t mainBufferSize, 
					void* bgmBuffer, uint32_t bgmBufferSize, 
					void* userAttr);

				struct AudioOutThreadInfo{
					SceInt32					priority;
					SceSize						stackSize;
					SceInt32					cpuAffinityMask;
				};

				class MainOutWorkerThread
				{
				public:
					static const uint32_t GRAIN_MAX = 1024;

					int32_t start(AudioOutRenderFunction renderFunction,
						void					*userArg,
						uint32_t              numGrain, 
						AudioOutThreadInfo    threadInfo);

					int32_t stop(void);
				private:
					SceUID             m_updateThreadId;	// Update thread ID
					volatile uint32_t m_isAborted;	        // Is aborted ?
					uint32_t m_numGrain;				    // Nubmer of grain
					AudioOutRenderFunction m_renderFunction;	// Rendering function
					void	*m_userArg;

					static int32_t updateThread(uint32_t args, void *argc);

				};

				class AudioContextImpl : public AudioContext
				{
				public:
					AudioContextImpl(void) :
					  m_mutex("AudioUtilMutex")
					{
						m_isInitialized = false;
					}
					int initialize(const AudioContextOption *option);
					int finalize(void);

					virtual ~AudioContextImpl(void)
					{
						finalize();
					}
					void lock(void)
					{
						m_mutex.lock();
					}
					void unlock(void)
					{
						m_mutex.unlock();
					}

					int closeVoiceFromID(const int voiceID);


					static 	void updateCallabck(void* mainBuffer, uint32_t mainBufferSize, 
						void* bgmBuffer, uint32_t bgmBufferSize, 
						void* userAttr);

					void process(void* mainBuffer, uint32_t mainSize,
						void* bgmBuffer, uint32_t bgmSize);

					int createVoice(Voice **outVoice, const VoiceData *voiceData);

					Buss *bussCreateReverb(int reverbMode);


					Buss *getMainBuss(void)
					{
						return m_voiceManager.getMainBuss();
					}
					Buss *getBgmBuss(void)
					{
						return m_voiceManager.getBgmBuss();
					}

					int createReverbBuss(Buss **outBuss, const int reverbMode)
					{
						*outBuss = bussCreateReverb(reverbMode);
						return SCE_OK;
					}
				private:
					int initNGS(const SceNgsSystemInitParams* initParams);
					int exitNgs(void);

					void					*m_sysMem;
					SceNgsHSynSystem		 m_sysHandle;

					SceNgsSystemInitParams	m_ngsInitParams;
					AudioContextOption m_option;

					sce::SampleUtil::Internal::Util::Mutex m_mutex;

					bool m_isInitialized;

					VoiceManager m_voiceManager;

					MainOutWorkerThread		 m_audioOutMain;
				};


			}
		}
	}
}


#endif /* _SCE_TARGET_OS_PSP2 */

#endif /* _SCE_SAMPLE_UTIL_CPP_AUDIO_NGS_INTERNAL_H */
