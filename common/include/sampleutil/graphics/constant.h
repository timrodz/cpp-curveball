/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_CONSTANT_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_CONSTANT_H

#include <sampleutil/sampleutil_common.h>
#include <sampleutil/sampleutil_error.h>

namespace sce
{

	namespace SampleUtil
	{

		/*!
		 * @~English
		 * @brief Graphics-associated definitions 
		 * @details These are the Graphics-associated definitions. 
		 * @~Japanese
		 * @brief Graphics関連の定義 
		 * @details Graphics関連の定義です。 
		 */
		namespace Graphics
		{

			/*!
			 * @~English
			 * @brief Multisample mode 
			 * @details This is the multisampling mode. 
			 * @~Japanese
			 * @brief マルチサンプルモード 
			 * @details マルチサンプリングモードです。 
			 */
			enum MultisampleMode {
				/*!
				 * @~English
				 * @brief Single central sample 
				 * @~Japanese
				 * @brief 単一の中心サンプル 
				 */
				kMultisampleNone,

				/*!
				 * @~English
				 * @brief 2x diagonal samples 
				 * @~Japanese
				 * @brief 2x 対角サンプル 
				 */
				kMultisample2x,

				/*!
				 * @~English
				 * @brief 4x rotation grids 
				 * @~Japanese
				 * @brief 4x 回転グリッド 
				 */
				kMultisample4x
			};

			/*!
			 * @~English
			 * @brief Format of a vertex attribute 
			 * @details This is the format of a vertex attribute. 
			 * @~Japanese
			 * @brief 頂点属性のフォーマット 
			 * @details 頂点属性のフォーマットです。 
			 */
			enum AttributeFormat
			{
				/*!
				 * @~English
				 * @brief 8-bit unsigned integer 
				 * @~Japanese
				 * @brief 8ビット符号なし整数 
				 */
				kAttributeFormatU8   ,

				/*!
				 * @~English
				 * @brief 8-bit signed integer 
				 * @~Japanese
				 * @brief 8ビット符号付き整数 
				 */
				kAttributeFormatS8   ,

				/*!
				 * @~English
				 * @brief 16-bit unsigned integer 
				 * @~Japanese
				 * @brief 16ビット符号なし整数 
				 */
				kAttributeFormatU16  ,

				/*!
				 * @~English
				 * @brief 16-bit signed integer 
				 * @~Japanese
				 * @brief 16ビット符号付き整数 
				 */
				kAttributeFormatS16  ,

				/*!
				 * @~English
				 * @brief 8-bit unsigned integer normalized within the range of [0,1] 
				 * @~Japanese
				 * @brief [0,1]の範囲に正規化された8ビット符号なし整数 
				 */
				kAttributeFormatU8N  ,

				/*!
				 * @~English
				 * @brief 8-bit signed integer normalized within the range of [-1,1] 
				 * @~Japanese
				 * @brief [-1,1]の範囲に正規化された8ビット符号付き整数 
				 */
				kAttributeFormatS8N  ,
				
				/*!
				 * @~English
				 * @brief 16-bit unsigned integer normalized within the range of [0,1] 
				 * @~Japanese
				 * @brief [0,1]の範囲に正規化された16ビット符号なし整数 
				 */
				kAttributeFormatU16N ,

				/*!
				 * @~English
				 * @brief 16-bit signed integer normalized within the range of [-1,1] 
				 * @~Japanese
				 * @brief [-1,1]の範囲に正規化された16ビット符号付き整数 
				 */
				kAttributeFormatS16N ,

				/*!
				 * @~English
				 * @brief 16-bit half-precision floating-point number 
				 * @~Japanese
				 * @brief 16ビット半精度浮動小数点数 
				 */
				kAttributeFormatF16  ,

				/*!
				 * @~English
				 * @brief 32-bit single precision floating-point number 
				 * @~Japanese
				 * @brief 32ビット単精度浮動小数点数 
				 */
				kAttributeFormatF32  
			};



			/*!
			 * @~English
			 * @brief Semantic associated with a program parameter 
			 * @details This is the semantic associated with a program parameter. 
			 * @~Japanese
			 * @brief プログラムパラメータに関連付けられたセマンティック 
			 * @details プログラムパラメータに関連付けられたセマンティックです。 
			 */
			enum ParameterSemantic 
			{
				/*!
				 * @~English
				 * @brief The parameter has no semantic. 
				 * @~Japanese
				 * @brief パラメータにはセマンティックがない。 
				 */
				kParameterSemanticNone          ,

				/*!
				 * @~English
				 * @brief The parameter has an ATTR semantic. 
				 * @~Japanese
				 * @brief パラメータは、ATTRセマンティックを持つ。 
				 */
				kParameterSemanticAttr         ,

				/*!
				 * @~English
				 * @brief The parameter has a BCOL semantic. 
				 * @~Japanese
				 * @brief パラメータは、BCOLセマンティックを持つ。 
				 */
				kParameterSemanticBcol          ,

				/*!
				 * @~English
				 * @brief The parameter has a BINORMAL semantic. 
				 * @~Japanese
				 * @brief パラメータは、BINORMALセマンティックを持つ。 
				 */
				kParameterSemanticBinormal      ,

				/*!
				 * @~English
				 * @brief The parameter has a BLENDINDICES semantic. 
				 * @~Japanese
				 * @brief パラメータは、BLENDINDICESセマンティックを持つ。 
				 */
				kParameterSemanticBlendindices  ,

				/*!
				 * @~English
				 * @brief The parameter has a BLENDWEIGHT semantic. 
				 * @~Japanese
				 * @brief パラメータは、BLENDWEIGHTセマンティックを持つ。 
				 */
				kParameterSemanticBlendweight  ,

				/*!
				 * @~English
				 * @brief The parameter has a COLOR semantic. 
				 * @~Japanese
				 * @brief パラメータは、COLORセマンティックを持つ。 
				 */
				kParameterSemanticColor         ,

				/*!
				 * @~English
				 * @brief The parameter has a DIFFUSE semantic. 
				 * @~Japanese
				 * @brief パラメータは、DIFFUSEセマンティックを持つ。 
				 */
				kParameterSemanticDiffuse       ,

