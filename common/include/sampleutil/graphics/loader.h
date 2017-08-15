/* SIE CONFIDENTIAL

* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_H



#include <sampleutil/graphics/constant.h>
#include <sampleutil/graphics/program.h>
#include <sampleutil/graphics/buffer.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{

			class Program;

			/*!
			 * @~English
			 * 
			 * @brief Vertex program 
			 * @details This is a vertex program. 
			 * @~Japanese
			 * 
			 * @brief 頂点プログラム 
			 * @details 頂点プログラムです。 
			 */
			class VertexProgram : public Resource
			{
			public:
				virtual ~VertexProgram(void){}
			};
			/*!
			 * @~English
			 * 
			 * @brief Fragment program 
			 * @details This is a fragment program. 
			 * @~Japanese
			 * 
			 * @brief フラグメントプログラム 
			 * @details フラグメントプログラムです。 
			 */
			class FragmentProgram : public Resource
			{
			public:
				virtual ~FragmentProgram(void){}
			};

#ifdef _SCE_SAMPLE_UTIL_ENABLE_COMPUTE_SHADER
			/*!
			 * @~English
			 * 
			 * @brief Compute program 
			 * @details This is a compute program. 
			 * @~Japanese
			 * 
			 * @brief コンピュートプログラム 
			 * @details コンピュートプログラムです。 
			 */
			class ComputeProgram : public Resource
			{
			public:
				virtual ~ComputeProgram(void){}
			};
			
