/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <sampleutil/graphics/collada.h>
#include "graphics/collada/collada_util.h"
#include "graphics/collada/dae/dae.h"
#include "graphics/collada/collada_internal.h"


namespace ssg = sce::SampleUtil::Graphics;
namespace ssgc = sce::SampleUtil::Graphics::Collada;
namespace ssgci = sce::SampleUtil::Graphics::Collada::Internal;
using namespace ssg;
using namespace ssg::Collada::Internal::Util;
using namespace ssg::Collada::Internal::geometry;

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
			struct VertexElementStream
			{
				Dae::VertexSemantic m_semantic;
				uint32_t m_dimension;
				uint32_t m_indexOffset;
				uint32_t m_offsetInVertex;
				uint32_t m_set;
				std::string m_sourceId;
				const Dae::DataFlow::FloatArray *m_source;
			};

			struct Indices
			{
				uint16_t m_numIndices;
				uint16_t *m_indices;

				Indices(void)
				{
					m_numIndices = 0;
					m_indices = NULL;
				}
				Indices(uint16_t numIndices, const uint16_t *indices)
				{
					m_numIndices = numIndices;
					m_indices = new uint16_t[numIndices];
					for(uint16_t i=0; i<numIndices; i++){
						m_indices[i] = indices[i];
					}
				}

				Indices(const Indices& indices)
				{
					m_numIndices = indices.m_numIndices;
					m_indices = new uint16_t[indices.m_numIndices];
					for(uint16_t i=0; i<indices.m_numIndices; i++){
						m_indices[i] = indices.m_indices[i];
					}
				}
				~Indices(void)
				{
					m_numIndices = 0;
					if(m_indices != NULL){
						delete [] m_indices;
						m_indices = NULL;
					}
				}

				size_t getHash(void) const
				{
					size_t ret=0;
					for(uint16_t i=0; i<m_numIndices; i++){
						ret = ret *3 + m_indices[i];
					}
					return ret;
				}

				bool operator < (const Indices& idx2) const
				{
					if(this->m_numIndices != idx2.m_numIndices){
						return this->m_numIndices < idx2.m_numIndices;
					}
					for(uint16_t i=0; i<this->m_numIndices; i++){
						if(this->m_indices[i] != idx2.m_indices[i]){
							return this->m_indices[i] < idx2.m_indices[i];
						}
					}
					return false;
				}
				bool operator == (const Indices& idx2) const
				{
					if(this->m_numIndices != idx2.m_numIndices){
						return false;
					}
					for(uint16_t i=0; i<this->m_numIndices; i++){
						if(this->m_indices[i] != idx2.m_indices[i]){
							return false;
						}
					}
					return true;
				}
			};

			class IndexTraits
			{
			public:
				enum
				{	
					bucket_size = 4,	
					min_buckets = 8
				};	
				size_t operator( )( const Indices& indices ) const
				{
					return indices.getHash();
				}
				bool operator( )( 
					const Indices& _Key1,
					const Indices& _Key2
					) const
				{
					return _Key1 < _Key2;
				}
			};


			struct VertexElementStreamManager
			{

				uint32_t m_numStreams;
				VertexElementStream *m_streams;
				uint32_t m_indexStride;
				uint32_t m_numFloatInVertex;
				uint32_t m_vertexIndexOffset;

				sce::SampleUtil::Map<Indices, uint16_t, IndexTraits>::Type m_indexMap;

			private:
				static uint32_t _getNumStreams(const Dae::Geometry::Triangles *dae_triangles,
					const Dae::Geometry::Mesh *mesh);
			public:


				int initialize(const Dae::Geometry::Triangles *dae_triangles,
					const Dae::Geometry::Mesh *mesh);

				void read(float *dst, const uint16_t *daeTriangleIndices);

				uint16_t getNewIndex(const uint16_t *daeTriangleIndices);


				VertexElementStream *findStream(Dae::VertexSemantic semantic, uint32_t set) const;

				VertexElementStream *findStreamBySourceId(std::string sourceId) const;

				int finalize(void);

				~VertexElementStreamManager(void)
				{
					finalize();
				}

			};
	}
	}
}
}
}
using namespace ssg::Collada::Internal;




