/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _COLLADA_INTERNAL_H
#define _COLLADA_INTERNAL_H

#include <sampleutil/graphics/collada.h>
#include <graphics/collada/collada_material.h>
#include <sampleutil/graphics/collada/collada_scene.h>
#include <sampleutil_internal.h>
#include <scebase_common.h>


#if _SCE_TARGET_OS_WINDOWS
#include <windows.h>
#endif

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{

			namespace Collada
			{
				namespace Internal{

					typedef std::vector<std::string> Skeleton;
					typedef std::vector<Skeleton*> LibrarySkeletons;


					class SkinMeshPart;

					class MeshImpl : public Mesh
					{
						friend class Internal::LibraryMeshes;
					public:
						bool hasSkin(void) const;
						const MeshPart *getMeshPart(uint32_t index) const;
						uint32_t getNumMeshParts(void) const;

						std::string getControllerId(void) const;
						std::string getGeometryId(void) const;
						JointNameType getJointNameType(void) const;

						uint32_t getNumJoints(void) const;
						std::string getJointName(uint32_t jointIndex) const;

						const sce::Vectormath::Simd::Aos::Matrix4 *getInvBindMatrices(void) const;
						sce::Vectormath::Simd::Aos::Matrix4 getBindShapeMatrix(void) const;

						void serialize(Internal::Serializer *serializer);
						const std::vector<std::string> *getJointNameArray(void) const;
					private:
						MeshImpl(void);
						virtual ~MeshImpl(void);

						int initialize(
							ColladaLoader *loader,
							Internal::controller::Skin *skin,
							Internal::geometry::GeomMesh *geomMesh);
						int finalize(void);


						std::string m_controllerId;
						std::string m_geometryId;

						MeshPart *m_meshParts;
						unsigned int m_numMeshParts;

						JointNameType m_jointNameType;
						std::vector<std::string> m_jointNameArray;
						sce::Vectormath::Simd::Aos::Matrix4 *m_invBindMatrices;
						sce::Vectormath::Simd::Aos::Matrix4 m_bindShapeMatrix;

						

					};




										
					namespace geometry 
					{

						class  Triangles 
						{
							friend class SkinMeshPart; 
						public:
							uint32_t m_numVertices;
							uint32_t m_numTriangles;
							float *m_vertices;
							uint16_t *m_indices;


							sce::SampleUtil::Graphics::VertexAttribute *m_vertexAttributes;
							uint32_t m_numVertexAttributes;
							uint32_t m_stride;

							int *m_vertexOldIndices;
						public:
							std::string m_material;


							int initialize(const Dae::Geometry::Triangles *collada_triangles, 
								const Dae::Geometry::Mesh *mesh, 
								sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig);

							void initializeFromPolylist(const Dae::Geometry::Polylist *collada_polylist, 
								const Dae::Geometry::Mesh *mesh, 
								sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig);

							void finalize(void);

							Triangles(void)
							{
							}

							virtual ~Triangles(void)
							{}

							/*
							virtual sce::SampleUtil::IndexBuffer *getIndices(void) 
							{
							return &m_triangleIndices;
							}
							*/

							virtual unsigned short getNumIndices(void) const 
							{
								return m_numTriangles * 3;
							}

							virtual uint32_t getNumVertexAttribute(void) const
							{
								return m_numVertexAttributes;

							}
							virtual sce::SampleUtil::Graphics::VertexAttribute getVertexAttribute(int index) const
							{
								return m_vertexAttributes[index];
							}

							unsigned short getNumVertices(void)  const { return m_numTriangles * 3; }


							/*
							virtual sce::SampleUtil::VertexBuffer *getVertexStream(void)  
							{
							return &m_vertices2;
							}
							*/

							virtual sce::SampleUtil::Graphics::VertexAttribute getVertexAttribute(sce::SampleUtil::Graphics::ParameterSemantic paramSemantic, int semanticIndex) const
							{
								for(uint32_t i=0; i<m_numVertexAttributes; i++){
									if((m_vertexAttributes[i].semantic == paramSemantic) && (m_vertexAttributes[i].semanticIndex == semanticIndex)){
										return m_vertexAttributes[i];
									}
								}
								sce::SampleUtil::Graphics::VertexAttribute ret;
								ret.streamIndex = 2;
								ret.offset = 0;			
								ret.format = sce::SampleUtil::Graphics::kAttributeFormatF32;			
								ret.componentCount = 1;
								ret.semantic = paramSemantic;
								ret.semanticIndex = semanticIndex;
								return ret;
							}



							virtual unsigned int getStride() const
							{
								return m_stride;
							}


						};


						class  GeomMesh
						{
						private:
							int m_numTriangless;
							Triangles** m_triangless;
							std::string m_id;
						public:
							const std::string getId(void) const
							{
								return m_id;
							}
							uint32_t getNumTriangless(void) const
							{
								return m_numTriangless;
							}

							Triangles* getTriangles(int index) {
								return m_triangless[index];
							}


							int initialize(const std::string &id, 
								const Dae::Geometry::Mesh *mesh, 
								sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig);
							void finalize(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig);
						};

						class  LibraryGeometries
						{
						public:
							std::vector<GeomMesh*> m_meshes;


						public:
							int initialize(const Dae::Geometry::LibraryGeometries *libraryGeometries, sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig);

							const uint32_t getNumMeshes(void) const
							{
								return m_meshes.size();
							}


							const GeomMesh *getMesh(int index) const
							{
								return m_meshes.at(index);
							}

							~LibraryGeometries(void)
							{}

							void finalize(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig)
							{
								for(unsigned int i=0; i<m_meshes.size(); i++){
									m_meshes.at(i)->finalize(loaderConfig);
									delete m_meshes.at(i);
								}
								m_meshes.clear();
							}

							GeomMesh *findMesh(std::string id) 
							{
								for(unsigned int i=0; i<m_meshes.size(); i++){
									if(m_meshes.at(i)->getId() == id){
										return m_meshes.at(i);
									}
								}

								return NULL;
							}



						};

					}


					namespace controller
					{
						struct VertexWeight;
					}
					class SkinMeshPart;
					namespace controller
					{


						class  Skin
						{
						public:
							std::string m_id;
							geometry::GeomMesh *m_mesh;

							bool m_joinNameArrayUseSid;
							std::vector<std::string> m_jointNameArray;
							sce::Vectormath::Simd::Aos::Matrix4 *m_invBindMatrices;

							Internal::SkinMeshPart *m_skinPrimitiveElements;
							unsigned int m_numPrimitiveElements;

							sce::Vectormath::Simd::Aos::Matrix4 m_bindShapeMatrix;

							Skin(void)
							{
								m_skinPrimitiveElements = NULL;
							}

							void initialize(const std::string &id, sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, 
								const Dae::Controller::Skin *skin, 
								geometry::LibraryGeometries *libraryGeometries);

							void dispose(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig);
						};


						class  LibraryControllers
						{
						public:
							std::vector<Skin*> m_skins;
						public:
							void initialize(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, 
								Dae::Controller::LibraryControllers *library_controllers, 
								geometry::LibraryGeometries* libraryGeometries);

							Skin *findSkin(std::string id) ;

							void dispose(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig);

							~LibraryControllers(void)
							{

							}
						};


					};

					class EffectDataShaderImpl : public EffectDataShader
					{
					public:
						std::string getPath(void) const
						{
							return m_shaderPath;
						}
						std::string getEntry(void) const
						{
							return m_shaderEntry;
						}
						const std::vector<EffectDataParameter> *getParameters(void) const
						{
							return &m_parameters;
						}
						uint32_t getNumParameters(void) const
						{
							return m_parameters.size();
						}
						const EffectDataParameter* getParameter(uint32_t index) const 
						{
							if(index >= m_parameters.size()){
								return NULL;
							}
							const EffectDataParameter &e = m_parameters.at(index);
							return &e;
						}

						void addParameter(const EffectDataParameter& param)
						{
							return m_parameters.push_back(param);
						}
						EffectDataShaderImpl(void)
						{
							m_shaderPath = "";
							m_shaderEntry = "";
						}
						EffectDataShaderImpl(std::string path, std::string entry)
						{
							m_shaderPath = path;
							m_shaderEntry = entry;
						}
						EffectDataShaderImpl(const EffectDataShaderImpl& param)
						{
							m_shaderPath = param.m_shaderPath;
							m_shaderEntry = param.m_shaderEntry;
							for(uint32_t i=0; i<param.m_parameters.size(); i++){
								m_parameters.push_back(param.m_parameters.at(i));
							}
						}
						EffectDataShaderImpl& operator=(const EffectDataShaderImpl& param)
						{
							m_shaderPath = param.m_shaderPath;
							m_shaderEntry = param.m_shaderEntry;
							for(uint32_t i=0; i<param.m_parameters.size(); i++){
								m_parameters.push_back(param.m_parameters.at(i));
							}
							return *this;
						}

						void serialize(Internal::Serializer *serializer);
					private:
						std::string m_shaderPath;
						std::string m_shaderEntry;
						std::vector<EffectDataParameter> m_parameters;

					};



					class EffectDataImpl : public EffectData
					{
					public:
						EffectDataImpl(void)
						{
							m_profileCommon = true;
						}

						EffectDataImpl(
							const EffectDataShaderImpl &vertexShader,
							const EffectDataShaderImpl &fragmentShader)
						{
							m_profileCommon = false;
							m_vertexShader = vertexShader;
							m_fragmentShader = fragmentShader;
						}

						EffectDataShader* getVertexShader(void)
						{
							return &m_vertexShader;
						}
						EffectDataShader* getFragmentShader(void)
						{
							return &m_fragmentShader;
						}
						const EffectDataShader* getVertexShader(void) const
						{
							return &m_vertexShader;
						}
						const EffectDataShader* getFragmentShader(void) const
						{
							return &m_fragmentShader;
						}

						void serialize(Internal::Serializer *serializer);

					private:
						bool m_profileCommon;
						EffectDataShaderImpl m_vertexShader;
						EffectDataShaderImpl m_fragmentShader;



					};



					class  MaterialImpl : public Material
					{
						friend class Internal::LibraryMaterials;
					public:
						std::string getId(void) const
						{
							return m_id;
						}

						const Graphics::Effect::EffectData *getEffect(void) const
						{
							return &m_effect;
						}
						const EffectData *getEffectData(void) const
						{
							return &m_effectData;
						}

						void serialize(Internal::Serializer *serializer);

					private:
						void initialize(Dae::material::Material *material, 
							Internal::LibraryEffects *libraryEffects);

						void finalize(void);

						std::string m_id;
						std::string m_effectId;
						Graphics::Effect::EffectData m_effect;

						Internal::ColladaLoaderImpl *m_loaderConfig;

						EffectDataImpl m_effectData;

						sce::SampleUtil::Graphics::Program* m_vertexProgramId;
						sce::SampleUtil::Graphics::Program* m_fragmentProgramId;
					};


					/*! @brief エフェクトクラス */
					class Effect : public sce::SampleUtil::Graphics::Effect::EffectData
					{
					public:

						Internal::ColladaLoaderImpl *m_loaderConfig;

						std::string m_id;

						EffectDataImpl m_effectData;

						sce::SampleUtil::Graphics::Program* m_vertexProgramId;
						sce::SampleUtil::Graphics::Program* m_fragmentProgramId;

						Effect(ColladaLoader *loaderConfig,
							Dae::effect::Effect* effect, 
							ColladaData *collada);

						~Effect(void);

						int finalize(void);

						void copyFrom(const Effect *from);

					private:
						int initializeProfileCg(
							ColladaLoader *loader,
							Dae::effect::ProfileCg* profileCg, 
							Dae::effect::Effect *effect,
							ColladaData *collada);

						int initializeProfileCommon(
							ColladaLoader *loaderConfig, 
							Dae::effect::ProfileCommon* profileCommon, 
							Dae::effect::Effect *effect,
							ColladaData *collada);



					};


					class EffectInstance : public Graphics::Effect::EffectInstance
					{
					private:
						ColladaLoader *m_loaderConfig;

						Graphics::VertexBuffer    *m_dummyStream;
						Graphics::UniformBuffer *m_dummySkinMatrices;

						int m_renderingStage;

						void setup(Graphics::GraphicsContext *graphicsContext,
							Graphics::VertexBuffer *vertexBuffer, 
							Graphics::UniformBuffer *skinMatrices);

						static Graphics::VertexAttribute getVertexAttribute(const MeshPart *meshPart, const sce::SampleUtil::Graphics::Parameter *parameter);

						int initializeEffect(ColladaLoader* loader,
							const Graphics::Effect::EffectData *effect,
							const MeshPart *meshPart,
							ShaderParameterReference **paramManagers, uint32_t numParamManagers);

					public:
						EffectInstance(void);
						virtual ~EffectInstance(void);

						int initialize(ColladaLoader *loaderConfig, 
							const MeshPart *meshPart,
							const sce::SampleUtil::Graphics::Effect::EffectData *effect,
							int renderingStage
							);

						int finalize(void);


						void render(Graphics::GraphicsContext *graphicsContext, 
							Graphics::VertexBuffer *geometryStream,
							Graphics::UniformBuffer *skinMatrices, 
							Graphics::IndexBuffer *indexBuffer,
							unsigned short numIndices,
							int renderingStage);

						void cloneFrom(EffectInstance &from);

						void applyNewParameterManager(ShaderParameterReference *instanceParameterManager);
					};




					class TransformAnimationTarget
					{
					public:
						enum Type { TYPE_MATRIX_ELEM, TYPE_ANGLE, TYPE_VECTOR, TYPE_MATRIX, 
							TYPE_VECTOR_X, TYPE_VECTOR_Y, TYPE_VECTOR_Z, TYPE_UNSUPPORTED };

					private:
						Type m_type;
						std::string m_originalString;
						std::string m_targetNodeId;
						std::string m_targetNodeSid;
						std::string m_attribute;

						int m_col;
						int m_row;
					public:
						int initialize(std::string targetString, bool maybeTagetIsMatrix);

						Type getType(void)const
						{
							return m_type;
						}

						const std::string& getTargetString(void) const
						{
							return m_originalString;
						}

						const std::string& getTargetNodeId(void) const
						{
							return m_targetNodeId;
						}
						const std::string& getTargetNodeSid(void)const
						{
							return m_targetNodeSid;
						}

						uint32_t getMatrixElemCol(void) const
						{
							return m_col;
						}
						uint32_t getMatrixElemRow(void) const
						{
							return m_row;
						}


						void print(void)
						{
							printf("(%s, %s, %s, %d, %d)\n", 
								m_targetNodeId.c_str(), 
								m_targetNodeSid.c_str(), 
								m_attribute.c_str(), 
								m_col, m_row);
						}


					};

					class TransformAnimationChannel
					{
					private:
						TransformAnimationTarget m_target;
						unsigned int m_numData;
						float *m_times;
						float *m_values;

						float m_beginTime;
						float m_endTime;
						float m_minPeriod;
						int *m_indexMap;
						unsigned int m_numIndexMap;

						std::pair<int,int> getIndex(float time) const;
						std::pair<int,int> getIndex(float time, int index) const;

					public:
						TransformAnimationChannel(void);

						void initialize(float *times, unsigned int numData, 
							float *values, unsigned int numValues,
							std::string targetString, bool maybeTagetIsMatrix);

						~TransformAnimationChannel();

						void finalize(void);

						std::pair<float,float> getTimeRange(void) const;

						float getFloatValue(float time) const;

						sce::Vectormath::Simd::Aos::Vector3 getVectorValue(float time) const;	
						sce::Vectormath::Simd::Aos::Matrix4 getMatrixValue(float time) const;

						const TransformAnimationTarget& getTarget(void) const
						{
							return m_target;
						}			

					};


					class TransformAnimation
					{

						TransformAnimationChannel *m_channels;
						unsigned int m_numChannels;
					public:
						TransformAnimation(Dae::Animation::Animation *animation);
						std::pair<float,float> getTimeRange(void);

						~TransformAnimation(void)
						{
							delete [] m_channels;
							m_channels = NULL;
						}

						uint32_t getNumChannels(void)
						{
							return m_numChannels;
						}

						const TransformAnimationChannel *getChannel(uint32_t index)
						{
							if(index < m_numChannels){
								return &m_channels[index];
							}
							return NULL;
						}


					};


					struct LibraryTransformAnimations
					{
						unsigned int m_numAnimations;
						Internal::TransformAnimation **m_animations;

						void initialize(const Dae::Animation::LibraryAnimations *libraryAnimations);
						std::pair<float,float> getTimeRange(void);

						void dispose(void);

						~LibraryTransformAnimations(void);
					};



					class InstanceMeshPartImpl : public  InstanceMeshPart
					{
						friend class Internal::InstanceMeshImpl;
					public:
						void draw(Graphics::GraphicsContext *graphicsContext,
							Graphics::UniformBuffer *skinMatrices,
							int renderingStage);

						void applyShaderParameterManager(ShaderParameterReference *nodeParameterManager);
						int addEffect(const Graphics::Effect::EffectData *effect,	int renderingStage);

					private:
						ColladaLoader *m_loaderConfig;
						const MeshPart *m_meshPart;
						std::vector<Internal::EffectInstance*> m_effectInstances;
						InstanceMeshPartImpl(void);
						virtual ~InstanceMeshPartImpl(void);
						int initialize(ColladaLoader *loaderConfig, const MeshPart *meshPart,	const Graphics::Effect::EffectData *effect);
						int finalize(void);
						void copyFrom(InstanceMeshPartImpl *instancePrimitiveElement );
					};





					class InstanceMeshImpl : public InstanceMesh
					{
						friend class Internal::NodeInternal;

					public:
						void updateSkinMatrices(void);

						void applyShaderParameterManager(ShaderParameterReference *nodeParameterManager);

						void draw( sce::Vectormath::Simd::Aos::Matrix4_arg localToWorld, 
							Graphics::GraphicsContext *graphicsContext, 
							int renderingStage);

						uint32_t getNumMeshParts(void) const;

						InstanceMeshPart *getMeshPart(uint32_t index);

						const Mesh* getMesh(void) const;

						int addEffect(const Graphics::Effect::EffectData *effect,	int renderingStage);

					private:
						MatrixChangeListener *m_shaderParameterManager;
						const MeshImpl *m_mesh;
						Internal::Skeleton  *m_skeleton;

						unsigned int    m_numMeshParts;
						InstanceMeshPartImpl *m_meshParts;

						class InstanceSkin
						{
						private:
							uint32_t m_numJoints;
							uint32_t m_currentJointIndex;
							sce::SampleUtil::Graphics::UniformBuffer *m_jointInfo[3];
							const sce::Vectormath::Simd::Aos::Matrix4 *m_jointInvMatrices;
							const sce::Vectormath::Simd::Aos::Matrix4 **m_jointMatrices;
						public:
							InstanceSkin(ColladaLoader* loader,
								uint32_t numJoints,
								const sce::Vectormath::Simd::Aos::Matrix4 *jointInvMatrices,
								const sce::Vectormath::Simd::Aos::Matrix4 **refJointMatrices
								);
							void update(void);

							sce::SampleUtil::Graphics::UniformBuffer *getCurrentJointBuffer(void)
							{
								return m_jointInfo[m_currentJointIndex]; 
							}

							~InstanceSkin(void);
						};

						InstanceSkin *m_instanceSkin ;

						int initialize(ColladaLoader *loaderConfig, 
							const Mesh *mesh,
							const Dae::Controller::InstanceController *instanceController,
							Internal::LibrarySkeletons *librarySkeletons,
							ColladaData *collada,
							Node *rootNode);

						int initialize(ColladaLoader *loaderConfig, 
							const Mesh *mesh,
							const Dae::Geometry::InstanceGeometry *daeInstanceGeometry, 
							Internal::LibrarySkeletons *librarySkeletons,
							ColladaData *collada,
							Node *rootNode);

						int finalize(void);

						InstanceMeshImpl(void);
						~InstanceMeshImpl(void);

						int cloneFrom(ColladaLoader* loaderConfig, 
							const InstanceMeshImpl *instanceController3,
							const Node *rootNode);

					};






					class SkinMeshPart
					{
						typedef uint16_t JointIndexType;
						static const int m_numJointsPerVertex = 8;
					private:
						uint32_t m_numVertices;
						uint32_t m_stride;
						uint32_t m_jointIndecesOffset;
						uint32_t m_jointWeightsOffset;
					public:
						sce::SampleUtil::Graphics::VertexBuffer *m_stream;
						void *m_vertices;

						int initialize(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, 
							const Dae::Controller::Skin *skin,
							const Internal::geometry::Triangles *triangles, 
							Internal::controller::VertexWeight *vweights);
						int finalize(void);

						virtual sce::SampleUtil::Graphics::VertexAttribute getVertexAttribute(const sce::SampleUtil::Graphics::ParameterSemantic paramSemantic, int semanticIndex) const
						{
							sce::SampleUtil::Graphics::VertexAttribute ret;

							if(paramSemantic == sce::SampleUtil::Graphics::kParameterSemanticBlendindices){
								ret.streamIndex = 1;
								ret.offset = m_jointIndecesOffset + sizeof(uint16_t)*4 * semanticIndex;			
								ret.format = sce::SampleUtil::Graphics::kAttributeFormatU16;			
								ret.componentCount = 4;
								ret.semantic = paramSemantic;
								ret.semanticIndex = semanticIndex;
							}else if(paramSemantic == sce::SampleUtil::Graphics::kParameterSemanticBlendweight){
								ret.streamIndex = 1;
								ret.offset = m_jointWeightsOffset + sizeof(float)*4 * semanticIndex;
								ret.format = sce::SampleUtil::Graphics::kAttributeFormatF32;			
								ret.componentCount = 4;
								ret.semantic = paramSemantic;
								ret.semanticIndex = semanticIndex;
							}else{
								ret.streamIndex = 2;
								ret.offset = 0;			
								ret.format = sce::SampleUtil::Graphics::kAttributeFormatF32;			
								ret.componentCount = 1;
								ret.semantic = paramSemantic;
								ret.semanticIndex = semanticIndex;
							}
							return ret;
						}
						virtual unsigned int getStride() const
						{
							return m_stride;
						}
						virtual ~SkinMeshPart(void){}
					};

					/*! @brief シーンのノード */
					class NodeInternal : public Node
					{
						std::string m_id;
						std::string m_sid;
						std::vector<Internal::TransformationElement*> m_transforms;
						InstanceMeshImpl* m_instanceMeshes;
						uint32_t m_numInstanceMeshes;

						sce::Vectormath::Simd::Aos::Matrix4 m_localMatrix; // TODO
						sce::Vectormath::Simd::Aos::Matrix4 m_localToWorld; // TODO

						const NodeInternal*      m_parentNode;
						NodeInternal*      m_childNodes;
						unsigned int m_numChildNodes;

					public:

						NodeInternal(void);
						~NodeInternal(void);



						int initialize(	Dae::Scene::Node *daeNode, NodeInternal *parent);

						int initializeMesh(ColladaLoader *loaderConfig, 
							Dae::Scene::Node *daeNode,
							ColladaData *collada,
							LibrarySkeletons *librarySkeletons,
							NodeInternal *rootNode);

						int cloneFromWithoutTransform(ColladaLoader *loaderConfig, 
							const NodeInternal* node3,
							const NodeInternal *rootNode);

						void cloneMeshFrom(ColladaLoader* loaderConfig, 
							const NodeInternal* node,
							const NodeInternal *rootNode);


						virtual Node *findNodeBySid(std::string sid);
						virtual Node *findNodeById(std::string id);

						virtual const Node *findNodeBySid(std::string sid) const;
						virtual const Node *findNodeById(std::string id) const;

						int finalize(void);

						Internal::TransformationElement* getTransformationElementBySid(std::string sid);
						Internal::TransformationElement* getTransformationElementByIndex(uint32_t index)
						{
							return m_transforms.at(index);
						}
						uint32_t getNumTransformationElements(void)
						{
							return m_transforms.size();
						}

						sce::Vectormath::Simd::Aos::Matrix4 getTransformMatrix(void) const;
						void setTransformMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg m)
						{
							m_localMatrix = m;
						}
						sce::Vectormath::Simd::Aos::Matrix4 getWorldMatrix(void) const
						{
							return m_localToWorld;
						}
						const sce::Vectormath::Simd::Aos::Matrix4* getWorldMatrixPointer(void) const
						{
							return &m_localToWorld;
						}
						/*! @brief このノードと子ノードのワールド行列を更新します。
						*  変換要素を元に、このノードと子ノードのワールド行列を更新します。
						*  変換要素を変更した場合は、描画前にこの関数を呼んでください。
						*/
						void updateWorldMatrixUsingTransform(void); 
						virtual void updateWorldMatrix(void); 

						/*! @brief このノードと子ノードのメッシュのスキン行列を更新します。
						* スキン行列のバッファを次のフレーム描画用のバッファに切り替え、
						*  ノードのワールド行列を元に、このノードと子ノードのメッシュのスキン行列を更新します。
						*  ワールド行列が更新されたら、描画前にこの関数を呼びスキン行列を更新してください。
						*/
						virtual void updateSkinMatrices(void);
						virtual void draw(sce::SampleUtil::Graphics::GraphicsContext *graphicsContext, 
							sce::Vectormath::Simd::Aos::Matrix4_arg matrix,
							int renderingStage) const;
						virtual void applyShaderParameterManager(ShaderParameterReference *nodeParameterManager);
						virtual std::string getId(void) const;
						virtual std::string getSid(void) const;


						virtual InstanceMesh* getInstanceMesh(uint32_t index)
						{
							if(index >= m_numInstanceMeshes){
								return NULL;
							}
							return &m_instanceMeshes[index];
						}
						virtual uint32_t getNumInstanceMeshes(void) const
						{
							return m_numInstanceMeshes;
						}

						virtual uint32_t getNumChildNodes(void) const
						{
							return m_numChildNodes;
						}
						virtual Node* getChildNode(uint32_t index)
						{
							if(index >= m_numChildNodes){
								return NULL;
							}
							return &m_childNodes[index];
						}

						virtual const Node* getChildNode(uint32_t index) const
						{
							if(index >= m_numChildNodes){
								return NULL;
							}
							return &m_childNodes[index];
						}

						virtual int addEffect(const Graphics::Effect::EffectData *effect, int renderingStage)
						{
							for(uint32_t i=0; i<getNumInstanceMeshes(); i++){
								int ret = getInstanceMesh(i)->addEffect(effect, renderingStage);
								if(ret != SCE_OK){
									return ret;
								}
							}
							for(uint32_t i=0; i<m_numChildNodes; i++){
								int ret = m_childNodes[i].addEffect(effect, renderingStage);
								if(ret != SCE_OK){
									return ret;
								}
							}
							return SCE_OK;
						}

					};



					struct TransformAnimationTargetInstance
					{
						Internal::TransformAnimation *m_animation;
						Internal::TransformationElement** m_targetTransElems; 

						TransformAnimationTargetInstance(void);
						~TransformAnimationTargetInstance(void);

						int initialize(Internal::TransformAnimation *animation, NodeInternal *sceneRootNode);

						void setTime(float time);

						int finalize(void);
					};


					struct TransFormAnimationPlayer
					{

						TransformAnimationTargetInstance *m_animationTargetInstance;
						unsigned int m_numTransformAnimationInstance;
						float m_startTime;
						float m_endTime;
						NodeInternal *m_rootNode;

						TransFormAnimationPlayer(void)
						{
							m_animationTargetInstance = NULL;
							m_numTransformAnimationInstance = 0;
						}

						int initialize(Internal::LibraryTransformAnimations *libraryTransformAnimations,
							NodeInternal *rootNode)
						{
							m_rootNode = rootNode;
							std::pair<float,float> range = libraryTransformAnimations->getTimeRange();
							m_startTime = range.first;
							m_endTime   = range.second;
							if(m_startTime > m_endTime){
								m_startTime = 0.0f;
								m_endTime = 0.0f;
							}
							m_numTransformAnimationInstance = libraryTransformAnimations->m_numAnimations;
							if(m_numTransformAnimationInstance > 0){
								m_animationTargetInstance = new TransformAnimationTargetInstance[ m_numTransformAnimationInstance ];
								for(unsigned int i=0; i<m_numTransformAnimationInstance; i++){
									m_animationTargetInstance[i].initialize(libraryTransformAnimations->m_animations[i],  m_rootNode);
								}
							}
							return SCE_OK;
						}

						void setTime(float time)
						{
							for(unsigned int i=0; i< m_numTransformAnimationInstance; i++){
								m_animationTargetInstance[i].setTime(time);
							}

							m_rootNode->updateWorldMatrixUsingTransform();
							m_rootNode->updateSkinMatrices();
						}

						int finalize(void)
						{
							if(m_animationTargetInstance != NULL){
								for(unsigned int i=0; i<m_numTransformAnimationInstance; i++){
									m_animationTargetInstance[i].finalize();
								}
								delete [] m_animationTargetInstance;
							}
							m_numTransformAnimationInstance = 0;
							m_animationTargetInstance = NULL;
							return SCE_OK;
						}
						~TransFormAnimationPlayer(void)
						{
							finalize();
						}
					};






					struct DummyVertex
					{
						uint16_t m_blendIndices[16];
						float m_blendWeight[16];
						float m_other[16];
					};

					class BuiltinResource
					{
					private:
						sce::SampleUtil::Graphics::GraphicsLoader *m_graphicsLoader;
					public:
						sce::SampleUtil::Graphics::Program*	m_profileCommonPhongSkinningVertexProgramId;
						sce::SampleUtil::Graphics::Program*	m_profileCommonPhongSkinningFragmentProgramId;
						sce::SampleUtil::Graphics::Texture *m_dummyTexture;
						sce::SampleUtil::Graphics::VertexBuffer *m_dummyVertexBuffer;
						sce::SampleUtil::Graphics::UniformBuffer *m_dummySkinMatrices;

						int initialize(sce::SampleUtil::Graphics::GraphicsLoader *graphicsLoader);
						int finalize(void);
					};


					class ResourceManager
					{
					private:
						sce::SampleUtil::Graphics::GraphicsLoader *m_graphicsLoader;

						struct Val
						{
							uint32_t count;
							std::string ext;
							const sce::SampleUtil::Graphics::Program* val;
						};

						typedef sce::SampleUtil::Map<std::string, Val>::Type Map;
						typedef sce::SampleUtil::Map<std::string, Val>::Type::iterator MapItr;
						Map m_map;

						static std::string fixShaderPathFromCgToGxp(std::string cgpath)
						{
							size_t p = cgpath.find_last_of(".");
							if(p == cgpath.npos){
								return "";
							}
							return cgpath.substr(0, p) + ".gxp";
						}

						sce::SampleUtil::Graphics::Program* loadVSInternal(std::string path, std::string entry);

						void unloadVSInternal(sce::SampleUtil::Graphics::Program* programId);

						sce::SampleUtil::Graphics::Program* loadFSInternal(std::string path, std::string entry);

						void unloadFSInternal(sce::SampleUtil::Graphics::Program* programId);

						struct TextureCache
						{
							sce::SampleUtil::Graphics::Texture* m_texture;
							uint32_t m_refCount;
						};

						typedef sce::SampleUtil::Map<std::string, TextureCache>::Type TextureMap;
						TextureMap m_textures;


					public:
						BuiltinResource m_builtinResource;
						int initialize(sce::SampleUtil::Graphics::GraphicsLoader *graphicsLoader);
						int finalize(void);

						sce::SampleUtil::Graphics::Program* loadVertexShader(std::string path, std::string entry);

						void unloadVertexShader(sce::SampleUtil::Graphics::Program* programId);

						sce::SampleUtil::Graphics::Program* loadFragmentShader(std::string path, std::string entry);

						void unloadFragmentShader(sce::SampleUtil::Graphics::Program* programId);

						sce::SampleUtil::Graphics::Texture* referTexture(const std::string &absPath);

						void decrReferenceTexture(const std::string &absPath);

						BuiltinResource *getBuiltinResource(void) 
						{
							return &m_builtinResource;
						}
					};




					class LibraryImages
					{
					public:
						Image *m_images;
						uint32_t m_numImages;

						sce::SampleUtil::Graphics::Texture *m_dummyTexture;

						LibraryImages(void)
						{
							m_images = NULL;
							m_numImages = 0;
						}

						int initialize(ColladaLoader *loaderConfig, 
							const std::string daePath, 
							Dae::Image::LibraryImages *libraryImages);
						Image *findImageById(std::string id) const
						{
							for(uint32_t i=0; i<m_numImages; i++){
								if(m_images[i].getId()== id){
									return &m_images[i];
								}
							}
							return NULL;
						}
						sce::SampleUtil::Graphics::Texture *getTexture(std::string id);
						int finalize(void);

						void serialize(Internal::Serializer *serializer);
					};


					struct LibraryEffects
					{
						std::vector<Effect*> m_effects;
						int initialize(ColladaLoader* loaderConfig, 
							Dae::effect::LibraryEffects *libraryEffects, 
							ColladaData *collada);

						Effect *findEffectById(std::string id);

						int finalize(void);
						~LibraryEffects(void);
					};

					class LibraryMeshes
					{
					public:
						std::vector<MeshImpl*> m_meshes;
					public:
						int initialize(ColladaLoader *loader, 
							Internal::controller::LibraryControllers *libraryControllers, 
							Internal::geometry::LibraryGeometries* libraryGeometries);

						Mesh *findMeshByControllerId(std::string controllerId) const;
						Mesh *findMeshByGeometryId(std::string geometryId) const;

						void serialize(Internal::Serializer *serializer);

						int finalize(void);
						~LibraryMeshes(void)
						{
						}
					};





					class LibraryMaterials
					{
					public:
						MaterialImpl *m_materials;
						unsigned int m_numMaterials;

						LibraryMaterials(void);
						~LibraryMaterials(void);

						int initialize(Dae::material::LibraryMaterials *libraryMaterials, 
							Internal::LibraryEffects *libraryEffects);

						Material *getMaterial(std::string id) const;

						int finalize(void);

						void serialize(Internal::Serializer *serializer);
					};


					class VisualSceneImpl : public VisualScene
					{
						friend class LibraryNodeAnimations;
						friend class Internal::InstanceVisualSceneImpl;
						friend class Internal::ColladaDataImpl;
					public:

						std::string  getId(void) const; 
						std::string  getName(void) const; 

						Node *findNodeById(std::string id);
						Node *findNodeBySid(std::string sid);
						const Node *findNodeById(std::string id) const;
						const Node *findNodeBySid(std::string sid) const;

						Node *getRootNode(void);

						const Node *getRootNode(void) const;

					private:
						const ColladaData *m_collada;
						ColladaLoader *m_loaderConfig;

						std::string m_id;
						std::string m_name;

						Node  *m_rootNode;

						int initialize(ColladaLoader *loaderConfig, 
							Dae::Scene::VisualScene *daeVisualScene,
							ColladaData *collada,
							Internal::LibrarySkeletons *librarySkeletons,
							Internal::LibraryTransformAnimations *libraryTransformAnimations
							);

						int finalize(void);

						VisualSceneImpl(void);
						~VisualSceneImpl(void);

					};







					class ColladaDataImpl : public ColladaData, public sce::SampleUtil::Impl::ResourceImpl
					{
						friend class Internal::Effect;
						friend class Material;
						friend class VisualScene;
					public:
						ColladaDataImpl(void);
						virtual ~ColladaDataImpl(void);
						int initialize(const char* daePath, 
							ColladaLoader *loader,
							float framePeriod=0.0166666666666666);
						int finalize(void);

						ColladaLoader  *getLoader() const;
						const Image *getImageById(std::string id) const;
						const Image *getImageByIndex(uint32_t index) const;
						Image *getImageByIndex(uint32_t index);
						uint32_t getNumImages(void);
						const Material *getMaterialById(std::string id) const;
						const Material *getMaterialByIndex(uint32_t index) const;
						uint32_t getNumMaterials(void) const;

						const Mesh *getMeshByControllerId(std::string controllerId) const;

						const Mesh *getMeshByGeometryId(std::string geometryId) const;
						const Mesh *getMeshByIndex(uint32_t index) const;

						uint32_t getNumMeshes(void) const;
						const VisualScene *getVisualScene(void) const;

						const Animation* getAnimation(void) const;

						int serialize(const char *filename);
						int deserialize(const char *filename);
						

						std::string getResourceType(void) const
						{
							return "Graphics::Collada::ColladaData";
						}

					private:



						ColladaLoader *m_loaderConfig;
						std::string m_daePath;

						Internal::LibraryImages         m_libraryImages;
						Internal::LibraryMaterials      m_libraryMaterials;
						Internal::LibraryTransformAnimations     *m_libraryAnimations;
						Internal::LibraryMeshes         m_libraryMeshes;
						Animation             m_animation;
						VisualSceneImpl           *m_visualScene;
						Internal::LibrarySkeletons      m_librarySkeletons;

						int initialize(Dae::Collada *daeCollada, 
							const char* daePath, 
							ColladaLoader *loaderConfig,
							float framePeriod);

					};









				class DefaultShaderParameterManagerImpl : public DefaultShaderParameterManager
					{
						friend class Internal::ColladaLoaderImpl;
					public:
						void setLight(sce::Vectormath::Simd::Aos::Vector3_arg light0Position,
							sce::Vectormath::Simd::Aos::Vector3_arg light0Color);

						void setWorldMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg world);

						void setViewMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg view) ;

						void setProjectionMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg projection);

						void setShadowTex(Texture *texture);

					void setShadowViewProjection(sce::Vectormath::Simd::Aos::Matrix4_arg shadowViewProjectionMatrix) ;

					virtual sce::SampleUtil::Graphics::Effect::ParameterValue *getParamValue(sce::SampleUtil::Graphics::ProgramType type, 
						const sce::SampleUtil::Graphics::Parameter* parameter);


				private:
					DefaultShaderParameterManagerImpl(Internal::BuiltinResource *resource);
					virtual ~DefaultShaderParameterManagerImpl(void);

					sce::Vectormath::Simd::Aos::Matrix4 m_paramView;
					sce::Vectormath::Simd::Aos::Matrix4 m_paramViewInverse;
					sce::Vectormath::Simd::Aos::Matrix4 m_paramWorldInverseTranspose;
					sce::Vectormath::Simd::Aos::Matrix4 m_paramWorldViewProjection;
					sce::Vectormath::Simd::Aos::Matrix4 m_paramWorld;

					sce::Vectormath::Simd::Aos::Matrix4 m_paramShadowViewProjection;

					sce::Vectormath::Simd::Aos::Matrix4 m_world;
					sce::Vectormath::Simd::Aos::Matrix4 m_view;
					sce::Vectormath::Simd::Aos::Matrix4 m_projection;

					sce::Vectormath::Simd::Aos::Matrix4 m_bindShapeMatrix;

					sce::Vectormath::Simd::Aos::Vector3 m_light0Position;
					sce::Vectormath::Simd::Aos::Vector3 m_light0Color;

					sce::Vectormath::Simd::Aos::Matrix4 m_shadowViewProjection;

					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueView;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueViewInverse;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueWorldViewProjection;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueWorld;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueWorldInverseTranspose;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valuebindShapeMatrix;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueLight0Position;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueLight0Color;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueShadowViewProjection;
					sce::SampleUtil::Graphics::Effect::ParameterValue m_valueShadowTex;

					void updateMatrices(void);

				};



					class ColladaLoaderImpl : public ColladaLoader, public sce::SampleUtil::Impl::ResourceImpl
					{
						friend class Graphics::Collada::Image;
						friend class Internal::LibraryImages;
						friend class Graphics::Collada::Internal::Effect;
						friend class Graphics::Collada::Internal::EffectInstance;
						friend class Graphics::Collada::Internal::MaterialImpl;
					public:
						ColladaLoaderImpl(void)
						{
							m_useDefaultParamManager = false;
							m_resourceManager = NULL;
						}
						virtual ~ColladaLoaderImpl(void)
						{
							finalize();
						}
						/*!
						* @~English
						* @brief Initialization 
						* @details If Collada is read using this object, for a parameter subject to the default parameter manager and other parameters, the value managed by the default parameter manager and the value written in collada will be set, respectively, in the reference value of the shader parameter. If uniformParamManager is specified, uniformParamManager is used on a top-priority basis.
						* @details matrixChangeListener is the callback function to notify the world metrix of the mesh when drawing a mesh. If NULL is specified, it is notified to the default parameter manager.
						* @param graphicsLoader The instance of GraphicsLoader 
						* @param uniformParamManager Parameter manager 
						* @param matrixChangeListener The notification object of the world matrix 
						* @retval SCE_OK Success 
						* @retval <SCE_OK Error 
					 * 
					 * @~Japanese
					 * @brief 初期化 
					 * @details このオブジェクトを使ってColladaを読み込む場合、シェーダパラメータの参照値はデフォルトパラメータマネージャの対象となっているパラメータの場合は、デフォルトパラメータマネージャが管理する値が設定され、それ以外のパラメータはcolladaに書かれた値が設定されます。uniformParamManagerが指定された場合、uniformParamManagerが最優先して使用されます。
					 * @details matrixChangeListener はメッシュを描画する際に、そのメッシュのワールド行列を通知するためのコールバック関数です。NULLが指定された場合は、デフォルトパラメータマネージャに通知されます。
					 * @param graphicsLoader GraphicsLoaderのインスタンス 
					 * @param uniformParamManager パラメータマネージャ 
					 * @param matrixChangeListener ワールド行列の通知オブジェクト 
					 * @retval SCE_OK 成功 
					 * @retval <SCE_OK エラー 
					 * 
					 */
						int initialize(sce::SampleUtil::Graphics::GraphicsLoader *graphicsLoader,
							ShaderParameterReference *uniformParamManager=NULL,
							MatrixChangeListener *matrixChangeListener=NULL);
						/*!
						* @~English
						* @brief Termination processing 
						* @details This executes termination processing to unload resources.
						* @retval SCE_OK Success 
						* @retval <SCE_OK Error 
					 * 
					 * @~Japanese
					 * @brief 終了処理 
					 * @details 終了処理を行いリソースを解放します。
					 * @retval SCE_OK 成功 
					 * @retval <SCE_OK エラー 
					 * 
					 */
						int finalize(void);

						int load(sce::SampleUtil::Graphics::Collada::ColladaData **outCollada, const char* daePath,
							float framePeriod=0.0166666666666666);
						DefaultShaderParameterManager *getDefaultParams(void) const;

						Graphics::GraphicsLoader *getGraphicsLoader(void) const;

						MatrixChangeListener *getMatrixChangeListener(void) const;

						std::string getResourceType(void) const
						{
							return "Graphics::Collada::ColladaLoader";
						}
						
					private:
						bool m_verbose;

						sce::SampleUtil::Graphics::GraphicsLoader *m_graphicsLoader;
						Internal::ResourceManager *m_resourceManager;

						std::string m_daePath;

						bool m_useDefaultParamManager;
						DefaultShaderParameterManager *m_defaultShaderParameterManager;
						ShaderParameterReference *m_globalUniformParamManager;
						MatrixChangeListener *m_matrixChangeListener;

					};



					class InstanceVisualSceneImpl : public InstanceVisualScene, public sce::SampleUtil::Impl::ResourceImpl
					{
					public:
						InstanceVisualSceneImpl(void);
						~InstanceVisualSceneImpl(void);

						int initialize(const VisualScene *visualScene);

						int finalize(void);

						Node *getRootNode(void);
						const Node *getRootNode(void) const;

						void draw(GraphicsContext *graphicsContext, 
							sce::Vectormath::Simd::Aos::Matrix4_arg matrix=sce::Vectormath::Simd::Aos::Matrix4::identity(),
							uint32_t renderingStage=0) const;

						void applyParameterReference(ShaderParameterReference *reference)
						{
							m_rootNode->applyShaderParameterManager(reference);
						}

						int addEffect(const Graphics::Effect::EffectData *effect, int renderingStage)
						{
							return m_rootNode->addEffect(effect, renderingStage);
						}

						Node *findNodeById(std::string id);
						Node *findNodeBySid(std::string sid);
						const Node *findNodeById(std::string id) const;

						const Node *findNodeBySid(std::string sid) const;
						const VisualScene *getVisualScene(void) const;


						std::string getResourceType(void) const
						{
							return "Graphics::Collada::InstanceVisualScene";
						}

					private:
						Node*  m_rootNode;
						const VisualScene *m_visualScene;

					};




					/*!
					* @~English
					* @brief Animation player 
					* 
					* @details This transfers the node of InstanceVisualScene according to the data of Animation and animate it. 
					* @~Japanese
					* @brief アニメーションプレイヤー 
					* 
					* @details InstanceVisualSceneのノードをAnimationのデータに従い移動させ、アニメーションさせます。 
					*/
					class AnimationPlayerImpl : public AnimationPlayer, public sce::SampleUtil::Impl::ResourceImpl
					{

					public:
						/*!
						* @~English
						* @brief Constructor 
						* @details This is a constructor. 
						* @~Japanese
						* @brief コンストラクタ 
						* @details コンストラクタです。 
						*/
						AnimationPlayerImpl(void);

						int initialize(InstanceVisualScene *instanceVisualScene, const Animation *animation);

						/*!
						* @~English
						* @brief Moves the node of a target scene to the position of the specified time.  
						* 
						* @details This changes the transform of the node of instanceVisualScene specified at the time of initialization to the value of the specified time of animation data.
						* @details If the time is out of range, the nearest valid time will be used.
						* 
						* @param time Animation time 
						* @~Japanese
						* @brief 対象のシーンのノードを指定された時刻の位置に移動する 
						* 
						* @details 初期化時に指定されたinstanceVisualSceneのノードのトランスフォームを、 アニメーションデータの指定された時刻の値に変更します。
						* @details 時刻が範囲外の場合、最も近い有効な時刻が使用されます。
						* 
						* @param time アニメーション時刻 
						*/
						void setTime(float time) const;

						/*!
						* @~English
						* @brief Moves the node of a target scene to the position of the specified frame. 
						* 
						* @details This changes the transform of the node of instanceVisualScene specified at the time of initialization to the value of the specified frame of animation data.
						* @details If the time is out of range, the nearest valid frame will be used. This function is synonymous with setTime (starting time + frame period * frame).
						* 
						* @param frame Animation frame number 
						* @~Japanese
						* @brief 対象のシーンのノードを指定されたフレームの位置に移動する 
						* 
						* @details 初期化時に指定されたinstanceVisualSceneのノードのトランスフォームを、 アニメーションデータの指定されたフレームの値に変更します。
						* @details 時刻が範囲外の場合、最も近い有効なフレームが使用されます。この関数はsetTime(開始時刻 + フレーム周期 * frame) と同義です。
						* 
						* @param frame アニメーションフレーム番号 
						*/
						void setFrame(uint32_t frame) const;

						/*!
						* @~English
						* @brief Termination processing 
						* 
						* @details Executes termination processing to unload resources.  
						* @retval SCE_OK Success 
						* @retval <SCE_OK Error 
						* @~Japanese
						* @brief 終了処理 
						* 
						* @details 終了処理を行い、リソースを解放します。 
						* @retval SCE_OK 成功 
						* @retval <SCE_OK エラー 
						*/
						int finalize(void);

						/*!
						* @~English
						* @brief Destructor 
						* @details This is a destructor. 
						* @~Japanese
						* @brief デストラクタ 
						* @details デストラクタです。 
						*/
						virtual ~AnimationPlayerImpl(void);

						/*!
						* @~English
						* @brief Returns animation data. 
						* 
						* @details This returns the animation data used at the time of initialization. 
						* @return The pointer to Animation NULL if not found. 
						* 
						* @~Japanese
						* @brief アニメーションデータを返す 
						* 
						* @details 初期化時に使用されたアニメーションデータを返します。 
						* @return Animationへのポインタ。見つからなかった場合はNULL。 
						* 
						*/
						const Animation* getAnimation(void) const;

						std::string getResourceType(void) const
						{
							return "Graphics::Collada::AnimationPlayer";
						}

					private:
						struct NodeAnimationInstance
						{
							const NodeAnimation *m_nodeAnimation;
							Node *m_node;
						};

						NodeAnimationInstance *m_nodeAnimationInstance;
						unsigned int m_numNodeAnimationInstance;

						const Animation *m_animation;

						InstanceVisualScene *m_instanceVisualScene;

					};






				}

			}

		}

	}
}


#endif /*_COLLADA_IMPL_H */