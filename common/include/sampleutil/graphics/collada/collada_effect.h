/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_EFFECT_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_EFFECT_H

#include <sampleutil/graphics/collada/collada_base.h>


namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Collada
			{
					/*!
					 * @~English
					 * 
					 * @brief Parameter of an effect 
					 * @details This is the parameter of an effect 
					 * @~Japanese
					 * 
					 * @brief エフェクトのパラメータ 
					 * @details エフェクトのパラメータです。 
					 */
					class SCE_SAMPLE_UTIL_API EffectDataParameter
					{
					public:
						/*!
						 * @~English
						 * 
						 * @brief Datatype 
						 * @details This is a datatype. 
						 * @~Japanese
						 * 
						 * @brief データのタイプ 
						 * @details データのタイプです。 
						 */
						enum Type { 
							/*!
							 * @~English
							 * @brief Array of float 
							 * 
							 * @~Japanese
							 * @brief floatの配列 
							 * 
							 */
							kTypeFloatArray,
							/*!
							 * @~English
							 * @brief Image 
							 * 
							 * @~Japanese
							 * @brief イメージ 
							 * 
							 */
							kTypeImage 
						};

						
						/*!
						 * @~English
						 * @brief Gets a parameter name. 
						 * @details This gets a parameter name. 
						 * @return Parameter name 
						 * @~Japanese
						 * @brief パラメータ名を取得する 
						 * @details パラメータ名を取得します。 
						 * @return パラメータ名 
						 */
						std::string getName(void) const
						{
							return m_name;
						}
						/*!
						 * @~English
						 * @brief Gets a parameter type. 
						 * @details This gets a parameter type. 
						 * @return Parameter type 
						 * @~Japanese
						 * @brief パラメータのタイプを取得する 
						 * @details パラメータのタイプを取得します。 
						 * @return パラメータのタイプ 
						 */
						Type getType(void) const
						{
							return m_type;
						}
						/*!
						 * @~English
						 * @brief Gets a parameter value as the array of float 
						 * @details This gets a parameter value as the array of float. 
						 * @return Parameter value. NULL in the case that the type of the parameter is not kTypeFloatArray. 
						 * @~Japanese
						 * @brief パラメータの値をfloatの配列として取得 
						 * @details パラメータの値をfloatの配列として取得します。 
						 * @return パラメータの値。パラメータの型がkTypeFloatArrayでない場合はNULL。 
						 */
						const float *getFloatArray(void) const
						{
							if(m_type != kTypeFloatArray){
								return NULL;
							}
							return m_floatArray; 
						}
						/*!
						 * @~English
						 * @brief Gets the size of the float array of a parameter value 
						 * @details This gets the size of the float array of a parameter value. 
						 * @return Size of the float array of a parameter value. 0 in the case that the type of the parameter is not TYPE_FLOAT_ARRAY 
						 * @~Japanese
						 * @brief パラメータの値のfloat配列のサイズの取得 
						 * @details パラメータの値のfloat配列のサイズを取得します。 
						 * @return パラメータの値のfloat配列のサイズ。パラメータの型がTYPE_FLOAT_ARRAYでない場合は0。 
						 */
						uint32_t getFloatArraySize(void) const
						{
							if(m_type != kTypeFloatArray){
								return 0;
							}
							return m_floatArraySize;
						}
						/*!
						 * @~English
						 * @brief Image ID of a parameter value 
						 * @details This gets the image ID of a parameter value. 
						 * @return Image ID of a parameter value. Blank character string in the case that the type of the parameter is not TYPE_IMAGE 
						 * @~Japanese
						 * @brief パラメータ値のイメージID 
						 * @details パラメータ値のイメージIDを取得します。 
						 * @return パラメータ値のイメージID。パラメータの型がTYPE_IMAGEでない場合は空文字列。 
						 */
						std::string getImageId(void) const
						{
							if(m_type != kTypeImage){
								return "";
							}
							return m_imageId;
						}

						void serialize(Internal::Serializer *serializer);

						/*!
						 * @~English
						 * @brief Constructor 
						 * 
						 * @details This initialized as a blank parameter. 
						 * @~Japanese
						 * @brief コンストラクタ 
						 * 
						 * @details 空のパラメータとして初期化します。 
						 */
						EffectDataParameter(void) : m_floatArray(NULL)
						{}
						/*!
						 * @~English
						 * @brief Constructor 
						 * 
						 * @details This initializes as data with a float array. The float data is copied in. 
						 * @param name Parameter name
						 * @param value float data
						 * @param size Size of float data
						 * @~Japanese
						 * @brief コンストラクタ 
						 * 
						 * @details float配列を持つデータとして初期化します。floatデータはcopyinされます。 
						 * @param name パラメータ名
						 * @param value floatデータ
						 * @param size floatデータのサイズ
						 */
						EffectDataParameter(std::string name, const float *value, uint32_t size)
						{
							m_type = kTypeFloatArray;
							m_name = name;
							m_floatArray = NULL;
							m_floatArraySize = size;
							if(m_floatArraySize > 0){
								m_floatArray = new float[size];
								for(uint32_t i=0; i<size; i++){
									m_floatArray[i] = value[i];
								}
							}
						}
						/*!
						 * @~English
						 * @brief Constructor 
						 * 
						 * @details This initializes as a parameter with an image. 
						 * @param name Parameter name
						 * @param imageId Image ID
						 * @~Japanese
						 * @brief コンストラクタ 
						 * 
						 * @details イメージを持つパラメータとして初期化します。 
						 * @param name パラメータ名
						 * @param imageId イメージID
						 */
						EffectDataParameter(std::string name, std::string imageId)
						{
							//printf(__FILE__"(%d) imageId=%s\n", __LINE__, imageId.c_str());
							m_type = kTypeImage;
							m_name = name;
							m_floatArray = NULL;
							m_imageId = imageId;
							m_floatArraySize =  0;
						}

						/*!
						 * @~English
						 * @brief Constructor 
						 * 
						 * @details This copies the value of an argument. 
						 * @param param The data of a copy source
						 * @~Japanese
						 * @brief コンストラクタ 
						 * 
						 * @details 引数の値をコピーします。 
						 * @param param コピー元のデータ
						 */
						EffectDataParameter(const EffectDataParameter& param)
						{
							m_type = param.m_type;
							m_name = param.m_name;
							m_floatArray = NULL;
							m_floatArraySize = param.m_floatArraySize;
							if(m_floatArraySize > 0){
								m_floatArray = new float[m_floatArraySize];
								for(uint32_t i=0; i<m_floatArraySize; i++){
									m_floatArray[i] = param.m_floatArray[i];
								}
							}
							m_imageId = param.m_imageId;
						}
						/*!
						 * @~English
						 * @brief Copy operator 
						 * 
						 * @details This copies the value of an argument. 
						 * @param param The data of a copy source
						 * @return The reference of this instance 
						 * @~Japanese
						 * @brief コピーオペレータ 
						 * 
						 * @details 引数の値をコピーします。 
						 * @param param コピー元のデータ
						 * @return このインスタンスの参照 
						 */
						EffectDataParameter& operator=(const EffectDataParameter& param)
						{
							m_type = param.m_type;
							m_name = param.m_name;
							m_floatArraySize = param.m_floatArraySize;
							m_floatArray = NULL;
							if(m_floatArraySize > 0){
								m_floatArray = new float[m_floatArraySize];
								for(uint32_t i=0; i<m_floatArraySize; i++){
									m_floatArray[i] = param.m_floatArray[i];
								}
							}
							return *this;
						}

						/*!
						 * @~English
						 * @brief Destructor 
						 * @details This is a destructor. 
						 * @~Japanese
						 * @brief デストラクタ 
						 * @details デストラクタです。 
						 */
						~EffectDataParameter(void)
						{
							if(m_floatArray != NULL){
								delete [] m_floatArray;
								m_floatArray = NULL;
							}
						}

					private:
#if _SCE_TARGET_OS_WINDOWS
#pragma warning(push,3)
#pragma warning(disable: 4251)
#endif // _SCE_TARGET_OS_WINDOWS
						std::string m_name;
						Type m_type;
						float *m_floatArray; 
						uint32_t m_floatArraySize;
						std::string m_imageId;
#if _SCE_TARGET_OS_WINDOWS
#pragma warning(pop)
#endif // _SCE_TARGET_OS_WINDOWS
					};
					/*!
					 * @~English
					 * 
					 * @brief The shader information of an effect 
					 * @details This is the shader information of an effect. 
					 * @~Japanese
					 * 
					 * @brief エフェクトのシェーダ情報 
					 * @details エフェクトのシェーダ情報です。 
					 */
					class SCE_SAMPLE_UTIL_API EffectDataShader
					{
					public:
						/*!
						 * @~English
						 * @brief Gets the path of a shader. 
						 * @details This gets the path of a shader. 
						 * @return Path of a shader 
						 * @~Japanese
						 * @brief シェーダのパスを取得 
						 * @details シェーダのパスを取得します。 
						 * @return シェーダのパス 
						 */
						virtual std::string getPath(void) const = 0;
						/*!
						 * @~English
						 * @brief Gets the entry function name of a shader. 
						 * @details This gets the entry function name of a shader. 
						 * @return The entry function name of a shader 
						 * @~Japanese
						 * @brief シェーダのエントリ関数名を取得 
						 * @details シェーダのエントリ関数名を取得します。 
						 * @return シェーダのエントリ関数名 
						 */
						virtual std::string getEntry(void) const = 0;
						/*!
						 * @~English
						 * @brief Gets the array of a parameter. 
						 * @details This gets the array of a parameter. 
						 * @return Array of a parameter 
						 * @~Japanese
						 * @brief パラメータの配列を取得 
						 * @details パラメータの配列を取得します。 
						 * @return パラメータの配列 
						 */
						virtual uint32_t getNumParameters(void) const = 0;
						virtual const EffectDataParameter* getParameter(uint32_t index) const = 0;

						/*!
						 * @~English
						 * @brief Adds a parameter. 
						 * 
						 * @details This adds a parameter. The parameter is copied in internally. 
						 * @param param Parameter
						 * @~Japanese
						 * @brief パラメータを追加 
						 * 
						 * @details パラメータを追加します。パラメータは内部にcopyinされます。 
						 * @param param パラメータ
						 */
						virtual void addParameter(const EffectDataParameter& param) = 0;

						virtual ~EffectDataShader(void){}
					};

				/*!
				 * @~English
				 * @brief  Effect data 
				 * 
				 * @details This is data to create an effect. 
				 * @~Japanese
				 * @brief  エフェクトデータ 
				 * 
				 * @details エフェクトを作成するためのデータです。 
				 */
				class SCE_SAMPLE_UTIL_API EffectData
				{
				public:
					/*!
					 * @~English
					 * @brief Gets the vertex shader 
					 * @return Vertex shader 
					 * 
					 * @details This gets the vertex shader. 
					 * @~Japanese
					 * @brief 頂点シェーダを取得 
					 * @return 頂点シェーダ 
					 * 
					 * @details 頂点シェーダを取得します。 
					 */
					virtual EffectDataShader* getVertexShader(void) = 0;
					/*!
					 * @~English
					 * @brief Gets the fragment shader 
					 * 
					 * @details This gets the fragment shader. 
					 * @return Fragment shader 
					 * @~Japanese
					 * @brief フラグメントシェーダを取得 
					 * 
					 * @details フラグメントシェーダを取得します。 
					 * @return フラグメントシェーダ 
					 */
					virtual EffectDataShader* getFragmentShader(void) = 0;
					/*!
					 * @~English
					 * @brief Gets the vertex shader 
					 * 
					 * @details This gets the vertex shader. 
					 * @return Vertex shader 
					 * @~Japanese
					 * @brief 頂点シェーダを取得 
					 * 
					 * @details 頂点シェーダを取得します。 
					 * @return 頂点シェーダ 
					 */
					virtual const EffectDataShader* getVertexShader(void) const = 0;
					/*!
					 * @~English
					 * @brief Gets the fragment shader 
					 * 
					 * @details This gets the fragment shader. 
					 * @return Fragment shader 
					 * @~Japanese
					 * @brief フラグメントシェーダを取得 
					 * 
					 * @details フラグメントシェーダを取得します。 
					 * @return フラグメントシェーダ 
					 */
					virtual const EffectDataShader* getFragmentShader(void) const = 0;

					virtual ~EffectData(void){}
				};

				/*!
				 * @~English
				 * @brief Material class 
				 * @details This is a material class 
				 * @~Japanese
				 * @brief マテリアルクラス 
				 * @details マテリアルクラスです。 
				 */
				class SCE_SAMPLE_UTIL_API Material
				{
				public:
					/*!
					 * @~English
					 * @brief Gets the ID of a material class. 
					 * @details This gets the number of materials. 
					 * 
					 * @return ID of a material 
					 * @~Japanese
					 * @brief マテリアルのIDを取得 
					 * @details マテリアルのIDを取得します。 
					 * 
					 * @return マテリアルのID 
					 */
					virtual std::string getId(void) const = 0;

					/*!
					 * @~English
					 * @brief Gets an effect. 
					 * @details This gets an effect. 
					 * @return Effect 
					 * @~Japanese
					 * @brief エフェクトを取得 
					 * @details エフェクトを取得します。 
					 * @return エフェクト 
					 */
					virtual const Graphics::Effect::EffectData *getEffect(void) const = 0;

					/*!
					 * @~English
					 * @brief Gets the information of an effect. 
					 * 
					 * @details This returns the data which was the generation source of an effect. 
					 * @return Effect data 
					 * @~Japanese
					 * @brief エフェクトの情報を取得 
					 * 
					 * @details エフェクトの生成元となったデータを返します。 
					 * @return エフェクトデータ 
					 */
					virtual const EffectData *getEffectData(void) const = 0;

					virtual ~Material(void){}
				};


			}
		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_EFFECT_H */