uint32_t VertexElementStreamManager::_getNumStreams(const ssg::Collada::Dae::Geometry::Triangles *dae_triangles,
													const ssg::Collada::Dae::Geometry::Mesh *mesh)
{
	Dae::DataFlow::Input *vertexInput = dae_triangles->findInput(Dae::kSemanticVertex);
	const Dae::Geometry::Vertices *dae_vertices = mesh->findVertices(Internal::Util::sourceToId(vertexInput->m_source));
	return dae_vertices->getInputs().size() + dae_triangles->getInputs().size() - 1;


}



int VertexElementStreamManager::initialize(const ssg::Collada::Dae::Geometry::Triangles *dae_triangles,
										   const ssg::Collada::Dae::Geometry::Mesh *mesh)
{
	uint32_t maxOffset = 0;
	uint32_t offsetInVertex = 0;
	m_numStreams = _getNumStreams(dae_triangles,  mesh);
	m_streams = new VertexElementStream[m_numStreams];

	Dae::DataFlow::Input *vertexInput = dae_triangles->findInput(Dae::kSemanticVertex);
	const Dae::Geometry::Vertices *dae_vertices = mesh->findVertices(Internal::Util::sourceToId(vertexInput->m_source));

	uint32_t numInputsInVertex = dae_vertices->getInputs().size();
	m_vertexIndexOffset = vertexInput->m_offset;
	for(uint32_t i=0; i<numInputsInVertex; i++){
		Dae::DataFlow::Input *input = dae_vertices->getInputs().at(i);
		const Dae::DataFlow::Source *source = mesh->findSource(Internal::Util::sourceToId(input->m_source));
		m_streams[i].m_semantic = input->m_semantic;
		m_streams[i].m_dimension = source->getDimension();
		m_streams[i].m_indexOffset = vertexInput->m_offset;
		m_streams[i].m_set = input->m_set;
		SCE_SAMPLE_UTIL_ASSERT(source->m_arrayElementType == Dae::DataFlow::Source::ARRAY_ELEMENT_TYPE_FLOAT);
		m_streams[i].m_sourceId = source->m_id;
		m_streams[i].m_source = source->m_floatArray;
		m_streams[i].m_offsetInVertex = offsetInVertex;
		//printf("semantic=%d, offset=%d\n", m_streams[i].m_semantic, m_streams[i].m_offsetInVertex);
		offsetInVertex+= m_streams[i].m_dimension;
		maxOffset = std::max(maxOffset, m_streams[i].m_indexOffset);
	}

	int index = numInputsInVertex;
	for(uint32_t i=0; i<dae_triangles->getInputs().size(); i++){
		Dae::DataFlow::Input *input = dae_triangles->getInputs().at(i);
		if(input->m_semantic == Dae::kSemanticVertex){
			continue;
		} 

		const Dae::DataFlow::Source *source = mesh->findSource(Internal::Util::sourceToId(input->m_source));
		m_streams[index].m_semantic = input->m_semantic;
		m_streams[index].m_dimension = source->getDimension();
		m_streams[index].m_indexOffset = input->m_offset;
		m_streams[index].m_set = input->m_set;
		SCE_SAMPLE_UTIL_ASSERT(source->m_arrayElementType == Dae::DataFlow::Source::ARRAY_ELEMENT_TYPE_FLOAT);
		m_streams[index].m_sourceId = source->m_id;
		m_streams[index].m_source = source->m_floatArray;
		m_streams[index].m_offsetInVertex = offsetInVertex;
		//printf("semantic=%d, offset=%d\n", m_streams[i].m_semantic, m_streams[i].m_offsetInVertex);
		offsetInVertex+= m_streams[index].m_dimension;

		maxOffset = std::max(maxOffset, m_streams[index].m_indexOffset);
		index++;
	}
	m_indexStride = maxOffset +1;

	m_numFloatInVertex = offsetInVertex;


	return 0;
}

void VertexElementStreamManager::read(float *dst, const uint16_t *daeTriangleIndices)
{
	for(uint32_t si=0; si<m_numStreams; si++){
		uint16_t index = daeTriangleIndices[m_streams[si].m_indexOffset];
		for(uint32_t di=0; di<m_streams[si].m_dimension; di++){
			dst[m_streams[si].m_offsetInVertex + di] = m_streams[si].m_source->m_data[ m_streams[si].m_dimension * index + di];
		}
	}
}

