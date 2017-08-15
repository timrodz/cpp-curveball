/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_CPP_AUDIO_NGS_MODULE_H
#define _SCE_SAMPLE_UTIL_CPP_AUDIO_NGS_MODULE_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/audio.h>
#include <ngs.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Audio
		{
			namespace Internal
			{
				namespace Data
				{
					enum CodecType
					{
						CODEC_TYPE_PCM,
						CODEC_TYPE_ADPCM,
						CODEC_TYPE_AT9,
						CODEC_TYPE_GENERATOR
					};
				}


				class At9Module
				{
				private:
					SceNgsHVoice m_voiceHandler;
					SceNgsAT9Params *m_param;
					SceNgsAT9SkipBufferInfo m_skipBufInfo;
					SceNgsAT9Params *lock(void);
					void unlock(void);
				public:
					int initialize(SceNgsHVoice voiceHandler,
						int sampleRate,
						int channelNum,
						uint32_t loopStartOffset,
						uint32_t loopEndOffset,
						int32_t  at9configData);

					void setBuffer(uint32_t buffIndex, 
						const void* pBuffer,
						SceInt32 nNumBytes,
						SceInt16 nLoopCount = 0,
						SceInt16 nNextBuff = SCE_NGS_PLAYER_NO_NEXT_BUFFER,
						bool isStart=false,
						bool isEnd=false);
					
					void setCallback(const SceNgsModuleCallbackFunc callbackFuncPtr, void* arg);
					void notifyLoopedBuffer(int finishedLoopNum, const void *buffer);
					void resetBuffer(void);
					void setPitch(float pitch);
				};

				class PcmModule
				{
				private:
					SceNgsHVoice m_voiceHandler;
					SceNgsPlayerParams *m_param;
				public:

					int initialize(SceNgsHVoice voiceHandler, int codecType, int sampleRate, int channelNum);

					SceNgsPlayerParams *lock(void);

					void unlock(void);

					void setBuffer(uint32_t buffIndex, 
						const void* pBuffer,
						SceInt32 nNumBytes,
						SceInt16 nLoopCount = 0,
						SceInt16 nNextBuff = SCE_NGS_PLAYER_NO_NEXT_BUFFER);

					void setStartBuffer(int index);

					void setCallback(const SceNgsModuleCallbackFunc callbackFuncPtr, void* arg);

				};

				class VoiceModule
				{
				public:
					typedef void (*OnSwappedBufferCallback)(void *userArg);
					typedef void (*OnLoopedBufferCallback)(int finishedLoopNum, const  void *buffer, void* userArg);

				private:
					uint32_t m_nextBufferIndex;
					int initializeMixerModule(SceNgsHVoice hVoiceHandle, SceUInt32 mixerModuleId);

					static void at9ModuleCallback( const SceNgsCallbackInfo *pCallbackInfo );
					static void pcmModuleCallback( const SceNgsCallbackInfo *pCallbackInfo );

					OnSwappedBufferCallback m_onSwappedBufferCallback;
					OnLoopedBufferCallback  m_onLoopedBufferCallback;
					void *m_userArg;

					static const uint32_t NUM_BUFFERS = 3;
					enum ModuleType { MODULE_AT9, MODULE_PCM, MODULE_GENERATOR };
					ModuleType m_type;
					At9Module m_at9;
					PcmModule m_pcm;
				public:				
					int initializeAsAt9(SceNgsHVoice voiceHandler,
						int sampleRate,
						int channelNum,
						uint32_t loopStartOffset,
						uint32_t loopEndOffset,
						int32_t  at9configData);

					int initializeAsPcm(SceNgsHVoice voiceHandler, int codecType, int sampleRate, int channelNum);

					int initializeAsGenerator(SceNgsHVoice voiceHandler, const sce::SampleUtil::Audio::GeneratorSettings  *pNoise);

					int resetBuffer(void);

					int pushBuffer(const void *buffer, uint32_t numBytes, int loopCount=0,
						bool isStart=false,
						bool isEnd=false
						);

					int pushEndBuffer(void);


					void notifyLoopedBuffer(int finishedLoopNum, const  void *buffer);

					void setCallback(OnSwappedBufferCallback onSwappedBufferCallback,
						OnLoopedBufferCallback  onLoopedBufferCallback,
						void *userArg);

					int setPitch(float pitch);
				};



			}
		}
	}
}




#endif /* _SCE_TARGET_OS_PSP2 */

#endif /* _SCE_SAMPLE_UTIL_CPP_AUDIO_NGS_MODULE_H */
