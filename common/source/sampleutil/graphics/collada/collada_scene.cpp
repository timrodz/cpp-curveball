/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#if _SCE_TARGET_OS_PSP2
#include <alloca.h>
#endif 
#if _SCE_TARGET_OS_WINDOWS
#include <malloc.h>
#define alloa _malloca
#endif

#include <stdlib.h>

#include "graphics/collada/collada_internal.h"
#include "graphics/collada/collada_scene.h"
#include <sampleutil/graphics/collada.h>



namespace ssg = sce::SampleUtil::Graphics;
namespace ssgc = sce::SampleUtil::Graphics::Collada;
namespace ssgci = sce::SampleUtil::Graphics::Collada::Internal;

using namespace ssgc::Internal::Util;
using namespace ssgc::Internal::controller;
using namespace ssgc::Internal::geometry;




ssgc::Internal::TransformAnimationTargetInstance::TransformAnimationTargetInstance(void)
{
	m_animation = NULL;
	m_targetTransElems = NULL;
}

ssgc::Internal::TransformAnimationTargetInstance::~TransformAnimationTargetInstance(void)
{
	finalize();
}


int ssgc::Internal::TransformAnimationTargetInstance::initialize(Internal::TransformAnimation *animation, Internal::NodeInternal *sceneRootNode)
{
	m_animation = animation;

	m_targetTransElems = new Internal::TransformationElement*[animation->getNumChannels()];
	for(unsigned int i=0; i<animation->getNumChannels(); i++){
		const Internal::TransformAnimationTarget &animTarget = animation->getChannel(i)->getTarget();

		NodeInternal *nodeInstance = (NodeInternal *)sceneRootNode->findNodeById(animTarget.getTargetNodeId());
		Internal::TransformationElement* transElem = nodeInstance->getTransformationElementBySid(animTarget.getTargetNodeSid());

		if(transElem == NULL){
			printf("animTarget %s/%s is not found\n", animTarget.getTargetNodeId().c_str(), animTarget.getTargetNodeSid().c_str());
		}

		m_targetTransElems[i] = transElem;
	}
	return SCE_OK;
}

int ssgc::Internal::TransformAnimationTargetInstance::finalize(void)
{
	if(m_targetTransElems != NULL){
		delete [] m_targetTransElems;
		m_targetTransElems = NULL;
	}
	return SCE_OK;
}


void ssgc::Internal::TransformAnimationTargetInstance::setTime(float time)
{
	for(unsigned int i=0; i<m_animation->getNumChannels(); i++){
		const Internal::TransformAnimationChannel *channel = m_animation->getChannel(i);

		if(m_targetTransElems[i] != NULL){
			const Internal::TransformAnimationTarget &animTarget = channel->getTarget();

			switch(animTarget.getType())
			{
			case Internal::TransformAnimationTarget::TYPE_MATRIX_ELEM: 
				{
					m_targetTransElems[i]->setElem(channel->getTarget().getMatrixElemCol(), 
						channel->getTarget().getMatrixElemRow(), 
						channel->getFloatValue(time));
					break;
				}
			case Internal::TransformAnimationTarget::TYPE_ANGLE:
				{
					m_targetTransElems[i]->setAngle(channel->getFloatValue(time));
					break;
				}
			case Internal::TransformAnimationTarget::TYPE_VECTOR:
				{
					sce::Vectormath::Simd::Aos::Vector3 v = channel->getVectorValue(time);
					m_targetTransElems[i]->setVector(v);
					break;
				}
			case Internal::TransformAnimationTarget::TYPE_MATRIX:
				{
					m_targetTransElems[i]->setMatrix(channel->getMatrixValue(time));
					break;
				}
			case Internal::TransformAnimationTarget::TYPE_VECTOR_X:
				{
					float v = channel->getFloatValue(time);
					//printf(__FILE__"(%d) v=%f\n", __LINE__, v);
					m_targetTransElems[i]->setVectorX(v);
					break;
				}
			case Internal::TransformAnimationTarget::TYPE_VECTOR_Y:
				{
					m_targetTransElems[i]->setVectorY(channel->getFloatValue(time));
					break;
				}
			case Internal::TransformAnimationTarget::TYPE_VECTOR_Z:
				{
					m_targetTransElems[i]->setVectorZ(channel->getFloatValue(time));
					break;
				}
			default:
				SCE_SAMPLE_UTIL_ASSERT(0);
			}
		}
	}
}









static const ssgc::Node *findSkeletonNodeBySid(const std::vector<const ssgc::Node *> &skeletonNodes, std::string sid)
{
	for(unsigned int i=0; i<skeletonNodes.size(); i++){
		const ssgc::Node *ret = skeletonNodes.at(i)->findNodeBySid(sid);
		if(ret != NULL){
			return ret;
		}
	}
	return NULL;
}