uint16_t VertexElementStreamManager::getNewIndex(const uint16_t *daeTriangleIndices)
{
	Indices indices(m_indexStride, daeTriangleIndices);
	uint16_t ret;
	if(m_indexMap.find(indices) == m_indexMap.end()){
		uint32_t newIndex = m_indexMap.size();
		m_indexMap[indices] = newIndex;
		ret = newIndex;
	}else{
		ret = m_indexMap[indices];
		SCE_SAMPLE_UTIL_ASSERT(ret < m_indexMap.size());
	}
/*
	for(uint32_t i=0; i<m_indexStride; i++){
		printf("%d \n", daeTriangleIndices[i]);
	}
	printf("=> %d\n" , ret);
*/
	return ret;

}


VertexElementStream *VertexElementStreamManager::findStream(ssg::Collada::Dae::VertexSemantic semantic, uint32_t set) const
{
	for(uint32_t i=0; i<m_numStreams; i++){
		if((m_streams[i].m_semantic == semantic) && (m_streams[i].m_set == set)){
			return &m_streams[i];
		}
	}

	return NULL;
}

VertexElementStream *VertexElementStreamManager::findStreamBySourceId(std::string sourceId) const
{
	for(uint32_t i=0; i<m_numStreams; i++){
		if((m_streams[i].m_sourceId == sourceId)){
			return &m_streams[i];
		}
	}
	return NULL;
}

int VertexElementStreamManager::finalize(void)
{
	if(m_streams != NULL){
		delete [] m_streams;
		m_streams = NULL;
	}
	return 0;
}








int GeomMesh::initialize(const std::string &id,
						 const ssg::Collada::Dae::Geometry::Mesh *mesh, 
					 ssg::Collada::ColladaLoader *loaderConfig)
{
	int ret;
	SCE_SAMPLE_UTIL_ASSERT(mesh != NULL);

	m_id = id;

	m_numTriangless = mesh->m_triangless.size() + mesh->m_polylists.size();
	m_triangless = new Triangles*[m_numTriangless];
	memset(m_triangless, 0, sizeof(Triangles*) * m_numTriangless);

	for(unsigned int i=0; i<mesh->m_triangless.size(); i++){
		m_triangless[i] = new Triangles();
		ret = m_triangless[i]->initialize(mesh->m_triangless.at(i), mesh, loaderConfig);
		if(ret < SCE_OK){
			m_triangless[i] = NULL;
			finalize(loaderConfig);
			return ret;
		}
	}
	int si = mesh->m_triangless.size();
	for(unsigned int i=0; i<mesh->m_polylists.size(); i++){
		m_triangless[si+i] = new Triangles();
		m_triangless[si+i]->initializeFromPolylist(mesh->m_polylists.at(i), mesh, loaderConfig);
	}
	return SCE_OK;
}

void GeomMesh::finalize(ssg::Collada::ColladaLoader *loaderConfig)
{
	if(m_triangless != NULL){
		for(int32_t i=0; i<m_numTriangless; i++){
			if(m_triangless[i] != NULL){
				m_triangless[i]->finalize();
				delete m_triangless[i];
			}
		}
		delete [] m_triangless;
		m_triangless = NULL;
	}
}

Collada::MeshPart::MeshPart(void)
{
	//m_skinMeshPart = NULL;
	//m_triangles    = NULL;
	m_vertexAttributes = NULL;
}
Collada::MeshPart::~MeshPart(void)
{
	finalize();
}

bool Collada::MeshPart::hasSkin(void) const
{
	return m_hasSkin;
}