				/*!
				 * @~English
				 * @brief The parameter has a FOGCOORD semantic. 
				 * @~Japanese
				 * @brief パラメータは、FOGCOORDセマンティックを持つ。 
				 */
				kParameterSemanticFogcoord      ,

				/*!
				 * @~English
				 * @brief The parameter has a NORMAL semantic. 
				 * @~Japanese
				 * @brief パラメータは、NORMALセマンティックを持つ。 
				 */
				kParameterSemanticNormal        ,

				/*!
				 * @~English
				 * @brief The parameter has a POINTSIZE semantic. 
				 * @~Japanese
				 * @brief パラメータは、POINTSIZEセマンティックを持つ。 
				 */
				kParameterSemanticPointsize     ,

				/*!
				 * @~English
				 * @brief The parameter has a POSITION semantic. 
				 * @~Japanese
				 * @brief パラメータは、POSITIONセマンティックを持つ。 
				 */
				kParameterSemanticPosition      ,

				/*!
				 * @~English
				 * @brief The parameter has a SPECULAR semantic. 
				 * @~Japanese
				 * @brief パラメータは、SPECULARセマンティックを持つ。 
				 */
				kParameterSemanticSpecular      ,

				/*!
				 * @~English
				 * @brief The parameter has a TANGENT semantic. 
				 * @~Japanese
				 * @brief パラメータは、TANGENTセマンティックを持つ。 
				 */
				kParameterSemanticTangent       ,

				/*!
				 * @~English
				 * @brief The parameter has a TEXCOORD semantic. 
				 * @~Japanese
				 * @brief パラメータは、TEXCOORDセマンティックを持つ。 
				 */
				kParameterSemanticTexcoord      


			};


			/*!
			 * @~English
			 * @brief The addressing mode of a texture 
			 * @details This is the addressing mode of a texture. 
			 * @~Japanese
			 * @brief テクスチャのアドレッシングモード 
			 * @details テクスチャのアドレッシングモードです。 
			 */
			enum TextureAddrMode
			{
				/*!
				 * @~English
				 * @brief This executes addressing of a texture using the fractional part of uv. In this case, the texture is repeated in this axis direction. 
				 * @~Japanese
				 * @brief uvの小数部分を使ってテクスチャのアドレッシングを行う。この場合、この軸方向にテクスチャが繰り返される。 
				 */
				kTextureAddrModeRepeat,

				/*!
				 * @~English
				 * @brief f and (1-f) are used alternately. However, f is the fractional part of uv. In this case, the texture is repeated in this axis direction, but at that time, an inverted texture and a non-inverted one are repeated alternately. 
				 * @~Japanese
				 * @brief fと(1-f)を交互に使う。ただし、fはuvの小数部分である。この場合、テクスチャがこの軸方向に繰り返されるが、その際、テクスチャを反転したものと反転していないものが交互に使用される。 
				 */
				kTextureAddrModeMirror,

				/*!
				 * @~English
				 * @brief uv is clamped in the range of [0,1] to avoid filtering over the edge of a texture in the case that filtering is enabled. In this case, the result of "clamped at the edge" is generated. 
				 * @~Japanese
				 * @brief uvを[0,1]の範囲にクランプし、フィルタリングが有効になっている場合にテクスチャのエッジを越えてフィルタリングしないようにする。この場合、「エッジにクランプ」の結果が生成される。 
				 */
				kTextureAddrModeClamp,

				/*!
				 * @~English
				 * @brief The absolute value of uv is clamped in the range of [0,1] to avoid filtering over the edge of a texture in the case that filtering is enabled. In this case, a texture mirrored once with 0 as the center is acquired. 
				 * @~Japanese
				 * @brief uvの絶対値を[0,1]の範囲にクランプし、フィルタリングが有効になっている場合にテクスチャのエッジを越えてフィルタリングしないようにする。この場合、0を中心に1回ミラーされたテクスチャが得られる。 
				 */
				kTextureAddrModeMirrorClamp,

				/*!
				 * @~English
				 * @brief The border texel is used for the data outside [0,1]. Because the address is not clamped, if uv is outside [0,1], a border texel of up to 100% is acquired as a result of filtering. 
				 * @~Japanese
				 * @brief [0,1]の外側のデータにはボーダーテクセルを使用する。アドレスはクランプされないため、uvが[0,1]の外側の場合には、フィルタリングの結果として最大100％のボーダーテクセルが得られる。 
				 */
				kTextureAddrModeClampFullBorder
			};


			/*!
			 * @~English
			 * @brief Texture filter mode 
			 * @details This is the texture filter mode. 
			 * @~Japanese
			 * @brief テクスチャフィルタモード 
			 * @details テクスチャフィルタモードです。 
			 */
			enum TextureFilter {
				/*!
				 * @~English
				 * @brief Point sampling 
				 * @~Japanese
				 * @brief ポイントサンプリング 
				 */
				kTextureFilterPoint,

				/*!
				 * @~English
				 * @brief Linear filtering 
				 * @~Japanese
				 * @brief リニアフィルタリング 
				 */
				kTextureFilterLinear
			};


			/*!
			 * @~English
			 * @brief Mip map filter mode 
			 * @details This is the mip map filter mode. 
			 * @~Japanese
			 * @brief ミップマップフィルタモード 
			 * @details ミップマップフィルタモードです。 
			 */
			enum TextureMipFilter {
				/*!
				 * @~English
				 * @brief Filtering between mip maps is not executed. 
				 * @~Japanese
				 * @brief ミップマップ間のフィルタリングを行わない。 
				 */
				kTextureMipFilterDisabled,

				/*!
				 * @~English
				 * @brief Filtering between mip maps is executed. 
				 * @~Japanese
				 * @brief ミップマップ間のフィルタリングを行う。 
				 */
				kTextureMipFilterEnabled
			};


