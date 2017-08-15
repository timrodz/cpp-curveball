/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_AUDIO_VOICE_H
#define _SCE_SAMPLE_UTIL_AUDIO_VOICE_H

#include <sampleutil/sampleutil_common.h>

#ifdef __psp2__
#include <ngs.h>  // for envelope
#endif /* __psp2__ */

namespace sce
{
	namespace SampleUtil
	{
		/*!
		 * @~English
		 * @brief Audio-associated definitions 
		 * @details These are the audio-associated definitions. 
		 * @~Japanese
		 * @brief Audio関連の定義 
		 * @details Audio関連の定義です。 
		 */
		namespace Audio
		{
			class Buss;

			static const int 	NUM_OUTPUT_INDEX = 4;

			/*!
			 * @~English
			 * @brief Infinite loop 
			 * @~Japanese
			 * @brief 無限ループ 
			 */
			static const int	LOOP_NUM_INFINITE			= -1;

			/*!
			 * @~English
			 * @brief The state of a voice 
			 * @details This gets the state of a voice. 
			 * @~Japanese
			 * @brief ボイスの状態 
			 * @details ボイスの状態取得で使用します。 
			 */
			enum VoiceState
			{
				/*!
				 * @~English
				 * @brief Usable conditions 
				 * @~Japanese
				 * @brief 利用可能な状態 
				 */
				kVoiceStateAvailable, 

				/*!
				 * @~English
				 * @brief Paused 
				 * @~Japanese
				 * @brief 一時停止中 
				 */
				kVoiceStatePause,

				/*!
				 * @~English
				 * @brief Keying off 
				 * @~Japanese
				 * @brief キーオフ中 
				 */
				kVoiceStateKeyoff,

				/*!
				 * @~English
				 * @brief Playing 
				 * @~Japanese
				 * @brief 再生中 
				 */
				kVoiceStatePlaying,

				/*!
				 * @~English
				 * @brief Invalid condition 
				 * @~Japanese
				 * @brief 不正な状態 
				 */
				kVoiceStateInvalid
			};

#ifdef __psp2__
			/*!
			 * @~English
			 * @brief Structure used to set an envelope 
			 * @details For details on envelops, refer to Chapter 6 "Amplitude Envelope DSP Effect Module Overview" in the "NGS Modules Overview" document. 
			 * 
			 * @~Japanese
			 * @brief エンベロープの設定に使用する構造体 
			 * @details エンベロープの詳細は、「NGSモジュール 概要」の第6章「振幅エンベロープDSPエフェクトモジュールの概要」を参照してください。 
			 * 
			 */
			struct SCE_SAMPLE_UTIL_API Envelope
			{
				/*!
				 * @~English
				 * @brief Structure required for initialization of the NGS envelope DSP effect module 
				 * @~Japanese
				 * @brief NGSエンベロープDSPエフェクトモジュールの初期化に必要となる構造体 
				 */
				SceNgsEnvelopePoint		envelopePoints[SCE_NGS_ENVELOPE_MAX_POINTS];

				/*!
				 * @~English
				 * @brief Release rate (msec) 
				 * @~Japanese
				 * @brief リリースレート（ミリ秒） 
				 */
				uint32_t				releaseMsecs;

				/*!
				 * @~English
				 * @brief Number of points in the envelope (1-4) 
				 * @~Japanese
				 * @brief エンベロープのポイント数（1～4） 
				 */
				uint32_t				numPoints;

				/*!
				 * @~English
				 * @brief Loop start number of the envelope (0-2) 
				 * @~Japanese
				 * @brief エンベロープのループ先頭番号（0～2） 
				 */
				uint32_t				loopStart;

				/*!
				 * @~English
				 * @brief Loop end number of the envelope (1-3, must be larger than loopStart) 
				 * @~Japanese
				 * @brief エンベロープのループ終了番号（1～3でloopStartより大きい） 
				 */
				int32_t					loopEnd;
			};
#endif