int Collada::MeshPart::initialize(ColladaLoader *loader,
								  ssg::Collada::Internal::SkinMeshPart *skinMeshPart, 
								  ssg::Collada::Internal::geometry::Triangles *triangles)
{
	m_hasSkin = skinMeshPart != NULL;

	m_material = triangles->m_material;

	uint32_t skinStride = (skinMeshPart==NULL)?0:skinMeshPart->getStride();
	m_stride = triangles->getStride() + skinStride;
	m_numVertices = triangles->m_numVertices;
	loader->getGraphicsLoader()->createVertexBuffer(&m_vertexBuffer, m_stride * m_numVertices);

	uintptr_t geombase = (uintptr_t)triangles->m_vertices;
	uintptr_t skinbase = (skinMeshPart==NULL)?0:(uintptr_t)skinMeshPart->m_vertices;

	uintptr_t vbuf = (uintptr_t)m_vertexBuffer->beginWrite();
	for(uint32_t i=0; i<m_numVertices; i++){
		memcpy((void*)(vbuf + i*m_stride), (void*)(geombase +i*triangles->getStride()), triangles->getStride());
		memcpy((void*)(vbuf + i*m_stride + triangles->getStride()), (void*)(skinbase +i*skinStride), skinStride);
	}
	m_vertexBuffer->endWrite();

	m_numIndices = triangles->getNumIndices();

	loader->getGraphicsLoader()->createIndexBuffer(&m_indexBuffer, sizeof(uint16_t) * m_numIndices);
	void *ibuf = m_indexBuffer->beginWrite();
	memcpy(ibuf, triangles->m_indices/*getIndices()->getData()*/, sizeof(uint16_t)* m_numIndices);
	m_indexBuffer->endWrite();

	// initilize vertex attribute
	m_numVertexAttributes = triangles->getNumVertexAttribute();
	if(m_hasSkin){
		if(skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendindices, 0).streamIndex < 2){
			m_numVertexAttributes++;
		}
		if(skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendindices, 1).streamIndex < 2){
			m_numVertexAttributes++;
		}
		if(skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendweight, 0).streamIndex < 2){
			m_numVertexAttributes++;
		}
		if(skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendweight, 1).streamIndex < 2){
			m_numVertexAttributes++;
		}
	}

	m_vertexAttributes = new ssg::VertexAttribute[m_numVertexAttributes];
	int index=0;
	for(uint32_t i=0; i<triangles->getNumVertexAttribute(); i++){
		m_vertexAttributes[index]= triangles->getVertexAttribute(i);
		index++;
	}
	if(m_hasSkin){
		if(skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendindices, 0).streamIndex < 2){
			m_vertexAttributes[index]= skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendindices, 0);
			m_vertexAttributes[index].streamIndex = 0;
			m_vertexAttributes[index].offset += triangles->getStride();
			index++;
		}
		if(skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendindices, 1).streamIndex < 2){
			m_vertexAttributes[index]= skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendindices, 1);
			m_vertexAttributes[index].streamIndex = 0;
			m_vertexAttributes[index].offset += triangles->getStride();
			index++;
		}
		if(skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendweight, 0).streamIndex < 2){
			m_vertexAttributes[index]= skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendweight, 0);
			m_vertexAttributes[index].streamIndex = 0;
			m_vertexAttributes[index].offset += triangles->getStride();
			index++;
		}
		if(skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendweight, 1).streamIndex < 2){
			m_vertexAttributes[index]= skinMeshPart->getVertexAttribute(ssg::kParameterSemanticBlendweight, 1);
			m_vertexAttributes[index].streamIndex = 0;
			m_vertexAttributes[index].offset += triangles->getStride();
			index++;
		}
	}


	SCE_SAMPLE_UTIL_ASSERT(m_numVertexAttributes==index);



	return SCE_OK;
}

int Collada::MeshPart::finalize(void)
{
	if(m_vertexBuffer != NULL){
		sce::SampleUtil::destroy(m_vertexBuffer);
		m_vertexBuffer = NULL;
	}
	
	if(m_indexBuffer != NULL){
		sce::SampleUtil::destroy(m_indexBuffer);
		m_indexBuffer = NULL;
	}
	if(m_vertexAttributes != NULL){
		delete [] m_vertexAttributes; 
		m_vertexAttributes = NULL;
	}
	m_numVertexAttributes = 0;

	return SCE_OK;
}




std::string Collada::MeshPart::getMaterial(void) const
{
	return m_material;
}

uint32_t Collada::MeshPart::getStride(void) const
{
	return m_stride;
}

ssg::VertexBuffer *Collada::MeshPart::getVertexBuffer(void) const
{
	return m_vertexBuffer;
}


ssg::IndexBuffer *Collada::MeshPart::getIndexBuffer(void) const
{
	return m_indexBuffer;
}
uint32_t Collada::MeshPart::getNumIndices(void) const
{
	return m_numIndices;
}

uint32_t Collada::MeshPart::getNumVertices(void) const
{
	return m_numVertices;
}

ssg::VertexAttribute Collada::MeshPart::getVertexAttribute(ssg::ParameterSemantic semantic, int semanticIndex) const
{
	for(uint32_t i=0; i<m_numVertexAttributes; i++){
		if((m_vertexAttributes[i].semantic == semantic)
			&& (m_vertexAttributes[i].semanticIndex == semanticIndex))
		{
			return m_vertexAttributes[i];
		}
	}

	VertexAttribute vattr;
	vattr.streamIndex = 0;		
	vattr.offset = 0;			
	vattr.format = kAttributeFormatF32;
	vattr.componentCount = 0;		
	vattr.semantic       = ssg::kParameterSemanticNone;
	vattr.semanticIndex  = 0;
	return vattr;
}

