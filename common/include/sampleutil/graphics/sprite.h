/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_SPRITE_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_SPRITE_H

#include <scebase_common.h>

#include <vectormath.h>
#include <sampleutil/graphics/context.h>
#include <sampleutil/graphics/effect.h>
#include <sampleutil/graphics/font.h>



namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			/*!
			 * @~English
			 * @brief 2D sprite rendering class 
			 * @details This is a 2D sprite rendering class. 
			 * @~Japanese
			 * @brief 2Dスプライト描画クラス 
			 * @details 2Dスプライトの描画クラスです。 
			 */
			class SCE_SAMPLE_UTIL_API SpriteRenderer : public Resource
			{
			public:
				/*!
				 * @~English
				 * @brief Sets the size of rendering target 
				 * @param renderTargetSize Rendering target size
				 * @details This sets the size of a rendering target. If drawString is to be used, make sure to execute setRenderTargetSize in advance. 
				 * @~Japanese
				 * @brief レンダリングターゲットのサイズを設定する 
				 * @param renderTargetSize レンダリングターゲットサイズ
				 * @details レンダリングターゲットのサイズ設定を行います。drawStringを使用する場合は、必ず先にsetRenderTargetSizeを行ってください。 
				 */
				virtual void setRenderTargetSize( sce::Vectormath::Simd::Aos::Vector2_arg renderTargetSize ) = 0;

				/*!
				 * @~English
				 * @brief Draws a square 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of a square (Specifies the width and height)
				 * @param rgba Color of a square (Specifies RGBA)
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws a square. The square is filled with the specified color. 
				 * @~Japanese
				 * @brief 四角形の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size 四角形のサイズ（幅・高さを指定）
				 * @param rgba 四角形の色（RGBAを指定）
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details 四角形を描画します。四角形は指定された色により塗りつぶされます。 
				 */
				virtual int fillRect(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f), 
					float depth=0.0f) = 0;

				/*!
				 * @~English
				 * @brief Draws a square 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of a square (Specifies the width and height)
				 * @param rgba Color of a square (Specifies RGBA)
				 * @param depth Depth value
				 * @param width Line width
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws a square. 
				 * @~Japanese
				 * @brief 四角形の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size 四角形のサイズ（幅・高さを指定）
				 * @param rgba 四角形の色（RGBAを指定）
				 * @param depth デプス値
				 * @param width 線分の幅
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details 四角形を描画します。 
				 */
				virtual int drawRect(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f), 
					float depth=0.0f,
					uint32_t width = 8) = 0;

				/*!
				 * @~English
				 * @brief Draws an ellipse. 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of an ellipse (Specifies the width and height)
				 * @param rgba Color of an ellipse (Specifies RGBA)
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws an ellipse. The ellipse is filled with the specified color. 
				 * @~Japanese
				 * @brief 楕円の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size 楕円のサイズ（幅・高さを指定）
				 * @param rgba 楕円の色（RGBAを指定）
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details 楕円を描画します。楕円は指定された色により塗りつぶされます。 
				 */
				virtual int fillOval(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f) = 0;

				/*!
				 * @~English
				 * @brief Draws an ellipse. 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of an ellipse (Specifies the width and height)
				 * @param rgba Color of an ellipse (Specifies RGBA)
				 * @param depth Depth value
				 * @param width Line width
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws an ellipse. 
				 * @~Japanese
				 * @brief 楕円の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size 楕円のサイズ（幅・高さを指定）
				 * @param rgba 楕円の色（RGBAを指定）
				 * @param depth デプス値
				 * @param width 線分の幅
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details 楕円を描画します。 
				 */
				virtual int drawOval(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f,
					uint32_t width = 1) = 0;

				/*!
				 * @~English
				 * @brief Draws a line segment. 
				 * @param context Pointer to GraphicsContext
				 * @param begin Line segment beginning position
				 * @param end Line segment ending position
				 * @param rgba Color of a line segment (Specifies RGBA)
				 * @param depth Depth value
				 * @param width Line width
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws a line segment. 
				 * @~Japanese
				 * @brief 線分の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param begin 線分の開始位置
				 * @param end 線分の終了位置
				 * @param rgba 線分の色（RGBAを指定）
				 * @param depth デプス値
				 * @param width 線分の幅
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details 線分を描画します。 
				 */
				virtual int drawLine(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	begin,
					sce::Vectormath::Simd::Aos::Vector2_arg	end,
					sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f,
					uint32_t width = 1) = 0;


				/*!
				 * @~English
				 * @brief Draws a texture 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of the render within the render target (specify width and height)
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param texture Pointer to Texture
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws a texture. 
				 * @~Japanese
				 * @brief テクスチャの描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size レンダーターゲット内の描画のサイズ（幅・高さを指定）
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param texture Textureへのポインタ
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details テクスチャを描画します。 
				 */
				virtual int drawTexture(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					Texture *texture,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f) = 0;

				/*!
				 * @~English
				 * @brief Draws a texture 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of the render within the render target (specify width and height)
				 * @param textureOffset Offset within the texture
				 * @param sizeInTexture Size in the texture
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param texture Pointer to Texture
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws a texture. 
				 * @~Japanese
				 * @brief テクスチャの描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size レンダーターゲット内の描画のサイズ（幅・高さを指定）
				 * @param textureOffset テクスチャ内のオフセット
				 * @param sizeInTexture テクスチャ内のサイズ
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param texture Textureへのポインタ
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details テクスチャを描画します。 
				 */
				virtual int drawTexture(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
					sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
					Texture *texture, 
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f
					) = 0;


				/*!
				 * @~English
				 * @brief Draws a texture in YUY2 format 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of the render within the render target (specify width and height)
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param texture Pointer to Texture
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details The RGBA elements of the texture are considered YUY2 format YUYV and are converted to RGB. 1.0f will be set for A. 
				 * @~Japanese
				 * @brief YUY2フォーマットのテクスチャを描画する 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size レンダーターゲット内の描画のサイズ（幅・高さを指定）
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param texture Textureへのポインタ
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details テクスチャのRGBAの要素を、YUY2フォーマットのYUYVとみなし、 RGBに変換して描画します。Aには1.0fが設定されます。 
				 */
				virtual int drawTextureYuy2(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					Texture *texture,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f) = 0;
				/*!
				 * @~English
				 * @brief Draws a texture in YUY2 format 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of the render within the render target (specify width and height)
				 * @param textureOffset Offset within the texture
				 * @param sizeInTexture Size in the texture
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param texture Pointer to Texture
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details The RGBA elements of the texture are considered YUY2 format YUYV and are converted to RGB. 1.0f will be set for A. 
				 * @~Japanese
				 * @brief YUY2フォーマットのテクスチャを描画する 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size レンダーターゲット内の描画のサイズ（幅・高さを指定）
				 * @param textureOffset テクスチャ内のオフセット
				 * @param sizeInTexture テクスチャ内のサイズ
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param texture Textureへのポインタ
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details テクスチャのRGBAの要素を、YUY2フォーマットのYUYVとみなし、 RGBに変換して描画します。Aには1.0fが設定されます。 
				 */
				virtual int drawTextureYuy2(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
					sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
					Texture *texture, 
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f
					) = 0;


