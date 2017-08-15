/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_AUDIO_DATA_H
#define _SCE_SAMPLE_UTIL_AUDIO_DATA_H

#include <sampleutil/sampleutil_common.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Audio
		{
			/*!
			 * @~English
			 * @brief Parameter used for the initialization of signal generator format voice data 
			 * @details This is the parameter used for the initialization of signal generator format voice data. 
			 * @~Japanese
			 * @brief シグナルジェネレータ形式のボイスデータの初期化に使うパラメータ 
			 * @details シグナルジェネレータ形式のボイスデータの初期化に使うパラメータです。 
			 */
			struct GeneratorSettings
			{
				/*!
				 * @~English
				 * @brief This is the type of wave forms generated. Specify NGS wave forms such as SCE_NGS_GENERATOR_SINE. 
				 * @~Japanese
				 * @brief 生成する波形のタイプ。SCE_NGS_GENERATOR_SINE等のNGSの波形を指定してください。 
				 */
				uint32_t 	generatorMode;
				/*!
				 * @~English
				 * @brief Frequency response (Hz) of wave forms 
				 * @~Japanese
				 * @brief 波形の再生周波数（Hz） 
				 */
				int32_t         frequency;
				/*!
				 * @~English
				 * @brief  This is the amplitude of wave forms. 1 = complete amplitude (no clipping), 0.5 = half amplitude 
				 * @~Japanese
				 * @brief  波形の振幅。1＝完全な振幅（クリッピングなし）、0.5＝半分の振幅 
				 */
				float       amplitude;
				/*!
				 * @~English
				 * @brief Pulse width (upon pulse wave form selection) 
				 * @~Japanese
				 * @brief パルス幅（パルス波形選択時） 
				 */
				float       pulseWidth;
				/*!
				 * @~English
				 * @brief Generation start offset within wave form (for example, ability to generate sine waves as cosine waves) 
				 * @~Japanese
				 * @brief 波形内の生成開始オフセット（たとえば、正弦波を余弦波として生成可能） 
				 */
				uint32_t        sampleOffset;
				/*!
				 * @~English
				 * @brief Start phase offset at the time of  sine wave generation (designate as angle (the degree of) between 0 and 360) 
				 * @~Japanese
				 * @brief 正弦波生成時の開始位相オフセット（0～360の範囲の角度（度）として指定） 
				 */
				uint32_t       phaseAngle;
			};




			/*!
			 * @~English
			 * @brief Voice data type 
			 * @details This is the voice data type for generating the voice instance. 
			 * @~Japanese
			 * @brief ボイスデータ型 
			 * @details ボイスのインスタンスを生成するためのボイスデータ型です。 
			 */
			class SCE_SAMPLE_UTIL_API VoiceData : public Resource
			{
			public:
				virtual ~VoiceData(void){}


			};


			/*!
			 * @~English
			 * @brief Generate voice data from a file 
			 * @details This generates voice data from a file. If isStreaming is true, streaming is used to read the data from the file. If isStreaming is false, all the data in the memory is read when voice data is generated. 
			 * 
			 * @param outVoiceData Pointer to which the generated voice data returns
			 * @param path Path of a data file
			 * @param isStreaming Set whether to initialize as streaming.
			 * @retval SCE_OK Success
			 * @retval <SCE_OK Failure
			 * @~Japanese
			 * @brief ファイルからボイスデータを生成する 
			 * @details ファイルからボイスデータを生成します。isStreamingがtrueの場合、ストリーミング形式でファイルからデータを読み込みます。isStreaming が falseの場合、ボイスデータ生成時に全てメモリ上にデータをロードします。 
			 * 
			 * @param outVoiceData 生成されたボイスデータが返るポインタ
			 * @param path データファイルのパス
			 * @param isStreaming ストリーミングとして初期化するかどうかの指定
			 * @retval SCE_OK 成功
			 * @retval <SCE_OK 失敗
			 */
			SCE_SAMPLE_UTIL_API int createVoiceDataFromFile(VoiceData **outVoiceData, const char* path, bool isStreaming);

			/*!
			 * @~English
			 * @brief Generate voice data as a signal generator 
			 * @details Generates voice data as a signal generator. 
			 * 
			 * @param outVoiceData Pointer to which the generated voice data returns
			 * @param setting Setting of signal generator
			 * @retval SCE_OK Success
			 * @retval <SCE_OK Failure
			 * @~Japanese
			 * @brief シグナルジェネレータとしてボイスデータを生成する 
			 * @details シグナルジェネレータとしてボイスデータを生成します。 
			 * 
			 * @param outVoiceData 生成されたボイスデータが返るポインタ
			 * @param setting シグナルジェネレータの設定
			 * @retval SCE_OK 成功
			 * @retval <SCE_OK 失敗
			 */
			SCE_SAMPLE_UTIL_API int createVoiceDataFromGeneratorSetting(VoiceData **outVoiceData, const GeneratorSettings *setting);


		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_AUDIO_DATA_H */