ssg::VertexAttribute Collada::MeshPart::getVertexAttribute(uint32_t index) const
{
	if(index >= m_numVertexAttributes){
		VertexAttribute vattr;
		vattr.streamIndex = 0;		
		vattr.offset = 0;			
		vattr.format = kAttributeFormatF32;
		vattr.componentCount = 0;		
		vattr.semantic       = ssg::kParameterSemanticNone;
		vattr.semanticIndex  = 0;
		return vattr;
	}
	return m_vertexAttributes[index];
}
uint32_t Collada::MeshPart::getNumVertexAttributes(void) const
{
	return m_numVertexAttributes;
}


const char *attributeFormatToString(AttributeFormat f)
{
	switch(f)
	{
	case kAttributeFormatF32:
		return "F32";
	case kAttributeFormatU16:
		return "U16";
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
		return "";
	}
}

const char *semanticToString(ParameterSemantic semantic)
{
	switch(semantic)
	{
	case kParameterSemanticNone   : return "NONE";
	case kParameterSemanticAttr   : return "ATTR";
#ifdef __psp2__
	case kParameterSemanticBcol     :  return "BCOL";
#endif
	case kParameterSemanticBinormal      : return "BINORMAL";
	case kParameterSemanticBlendindices  :return "BLENDINDICES";
	case kParameterSemanticBlendweight   :return "BLENDWEIGHT";
	case kParameterSemanticColor         :return "COLOR";
	case kParameterSemanticDiffuse       :return "DIFFUSE";
#ifdef __psp2__
	case kParameterSemanticFogcoord      :return "FOGCOORD";
	case kParameterSemanticPointsize     :return "POINTSIZE";
#endif
	case kParameterSemanticNormal        :return "NORMAL";
	case kParameterSemanticPosition      :return "POSITION";
	case kParameterSemanticSpecular      :return "SPECULAR";
	case kParameterSemanticTangent       :return "TANGENT";
	case kParameterSemanticTexcoord      :return "TEXCOORD";
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
		return "";
		//break;
	}
}

static void serializeVertexAttribute(ssgc::Internal::Serializer *serializer, const VertexAttribute *attr)
{
	printf("       vertex attr\n");
	printf("          streamIndex=%d\n", attr->streamIndex);
	printf("          offset=%d\n", attr->offset);
	printf("          format=%s\n", attributeFormatToString(attr->format));
	printf("          componentCount=%d\n", attr->componentCount); 
	printf("          semantic=%s\n", semanticToString(attr->semantic));
	printf("          semanticIndex=%d\n", attr->semanticIndex);


}

void Collada::MeshPart::serialize(ssgc::Internal::Serializer *serializer)
{
	printf("      MeshPart\n");
	printf("         material : %s\n", m_material.c_str());
	printf("         numVertices = %d\n", m_numVertices);
	printf("         numIndices  = %d\n", m_numIndices);
	printf("         stride = %d\n", m_stride);
	printf("         numAttributes = %d\n", m_numVertexAttributes);
	for(uint32_t i=0; i<m_numVertexAttributes; i++){
		serializeVertexAttribute(serializer, &m_vertexAttributes[i]);
	}

}

//---------------------------------------------------------------------------------------------------

