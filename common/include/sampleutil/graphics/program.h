/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/



#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PROGRAM_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PROGRAM_H

#include <scebase_common.h>
#include <sampleutil/graphics/constant.h>


namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			/*!
			 * @~English
			 * @brief Parameter class of a shader 
			 * @details This is the parameter class of a shader. 
			 * @~Japanese
			 * @brief シェーダのパラメータクラス 
			 * @details シェーダのパラメータクラスです。 
			 */
			class SCE_SAMPLE_UTIL_API Parameter
			{
			public:
				virtual ~Parameter(void){}
				/*!
				 * @~English
				 * @brief Returns the semantic of an attribute parameter. 
				 * @details This returns the semantic of an attribute parameter. 
				 * @return Semantic of an attribute parameter. In the case of other than an attribute parameter, PARAMETER_SEMANTIC_NONE is returned. 
				 * @~Japanese
				 * @brief アトリビュートパラメータのセマンティックを返す 
				 * @details アトリビュートパラメータのセマンティックを返します。 
				 * @return アトリビュートパラメータのセマンティック。アトリビュートパラメータ以外の場合はPARAMETER_SEMANTIC_NONEが返ります。 
				 */
				virtual ParameterSemantic getSemantic(void) const = 0;

				/*!
				 * @~English
				 * @brief Gets the number of the scalar components of each array element. 
				 * @details This gets the number of the scalar components of each array element. 
				 * @return Number of the scalar components of each array element 
				 * @~Japanese
				 * @brief 各配列要素のスカラーコンポーネントの数を取得する 
				 * @details 各配列要素のスカラーコンポーネントの数を取得します。 
				 * @return 各配列要素のスカラーコンポーネントの数 
				 */
				virtual uint32_t getComponentCount(void) const = 0;
				/*!
				 * @~English
				 * 
				 * @brief Gets the size of an array. 
				 * @details This gets the size of an array. 
				 * @return Size of an array 
				 * @~Japanese
				 * 
				 * @brief 配列のサイズを取得する 
				 * @details 配列のサイズを取得します。 
				 * @return 配列のサイズ 
				 */
				virtual uint32_t getArraySize(void) const = 0;

				/*!
				 * @~English
				 * @brief Confirms the use of the default uniform buffer. 
				 * @details This confirms whether the default uniform buffer is used. 
				 * @return true if used, and false if not. 
				 * 
				 * @~Japanese
				 * @brief デフォルトユニフォームバッファの使用を確認する 
				 * @details デフォルトユニフォームバッファを使用するかどうかを確認します。 
				 * @return 使用する場合はtrue, そうでない場合はfalse。 
				 * 
				 */
				virtual bool isUseDefaultBuf(void) const = 0;
				/*!
				 * @~English
				 * @brief Gets a resource index. 
				 * 
				 * @details This returns a different value according to the category of a parameter. - Attribute: Register index - Uniform: Word (32-bit) offset from the head of a buffer - Sampler: Texture unit index 
				 * @return Resource index 
				 * 
				 * @~Japanese
				 * @brief リソースインデックスの取得 
				 * 
				 * @details パラメータのカテゴリに応じて異なる値を返します。- 属性：レジスタインデックス- ユニフォーム：バッファの先頭からのワード（32ビット）オフセット- サンプラ：テクスチャユニットインデックス 
				 * @return リソースインデックス 
				 * 
				 */
				virtual uint32_t getResourceIndex(void) const = 0;
				/*!
				 * @~English
				 * @brief Gets a container index. 
				 * 
				 * @details In the case of a uniform buffer, the index of uniform buffers is returned. As for other parameters, the returned values are not determined. 
				 * @return Container index 
				 * 
				 * @~Japanese
				 * @brief コンテナインデックスの取得 
				 * 
				 * @details ユニフォームバッファの場合はユニフォームバッファのインデックスを返します。それ以外のパラメータに関しては返り値は不定です。 
				 * @return コンテナインデックス 
				 * 
				 */
				virtual uint32_t getContainerIndex(void) const = 0;

				/*!
				 * @~English
				 * @brief Returns the semantic index of attribute parameters. 
				 * @details  This returns the semantic index of attribute parameters. 
				 * @return Semantic index of attribute parameters. In the case of other than an attribute parameter, 0 is returned. 
				 * @~Japanese
				 * @brief アトリビュートパラメータのセマンティックインデックスを返す 
				 * @details  アトリビュートパラメータのセマンティックインデックスを返します。 
				 * @return アトリビュートパラメータのセマンティックインデックス。アトリビュートパラメータ以外の場合は0が返ります。 
				 */
				virtual uint32_t getSemanticIndex(void) const = 0;

				/*!
				 * @~English
				 * @brief Writes the value of a parameter to the appropriate offset on a uniform buffer. 
				 * @details This writes the value of this parameter to the appropriate offset on a uniform buffer. sourceData requires getComponentCount() * getArraySize() elements. 
				 * @param uniformBuffer The starting address of the buffer of the write destination
				 * @param sourceData The data to be written
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief ユニフォームバッファ上の適切なオフセットへパラメータの値を書き込む 
				 * @details ユニフォームバッファ上の適切なオフセットに、このパラメータの値を書き込みます。sourceData は getComponentCount() * getArraySize() 個の要素が必要です。 
				 * @param uniformBuffer 書き込み先のバッファの先頭アドレス
				 * @param sourceData 書き込むデータ
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				virtual int setUniformDataF(void *uniformBuffer, const float *sourceData) const = 0;

				/*!
				 * @~English
				 * @brief Returns the name of a parameter 
				 * @details This returns the name of a parameter. 
				 * @return Name of a parameter 
				 * @~Japanese
				 * @brief パラメータの名前を返す 
				 * @details パラメータの名前を返します。 
				 * @return パラメータの名前 
				 */
				virtual const char *getName(void) const = 0;

				/*!
				 * @~English
				 * @brief Returns the category of a parameter 
				 * @details This returns the category of a parameter. 
				 * @return Category of a parameter 
				 * @~Japanese
				 * @brief パラメータのカテゴリを返す 
				 * @details パラメータのカテゴリを返します。 
				 * @return パラメータのカテゴリ 
				 */
				virtual ParameterCategory getCategory(void) const = 0;

				/*!
				 * @~English
				 * @brief Confirms whether this parameter is valid 
				 * @details This confirms whether this parameter is valid. 
				 * @return true if valid, and false if not. 
				 * @~Japanese
				 * @brief このパラメータが有効か確認する 
				 * @details このパラメータが有効か確認します。 
				 * @return 有効の場合はtrue, それ以外の場合はfalse。 
				 */
				virtual bool isValid(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the type of a parameter 
				 * @details This returns the format of a parameter. 
				 * @return Format of a parameter 
				 * @~Japanese
				 * @brief パラメータの型を返す 
				 * @details パラメータの型を返します。 
				 * @return パラメータの型 
				 */
				virtual ParameterType getType(void) const = 0;


			};




			/*!
			 * @~English
			 * @brief ID of a shader program 
			 * @details This is the ID of a shader program. 
			 * @~Japanese
			 * @brief シェーダプログラムのID 
			 * @details シェーダプログラムのIDです。 
			 */
			class SCE_SAMPLE_UTIL_API Program : public Resource
			{
			public:
				/*!
				 * @~English
				 * @brief Confirms whether a shader program is valid 
				 * @details This confirms whether a shader program is valid. 
				 * 
				 * @return This returns true if valid, and false if not. 
				 * @~Japanese
				 * @brief シェーダプログラムが有効かどうかを確認する 
				 * @details シェーダプログラムが有効かどうかを確認します。 
				 * 
				 * @return 有効な場合はtrue, そうでなければfalseを返します。 
				 */
				virtual bool isValid(void) const = 0;
				/*!
				 * @~English
				 * @brief Gets the number of parameters 
				 * @details  This gets the number of parameters. 
				 * @return Number of parameters 
				 * @~Japanese
				 * @brief パラメータ数の取得 
				 * @details  パラメータ数を取得します。 
				 * @return パラメータ数 
				 */
				virtual uint32_t getParameterCount(void) const = 0;
				/*!
				 * @~English
				 * @brief Returns the parameter of the specified name 
				 * @details  This returns the parameter of the specified name. 
				 * @param name Parameter name
				 * @return Parameter. If not found, an invalid parameter will be returned. 
				 * @~Japanese
				 * @brief 指定された名前のパラメータを返す 
				 * @details  指定された名前のパラメータを返します。 
				 * @param name パラメータ名
				 * @return パラメータ。見つからなかった場合は不正なパラメータが返ります。 
				 */
				virtual const Parameter* findParameterByName(const char *name) const = 0;
				/*!
				 * @~English
				 * @brief Returns the parameter of the specified semantic 
				 * @details This returns the parameter of the specified semantic. 
				 * @param semantic Semantic of an parameter
				 * @param semanticIndex Semantic index of a parameter
				 * @return Parameter. If not found, an invalid parameter will be returned. 
				 * @~Japanese
				 * @brief 指定されたセマンティックのパラメータを返す 
				 * @details 指定されたセマンティックのパラメータを返します。 
				 * @param semantic パラメータのセマンティック
				 * @param semanticIndex パラメータのセマンティックインデックス
				 * @return パラメータ。見つからなかった場合は不正なパラメータが返ります。 
				 */
				virtual const Parameter* findParameterBySemantic(ParameterSemantic semantic, uint32_t semanticIndex) const = 0;
				/*!
				 * @~English
				 * @brief Returns the parameter of the specified index 
				 * @details This returns the parameter of the specified index. 
				 * @param index Index of parameters
				 * @return Parameter. Returns an invalid parameter if the index is out of range. 
				 * @~Japanese
				 * @brief 指定されたインデックスのパラメータを返す 
				 * @details 指定されたインデックスのパラメータを返します。 
				 * @param index パラメータのインデックス
				 * @return パラメータ。インデックスが範囲外の場合は不正なパラメータが返ります。 
				 */
				virtual const Parameter* getParameter(uint32_t index) const = 0;


				virtual ~Program(void){}
			};




		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PROGRAM_H */