static const ssgc::Node *findSkeletonNodeById(const std::vector<const ssgc::Node *> &skeletonNodes, std::string sid)
{
	for(unsigned int i=0; i<skeletonNodes.size(); i++){
		const ssgc::Node *ret = skeletonNodes.at(i)->findNodeById(sid);
		if(ret != NULL){
			return ret;
		}
	}
	return NULL;
}

// This constructor must be called after initializing all nods in a sceneInstance.
int ssgci::InstanceMeshImpl::cloneFrom(ssgc::ColladaLoader* loaderConfig, 
									   const ssgci::InstanceMeshImpl *instanceMesh,
									   const ssgc::Node *rootNode
									 )
{
	m_mesh = instanceMesh->m_mesh;
	m_shaderParameterManager = instanceMesh->m_shaderParameterManager;
	SCE_SAMPLE_UTIL_ASSERT(m_shaderParameterManager != NULL);

	m_numMeshParts = instanceMesh->m_numMeshParts;
	m_meshParts = new InstanceMeshPartImpl[m_numMeshParts];
	for(uint32_t i=0; i<m_numMeshParts; i++){
		m_meshParts[i].copyFrom(&instanceMesh->m_meshParts[i]);
		//m_meshParts[i].applyShaderParameterManager(instanceParameterManager);
	}

	SCE_SAMPLE_UTIL_ASSERT(instanceMesh != NULL);

	if(instanceMesh->m_skeleton == NULL){
		m_instanceSkin = NULL;
		return 0;
	}

	const std::vector<std::string> *jointNameArray = instanceMesh->m_mesh->getJointNameArray();
	const Matrix4** jointMatrices  = (const Matrix4**)alloca(sizeof(Matrix4*) * jointNameArray->size());

	std::vector<const Node*> skeletonNodes;
	// construct skeleton name list
	
	const std::vector<std::string>   *skeletons = instanceMesh->m_skeleton;
	for(unsigned int i=0; i<skeletons->size(); i++){
		std::string nodeId = sourceToId(skeletons->at(i));
		const Node *ni = rootNode->findNodeById(nodeId);
		SCE_SAMPLE_UTIL_ASSERT(ni != NULL);
		skeletonNodes.push_back(ni);
	}
	if(skeletonNodes.size() == 0){
		skeletonNodes.push_back(rootNode); //work around for softimage+crosswalk.
	}

	// find skeleton node
	for(unsigned int i=0; i<jointNameArray->size(); i++){
		std::string jointName = jointNameArray->at(i);

		const Node *jointNode = NULL;
		if(m_mesh->getJointNameType() == m_mesh->kJointNameTypeSid){
			jointNode = ::findSkeletonNodeBySid(skeletonNodes, jointName);
		}else{
			jointNode = ::findSkeletonNodeById(skeletonNodes, jointName);
		}

		if(jointNode== NULL){
			printf("Joint SID=%s is not found\n", jointName.c_str());
			printf("skeletons are (%d)\n", (int)skeletonNodes.size());
			for(uint32_t i=0; skeletonNodes.size(); i++){
				printf("  %s\n", skeletonNodes.at(i)->getId().c_str());
			}

		
		}


		SCE_SAMPLE_UTIL_ASSERT(jointNode != NULL);
		jointMatrices[i] = jointNode->getWorldMatrixPointer();

	}

	m_instanceSkin = new InstanceSkin(loaderConfig, 
		                            jointNameArray->size(),
									instanceMesh->m_mesh->getInvBindMatrices(),
									jointMatrices);
	return 0;
}



ssgc::Internal::NodeInternal::NodeInternal(void)
{
	//m_node2 = NULL;
	m_localMatrix = sce::Vectormath::Simd::Aos::Matrix4::identity();
	m_localToWorld = sce::Vectormath::Simd::Aos::Matrix4::identity();
	m_instanceMeshes = NULL;
	m_numInstanceMeshes = 0;
	m_numChildNodes = 0;
	m_childNodes = NULL;
	m_parentNode = NULL;

}

int ssgc::Internal::NodeInternal::cloneFromWithoutTransform(ssgc::ColladaLoader *loaderConfig, 
						  const Internal::NodeInternal* node,
						  const Internal::NodeInternal* parentNode)
{
	m_id = node->m_id;
	m_sid = node->m_sid;
	m_parentNode = parentNode;
	m_localMatrix = node->m_localMatrix;
	m_numInstanceMeshes = 0;

	m_numChildNodes = node->m_numChildNodes;
	m_childNodes = new Internal::NodeInternal[m_numChildNodes];
	for(unsigned int i=0; i<m_numChildNodes; i++)
	{
		int ret = m_childNodes[i].cloneFromWithoutTransform(loaderConfig, 
			&node->m_childNodes[i],
			this);
		if(ret != SCE_OK){
			return ret;
		}
	}
	return 0;
}


ssgc::Internal::NodeInternal::~NodeInternal(void)
{
	finalize();
}