int ssgci::MeshImpl::initialize(ssgc::ColladaLoader *loader,
								ssgc::Internal::controller::Skin *skin,
								ssgc::Internal::geometry::GeomMesh *geomMesh)
{
	int ret;

	//printf(__FILE__"(%d) init MESH %p\n", __LINE__, this);
	m_geometryId   = geomMesh->getId();

	m_numMeshParts = geomMesh->getNumTriangless();
	m_meshParts = new ssgc::MeshPart[m_numMeshParts];

	if(skin != NULL){
		m_controllerId = skin->m_id;
		for(uint32_t i=0; i<m_numMeshParts; i++){
			ret = m_meshParts[i].initialize(loader,
				&skin->m_skinPrimitiveElements[i],
				geomMesh->getTriangles(i));
			if(ret != SCE_OK){
				return ret;
			}
		}
		m_invBindMatrices = new sce::Vectormath::Simd::Aos::Matrix4[skin->m_jointNameArray.size()];

		for(uint32_t i=0; i<skin->m_jointNameArray.size(); i++){
			m_jointNameArray.push_back(skin->m_jointNameArray[i]);
			m_invBindMatrices[i] = skin->m_invBindMatrices[i];
		}
		m_jointNameType = skin->m_joinNameArrayUseSid ? kJointNameTypeSid : kJointNameTypeId;

		m_bindShapeMatrix = skin->m_bindShapeMatrix;

	}else{

		m_controllerId = "";
		for(uint32_t i=0; i<m_numMeshParts; i++){
			ret = m_meshParts[i].initialize(loader, NULL, 	geomMesh->getTriangles(i));
			if(ret != SCE_OK){
				return ret;
			}
		}
		m_invBindMatrices = NULL;
		m_bindShapeMatrix = sce::Vectormath::Simd::Aos::Matrix4::identity();
	}
	//printf(__FILE__"(%d) init MESH %p geomId=%s, contId=%s\n", __LINE__, this, 	   m_geometryId.c_str(), m_controllerId.c_str());

	return SCE_OK;
}

uint32_t ssgci::MeshImpl::getNumJoints(void) const
{
	return m_jointNameArray.size();
}

int ssgci::MeshImpl::finalize(void)
{
	if(m_meshParts!= NULL){
		delete [] m_meshParts;
		m_meshParts = NULL;
	}
	if(m_invBindMatrices != NULL){
		delete [] m_invBindMatrices;
		m_invBindMatrices = NULL;
	}

	m_jointNameArray.clear();
	return SCE_OK;
}

ssgci::MeshImpl::MeshImpl(void)
{
	m_meshParts  = NULL;
	m_invBindMatrices = NULL;
}
ssgci::MeshImpl::~MeshImpl(void)
{
	finalize();
}


void ssgci::MeshImpl::serialize(ssgc::Internal::Serializer *serializer)
{
	printf("  Mesh : %s, %s\n", m_geometryId.c_str(), m_controllerId.c_str());
	printf("     JointNameType : %s\n", m_jointNameType==kJointNameTypeId ? "ID" : "SID");
	printf("     JointNameArray : ");
	for(uint32_t i=0; i<m_jointNameArray.size(); i++){
		printf("%s ", m_jointNameArray.at(i).c_str());
	}
	printf("\n");
	printf("     BindShapeMatrix : ");
	const float *b = (const float*)&m_bindShapeMatrix;
	for(uint32_t i=0; i<16; i++){
		printf("%.2f ", b[i]);
	}
	printf("\n");

	printf("     NumMeshParts : %d\n", m_numMeshParts);
	for(uint32_t i=0; i<m_numMeshParts; i++){
		m_meshParts[i].serialize(serializer);
	}

}


bool ssgci::MeshImpl::hasSkin(void) const
{
	return m_controllerId != "";
}

const Collada::MeshPart *ssgci::MeshImpl::getMeshPart(uint32_t index) const
{
	if(index >= m_numMeshParts){
		return NULL;
	}
	return &m_meshParts[index];
}
uint32_t ssgci::MeshImpl::getNumMeshParts(void) const
{
	return m_numMeshParts;
}

std::string ssgci::MeshImpl::getControllerId(void) const
{
	return m_controllerId;
}
std::string ssgci::MeshImpl::getGeometryId(void) const
{
	return m_geometryId;
}

Collada::Mesh::JointNameType ssgci::MeshImpl::getJointNameType(void) const 
{
	return m_jointNameType;
}
const std::vector<std::string> *ssgci::MeshImpl::getJointNameArray(void) const
{
	return &m_jointNameArray;
}

std::string ssgci::MeshImpl::getJointName(uint32_t index) const
{
	if(index >= m_jointNameArray.size()){
		return "";
	}
	return m_jointNameArray.at(index);
}

const sce::Vectormath::Simd::Aos::Matrix4 *ssgci::MeshImpl::getInvBindMatrices(void) const
{
	return m_invBindMatrices;
}
sce::Vectormath::Simd::Aos::Matrix4 ssgci::MeshImpl::getBindShapeMatrix(void) const
{
	return m_bindShapeMatrix;
}



//---------------------------------------------------------------------------------------------------

