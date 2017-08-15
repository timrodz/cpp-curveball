/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_H


#include <sampleutil/graphics/constant.h>
#include <sampleutil/graphics/loader.h>
#include <sampleutil/graphics/buffer.h>
#include <vectormath.h>
#include <stdio.h>


namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
 			/*!
 			 * @~English
 			 * 
 			 * @brief Structure for initializing GraphicsContext 
 			 * @details This is the structure for initializing GraphicsContext. This is used by specifying it to the argument "option" of GrahicsContext::initialize(). 
 			 * @~Japanese
 			 * 
 			 * @brief GraphicsContextの初期化用構造体 
 			 * @details GraphicsContextの初期化用構造体です。GrahicsContext::initialize()の引数optionに指定することで利用します。 
 			 */
			struct GraphicsContextOption
			{
				/*!
				 * @~English
				 * @brief The settings of MSAA of the main render target displayed on the display (Default: kMultisampleNone) 
				 * @~Japanese
				 * @brief ディスプレイに表示するメインレンダーターゲットのMSAAの設定(デフォルト: kMultisampleNone) 
				 */
				MultisampleMode multisampleMode; 

				/*!
				 * @~English
				 * @brief Number of ring buffers for the main render target displayed on the display (Default: 3) 
				 * @~Japanese
				 * @brief ディスプレイに表示するメインレンダーターゲットのリングバッファ数(デフォルト: 3) 
				 */
				uint32_t numFrameBuffer;
#if _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief Heap size to be used as a graphics resource. The heap will be allocated onto direct memory of SCE_KERNEL_WC_GARLIC. 
				 * @~Japanese
				 * @brief Graphicsリソース用に使われるヒープのサイズ。ヒープはSCE_KERNEL_WC_GARLICのダイレクトメモリに確保されます。 
				 */
				size_t videoHeapSize;

				/*!
				 * @~English
				 * @brief When this flag is true, submitDone() will not be called from within GraphicsContext::flip() 
				 * @~Japanese
				 * @brief このフラグが真のときはGraphicsContext::flip()の中でsubmitDone()が呼ばれない 
				 */
				bool disableSubmitDoneInFlip;

				/*!
				 * @~English
				 * @brief Enable VR video output 
				 * @~Japanese
				 * @brief ビデオ出力をVR出力可能に設定します。 
				 */
				bool enableVrView;
				
				/*!
				 * @~English
				 * @brief Enable Stereo video output
				 * @~Japanese
				 * @brief ビデオ出力をステレオ出力可能に設定します。 
				 */
				bool enableS3dView;
#endif
				/*!
				 * @~English
				 * @brief Constructor 
				 * @details This is a constructor. 
				 * @~Japanese
				 * @brief コンストラクタ 
				 * @details コンストラクタです。 
				 */
				GraphicsContextOption(void) :
					multisampleMode(kMultisampleNone),
					numFrameBuffer(3)
#if _SCE_TARGET_OS_ORBIS
					,
					videoHeapSize(1024*1024*1024), // 1 GiB
					disableSubmitDoneInFlip(false),
					enableVrView(false),
					enableS3dView(false)
#endif
				{
				}
			};

			/*!
			 * @~English
			 * 
			 * @brief Graphics context 
			 * 
			 * @details Manages the data required for drawing of graphics and sets the state. 
			 * @~Japanese
			 * 
			 * @brief グラフィックスコンテキスト 
			 * 
			 * @details グラフィックスの描画に必要なデータの管理、状態の設定を行います。 
			 */
			class SCE_SAMPLE_UTIL_API GraphicsContext : public GraphicsLoader
			{

			public:
				virtual ~GraphicsContext(void){}


				/*!
				 * @~English
				 * @brief Clears the render targets that are currently set. 
				 * 
				 * @details This fills the current render targets with the specified color. 
				 * @param color Color used for filling
				 * @~Japanese
				 * @brief 現在設定されているレンダーターゲットをクリアする 
				 * 
				 * @details 指定された色で現在のレンダーターゲットを塗りつぶします。 
				 * @param color 塗りつぶしに使う色
				 */
				virtual void clearRenderTarget(uint32_t color) = 0;

				/*!
				 * @~English
				 * @brief Frame buffer side 
				 * @details Used for the frame buffer side when Stereoscopic 3D is enabled. When disabled, only LEFT will be displayed on the display. 
				 * @~Japanese
				 * @brief フレームバッファの面 
				 * @details Stereoscopic 3Dを有効にした場合のフレームバッファの面に使用します。有効になっていない場合は、LEFTのみがディスプレイに表示されます。 
				 */
				enum FrameBufferSide
				{
					/*!
					 * @~English
					 * @brief Left side 
					 * @~Japanese
					 * @brief 左面 
					 */
					kFrameBufferSideLeft = 0,
					/*!
					 * @~English
					 * @brief Right side 
					 * @~Japanese
					 * @brief 右面 
					 */
					kFrameBufferSideRight = 1
				};

				/*!
				 * @~English
				 * @brief Display mode 
				 * @details This is the display mode. The normal display mode and Stereoscopic 3D display mode are supported. 
				 * @~Japanese
				 * @brief ディスプレイモード 
				 * @details ティスプレイモードです。通常のディスプレイモード、もしくはStereoscopic 3Dディスプレイモードの２つをサポートします。 
				 */
				enum DisplayMode
				{
					/*!
					 * @~English
					 * @brief The normal display mode that only shows one side 
					 * @~Japanese
					 * @brief 通常の、片面のみを表示するディスプレイモード 
					 */
					kDisplayModeNormal,
					/*!
					 * @~English
					 * @brief Stereoscopic 3D mode. Displays both the left side and the right side. 
					 * @~Japanese
					 * @brief Stereoscopic 3Dモード。左面、右面の両方を表示します。 
					 */
					kDisplayModeS3dView,
					
					kDisplayModeVrView90Hz,

					kDisplayModeVrView120Hz,
				};
				/*!
				 * @~English
				 * @brief Check display mode support 
				 * @details This checks if the specified display mode is supported by the display. 
				 * @param mode Display mode
				 * @return True if supported, false if not supported. 
				 * @~Japanese
				 * @brief ディスプレイモードのサポートの確認 
				 * @details ディスプレイが指定されたティスプレイモードをサポートしているかどうかの確認を行います。 
				 * @param mode ディスプレイモード
				 * @return サポートされていればtrue, そうでなければfalse. 
				 */
				virtual bool isDisplaymodeSupported(DisplayMode mode)= 0;
				/*!
				 * @~English
				 * @brief Change the display mode 
				 * @details This changes the display mode. An error will return if the specified mode is not supported by the display. The initial value for the display mode is kDisplayModeNormal. 
				 * @param mode Display mode
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief ディスプレイモードの変更 
				 * @details ディスプレイモードを変更します。ディスプレイが指定されたモードをサポートしてない場合はエラーが返ります。ディスプレイモードの初期値は kDisplayModeNormal です。 
				 * @param mode ディスプレイモード
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int setDisplaymode(DisplayMode mode) = 0;

				/*!
				 * @~English
				 * @brief Gets the render target of the frame buffer to be drawn next. 
				 * @details This gets the render target of the frame buffer to be drawn next. 
				 * @param size Frame buffer side (default: kFrameBufferSideLeft)
				 * @return Pointer to a render target 
				 * @~Japanese
				 * @brief 次に描画するフレームバッファのレンダーターゲットを取得する 
				 * @details 次に描画するフレームバッファのレンダーターゲットを取得します。 
				 * @param size フレームバッファの面(デフォルト：kFrameBufferSideLeft)
				 * @return レンダーターゲットへのポインタ 
				 */
				virtual RenderTarget *getNextRenderTarget(FrameBufferSide side=kFrameBufferSideLeft) = 0;
				/*!
				 * @~English
				 * @brief Gets the depth and stencil target for frame buffer. 
				 * @details This gets the depth and stencil target for frame buffer. 
				 * 
				 * @return Pointer to a depth and stencil target 
				 * @~Japanese
				 * @brief フレームバッファ用デプスステンシルターゲットを取得する 
				 * @details フレームバッファ用デプスステンシルターゲットを取得します。 
				 * 
				 * @return デプスステンシルターゲットへのポインタ 
				 */
				virtual DepthStencilSurface *getDepthStencilSurface(void) = 0;

				/*!
				 * @~English
				 * @brief Starts drawing on the render target. 
				 * @details This starts drawing on the render target. 
				 * 
				 * @param renderTarget Render target
				 * @param depthStencilSurface Depth stencil target
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief レンダーターゲットへの描画を開始する 
				 * @details レンダーターゲットへの描画を開始します。 
				 * 
				 * @param renderTarget レンダーターゲット
				 * @param depthStencilSurface デプスステンシルターゲット
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int beginScene(RenderTarget *renderTarget, DepthStencilSurface *depthStencilSurface) = 0;
				/*!
				 * @~English
				 * @brief Terminates drawing on the render target. 
				 * @details This terminates drawing on the render target. 
				 * 
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief レンダーターゲットへの描画を終了する 
				 * @details レンダーターゲットへの描画を終了します。 
				 * 
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int endScene(void) = 0;

				/*!
				 * @~English
				 * @brief Flip processing of a frame buffer 
				 * @details This executes flip processing of a frame buffer 
				 * 
				 * @param numVSync Number of VSYNCs to wait until flip
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief フレームバッファのフリップ処理 
				 * @details フレームバッファのフリップ処理を行います。 
				 * 
				 * @param numVSync フリップまでに待つVSYNC数
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int flip(uint32_t numVSync) = 0;


				/*!
				 * @~English
				 * @brief Sets the comparison function of depth values. 
				 * @details This sets the comparison function of depth values. 
				 * @param depthFunc Comparison function of depth values
				 * 
				 * @~Japanese
				 * @brief デプス値の比較関数を設定する 
				 * @details デプス値の比較関数を設定します。 
				 * @param depthFunc デプス値の比較関数
				 * 
				 */
				virtual void setDepthFunc(DepthFunc depthFunc) = 0;

				/*!
				 * @~English
				 * @brief Enables or disables depth writing. 
				 * @details This enables or disables depth writing. 
				 * @param enable Enables or disables. Enabled if it is true.
				 * @~Japanese
				 * @brief デプス書き込みの有効・無効を設定する 
				 * @details デプス書き込みの有効・無効を設定します。 
				 * @param enable 有効・無効の指定。trueの場合有効。
				 */
				virtual void setDepthWriteEnable(bool enable) = 0;

				/*!
				 * @~English
				 * @brief Sets the fill mode. 
				 * @details This sets the fill mode. 
				 * @param isFill Enables or disables. Enabled if it is true.
				 * @~Japanese
				 * @brief フィルモードを設定する 
				 * @details フィルモードを設定します。 
				 * @param isFill 有効・無効の指定。trueの場合有効。
				 */
				virtual void setFillMode(bool isFill) = 0;

				/*!
				 * @~English
				 * @brief Sets the culling mode. 
				 * @details This sets the culling mode. 
				 * @param cullMode Culling mode
				 * @~Japanese
				 * @brief カリングモードを設定する 
				 * @details カリングモードを設定します。 
				 * @param cullMode カリングモード
				 */
				virtual void setCullMode(CullMode cullMode) = 0;


				/*!
				 * @~English
				 * @brief Sets a vertex program. 
				 * @details This sets a vertex program. 
				 * @param vertexProgram Vertex program
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief 頂点プログラムを設定する 
				 * @details 頂点プログラムを設定します。 
				 * @param vertexProgram 頂点プログラム
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				virtual int setVertexProgram(VertexProgram *vertexProgram) = 0;
				/*!
				 * @~English
				 * @brief Sets a fragment program. 
				 * @details This sets a fragment program. 
				 * @param fragmentProgram Fragment program
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief フラグメントプログラムを設定する 
				 * @details フラグメントプログラムを設定します。 
				 * @param fragmentProgram フラグメントプログラム
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				virtual int setFragmentProgram(FragmentProgram *fragmentProgram) = 0;
				/*!
				 * @~English
				 * @brief Sets a vertex buffer. 
				 * @details This sets a vertex buffer. 
				 * 
				 * @param vertexBuffer Vertex buffer
				 * @param streamIndex Index of buffers
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief 頂点バッファを設定する 
				 * @details 頂点バッファを設定します。 
				 * 
				 * @param vertexBuffer 頂点バッファ
				 * @param streamIndex バッファのインデックス
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				virtual int setVertexBuffer(VertexBuffer *vertexBuffer, uint32_t streamIndex) = 0;

				/*!
				 * @~English
				 * @brief Drawing 
				 * @details This Draws. 
				 * 
				 * @param primitive Drawing primitive
				 * @param indexBuffer Index buffer.
				 * @param count Number of indices
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief 描画 
				 * @details 描画します。 
				 * 
				 * @param primitive 描画プリミティブ
				 * @param indexBuffer インデックスバッファ
				 * @param count インデックス数
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				virtual int draw(Primitive primitive, IndexBuffer *indexBuffer, uint32_t count) = 0;

				/*!
				 * @~English
				 * @brief Sets a uniform buffer. 
				 * @details This sets a uniform buffer. 
				 * 
				 * @param programType Type of a shader program
				 * @param buffer Uniform buffer
				 * @param bufferIndex Index of uniform buffers
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief ユニフォームバッファを設定する 
				 * @details ユニフォームバッファを設定します。 
				 * 
				 * @param programType シェーダプログラムのタイプ
				 * @param buffer ユニフォームバッファ
				 * @param bufferIndex ユニフォームバッファのインデックス
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				virtual int setUniformBuffer(ProgramType programType, UniformBuffer *buffer, uint32_t bufferIndex) = 0;

				/*!
				 * @~English
				 * @brief Sets a texture. 
				 * @details This sets a texture. 
				 * 
				 * @param programType Type of a shader program
				 * @param texture Texture
				 * @param index Index of textures
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief テクスチャを設定する 
				 * @details テクスチャを設定します。 
				 * 
				 * @param programType シェーダプログラムのタイプ
				 * @param texture テクスチャ
				 * @param index テクスチャのインデックス
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				virtual int setTexture(ProgramType programType, Texture *texture, uint32_t index) = 0;

				/*!
				 * @~English
				 * @brief Allocates the area for the default uniform buffer. 
				 * 
				 * @details This allocates the area for the default uniform buffer. Call this function after setting a shader program. 
				 * 
				 * @param programType Type of a shader program
				 * @return Pointer to an area 
				 * @~Japanese
				 * @brief デフォルトユニフォームバッファ用の領域を確保する 
				 * 
				 * @details デフォルトユニフォームバッファ用の領域を確保します。シェーダプログラムを設定してからこの関数を呼んでください。 
				 * 
				 * @param programType シェーダプログラムのタイプ
				 * @return 領域へのポインタ 
				 */
				virtual void *reserveDefaultUniformBuffer(ProgramType programType) = 0;

				/*!
				 * @~English
				 * @brief Sets a view port. 
				 * 
				 * @param topLeftX The x coordinate at the upper left of a view port
				 * @param topLeftY The y coordinate at the upper left of a view port
				 * @param width Width of a view port
				 * @param height Height of a view port
				 * @param minDepth The minimum depth of a view port (0 to 1)
				 * @param maxDepth The maximum depth of a view port (0 to 1)
				 * @details This sets a view port. In PlayStation(R)Vita, the coordinate for clipping is aligned to the granularity of the tile. This alignment will be performed so that the view port will be in an area inside the area specified with the argument. 
				 * @~Japanese
				 * @brief ビューポートを設定する 
				 * 
				 * @param topLeftX ビューポートの左上のx座標
				 * @param topLeftY ビューポートの左上のy座標
				 * @param width ビューポートの幅
				 * @param height ビューポートの高さ
				 * @param minDepth ビューポートの最小深度(0から1)
				 * @param maxDepth ビューポートの最大深度(0から1)
				 * @details ビューポートを設定します。PlayStation(R)Vitaでは、クリップされる座標はタイルの粒度にアラインされます。このアラインは引数に指定された領域の内側の領域になるように行われます。 
				 */
				virtual void setViewPort(uint32_t topLeftX,
								 uint32_t topLeftY,
								 uint32_t width,
								 uint32_t height,
								 float minDepth,
								 float maxDepth) = 0;

				/*!
				 * @~English
				 * @brief Sets a region clip. 
				 * 
				 * @param topLeftX The x coordinate at the upper left of a clip area.
				 * @param topLeftY The y coordinate at the upper left of a clip area.
				 * @param width Width of a clip region
				 * @param height Height of a clip area
				 * @details This sets a region clip. In PlayStation(R)Vita, the coordinate for clipping is aligned to the granularity of the tile. This alignment will be performed so that the view port will be in an area inside the area specified with the argument. 
				 * @~Japanese
				 * @brief リージョンクリップを設定する 
				 * 
				 * @param topLeftX クリップ領域の左上のx座標
				 * @param topLeftY クリップ領域の左上のy座標
				 * @param width クリップ領域の幅
				 * @param height クリップ領域の高さ
				 * @details リージョンクリップを設定します。PlayStation(R)Vitaでは、クリップされる座標はタイルの粒度にアラインされます。このアラインは引数に指定された領域の内側の領域になるように行われます。 
				 */
				virtual void setRegionClip(uint32_t topLeftX,
								   uint32_t topLeftY,
								   uint32_t width,
								   uint32_t height) = 0;

				/*!
				 * @~English
				 * @brief Sets a stencil function. 
				 * @details This sets a stencil function. 
				 * 
				 * @param compareMask Bit mask used for comparison of stencil buffers. An AND operation is executed for the current stencil value with this value before conducting a test.
				 * @param writeMask Mask on a bit basis applied to stencil values after stencil operations
				 * @param frontFaceFunc Stencil comparison function for the front
				 * @param frontFaceStencilFail Stencil operation executed when a stencil test for the front fails
				 * @param frontFaceDepthFail Stencil operation executed when a depth test for the front fails
				 * @param frontFaceDepthPass Stencil operation executed when a depth test for the front is passed
				 * @param backFaceFunc Stencil comparison function for the back
				 * @param backFaceStencilFail Stencil operation executed when a stencil test for the back fails
				 * @param backFaceDepthFail Stencil operation executed when a depth test for the back fails
				 * @param backFaceDepthPass Stencil operation executed when a depth test for the back is passed
				 * @param stencilRef Stencil reference value
				 * @~Japanese
				 * @brief ステンシル関数を設定する 
				 * @details ステンシル関数を設定します。 
				 * 
				 * @param compareMask ステンシルバッファの比較を行う際に使われるビットマスク。テストを行う前に、現在のステンシル値はこの値とAND演算される
				 * @param writeMask ステンシル操作の後でステンシル値に適用されるビット単位のマスク
				 * @param frontFaceFunc 前面用ステンシル比較関数
				 * @param frontFaceStencilFail 前面用ステンシルテストに失敗した場合に行われるステンシル操作
				 * @param frontFaceDepthFail 前面用デプステストに失敗した場合に行われるステンシル操作
				 * @param frontFaceDepthPass 前面用デプステストにパスした場合に行われるステンシル操作
				 * @param backFaceFunc 背面用ステンシル比較関数
				 * @param backFaceStencilFail 背面用ステンシルテストに失敗した場合に行われるステンシル操作
				 * @param backFaceDepthFail 背面用デプステストに失敗した場合に行われるステンシル操作
				 * @param backFaceDepthPass 背面用デプステストにパスした場合に行われるステンシル操作
				 * @param stencilRef ステンシル参照値
				 */
				virtual void setStencilFunc(
					uint8_t compareMask,
					uint8_t writeMask,
					StencilFunc frontFaceFunc,
					StencilOp   frontFaceStencilFail,
					StencilOp   frontFaceDepthFail,
					StencilOp   frontFaceDepthPass,
					StencilFunc backFaceFunc,
					StencilOp   backFaceStencilFail,
					StencilOp   backFaceDepthFail,
					StencilOp   backFaceDepthPass,
					uint8_t stencilRef
					) = 0;


				/*!
				 * @~English
				 * @brief Gets the current render target. 
				 * @details This gets the current render target. 
				 * 
				 * @return Current render target 
				 * @~Japanese
				 * @brief 現在のレンダーターゲットを取得する 
				 * @details 現在のレンダーターゲットを取得します。 
				 * 
				 * @return 現在のレンダーターゲット 
				 */
				virtual RenderTarget *getCurrentRenderTarget(void) const = 0;

				/*!
				 * @~English
				 * @brief Gets the current depth and stencil targets. 
				 * @details This gets the current depth and stencil targets. 
				 * 
				 * @return Current depth and stencil targets 
				 * @~Japanese
				 * @brief 現在のデプスステンシルターゲットを取得する 
				 * @details 現在のデプスステンシルターゲットを取得します。 
				 * 
				 * @return 現在のデプスステンシルターゲット 
				 */
				virtual DepthStencilSurface *getCurrentDepthStencilSurface(void) const = 0;

				/*!
				 * @~English
				 * @brief Change width of lines and points 
				 * @details This changes the pixel unit for the width of lines and points. 
				 * 
				 * @param width Line and dot width
				 * @~Japanese
				 * @brief 線と点の幅を変更する 
				 * @details 線と点の幅をピクセル単位で変更します。 
				 * 
				 * @param width 線と点の幅
				 */
				virtual void setLineWidth(uint32_t width) = 0;
				virtual void setPointSize(uint16_t halfWidth, uint16_t halfHeight) = 0;

				/*!
				 * @~English
				 * @brief Saves a back buffer in a BMP file 
				 * @details This saves a back buffer in a BMP file. 
				 * 
				 * @param path Path of a BMP file
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief バックバッファをBMPファイルに保存する 
				 * @details バックバッファをBMPファイルに保存します。 
				 * 
				 * @param path BMPファイルのパス
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int saveBackBufferAsBmp(const char* path) = 0;


#ifdef _SCE_SAMPLE_UTIL_ENABLE_COMPUTE_SHADER

				virtual int setComputeProgram(ComputeProgram *computeProgram) = 0;
				
				virtual int setResource(ProgramType programType, Texture *texture, uint32_t index) = 0;
				virtual int setResource(ProgramType programType, StructuredBuffer *buffer, uint32_t index) = 0;
				virtual int setRwResource(ProgramType programType, RwStructuredBuffer *buffer, uint32_t index) = 0;

				virtual int dispatch(uint32_t threadGroupX,
									 uint32_t threadGroupY,
									 uint32_t threadGroupZ) = 0;

#endif

#if _SCE_TARGET_OS_ORBIS
				virtual int submitOnly(void) = 0;
				virtual int prepareNextFrame(void) = 0;
				
				virtual int resizeFrameBuffer(uint32_t width, uint32_t height) = 0;
				virtual void waitVBlank(void) = 0;
#endif
			};

			/*!
			 * @~English
			 * @brief Generate graphics context 
			 * 
			 * @details This generates a graphics context. To delete the generated instance, use sce::SampleUtil::destroy(). 
			 * @param outGraphicsContext Pointer to which the generated graphics context returns
			 * @param width Width of the main render target displayed on the display
			 * @param height Height of the main render target displayed on the display
			 * @param option GraphicsContexOption structure. This is initialized by the default value if NULL is specified.
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @~Japanese
			 * @brief グラフィックコンテキストの生成 
			 * 
			 * @details グラフィックスコンテキストを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
			 * @param outGraphicsContext 生成されたグラフィックスコンテキストが返るポインタ
			 * @param width ディスプレイに表示するメインレンダーターゲットの幅
			 * @param height ディスプレイに表示するメインレンダーターゲットの高さ
			 * @param option GraphicsContexOption構造体。NULLを指定するとデフォルトの値で初期化されます。
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 */
			SCE_SAMPLE_UTIL_API int createGraphicsContext(GraphicsContext **outGraphicsContext, uint32_t width, uint32_t height,
				                                           GraphicsContextOption* option=NULL);

		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_H */