ssgc::Internal::TransformationElement* ssgc::Internal::NodeInternal::getTransformationElementBySid(std::string sid)
{
	for(unsigned int i=0; i<m_transforms.size(); i++){
		if(m_transforms.at(i)->getSid() == sid){
			return m_transforms.at(i);
		}
	}
	return NULL;
}

Matrix4 ssgc::Internal::NodeInternal::getTransformMatrix(void) const
{
	Matrix4 ret = Matrix4::identity();
	for(unsigned int i=0; i<m_transforms.size(); i++){
		ret =  ret* m_transforms.at(i)->asMatrix();
	}
	ret.setElem(3, 3, 1.0f); //update (3,3), since it may be set to 0 by animation.
	return ret;
}

void ssgc::Internal::NodeInternal::updateWorldMatrixUsingTransform(void) 
{
	if(m_parentNode == NULL){
		// Root Node

	}else{
		Matrix4 transformMatrix = getTransformMatrix();
		m_localToWorld = m_parentNode->m_localToWorld * transformMatrix;
	}

	for(unsigned int i=0; i<m_numChildNodes; i++){
		m_childNodes[i].updateWorldMatrixUsingTransform();
	}
}


void ssgc::Internal::NodeInternal::updateWorldMatrix(void) 
{
	if(m_parentNode == NULL){
		// Root Node
		m_localToWorld = m_localMatrix;
	}else{
		m_localToWorld = m_parentNode->m_localToWorld * m_localMatrix;
	}

	for(unsigned int i=0; i<m_numChildNodes; i++){
		m_childNodes[i].updateWorldMatrix();
	}
}

void ssgc::Internal::NodeInternal::updateSkinMatrices(void)
{
	for(unsigned int i=0; i<m_numInstanceMeshes; i++){
		m_instanceMeshes[i].updateSkinMatrices();
	}
	for(unsigned int i=0; i<m_numChildNodes; i++){
		m_childNodes[i].updateSkinMatrices();
	}
}

void ssgc::Internal::NodeInternal::draw(sce::SampleUtil::Graphics::GraphicsContext *graphicsContext, 
					  sce::Vectormath::Simd::Aos::Matrix4_arg matrix,
					  int renderingStage) const
{
	for(unsigned int i=0; i<m_numInstanceMeshes; i++){
		m_instanceMeshes[i].draw(matrix * m_localToWorld, graphicsContext, renderingStage);
	}

	for(unsigned int i=0; i<m_numChildNodes; i++){
		m_childNodes[i].draw(graphicsContext, matrix, renderingStage);
	}

}


void ssgc::Internal::NodeInternal::cloneMeshFrom(ssgc::ColladaLoader* loaderConfig, 
							   const Internal::NodeInternal* node,
							   const Internal::NodeInternal *rootNode)
{
	SCE_SAMPLE_UTIL_ASSERT(rootNode != NULL);

	if(node != NULL){
		m_numInstanceMeshes = node->m_numInstanceMeshes;
		m_instanceMeshes = new InstanceMeshImpl[m_numInstanceMeshes];
		for(unsigned int i=0; i<node->m_numInstanceMeshes; i++){
			m_instanceMeshes[i].cloneFrom(loaderConfig, 
				&node->m_instanceMeshes[i],
				rootNode);
		}
	}
	for(unsigned int i=0; i<m_numChildNodes; i++){
		m_childNodes[i].cloneMeshFrom(loaderConfig, &node->m_childNodes[i], rootNode);
	}
}

std::string ssgc::Internal::NodeInternal::getId(void) const
{
	return m_id;

}
std::string ssgc::Internal::NodeInternal::getSid(void) const
{
	return m_sid;

}


void ssgc::Internal::NodeInternal::applyShaderParameterManager(ShaderParameterReference *nodeParameterManager)
{
	for(unsigned int i=0; i<m_numInstanceMeshes; i++){
		m_instanceMeshes[i].applyShaderParameterManager(nodeParameterManager);
	}
	for(unsigned int i=0; i<m_numChildNodes; i++){
		m_childNodes[i].applyShaderParameterManager(nodeParameterManager);
	}
}


uint32_t ssgci::InstanceMeshImpl::getNumMeshParts(void) const
{
	return m_numMeshParts;
}
ssgc::InstanceMeshPart *ssgci::InstanceMeshImpl::getMeshPart(uint32_t index)
{
	if(index >= m_numMeshParts){
		return NULL;
	}
	return &m_meshParts[index];
}


int ssgci::InstanceMeshImpl::finalize(void)
{
	int ret;
	if(m_numMeshParts > 0){
		for(unsigned int i=0; i<m_numMeshParts; i++){
			ret = m_meshParts[i].finalize();
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		}
		delete [] m_meshParts;
		m_meshParts = NULL;
		m_numMeshParts = 0;

	}

	m_skeleton = NULL;
	if(m_instanceSkin != NULL){
		delete m_instanceSkin;
		m_instanceSkin = NULL;
	}
	(void)ret;
	return SCE_OK;
}