int Collada::Internal::LibraryMeshes::initialize(ColladaLoader *loader, 
												Internal::controller::LibraryControllers *libraryControllers, 
												Internal::geometry::LibraryGeometries* libraryGeometries)
{
	int ret;
	for(uint32_t i=0; i<libraryControllers->m_skins.size(); i++){
		Internal::controller::Skin *skin = libraryControllers->m_skins.at(i);
		Internal::geometry::GeomMesh *geomMesh = skin->m_mesh;

		MeshImpl *mesh = new MeshImpl();
		ret = mesh->initialize(loader, skin,geomMesh);
		if(ret != SCE_OK){
			return ret;
		}
		m_meshes.push_back(mesh);
	}

	for(uint32_t i=0; i<libraryGeometries->m_meshes.size(); i++){
		Internal::geometry::GeomMesh *geomMesh = libraryGeometries->m_meshes.at(i);
		bool found = false;
		for(uint32_t j=0; j<libraryControllers->m_skins.size(); j++){
			Internal::controller::Skin *skin = libraryControllers->m_skins.at(i);
			if(skin->m_mesh->getId() == geomMesh->getId()){
				found = true;
				break;
			}
		}
		if(found){
			continue;
		}
		MeshImpl *mesh = new MeshImpl();
		ret = mesh->initialize(loader, NULL,geomMesh);
		if(ret != SCE_OK){
			return ret;
		}
		m_meshes.push_back(mesh);
	}

	return SCE_OK;
}


int Collada::Internal::LibraryMeshes::finalize(void)
{
	for(uint32_t i=0; i<m_meshes.size(); i++){
		Mesh *m = m_meshes.at(i);
		delete m;
	}
	m_meshes.clear();
	return SCE_OK;
}


Collada::Mesh * Collada::Internal::LibraryMeshes::findMeshByControllerId(std::string controllerId) const
{
	for(uint32_t i=0; i<m_meshes.size(); i++){
		Mesh *m = m_meshes.at(i);
		if(m->getControllerId() == controllerId){
			return m;
		}
	}
	return NULL;
}
Collada::Mesh * Collada::Internal::LibraryMeshes::findMeshByGeometryId(std::string geometryId) const
{
	for(uint32_t i=0; i<m_meshes.size(); i++){
		Mesh *m = m_meshes.at(i);
		if(m->getGeometryId() == geometryId){
			return m;
		}
	}
	return NULL;
}








int LibraryGeometries::initialize(const ssg::Collada::Dae::Geometry::LibraryGeometries *libraryGeometries, 
								   ssg::Collada::ColladaLoader *loaderConfig)
{
	int ret;
	for(unsigned int i=0; i<libraryGeometries->m_geometries.size(); i++){
		GeomMesh* m = new GeomMesh();

		const Dae::Geometry::Mesh *daeMesh = libraryGeometries->m_geometries.at(i)->m_mesh;
		ret = m->initialize(
			libraryGeometries->m_geometries.at(i)->m_id,
			daeMesh, loaderConfig);
		if(ret < SCE_OK){
			finalize(loaderConfig);
			return ret;
		}
		m_meshes.push_back(m);
	}
	return SCE_OK;
}



static ssg::ParameterSemantic convertVattrSemantic(Collada::Dae::VertexSemantic colladaSemantic)
{
	switch(colladaSemantic)
	{
	case Collada::Dae::kSemanticBinormal: return ssg::kParameterSemanticBinormal;
	case Collada::Dae::kSemanticColor:    return ssg::kParameterSemanticColor;
	case Collada::Dae::kSemanticNormal:   return ssg::kParameterSemanticNormal;
	case Collada::Dae::kSemanticPosition: return ssg::kParameterSemanticPosition;
	case Collada::Dae::kSemanticTangent:  return ssg::kParameterSemanticTangent;
	case Collada::Dae::kSemanticTexbinormal: return ssg::kParameterSemanticBinormal;
	case Collada::Dae::kSemanticTexcoord: return ssg::kParameterSemanticTexcoord;
	case Collada::Dae::kSemanticTextangent:  return ssg::kParameterSemanticTangent;
	case Collada::Dae::kSemanticUv:       return ssg::kParameterSemanticTexcoord;
	case Collada::Dae::kSemanticVertex:   return ssg::kParameterSemanticPosition;
	case Collada::Dae::kSemanticWeight:   return ssg::kParameterSemanticBlendweight;
	default:
		{
			return ssg::kParameterSemanticNone;
		}
	}
}