#if _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief Draws a texture in YUY2 format 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of the render within the render target (specify width and height)
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param yTexture Pointer to the Texture containing the Y value
				 * @param cbcrTexture Pointer to the Texture containing the Cb value and Cr value
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details Evaluates the R element of the first texture as Y, the RG element of the second texture as CbCr, and renders by converting them to the RBG format within the shader.  1.0f will be set for A. 
				 * @~Japanese
				 * @brief YUY2フォーマットのテクスチャを描画する 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size レンダーターゲット内の描画のサイズ（幅・高さを指定）
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param yTexture Y値を含むTextureへのポインタ
				 * @param cbcrTexture Cb値Cr値を含むTextureへのポインタ
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details 最初のテクスチャのRの要素をYとみなし、２番めのテクスチャのRG要素をCbCrとみなし、シェーダー内でRGBフォーマットに変換して描画します。 Aには1.0fが設定されます。 
				 */
				virtual int drawTextureYcbcr(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					Texture *yTexture,
					Texture *cbcrTexture,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f) = 0;

				/*!
				 * @~English
				 * @brief Draws a texture in YUY2 format 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param size Size of the render within the render target (specify width and height)
				 * @param textureOffset Offset within the texture
				 * @param sizeInTexture Size in the texture
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param yTexture Pointer to the Texture containing the Y value
				 * @param cbcrTexture Pointer to the Texture containing the Cb value and Cr value
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details Evaluates the R element of the first texture as Y, the RG element of the second texture as CbCr, and renders by converting them to the RBG format within the shader.  1.0f will be set for A. 
				 * @~Japanese
				 * @brief YUY2フォーマットのテクスチャを描画する 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param size レンダーターゲット内の描画のサイズ（幅・高さを指定）
				 * @param textureOffset テクスチャ内のオフセット
				 * @param sizeInTexture テクスチャ内のサイズ
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param yTexture Y値を含むTextureへのポインタ
				 * @param cbcrTexture Cb値Cr値を含むTextureへのポインタ
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details 最初のテクスチャのRの要素をYとみなし、２番めのテクスチャのRG要素をCbCrとみなし、シェーダー内でRGBフォーマットに変換して描画します。 Aには1.0fが設定されます。 
				 */
				virtual int drawTextureYcbcr(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg	position,
					sce::Vectormath::Simd::Aos::Vector2_arg	size,
					sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
					sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
					Texture *yTexture,
					Texture *cbcrTexture,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f
					) = 0;