void ssgci::InstanceMeshImpl::updateSkinMatrices(void)
{
	if(m_instanceSkin != NULL){
		m_instanceSkin->update();
	}
}

void ssgci::InstanceMeshImpl::applyShaderParameterManager(ssgc::ShaderParameterReference *nodeParameterManager)
{
	for(unsigned int i=0; i<m_numMeshParts; i++){
		m_meshParts[i].applyShaderParameterManager(nodeParameterManager);
	}

}

int ssgci::InstanceMeshImpl::addEffect(const ssg::Effect::EffectData *effect,	int renderingStage)
{
	for(unsigned int i=0; i<m_numMeshParts; i++){
		int ret =m_meshParts[i].addEffect(effect,	renderingStage);
		if(ret != SCE_OK){
			return ret;
		}
	}
	return SCE_OK;

}

void ssgci::InstanceMeshImpl::draw(Matrix4_arg world, 
										sce::SampleUtil::Graphics::GraphicsContext *graphicsContext, 
										int renderingStage)
{
	if(m_numMeshParts > 0){
		m_shaderParameterManager->setWorldMatrix(world);
	}

	for(unsigned int i=0; i<m_numMeshParts; i++){
		m_meshParts[i].draw(
			graphicsContext,  
			(m_instanceSkin == NULL) ? NULL : m_instanceSkin->getCurrentJointBuffer(), 
			renderingStage);
	}
}



ssgci::InstanceMeshImpl::InstanceMeshImpl(void)
{
	m_mesh = NULL;
	m_skeleton = NULL;
	m_numMeshParts = 0;
	m_meshParts = NULL;
	m_instanceSkin = NULL;
	m_shaderParameterManager = NULL;
}
ssgci::InstanceMeshImpl::~InstanceMeshImpl(void)
{
	finalize();
}

const ssgc::Mesh* ssgci::InstanceMeshImpl::getMesh(void) const
{
	return m_mesh;
}



int ssgci::InstanceMeshImpl::initialize(ssgc::ColladaLoader *loaderConfig, 
										const ssgc::Mesh *mesh,
										const ssgc::Dae::Controller::InstanceController *daeInstanceController,
										ssgc::Internal::LibrarySkeletons *librarySkeletons,
										ssgc::ColladaData *collada,
										ssgc::Node *rootNode)
{
	m_mesh = dynamic_cast<const ssgci::MeshImpl*>(mesh);

	m_numMeshParts = mesh->getNumMeshParts();
	m_shaderParameterManager = loaderConfig->getMatrixChangeListener();
	SCE_SAMPLE_UTIL_ASSERT(m_shaderParameterManager != NULL);

	if(m_numMeshParts > 0){
		m_meshParts = new InstanceMeshPartImpl[m_numMeshParts];

		for(unsigned int i=0; i<m_numMeshParts; i++){

			const MeshPart *meshPart = mesh->getMeshPart(i);
			std::string materialId = meshPart->getMaterial();

			Dae::Common::InstanceMaterial *instanceMaterial = 
				daeInstanceController->m_bindMaterial->m_techniqueCommon->findInstanceMaterialBySymbol(materialId);

			ssgc::Internal::InstanceMaterial instanceMaterial2;
			instanceMaterial2.initialize(loaderConfig, instanceMaterial);

			const Material *material = collada->getMaterialById(instanceMaterial2.getTargetId());
			instanceMaterial2.finalize();

			SCE_SAMPLE_UTIL_ASSERT(material != NULL);
			const Graphics::Effect::EffectData *effect = material->getEffect();

			m_meshParts[i].initialize(loaderConfig, 
				meshPart,
				effect);

		}
	}

	/* --------------------------------------------------------------------- */
	m_skeleton = new Internal::Skeleton;
	librarySkeletons->push_back(m_skeleton); // For deleting skeleton later.

	//printf(__FILE__"(%d) daeInstanceController->m_skeletons.size()=%d\n", __LINE__, daeInstanceController->m_skeletons.size());
	for(uint32_t i=0; i<daeInstanceController->m_skeletons.size(); i++){
		std::string skel = daeInstanceController->m_skeletons[i];
		m_skeleton->push_back( skel );
	}

	const std::vector<std::string> *jointNameArray = m_mesh->getJointNameArray();
	const Matrix4** jointMatrices  = (const Matrix4**)alloca(sizeof(Matrix4*) * jointNameArray->size());

	std::vector<const Node*> skeletonNodes;
	//printf(__FILE__"(%d) check\n", __LINE__);
	// construct skeleton name list
	for(unsigned int i=0; i<m_skeleton->size(); i++){
		std::string nodeId = sourceToId(m_skeleton->at(i));
		//printf(__FILE__"(%d) nodeId=%s\n", __LINE__, nodeId.c_str());
		Node *ni = rootNode->findNodeById(nodeId);
		SCE_SAMPLE_UTIL_ASSERT(ni != NULL);
		skeletonNodes.push_back(ni);
	}
	if(skeletonNodes.size() == 0){
		// For collada exported from SoftImage(crosswalk)
		skeletonNodes.push_back(rootNode);
	}

	// find skeleton node
	for(unsigned int i=0; i<jointNameArray->size(); i++){
		std::string jointName = jointNameArray->at(i);
		const Node *jointNode = NULL;
		if(mesh->getJointNameType() == mesh->kJointNameTypeSid){
			jointNode = ::findSkeletonNodeBySid(skeletonNodes, jointName);
		}else{
			jointNode = ::findSkeletonNodeById(skeletonNodes, jointName);
		}
		if(jointNode== NULL){
			printf("Joint SID=%s is not found\n", jointName.c_str());
			printf("skeletons are (%d)\n", (int)skeletonNodes.size());
			for(uint32_t i=0; skeletonNodes.size(); i++){
				printf("  %s\n", skeletonNodes.at(i)->getId().c_str());
			}

		
		}

		SCE_SAMPLE_UTIL_ASSERT(jointNode != NULL);
		jointMatrices[i] = jointNode->getWorldMatrixPointer();

	}
	m_instanceSkin = new InstanceSkin(loaderConfig, 
		                            jointNameArray->size(),
									m_mesh->getInvBindMatrices(),
									jointMatrices);



	return SCE_OK;
}


