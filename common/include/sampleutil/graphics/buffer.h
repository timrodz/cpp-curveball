/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
 

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_BUFFER_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_BUFFER_H

#include <sampleutil/sampleutil_common.h>
#include <sampleutil/graphics/constant.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			class GraphicsLoader;

			/*!
			 * @~English
			 * 
			 * @brief Class which becomes the base of a buffer object 
			 * @details This is the class which becomes the base of a buffer object. 
			 * @~Japanese
			 * 
			 * @brief バッファオブジェクトの基底となるクラス 
			 * @details バッファオブジェクトの基底となるクラスです。 
			 */
			class SCE_SAMPLE_UTIL_API BufferInterface
			{
				friend class GraphicsLoader;
			public:
				/*!
				 * @~English
				 * @brief Starts writing to the buffer. 
				 * @details This starts writing to the buffer. 
				 * 
				 * @return Pointer to the data area of a buffer object 
				 * @~Japanese
				 * @brief バッファへの書き込みを開始する 
				 * @details バッファへの書き込みを開始します。 
				 * 
				 * @return バッファオブジェクトのデータ領域へのポインタ 
				 */
				virtual void *beginWrite(void) = 0;
				/*!
				 * @~English
				 * @brief Terminates writing to the buffer. 
				 * @details This terminates writing to the buffer. 
				 * 
				 * @~Japanese
				 * @brief バッファへの書き込みを終了する 
				 * @details バッファへの書き込みを終了します。 
				 * 
				 */
				virtual void endWrite(void) = 0;

				/*!
				 * @~English
				 * @brief Gets the pointer to GraphicsLoader. 
				 * @return Pointer to GraphicsLoader 
				 * @details This gets the pointer to GraphicsLoader. 
				 * @~Japanese
				 * @brief GraphicsLoaderのポインタを取得する 
				 * @return GraphicsLoaderのポインタ 
				 * @details GraphicsLoaderのポインタを取得します。 
				 */
				virtual GraphicsLoader* getLoader(void) const = 0;

				/*!
				 * @~English
				 * @brief Return buffer dimension 
				 * @return Buffer dimension 
				 * @details This function returns the buffer dimension. 
				 * @~Japanese
				 * @brief バッファの次元を返す 
				 * @return バッファの次元 
				 * @details この関数はバッファの次元を返します。 
				 */
				virtual BufferDimension getDimension(void) const = 0;
			protected:
				virtual ~BufferInterface(void){}
			};


			/*!
			 * @~English
			 * 
			 * @brief One-dimensional buffer class 
			 * @details A low-level buffer implementation of a one-dimensional buffer object such as, IndexBuffer and StructuredBuffer. 
			 * @~Japanese
			 * 
			 * @brief １次元バッファクラス 
			 * @details IndexBufferやStructuredBufferなど一次元のバッファオブジェクトの下層のバッファの実装です。 
			 */
			class SCE_SAMPLE_UTIL_API Buffer : public Resource, public BufferInterface
			{
			public:
				/*!
				 * @~English
				 * @brief Starts writing to the buffer. 
				 * @details This starts writing to the buffer. 
				 * 
				 * @return Pointer to the data area of a buffer object 
				 * @~Japanese
				 * @brief バッファへの書き込みを開始する 
				 * @details バッファへの書き込みを開始します。 
				 * 
				 * @return バッファオブジェクトのデータ領域へのポインタ 
				 */
				virtual void *beginWrite(void) = 0;
				/*!
				 * @~English
				 * @brief Terminates writing to the buffer. 
				 * @details This terminates writing to the buffer. 
				 * 
				 * @~Japanese
				 * @brief バッファへの書き込みを終了する 
				 * @details バッファへの書き込みを終了します。 
				 * 
				 */
				virtual void endWrite(void) = 0;
				/*!
				 * @~English
				 * @brief Gets the pointer to GraphicsLoader. 
				 * @return Pointer to GraphicsLoader 
				 * @details This gets the pointer to GraphicsLoader. 
				 * @~Japanese
				 * @brief GraphicsLoaderのポインタを取得する 
				 * @return GraphicsLoaderのポインタ 
				 * @details GraphicsLoaderのポインタを取得します。 
				 */
				virtual GraphicsLoader* getLoader(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the data area size of a buffer object 
				 * @details This returns the data area size of a buffer object. 
				 * 
				 * @return Data area size of a buffer object 
				 * @~Japanese
				 * @brief バッファオブジェクトのデータ領域のサイズを返す 
				 * @details バッファオブジェクトのデータ領域のサイズを返します。 
				 * 
				 * @return バッファオブジェクトのデータ領域のサイズ 
				 */
				virtual size_t getSize(void) const = 0;

				/*!
				 * @~English
				 * @brief Returns the bind flag of a buffer object 
				 * @details This returns the bind flag of a buffer object.  The bind flag is a bit OR of kBufferBindFlag.... 
				 * 
				 * @return Bind flag of a buffer object 
				 * @~Japanese
				 * @brief バッファオブジェクトのバインドフラグを返す 
				 * @details バッファオブジェクトのバインドフラグを返します。 バインドフラグは kBufferBindFlag... のビット和です。 
				 * 
				 * @return バッファオブジェクトのバインドフラグ 
				 */
				virtual uint32_t getBindFlags(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the access flag of a buffer object 
				 * @details This returns the access flag of a buffer object. 
				 * 
				 * @return Access flag of a buffer object 
				 * @~Japanese
				 * @brief バッファオブジェクトのアクセスフラグを返す 
				 * @details バッファオブジェクトのアクセスフラグを返します。 
				 * 
				 * @return バッファオブジェクトのアクセスフラグ 
				 */
				virtual BufferAccessMode getAccessMode(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the size of one buffer object element 
				 * @details This returns the size of one buffer object element. 
				 * 
				 * @return Size of one buffer object element 
				 * @~Japanese
				 * @brief バッファオブジェクトの1要素のサイズを返す 
				 * @details バッファオブジェクトの1要素のサイズを返します。 
				 * 
				 * @return バッファオブジェクトの1要素のサイズ 
				 */
				virtual size_t getElementSize(void) const = 0;
				virtual ~Buffer(void){}
			};

			class SCE_SAMPLE_UTIL_API Texture2dBuffer: public Resource, public BufferInterface
			{
			public:
				/*!
				 * @~English
				 * @brief Starts writing to the buffer. 
				 * @details This starts writing to the buffer. 
				 * 
				 * @return Pointer to the data area of a buffer object 
				 * @~Japanese
				 * @brief バッファへの書き込みを開始する 
				 * @details バッファへの書き込みを開始します。 
				 * 
				 * @return バッファオブジェクトのデータ領域へのポインタ 
				 */
				virtual void *beginWrite(void) = 0;
				/*!
				 * @~English
				 * @brief Terminates writing to the buffer. 
				 * @details This terminates writing to the buffer. 
				 * 
				 * @~Japanese
				 * @brief バッファへの書き込みを終了する 
				 * @details バッファへの書き込みを終了します。 
				 * 
				 */
				virtual void endWrite(void) = 0;
				/*!
				 * @~English
				 * @brief Gets the pointer to GraphicsLoader. 
				 * @return Pointer to GraphicsLoader 
				 * @details This gets the pointer to GraphicsLoader. 
				 * @~Japanese
				 * @brief GraphicsLoaderのポインタを取得する 
				 * @return GraphicsLoaderのポインタ 
				 * @details GraphicsLoaderのポインタを取得します。 
				 */
				virtual GraphicsLoader* getLoader(void) const = 0;

				/*!
				 * @~English
				 * @brief Returns the bind flag of a buffer object 
				 * @details This returns the bind flag of a buffer object.  The bind flag is a bit OR of kBufferBindFlag.... 
				 * 
				 * @return Bind flag of a buffer object 
				 * @~Japanese
				 * @brief バッファオブジェクトのバインドフラグを返す 
				 * @details バッファオブジェクトのバインドフラグを返します。 バインドフラグは kBufferBindFlag... のビット和です。 
				 * 
				 * @return バッファオブジェクトのバインドフラグ 
				 */
				virtual uint32_t getBindFlags(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the access flag of a buffer object 
				 * @details This returns the access flag of a buffer object. 
				 * 
				 * @return Access flag of a buffer object 
				 * @~Japanese
				 * @brief バッファオブジェクトのアクセスフラグを返す 
				 * @details バッファオブジェクトのアクセスフラグを返します。 
				 * 
				 * @return バッファオブジェクトのアクセスフラグ 
				 */
				virtual BufferAccessMode getAccessMode(void) const = 0;
				
				/*!
				 * @~English
				 * @brief Returns the format of one buffer object element 
				 * @details This returns the format of one buffer object element. 
				 * 
				 * @return Format of one buffer object element 
				 * @~Japanese
				 * @brief バッファオブジェクトの1要素のフォーマットを返す 
				 * @details バッファオブジェクトの1要素のフォーマットを返します。 
				 * 
				 * @return バッファオブジェクトの1要素のフォーマット 
				 */
				virtual BufferFormat getFormat(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the width of a texture buffer object 
				 * @details This returns the width of a texture buffer object. 
				 * 
				 * @return Width of a texture buffer object 
				 * @~Japanese
				 * @brief テクスチャバッファオブジェクトの幅を返す 
				 * @details テクスチャバッファオブジェクトの幅を返します。 
				 * 
				 * @return テクスチャバッファオブジェクトの幅 
				 */
				virtual uint32_t getWidth(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the height of a texture buffer object 
				 * @details This returns the height of a texture buffer object. 
				 * 
				 * @return Height of a texture buffer object 
				 * @~Japanese
				 * @brief テクスチャバッファオブジェクトの高さを返す 
				 * @details テクスチャバッファオブジェクトの高さを返します。 
				 * 
				 * @return テクスチャバッファオブジェクトの高さ 
				 */
				virtual uint32_t getHeight(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the number of mip maps in a texture buffer object 
				 * @details This returns the number of mip maps in a texture buffer object. 
				 * 
				 * @return Number of mip maps in a texture buffer object 
				 * @~Japanese
				 * @brief テクスチャバッファオブジェクトのミップマップ数を返す 
				 * @details テクスチャバッファオブジェクトのミップマップ数を返します。 
				 * 
				 * @return テクスチャバッファオブジェクトのミップマップ数 
				 */
				virtual uint32_t getMipCount(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the slide of a texture buffer object 
				 * @details This returns the slide of a texture buffer object. 
				 * 
				 * @return Stride of a texture buffer object 
				 * @~Japanese
				 * @brief テクスチャバッファオブジェクトのストライドを返す 
				 * @details テクスチャバッファオブジェクトのストライドを返します。 
				 * 
				 * @return テクスチャバッファオブジェクトのストライド 
				 */
				virtual uint32_t getStride(void) const = 0; 
				/*!
				 * @~English
				 * @brief Returns the multisample mode of a texture buffer object 
				 * @details This returns the multisample mode of a texture buffer object. 
				 * 
				 * @return Multisample mode of a texture buffer object 
				 * @~Japanese
				 * @brief テクスチャバッファオブジェクトのマルチサンプルモードを返す 
				 * @details テクスチャバッファオブジェクトのマルチサンプルモードを返します。 
				 * 
				 * @return テクスチャバッファオブジェクトのマルチサンプルモード 
				 */
				virtual MultisampleMode getMultiSampleMode(void) const = 0;


				virtual ~Texture2dBuffer(void){}
			};
			


			/*!
			 * @~English
			 * @brief Index buffer. 
			 * @details This is the index buffer class. 
			 * @~Japanese
			 * @brief インデックスバッファ 
			 * @details インデックスバッファクラスです。 
			 */
			class SCE_SAMPLE_UTIL_API IndexBuffer : public Resource, public BufferInterface
			{
			protected:
				virtual ~IndexBuffer(void){}
				/*!
				 * @~English
				 * @brief Returns a low-level buffer object 
				 * @details  This returns a low-level buffer object. 
				 * 
				 * @return Low-level buffer object 
				 * @~Japanese
				 * @brief 下層のバッファオブジェクトを返す 
				 * @details  下層のバッファオブジェクトを返します。 
				 * 
				 * @return 下層のバッファオブジェクト 
				 */
				virtual Buffer *getBaseBuffer(void) = 0;
			public:	
			};


			/*!
			 * @~English
			 * @brief Vertex buffer 
			 * @details This is the vertex buffer class. 
			 * 
			 * @~Japanese
			 * @brief バーテックスバッファ 
			 * @details バーテックスバッファクラスです。 
			 * 
			 */
			class SCE_SAMPLE_UTIL_API VertexBuffer : public Resource, public BufferInterface
			{
			protected:
				virtual ~VertexBuffer(void){}
				/*!
				 * @~English
				 * @brief Returns a low-level buffer object 
				 * @details  This returns a low-level buffer object. 
				 * 
				 * @return Low-level buffer object 
				 * @~Japanese
				 * @brief 下層のバッファオブジェクトを返す 
				 * @details  下層のバッファオブジェクトを返します。 
				 * 
				 * @return 下層のバッファオブジェクト 
				 */
				virtual Buffer *getBaseBuffer(void) = 0;
			public:	
			};


			/*!
			 * @~English
			 * @brief Uniform buffer 
			 * @details This is the uniform buffer class. 
			 * 
			 * @~Japanese
			 * @brief ユニフォームバッファ 
			 * @details ユニフォームバッファクラスです。 
			 * 
			 */
			class SCE_SAMPLE_UTIL_API UniformBuffer : public Resource, public BufferInterface
			{
			protected:
				virtual ~UniformBuffer(void){}
				/*!
				 * @~English
				 * @brief Returns a low-level buffer object 
				 * @details  This returns a low-level buffer object. 
				 * 
				 * @return Low-level buffer object 
				 * @~Japanese
				 * @brief 下層のバッファオブジェクトを返す 
				 * @details  下層のバッファオブジェクトを返します。 
				 * 
				 * @return 下層のバッファオブジェクト 
				 */
				virtual Buffer *getBaseBuffer(void) = 0;
			public:	
			};

			/*!
			 * @~English
			 * @brief Structured buffer 
			 * @details Structuring. 
			 * 
			 * @~Japanese
			 * @brief 構造化バッファ 
			 * @details 構造化。 
			 * 
			 */
			class SCE_SAMPLE_UTIL_API StructuredBuffer : public Resource, public BufferInterface
			{
			protected:
				virtual ~StructuredBuffer(void){}
				/*!
				 * @~English
				 * @brief Returns a low-level buffer object 
				 * @details  This returns a low-level buffer object. 
				 * 
				 * @return Low-level buffer object 
				 * @~Japanese
				 * @brief 下層のバッファオブジェクトを返す 
				 * @details  下層のバッファオブジェクトを返します。 
				 * 
				 * @return 下層のバッファオブジェクト 
				 */
				virtual Buffer *getBaseBuffer(void) = 0;
			public:	
			};

			class SCE_SAMPLE_UTIL_API RwStructuredBuffer : public Resource, public BufferInterface
			{
			protected:
				virtual ~RwStructuredBuffer(void){}
				/*!
				 * @~English
				 * @brief Returns a low-level buffer object 
				 * @details  This returns a low-level buffer object. 
				 * 
				 * @return Low-level buffer object 
				 * @~Japanese
				 * @brief 下層のバッファオブジェクトを返す 
				 * @details  下層のバッファオブジェクトを返します。 
				 * 
				 * @return 下層のバッファオブジェクト 
				 */
				virtual Buffer *getBaseBuffer(void) = 0;
			public:	
			};

			/*!
			 * @~English
			 * @brief Texture buffer class 
			 * @details This is the texture buffer class. 
			 * 
			 * @~Japanese
			 * @brief テクスチャバッファクラス 
			 * @details テクスチャバッファクラスです。 
			 * 
			 */
			class SCE_SAMPLE_UTIL_API Texture : public Resource, public BufferInterface
			{
			public:
				virtual ~Texture(void){}
				/*!
				 * @~English
				 * @brief Sets the addressing mode of a texture. 
				 * @details This sets the addressing mode of a texture. 
				 * 
				 * @param uAddrMode U addressing mode
				 * @param vAddrMode V addressing mode
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief テクスチャのアドレッシングモードを設定する 
				 * @details テクスチャのアドレッシングモードを設定します。 
				 * 
				 * @param uAddrMode Uアドレッシングモード
				 * @param vAddrMode Vアドレッシングモード
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int setAddrMode(TextureAddrMode uAddrMode, 
                                TextureAddrMode vAddrMode) = 0;

				/*!
				 * @~English
				 * @brief Sets a texture filter. 
				 * @details This sets a texture filter. 
				 * 
				 * @param minFilter Reduction filter mode
				 * @param magFilter Magnification filter mode
				 * @param mipFilter Mip filter mode
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief テクスチャフィルタを設定する 
				 * @details テクスチャフィルタを設定します。 
				 * 
				 * @param minFilter 縮小フィルタモード
				 * @param magFilter 拡大フィルタモード
				 * @param mipFilter ミップフィルタモード
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int setFilter(
					TextureFilter minFilter,
					TextureFilter magFilter,
					TextureMipFilter mipFilter) = 0;

				/*!
				 * @~English
				 * @brief Gets the width of a texture. 
				 * @details This gets the width of a texture. 
				 * 
				 * @return Width of a texture 
				 * @~Japanese
				 * @brief テクスチャの幅を取得する 
				 * @details テクスチャの幅を取得します。 
				 * 
				 * @return テクスチャの幅 
				 */
				virtual uint32_t getWidth(void) const = 0;
				/*!
				 * @~English
				 * @brief Gets the height of a texture. 
				 * @details This gets the height of a texture. 
				 * 
				 * @return Height of a texture 
				 * @~Japanese
				 * @brief テクスチャの高さを取得する 
				 * @details テクスチャの高さを取得します。 
				 * 
				 * @return テクスチャの高さ 
				 */
				virtual uint32_t getHeight(void) const = 0;
				/*!
				 * @~English
				 * @brief Gets the stride of a texture. 
				 * 
				 * @details This gets the stride of a texture. For type other than SCE_GXM_TEXTURE_LINEAR_STRIDED, 0 is returned. 
				 * @return Stride of a texture (Unit: byte) 
				 * @~Japanese
				 * @brief テクスチャのストライドを取得する 
				 * 
				 * @details テクスチャのストライドを取得します。SCE_GXM_TEXTURE_LINEAR_STRIDED以外の型では0が返されます。 
				 * @return テクスチャのストライド(バイト単位) 
				 */
				virtual uint32_t getStride(void) const = 0;

			};



			/*!
			 * @~English
			 * @brief Render target 
			 * @details This is a render target. 
			 * 
			 * @~Japanese
			 * @brief レンダーターゲット 
			 * @details レンダーターゲットです。 
			 * 
			 */
			 class SCE_SAMPLE_UTIL_API RenderTarget : public Resource
			{
			public:
				/*!
				 * @~English
				 * @brief Gets the width of a render target. 
				 * @details This gets the width of a render target. 
				 * 
				 * @return Width of the render target 
				 * @~Japanese
				 * @brief レンダーターゲットの幅を取得する 
				 * @details レンダーターゲットの幅を取得します。 
				 * 
				 * @return レンダーターゲットの幅 
				 */
				virtual uint32_t getWidth(void) const = 0;

				/*!
				 * @~English
				 * @brief Gets the height of a render target 
				 * @details This gets the height of a render target 
				 * 
				 * @return Height of the render target 
				 * @~Japanese
				 * @brief レンダーターゲットの高さを取得する 
				 * @details レンダーターゲットの高さを取得します。 
				 * 
				 * @return レンダーターゲットの高さ 
				 */
				virtual uint32_t getHeight(void) const = 0;

				/*!
				 * @~English
				 * @brief Gets the pointer to a GXM texture. 
				 * @details This gets the pointer to a GXM texture. 
				 * 
				 * @return Pointer to a GXM texture 
				 * @~Japanese
				 * @brief テクスチャへのポインタを取得する 
				 * @details テクスチャへのポインタを取得します。 
				 * 
				 * @return テクスチャへのポインタ 
				 */
				virtual Texture    *getTexture(void) = 0;

				virtual ~RenderTarget(void){}
			};

			/*!
			 * @~English
			 * @brief Depth stencil target 
			 * @details This is a depth stencil target. 
			 * 
			 * @~Japanese
			 * @brief デプスステンシルターゲット 
			 * @details デプスステンシルターゲットです。 
			 * 
			 */
            class SCE_SAMPLE_UTIL_API DepthStencilSurface : public Resource
			{
			public:

				/*!
				 * @~English
				 * @brief Gets the pointer to a depth and stencil texture. 
				 * @details This gets the pointer to a depth and stencil texture. 
				 * 
				 * @return Pointer to a depth and stencil texture 
				 * @~Japanese
				 * @brief デプス・ステンシルテクスチャへのポインタを取得する 
				 * @details デプス・ステンシルテクスチャへのポインタを取得します。 
				 * 
				 * @return デプス・ステンシルテクスチャへのポインタ 
				 */
				virtual Texture *getTexture(void) = 0;

				virtual ~DepthStencilSurface(void){}
			};

		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_BUFFER_H */