#endif


			/*!
			 * @~English
			 * 
			 * @brief Vertex stream 
			 * @details This is the definition of a vertex stream. 
			 * @~Japanese
			 * 
			 * @brief 頂点ストリーム 
			 * @details 頂点ストリームの定義です。 
			 */
			struct VertexStream {

				/*!
				 * @~English
				 * 
				 * @brief Stride of a vertex stream 
				 * @~Japanese
				 * 
				 * @brief 頂点ストリームのストライド 
				 */
				uint16_t stride;
				/*!
				 * @~English
				 * 
				 * @brief Index method. Specify a value of the IndexSource type. 
				 * @~Japanese
				 * 
				 * @brief インデックス方法。IndexSource型の値を指定してください。 
				 */
				uint16_t indexSource;
			};

			/*!
			 * @~English
			 * @brief Structure to indicate the layout of VertexBuffer 
			 * @details This is the structure to indicate the layout of VertexBuffer. This is used when generating VertexProgram. 
			 * @~Japanese
			 * @brief VertexBufferのレイアウトを表す構造体 
			 * @details VertexBufferのレイアウトを表す構造体です。VertexProgramを生成する際に使用します。 
			 */
			struct VertexAttribute
			{
				/*!
				 * @~English
				 * @brief Index of Vertex streams 
				 * 
				 * @~Japanese
				 * @brief Vertexストリームのインデックス 
				 * 
				 */
				uint16_t                   streamIndex;		
				/*!
				 * @~English
				 * @brief Offset in Vertex stream 
				 * 
				 * @~Japanese
				 * @brief Vertexストリーム内のオフセット 
				 * 
				 */
				uint16_t                   offset;	   
				/*!
				 * @~English
				 * @brief Format of the value of an attribute parameter 
				 * 
				 * @~Japanese
				 * @brief アトリビュートパラメータの値のフォーマット 
				 * 
				 */
				AttributeFormat                     format;	
				/*!
				 * @~English
				 * @brief Number of the values of an attribute parameter 
				 * 
				 * @~Japanese
				 * @brief アトリビュートパラメータの値の数 
				 * 
				 */
				uint8_t                    componentCount;  
				/*!
				 * @~English
				 * @brief Semantic of an attribute parameter 
				 * 
				 * @~Japanese
				 * @brief アトリビュートパラメータのセマンティック 
				 * 
				 */
				ParameterSemantic         semantic;         
				/*!
				 * @~English
				 * @brief Semantic index of attribute parameters 
				 * 
				 * @~Japanese
				 * @brief アトリビュートパラメータのセマンティックインデックス 
				 * 
				 */
				uint32_t                   semanticIndex;   
			};

			/*!
			 * @~English
			 * @brief Class to manage the resources required for drawing of graphics 
			 * @details This is the class to manage the resources required for drawing of graphics. 
			 * @~Japanese
			 * @brief グラフィックスの描画に必要なリソースを管理するクラス 
			 * @details グラフィックスの描画に必要なリソースを管理するクラスです。 
			 */
			class SCE_SAMPLE_UTIL_API GraphicsLoader : public Resource
			{
			public:

				/*!
				 * @~English
				 * @brief Registers a vertex program. 
				 * @details This registers a vertex program. 
				 * 
				 * @param outProgram Pointer to which the generated program returns
				 * @param binary Pointer to a vertex program
				 * @param binarySize Size (bytes) of the program
				 * @param shaderFormat Shader format
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief 頂点プログラムを登録する 
				 * @details 頂点プログラムを登録します。 
				 * 
				 * @param outProgram 生成されたプログラムが返されるポインタ
				 * @param binary 頂点プログラムへのポインタ
				 * @param binarySize プログラムのサイズ(byte)
				 * @param shaderFormat シェーダフォーマット
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int registerVertexProgram(Program** outProgram, const void *binary, uint32_t binarySize, ShaderFormat shaderFormat=kShaderFormatDefault) = 0;
				/*!
				 * @~English
				 * @brief Registers a fragment program. 
				 * @details This registers a fragment program. 
				 * 
				 * @param outProgram Pointer to which the generated program returns
				 * @param binary Pointer to a fragment program
				 * @param binarySize Size (bytes) of the program
				 * @param shaderFormat Shader format
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief フラグメントプログラムを登録する 
				 * @details フラグメントプログラムを登録します。 
				 * 
				 * @param outProgram 生成されたプログラムが返されるポインタ
				 * @param binary フラグメントプログラムへのポインタ
				 * @param binarySize プログラムのサイズ(byte)
				 * @param shaderFormat シェーダフォーマット
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int registerFragmentProgram(Program** outProgram, const void *binary, uint32_t binarySize, ShaderFormat shaderFormat=kShaderFormatDefault)= 0;

#ifdef _SCE_SAMPLE_UTIL_ENABLE_COMPUTE_SHADER
				/*!
				 * @~English
				 * @brief Register compute program 
				 * @details This registers a compute program. 
				 * 
				 * @param outProgram Pointer to which the generated program returns
				 * @param binary Pointer to a compute program
				 * @param binarySize Size (bytes) of the program
				 * @param shaderFormat Shader format
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief コンピュートプログラムを登録する 
				 * @details コンピュートプログラムを登録します。 
				 * 
				 * @param outProgram 生成されたプログラムが返されるポインタ
				 * @param binary コンピュートプログラムへのポインタ
				 * @param binarySize プログラムのサイズ(byte)
				 * @param shaderFormat シェーダフォーマット
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int registerComputeProgram(Program** outProgram, const void *binary, uint32_t binarySize, ShaderFormat shaderFormat=kShaderFormatDefault)= 0;
#endif

				/*!
				 * @~English
				 * @brief Generates a vertex program. 
				 * @details This generates a vertex program. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * 
				 * @param vertexProgram Pointer to a vertex program
				 * @param programId ID of a vertex program
				 * @param attrs Array of attributes
				 * @param numAttrs Length of an array of attributes
				 * @param streams Array of streams
				 * @param numStreams Length of an array of streams
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief 頂点プログラムを生成する 
				 * @details 頂点プログラムを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * 
				 * @param vertexProgram 頂点プログラムへのポインタ
				 * @param programId 頂点プログラムのID
				 * @param attrs アトリビュートの配列
				 * @param numAttrs アトリビュートの配列の長さ
				 * @param streams ストリームの配列
				 * @param numStreams ストリームの配列の長さ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createVertexProgram(VertexProgram **vertexProgram, const Program* programId, 
					const VertexAttribute *attrs, uint32_t numAttrs,
					const VertexStream *streams, uint32_t numStreams) = 0;

				/*!
				 * @~English
				 * @brief Generates a fragment program. 
				 * @details This generates a fragment program. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * 
				 * @param fragmentProgram Pointer to a fragment program
				 * @param programId ID of a fragment program
				 * @param vertexProgramId ID of a vertex program used simultaneously with a fragment program
				 * @param blendInfo Blend information. If not blended, NULL is acceptable.
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief フラグメントプログラムを生成する 
				 * @details フラグメントプログラムを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * 
				 * @param fragmentProgram フラグメントプログラムへのポインタ
				 * @param programId フラグメントプログラムのID
				 * @param vertexProgramId フラグメントプログラムと同時につかう頂点プログラムのID
				 * @param blendInfo ブレンド情報。ブレンドしない場合はNULLでもよい。
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createFragmentProgram(FragmentProgram **fragmentProgram, 
					const Program* programId, 
					const Program* vertexProgramId, 
					const BlendInfo *blendInfo) = 0;

#ifdef _SCE_SAMPLE_UTIL_ENABLE_COMPUTE_SHADER
				virtual int createComputeProgram(ComputeProgram **computeProgram, const Program* programId) = 0;
#endif

				/*!
				 * @~English
				 * @brief Copies a vertex program. 
				 * @details This copies a vertex program. 
				 * 
				 * @param vertexProgram Vertex program of the copy destination
				 * @param from Vertex program of the copy source
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief 頂点プログラムを複製する 
				 * @details 頂点プログラムを複製します。 
				 * 
				 * @param vertexProgram 複製先の頂点プログラム
				 * @param from 複製元の頂点プログラム
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int cloneVertexProgram(VertexProgram **vertexProgram, const VertexProgram *from) = 0;

				/*!
				 * @~English
				 * @brief Copies a fragment program. 
				 * @details This copies a fragment program. 
				 * 
				 * @param fragmentProgram Fragment program of the copy destination
				 * @param from Fragment program of the copy source
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief フラグメントプログラムを複製する 
				 * @details フラグメントプログラムを複製します。 
				 * 
				 * @param fragmentProgram 複製先のフラグメントプログラム
				 * @param from 複製元のフラグメントプログラム
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int cloneFragmentProgram(FragmentProgram **fragmentProgram, const FragmentProgram *from) = 0;




				/*!
				 * @~English
				 * @brief Create index buffer 
				 * @details This generates an index buffer. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outIndexBuffer Pointer to which the generated index buffer returns
				 * @param size Buffer size
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief インデックスバッファの生成 
				 * @details インデックスバッファを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outIndexBuffer 生成されたインデックスバッファが返されるポインタ
				 * @param size バッファのサイズ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createIndexBuffer(IndexBuffer **outIndexBuffer, uint32_t size) = 0;
				/*!
				 * @~English
				 * @brief Create vertex buffer 
				 * @details This generates a vertex buffer. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outVertexBuffer Pointer to which the generated vertex buffer returns
				 * @param size Buffer size
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief 頂点バッファの生成 
				 * @details 頂点バッファを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outVertexBuffer 生成された頂点バッファが返されるポインタ
				 * @param size バッファのサイズ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createVertexBuffer(VertexBuffer **outVertexBuffer, uint32_t size) = 0;

				/*!
				 * @~English
				 * @brief Create uniform buffer 
				 * @details This generates a uniform buffer. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * 
				 * @param outVertexBuffer Pointer to which the generated uniform buffer returns
				 * @param size Buffer size
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief ユニフォームバッファの生成 
				 * @details ユニフォームバッファを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * 
				 * @param outVertexBuffer 生成されたユニフォームバッファが返されるポインタ
				 * @param size バッファのサイズ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createUniformBuffer(UniformBuffer * *outVertexBuffer, uint32_t size) = 0;

				/*!
				 * @~English
				 * @brief Create buffer 
				 * @details Creates a buffer for storing one-dimensional number data. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * 
				 * @param outDataBuffer Pointer to which the generated uniform buffer returns
				 * @param size Buffer size
				 * @param accessMode Buffer access mode
				 * @param bufferBindFlags Bind flag (bit OR of BufferBindFlag values)
				 * @param elementSize Element size
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief バッファの生成 
				 * @details １次元の数値データを格納するバッファを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * 
				 * @param outDataBuffer 生成されたユニフォームバッファが返されるポインタ
				 * @param size バッファのサイズ
				 * @param accessMode バッファのアクセスモード
				 * @param bufferBindFlags バインドフラグ（BufferBindFlagの値のビット和）
				 * @param elementSize 要素のサイズ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createBuffer(Buffer **outDataBuffer,
										 size_t              size,
										 BufferAccessMode    accessMode,
										 uint32_t            bufferBindFlags,
										 size_t              elementSize) = 0;

				/*!
				 * @~English
				 * @brief Create a two-dimensional texture buffer 
				 * @details Creates a buffer for storing a two-dimensional texture. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * 
				 * @param outTexture2dBuffer Pointer to which the generated two-dimensional texture buffer is returned
				 * @param format Buffer element format
				 * @param width Texture buffer width
				 * @param height Texture buffer height
				 * @param mipCount Number of mip maps in a texture buffer
				 * @param accessMode Buffer access mode
				 * @param bufferBindFlags Bind flag (bit OR of BufferBindFlag values)
				 * @param multiSampleMode Multisample mode of a texture buffer
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief ２次元テクスチャバッファの生成 
				 * @details ２次元テクスチャを格納するバッファを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * 
				 * @param outTexture2dBuffer 生成された二次元テクスチャバッファが返されるポインタ
				 * @param format バッファの要素のフォーマット
				 * @param width テクスチャバッファの幅
				 * @param height テクスチャバッファの高さ
				 * @param mipCount テクスチャバッファのミップマップ数
				 * @param accessMode バッファのアクセスモード
				 * @param bufferBindFlags バインドフラグ（BufferBindFlagの値のビット和）
				 * @param multiSampleMode テクスチャバッファのマルチサンプルモード
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createTexture2dBuffer(Texture2dBuffer **outTexture2dBuffer,
												  BufferFormat format,
												  uint32_t width,
												  uint32_t height,
												  uint32_t mipCount,
												  BufferAccessMode accessMode,
												  uint32_t bufferBindFlags, /* BufferBindFlag */
												  MultisampleMode multiSampleMode
												  ) = 0;
				