int ssgci::InstanceMeshImpl::initialize(ssgc::ColladaLoader *loaderConfig, 
										const ssgc::Mesh *mesh,
										const ssgc::Dae::Geometry::InstanceGeometry *daeInstanceGeometry, 
										ssgc::Internal::LibrarySkeletons *librarySkeletons,
										ssgc::ColladaData *collada,
										ssgc::Node *rootNode)
{
	m_mesh = dynamic_cast<const ssgci::MeshImpl*>(mesh);

	m_skeleton = NULL;
	m_instanceSkin = NULL;
	m_numMeshParts = mesh->getNumMeshParts();

	m_shaderParameterManager = loaderConfig->getMatrixChangeListener();
	SCE_SAMPLE_UTIL_ASSERT(m_shaderParameterManager != NULL);

	if(m_numMeshParts > 0){
		m_meshParts = new InstanceMeshPartImpl[m_numMeshParts];
		for(unsigned int i=0; i<m_numMeshParts; i++){

			const MeshPart *meshPart = mesh->getMeshPart(i);
			std::string materialId = meshPart->getMaterial();

			Dae::Common::InstanceMaterial *instanceMaterial = daeInstanceGeometry->m_bindMaterial->m_techniqueCommon->findInstanceMaterialBySymbol( materialId);
			if(instanceMaterial == NULL){
				printf("materialId %s was not found\n", materialId.c_str());
				return -1;
			}
			ssgc::Internal::InstanceMaterial instanceMaterial2;
			instanceMaterial2.initialize(loaderConfig, instanceMaterial);

			const Material *material = collada->getMaterialById(instanceMaterial2.getTargetId());
			instanceMaterial2.finalize();


			SCE_SAMPLE_UTIL_ASSERT(material != NULL);
			const Graphics::Effect::EffectData *effect = material->getEffect();

			m_meshParts[i].initialize(loaderConfig, 
				mesh->getMeshPart(i),
				effect);

		}
	}
	return SCE_OK;
}


ssgci::InstanceMeshImpl::InstanceSkin::InstanceSkin(ssgc::ColladaLoader* loader, 
											   uint32_t numJoints,
											   const sce::Vectormath::Simd::Aos::Matrix4 *jointInvMatrices,
											   const sce::Vectormath::Simd::Aos::Matrix4 **refJointMatrices
											   )
{
	m_numJoints = numJoints;
	m_currentJointIndex = 0;
	GraphicsLoader *gl = loader->getGraphicsLoader();
	gl->createUniformBuffer(&m_jointInfo[0], sizeof(Matrix4)  * numJoints); 
	gl->createUniformBuffer(&m_jointInfo[1], sizeof(Matrix4)  * numJoints); 
	gl->createUniformBuffer(&m_jointInfo[2], sizeof(Matrix4)  * numJoints); 

	m_jointInvMatrices = jointInvMatrices;
	m_jointMatrices = new const Matrix4*[numJoints];
	memcpy(m_jointMatrices, refJointMatrices, sizeof(Matrix4*)*numJoints);
}

void ssgci::InstanceMeshImpl::InstanceSkin::update(void)
{
	m_currentJointIndex = (m_currentJointIndex + 1)%3;

	Matrix4 *matrices = (Matrix4 *)m_jointInfo[m_currentJointIndex]->beginWrite();
	for(unsigned int j=0; j<m_numJoints; j++){
		matrices[j] = transpose((*m_jointMatrices[j]) * m_jointInvMatrices[j]);
	}
	m_jointInfo[m_currentJointIndex]->endWrite();
}