int Triangles::initialize(const ssg::Collada::Dae::Geometry::Triangles *dae_triangles, 
						  const ssg::Collada::Dae::Geometry::Mesh *mesh, 
						  ssg::Collada::ColladaLoader *loaderConfig)
{
	VertexElementStreamManager streamManager;

	int ret = streamManager.initialize(dae_triangles, mesh);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_material = dae_triangles->m_matrial;
	Triangles *triangles = this;

	triangles->m_numTriangles = dae_triangles->m_count;

	m_numVertices = triangles->m_numTriangles * 3;

	triangles->m_vertexOldIndices = new int      [m_numVertices];
	for(uint32_t i=0; i<m_numVertices; i++){
		triangles->m_vertexOldIndices[i] = -1;
	}

	m_vertices = new float[m_numVertices * streamManager.m_numFloatInVertex];
	m_indices = new uint16_t[ 3 * triangles->m_numTriangles];

	uint16_t *daeTriangleIndices = dae_triangles->p;
	uint32_t numIndices = dae_triangles->m_count * 3;
	int maxIndices = -1;
	for(uint32_t i=0; i<numIndices; i++){
		//uint32_t indexInFloatArray = streamManager.m_numFloatInVertex * i;
		//streamManager.read(&m_vertices[ indexInFloatArray ],	&daeTriangleIndices[ streamManager.m_indexStride * i]);

		uint16_t *daeIndices = &daeTriangleIndices[ streamManager.m_indexStride * i];
		m_indices[i] = streamManager.getNewIndex(daeIndices);
		if(maxIndices < m_indices[i]){ // note taht index is assinged incrementally.
			streamManager.read(&m_vertices[ streamManager.m_numFloatInVertex * m_indices[i] ], daeIndices);
			triangles->m_vertexOldIndices[m_indices[i]] = daeTriangleIndices[streamManager.m_indexStride * i + streamManager.m_vertexIndexOffset];
			maxIndices = m_indices[i];
		}

		//triangles->m_vertexOldIndices[i] = daeTriangleIndices[streamManager.m_indexStride * i + streamManager.m_vertexIndexOffset];
/*
		for(uint32_t k=0; k<streamManager.m_indexStride; k++){
			printf("%d ", daeIndices[k]);
		}
		printf("=> %d (old=%d)\n" ,  m_indices[i], triangles->m_vertexOldIndices[i]);
*/
	}

	m_numVertexAttributes = streamManager.m_numStreams;
	if(m_numVertexAttributes != 0){
		m_vertexAttributes = new ssg::VertexAttribute[m_numVertexAttributes];

		for(uint32_t i=0; i<m_numVertexAttributes; i++){
			m_vertexAttributes[i].streamIndex = 0;
			m_vertexAttributes[i].offset  =streamManager.m_streams[i].m_offsetInVertex * sizeof(float); 	
			m_vertexAttributes[i].format = ssg::kAttributeFormatF32;				
			m_vertexAttributes[i].componentCount = streamManager.m_streams[i].m_dimension;
			m_vertexAttributes[i].semantic = convertVattrSemantic(streamManager.m_streams[i].m_semantic);
			m_vertexAttributes[i].semanticIndex  = streamManager.m_streams[i].m_set;
		}
	}else{
		m_vertexAttributes = NULL;
	}
	m_stride = streamManager.m_numFloatInVertex * sizeof(float);


	(void)ret;
	return SCE_OK;
}


void Triangles::initializeFromPolylist(const ssg::Collada::Dae::Geometry::Polylist *collada_polylist, 
									   const ssg::Collada::Dae::Geometry::Mesh *mesh, 
									   ssg::Collada::ColladaLoader *loaderConfig)
{
	SCE_SAMPLE_UTIL_ASSERT(0);
}


void Triangles::finalize(void)
{
	if(m_vertexAttributes != NULL){
		delete [] m_vertexAttributes;
		m_vertexAttributes = NULL;
	}
	//m_triangleIndices.finalize();

	//m_vertices2.finalize();

	if(m_vertexOldIndices != NULL){
		delete [] m_vertexOldIndices;
		m_vertexOldIndices = NULL;
	}

	if(m_vertices != NULL){
		delete [] m_vertices;
		m_vertices = NULL;
	}
	if(m_indices != NULL){
		delete [] m_indices;
		m_indices = NULL;
	}

}