			/*!
			 * @~English
			 * @brief Program type 
			 * @details This is a program type. 
			 * @~Japanese
			 * @brief プログラムの種類 
			 * @details プログラムの種類です。 
			 */
			enum ProgramType 
			{
				/*!
				 * @~English
				 * @brief The shader is a vertex program. 
				 * @~Japanese
				 * @brief シェーダは頂点プログラムである。 
				 */
				kProgramTypeVertex,

				/*!
				 * @~English
				 * @brief The shader is a fragment program. 
				 * @~Japanese
				 * @brief シェーダはフラグメントプログラムである。 
				 */
				 kProgramTypeFragment
#ifdef _SCE_SAMPLE_UTIL_ENABLE_COMPUTE_SHADER
				,
				/*!
				 * @~English
				 * @brief The shader is a compute program. 
				 * @~Japanese
				 * @brief シェーダはコンピュートプログラムである。 
				 */
				kProgramTypeCompute
#endif

			};


			/*!
			 * @~English
			 * @brief Depth comparison function 
			 * @details This is the depth comparison function. 
			 * @~Japanese
			 * @brief デプス比較関数 
			 * @details デプス比較関数です。 
			 */
			enum DepthFunc
			{
				/*!
				 * @~English
				 * @brief This does not pass. 
				 * @~Japanese
				 * @brief パスしない。 
				 */
				kDepthFuncNever         ,

				/*!
				 * @~English
				 * @brief Passes if the input depth is smaller than the saved depth. 
				 * @~Japanese
				 * @brief 入力デプスが保存されているデプス未満の場合にパスする。 
				 */
				kDepthFuncLess          ,

				/*!
				 * @~English
				 * @brief Passes if the input depth equals to the saved depth. 
				 * @~Japanese
				 * @brief 入力デプスが保存されているデプスに等しい場合にパスする。 
				 */
				kDepthFuncEqual         ,

				/*!
				 * @~English
				 * @brief Passes if the input depth equals to or smaller than the saved depth. 
				 * @~Japanese
				 * @brief 入力デプスが保存されているデプス以下の場合にパスする。 
				 */
				kDepthFuncLessEqual   ,

				/*!
				 * @~English
				 * @brief Passes if the input depth is greater than the saved depth. 
				 * @~Japanese
				 * @brief 入力デプスが保存されているデプスより大きい場合にパスする。 
				 */
				kDepthFuncGreater       ,

				/*!
				 * @~English
				 * @brief Passes if the input depth is different from the saved depth. 
				 * @~Japanese
				 * @brief 入力デプスが保存されているデプスと異なる場合にパスする。 
				 */
				kDepthFuncNotEqual     ,

				/*!
				 * @~English
				 * @brief (1 - A_d, 1 - A_d, 1 - A_d, 1 - A_d) 
				 * @~Japanese
				 * @brief (1 - A_d, 1 - A_d, 1 - A_d, 1 - A_d) 
				 */
				kDepthFuncGreaterEqual ,

				/*!
				 * @~English
				 * @brief Always passes. 
				 * @~Japanese
				 * @brief 常にパスする。 
				 */
				kDepthFuncAlways        
			};


			/*!
			 * @~English
			 * @brief The category of a shader program parameter 
			 * @details This is the category of a shader program parameter. 
			 * @~Japanese
			 * @brief シェーダプログラムパラメータのカテゴリ 
			 * @details シェーダプログラムパラメータのカテゴリです。 
			 */
			enum ParameterCategory 
			{
				/*!
				 * @~English
				 * @brief The parameter is vertex attribute. 
				 * @~Japanese
				 * @brief パラメータは、頂点属性 
				 */
				kParameterCategoryAttribute,
				

				/*!
				 * @~English
				 * @brief The parameter is uniform. 
				 * @~Japanese
				 * @brief パラメータは、ユニフォーム 
				 */
				kParameterCategoryUniform,

				/*!
				 * @~English
				 * @brief The parameter is sampler. 
				 * @~Japanese
				 * @brief パラメータは、サンプラ 
				 */
				kParameterCategorySampler,

				/*!
				 * @~English
				 * @brief The parameter is uniform buffer. 
				 * @~Japanese
				 * @brief パラメータは、ユニフォームバッファ 
				 */
				kParameterCategoryUnknown
			};

			/*!
			 * @~English
			 * @brief The datatype of a program parameter. 
			 * @details This is the datatype of a program parameter. 
			 * @~Japanese
			 * @brief プログラムパラメータのデータ型 
			 * @details プログラムパラメータのデータ型です。 
			 */
			enum ParameterType 
			{
				/*!
				 * @~English
				 * @brief The 32-bit floating-point number corresponding to the "float" type of Cg. 
				 * @~Japanese
				 * @brief Cgの「float」型に相当する32ビットの浮動小数点数 
				 */
				kParameterTypeF32,

				/*!
				 * @~English
				 * @brief The 16-bit floating-point number corresponding to the "half" type of Cg. 
				 * @~Japanese
				 * @brief Cgの「half」型に相当する16ビットの浮動小数点数 
				 */
				kParameterTypeF16,

				/*!
				 * @~English
				 * @brief The 10-bit fixed-point number corresponding to the "fixed" type of Cg. 
				 * @~Japanese
				 * @brief Cgの「fixed」型に相当する10ビットの固定小数点数 
				 */
				kParameterTypeC10,

				/*!
				 * @~English
				 * @brief 32-bit unsigned integer 
				 * @~Japanese
				 * @brief 32ビット符号なし整数 
				 */
				kParameterTypeU32,

				/*!
				 * @~English
				 * @brief 32-bit signed integer 
				 * @~Japanese
				 * @brief 32ビット符号付き整数 
				 */
				kParameterTypeS32,

				/*!
				 * @~English
				 * @brief 16-bit unsigned integer 
				 * @~Japanese
				 * @brief 16ビット符号なし整数 
				 */
				kParameterTypeU16,

				/*!
				 * @~English
				 * @brief 16-bit signed integer 
				 * @~Japanese
				 * @brief 16ビット符号付き整数 
				 */
				kParameterTypeS16,

				/*!
				 * @~English
				 * @brief 8-bit unsigned integer 
				 * @~Japanese
				 * @brief 8ビット符号なし整数 
				 */
				kParameterTypeU8,