ssgci::InstanceMeshImpl::InstanceSkin::~InstanceSkin(void)
{
	sce::SampleUtil::destroy(m_jointInfo[0]);
	sce::SampleUtil::destroy(m_jointInfo[1]);
	sce::SampleUtil::destroy(m_jointInfo[2]);

	delete [] m_jointMatrices;
}




int ssgc::Internal::NodeInternal::initialize(Dae::Scene::Node *daeNode, ssgc::Internal::NodeInternal* parent)
{
	m_id = daeNode->m_id;
	m_sid = daeNode->m_sid;
	m_parentNode = parent;

	m_numChildNodes = daeNode->m_nodes.size();
	if(m_numChildNodes > 0){
		m_childNodes = new Internal::NodeInternal[m_numChildNodes];
		for(unsigned int i=0; i<m_numChildNodes; i++){
			m_childNodes[i].initialize(	daeNode->m_nodes.at(i), this);
		}
	}
	return SCE_OK;
}

int ssgc::Internal::NodeInternal::initializeMesh(ssgc::ColladaLoader *loaderConfig, 
														Dae::Scene::Node *daeNode,
														ColladaData *collada,
														LibrarySkeletons *librarySkeletons,
														Internal::NodeInternal *rootNode)
{
	m_numInstanceMeshes = daeNode->m_instanceController.size() + daeNode->m_instanceGeometry.size();
	m_instanceMeshes = new InstanceMeshImpl[m_numInstanceMeshes];

	int index=0;
	for(unsigned int i=0; i<daeNode->m_instanceController.size(); i++){
		std::string controllerId = ssgc::Internal::Util::urlToId(daeNode->m_instanceController.at(i)->m_url);

		m_instanceMeshes[index].initialize(loaderConfig, 
			collada->getMeshByControllerId(controllerId),
			daeNode->m_instanceController[i],
			librarySkeletons,
			collada,
			rootNode);

		index++;
	}

	for(unsigned int i=0; i<daeNode->m_instanceGeometry.size(); i++){
		std::string geometryId = ssgc::Internal::Util::urlToId(daeNode->m_instanceGeometry.at(i)->m_url);
		const Mesh *mesh = collada->getMeshByGeometryId(geometryId);
		if(mesh != NULL){
			m_instanceMeshes[index].initialize(loaderConfig,
				mesh,
				daeNode->m_instanceGeometry[i],
				librarySkeletons,
				collada,
				rootNode);
			index++;
		}else{
			printf("Geometry %s was not found\n", geometryId.c_str());
			return -1;
		}
	}

	for(unsigned int i=0; i<daeNode->m_matrices.size(); i++){
		m_transforms.push_back(daeNode->m_matrices.at(i)->duplicate());
	}



	for(unsigned int i=0; i<m_numChildNodes; i++){
		m_childNodes[i].initializeMesh(
			loaderConfig, 
			daeNode->m_nodes.at(i),
			collada,
			librarySkeletons,
			rootNode);
	}

	m_localMatrix = getTransformMatrix();

	return SCE_OK;
}

int ssgc::Internal::NodeInternal::finalize(void)
{
	//printf(__FILE__"(%d) check\n", __LINE__);
	for(unsigned int i=0; i<m_transforms.size(); i++){
		delete m_transforms.at(i);
	}
	//printf(__FILE__"(%d) check\n", __LINE__);
	m_transforms.clear();
	//printf(__FILE__"(%d) check\n", __LINE__);
	for(unsigned int i=0; i<m_numChildNodes; i++){
		m_childNodes[i].finalize();
	}
	//printf(__FILE__"(%d) check\n", __LINE__);
	delete [] m_childNodes;
	m_childNodes = NULL;
	m_numChildNodes = 0;


	for(unsigned int i=0; i<m_numInstanceMeshes; i++){
		m_instanceMeshes[i].finalize();
	}
	delete [] m_instanceMeshes;
	m_instanceMeshes = NULL;
	m_numInstanceMeshes = 0;


	return SCE_OK;
}


ssgc::Node *ssgc::Internal::NodeInternal::findNodeBySid(std::string sid)
{
	if(m_sid == sid){
		return this;
	}
	for(unsigned int i=0; i<m_numChildNodes; i++){
		Node *ret = m_childNodes[i].findNodeBySid(sid);
		if(ret != NULL){
			return ret;
		}
	}
	return NULL;
}

ssgc::Node *ssgc::Internal::NodeInternal::findNodeById(std::string id)
{
	if(m_id == id){
		return this;
	}
	for(unsigned int i=0; i<m_numChildNodes; i++){
		Node *ret = m_childNodes[i].findNodeById(id);
		if(ret != NULL){
			return ret;
		}
	}
	return NULL;
}