			/*!
			 * @~English
			 * @brief Structure used to set a volume value 
			 * @details When 1.0f or 0.5f is specified, the volume is adjusted to the same volume as the original volume or half the original volume respectively. 
			 * @~Japanese
			 * @brief ボリューム値の設定に使用する構造体 
			 * @details 1.0fを指定するとオリジナル音量と同じ音量となり、0.5fを指定するとオリジナル音量の半分の音量となります。 
			 */
			struct SCE_SAMPLE_UTIL_API Volume
			{
				/*!
				 * @~English
				 * @brief Volume value for the left channel 
				 * @~Japanese
				 * @brief 左チャンネルのボリューム値 
				 */
				float					left;

				/*!
				 * @~English
				 * @brief Volume value for the right channel 
				 * @~Japanese
				 * @brief 右チャンネルのボリューム値 
				 */
				float					right;
			};

			/*!
			 * @~English
			 * @brief Class to provide the voice playing back function 
			 * @details This is the class for an application to easily play sound. It is necessary to initialize the AudioContext class before executing initialize for the Voice class. 
			 * @~Japanese
			 * @brief ボイス再生機能を提供するクラス 
			 * @details アプリケーションが簡便にサウンド再生を行うためのクラスです。Voiceクラスのinitializeを行う前に、AudioContextクラスの初期化を行う必要があります。 
			 */
			class SCE_SAMPLE_UTIL_API Voice : public Resource
			{
			public:
                virtual ~Voice(void){}

				/*!
				 * @~English
				 * @brief Plays a voice. 
				 * @param loopCount Loop count
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Invalid initialization parameter (VoiceInitParam)
				 * @retval <SCE_OK Failure
				 * @details This plays voice. 
				 * @~Japanese
				 * @brief ボイスの再生 
				 * @param loopCount ループ回数
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM 初期化パラメータ（VoiceInitParam）不正
				 * @retval <SCE_OK 失敗
				 * @details ボイスを再生します。 
				 */
				virtual int play( int loopCount=0 ) = 0;

				/*!
				 * @~English
				 * @brief Key-off of a voice. 
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Invalid initialization parameter (VoiceInitParam)
				 * @retval <SCE_OK Failure
				 * @details Performs a key-off of a voice. 
				 * @~Japanese
				 * @brief ボイスのキーオフ 
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM 初期化パラメータ（VoiceInitParam）不正
				 * @retval <SCE_OK 失敗
				 * @details ボイスのキーオフを行います。 
				 */
				virtual int keyOff(void) = 0;
				/*!
				 * @~English
				 * @brief Stop voice playback 
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Invalid initialization parameter (VoiceInitParam)
				 * @retval <SCE_OK Failure
				 * @details This stops voice play. 
				 * @~Japanese
				 * @brief ボイス再生の停止 
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM 初期化パラメータ（VoiceInitParam）不正
				 * @retval <SCE_OK 失敗
				 * @details ボイスの再生を停止します。 
				 */
				virtual int kill(void) = 0;

				/*!
				 * @~English
				 * @brief Pauses the voice being played. 
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Invalid initialization parameter (VoiceInitParam)
				 * @retval <SCE_OK Failure
				 * @details Pauses the voice being played. 
				 * @~Japanese
				 * @brief ボイス再生のポーズ 
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM 初期化パラメータ（VoiceInitParam）不正
				 * @retval <SCE_OK 失敗
				 * @details ボイス再生のポーズを行います。 
				 */
				virtual int pause(void) = 0;