#if _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief Create a two-dimensional texture buffer 
				 * @details Creates a buffer for storing a two-dimensional texture using an already-existing buffer. To delete the generated instance, use sce::SampleUtil::destroy().  When a two-dimensional buffer is deleted, the already-existing buffer is not deleted. 
				 * 
				 * @param outTexture2dBuffer Pointer to which the generated two-dimensional texture buffer is returned
				 * @param format Buffer element format
				 * @param width Texture buffer width
				 * @param height Texture buffer height
				 * @param mipCount Number of mip maps in a texture buffer
				 * @param accessMode Buffer access mode
				 * @param bufferBindFlags Bind flag (bit OR of BufferBindFlag values)
				 * @param multiSampleMode Multisample mode of a texture buffer
				 * @param buffer Already-existing buffer
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief ２次元テクスチャバッファの生成 
				 * @details ２次元テクスチャを格納するバッファを既存のバッファを使って生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 ２次元テクスチャバッファが破棄される時、既存のバッファは破棄されません。 
				 * 
				 * @param outTexture2dBuffer 生成された二次元テクスチャバッファが返されるポインタ
				 * @param format バッファの要素のフォーマット
				 * @param width テクスチャバッファの幅
				 * @param height テクスチャバッファの高さ
				 * @param mipCount テクスチャバッファのミップマップ数
				 * @param accessMode バッファのアクセスモード
				 * @param bufferBindFlags バインドフラグ（BufferBindFlagの値のビット和）
				 * @param multiSampleMode テクスチャバッファのマルチサンプルモード
				 * @param buffer 既存のバッファ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createTexture2dBufferUsingExistentBuffer(Texture2dBuffer **outTexture2dBuffer,
												  BufferFormat format,
												  uint32_t width,
												  uint32_t height,
												  uint32_t mipCount,
												  BufferAccessMode accessMode,
												  uint32_t bufferBindFlags, /* BufferBindFlag */
												  MultisampleMode multiSampleMode,
												  void *buffer
												  ) = 0;
				/*!
				 * @~English
				 * @brief Create a texture from an already-existing texture buffer 
				 * 
				 * @details Creates a texture from an already-existing texture buffer. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outTexture Pointer to which the generated texture is returned
				 * @param buffer Texture buffer
				 * @param texFormat Texture format
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief 既存のテクスチャバッファからテクスチャを生成する 
				 * 
				 * @details 既存のテクスチャバッファからテクスチャを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outTexture 生成されたテクスチャが返されるポインタ
				 * @param buffer テクスチャバッファ
				 * @param texFormat テクスチャフォーマット
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createTextureFromBuffer(Texture **outTexture, Texture2dBuffer *buffer, TextureFormat texFormat) = 0;

#endif



#ifdef _SCE_SAMPLE_UTIL_ENABLE_COMPUTE_SHADER
				/*!
				 * @~English
				 * @brief Create a structured buffer 
				 * 
				 * @details Creates a structured buffer. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outStructuredBuffer Pointer to which the generated structured buffer is returned
				 * @param stride Stride of the structured buffer
				 * @param numElements Number of elements in the structured buffer
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief 構造化バッファを生成する 
				 * 
				 * @details 構造化バッファを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outStructuredBuffer 生成された構造化バッファが返されるポインタ
				 * @param stride 構造化バッファのストライド
				 * @param numElements 構造化バッファのエレメント数
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createStructuredBuffer(StructuredBuffer * *outStructuredBuffer, uint32_t stride, uint32_t numElements) = 0;
				/*!
				 * @~English
				 * @brief Create a read/write enabled structured buffer 
				 * 
				 * @details Creates a read/write enabled structured buffer. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outStructuredBuffer Pointer to which the generated structured buffer is returned
				 * @param stride Stride of the structured buffer
				 * @param numElements Number of elements in the structured buffer
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief 読み書き可能構造化バッファを生成する 
				 * 
				 * @details 読み書き可能構造化バッファを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outStructuredBuffer 生成された構造化バッファが返されるポインタ
				 * @param stride 構造化バッファのストライド
				 * @param numElements 構造化バッファのエレメント数
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createRwStructuredBuffer(RwStructuredBuffer * *outStructuredBuffer, uint32_t stride, uint32_t numElements) = 0;
#endif

				/*!
				 * @~English
				 * @brief Create render target 
				 * @details This generates a render target. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * 
				 * @param outRenderTarget Pointer to which the generated render target returns
				 * @param width Width of the render target
				 * @param height Height of the render target
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief レンダーターゲットの生成 
				 * @details レンダーターゲットを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * 
				 * @param outRenderTarget 生成されたレンダーターゲットが返されるポインタ
				 * @param width レンダーターゲットの幅
				 * @param height レンダーターゲットの高さ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createRenderTarget(RenderTarget **outRenderTarget, 
					uint32_t width, 
					uint32_t height) = 0;

				/*!
				 * @~English
				 * @brief Create depth and stencil target 
				 * @details This generates a depth and stencil target. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * 
				 * @param outDepthStencilSurface Pointer to which the generated depth and stencil target returns
				 * @param width Width of a depth and stencil target
				 * @param height Height of a depth and stencil target
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief デプスステンシルターゲットの生成 
				 * @details デプスステンシルターゲットを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * 
				 * @param outDepthStencilSurface 生成されたデプスステンシルターゲットが返されるポインタ
				 * @param width デプスステンシルターゲットの幅
				 * @param height デプスステンシルターゲットの高さ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface, 
					uint32_t width, 
					uint32_t height) = 0;

				/*!
				 * @~English
				 * @brief This generates a texture from an image file. 
				 * 
				 * @details The supported formats are 24 bit uncompressed TGA (stores form left to right), 32 bit uncompressed TGA (stores from left to right), GNF, 8 bit BMP, 24 bit BMP, and 32 bit BMP. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outTexture Pointer to which the generated texture is returned
				 * @param filename Image file name
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief 画像ファイルからテクスチャを生成する 
				 * 
				 * @details 対応フォーマットは、24bit非圧縮TGA(左から右格納)、32bit非圧縮TGA(左から右格納)、GNF、8bitBMP、24bitBMP、32bitBMPフォーマットです。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outTexture 生成されたテクスチャが返されるポインタ
				 * @param filename 画像ファイル名
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createTextureFromFile(Texture **outTexture, const char *filename) = 0;

				/*!
				 * @~English
				 * @brief Create texture 
				 * @details This generates a texture from an image data in memory. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outTexture Pointer to which the generated texture is returned
				 * @param image Image data
				 * @param imageSize Size (bytes) of the image data
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief テクスチャの生成 
				 * @details メモリ上の画像データからテクスチャを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outTexture 生成されたテクスチャが返されるポインタ
				 * @param image 画像データ
				 * @param imageSize 画像データのサイズ(byte)
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createTextureFromMemory(Texture **outTexture, const void *image, uint32_t imageSize) = 0;

				/*!
				 * @~English
				 * @brief Create texture 
				 * @details This generates a texture. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outTexture Pointer to which the generated texture is returned
				 * @param texFormat Format of a texture
				 * @param width Width of a texture
				 * @param height Height of a texture
				 * @param mipCount Number of mip maps in a texture
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief テクスチャの生成 
				 * @details テクスチャを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outTexture 生成されたテクスチャが返されるポインタ
				 * @param texFormat テクスチャのフォーマット
				 * @param width テクスチャの幅
				 * @param height テクスチャの高さ
				 * @param mipCount テクスチャ内のミップマップ数
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int createTexture(
					Texture **outTexture, 
					TextureFormat texFormat,
					uint32_t width,
					uint32_t height,
					uint32_t mipCount) = 0;


				virtual ~GraphicsLoader(void){}


			};
		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_H */