const ssgc::Node *ssgc::Internal::NodeInternal::findNodeBySid(std::string sid) const
{
	if(m_sid == sid){
		return this;
	}
	for(unsigned int i=0; i<m_numChildNodes; i++){
		const Node *ret = m_childNodes[i].findNodeBySid(sid);
		if(ret != NULL){
			return ret;
		}
	}
	return NULL;
}

const ssgc::Node *ssgc::Internal::NodeInternal::findNodeById(std::string id) const
{
	if(m_id == id){
		return this;
	}
	for(unsigned int i=0; i<m_numChildNodes; i++){
		const Node *ret = m_childNodes[i].findNodeById(id);
		if(ret != NULL){
			return ret;
		}
	}
	return NULL;
}




int ssgci::InstanceMeshPartImpl::initialize(
	ssgc::ColladaLoader *loaderConfig, 
	const ssgc::MeshPart *meshPart,
	const sce::SampleUtil::Graphics::Effect::EffectData *effect)
{
	m_loaderConfig = loaderConfig;
	m_meshPart = meshPart;

	return addEffect(effect, 0);
}

int ssgci::InstanceMeshPartImpl::addEffect(
	const sce::SampleUtil::Graphics::Effect::EffectData *effect,
	int renderingStage)
{
	Internal::EffectInstance *ei = new Internal::EffectInstance;
	int ret = ei->initialize(m_loaderConfig, 
		m_meshPart,
		effect,
		renderingStage);
	if(ret != SCE_OK){
		delete ei;
		return ret;
	}

	m_effectInstances.push_back(ei);
	return SCE_OK;                           
}


void ssgci::InstanceMeshPartImpl::applyShaderParameterManager(ssgc::ShaderParameterReference *parameterManager )
{
	for(uint32_t i=0; i<m_effectInstances.size(); i++){
		m_effectInstances.at(i)->applyNewParameterManager(parameterManager);
	}
}

void ssgci::InstanceMeshPartImpl::copyFrom(InstanceMeshPartImpl *instancePrimitiveElement )
{

	m_loaderConfig       = instancePrimitiveElement->m_loaderConfig;
	m_meshPart           = instancePrimitiveElement->m_meshPart;

	for(uint32_t i=0; i<instancePrimitiveElement->m_effectInstances.size(); i++){
		Internal::EffectInstance *ei = new Internal::EffectInstance;
		ei->cloneFrom(*instancePrimitiveElement->m_effectInstances.at(i));
		m_effectInstances.push_back(ei);
	}
}



int ssgci::InstanceMeshPartImpl::finalize(void)
{
	int ret;
	for(uint32_t i=0; i<m_effectInstances.size(); i++){
		ret = m_effectInstances.at(i)->finalize();
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		delete m_effectInstances.at(i);
	}
	m_effectInstances.clear();
	(void)ret;
	return SCE_OK;
}


void ssgci::InstanceMeshPartImpl::draw(
	sce::SampleUtil::Graphics::GraphicsContext *graphicsContext,
	sce::SampleUtil::Graphics::UniformBuffer *skinMatrices,
	int renderingStage)
{
	for(uint32_t j=0; j<m_effectInstances.size(); j++){
		Internal::EffectInstance *cgEffect = m_effectInstances.at(j);
		cgEffect->render( graphicsContext, 
			m_meshPart->getVertexBuffer(), 
			//m_meshPart->getVertexBuffer(1), 
			skinMatrices, 
			m_meshPart->getIndexBuffer(),
			m_meshPart->getNumIndices(),
			renderingStage);

	}
}

ssgci::InstanceMeshPartImpl::InstanceMeshPartImpl(void)
{
	m_loaderConfig = NULL;
	m_meshPart = NULL;

}
ssgci::InstanceMeshPartImpl::~InstanceMeshPartImpl(void)
{
	finalize();
}


// ------------------------------------------------------------------------------

int ssgci::VisualSceneImpl::initialize(ssgc::ColladaLoader *loaderConfig, 
									   ssgc::Dae::Scene::VisualScene *daeVisualScene,
											ssgc::ColladaData *collada, 
											ssgc::Internal::LibrarySkeletons *librarySkeletons,
											ssgc::Internal::LibraryTransformAnimations *libraryTransformAnimations
											)
{
	m_collada = collada;
	m_loaderConfig = loaderConfig;

	m_id = daeVisualScene->m_id;
	m_name = daeVisualScene->m_name;

	Internal::NodeInternal *rootNode = new Internal::NodeInternal;
	rootNode->initialize(&daeVisualScene->m_rootNode, NULL);

	rootNode->initializeMesh(loaderConfig, 
		&daeVisualScene->m_rootNode,
		collada,
		librarySkeletons,
		rootNode);

	//int ret = m_transformAnimationPlayer.initialize(libraryTransformAnimations, &m_rootNode);
	//SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	rootNode->updateSkinMatrices();

	m_rootNode = rootNode;

	return SCE_OK;
}

