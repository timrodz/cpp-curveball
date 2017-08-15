/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */
#include "stdafx.h"


#include <graphics/collada/dae/dae.h>
#include "collada_internal.h"
#include <graphics/collada/collada_util.h>
#include <float.h>

using namespace sce::SampleUtil::Graphics;
using namespace sce::SampleUtil::Graphics::Collada::Internal;
using namespace sce::SampleUtil::Graphics::Collada::Internal::controller;
using namespace sce::SampleUtil::Graphics::Collada::Internal::geometry;
using namespace sce::SampleUtil::Graphics::Collada::Internal::Util;


namespace sce
{
namespace SampleUtil
{
namespace Graphics
{
	namespace Collada
	{
		namespace Internal
		{
			namespace controller
			{
				struct JointWeight
				{
					std::string m_joint;
					int        m_jointIndex;
					float      m_weight;
					void  initialize(std::string joint, int jointIndex, float weight)
					{
						m_joint      = joint;
						m_jointIndex = jointIndex;
						m_weight     = weight;
					}
				};

				struct VertexWeight
				{

					int m_numJoints;
					JointWeight *m_jointWeights;
				};
			}
		}
	}
}
}
}



static Matrix4 *readInvBindMatrix(const sce::SampleUtil::Graphics::Collada::Dae::Controller::Skin *skin)
{
	sce::SampleUtil::Graphics::Collada::Dae::Controller::Joints *joints = skin->m_joints;
	sce::SampleUtil::Graphics::Collada::Dae::DataFlow::Input *jointInput = joints->findInput(sce::SampleUtil::Graphics::Collada::Dae::kSemanticJoint);
	SCE_SAMPLE_UTIL_ASSERT(jointInput!= NULL);

	sce::SampleUtil::Graphics::Collada::Dae::DataFlow::Input *invBindMatrixInput = joints->findInput(sce::SampleUtil::Graphics::Collada::Dae::kSemanticInvBindMatrix);
	SCE_SAMPLE_UTIL_ASSERT(jointInput!= NULL);

	sce::SampleUtil::Graphics::Collada::Dae::DataFlow::Source *jointsSource = skin->findSource(Util::sourceToId(jointInput->m_source));
	SCE_SAMPLE_UTIL_ASSERT(jointsSource != NULL);
	sce::SampleUtil::Graphics::Collada::Dae::DataFlow::Source *invBindMatrixSource = skin->findSource(Util::sourceToId(invBindMatrixInput->m_source));
	SCE_SAMPLE_UTIL_ASSERT(invBindMatrixSource!=NULL);

	//unsigned int size = jointsSource->m_nameArray->m_count;

	Matrix4* readInvBindMatrices = new Matrix4[jointsSource->m_nameArray->m_count];

	for(int i=0; i<jointsSource->m_nameArray->m_count; i++){
		readInvBindMatrices[i]  = Util::fromFloatArray( &invBindMatrixSource->m_floatArray->m_data[i*16] );
	}
	return readInvBindMatrices;
}