				/*!
				 * @~English
				 * @brief Resumes voice playback. 
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Invalid initialization parameter (VoiceInitParam)
				 * @retval <SCE_OK Failure
				 * @details Restarts playing the voice paused. 
				 * @~Japanese
				 * @brief ボイスの再生再開 
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM 初期化パラメータ（VoiceInitParam）不正
				 * @retval <SCE_OK 失敗
				 * @details ポーズ状態のボイスを再生再開します。 
				 */
				virtual int resume(void) = 0;

#ifdef __psp2__
				/*!
				 * @~English
				 * @brief Sets the voice envelope information. 
				 * @param envelopeInfo Pointer to the envelop information structure
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @retval <SCE_OK Failure
				 * @details Sets the voice envelope information. 
				 * @~Japanese
				 * @brief ボイスのエンベロープ情報を設定 
				 * @param envelopeInfo エンベロープ情報構造体へのポインタ
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @retval <SCE_OK 失敗
				 * @details ボイスのエンベロープ情報を設定します。 
				 */
				virtual int setEnvelope( const Envelope* envelopeInfo ) = 0;
#endif /* __psp2__ */
				/*!
				 * @~English
				 * @brief Connects a voice and a buss. 
				 * @param outIndex The voice output index
				 * @param buss The connecting buss
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @retval SCE_SAMPLE_UTIL_ERROR_FATAL Buss connection failure
				 * @retval <SCE_OK Failure
				 * @details Connects the output destination of a voice with a buss. 
				 * @~Japanese
				 * @brief ボイスとバスの接続 
				 * @param outIndex ボイスの出力インデックス
				 * @param buss 接続先のバス
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @retval SCE_SAMPLE_UTIL_ERROR_FATAL バス接続失敗
				 * @retval <SCE_OK 失敗
				 * @details ボイスの出力先とバスを接続します。 
				 */
				virtual int setOutput(uint32_t outIndex, Buss *buss) = 0;

				/*!
				 * @~English
				 * @brief Gets the state of a voice. 
				 * 
				 * @retval >=SCE_OK The state of the voice (Normal termination)
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Invalid initialization parameter (VoiceInitParam)
				 * @retval <SCE_OK Failure
				 * @details Gets the state of a voice (VoiceState). 
				 * @~Japanese
				 * @brief ボイスの状態を取得 
				 * 
				 * @retval >=SCE_OK ボイスの状態（正常終了）
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM 初期化パラメータ（VoiceInitParam）不正
				 * @retval <SCE_OK 失敗
				 * @details ボイスの状態（VoiceState）を取得します。 
				 */
				virtual VoiceState getState(void) const = 0;

#if _SCE_TARGET_OS_ORBIS || _SCE_TARGET_OS_PSP2
				/*!
				 * @~English
				 * @brief Volume settings 
				 * 
				 * @param outIndex The output destination of a voice
				 * @param volume Volume
				 * @retval >=SCE_OK The state of the voice (Normal termination)
				 * @retval <SCE_OK Failure
				 * @details This sets the volume. 
				 * @~Japanese
				 * @brief 音量の設定 
				 * 
				 * @param outIndex ボイスの出力先
				 * @param volume ボリューム
				 * @retval >=SCE_OK ボイスの状態（正常終了）
				 * @retval <SCE_OK 失敗
				 * @details 音量を設定します。 
				 */
				virtual int setVolume(uint32_t outIndex, const Volume *volume) = 0;
#endif /* __psp2__ */


#if _SCE_TARGET_OS_ORBIS 
				/*!
				 * @~English
				 * @brief Add waveform data 
				 * @details This function adds waveform data to a voice. To use this function, voice must be generated with AudioContext::createRawVoice(). 
				 * @param data Waveform data
				 * @param dataSize Size of waveform data
				 * @retval >=SCE_OK Success. Normal termination
				 * @retval <SCE_OK Failure
				 * @details This sets the volume. 
				 * @~Japanese
				 * @brief 波形データの追加 
				 * @details ボイスに波形データを追加します。この関数を使用するにはボイスをAudioContext::createRawVoice()で生成しておく必要があります。 
				 * @param data 波形データ
				 * @param dataSize 波形データのサイズ
				 * @retval >=SCE_OK 成功。正常終了
				 * @retval <SCE_OK 失敗
				 * @details 音量を設定します。 
				 */
				virtual int addData(const void* data, uint32_t dataSize) = 0;
#endif
			};

		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_AUDIO_VOICE_H */