				/*!
				 * @~English
				 * @brief 8-bit signed integer 
				 * @~Japanese
				 * @brief 8ビット符号付き整数 
				 */
				kParameterTypeS8,

				/*!
				 * @~English
				 * @brief Texture sampler 
				 * @~Japanese
				 * @brief テクスチャサンプラー 
				 */
				kParameterTypeSampler,

				/*!
				 * @~English
				 * @brief Unknown type 
				 * @~Japanese
				 * @brief 不明な型 
				 */
				kParameterTypeUnknown
			};

			/*!
			 * @~English
			 * @brief Back-face culling mode 
			 * @details This is the back-face culling mode. 
			 * @~Japanese
			 * @brief バックフェースカリングモード 
			 * @details バックフェースカリングモードです。 
			 */
			enum CullMode
			{
				/*!
				 * @~English
				 * @brief No culling. 
				 * @~Japanese
				 * @brief カリングなし。 
				 */
				kCullNone,

				/*!
				 * @~English
				 * @brief Culls a triangle of clockwise rotation in the window coordinate. 
				 * @~Japanese
				 * @brief ウィンドウ座標で時計回りの三角形をカリングする。 
				 */
				 kCullCw,

				/*!
				 * @~English
				 * @brief Culls a triangle of counterclockwise rotation in the window coordinate. 
				 * @~Japanese
				 * @brief ウィンドウ座標で反時計回りの三角形をカリングする。 
				 */
				kCullCcw

			};


			/*!
			 * @~English
			 * @brief Primitive type 
			 * @details This is a primitive type. 
			 * @~Japanese
			 * @brief プリミティブタイプ 
			 * @details プリミティブタイプです。 
			 */
			enum Primitive
			{
				/*!
				 * @~English
				 * @brief Indexed point list 
				 * @~Japanese
				 * @brief インデックス付きポイントリスト 
				 */
				kPrimitivePoints,

				/*!
				 * @~English
				 * @brief Indexed line list 
				 * @~Japanese
				 * @brief インデックス付きラインリスト 
				 */
				kPrimitiveLines,

				/*!
				 * @~English
				 * @brief Indexed triangle list 
				 * @~Japanese
				 * @brief インデックス付きトライアングルリスト 
				 */
				kPrimitiveTriangles,

				/*!
				 * @~English
				 * @brief Indexed triangle strip 
				 * @~Japanese
				 * @brief インデックス付きトライアングルストリップ 
				 */
				kPrimitiveTriangleStrip
			};


			/*!
			 * @~English
			 * @brief The type of an index source for indexing of a vertex stream 
			 * @details This is the type of an index source for indexing of vertex stream. 
			 * @~Japanese
			 * @brief 頂点ストリームのインデックシング用のインデックスソースの種類 
			 * @details 頂点ストリームのインデックシング用のインデックスソースの種類です。 
			 */
			enum IndexSource
			{
				/*!
				 * @~English
				 * @brief The type of an index stream 
				 * @details The stream is indexed using index values. All values have to be lower than 64K. However, the U16 and U32 formats can also be used in memory. 
				 * @~Japanese
				 * @brief インデックスストリームのタイプ 
				 * @details ストリームはインデックス値を使ってインデックス化される。値はすべて64K 未満である必要がある。ただし、メモリ上ではU16 やU32 フォーマットを利用することもできる。 
				 */
				kIndexSourceIndex16Bit     ,

				/*!
				 * @~English
				 * @brief The stream is indexed using index values. 
				 * @~Japanese
				 * @brief ストリームはインデックス値を使ってインデックス化される。 
				 */
				kIndexSourceIndex32Bit	 ,

				/*!
				 * @~English
				 * @brief The stream is indexed using instance numbers. This can only be used for a drawing call including 64 K or less instances. 
				 * @~Japanese
				 * @brief ストリームはインスタンス番号を使ってインデックス化される。64K個以内のインスタンスを含む描画コールでしか使用できない。 
				 */
				 kIndexSourceInstance16Bit	 ,

				/*!
				 * @~English
				 * @brief The stream is indexed using instance numbers. 
				 * @~Japanese
				 * @brief ストリームはインスタンス番号を使ってインデックス化される。 
				 */
				kIndexSourceInstance32Bit	 
			};


			/*!
			 * @~English
			 * @brief Stencil comparison function 
			 * @details This is the stencil comparison function. 
			 * @~Japanese
			 * @brief ステンシル比較関数 
			 * @details ステンシル比較関数です。 
			 */
			enum StencilFunc {
				/*!
				 * @~English
				 * @brief This does not pass. 
				 * @~Japanese
				 * @brief パスしない。 
				 */
				kStencilFuncNever,

				/*!
				 * @~English
				 * @brief Passes if (reference & mask) is smaller than (stencil & mask). 
				 * @~Japanese
				 * @brief (参照&マスク)が(ステンシル&マスク)より小さい場合にパスする。 
				 */
				kStencilFuncLess,

				/*!
				 * @~English
				 * @brief Passes if (reference & mask) equals to (stencil & mask). 
				 * @~Japanese
				 * @brief (参照&マスク)が(ステンシル&マスク)と等しい場合にパスする。 
				 */
				kStencilFuncEqual,

				/*!
				 * @~English
				 * @brief Passes if (reference & mask) equals to or smaller than (stencil & mask). 
				 * @~Japanese
				 * @brief (参照&マスク)が(ステンシル&マスク)以下の場合にパスする。 
				 */
				kStencilFuncLessEqual,

				/*!
				 * @~English
				 * @brief Passes if (reference & mask) is greater than (stencil & mask). 
				 * @~Japanese
				 * @brief (参照&マスク)が(ステンシル&マスク)より大きい場合にパスする。 
				 */
				kStencilFuncGreater,

				/*!
				 * @~English
				 * @brief Passes if (reference & mask) does not equal to (stencil & mask). 
				 * @~Japanese
				 * @brief (参照&マスク)が(ステンシル&マスク)と等しくない場合にパスする。 
				 */
				kStencilFuncNotEqual,