void Skin::initialize(const std::string &id, sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, const sce::SampleUtil::Graphics::Collada::Dae::Controller::Skin *skin, LibraryGeometries *libraryGeometries)
{
	m_id = id;
	SCE_SAMPLE_UTIL_ASSERT(skin != NULL);

	m_mesh = libraryGeometries->findMesh(sourceToId(skin->m_source));
	SCE_SAMPLE_UTIL_ASSERT(m_mesh != NULL);

	m_invBindMatrices = readInvBindMatrix(skin);

	m_bindShapeMatrix = skin->m_bindShapeMatrix;

	const sce::SampleUtil::Graphics::Collada::Dae::Controller::VertexWeights *c_vertexWeights = skin->m_vertexWeights;
	SCE_SAMPLE_UTIL_ASSERT(c_vertexWeights != NULL);

	const sce::SampleUtil::Graphics::Collada::Dae::DataFlow::Input *jointInput = c_vertexWeights->findInput(sce::SampleUtil::Graphics::Collada::Dae::kSemanticJoint);
	SCE_SAMPLE_UTIL_ASSERT(jointInput != NULL);
	const sce::SampleUtil::Graphics::Collada::Dae::DataFlow::Input *weightInput = c_vertexWeights->findInput(sce::SampleUtil::Graphics::Collada::Dae::kSemanticWeight);
	int stride = std::max(jointInput->m_offset, weightInput->m_offset) +1;

	const sce::SampleUtil::Graphics::Collada::Dae::DataFlow::Source *jointSource = skin->findSource(sourceToId(jointInput->m_source));
	SCE_SAMPLE_UTIL_ASSERT(jointSource != NULL);
	for(int i=0; i<jointSource->m_nameArray->m_count; i++){
		m_jointNameArray.push_back( std::string(jointSource->m_nameArray->m_data[i]) );
	}
	m_joinNameArrayUseSid = jointSource->m_nameArray->m_type == jointSource->m_nameArray->TYPE_NAME;
	const float *weight_array = skin->findSource(sourceToId(weightInput->m_source))->m_floatArray->m_data;


	SCE_SAMPLE_UTIL_ASSERT(weightInput != NULL);
	const int *current_v = c_vertexWeights->m_v;
	//m_numVertexWeight = c_vertexWeights->m_count;
	sce::SampleUtil::Graphics::Collada::Internal::controller::VertexWeight *vweights = new sce::SampleUtil::Graphics::Collada::Internal::controller::VertexWeight[c_vertexWeights->m_count];
	for(int i=0; i<c_vertexWeights->m_count; i++){
		vweights[i].m_numJoints = c_vertexWeights->m_vcount[i];
		vweights[i].m_jointWeights = new Internal::controller::JointWeight[vweights[i].m_numJoints];
		for(int j=0; j<vweights[i].m_numJoints; j++){
			int jointIndex  = current_v[jointInput ->m_offset];
			int weightIndex = current_v[weightInput->m_offset];

			SCE_SAMPLE_UTIL_ASSERT((unsigned int)jointIndex < m_jointNameArray.size());
			std::string jointName = m_jointNameArray.at(jointIndex);
			vweights[i].m_jointWeights[j].initialize( jointName, jointIndex, 
														 weight_array[weightIndex]);
			current_v += stride;
		}
	}

	m_numPrimitiveElements = m_mesh->getNumTriangless();

	//vertexWeights = new VertexWeight2[m_numPrimitiveElements];
	m_skinPrimitiveElements = new Internal::SkinMeshPart[m_numPrimitiveElements];
	for(unsigned int i=0; i<m_numPrimitiveElements ;i++){
		
		m_skinPrimitiveElements[i].initialize(loaderConfig, skin, m_mesh->getTriangles(i), vweights);
		//m_skinningData[i].m_numSkinMatrices    = m_jointNameArray.size();
	}

	for(int i=0; i<c_vertexWeights->m_count; i++){
		delete [] vweights[i].m_jointWeights;
	}
	delete [] vweights;


}


void Skin::dispose(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig)
{
	if(m_skinPrimitiveElements != NULL){
		for(uint32_t i=0; i<m_numPrimitiveElements; i++){
			m_skinPrimitiveElements[i].finalize();
		}
		delete [] m_skinPrimitiveElements;
	}
	m_skinPrimitiveElements = NULL;
}