#endif // _SCE_TARGET_OS_ORBIS


				/*!
				 * @~English
				 * @brief Draws a dot 
				 * @param context Pointer to GraphicsContext
				 * @param psize Point size
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param texture Pointer to Texture
				 * @param vertices Pointer to VertexBuffer
				 * @param indices Pointer to IndexBuffer
				 * @param numIndices Number of indices
				 * @param depth Depth value
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws a dot. 
				 * @~Japanese
				 * @brief 点の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param psize 点の大きさ
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param texture Textureへのポインタ
				 * @param vertices VertexBufferへのポインタ
				 * @param indices IndexBufferへのポインタ
				 * @param numIndices indicesの数
				 * @param depth デプス値
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details 点を描画します。 
				 */
				virtual int drawPoints(
					GraphicsContext *context,
					float psize,
					Texture *texture,
					VertexBuffer *vertices,
					IndexBuffer *indices, 
					uint32_t numIndices,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
					float depth=0.0f
					) = 0;

				/*!
				 * @~English
				 * @brief Draws a character texture 
				 * @param context Pointer to GraphicsContext
				 * @param font Pointer to Font
				 * @param ucs2Charcode Pointer to a UCS2 character code
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param depth Depth value
				 * @param scale Scale (enlargement factor)
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This creates a character texture by pointer specification of a UCS2 character code and draws it. It is possible to specify multiple characters. Make sure to always set the termination of a UCS2 character code to NULL. 
				 * @details *When drawString is used, it is necessary to execute setRenderTargetSize after initializing SpriteRenderer. 
				 * @~Japanese
				 * @brief 文字テクスチャの描画 
				 * @param context GraphicsContextへのポインタ
				 * @param font Fontへのポインタ
				 * @param ucs2Charcode UCS2文字コードへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param depth デプス値
				 * @param scale 倍率
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details UCS2文字コードへのポインタ指定により、文字テクスチャを作成して描画します。複数文字の指定が可能です。UCS2文字コードの終端は、必ずNULLとしてください。 
				 * @details ※drawString使用時は、SpriteRendererの初期化後にsetRenderTargetSizeを行う必要があります。 
				 */
				virtual int drawString(
					GraphicsContext *context,
					Font *font,
					const uint16_t* ucs2Charcode,
					sce::Vectormath::Simd::Aos::Vector2_arg position,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
					float depth=0.0f,
					float scale=1.0f
					) = 0;