				/*!
				 * @~English
				 * @brief Passes if (reference & mask) equals to or greater than (stencil & mask). 
				 * @~Japanese
				 * @brief (参照&マスク)が(ステンシル&マスク)以上の場合にパスする。 
				 */
				kStencilFuncGreaterEqual,

				/*!
				 * @~English
				 * @brief Always passes. 
				 * @~Japanese
				 * @brief 常にパスする。 
				 */
				kStencilFuncAlways
			};


			/*!
			 * @~English
			 * @brief Stencil operation 
			 * @details This is a stencil operation. 
			 * @~Japanese
			 * @brief ステンシル操作 
			 * @details ステンシル操作です。 
			 */
			enum StencilOp {
				/*!
				 * @~English
				 * @brief Retains the current stencil buffer value. 
				 * @~Japanese
				 * @brief 現在のステンシルバッファ値を保持する。 
				 */
				kStencilOpKeep,

				/*!
				 * @~English
				 * @brief Sets the stencil buffer value to 0. 
				 * @~Japanese
				 * @brief ステンシルバッファ値を0に設定する。 
				 */
				kStencilOpZero,

				/*!
				 * @~English
				 * @brief Replaces the value of a stencil buffer with a stencil reference value. 
				 * @~Japanese
				 * @brief ステンシルバッファの値をステンシル参照値に置き換える。 
				 */
				kStencilOpReplace,

				/*!
				 * @~English
				 * @brief Increments the value of a stencil buffer. (Clamped at 255 if it exceeds 255) 
				 * @~Japanese
				 * @brief ステンシルバッファの値をインクリメントする。（255を超えたら255にクランプ） 
				 */
				kStencilOpIncr,

				/*!
				 * @~English
				 * @brief Decrements the value of a stencil buffer. (Clamped at 0 if it exceeds 0) 
				 * @~Japanese
				 * @brief ステンシルバッファの値をデクリメントする。（0を超えたら0にクランプ） 
				 */
				kStencilOpDecr,

				/*!
				 * @~English
				 * @brief Bit-inverts the value of a stencil buffer. 
				 * @~Japanese
				 * @brief ステンシルバッファの値をビット反転させる。 
				 */
				kStencilOpInvert,

				/*!
				 * @~English
				 * @brief Increments the value of a stencil buffer. (Lapped at 0 if it exceeds 255) 
				 * @~Japanese
				 * @brief ステンシルバッファの値をインクリメントする。（255を超えたら0にラップ） 
				 */
				kStencilOpIncrWrap,

				/*!
				 * @~English
				 * @brief Decrements the value of a stencil buffer. (Lapped at 255 if it exceeds 0) 
				 * @~Japanese
				 * @brief ステンシルバッファの値をデクリメントする。（0を超えたら255にラップ） 
				 */
				kStencilOpDecrWrap 
			};


			/*!
			 * @~English
			 * @brief Texture format 
			 * @details This is the texture format. 
			 * @~Japanese
			 * @brief テクスチャフォーマット 
			 * @details テクスチャフォーマットです。 
			 */
			enum TextureFormat
			{
				/*!
				 * @~English
				 * @brief The U8U8U8U8 data is read from memory in the order of ABGR. 
				 * @~Japanese
				 * @brief U8U8U8U8データがメモリからABGRの順番で読み取られる。 
				 */
				kTextureFormatA8B8G8R8Unorm,
#if _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief The U8U8 data is read from memory in the order of GR. 
				 * @~Japanese
				 * @brief U8U8データがメモリからGRの順番で読み取られる。 
				 */
				kTextureFormatG8R8Unorm,
#endif
				/*!
				 * @~English
				 * @brief The U8 data is read from memory as R. 
				 * @~Japanese
				 * @brief U8データがメモリからRとして読み取られる。 
				 */
				kTextureFormatR8Unorm
#if _SCE_TARGET_OS_ORBIS
				,

				/*!
				 * @~English
				 * @brief The U16U16U16U16 data is read from memory in the order of ABGR. 
				 * @~Japanese
				 * @brief U16U16U16U16データがメモリからABGRの順番で読み取られる。 
				 */
				kTextureFormatA16B16G16R16Unorm,
				/*!
				 * @~English
				 * @brief The U16U16 data is read from memory in the order of GR. 
				 * @~Japanese
				 * @brief U16U16データがメモリからGRの順番で読み取られる。 
				 */
				kTextureFormatG16R16Unorm,
				/*!
				 * @~English
				 * @brief The U16 data is read from memory in the order of R. 
				 * @~Japanese
				 * @brief U16データがメモリからRの順番で読み取られる。 
				 */
				kTextureFormatR16Unorm
#endif
			};



			/*!
			 * @~English
			 * 
			 * @brief Color mask for shader code patching of a runtime by the shader patcher 
			 * @details This is the color mask for shader code patching of a runtime by the shader patcher. 
			 * @~Japanese
			 * 
			 * @brief シェーダパッチャによるランタイムのシェーダコードパッチング用のカラーマスク 
			 * @details シェーダパッチャによるランタイムのシェーダコードパッチング用のカラーマスクです。 
			 */
			enum ColorMask {
				/*!
				 * @~English
				 * @brief Not written in any channels. 
				 * @~Japanese
				 * @brief どのチャンネルにも書き込まない。 
				 */
				kColorMaskNone	= 0,

				/*!
				 * @~English
				 * @brief Written in the alpha channel. 
				 * @~Japanese
				 * @brief アルファチャンネルに書き込まれる。 
				 */
				kColorMaskA	= (1 << 0),

				/*!
				 * @~English
				 * @brief Written in the red channel. 
				 * @~Japanese
				 * @brief 赤色チャンネルに書き込まれる。 
				 */
				 kColorMaskR	= (1 << 1),

				/*!
				 * @~English
				 * @brief Written in the green channel. 
				 * @~Japanese
				 * @brief 緑色チャンネルに書き込まれる。 
				 */
				kColorMaskG	= (1 << 2),

				/*!
				 * @~English
				 * @brief Written in the blue channel. 
				 * @~Japanese
				 * @brief 青色チャンネルに書き込まれる。 
				 */
				kColorMaskB	= (1 << 3),

