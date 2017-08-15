/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PLATFORM_GXM_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PLATFORM_GXM_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2


#include <common_dialog.h>
#include <sampleutil/graphics/loader.h>
#include <sampleutil/graphics/context.h>
#include <stdio.h>
#include <stdint.h>
#include <alloca.h>
#include <display.h>
#include <gxm.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Gxm
			{

				class SCE_SAMPLE_UTIL_API BufferGxm : public BufferInterface
				{
				protected:
					virtual ~BufferGxm(void){}
				public:	
					/*!
					 * @~English
					 * @brief Returns the pointer to the data area of a buffer object 
					 * @details This returns the pointer to the data area of a buffer object. 
					 * 
					 * @return Pointer to the data area of a buffer object 
					 * @~Japanese
					 * @brief バッファオブジェクトのデータ領域へのポインタを返す 
					 * @details バッファオブジェクトのデータ領域へのポインタを返します。 
					 * 
					 * @return バッファオブジェクトのデータ領域へのポインタ 
					 */
					virtual void *getData(void) = 0;

					/*!
					 * @~English
					 * @brief Returns the pointer to the data area of a buffer object 
					 * @details This returns the pointer to the data area of a buffer object. 
					 * 
					 * @return Pointer to the data area of a buffer object 
					 * @~Japanese
					 * @brief バッファオブジェクトのデータ領域へのポインタを返す 
					 * @details バッファオブジェクトのデータ領域へのポインタを返します。 
					 * 
					 * @return バッファオブジェクトのデータ領域へのポインタ 
					 */
					virtual const void *getData(void) const = 0;

					static BufferGxm* downcast(BufferInterface *buffer);
					static const BufferGxm* downcast(const BufferInterface *buffer);
				};


				/*!
				 * @~English
				 * @brief Parameter for sceCommonDialogUpdate 
				 * @details This is a parameter for sceCommonDialogUpdate. 
				 * 
				 * @~Japanese
				 * @brief sceCommonDialogUpdate 用パラメータ 
				 * @details sceCommonDialogUpdate 用パラメータです。 
				 * 
				 */
				class SCE_SAMPLE_UTIL_API CommonDialogUpdateParam : public SceCommonDialogUpdateParam
				{
				private:
					RenderTarget        *m_renderTarget;
					DepthStencilSurface *m_depthStencilSurface;

				public:
					/*!
					 * @~English
					 * @brief Constructor 
					 * @details This is a constructor. 
					 * 
					 * @~Japanese
					 * @brief コンストラクタ 
					 * @details コンストラクタです。 
					 * 
					 */
					CommonDialogUpdateParam(void)
					{
						m_renderTarget = NULL;
						m_depthStencilSurface = NULL;
						renderTarget.colorSurfaceData		= NULL;
						renderTarget.depthSurfaceData		= NULL;
					}

					/*!
					 * @~English
					 * @brief  Initializes the parameter for sceCommonDialogUpdate. 
					 * @details  This initializes the parameter for sceCommonDialogUpdate. 
					 * 
					 * @param renderTarget Pointer to a render target
					 * @param depthStencilSurface Pointer to a depth and stencil surface
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief  sceCommonDialogUpdate 用パラメータの初期化 
					 * @details  sceCommonDialogUpdate 用パラメータを初期化します。 
					 * 
					 * @param renderTarget レンダーターゲットへのポインタ
					 * @param depthStencilSurface デプス・ステンシルサーフェースへのポインタ
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					int initialize(RenderTarget        *renderTarget,
						DepthStencilSurface *depthStencilSurface);

					/*!
					 * @~English
					 * @brief  Termination processing of the parameter for sceCommonDialogUpdate 
					 * @details  This executes termination processing of the parameter for sceCommonDialogUpdate. 
					 * 
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief  sceCommonDialogUpdate 用パラメータの終了処理 
					 * @details  sceCommonDialogUpdate 用パラメータの終了処理を行います。 
					 * 
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					int finalize(void)
					{
						m_renderTarget = NULL;
						m_depthStencilSurface = NULL;
						this->renderTarget.colorSurfaceData		= NULL;
						this->renderTarget.depthSurfaceData		= NULL;
						return SCE_OK;
					}

				};


				class SCE_SAMPLE_UTIL_API TextureGxm : public Texture
				{
				protected:
					virtual ~TextureGxm(void){}
				public:	

					/*!
					 * @~English
					 * @brief Get pointer to a GXM texture 
					 * @details This gets the pointer to a GXM texture. 
					 * 
					 * @return Pointer to a GXM texture 
					 * @~Japanese
					 * @brief GXMテクスチャへのポインタを取得する 
					 * @details GXMテクスチャへのポインタを取得します。 
					 * 
					 * @return GXMテクスチャへのポインタ 
					 */
					virtual SceGxmTexture *getGxmTexture(void) = 0;
					/*!
					 * @~English
					 * @brief Get pointer to a GXM texture 
					 * @details This gets the pointer to a GXM texture. 
					 * 
					 * @return Pointer to a GXM texture 
					 * @~Japanese
					 * @brief GXMテクスチャへのポインタを取得する 
					 * @details GXMテクスチャへのポインタを取得します。 
					 * 
					 * @return GXMテクスチャへのポインタ 
					 */
					virtual const SceGxmTexture *getGxmTexture(void) const = 0;

					static TextureGxm* downcast(Texture *texture);
					static const TextureGxm* downcast(const Texture *texture);
				};



				class SCE_SAMPLE_UTIL_API ParameterGxm : public Parameter
				{
				public:

					/*!
					 * @~English
					 * @brief Cast operator to the GXM parameter type 
					 * @details This is the cast operator to the GXM parameter type. 
					 * @return Pointer to a GXM parameter 
					 * @~Japanese
					 * @brief GXMパラメータ型へのキャストオペレータ 
					 * @details GXMパラメータ型へのキャストオペレータです。 
					 * @return GXMパラメータへのポインタ 
					 */
					virtual const SceGxmProgramParameter* getGxmParameter(void) const = 0;

					static const ParameterGxm *downcast(const Parameter* param);

					virtual ~ParameterGxm(void){}
				};

				class SCE_SAMPLE_UTIL_API ProgramGxm : public Program
				{
				public:

					virtual const SceGxmProgram *getGxmProgram(void) const = 0;
					virtual SceGxmShaderPatcherId getGxmShaderPatcherId(void) const  =0;

					static const ProgramGxm *downcast(const Program* param);

					virtual ~ProgramGxm(void){}
				};


				class SCE_SAMPLE_UTIL_API GraphicsLoaderGxm : public GraphicsLoader
				{
				public:
					virtual int createGxmRenderTarget(
						RenderTarget **outRenderTarget, 
						uint32_t width,
						uint32_t height,
						SceGxmColorSurfaceType type /*=SCE_GXM_COLOR_SURFACE_LINEAR*/,
						SceGxmColorFormat format=SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR,
						SceGxmOutputRegisterSize outputRegisterSize = SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,

						uint32_t renderTargetFlags=0,
						uint16_t scenesPerFrame=1,
						SceGxmMultisampleMode msaaMode=SCE_GXM_MULTISAMPLE_NONE,
						uint32_t multisampleLocations=0,

						SceGxmSyncObject*  vertexSyncObject=NULL,
						SceGxmSyncObject*  fragmentSyncObject=NULL) = 0;

					virtual int createGxmDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface, 
						uint32_t width,
						uint32_t height, 
						SceGxmDepthStencilFormat fort /*= SCE_GXM_DEPTH_STENCIL_FORMAT_DF32*/,
						SceGxmDepthStencilSurfaceType type = SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
						SceGxmMultisampleMode multisampleMode=SCE_GXM_MULTISAMPLE_NONE,
						bool enableForceStore=false,
						bool enableForceLoad=false
						) = 0;

					/*!
					 * @~English
					 * @brief Generates a texture of the linear format. 
					 * @details This generates a texture of the linear format. 
					 * 
					 * @param outTexture Pointer to Texture
					 * @param texFormat Format of a texture
					 * @param width Width of a texture
					 * @param height Height of a texture
					 * @param mipCount Number of mip maps in a texture
					 * @param bufferAlign Size alignment of buffer
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief リニアフォーマットのテクスチャを生成する 
					 * @details リニアフォーマットのテクスチャを生成します。 
					 * 
					 * @param outTexture Textureへのポインタ
					 * @param texFormat テクスチャのフォーマット
					 * @param width テクスチャの幅
					 * @param height テクスチャの高さ
					 * @param mipCount テクスチャ内のミップマップ数
					 * @param bufferAlign バッファのサイズアラインメント
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					virtual int createTextureGxmLinear(
						Texture **outTexture,
						SceGxmTextureFormat texFormat,
						uint32_t width,
						uint32_t height,
						uint32_t mipCount,
						uint32_t bufferAlign=16) = 0;

					/*!
					 * @~English
					 * @brief Generates a texture of the linear stride format. 
					 * @details This generates a texture of the linear stride format. 
					 * 
					 * @param outTexture Pointer to Texture
					 * @param texFormat Format of a texture
					 * @param width Width of a texture
					 * @param height Height of a texture
					 * @param byteStride Stride of a texture (Unit: byte)
					 * @param bufferAlign Size alignment of buffer
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief リニアストライドフォーマットのテクスチャを生成する 
					 * @details リニアストライドフォーマットのテクスチャを生成します。 
					 * 
					 * @param outTexture Textureへのポインタ
					 * @param texFormat テクスチャのフォーマット
					 * @param width テクスチャの幅
					 * @param height テクスチャの高さ
					 * @param byteStride テクスチャのストライド(バイト単位)
					 * @param bufferAlign バッファのサイズアラインメント
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					virtual int createTextureGxmLinearStrided(
						Texture **outTexture,
						SceGxmTextureFormat texFormat,
						uint32_t width,
						uint32_t height,
						uint32_t byteStride,
						uint32_t bufferAlign=16) = 0;

					/*!
					 * @~English
					 * @brief Generates a texture of the tile format. 
					 * @details This generates a texture of the tile format. 
					 * 
					 * @param outTexture Pointer to Texture
					 * @param texFormat Format of a texture
					 * @param width Width of a texture
					 * @param height Height of a texture
					 * @param mipCount Number of mip maps in a texture
					 * @param bufferAlign Size alignment of buffer
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief タイルフォーマットのテクスチャを生成する 
					 * @details タイルフォーマットのテクスチャを生成します。 
					 * 
					 * @param outTexture Textureへのポインタ
					 * @param texFormat テクスチャのフォーマット
					 * @param width テクスチャの幅
					 * @param height テクスチャの高さ
					 * @param mipCount テクスチャ内のミップマップ数
					 * @param bufferAlign バッファのサイズアラインメント
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					virtual int createTextureGxmTiled(
						Texture **outTexture,
						SceGxmTextureFormat texFormat,
						uint32_t width,
						uint32_t height,
						uint32_t mipCount,
						uint32_t bufferAlign=16) = 0;

					/*!
					 * @~English
					 * @brief Generates a texture of a swizzle format. 
					 * @details This generates a texture of a swizzle format. 
					 * 
					 * @param outTexture Pointer to Texture
					 * @param texFormat Format of a texture
					 * @param width Width of a texture
					 * @param height Height of a texture
					 * @param mipCount Number of mip maps in a texture
					 * @param bufferAlign Size alignment of buffer
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief スウィズルフォーマットのテクスチャを生成する 
					 * @details スウィズルフォーマットのテクスチャを生成します。 
					 * 
					 * @param outTexture Textureへのポインタ
					 * @param texFormat テクスチャのフォーマット
					 * @param width テクスチャの幅
					 * @param height テクスチャの高さ
					 * @param mipCount テクスチャ内のミップマップ数
					 * @param bufferAlign バッファのサイズアラインメント
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					virtual int createTextureGxmSwizzled(
						Texture **outTexture,
						SceGxmTextureFormat texFormat,
						uint32_t width,
						uint32_t height,
						uint32_t mipCount,
						uint32_t bufferAlign=16) = 0;

					/*!
					 * @~English
					 * @brief Generates a texture of a cube format. 
					 * @details This generates a texture of a cube format. 
					 * 
					 * @param outTexture Pointer to Texture
					 * @param texFormat Format of a texture
					 * @param width Width of a texture
					 * @param height Height of a texture
					 * @param mipCount Number of mip maps in a texture
					 * @param bufferAlign Size alignment of buffer
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief キューブフォーマットのテクスチャを生成する 
					 * @details キューブフォーマットのテクスチャを生成します。 
					 * 
					 * @param outTexture Textureへのポインタ
					 * @param texFormat テクスチャのフォーマット
					 * @param width テクスチャの幅
					 * @param height テクスチャの高さ
					 * @param mipCount テクスチャ内のミップマップ数
					 * @param bufferAlign バッファのサイズアラインメント
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					virtual int createTextureGxmCube(
						Texture **outTexture,
						SceGxmTextureFormat texFormat,
						uint32_t width,
						uint32_t height,
						uint32_t mipCount,
						uint32_t bufferAlign=16) = 0;

					/*!
					 * @~English
					 * @brief Generates a fragment program. 
					 * @details This generates a fragment program. 
					 * 
					 * @param fragmentProgram Fragment program
					 * @param programId ID of a fragment program
					 * @param vertexProgramId ID of a vertex program.
					 * @param blendInfo Blend information
					 * @param outputFormat32BIT Output register format if the render target is 32-bit
					 * @param outputFormat64BIT Output register format if the render target is 64-bit
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief フラグメントプログラムを生成する 
					 * @details フラグメントプログラムを生成します。 
					 * 
					 * @param fragmentProgram フラグメントプログラム
					 * @param programId フラグメントプログラムのID
					 * @param vertexProgramId バーテックスプログラムのID
					 * @param blendInfo ブレンド情報
					 * @param outputFormat32BIT レンダーターゲットが32ビットの場合の出力レジスタフォーマット
					 * @param outputFormat64BIT レンダーターゲットが64ビットの場合の出力レジスタフォーマット
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					virtual int createFragmentProgramGxm(FragmentProgram **fragmentProgram, 
						const Program* programId, 
						const Program* vertexProgramId, 
						const BlendInfo *blendInfo,
						SceGxmOutputRegisterFormat outputFormat32BIT,
						SceGxmOutputRegisterFormat outputFormat64BIT) = 0;


					virtual SceGxmShaderPatcher *getGxmShaderPatcher(void) const = 0;

					/*!
					 * @~English
					 * @brief Memory type 
					 * @details This is the memory type. 
					 * 
					 * @~Japanese
					 * @brief メモリのタイプ 
					 * @details メモリのタイプです。 
					 * 
					 */
					enum MemType
					{
						/*!
						 * @~English
						 * @brief The area on CDRAM. 
						 * 
						 * @~Japanese
						 * @brief CDRAM上の領域 
						 * 
						 */
						MEMTYPE_CDRAM, 
						/*!
						 * @~English
						 * @brief The non-cached area on LPPDR 
						 * 
						 * @~Japanese
						 * @brief LPPDR上の非キャッシュ領域 
						 * 
						 */
					 MEMTYPE_LPDDR_UC, 
					 /*!
					  * @~English
					  * @brief The USSE-mapped area on CDRAM 
					  * 
					  * @~Japanese
					  * @brief CDRAM上のUSSEマップされた領域 
					  * 
					  */
					 MEMTYPE_USSE 
					};

					/*!
					 * @~English
					 * @brief Allocate memory mapped in GPU 
					 * @details This allocates memory mapped in the GPU. 
					 * 
					 * @param memtype Memory type
					 * @param boundary Area boundary
					 * @param size Size of an area
					 * @return Pointer to the buffer allocated 
					 * @~Japanese
					 * @brief GPUにマップされたメモリの確保 
					 * @details GPUにマップされたメモリを確保します。 
					 * 
					 * @param memtype メモリのタイプ
					 * @param boundary 領域の境界
					 * @param size 領域のサイズ
					 * @return 確保したバッファへのポインタ 
					 */
					virtual void *memalign(MemType memtype, uint32_t boundary, uint32_t size) = 0;

					/*!
					 * @~English
					 * @brief Unload memory allocated by memalign 
					 * @details This unloads the memory allocated by memalign. 
					 * 
					 * @param ptr Pointer to an area
					 * @~Japanese
					 * @brief memalignで確保したメモリの解放 
					 * @details memalignで確保したメモリを解放します。 
					 * 
					 * @param ptr 領域へのポインタ
					 */
					virtual void free(void *ptr) = 0;

					struct HeapUsage
					{
						uint32_t totalSize;
						uint32_t maxTotalSize;
						uint32_t memblockTotalSize;
					};

					struct MemoryUsage
					{
						HeapUsage usse;
						HeapUsage lpddrUcRw;
						HeapUsage cdramRw;
					};

					virtual void getMemoryUsage(MemoryUsage *usage) = 0;


					static GraphicsLoaderGxm *downcast(GraphicsLoader *loader);
					static const GraphicsLoaderGxm *downcast(const GraphicsLoader *loader);

					virtual ~GraphicsLoaderGxm(void){}
				};



				class SCE_SAMPLE_UTIL_API GraphicsContextGxm : public GraphicsLoaderGxm, public GraphicsContext
				{
				public:

					/*!
					 * @~English
					 * @brief This is the type of the callback called just before the switch of a frame buffer 
					 * @details This is the type of the callback called just before the switch of a frame buffer 
					 * @param framebuf The information of a frame buffer
					 * @param arg Argument specified when registering a callback
					 * @~Japanese
					 * @brief フレームバッファの切り替え直前に呼ばれるコールバックの型 
					 * @details フレームバッファの切り替え直前に呼ばれるコールバックの型 
					 * @param framebuf フレームバッファの情報
					 * @param arg コールバック登録時に指定した引数
					 */
					typedef void (*OnSetFrameBufCallback)(const SceDisplayFrameBuf *framebuf, uint32_t frameBufferIndex, void *arg);

					/*!
					 * @~English
					 * @brief Set callback function called just before the switch of a frame buffer. 
					 * @details This sets the callback function called just before the switch of a frame buffer. 
					 * 
					 * @param onSetFrameBufCallback Callback function
					 * @param onSetFrameBufCallbackArg Callback function argument
					 * @~Japanese
					 * @brief フレームバッファを切り替える直前に呼ばれるコールバック関数の設定 
					 * @details フレームバッファを切り替える直前に呼ばれるコールバック関数を設定します。 
					 * 
					 * @param onSetFrameBufCallback コールバック関数
					 * @param onSetFrameBufCallbackArg コールバック関数の引数
					 */
					virtual void addOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg) = 0;
					virtual void removeOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg) = 0;

					/*!
					 * @~English
					 * @brief Gets the display size. 
					 * @details This gets the display size. 
					 * 
					 * @return Display size (Width, Height) 
					 * @~Japanese
					 * @brief ディスプレイサイズを取得する 
					 * @details ディスプレイサイズを取得します。 
					 * 
					 * @return ディスプレイサイズ (Width, Height) 
					 */
					static sce::Vectormath::Simd::Aos::Vector2 getDisplaySize(void);

					/*!
					 * @~English
					 * @brief Get GXM context used in this class 
					 * @details This gets the GXM context used in this class. 
					 * 
					 * @return GXM context 
					 * @~Japanese
					 * @brief このクラス内で使用されているGXMコンテキストの取得 
					 * @details このクラス内で使用されているGXMコンテキストを取得します。 
					 * 
					 * @return GXMコンテキスト 
					 */
					virtual SceGxmContext *getGxmContext(void) const = 0;
					/*!
					 * @~English
					 * @brief Save back buffer in a BMP file 
					 * @details This saves a back buffer in a BMP file. 
					 * 
					 * @param path Path of a BMP file
					 * @retval SCE_OK Success
					 * @retval (<0) Error code
					 * @~Japanese
					 * @brief バックバッファをBMPファイルに保存 
					 * @details バックバッファをBMPファイルに保存します。 
					 * 
					 * @param path BMPファイルのパス
					 * @retval SCE_OK 成功
					 * @retval (<0) エラーコード
					 */
					virtual int saveBackBufferAsBmp(const char* path) = 0;

					virtual int endSceneGxm(const SceGxmNotification *vertexNotification, 
						const SceGxmNotification *fragmentNotification) = 0;




					static GraphicsContextGxm *downcast(GraphicsContext *loader);
					static const GraphicsContextGxm *downcast(const GraphicsContext *loader);


					virtual ~GraphicsContextGxm(void){}
				};

				/*!
				 * @~English
				 * @brief Initialize graphics loader 
				 * 
				 * @details This initializes the graphics loader. A shader patcher to be used internally is automatically created. 
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief グラフィックスローダーの初期化 
				 * 
				 * @details グラフィックスローダーを初期化します。内部で使用するシェーダパッチャは自動的に作られます。 
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				SCE_SAMPLE_UTIL_API int createGraphicsLoader(GraphicsLoader **outGraphicsLoader, SceGxmShaderPatcher *patcher);
			}
		}
	}
}




#endif /* _SCE_TARGET_OS_PSP2 */


#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PLATFORM_GXM_H */