#if _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief Draws a character texture 
				 * @param context Pointer to GraphicsContext
				 * @param font Pointer to Font
				 * @param ucs2Charcode Pointer to a UCS2 character code
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param depth Depth value
				 * @param scale Scale (enlargement factor)
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This creates a character texture by pointer specification of a UCS2 character code and draws it. It is possible to specify multiple characters. Make sure to always set the termination of a UCS2 character code to NULL. 
				 * @details *When drawString is used, it is necessary to execute setRenderTargetSize after initializing SpriteRenderer. 
				 * @~Japanese
				 * @brief 文字テクスチャの描画 
				 * @param context GraphicsContextへのポインタ
				 * @param font Fontへのポインタ
				 * @param ucs2Charcode UCS2文字コードへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param depth デプス値
				 * @param scale 倍率
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details UCS2文字コードへのポインタ指定により、文字テクスチャを作成して描画します。複数文字の指定が可能です。UCS2文字コードの終端は、必ずNULLとしてください。 
				 * @details ※drawString使用時は、SpriteRendererの初期化後にsetRenderTargetSizeを行う必要があります。 
				 */
				virtual int drawString(
					GraphicsContext *context,
					Font *font,
					const char16_t* ucs2Charcode,
					sce::Vectormath::Simd::Aos::Vector2_arg position,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
					float depth=0.0f,
					float scale=1.0f
					) = 0;
#endif

				/*!
				 * @~English
				 * @brief Gets the size of a character texture. 
				 * @param font Pointer to Font
				 * @param ucs2Charcode Pointer to a UCS2 character code
				 * @param scale Scale (enlargement factor)
				 * 
				 * @retval sce::Vectormath::Simd::Aos::Vector2 Character texture size
				 * @details This returns the size of a character texture created by pointer specification of a UCS2 character code. It is possible to specify multiple characters. Make sure to always set the termination of a UCS2 character code to NULL. 
				 * @~Japanese
				 * @brief 文字テクスチャのサイズ取得 
				 * @param font Fontへのポインタ
				 * @param ucs2Charcode UCS2文字コードへのポインタ
				 * @param scale 倍率
				 * 
				 * @retval sce::Vectormath::Simd::Aos::Vector2 文字テクスチャのサイズ
				 * @details UCS2文字コードへのポインタ指定により、作成される文字テクスチャのサイズを返します。複数文字の指定が可能です。UCS2文字コードの終端は、必ずNULLとしてください。 
				 */
				virtual sce::Vectormath::Simd::Aos::Vector2 getStringTextureSize(
					Font *font,
					const uint16_t* ucs2Charcode,
					float scale=1.0f
					) = 0;

#if _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief Gets the height of alphanumeric characters.
				 * @param font Pointer to Font
				 * @param scale Scale (enlargement factor)
				 * 
				 * @retval float Height of alphanumeric characters
				 * @details This returns the height of alphanumeric characters in texture created with the specified font.
				 * @~Japanese
				 * @brief 英数文字の高さの取得
				 * @param font Fontへのポインタ
				 * @param scale 倍率
				 * 
				 * @retval float 英数文字の高さ
				 * @details 指定したフォントにおいて作成される文字テクスチャの英数文字の高さを返します。
				 */
				virtual float getAlphanumericCharactersHeight(
					Font *font,
					float scale=1.0f
					) = 0;

				/*!
				 * @~English
				 * @brief Gets the baseline offset value of alphanumeric characters.
				 * @param font Pointer to Font
				 * @param scale Scale (enlargement factor)
				 * 
				 * @retval float Baseline offset value of alphanumeric characters
				 * @details This returns the offset value from the top edge of the design as the the baseline position of alphanumeric characters in texture created with the specified font.
				 * @~Japanese
				 * @brief 英数文字のベースラインオフセット値の取得
				 * @param font Fontへのポインタ
				 * @param scale 倍率
				 * 
				 * @retval float 英数文字のベースラインオフセット値
				 * @details 指定したフォントにおいて作成される文字テクスチャの英数文字のベースライン位置として、デザイン上端からのオフセット値を返します。
				 */
				virtual float getAlphanumericCharactersBaselineOffset(
					Font *font,
					float scale=1.0f
					) = 0;