				/*!
				 * @~English
				 * @brief Written in all channels. 
				 * @~Japanese
				 * @brief 全チャンネルに書き込まれる。 
				 */
				kColorMaskAll	= (kColorMaskA | kColorMaskB | kColorMaskG | kColorMaskR)
			};


			/*!
			 * @~English
			 * 
			 * @brief Blend function for shader code patching of a runtime by the shader patcher 
			 * @details This is the blend function for shader code patching of a runtime by the shader patcher. 
			 * @~Japanese
			 * 
			 * @brief シェーダパッチャによるランタイムのシェーダコードパッチング用のブレンド関数 
			 * @details シェーダパッチャによるランタイムのシェーダコードパッチング用のブレンド関数です。 
			 */
			enum BlendFunc {
				/*!
				 * @~English
				 * @brief D = S 
				 * @~Japanese
				 * @brief D = S 
				 */
				kBlendFuncNone,

				/*!
				 * @~English
				 * @brief D = S * Coefficient S + D * Coefficient D 
				 * @~Japanese
				 * @brief D = S * 係数S + D * 係数D 
				 */
				kBlendFuncAdd,

				/*!
				 * @~English
				 * @brief D = S * Coefficient S - D * Coefficient D 
				 * @~Japanese
				 * @brief D = S * 係数S - D * 係数D 
				 */
				kBlendFuncSubtract,

				/*!
				 * @~English
				 * @brief D = D * Coefficient D - S * Coefficient S 
				 * @~Japanese
				 * @brief D = D * 係数D - S * 係数S 
				 */
				kBlendFuncReverseSubtract,

				/*!
				 * @~English
				 * @brief D = min(S, D) 
				 * @~Japanese
				 * @brief D = min(S, D) 
				 */
				kBlendFuncMin,

				/*!
				 * @~English
				 * @brief D = max(S, D) 
				 * @~Japanese
				 * @brief D = max(S, D) 
				 */
				kBlendFuncMax
			} ;


			/*!
			 * @~English
			 * 
			 * @brief The blend coefficient for shader code patching of a runtime by the shader patcher 
			 * @details This is the blend coefficient for shader code patching of a runtime by the shader patcher. 
			 * @~Japanese
			 * 
			 * @brief シェーダパッチャによるランタイムのシェーダコードパッチング用のブレンド係数 
			 * @details シェーダパッチャによるランタイムのシェーダコードパッチング用のブレンド係数です。 
			 */
			enum BlendFactor {
				/*!
				 * @~English
				 * @brief (0, 0, 0, 0) 
				 * @~Japanese
				 * @brief (0, 0, 0, 0) 
				 */
				kBlendFactorZero,

				/*!
				 * @~English
				 * @brief (1, 1, 1, 1) 
				 * @~Japanese
				 * @brief (1, 1, 1, 1) 
				 */
				kBlendFactorOne,

				/*!
				 * @~English
				 * @brief (R_s, G_s, B_s, A_s) 
				 * @~Japanese
				 * @brief (R_s, G_s, B_s, A_s) 
				 */
				kBlendFactorSrcColor,

				/*!
				 * @~English
				 * @brief (1 - R_s, 1 - G_s, 1 - B_s, 1 - A_s) 
				 * @~Japanese
				 * @brief (1 - R_s, 1 - G_s, 1 - B_s, 1 - A_s) 
				 */
				kBlendFactorOneMinusSrcColor,

				/*!
				 * @~English
				 * @brief (A_s, A_s, A_s, A_s) 
				 * @~Japanese
				 * @brief (A_s, A_s, A_s, A_s) 
				 */
				kBlendFactorSrcAlpha,

				/*!
				 * @~English
				 * @brief (1 - A_s, 1 - A_s, 1 - A_s, 1 - A_s) 
				 * @~Japanese
				 * @brief (1 - A_s, 1 - A_s, 1 - A_s, 1 - A_s) 
				 */
				kBlendFactorOneMinusSrcAlpha,

				/*!
				 * @~English
				 * @brief (R_d, G_d, B_d, A_d) 
				 * @~Japanese
				 * @brief (R_d, G_d, B_d, A_d) 
				 */
				kBlendFactorDstColor,

				/*!
				 * @~English
				 * @brief (1 - R_d, 1 - G_d, 1 - B_d, 1 - A_d) 
				 * @~Japanese
				 * @brief (1 - R_d, 1 - G_d, 1 - B_d, 1 - A_d) 
				 */
				kBlendFactorOneMinusDstColor,

				/*!
				 * @~English
				 * @brief (A_d, A_d, A_d, A_d) 
				 * @~Japanese
				 * @brief (A_d, A_d, A_d, A_d) 
				 */
				kBlendFactorDstAlpha,

				/*!
				 * @~English
				 * @brief (1 - A_d, 1 - A_d, 1 - A_d, 1 - A_d) 
				 * @~Japanese
				 * @brief (1 - A_d, 1 - A_d, 1 - A_d, 1 - A_d) 
				 */
				kBlendFactorOneMinusDstAlpha
			} ;


			/*!
			 * @~English
			 * 
			 * @brief The description about blending and masking to the color surface of a shader result 
			 * @details This is the description about blending and masking to the color surface of a shader result. 
			 * @~Japanese
			 * 
			 * @brief シェーダ結果のカラーサーフェスへのブレンディングおよびマスキングについての記述 
			 * @details シェーダ結果のカラーサーフェスへのブレンディングおよびマスキングについての記述です。 
			 */
			struct  BlendInfo
			{
				/*!
				 * @~English
				 * @brief The bit field mask using the value of SceGxmColorMask 
				 * @~Japanese
				 * @brief SceGxmColorMaskの値を使ったビットフィールドマスク 
				 */
				uint8_t	colorMask;

				/*!
				 * @~English
				 * @brief Color blend function (From SceGxmBlendFunc) 
				 * @~Japanese
				 * @brief カラーブレンド関数（SceGxmBlendFuncより） 
				 */
				uint8_t	colorFunc;

