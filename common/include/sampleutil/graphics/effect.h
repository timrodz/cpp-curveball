/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_EFFECT_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_EFFECT_H


#include <sampleutil/graphics/context.h>
#include <string>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			/*!
			 * @~English
			 * 
			 * @brief Name space associated with the effect 
			 * @details This is the name space associated with the effect. 
			 * @~Japanese
			 * 
			 * @brief エフェクト関連の名前空間 
			 * @details エフェクト関連の名前空間です。 
			 */
			namespace Effect
			{
				/*!
				 * @~English
				 * 
				 * @brief The value of a shader parameter 
				 * @details This is the value of a shader parameter. 
				 * 
				 * @~Japanese
				 * 
				 * @brief シェーダパラメータの値 
				 * @details シェーダパラメータの値です。 
				 * 
				 */
				class SCE_SAMPLE_UTIL_API  ParameterValue
				{
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
					ParameterValue(void);

					/*!
					 * @~English
					 * @brief Destructor 
					 * @details This is a destructor. 
					 * 
					 * @~Japanese
					 * @brief デストラクタ 
					 * @details デストラクタです。 
					 * 
					 */
					~ParameterValue(void);

					/*!
					 * @~English
					 * @brief Initializes as array data. 
					 * @details This initializes as array data. 
					 * 
					 * @param type Type of an element of an array
					 * @param count Number of elements
					 * @param initData The initial value. It is ignored in the case of NULL.
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 配列データとして初期化する 
					 * @details 配列データとして初期化します。 
					 * 
					 * @param type 配列の要素の型
					 * @param count 要素数
					 * @param initData 初期値。NULLの場合は無視されます。
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int initializeAsArray(ParameterType type, uint32_t count, const void *initData);

					/*!
					 * @~English
					 * @brief Initializes as texture data. 
					 * @details This initializes as texture data. 
					 * 
					 * @param texture Texture
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief テクスチャデータとして初期化する 
					 * @details テクスチャデータとして初期化します。 
					 * 
					 * @param texture テクスチャ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int initializeAsTexture(Texture *texture);

					/*!
					 * @~English
					 * @brief Sets a value in array data. 
					 * 
					 * @details This sets a value in array data. The value of float is cast in the type of an array element appropriately and set. 
					 * @param value Value to set
					 * @param componentCount Number of elements to set. If 0 is specified, the number of elements specified at the time of initialization will be set.
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 配列データに値を設定する 
					 * 
					 * @details 配列データに値を設定します。floatの値が配列要素の型に適切にキャストされ設定されます。 
					 * @param value 設定する値
					 * @param componentCount 設定する要素数。0が指定された場合は初期化時に指定された要素分設定されます。
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int setArrayDataF(const float *value, uint32_t componentCount=0);
					/*!
					 * @~English
					 * @brief Sets a value in array data. 
					 * 
					 * @details This sets a value in array data. The value of value is copied as it is. 
					 * @param value Value to set
					 * @param numBytes Number of bytes copied in an array. If 0 is specified, the size allocated at the time of initialization will be copied.
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 配列データに値を設定する 
					 * 
					 * @details 配列データに値を設定します。valueの値が配列にそのままコピーされます。 
					 * @param value 設定する値
					 * @param numBytes 配列にコピーするバイト数。0が指定された場合は初期化時に確保されたサイズ分コピーされます。
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int setArrayData(const void *value, uint32_t numBytes=0);

					/*!
					 * @~English
					 * @brief Set texture data 
					 * @details This sets texture data. 
					 * 
					 * @param texture Texture
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief テクスチャデータの設定 
					 * @details テクスチャデータを設定します。 
					 * 
					 * @param texture テクスチャ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int setTexture(Texture *texture);

					/*!
					 * @~English
					 * @brief Termination processing 
					 * 
					 * @details This executes termination processing to unload internal resources. 
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 終了処理 
					 * 
					 * @details 終了処理を行い、内部リソースを解放します。 
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int finalize(void);

					/*!
					 * @~English
					 * @brief Returns the category of a value. 
					 * 
					 * @details This returns PARAMETER_CATEGORY_UNIFORM in the case of array data and PARAMETER_CATEGORY_SAMPLER in the case of texture data. 
					 * @return Category of a value 
					 * @~Japanese
					 * @brief 値のカテゴリを返す 
					 * 
					 * @details 配列データの場合はPARAMETER_CATEGORY_UNIFORM、テクスチャデータの場合はPARAMETER_CATEGORY_SAMPLERを返します。 
					 * @return 値のカテゴリ 
					 */
					ParameterCategory getCategory(void) const
					{
						return m_category;
					}
					/*!
					 * @~English
					 * @brief Returns the type of a value. 
					 * 
					 * @details The type of an element in the case of array data This returns PARAMETER_TYPE_SAMPLER in the case of a texture. 
					 * @return Type of a value 
					 * @~Japanese
					 * @brief 値の型を返す 
					 * 
					 * @details 配列データの場合は要素の型。テクスチャの場合はPARAMETER_TYPE_SAMPLERを返します。 
					 * @return 値の型 
					 */
					ParameterType getType(void) const
					{
						return m_parameterType;
					}

					/*!
					 * @~English
					 * @brief Gets the pointer to array data 
					 * @details This gets the pointer to array data. 
					 * 
					 * @return Pointer to array data 
					 * @~Japanese
					 * @brief 配列データへのポインタを取得する 
					 * @details 配列データへのポインタを取得します。 
					 * 
					 * @return 配列データへのポインタ 
					 */
					void *getData(void)
					{
						if(m_category != kParameterCategorySampler){
							return m_array;
						}else{
							return NULL;
						}
					}
					/*!
					 * @~English
					 * @brief Gets the pointer to array data 
					 * @details This gets the pointer to array data. 
					 * 
					 * @return Pointer to array data 
					 * @~Japanese
					 * @brief 配列データへのポインタを取得する 
					 * @details 配列データへのポインタを取得します。 
					 * 
					 * @return 配列データへのポインタ 
					 */
					const void *getData(void) const
					{
						if(m_category != kParameterCategorySampler){
							return m_array;
						}else{
							return NULL;
						}
					}

					/*!
					 * @~English
					 * @brief Gets texture data. 
					 * @details This gets texture data. 
					 * 
					 * @return Texture Data 
					 * @~Japanese
					 * @brief テクスチャデータの取得 
					 * @details テクスチャデータを取得します。 
					 * 
					 * @return テクスチャデータ 
					 */
					Texture* getTexture(void) const
					{
						if(m_category == kParameterCategorySampler){
							return m_texture;
						}else{
							return NULL;
						}
					}

					/*!
					 * @~English
					 * @brief Generates a copy 
					 * 
					 * @details This generates an instance with the same contents as from. 
					 * @param from Copy source
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Failure
					 * @~Japanese
					 * @brief コピーの生成 
					 * 
					 * @details fromと同じ内容のインスタンスを生成します。 
					 * @param from コピー元
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK 失敗
					 */
					int copyFrom(const ParameterValue *from);

				private:
					ParameterCategory m_category;
					ParameterType m_parameterType;

					union {
						Texture *m_texture;
						void *m_array;
					};
					uint32_t m_count;


				};




				/*!
				 * @~English
				 * 
				 * @brief Uniform parameter with a value in an effect 
				 * @details This is a uniform parameter with a value in an effect. 
				 * @~Japanese
				 * 
				 * @brief エフェクト内の値つきユニフォームパラメータ 
				 * @details エフェクト内の値つきユニフォームパラメータです。 
				 */
				class SCE_SAMPLE_UTIL_API EffectParameter
				{
				public:
					/*!
					 * @~English
					 * 
					 * @brief Constructor 
					 * @details This is a constructor. 
					 * 
					 * @~Japanese
					 * 
					 * @brief コンストラクタ 
					 * @details コンストラクタです。 
					 * 
					 */
					EffectParameter(void);

					/*!
					 * @~English
					 * @brief Initialization 
					 * @details This executes initialization. 
					 * 
					 * @param param Shader parameter
					 * @param programType Type of the program of this parameter
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 初期化 
					 * @details 初期化します。 
					 * 
					 * @param param シェーダパラメータ
					 * @param programType このパラメータのプログラムのタイプ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int initialize(const Parameter* param, ProgramType programType);

					/*!
					 * @~English
					 * @brief Gets a shader parameter. 
					 * @details This gets a shader parameter. 
					 * 
					 * @return Shader parameter 
					 * @~Japanese
					 * @brief シェーダパラメータを取得する 
					 * @details シェーダパラメータを取得します。 
					 * 
					 * @return シェーダパラメータ 
					 */
					const Parameter* getParameter(void) const 
					{
						return m_param;
					}

					/*!
					 * @~English
					 * @brief Gets a parameter value 
					 * @details This gets a parameter value. 
					 * 
					 * @return Parameter value 
					 * @~Japanese
					 * @brief パラメータの値を取得する 
					 * @details パラメータの値を取得します。 
					 * 
					 * @return パラメータの値 
					 */
					ParameterValue* getValue(void)
					{
						return &m_value;
					}
					/*!
					 * @~English
					 * @brief Gets a parameter value 
					 * @details This gets a parameter value. 
					 * 
					 * @return Parameter value 
					 * @~Japanese
					 * @brief パラメータの値を取得する 
					 * @details パラメータの値を取得します。 
					 * 
					 * @return パラメータの値 
					 */
					const ParameterValue* getValue(void) const
					{
						return &m_value;
					}

					/*!
					 * @~English
					 * @brief Copies a value to the inside of a uniform buffer. 
					 * 
					 * @details The value is copied to the offset position of this parameter within a uniform buffer. 
					 * @param context Graphics context
					 * @param buffer Uniform buffer
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Failure
					 * @~Japanese
					 * @brief ユニフォームバッファ内に値をコピーする 
					 * 
					 * @details ユニフォームバッファ内のこのパラメータのオフセット位置に、値をコピーします。 
					 * @param context グラフィックスコンテキスト
					 * @param buffer ユニフォームバッファ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK 失敗
					 */
					int setToUniformBuffer(GraphicsContext *context, void *buffer);

					/*!
					 * @~English
					 * @brief Termination processing 
					 * 
					 * @details This executes termination processing to unload internal resources. 
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 終了処理 
					 * 
					 * @details 終了処理を行い、内部リソースを解放します。 
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int finalize(void);
					/*!
					 * @~English
					 * 
					 * @brief Destructor 
					 * @details This is a destructor. 
					 * 
					 * @~Japanese
					 * 
					 * @brief デストラクタ 
					 * @details デストラクタです。 
					 * 
					 */
					~EffectParameter(void);

					/*!
					 * @~English
					 * @brief Generates a copy 
					 * @details This generates a copy. 
					 * @param from Parameter to be the copy source
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Failure
					 * @~Japanese
					 * @brief コピーの生成 
					 * @details コピーを生成します。 
					 * @param from コピー元となるパラメータ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK 失敗
					 */
					int copyFrom(const EffectParameter* from);


					/*!
					 * @~English
					 * @brief Gets a program type. 
					 * 
					 * @details This returns the type of the program to which this parameter belongs. 
					 * @return Program type 
					 * @~Japanese
					 * @brief プログラムタイプの取得 
					 * 
					 * @details このパラメータが所属するプログラムのタイプを返します。 
					 * @return プログラムタイプ 
					 */
					ProgramType    getProgramType(void) const;

				private:
					const Parameter*  m_param;
					ParameterValue m_value;
					ProgramType    m_programType;

				};

				/*!
				 * @~English
				 * 
				 * @brief The shader information in an effect 
				 * @details This is the shader information in an effect. 
				 * @~Japanese
				 * 
				 * @brief エフェクト内シェーダ情報 
				 * @details エフェクト内シェーダ情報です。 
				 */
				class SCE_SAMPLE_UTIL_API EffectShader
				{
				public:
					/*!
					 * @~English
					 * @brief Initialization 
					 * 
					 * @details This initializes the shader information in an effect. 
					 * @param programId ID of a shader program
					 * @param programType Program type
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 初期化 
					 * 
					 * @details エフェクト内シェーダ情報を初期化します。 
					 * @param programId シェーダプログラムのID
					 * @param programType プログラムのタイプ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int initialize(const Program* programId, ProgramType programType);

					/*!
					 * @~English
					 * @brief Gets the number of parameters 
					 * @details This gets the number of parameters. 
					 * 
					 * @return Number of parameters 
					 * @~Japanese
					 * @brief パラメータ数の取得 
					 * @details パラメータ数を取得します。 
					 * 
					 * @return パラメータ数 
					 */
					uint32_t getNumParams(void) const;
					/*!
					 * @~English
					 * @brief Gets the index of parameters. 
					 * 
					 * @details This returns the index of the parameters of the specified name. 
					 * @param name Parameter name
					 * @return Index of parameters. -1 if not found. 
					 * @~Japanese
					 * @brief パラメータのインデックスを取得する 
					 * 
					 * @details 指定された名前のパラメータのインデックスを返します。 
					 * @param name パラメータ名
					 * @return パラメータのインデックス。見つからなかった場合は -1。 
					 */

					int getParamIndex(std::string name) const;

					/*!
					 * @~English
					 * @brief Gets a parameter 
					 * @details This gets a parameter. 
					 * 
					 * @param index Index of parameters
					 * @return Parameter. NULL if index is greater than the number of parameters. 
					 * @~Japanese
					 * @brief パラメータの取得 
					 * @details パラメータを取得します。 
					 * 
					 * @param index パラメータのインデックス
					 * @return パラメータ。indexがパラメータ数以上の場合はNULL。 
					 */
					EffectParameter *getParamByIndex(uint32_t index); 

					/*!
					 * @~English
					 * @brief Gets a parameter 
					 * @details This gets a parameter. 
					 * 
					 * @param name Parameter name
					 * @return Parameter. NULL if not found. 
					 * @~Japanese
					 * @brief パラメータの取得 
					 * @details パラメータを取得します。 
					 * 
					 * @param name パラメータ名
					 * @return パラメータ。見つからかった場合はNULL。 
					 */
					EffectParameter *getParamByName(std::string name);

					/*!
					 * @~English
					 * @brief Gets a parameter 
					 * @details This gets a parameter. 
					 * 
					 * @param index Index of parameters
					 * @return Parameter. NULL if index is greater than the number of parameters. 
					 * @~Japanese
					 * @brief パラメータの取得 
					 * @details パラメータを取得します。 
					 * 
					 * @param index パラメータのインデックス
					 * @return パラメータ。indexがパラメータ数以上の場合はNULL。 
					 */
					const EffectParameter *getParamByIndex(uint32_t index) const; 

					/*!
					 * @~English
					 * @brief Gets a parameter 
					 * @details This gets a parameter. 
					 * 
					 * @param name Parameter name
					 * @return Parameter. NULL if not found. 
					 * @~Japanese
					 * @brief パラメータの取得 
					 * @details パラメータを取得します。 
					 * 
					 * @param name パラメータ名
					 * @return パラメータ。見つからなかった場合はNULL。 
					 */
					const EffectParameter *getParamByName(std::string name) const;

					/*!
					 * @~English
					 * @brief Gets a program ID. 
					 * @details This gets a program ID. 
					 * 
					 * @return Program ID 
					 * @~Japanese
					 * @brief プログラムIDの取得 
					 * @details プログラムIDを取得します。 
					 * 
					 * @return プログラムID 
					 */
					const Program* getProgramId(void) const
					{
						return m_programId;
					}

					/*!
					 * @~English
					 * @brief Termination processing 
					 * 
					 * @details This executes termination processing to unload internal resources. 
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 終了処理 
					 * 
					 * @details 終了処理を行い、内部リソースを解放します。 
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int finalize(void);

					/*!
					 * @~English
					 * @brief Constructor 
					 * @details This is a constructor. 
					 * @~Japanese
					 * @brief コンストラクタ 
					 * @details コンストラクタです。 
					 */
					EffectShader(void);

					/*!
					 * @~English
					 * @brief Destructor 
					 * @details This is a destructor. 
					 * 
					 * @~Japanese
					 * @brief デストラクタ 
					 * @details デストラクタです。 
					 * 
					 */
					~EffectShader(void);

					/*!
					 * @~English
					 * @brief Generates a copy 
					 * 
					 * @details This generates an instance with the same contents as from. 
					 * @param from Copy source
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Failure
					 * @~Japanese
					 * @brief コピーの生成 
					 * 
					 * @details fromと同じ内容のインスタンスを生成します。 
					 * @param from コピー元
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK 失敗
					 */
					int copyFrom(const EffectShader *from);
				private:
					const Program*        m_programId;
					uint32_t         m_numParameters;
					EffectParameter *m_parameters;
					ProgramType      m_programType;

				};

				/*!
				 * @~English
				 * 
				 * @brief Effect class 
				 * @details This is the effect class. 
				 * @~Japanese
				 * 
				 * @brief エフェクトクラス 
				 * @details エフェクトクラスです。 
				 */
				class SCE_SAMPLE_UTIL_API EffectData
				{
					friend class EffectInstance;

				public:
					/*!
					 * @~English
					 * @brief Initialization 
					 * @details This executes initialization. 
					 * 
					 * 
					 * @param vertexProgramId ID of a vertex program
					 * @param fragmentProgramId ID of a fragment program
					 * @param blendInfo Blend information. If not blended, specify NULL.
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 初期化 
					 * @details 初期化します。 
					 * 
					 * 
					 * @param vertexProgramId 頂点プログラムのID
					 * @param fragmentProgramId フラグメントプログラムのID
					 * @param blendInfo ブレンド情報。ブレンドしない場合はNULLを指定してください。
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int initialize(
						const Program* vertexProgramId, 
						const Program* fragmentProgramId, 
						BlendInfo *blendInfo);

					/*!
					 * @~English
					 * @brief Termination processing 
					 * 
					 * @details This executes termination processing to unload internal resources. 
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 終了処理 
					 * 
					 * @details 終了処理を行い、内部リソースを解放します。 
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int finalize(void);

					/*!
					 * @~English
					 * @brief Returns the information of a vertex shader 
					 * @details This returns the information of a vertex shader. 
					 * 
					 * @return Information of a vertex shader 
					 * @~Japanese
					 * @brief 頂点シェーダの情報を返す 
					 * @details 頂点シェーダの情報を返します。 
					 * 
					 * @return 頂点シェーダの情報 
					 */
					const EffectShader *getVertexShader(void) const
					{
						return &m_vertexShader;
					}
					/*!
					 * @~English
					 * @brief Returns the information of a fragment shader 
					 * @details This returns the information of a fragment shader. 
					 * 
					 * @return Information of a fragment shader 
					 * @~Japanese
					 * @brief フラグメントシェーダの情報を返す 
					 * @details フラグメントシェーダの情報を返します。 
					 * 
					 * @return フラグメントシェーダの情報 
					 */
					const EffectShader *getFragmentShader(void) const
					{
						return &m_fragmentShader;
					}

					/*!
					 * @~English
					 * @brief Returns the information of a vertex shader 
					 * @details This returns the information of a vertex shader. 
					 * 
					 * @return Information of a vertex shader 
					 * @~Japanese
					 * @brief 頂点シェーダの情報を返す 
					 * @details 頂点シェーダの情報を返します。 
					 * 
					 * @return 頂点シェーダの情報 
					 */
					EffectShader *getVertexShader(void) 
					{
						return &m_vertexShader;
					}
					/*!
					 * @~English
					 * @brief Returns the information of a fragment shader 
					 * @details This returns the information of a fragment shader. 
					 * 
					 * @return Information of a fragment shader 
					 * @~Japanese
					 * @brief フラグメントシェーダの情報を返す 
					 * @details フラグメントシェーダの情報を返します。 
					 * 
					 * @return フラグメントシェーダの情報 
					 */
					EffectShader *getFragmentShader(void) 
					{
						return &m_fragmentShader;
					}

					/*!
					 * @~English
					 * @brief Returns the number of the attribute parameters of a vertex shader. 
					 * @details This returns the number of the attribute parameters of a vertex shader. 
					 * 
					 * @return Number of the attribute parameters of a vertex shader 
					 * @~Japanese
					 * @brief 頂点シェーダのアトリビュートパラメータ数を返す 
					 * @details 頂点シェーダのアトリビュートパラメータ数を返します。 
					 * 
					 * @return 頂点シェーダのアトリビュートパラメータ数 
					 */
					uint32_t getNumAttributeParams(void) const
					{
						return m_numAttributeParams;
					}
					/*!
					 * @~English
					 * @brief Gets the attribute parameters of a vertex shader. 
					 * @details This gets the attribute parameters of a vertex shader. 
					 * @param index Index of attribute parameters
					 * @return Attribute parameter 
					 * @~Japanese
					 * @brief 頂点シェーダのアトリビュートパラメータを取得する 
					 * @details 頂点シェーダのアトリビュートパラメータを取得します。 
					 * @param index アトリビュートパラメータのインデックス
					 * @return アトリビュートパラメータ 
					 */
					const Parameter *getAttributeParam(uint32_t index) const
					{
						if(index >= m_numAttributeParams){
							return NULL;
						}
						return m_attributeParams[index];
					}
					
					/*!
					 * @~English
					 * @brief Generates a copy 
					 * 
					 * @details This generates a new instance with the same contents as from. 
					 * @param from Copy source
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Failure
					 * @~Japanese
					 * @brief コピーの生成 
					 * 
					 * @details fromと同じ内容を持つ新しいインスタンスを生成します。 
					 * @param from コピー元
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK 失敗
					 */
					int copyFrom(const EffectData* from);

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
					EffectData(void);
					/*!
					 * @~English
					 * @brief Destructor 
					 * @details This is a destructor. 
					 * 
					 * @~Japanese
					 * @brief デストラクタ 
					 * @details デストラクタです。 
					 * 
					 */
					~EffectData(void);
				private:

					EffectShader m_vertexShader;
					EffectShader m_fragmentShader;
					BlendInfo m_blendInfo;
					uint32_t m_numAttributeParams;
					const Parameter **m_attributeParams;

				};

				/*!
				 * @~English
				 * 
				 * @brief Instance of an effect 
				 * @details This is the instance of an effect. 
				 * @~Japanese
				 * 
				 * @brief エフェクトのインスタンス 
				 * @details エフェクトのインスタンスです。 
				 */
				class SCE_SAMPLE_UTIL_API EffectInstance
				{
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
					EffectInstance(void);
					/*!
					 * @~English
					 * @brief Destructor 
					 * @details This is a destructor. 
					 * 
					 * @~Japanese
					 * @brief デストラクタ 
					 * @details デストラクタです。 
					 * 
					 */
					virtual ~EffectInstance(void);

					/*!
					 * @~English
					 * @brief Initialization 
					 * 
					 * @details This generates an instance effect. The uniform parameter is initialized to refer to the value of a parameter in the effect which was the generation source. 
					 * @param graphicsLoader Graphics loader
					 * @param effect The effect to be the source
					 * @param attrs Array of vertex attributes
					 * @param numAttrs Size of an array of vertex attributes
					 * @param streams Array of vertex streams
					 * @param numStreams Number of vertex streams
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 初期化 
					 * 
					 * @details インスタンスエフェクトを生成します。ユニフォームパラメータは生成元となったエフェクト内のパラメータの値を参照するように初期化されます。 
					 * @param graphicsLoader グラフィックスローダー
					 * @param effect 元となるエフェクト
					 * @param attrs 頂点アトリビュートの配列
					 * @param numAttrs 頂点アトリビュートの配列のサイズ
					 * @param streams 頂点ストリームの配列
					 * @param numStreams 頂点ストリームの数
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int initialize(GraphicsLoader *graphicsLoader, 
						const EffectData *effect,
						const VertexAttribute *attrs, 
						uint32_t numAttrs,

						const VertexStream *streams, 
						uint32_t numStreams);
					/*!
					 * @~English
					 * @brief Termination processing 
					 * @details This executes termination processing to unload internal resources. 
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief 終了処理 
					 * @details 終了処理を行い、内部リソースを解放します。 
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int finalize(void);

					/*!
					 * @~English
					 * @brief Gets the effect which was the generation source. 
					 * @details This gets the effect which was the generation source. 
					 * 
					 * @return Effect 
					 * @~Japanese
					 * @brief 生成元となったエフェクトの取得 
					 * @details 生成元となったエフェクトを取得します。 
					 * 
					 * @return エフェクト 
					 */
					const EffectData *getEffect(void) const
					{
						return m_effect;
					}

					/*!
					 * @~English
					 * @brief Sets the reference of a shader parameter. 
					 * 
					 * @details This sets the reference destination of a uniform parameter. The value of a parameter is read from the reference destination when the apply function is called. Each parameter refers to the value of the corresponding parameter in the effect which was the generation source at the time of generation. 
					 * @param programType Type of a shader program
					 * @param name Parameter name
					 * @param value New reference destination
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief シェーダパラメータのリファレンスを設定する 
					 * 
					 * @details ユニフォームパラメータの参照先を設定します。apply関数が呼ばれた時に参照先からパラメータの値が読み込まれます。各パラメータは生成時には生成元となったエフェクト内の対応するパラメータの値を参照しています。 
					 * @param programType シェーダプログラムのタイプ
					 * @param name パラメータ名
					 * @param value 新たな参照先
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int setParamValueReferenceByName(ProgramType programType, 
						                               const char* name, 
													   const ParameterValue *value);

					/*!
					 * @~English
					 * @brief Sets an effect in GPU. 
					 * 
					 * @details Sets the contents of this effect instance and make it drawable. After this function is called, drawing using this effect by the draw call becomes possible. 
					 * @param context Graphics context
					 * @param vertexBuffer Vertex buffer array
					 * @param numVertexBuffers Number of vertex buffer arrays
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief エフェクトをGPUに設定する 
					 * 
					 * @details このエフェクトインスタンスの内容をGPUに設定し、描画可能な状態にします。この関数を呼んだ後、drawコールによりこのエフェクトを使った描画が可能です。 
					 * @param context グラフィックスコンテキスト
					 * @param vertexBuffer 頂点バッファの配列
					 * @param numVertexBuffers 頂点バッファの配列の数
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					int apply(GraphicsContext *context,
						VertexBuffer *const*vertexBuffer, uint32_t numVertexBuffers
						);


					/*!
					 * @~English
					 * @brief Generates a copy 
					 * 
					 * @details This generates a new instance with the same contents as from. 
					 * @param from Copy source
					 * @~Japanese
					 * @brief コピーの生成 
					 * 
					 * @details fromと同じ内容を持つ新しいインスタンスを生成します。 
					 * @param from コピー元
					 */
					void cloneFrom(const EffectInstance &from);
				private:
					GraphicsLoader *m_graphicsLoader;
					VertexProgram		*m_vertexProgram;
					FragmentProgram		*m_fragmentProgram;

					const EffectData *m_effect;

					struct ValueRef
					{
						const EffectParameter  *m_param;
						const ParameterValue *m_value;

						void setToDefaultUniformBuffer(GraphicsContext *context, void *buffer);

					};

					ValueRef *m_vertexValueRefs;
					ValueRef *m_fragmentValueRefs;

				};
			}
		}

	}
}


#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_EFFECT_H */