int SkinMeshPart::initialize(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, 
									 const sce::SampleUtil::Graphics::Collada::Dae::Controller::Skin *skin,
									 const geometry::Triangles *triangles, 
									 sce::SampleUtil::Graphics::Collada::Internal::controller::VertexWeight *vweights
									 )
{

	//m_bindShapeMatrix    = skin->m_bindShapeMatrix;
	m_stride             = (sizeof(JointIndexType) + sizeof(float)) * m_numJointsPerVertex;
	m_numVertices        = triangles->getNumVertices();
	m_jointIndecesOffset = 0;
	m_jointWeightsOffset = sizeof(JointIndexType) * m_numJointsPerVertex;

	loaderConfig->getGraphicsLoader()->createVertexBuffer(&m_stream, m_stride * m_numVertices);
	m_vertices = malloc(m_stride * m_numVertices);

	for(unsigned int i=0; i<m_numVertices; i++){
		int oldIndex = triangles->m_vertexOldIndices[i];
		if(oldIndex < 0){
			// unused vertex
			continue;
		}
		sce::SampleUtil::Graphics::Collada::Internal::controller::VertexWeight &vweight = vweights[oldIndex];

		uintptr_t up = (uintptr_t)m_vertices;
		JointIndexType *p_indices = (JointIndexType*)(up + m_stride*i + m_jointIndecesOffset);
		float   *p_weights = (float*)(up + m_stride* i + m_jointWeightsOffset);

		int weightIndex = 0;
		for(int j=0; j < vweight.m_numJoints; j++){
			if(weightIndex >= m_numJointsPerVertex){
				printf(__FILE__"(%d) Warning: skin %s has more than 8 weights per vertex\n", __LINE__, skin->m_source.c_str());
				break;
			}
			if(vweight.m_jointWeights[j].m_weight > 0){
				p_indices[weightIndex] = (JointIndexType)vweight.m_jointWeights[j].m_jointIndex;
				p_weights[weightIndex] = vweight.m_jointWeights[j].m_weight;
				weightIndex++;
			}
		}
		for(int j=weightIndex; j<m_numJointsPerVertex; j++){
			p_indices[j] = 0;
			p_weights[j] = 0;
		}

		float sum = 0.0f;
		for(int j=0; j<m_numJointsPerVertex; j++){
			sum += p_weights[j];
		}


	}
	void *pdata = m_stream->beginWrite();
	memcpy(pdata, m_vertices, m_stride * m_numVertices);
	m_stream->endWrite();

	return SCE_OK;
}


int SkinMeshPart::finalize(void)
{
	if(m_vertices != NULL){
		free(m_vertices);
		m_vertices = NULL;
	}
	sce::SampleUtil::destroy(m_stream);
	return SCE_OK;
}



void LibraryControllers::initialize(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, 
									sce::SampleUtil::Graphics::Collada::Dae::Controller::LibraryControllers *library_controllers, 
				geometry::LibraryGeometries* libraryGeometries)
{
	for(unsigned int i=0; i<library_controllers->m_controllers.size(); i++){
		Skin *s = new Skin();
		sce::SampleUtil::Graphics::Collada::Dae::Controller::Controller *c = library_controllers->m_controllers.at(i);
		s->initialize(c->m_id, loaderConfig, c->m_skin, libraryGeometries);
		m_skins.push_back(s);
	}
}

Skin *LibraryControllers::findSkin(std::string id) 
{

	for(unsigned int i=0; i<m_skins.size(); i++){
		if(m_skins.at(i)->m_id == id){

			return m_skins.at(i);
		}
	}
	return NULL;
}

void LibraryControllers::dispose(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig)
{
	for(unsigned int i=0; i<m_skins.size(); i++){
		m_skins.at(i)->dispose(loaderConfig);
		delete m_skins.at(i);
	}
	m_skins.clear();
}


void Collada::Internal::LibraryTransformAnimations::initialize(const sce::SampleUtil::Graphics::Collada::Dae::Animation::LibraryAnimations *libraryAnimations)
{
	m_numAnimations = libraryAnimations->m_animations.size();
	m_animations = new TransformAnimation*[m_numAnimations];
	for(unsigned int i=0; i<m_numAnimations; i++){
		m_animations[i] = new TransformAnimation(libraryAnimations->m_animations.at(i));
	}
}

std::pair<float,float> Collada::Internal::LibraryTransformAnimations::getTimeRange(void)
{
	std::pair<float,float> ret(FLT_MAX, FLT_MIN);
	for(unsigned int i=0; i<m_numAnimations; i++){
		std::pair<float,float> range = m_animations[i]->getTimeRange();
		ret = std::pair<float,float>(std::min(ret.first, range.first),
			                           std::max(ret.second, range.second));
	}
	return ret;
}

void Collada::Internal::LibraryTransformAnimations::dispose(void)
{
	if(m_animations != NULL){
		for(unsigned int i=0; i<m_numAnimations; i++){
			delete m_animations[i];
		}
		delete [] m_animations;
		m_animations = NULL;
	}
}

Collada::Internal::LibraryTransformAnimations::~LibraryTransformAnimations(void)
{
	dispose();
}