				/*!
				 * @~English
				 * @brief Alpha blend function (From SceGxmBlendFunc) 
				 * @~Japanese
				 * @brief アルファブレンド関数（SceGxmBlendFuncより） 
				 */
				uint8_t	alphaFunc;

				/*!
				 * @~English
				 * @brief Source color blend coefficient (From SceGxmBlendFactor) 
				 * @~Japanese
				 * @brief ソースカラーブレンド係数（SceGxmBlendFactorより） 
				 */
				uint8_t	colorSrc;

				/*!
				 * @~English
				 * @brief Destination color blend coefficient (From SceGxmBlendFactor) 
				 * @~Japanese
				 * @brief デスティネーションカラーブレンド係数（SceGxmBlendFactorより） 
				 */
				uint8_t	colorDst;

				/*!
				 * @~English
				 * @brief Source alpha blend coefficient (From SceGxmBlendFactor) 
				 * @~Japanese
				 * @brief ソースアルファブレンド係数（SceGxmBlendFactorより） 
				 */
				uint8_t	alphaSrc;

				/*!
				 * @~English
				 * @brief Destination alpha blend coefficient (From SceGxmBlendFactor) 
				 * @~Japanese
				 * @brief デスティネーションアルファブレンド係数（SceGxmBlendFactorより） 
				 */
				uint8_t	alphaDst;

				/*!
				 * @~English
				 * 
				 * @brief Initializes by settings without blending. 
				 * 
				 * @details This initializes by settings without blending. 
				 * @~Japanese
				 * 
				 * @brief ブレンドしない設定で初期化する 
				 * 
				 * @details ブレンドしない設定で初期化します。 
				 */
				void initializeAsNoBlend(void)
				{
					colorMask = kColorMaskAll;
					colorFunc = kBlendFuncNone;
					alphaFunc = kBlendFuncNone;
					colorSrc = kBlendFactorOne;
					colorDst = kBlendFactorZero;
					alphaSrc = kBlendFactorOne;
					alphaDst = kBlendFactorZero;
				}

				/*!
				 * @~English
				 * 
				 * @brief Initializes by settings with alpha blending. 
				 * 
				 * @details This initializes by settings with alpha blending. 
				 * @~Japanese
				 * 
				 * @brief アルファブレンドを行う設定で初期化する 
				 * 
				 * @details アルファブレンドを行う設定で初期化します。 
				 */
				void initializeAsAlphaBlend(void)
				{
					colorMask = kColorMaskAll;
					colorFunc = kBlendFuncAdd;
					alphaFunc = kBlendFuncAdd;
					colorSrc = kBlendFactorSrcAlpha;
					colorDst = kBlendFactorOneMinusSrcAlpha;
					alphaSrc = kBlendFactorOne;
					alphaDst = kBlendFactorZero;
				}

				/*!
				 * @~English
				 * 
				 * @brief Initializes by settings with additive blending. 
				 * 
				 * @details This initializes by settings with additive blending. 
				 * @~Japanese
				 * 
				 * @brief 加算ブレンドを行う設定で初期化する 
				 * 
				 * @details 加算ブレンドを行う設定で初期化します。 
				 */
				void initializeAsAddBlend(void)
				{
					colorMask = kColorMaskAll;
					colorFunc = kBlendFuncAdd;
					alphaFunc = kBlendFuncAdd;
					colorSrc =  kBlendFactorSrcAlpha;
					colorDst =  kBlendFactorOne;
					alphaSrc =  kBlendFactorSrcAlpha;
					alphaDst =  kBlendFactorOne;

				}

			};


			/*!
			 * @~English
			 * @brief Shader format 
			 * @details This enumerated type represents the shader format. It is passed together with shader data upon creating a shader object. 
			 * @~Japanese
			 * @brief シェーダフォーマット 
			 * @details シェーダのフォーマット。シェーダーオブジェクトを作る際にシェーダデータと共に渡します。 
			 */
			enum ShaderFormat
			{
#if _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief PSSL binary code (can only be used on PlayStation(R)4) 
				 * @~Japanese
				 * @brief PSSLのバイナリコード(PlayStation(R)4でのみ使用可) 
				 */
				kShaderFormatPsslBinary,
#elif  _SCE_TARGET_OS_PSP2
				/*!
				 * @~English
				 * @brief PlayStation(R)Vita GXP format (can only be used on PlayStation(R)Vita) 
				 * @~Japanese
				 * @brief PlayStation(R)VitaのGXPフォーマット(PlayStation(R)Vitaでのみ使用可) 
				 */
				kShaderFormatGxp,
#elif  _SCE_TARGET_OS_WINDOWS
				/*!
				 * @~English
				 * @brief Text format of the Cg program 
				 * @~Japanese
				 * @brief Cgプログラムのテキスト形式 
				 */
				kShaderFormatCgSource,
				/*!
				 * @~English
				 * @brief Binary format of the Cg program 
				 * @~Japanese
				 * @brief Cgプログラムのバイナリ形式 
				 */
				kShaderFormatCgBinary,
				/*!
				 * @~English
				 * @brief Text format of the HLSL program 
				 * @~Japanese
				 * @brief HLSLプログラムのテキスト形式 
				 */
				kShaderFormatHlslSource,
#endif
				/*!
				 * @~English
				 * @brief Default format on each platform. PlayStation(R)4 is kShaderFormatPsslBinary, PlayStation(R)Vita is kShaderFormatGxp, Windows is kShaderFormatCgSource 
				 * @~Japanese
				 * @brief 各プラットフォームのデフォルトの形式。PlayStation(R)4はkShaderFormatPsslBinary, PlayStation(R)VitaはkShaderFormatGxp, WindowsはkShaderFormatCgSource 
				 */
				kShaderFormatDefault
			};
			