#endif

				/*!
				 * @~English
				 * @brief Drawing a debug character string 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param fontHeight Height of the font drawn
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param backgroundColor Background color (specifies RGBA)
				 * @param depth Depth value
				 * @param string Character string to render
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details Draws a C Language style character string (Char array terminated with NULL) on the screen. 
				 * @~Japanese
				 * @brief デバッグ文字列の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param fontHeight 描画するフォントの高さ
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param backgroundColor 背景色（RGBAを指定）
				 * @param depth デプス値
				 * @param string 描画する文字列
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details C言語スタイルの文字列(NULL終端のChar配列)を画面上に描画します 
				 */
				virtual int drawDebugString(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg position,
					float fontHeight,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
					sce::Vectormath::Simd::Aos::Vector4_arg backgroundColor,
					float depth,
					const char *string) = 0;

				/*!
				 * @~English
				 * @brief Drawing a debug character string 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param fontHeight Height of the font drawn
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param string Character string to render
				 * 
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details Draws a C Language style character string (Char array terminated with NULL) on the screen. 
				 * @~Japanese
				 * @brief デバッグ文字列の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param fontHeight 描画するフォントの高さ
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param string 描画する文字列
				 * 
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details C言語スタイルの文字列(NULL終端のChar配列)を画面上に描画します 
				 */
				virtual int drawDebugString(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg position,
					float fontHeight,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
					const char *string) = 0;

				/*!
				 * @~English
				 * @brief Drawing a debug character string 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param fontHeight Height of the font drawn
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param backgroundColor Background color (specifies RGBA)
				 * @param depth Depth value (set 1 to match other debugString functions)
				 * @param format Character string designated in the C Language style (Char array terminated with NULL)
				 * @param ... Variable arguments
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws a character string designated in the C Language style (Char array terminated with NULL) on the screen. 
				 * @~Japanese
				 * @brief デバッグ文字列の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param fontHeight 描画するフォントの高さ
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param backgroundColor 背景色（RGBAを指定）
				 * @param depth デプス値(他のdebugString系関数と整合を取るには、1を設定してください)
				 * @param format C言語スタイルの書式指定された文字列(NULL終端のChar配列)
				 * @param ... 可変個引数
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details C言語スタイルの書式指定された文字列(NULL終端のChar配列)を画面上に描画します 
				 */
				virtual int drawDebugStringf(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg position,
					float fontHeight,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
					sce::Vectormath::Simd::Aos::Vector4_arg backgroundColor,
					float depth,
					const char *format, ...) = 0;

				/*!
				 * @~English
				 * @brief Drawing a debug character string 
				 * @param context Pointer to GraphicsContext
				 * @param position Position of drawing (Specifies the upper left coordinate)
				 * @param fontHeight Height of the font drawn
				 * @param colorCoeff Color coefficient of a texture (Specifies RGBA)
				 * @param format Character string designated in the C Language style (Char array terminated with NULL)
				 * @param ... Variable arguments
				 * @retval SCE_OK Success
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
				 * @details This draws a character string designated in the C Language style (Char array terminated with NULL) on the screen. 
				 * @~Japanese
				 * @brief デバッグ文字列の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param position 描画する位置（左上の座標を指定）
				 * @param fontHeight 描画するフォントの高さ
				 * @param colorCoeff テクスチャの色係数（RGBAを指定）
				 * @param format C言語スタイルの書式指定された文字列(NULL終端のChar配列)
				 * @param ... 可変個引数
				 * @retval SCE_OK 成功
				 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
				 * @details C言語スタイルの書式指定された文字列(NULL終端のChar配列)を画面上に描画します 
				 */
				virtual int drawDebugStringf(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Vector2_arg position,
					float fontHeight,
					sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
					const char *format, ...) = 0;

				/*!
				 * @~English
				 * @brief Gets width of a debug character. 
				 * @param charHeight Height of a debug character
				 * 
				 * @retval float Width of a debug character
				 * @details Returns character width for the designated height of specified characters in the current render target. 
				 * @~Japanese
				 * @brief デバッグ文字の幅を取得 
				 * @param charHeight デバッグ文字の高さ
				 * 
				 * @retval float デバッグ文字の幅
				 * @details 現在のレンダーターゲット内での、指定された文字の高さに対する文字幅を返します 
				 */
				virtual float getWidthOfDebugChar(float charHeight) = 0;
			};

			/*!
			 * @~English
			 * @brief Create SpriteRenderer 
			 * @param outSpriteRenderer Pointer to which the generated SpriteRenderer returns
			 * @param loader GraphicsLoader instance
			 * 
			 * @retval SCE_OK Success
			 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM Parameter invalid
			 * @details This generates the SpriteRenderer. To delete the generated SpriteRenderer, use sce::SampleUtil::destroy(). 
			 * @~Japanese
			 * @brief SpriteRendererの生成 
			 * @param outSpriteRenderer 生成されたSpriteRendererが返るポインタ。
			 * @param loader GraphicsLoaderのインスタンス
			 * 
			 * @retval SCE_OK 成功
			 * @retval SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM パラメータ不正
			 * @details SpriteRendererを生成します。生成されたSpriteRendererを削除する場合は sce::SampleUtil::destroy()を使用してください。 
			 */
			SCE_SAMPLE_UTIL_API int createSpriteRenderer(SpriteRenderer** outSpriteRenderer, GraphicsLoader *loader);


			/*!
			 * @~English
			 * @brief 3D object renderer class 
			 * @details This is a class for rendering simple 3D objects. 
			 * @~Japanese
			 * @brief 3Dオブジェクト描画クラス 
			 * @details 簡単な3Dオブジェクトを描画するためのクラスです。 
			 */
			class SCE_SAMPLE_UTIL_API Object3dRenderer : public Resource
			{
			public:
				/*!
				 * @~English
				 * @brief Cube render 
				 * @param context Pointer to GraphicsContext
				 * @param world Transformation matrix for world coordinate system
				 * @param view View matrix
				 * @param projection Projection matrix
				 * @param lightPosition Point light position
				 * @param color Color
				 * @param ambient Ambient value for phone shader
				 * @param shininess Shininess of phone shader
				 * 
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @details Renders a filled cube in a 3D space. The original cube will have the center as the starting point, the length of each side will be 1, and all sides will be parallel to the coordinate axes. 
				 * @~Japanese
				 * @brief 立方体の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param world world座標系への変換行列
				 * @param view 視点行列
				 * @param projection 射影行列
				 * @param lightPosition ポイントライトの位置
				 * @param color 色
				 * @param ambient phoneシェーダのアンビエント値
				 * @param shininess phoneシェーダの輝度
				 * 
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 * @details 3D空間に塗りつぶされた立方体を描画します。元の立方体は中心が原点、各辺の長さが１、全ての辺が座標軸に平行な立方体です。 
				 */
				virtual int fillCube(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Matrix4_arg world,
					sce::Vectormath::Simd::Aos::Matrix4_arg view,
					sce::Vectormath::Simd::Aos::Matrix4_arg projection,
					sce::Vectormath::Simd::Aos::Vector3 lightPosition,
					sce::Vectormath::Simd::Aos::Vector4 color,
					float ambient,
					float shininess) = 0;

				/*!
				 * @~English
				 * @brief Cube render 
				 * @param context Pointer to GraphicsContext
				 * @param world Transformation matrix for world coordinate system
				 * @param view View matrix
				 * @param projection Projection matrix
				 * @param color Color
				 * @param lineWidth Line width
				 * 
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @details Renders a wire frame cube in a 3D space. The original cube will have the center as the starting point, the length of each side will be 1, and all sides will be parallel to the coordinate axes. 
				 * @~Japanese
				 * @brief 立方体の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param world world座標系への変換行列
				 * @param view 視点行列
				 * @param projection 射影行列
				 * @param color 色
				 * @param lineWidth 線の太さ
				 * 
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 * @details 3D空間にワイヤフレームの立方体を描画します。元の立方体は中心が原点、各辺の長さが１、全ての辺が座標軸に平行な立方体です。 
				 */
				virtual int drawCube(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Matrix4_arg world,
					sce::Vectormath::Simd::Aos::Matrix4_arg view,
					sce::Vectormath::Simd::Aos::Matrix4_arg projection,
					sce::Vectormath::Simd::Aos::Vector4 color,
					uint32_t lineWidth) = 0;

				/*!
				 * @~English
				 * @brief Draws a line segment. 
				 * @param context Pointer to GraphicsContext
				 * @param view View matrix
				 * @param projection Projection matrix
				 * @param begin Line segment beginning position
				 * @param end Line segment ending position
				 * @param color Color
				 * @param lineWidth Line width
				 * 
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @details Renders a line segment in 3D space. 
				 * @~Japanese
				 * @brief 線分の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param view 視点行列
				 * @param projection 射影行列
				 * @param begin 線分の開始位置
				 * @param end 線分の終了位置
				 * @param color 色
				 * @param lineWidth 線の太さ
				 * 
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 * @details 3D空間に線分を描画します。 
				 */
				virtual int drawLine(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Matrix4_arg view,
					sce::Vectormath::Simd::Aos::Matrix4_arg projection,
					sce::Vectormath::Simd::Aos::Vector3 begin,
					sce::Vectormath::Simd::Aos::Vector3 end,
					sce::Vectormath::Simd::Aos::Vector4 color,
					uint32_t lineWidth) = 0;

				/*!
				 * @~English
				 * @brief Sphere rendering 
				 * @param context Pointer to GraphicsContext
				 * @param world Transformation matrix for world coordinate system
				 * @param view View matrix
				 * @param projection Projection matrix
				 * @param color Color
				 * @param lineWidth Line width
				 * 
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @details Renders a wire frame sphere in 3D space. The original sphere has its center at the origin with a diameter of 1. 
				 * @~Japanese
				 * @brief 球の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param world world座標系への変換行列
				 * @param view 視点行列
				 * @param projection 射影行列
				 * @param color 色
				 * @param lineWidth 線の太さ
				 * 
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 * @details 3D空間にワイヤフレームの球を描画します。元の球は中心が原点、直径が１の球です。 
				 */
				virtual int drawSphere(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Matrix4_arg world,
					sce::Vectormath::Simd::Aos::Matrix4_arg view,
					sce::Vectormath::Simd::Aos::Matrix4_arg projection,
					sce::Vectormath::Simd::Aos::Vector4 color,
					uint32_t lineWidth) = 0;


				/*!
				 * @~English
				 * @brief Sphere rendering 
				 * @param context Pointer to GraphicsContext
				 * @param world Transformation matrix for world coordinate system
				 * @param view View matrix
				 * @param projection Projection matrix
				 * @param lightPosition Point light position
				 * @param color Color
				 * @param ambient Ambient value for phone shader
				 * @param shininess Shininess of phone shader
				 * 
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @details Renders a colored-in sphere in 3D space. The original sphere has its center at the origin with a radius of 0.5. 
				 * @~Japanese
				 * @brief 球の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param world world座標系への変換行列
				 * @param view 視点行列
				 * @param projection 射影行列
				 * @param lightPosition ポイントライトの位置
				 * @param color 色
				 * @param ambient phoneシェーダのアンビエント値
				 * @param shininess phoneシェーダの輝度
				 * 
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 * @details 3D空間に塗りつぶされた球を描画します。元の球は中心が原点、半径0.5です。 
				 */
				virtual int fillSphere(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Matrix4_arg world,
					sce::Vectormath::Simd::Aos::Matrix4_arg view,
					sce::Vectormath::Simd::Aos::Matrix4_arg projection,
					sce::Vectormath::Simd::Aos::Vector3 lightPosition,
					sce::Vectormath::Simd::Aos::Vector4 color,
					float ambient,
					float shininess) = 0;

				/*!
				 * @~English
				 * @brief Cylinder rendering 
				 * @param context Pointer to GraphicsContext
				 * @param world Transformation matrix for world coordinate system
				 * @param view View matrix
				 * @param projection Projection matrix
				 * @param lightPosition Point light position
				 * @param color Color
				 * @param ambient Ambient value for phone shader
				 * @param shininess Shininess of phone shader
				 * 
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @details Renders a colored-in cylinder in 3D space. The original cylinder has its circle center at the origin with a radius of 0.5, and a height of 1.0. 
				 * @~Japanese
				 * @brief 円柱の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param world world座標系への変換行列
				 * @param view 視点行列
				 * @param projection 射影行列
				 * @param lightPosition ポイントライトの位置
				 * @param color 色
				 * @param ambient phoneシェーダのアンビエント値
				 * @param shininess phoneシェーダの輝度
				 * 
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 * @details 3D空間に塗りつぶされた円柱を描画します。元の円柱は中心が原点、円の半径が0.5、高さが1.0です。 
				 */
				virtual int fillCylinder(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Matrix4_arg world,
					sce::Vectormath::Simd::Aos::Matrix4_arg view,
					sce::Vectormath::Simd::Aos::Matrix4_arg projection,
					sce::Vectormath::Simd::Aos::Vector3 lightPosition,
					sce::Vectormath::Simd::Aos::Vector4 color,
					float ambient,
					float shininess) = 0;


				/*!
				 * @~English
				 * @brief Cylinder rendering 
				 * @param context Pointer to GraphicsContext
				 * @param world Transformation matrix for world coordinate system
				 * @param view View matrix
				 * @param projection Projection matrix
				 * @param color Color
				 * @param lineWidth Line width
				 * 
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @details Renders a wire frame cylinder in 3D space. The original cylinder has its circle center at the origin with a radius of 0.5, and a height of 1.0. 
				 * @~Japanese
				 * @brief 円柱の描画 
				 * @param context GraphicsContextへのポインタ
				 * @param world world座標系への変換行列
				 * @param view 視点行列
				 * @param projection 射影行列
				 * @param color 色
				 * @param lineWidth 線の太さ
				 * 
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 * @details 3D空間にワイヤフレームの円柱を描画します。元の円柱は中心が原点、円の半径が0.5、高さが1.0です。 
				 */
				virtual int drawCylinder(
					GraphicsContext *context,
					sce::Vectormath::Simd::Aos::Matrix4_arg world,
					sce::Vectormath::Simd::Aos::Matrix4_arg view,
					sce::Vectormath::Simd::Aos::Matrix4_arg projection,
					sce::Vectormath::Simd::Aos::Vector4 color,
					uint32_t lineWidth) = 0;
			};

			/*!
			 * @~English
			 * @brief Generate Object3dRenderer 
			 * @param outObject3dRenderer Pointer to which the generated Object3dRenderer returns.
			 * @param loader GraphicsLoader instance
			 * 
			 * @retval SCE_OK Success
			 * @retval <SCE_OK Error
			 * @details This generates the Object3dRenderer. To delete the generated Object3dRenderer, use sce::SampleUtil::destroy(). 
			 * @~Japanese
			 * @brief Object3dRendererを生成 
			 * @param outObject3dRenderer 生成されたObject3dRendererが返るポインタ。
			 * @param loader GraphicsLoaderのインスタンス
			 * 
			 * @retval SCE_OK 成功
			 * @retval <SCE_OK エラー
			 * @details Object3dRendererを生成します。生成されたObject3dRendererを削除する場合は sce::SampleUtil::destroy()を使用してください。 
			 */
			SCE_SAMPLE_UTIL_API int createObject3dRenderer(Object3dRenderer** outObject3dRenderer, GraphicsLoader *loader);


		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_CPP_SPRITE_H */