int ssgci::VisualSceneImpl::finalize(void)
{
	if(	m_rootNode != NULL ){
		Internal::NodeInternal *ni = (Internal::NodeInternal *)m_rootNode;
		ni->finalize();
		delete m_rootNode;
		m_rootNode = NULL;
	}
	return SCE_OK;
}


ssgci::VisualSceneImpl::VisualSceneImpl(void)
{
	m_rootNode = NULL;
}
ssgci::VisualSceneImpl::~VisualSceneImpl(void)
{
	finalize();
}

std::string  ssgci::VisualSceneImpl::getId(void) const 
{
	return m_id;
}

std::string  ssgci::VisualSceneImpl::getName(void) const 
{
	return m_name;
}


ssgc::Node *ssgci::VisualSceneImpl::findNodeById(std::string id)
{
	return m_rootNode->findNodeById(id);
}
ssgc::Node *ssgci::VisualSceneImpl::findNodeBySid(std::string sid)
{
	return m_rootNode->findNodeBySid(sid);
}
const ssgc::Node *ssgci::VisualSceneImpl::findNodeById(std::string id) const
{
	return m_rootNode->findNodeById(id);
}
const ssgc::Node *ssgci::VisualSceneImpl::findNodeBySid(std::string sid) const
{
	return m_rootNode->findNodeBySid(sid);
}

ssgc::Node *ssgci::VisualSceneImpl::getRootNode(void)
{
	return m_rootNode;
}
const ssgc::Node *ssgci::VisualSceneImpl::getRootNode(void) const
{
	return m_rootNode;
}




ssgci::InstanceVisualSceneImpl::InstanceVisualSceneImpl(void)
{
	m_rootNode = NULL;
	m_visualScene = NULL;

}
ssgci::InstanceVisualSceneImpl::~InstanceVisualSceneImpl(void)
{
	finalize();
}




int ssgci::InstanceVisualSceneImpl::initialize(const ssgc::VisualScene *_visualScene)
{
	const ssgci::VisualSceneImpl *visualScene = (const ssgci::VisualSceneImpl *)_visualScene;
	m_visualScene = visualScene;
	const Internal::NodeInternal *rootNode = (const Internal::NodeInternal *)visualScene->m_rootNode;

	Internal::NodeInternal *newRootNode = new Internal::NodeInternal();

	SCE_SAMPLE_UTIL_ASSERT(newRootNode != NULL);

	newRootNode->cloneFromWithoutTransform(visualScene->m_loaderConfig,
		rootNode,
		NULL);

	newRootNode->cloneMeshFrom(visualScene->m_loaderConfig,
		rootNode,
		newRootNode);

	newRootNode->updateWorldMatrix();

	m_rootNode = newRootNode;
	return SCE_OK;
}

int ssgci::InstanceVisualSceneImpl::finalize(void)
{
	if(m_rootNode != NULL){
		Internal::NodeInternal *root = (Internal::NodeInternal *)m_rootNode;
		int ret = root->finalize();
		if(ret != SCE_OK){
			return ret;
		}
		delete m_rootNode;
		m_rootNode = NULL;
	}
	return SCE_OK;

}


void ssgci::InstanceVisualSceneImpl::draw(sce::SampleUtil::Graphics::GraphicsContext *graphicsContext, 
									 sce::Vectormath::Simd::Aos::Matrix4_arg matrix,
									 uint32_t renderingStage) const
{
	m_rootNode->draw(graphicsContext, matrix, renderingStage);
}


ssgc::Node *ssgci::InstanceVisualSceneImpl::findNodeById(std::string id)
{
	return m_rootNode->findNodeById(id);
}
ssgc::Node *ssgci::InstanceVisualSceneImpl::findNodeBySid(std::string sid)
{
	return m_rootNode->findNodeBySid(sid);
}
const ssgc::Node *ssgci::InstanceVisualSceneImpl::findNodeById(std::string id) const
{
	return m_rootNode->findNodeById(id);
}
const ssgc::Node *ssgci::InstanceVisualSceneImpl::findNodeBySid(std::string sid) const
{
	return m_rootNode->findNodeBySid(sid);
}


ssgc::Node *ssgci::InstanceVisualSceneImpl::getRootNode(void)
{
	return m_rootNode;
}
const ssgc::Node *ssgci::InstanceVisualSceneImpl::getRootNode(void) const
{
	return m_rootNode;
}


const ssgc::VisualScene *ssgci::InstanceVisualSceneImpl::getVisualScene(void) const
{
	return m_visualScene;
}

int ssgc::createInstanceVisualScene(InstanceVisualScene **outInstanceVisualScene, 
                                     const VisualScene *visualScene)
{
	if(visualScene == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(outInstanceVisualScene == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssgci::InstanceVisualSceneImpl* impl = new ssgci::InstanceVisualSceneImpl();
	int ret = impl->initialize(visualScene);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outInstanceVisualScene = impl;
	return SCE_OK;
}
