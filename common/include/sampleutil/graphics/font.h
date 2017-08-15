/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef	_SCE_SAMPLE_UTIL_CPP_FONT_H
#define	_SCE_SAMPLE_UTIL_CPP_FONT_H

#include <sampleutil/sampleutil_common.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil/graphics/buffer.h>
#include <scebase_common.h>

#ifdef __psp2__
#include <font/libpvf.h>
#endif



namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			/*!
			 * @~English
			 * @brief Photo language code 
			 * @details This is the photo language code. 
			 * @~Japanese
			 * @brief フォント言語コード 
			 * @details フォント言語コードです。 
			 */
			enum FontLanguage
			{
				/*!
				 * @~English
				 * @brief English 
				 * @~Japanese
				 * @brief 英語 
				 */
				kFontLanguageLatin,
				/*!
				 * @~English
				 * @brief Japanese 
				 * @~Japanese
				 * @brief 日本語 
				 */
				kFontLanguageJapanese,
				/*!
				 * @~English
				 * @brief Chinese 
				 * @~Japanese
				 * @brief 中国語 
				 */
				kFontLanguageChinese,
				/*!
				 * @~English
				 * @brief Korean 
				 * @~Japanese
				 * @brief 韓国語 
				 */
				kFontLanguageKorean
			};
			
			/*!
			 * @~English
			 * @brief Font weight 
			 * @details This value indicates the font thickness. 
			 * @~Japanese
			 * @brief フォントウェイト 
			 * @details フォントの太さを表す値です。 
			 */
			enum FontWeight
			{
				/*!
				 * @~English
				 * @brief Narrow 
				 * @~Japanese
				 * @brief 細い 
				 */
				kFontWeightNarrow,
				/*!
				 * @~English
				 * @brief Normal thickness 
				 * @~Japanese
				 * @brief 標準の太さ 
				 */
				kFontWeightNormal,
				/*!
				 * @~English
				 * @brief Bold 
				 * @~Japanese
				 * @brief 太い 
				 */
				kFontWeightBold,

				/*!
				 * @~English
				 * @brief Extra bold 
				 * @~Japanese
				 * @brief より太い 
				 */
				kFontWeightBlack
			};

			/*!
			 * @~English
			 * @brief Font family 
			 * @details This is the font family. It specifies the font type. 
			 * @~Japanese
			 * @brief フォントファミリ 
			 * @details フォントファミリです。フォントの書体を指定します。 
			 */
			enum FontFamily
			{
				/*!
				 * @~English
				 * @brief Sans serif 
				 * @~Japanese
				 * @brief サンセリフ 
				 */
				kFontFamilySansSerif,
				/*!
				 * @~English
				 * @brief Serif 
				 * @~Japanese
				 * @brief セリフ 
				 */
				kFontFamilySerif,
				/*!
				 * @~English
				 * @brief Rounded 
				 * @~Japanese
				 * @brief 丸文字 
				 */
				kFontFamilyRounded
			};

			/*!
			 * @~English
			 * @brief Structure used to initialize the Font class 
			 * @details This is used as the argument of initialize by initializing with initParam of the Font class. It is possible to set various operating environments associated with texture generation for the Font class. 
			 * @~Japanese
			 * @brief Fontクラスの初期化に用いる構造体 
			 * @details FontクラスのinitParamにより初期設定を行い、initializeの引数として使用します。Fontクラスに対して、テクスチャ生成に関わる各種動作環境の設定が可能です。 
			 */
			struct SCE_SAMPLE_UTIL_API FontParam
			{
				/*!
				 * @~English
				 * @brief Photo language code 
				 * @details The initial value is kFontLanguageLatin. 
				 * @~Japanese
				 * @brief フォント言語コード 
				 * @details 初期値はkFontLanguageLatinです。 
				 */
				FontLanguage				language;

				/*!
				 * @~English
				 * @brief Font family code 
				 * @details The initial value is kFontFamilySansSerif. 
				 * @~Japanese
				 * @brief フォントファミリーコード 
				 * @details 初期値はkFontFamilySansSerifです。 
				 */
				FontFamily				family;

				/*!
				 * @~English
				 * @brief Font weight 
				 * @details The initial value is kFontWeightNormal. 
				 * @~Japanese
				 * @brief フォントウェイト 
				 * @details 初期値はkFontWeightNormalです。 
				 */
				FontWeight              weight;
				/*!
				 * @~English
				 * @brief Italic or not 
				 * @details The initial value is false. 
				 * @~Japanese
				 * @brief イタリック体かどうか 
				 * @details 初期値はfalseです。 
				 */
				bool                   isItalic;

				/*!
				 * @~English
				 * @brief Horizontal font size 
				 * @details This is specified by pixel value. The initial value is 64.0. 
				 * @~Japanese
				 * @brief 横方向の文字サイズ 
				 * @details ピクセル値で指定します。初期値は64.0です。 
				 */
				float					fontSizePixelH;

				/*!
				 * @~English
				 * @brief Vertical font size 
				 * @details This is specified by pixel value. The initial value is 64.0. 
				 * @~Japanese
				 * @brief 縦方向の文字サイズ 
				 * @details ピクセル値で指定します。初期値は64.0です。 
				 */
				float					fontSizePixelV;

				/*!
				 * @~English
				 * @brief Baseline offset value 
				 * @details Specify the baseline position used when character images are lined up horizontally with the offset value (pixel) from the top edge of the design. If not specified (when the value is 0), the baseline will be automatically determined. 
				 * @~Japanese
				 * @brief ベースラインオフセット値 
				 * @details 文字イメージを横方向へ並べるときのベースライン位置を、デザイン上端からのオフセット値（ピクセル）で指定します。指定が無いとき（0のとき）は、ベースラインを自動的に決めます。 
				 */
				float					baselineOffsetY;

				FontParam(void);
				/*!
				 * @~English
				 * @brief Initialize FontParam 
				 * 
				 * @details This is the initialization function of FontParam. This sets the default value in each variable of FontParam. Change each set value after initializing FontParam with initParam. 
				 * @~Japanese
				 * @brief FontParamの初期化 
				 * 
				 * @details FontParamの初期化関数です。FontParamの各変数に初期値を設定します。initParamでFontParamを初期化した後に、各設定値の変更を行ってください。 
				 */
				void setDefaults(void);
			};


			/*!
			 * @~English
			 * @brief Class to handle the font texture 
			 * @details This generates a texture by specifying a UCS2 code. This has a cache function for the texture generated. 
			 * @~Japanese
			 * @brief フォントテクスチャを扱うためのクラス 
			 * @details UCS2コードの指定により、テクスチャを生成します。生成されたテクスチャのキャッシュ機能を備えています。 
			 */
			class SCE_SAMPLE_UTIL_API Font : public Resource
			{
			public:
				virtual ~Font(void){}

				/*!
				 * @~English
				 * @brief Cache of a character texture 
				 * @param ucs2Charcode Pointer to a UCS2 character code
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This creates a character texture by specifying the pointer to UCS2 character code and caches it. It is possible to specify multiple characters. Make sure to always set the termination of a UCS2 character code to NULL. 
				 * @~Japanese
				 * @brief 文字テクスチャのキャッシュ 
				 * @param ucs2Charcode UCS2文字コードへのポインタ
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details UCS2文字コードへのポインタ指定により文字テクスチャを作成し、キャッシュします。複数文字の指定が可能です。UCS2文字コードの終端は、必ずNULLとしてください。 
				 */
				virtual int cacheCharacters( const uint16_t* ucs2Charcode ) = 0;

				/*!
				 * @~English
				 * @brief Clears the cache. 
				 * @details This clears the cache of a character texture. 
				 * @~Japanese
				 * @brief キャッシュのクリア 
				 * @details 文字テクスチャのキャッシュをクリアします。 
				 */
				virtual void clearCache(void) = 0;

			};

			/*!
			 * @~English
			 * @brief Class that generates fonts 
			 * @details  This is the class that generates fonts. 
			 * @~Japanese
			 * @brief フォントを生成するクラス 
			 * @details  フォントを生成するクラスです。 
			 */
			class SCE_SAMPLE_UTIL_API FontLoader : public Resource
			{
			public:
				virtual ~FontLoader(void){}

				/*!
				 * @~English
				 * @brief Generate font 
				 * @param outFont Pointer to which the generated font returns
				 * @param param Pointer to FontParam. This is initialized by the default value if NULL is specified.
				 * @param numMaxCache This is the limit for the cache that can be saved in a font. The limit that can be specified varies depending on the font size.
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This generates fonts. 
				 * @~Japanese
				 * @brief フォントの生成 
				 * @param outFont 生成されたフォントが返るポインタ
				 * @param param FontParamのポインタ。NULLを指定するとデフォルトの値で初期化されます。
				 * @param numMaxCache フォント内に保存できるキャッシュの上限。フォントのサイズによって指定可能な上限が異なります。
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details フォントを生成します。 
				 */
				virtual int createFont( Font **outFont, const FontParam* param = NULL, uint32_t numMaxCache=1024) = 0;

			};

			/*!
			 * @~English
			 * @brief Create FontLoader class 
			 * @param outFontLoader Pointer to the FontLoader generated
			 * @param loader Pointer to GraphicsLoader
			 * @retval SCE_OK Success
			 * @retval <SCE_OK Failure
			 * @details This generates the FontLoader class. 
			 * @~Japanese
			 * @brief FontLoaderクラスの生成 
			 * @param outFontLoader 生成されるFontLoaderのポインタ
			 * @param loader GraphicsLoaderのポインタ
			 * @retval SCE_OK 成功
			 * @retval <SCE_OK 失敗
			 * @details FontLoaderクラスを生成します。 
			 */
			SCE_SAMPLE_UTIL_API int createFontLoader(FontLoader **outFontLoader, GraphicsLoader *loader);


		}
	}
}

#endif	/* _SCE_SAMPLE_UTIL_CPP_FONT_H */