			/*!
			 * @~English
			 * @brief Buffer flag 
			 * @details This enumerated type represents flags to indicate how a buffer is to be used in the graphics pipeline 
			 * @~Japanese
			 * @brief バッファのフラグ 
			 * @details グラフィックスパイプラインにおけるバッファの使用方法を示すフラグ 
			 */
			enum BufferBindFlag 
				{
					/*!
					 * @~English
					 * @brief Buffer used as a shader resource such as, a texture buffer or structured buffer 
					 * @~Japanese
					 * @brief テクスチャや構造化バッファーなど、シェーダリソースとして使われるバッファ 
					 */
					kBufferBindFlagShaderResource  = 1<<0,
					/*!
					 * @~English
					 * @brief Buffer used as a render target 
					 * @~Japanese
					 * @brief レンダーターゲットとして使用されるバッファ 
					 */
					kBufferBindFlagRenderTarget    = 1<<1,
					/*!
					 * @~English
					 * @brief Buffer used as a depth stencil target 
					 * @~Japanese
					 * @brief デプスステンシルターゲットとして使用されるバッファ 
					 */
					kBufferBindFlagDepthStencil    = 1<<2,
					/*!
					 * @~English
					 * @brief Buffer used as a vertex buffer 
					 * @~Japanese
					 * @brief 頂点バッファとして使用されるバッファ 
					 */
					kBufferBindFlagVertexBuffer    = 1<<3,
					/*!
					 * @~English
					 * @brief Buffer used as an index buffer 
					 * @~Japanese
					 * @brief インデックスバッファとして使用されるバッファ 
					 */
					kBufferBindFlagIndexBuffer     = 1<<4,
					/*!
					 * @~English
					 * @brief Buffer used as a constant buffer 
					 * @~Japanese
					 * @brief 定数バッファとして使用されるバッファ 
					 */
					kBufferBindFlagConstantBuffer  = 1<<5,
					/*!
					 * @~English
					 * @brief Buffer used as an unordered access buffer 
					 * @~Japanese
					 * @brief アンオーダードアクセスバッファとして使用されるバッファ 
					 */
					kBufferBindFlagUnorderedAccess = 1<<6
				};
			
			/*!
			 * @~English
			 * @brief Buffer access mode 
			 * @details This enumerated type represents the buffer access mode. 
			 * @~Japanese
			 * @brief バッファのアクセスモード 
			 * @details バッファのアクセスモードです。 
			 */
			enum BufferAccessMode
				{
					/*!
					 * @~English
					 * @brief Read/write from the GPU enabled 
					 * @~Japanese
					 * @brief GPUからの読み書きのみ可 
					 */
					kBufferAccessModeGpuReadWrite,
					/*!
					 * @~English
					 * @brief Only read from the GPU enabled 
					 * @~Japanese
					 * @brief GPUからの読みこみのみ可 
					 */
					kBufferAccessModeGpuReadOnly,
					/*!
					 * @~English
					 * @brief Read from the GPU and write from the CPU enabled 
					 * @~Japanese
					 * @brief GPUからの読みこみとCPUからの書き込みのみ可 
					 */
					kBufferAccessModeGpuReadCpuWrite
				};
			/*!
			 * @~English
			 * @brief Buffer element format 
			 * @details This enumerated type represents the buffer element format. 
			 * @~Japanese
			 * @brief バッファの要素のフォーマット 
			 * @details バッファの要素のフォーマットです。 
			 */
			enum  BufferFormat
				{
					/*!
					 * @~English
					 * @brief Unknown format 
					 * @~Japanese
					 * @brief 未知のフォーマット 
					 */
					kBufferFormatUnknown,
					/*!
					 * @~English
					 * @brief Three 8-bit values 
					 * @~Japanese
					 * @brief 3つの8bitの値 
					 */
					kBufferFormatX8X8X8X8,
					/*!
					 * @~English
					 * @brief Two 8-bit values 
					 * @~Japanese
					 * @brief 2つの8bitの値 
					 */
					kBufferFormatX8X8,
					/*!
					 * @~English
					 * @brief One 8-bit value 
					 * @~Japanese
					 * @brief 1つの8bitの値 
					 */
					kBufferFormatX8,
					/*!
					 * @~English
					 * @brief Four 16-bit values 
					 * @~Japanese
					 * @brief 4つの16bitの値 
					 */
					kBufferFormatX16X16X16X16,
					/*!
					 * @~English
					 * @brief Two 16-bit values 
					 * @~Japanese
					 * @brief 2つの16bitの値 
					 */
					kBufferFormatX16X16,
					/*!
					 * @~English
					 * @brief One 16-bit value 
					 * @~Japanese
					 * @brief 1つの16bitの値 
					 */
					kBufferFormatX16,
					/*!
					 * @~English
					 * @brief One 32-bit value, one 8-bit value, and one 24-bit value 
					 * @~Japanese
					 * @brief 1つの32bitの値と、1つの8bitの値と、1つの24bitの値 
					 */
					kBufferFormatX32X8X24,
					/*!
					 * @~English
					 * @brief One 32-bit value 
					 * @~Japanese
					 * @brief 1つの32bitの値 
					 */
					kBufferFormatX32,
					/*!
					 * @~English
					 * @brief One 24-bit value and one 8-bit value 
					 * @~Japanese
					 * @brief 1つの24bitの値、1つの8bitの値 
					 */
					kBufferFormatX24X8
				};

			/*!
			 * @~English
			 * @brief Buffer dimension 
			 * @details This enumerated type represents the buffer dimension. 
			 * @~Japanese
			 * @brief バッファの次元 
			 * @details バッファの次元です。 
			 */
			enum BufferDimension
				{
					/*!
					 * @~English
					 * @brief One-dimensional number 
					 * @~Japanese
					 * @brief 1次元の数値 
					 */
					kBufferDimensionBuffer,
					/*!
					 * @~English
					 * @brief One-dimensional texture 
					 * @~Japanese
					 * @brief 1次元のテクスチャ 
					 */
					kBufferDimensionTexture1D,
					/*!
					 * @~English
					 * @brief Two-dimensional texture 
					 * @~Japanese
					 * @brief 2次元のテクスチャ 
					 */
					kBufferDimensionTexture2D,
					/*!
					 * @~English
					 * @brief Three-dimensional texture 
					 * @~Japanese
					 * @brief 3次元のテクスチャ 
					 */
					kBufferDimensionTexture3D
				};
			

		}
	}

}

#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_H */
