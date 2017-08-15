/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_BASE_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_BASE_H


#include <sampleutil/graphics/context.h>
#include <sampleutil/graphics/effect.h>
#include <sampleutil/graphics/platform.h>
#include <assert.h>

#ifndef __psp2__
#  ifndef _USE_MATH_DEFINES
#    define _USE_MATH_DEFINES 1
#  endif

#include <stdlib.h>




#else
#  include <sceconst.h>
#endif
#include <math.h>
#include <vectormath.h>

#ifndef M_PI
#define M_PI SCE_MATH_PI
#endif

#include <stdio.h>
#include <stdlib.h>
//#include <scebase_common.h>
#include <string>
#include <vectormath.h>
//#include <libdbg.h>

#define SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_IGNORE (-1)
#define SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL (-2)
#define SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_PARSE (-3)
#define SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_UNSUPPORTED (-4)
#define SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_NOMEM (-5)
#define SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_NULL_POINTER (-6)
#define SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_INVALID_PARAM (-7)

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Collada
			{
				class ColladaLoader;

				class Material;
				class Image;
				namespace Dae {
					struct Collada;
					namespace Image{
						struct Image;
						struct LibraryImages;
					}

					namespace Controller{
						struct Skin;
						struct Controller;
						struct LibraryControllers;
						struct InstanceController;
					}
					namespace effect{
						struct Effect;
						struct ProfileCommon;
						struct ProfileCg;
						struct ColorOrTexture;
						struct Sampler2D;
						struct NewParam;
						struct Bind;
						struct Shader;
						struct LibraryEffects;
					}

					namespace material
					{
						struct Material;
						struct LibraryMaterials;
					}

					namespace Common
					{
						struct InstanceMaterial;
					}

					namespace DataFlow
					{
						struct FloatArray;
						struct Param;
					}

					namespace Geometry
					{
						struct Triangles;
						struct Mesh;
						struct Polylist;
						struct Geometry;
						struct LibraryGeometries;
						struct InstanceGeometry;
					}

					namespace Animation{
						struct Sampler;
						struct Channel;
						struct Animation;
						struct LibraryAnimations;
					}

					namespace Scene {
						struct Node;
						struct LibraryVisualScenes;
						struct VisualScene;
					}


					typedef enum ValueType
					{
						kValueFloat = 0x0,
						kValueFloat2 = 0x1, 
						kValueFloat3 = 0x2, 
						kValueFloat4 = 0x3, 

						kValueFloat1x1 = 0x4, 
						kValueFloat1x2 = 0x5, 
						kValueFloat1x3 = 0x6, 
						kValueFloat1x4 = 0x7, 

						kValueFloat2x1 = 0x8, 
						kValueFloat2x2 = 0x9, 
						kValueFloat2x3 = 0xa, 
						kValueFloat2x4 = 0xb, 

						kValueFloat3x1 = 0xc, 
						kValueFloat3x2 = 0xd, 
						kValueFloat3x3 = 0xe, 
						kValueFloat3x4 = 0xf, 

						kValueFloat4x1 = 0x10, 
						kValueFloat4x2 = 0x11, 
						kValueFloat4x3 = 0x12, 
						kValueFloat4x4 = 0x13, 

						_kValueFloatInvalid = 0x14, 

						_kValueSurface = 0x15,
						kValueSampler1d = 0x16,
						kValueSampler2d = 0x17,
						kValueSamplerCube = 0x18,

						kValueInvalid = 0x19
					} ValueType;

					typedef enum VertexSemantic
					{
						kSemanticBinormal = 0,
						kSemanticColor = 1,
						kSemanticContinuity = 2,
						kSemanticImage = 3,
						kSemanticInput = 4,
						kSemanticInTangent = 5,
						kSemanticInterpolation = 6,
						kSemanticInvBindMatrix = 7,
						kSemanticJoint = 8,
						kSemanticLinearSteps = 9,
						kSemanticMorphTargetmorphWeight = 10,
						kSemanticNormal= 11,
						kSemanticOutput= 12,
						kSemanticOutTangent= 13,
						kSemanticPosition= 14,
						kSemanticTangent= 15,
						kSemanticTexbinormal= 16,
						kSemanticTexcoord= 17,
						kSemanticTextangent= 18,
						kSemanticUv= 19,
						kSemanticVertex= 20,
						kSemanticWeight= 21,
						kSemanticInvalid= 22
					}  VertexSemantic;

				}





				namespace Internal
				{
					class Effect;
					class EffectInstance;
					
					namespace geometry
					{	
						class Triangles;
						class GeomMesh;
						class LibraryGeometries;
					}
					namespace controller
					{
						struct VertexWeight;
						class Skin;
						class LibraryControllers;
					}
					class SkinMeshPart;
					struct LibraryEffects;
					class LibraryImages;
					class LibraryMaterials;

					class BuiltinResource;
					class ResourceManager;
					class ColladaLoaderImpl;
				}


				namespace Internal
				{

					class Serializer
					{
					public:
						FILE *m_file;
						sce::SampleUtil::Map<std::string, uint32_t>::Type m_stringIndexMap;
						char *m_stringTable;
						uint32_t m_lastStringIndex;
						uint32_t m_stringTableBufferSize;

						int initialize(const char *path)
						{
							m_file = fopen(path, "w");
							if(m_file == NULL){
								printf("Error : Couldn't open %s.\n",  path);
								return -1;
							}

							m_stringTableBufferSize = 1024;
							m_stringTable = (char*)malloc(m_stringTableBufferSize);
							memset(m_stringTable, 0, m_stringTableBufferSize);
							m_lastStringIndex = 0;


							return SCE_OK;
						}

						int finalize(void)
						{
							free(m_stringTable);

							int ret = fclose(m_file);
							if(ret < 0){
								return ret;
							}
							return SCE_OK;
						}

						int write(const void *data, uint32_t size)
						{
							uint32_t r = fwrite(data, 1, size, m_file); 
							SCE_SAMPLE_UTIL_ASSERT(r == size);
							(void)r;
							return SCE_OK;
						}

						void setOffset(uint32_t offset)
						{
							fseek(m_file, offset, SEEK_SET);
						}

						uint32_t getOffset(void)
						{
							return ftell(m_file);
						}

						uint32_t getStringIndex(const char* string)
						{
							std::string s = string;

							if(m_stringIndexMap.find(s) == m_stringIndexMap.end()){
								uint32_t len = strlen(string)+1;
								uint32_t ret = m_lastStringIndex;
								if((m_lastStringIndex+len) >= m_stringTableBufferSize){
									m_stringTable = (char*)realloc(m_stringTable, m_stringTableBufferSize * 2);
									m_stringTableBufferSize = m_stringTableBufferSize *2;
								}

								m_stringIndexMap[s] = m_lastStringIndex;

								memcpy(&m_stringTable[m_lastStringIndex], string, len);
								m_lastStringIndex += len;
								return ret;
							}else{
								return m_stringIndexMap[s];
							}

						}

					};
				}



				class ColladaData;


				/*!
				 * @~English
				 * @brief The interface for a class to refer to a shader parameter 
				 * 
				 * @details This is the interface to manage the reference to a shader parameter value. This is used when managing the values of parameters shared in a mesh in a scene in bulk. 
				 * @details Specify the instance of the class that inherited this interface in Node::applyParameterReference(). For the parameters to which a value other than NULL is returned when getParamValue() is called to all parameters, the values will be referenced every time rendering is executed. 
				 * @~Japanese
				 * @brief シェーダパラメータの参照を行うクラス用インタフェース 
				 * 
				 * @details シェーダのパラメータの値の参照を管理するインタフェースです。シーン内のメッシュで共有するパラメータの値をまとめて管理する際に使用します。 
				 * @details このインタフェースを継承したクラスのインスタンスを Node::applyParameterReference()に指定してください。全てのパラメータに対し、getParamValue()が呼ばれ、NULL以外の値が返ったパラメータに関しては、レンダリングのたびにその値が参照されるようになります。 
				 */
				class SCE_SAMPLE_UTIL_API ShaderParameterReference
				{
				public:
					/*!
					 * @~English
					 * @brief Destructor 
					 * 
					 * @details This is a destructor. 
					 * @~Japanese
					 * @brief デストラクタ 
					 * 
					 * @details デストラクタです。 
					 */
					virtual ~ShaderParameterReference(void){}
					/*!
					 * @~English
					 * @brief Returns the reference destination of a shader parameter. 
					 * 
					 * @details This returns the reference destination of a shader parameter. This function is called to all mesh material shader parameters in Node::applyParameterReference(). Though the value specified in the Collada file or the value of the default parameter manager is set to the parameter, for the parameters to which this function returned a value other than NULL, the value of the parameter will be referenced at the time of drawing. 
					 * @details For example, if a valid pointer to ParmeterValue is returned when this function is called to the shader parameter "light," the value of the destination of the returned pointer will be set when all of the values of the "light" parameters are set in GPU at the time of drawing. It is possible to batch-change the values of "light" parameters by changing the value of the pointer destination. 
					 * 
					 * @param programType Program type
					 * @param parameter Shader parameter
					 * @return Pointer to a new ParameterValue in the case of changing the reference destination of the parameter value. NULL in the case that there is no change. 
					 * @~Japanese
					 * @brief シェーダパラメータの参照先を返す 
					 * 
					 * @details シェーダパラメータの参照先を返します。この関数はNode::applyParameterReference()内で、全てのメッシュのマテリアルのシェーダパラメータに対し呼ばれます。パラメータにはColladaファイル内で指定された値や、デフォルトパラメータマネージャの値が設定されていますが、この関数がNULL以外の値を返したパラメータに対しては、描画時にそのパラメータの値が参照されるようになります。 
					 * @details 例えば "light" というシェーダパラメータに対してこの関数が呼ばれたときに、有効なParmeterValueへのポインタを返した場合、描画の際に全ての"light"というパラメータの値をGPUに設定する際に、返したポインタの先の値が設定されます。ポインタの先の値変更することで"light"というパラメータの値を一括して変更することが可能です。 
					 * 
					 * @param programType プログラムのタイプ
					 * @param parameter シェーダパラメータ
					 * @return パラメータの値の参照先を変更する場合は、新しいParameterValueへのポインタ。変更しない場合はNULL。 
					 */
					virtual Graphics::Effect::ParameterValue *getParamValue(Graphics::ProgramType programType, 
						const Graphics::Parameter* parameter) = 0;
				};

				/*!
				 * @~English
				 * @brief Interface to notify the World matrix when drawing a node 
				 * @details If the instance of this interface is set in ColladaLoader, the value of the World matrix of the node will be notified when drawing the node of a scene. To use the value of the World matrix, set the instance of this class. 
				 * @details If MatrixChangeListener is set in ColladaLoader, the default parameter manager will no longer be notified. 
				 * @~Japanese
				 * @brief ノードを描画する際のWorldマトリックスを通知するためのインタフェース 
				 * @details ColladaLoaderにこのインタフェースのインスタンスを設定すると、シーンのノードを描画する際に、そのノードのWorldマトリックスの値が通知されます。World行列の値を使用したい場合はこのクラスのインスタンスを設定してください。 
				 * @details MatrixChangeListener を ColladaLoaderに設定した場合デフォルトのパラメータマネージャへの通知は行われなくなります。 
				 */
				class SCE_SAMPLE_UTIL_API MatrixChangeListener
				{
				public:
					/*!
					 * @~English
					 * @param world The world matrix of a node
					 * @brief Notifies the World matrix of a node. 
					 * 
					 * @details This notifies the World matrix of a node. 
					 * @~Japanese
					 * @param world ノードのworldマトリックス
					 * @brief ノードのWorldマトリックスを通知する 
					 * 
					 * @details ノードのWorldマトリックスを通知します。 
					 */
					virtual void setWorldMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg world) = 0;
					/*!
					 * @~English
					 * 
					 * @brief Destructor 
					 * @details This is a destructor. 
					 * @~Japanese
					 * 
					 * @brief デストラクタ 
					 * @details デストラクタです。 
					 */
					virtual ~MatrixChangeListener(void){}
				};


				/*!
				 * @~English
				 * @brief Default shader parameter manager 
				 * 
				 * @details The instance of this class is automatically created in ColladaLoader. The parameters provided by this class are listed. "View" View matrix, "World" World matrix, "WorldInverseTranspose" Inverse transpose matrix of the world matrix, "WorldViewProjection" Projection matrix * View matrix * World matrix, "ViewInverse" Inverse matrix of the view matrix- "BindShapeMatrix" Bind shape matrix, "Light0Position" Position of light 0, "light0Color" Color of light 0. 
				 * 
				 * @~Japanese
				 * @brief デフォルトのシェーダパラメータマネージャ 
				 * 
				 * @details このクラスのインスタンスはColladaLoader内に自動的に作られます。このクラスが提供するパラメータを列挙します。"View" ビューマトリックス、 "World" ワールドマトリックス、"WorldInverseTranspose" ワールドマトリックスの逆転置行列、 "WorldViewProjection" 射影行列 * ビューマトリックス * ワールドマトリックス、"ViewInverse" ビューマトリックスの逆行列、"BindShapeMatrix" バインドシェイプ行列、"Light0Position" ライト0の位置、"light0Color" ライト0のカラー。 
				 * 
				 */
				class SCE_SAMPLE_UTIL_API DefaultShaderParameterManager : public ShaderParameterReference, public MatrixChangeListener
				{
				public:
					/*!
					 * @~English
					 * @param light0Position Light 0 position
					 * @param light0Color Light 0 color
					 * @brief Sets light 0. 
					 * 
					 * @details This sets light 0. 
					 * @~Japanese
					 * @param light0Position ライト0のポジション
					 * @param light0Color ライト0のカラー
					 * @brief ライト0の設定を行う 
					 * 
					 * @details ライト0の設定を行います。 
					 */
					virtual void setLight(sce::Vectormath::Simd::Aos::Vector3_arg light0Position,
						sce::Vectormath::Simd::Aos::Vector3_arg light0Color) = 0;

					/*!
					 * @~English
					 * @param world World matrix
					 * @brief Sets the world matrix. 
					 * 
					 * @details If MatrixChangeListener of ColladLoader is not overwritten, the world matrix will be automatically set. 
					 * @~Japanese
					 * @param world ワールドマトリックス
					 * @brief ワールドマトリックスの設定を行う 
					 * 
					 * @details ColladaLoaderのMatrixChangeListenerを上書きしなかった場合は、ワールドマトリックスは自動的に設定されます。 
					 */
					virtual void setWorldMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg world) = 0;

					/*!
					 * @~English
					 * @param view View matrix
					 * @brief Sets the view matrix. 
					 * 
					 * @details This sets the view matrix. 
					 * @~Japanese
					 * @param view ビューマトリックス
					 * @brief ビューマトリックスの設定を行う 
					 * 
					 * @details ビューマトリックスの設定を行います。 
					 */
					virtual void setViewMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg view)  = 0;

					/*!
					 * @~English
					 * @param projection Projection matrix
					 * @brief Sets the projection matrix. 
					 * 
					 * @details This sets the projection matrix. 
					 * 
					 * @~Japanese
					 * @param projection 射影行列
					 * @brief 射影行列の設定を行う 
					 * 
					 * @details 射影行列の設定を行います。 
					 * 
					 */
					virtual void setProjectionMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg projection)= 0;

					/*!
					 * @~English
					 * @param texture Shadow map
					 * @brief Sets the shadow map. 
					 * 
					 * @details This sets the shadow map. 
					 * @~Japanese
					 * @param texture シャドウマップ
					 * @brief シャドウマップを設定する 
					 * 
					 * @details シャドウマップを設定します。 
					 */
					virtual void setShadowTex(Texture *texture)= 0;

					/*!
					 * @~English
					 * @param shadowViewProjectionMatrix Projection matrix for a shadow map
					 * @brief Sets the projection matrix for the shadow map. 
					 * 
					 * @details This sets the projection matrix for the shadow map. 
					 * 
					 * @~Japanese
					 * @param shadowViewProjectionMatrix シャドウマップ用射影行列
					 * @brief シャドウマップ用射影行列を設定する 
					 * 
					 * @details シャドウマップ用射影行列を設定します。 
					 * 
					 */
					virtual void setShadowViewProjection(sce::Vectormath::Simd::Aos::Matrix4_arg shadowViewProjectionMatrix) = 0;




				};


				/*!
				 * @~English
				 * 
				 * @brief Manages the data shared by multiple Colladas. 
				 * 
				 * @details This class manages the data shared by multiple Colladas such as the shader for profile_common effect and default parameter manager. 
				 * 
				 * @~Japanese
				 * 
				 * @brief 複数のColladaで共有されるデータを管理する 
				 * 
				 * @details このクラスは、profile_commonエフェクト用シェーダや、デフォルトのパラメータマネージャなど、複数のCollada間で共有されるデータの管理を行います。 
				 * 
				 */
				 class SCE_SAMPLE_UTIL_API ColladaLoader : public Resource
				{
				public:
					virtual ~ColladaLoader(void){}

					/*!
					 * @~English
					 * @brief Reads collada. 
					 * @details This reads data from the specified collada file. To delete the generated instance, use sce::SampleUtil::destroy(). 
					 * @param outCollada Pointer to which the generated Collada returns
					 * @param daePath Path of a collada file
					 * @param framePeriod Sampling cycle of animation
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * 
					 * @~Japanese
					 * @brief colladaを読み込む 
					 * @details 指定したcolladaファイルからデータを読み込みます。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
					 * @param outCollada 生成されたColladaが返るポインタ
					 * @param daePath colladaファイルのパス
					 * @param framePeriod アニメーションのサンプリング周期
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 * 
					 */
					virtual int load(sce::SampleUtil::Graphics::Collada::ColladaData **outCollada, const char* daePath,
						              float framePeriod=0.0166666666666666) = 0;
					/*!
					 * @~English
					 * @brief Gets the default shader parameter manager. 
					 * @details This gets the default shader parameter manager. 
					 * @return Default shader parameter manager 
					 * @~Japanese
					 * @brief デフォルトシェーダパラメータマネージャの取得 
					 * @details デフォルトシェーダパラメータマネージャを取得します。 
					 * @return デフォルトシェーダパラメータマネージャ 
					 */
					virtual DefaultShaderParameterManager *getDefaultParams(void) const = 0;

					/*!
					 * @~English
					 * @brief Return graphics loader used at the time of initialization 
					 * @details  This returns the graphics loader used at the time of initialization. 
					 * @return Graphics loader 
					 * @~Japanese
					 * @brief 初期化時に使用したグラフィックスローダーを返す 
					 * @details  初期化時に使用したグラフィックスローダーを返します。 
					 * @return グラフィックスローダー 
					 */
					virtual Graphics::GraphicsLoader *getGraphicsLoader(void) const = 0;

					/*!
					 * @~English
					 * @brief Returns a world matrix notification object. 
					 * @details This returns a world matrix notification object. 
					 * @return World matrix notification object 
					 * @~Japanese
					 * @brief ワールド行列通知オブジェクトを返す 
					 * @details ワールド行列通知オブジェクトを返します。 
					 * @return ワールド行列通知オブジェクト 
					 */
					virtual MatrixChangeListener *getMatrixChangeListener(void) const = 0;



				};


				 /*!
				  * @~English
				  * @brief ColladaLoader generation 
				  * @details This generates a loader for reading Collada. Upon reading Collada using this object, initialization is conducted so that each shader parameter references the values defined in uniformParamManager. If uniformParamManager is NULL, the default parameter manager is used. For shader parameter without definitions in the uniformParamManager, initialization is conducted so that the values defined in collada are referenced. matrixChangeListener is the callback function to notify the world matrix of the mesh when drawing a mesh. If NULL is specified, it is notified to the default parameter manager. To delete the generated instance, use sce::SampleUtil::destroy(). 
				  * @param outColladaLoder Pointer to which the generated ColladaLoader returns
				  * @param graphicsLoader GraphicsLoader instance
				  * @param uniformParamManager Parameter manager
				  * @param matrixChangeListener World matrix notification object
				  * @retval SCE_OK Success
				  * @retval <SCE_OK Error
				  * 
				  * @~Japanese
				  * @brief ColladaLoaderの生成 
				  * @details Colladaを読み込むためのローダーを生成します。このオブジェクトを使ってColladaを読み込む際、各シェーダパラメータはuniformParamManager内に定義された値を参照するように初期化されます。uniformParamManager がNULLの場合はデフォルトのパラメータマネージャが使用されます。uniformParamManagerに定義が存在しないシェーダパラメータはcollada内に定義された値を参照するよう初期化されます。matrixChangeListener はメッシュを描画する際に、そのメッシュのワールド行列を通知するためのコールバック関数です。NULLが指定された場合は、デフォルトパラメータマネージャに通知されます。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				  * @param outColladaLoder 生成されたColladaLoaderが返されるポインタ
				  * @param graphicsLoader GraphicsLoaderのインスタンス
				  * @param uniformParamManager パラメータマネージャ
				  * @param matrixChangeListener ワールド行列の通知オブジェクト
				  * @retval SCE_OK 成功
				  * @retval <SCE_OK エラー
				  * 
				  */
				SCE_SAMPLE_UTIL_API int createColladaLoader(ColladaLoader** outColladaLoder, 
					                                        sce::SampleUtil::Graphics::GraphicsLoader *graphicsLoader,
					                                        ShaderParameterReference *uniformParamManager=NULL,
															MatrixChangeListener *matrixChangeListener=NULL);


			}



		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_BASE_H */
