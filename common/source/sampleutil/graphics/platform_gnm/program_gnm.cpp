/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS


#include <sampleutil/graphics.h>
#include "constant_internal_gnm.h"
#include "program_internal_gnm.h"
#include "loader_internal_gnm.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

#pragma region ProgramImpl

ssg::ProgramImpl::~ProgramImpl(void)
{
	m_loader->_unregisterProgram(m_program->m_binary);
}

ssg::ProgramImpl::ProgramImpl(const GnmProgram *program, Impl::GraphicsLoaderImpl* loader)
{
	m_program = program;
	m_loader = loader;
}

ssg::ProgramImpl::ProgramImpl(const ProgramImpl& from)
{
	m_program = from.m_program;
	m_loader  = from.m_loader;
	m_loader->_referProgram(m_program->m_binary);
}
ssg::ProgramImpl &ssg::ProgramImpl::operator=(const ProgramImpl &from)
{
	m_program = from.m_program;
	m_loader  = from.m_loader;
	m_loader->_referProgram(m_program->m_binary);
	return *this;
}

bool ssg::ProgramImpl::isValid(void) const
{ 
	return m_program->isValid(); 
}
uint32_t ssg::ProgramImpl::getParameterCount(void) const
{
	return m_program->getParameterCount();
}
const ssg::Parameter* ssg::ProgramImpl::findParameterByName(const char *name) const
{
	return m_program->findParameterByName(name);
}
const ssg::Parameter* ssg::ProgramImpl::findParameterBySemantic(ssg::ParameterSemantic semantic, uint32_t semanticIndex) const
{
	return m_program->findParameterBySemantic(semantic, semanticIndex);
}
const ssg::Parameter* ssg::ProgramImpl::getParameter(uint32_t index) const
{
	return m_program->getParameter(index);
}
void ssg::ProgramImpl::getAllParameters(std::vector<const ssg::Parameter*> &params) const
{
	return m_program->getAllParameters(params);
}

#pragma endregion // ProgramImpl


/* --------------------------------------------------------- */

#pragma region GnmParameter

ssg::ParameterSemantic ssg::GnmParameter::getSemantic(void) const
{
	return m_semantic;
}
uint32_t ssg::GnmParameter::getSemanticIndex(void) const
{
	return m_semanticIndex;
}


uint32_t ssg::GnmParameter::getComponentCount(void) const
{
	return m_componentCount;
}
uint32_t ssg::GnmParameter::getArraySize(void) const
{
	return 1;// TODO
}
bool ssg::GnmParameter::isUseDefaultBuf(void) const
{
	return (getCategory() == kParameterCategoryUniform) 
		&& (getBufferIndex() == 0);
}
uint32_t ssg::GnmParameter::getBufferIndex(void) const
{
	return m_containerIndex;
}
uint32_t ssg::GnmParameter::getContainerIndex(void) const
{
	return m_containerIndex;
}

int ssg::GnmParameter::setUniformDataF(void *uniformBuffer, const float *sourceData) const
{
	if(uniformBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(sourceData == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(getCategory() != kParameterCategoryUniform){
		return -1;
	}
	//printf("setUniformDataF param:%s offset=%d, size=%d\n", getName(), m_gnmParam->m_offset, m_gnmParam->m_size);
	uintptr_t p = ((uintptr_t)uniformBuffer) + m_offset;

	memcpy((void*)p, sourceData, m_size);
	return SCE_OK;
}

const char *ssg::GnmParameter::getName(void) const
{
	return (const char*)m_name.c_str();
}

ssg::ParameterCategory ssg::GnmParameter::getCategory(void) const
{
	return m_category;
}

bool ssg::GnmParameter::isValid(void) const
{
	return m_category != kParameterCategoryUnknown;
}

uint32_t ssg::GnmParameter::getResourceIndex(void) const
{
	return m_resourceIndex;
}

void ssg::GnmParameter::dump(void) const
{
	printf("name : %15s, semantic=%#x, semanticIndex=%d, size=%d, componentCount=%d, offset=%d, resourceIndex=%d, elementType=%s, containerIndex=%d\n", 
		getName(),
		getSemantic(),
		getSemanticIndex(),
		m_size,
		m_componentCount,
		m_offset,
		m_resourceIndex,
		Shader::Binary::getPsslTypeString((Shader::Binary::PsslType)m_elementType),
		m_containerIndex
		);
}


ssg::ParameterType ssg::GnmParameter::getType(void) const
{
	ParameterCategory c = getCategory();
	if(c == kParameterCategorySampler){
		return kParameterTypeSampler;
	}else if(c == kParameterCategoryUnknown){
		return kParameterTypeUnknown;
	}
	switch(m_elementType)
	{
	case Shader::Binary::kTypeFloat1:	case Shader::Binary::kTypeFloat2: 	case Shader::Binary::kTypeFloat3:  case Shader::Binary::kTypeFloat4:
		{
			return kParameterTypeF32;
		}
	case Shader::Binary::kTypeHalf1:	case Shader::Binary::kTypeHalf2:	case Shader::Binary::kTypeHalf3: 	case Shader::Binary::kTypeHalf4:
		{
			return kParameterTypeF16;
		}
	case Shader::Binary::kTypeInt1:		case Shader::Binary::kTypeInt2:		case Shader::Binary::kTypeInt3:		case Shader::Binary::kTypeInt4:
		{
			return kParameterTypeS32;
		}
	case Shader::Binary::kTypeUint1:	case Shader::Binary::kTypeUint2:	case Shader::Binary::kTypeUint3:	case Shader::Binary::kTypeUint4:
		{
			return kParameterTypeU32;
		}
	case Shader::Binary::kTypeFloat1x1:	case Shader::Binary::kTypeFloat2x1:	case Shader::Binary::kTypeFloat3x1:	case Shader::Binary::kTypeFloat4x1:
	case Shader::Binary::kTypeFloat1x2:	case Shader::Binary::kTypeFloat2x2:	case Shader::Binary::kTypeFloat3x2:	case Shader::Binary::kTypeFloat4x2:
	case Shader::Binary::kTypeFloat1x3:	case Shader::Binary::kTypeFloat2x3:	case Shader::Binary::kTypeFloat3x3:	case Shader::Binary::kTypeFloat4x3:
	case Shader::Binary::kTypeFloat1x4:	case Shader::Binary::kTypeFloat2x4:	case Shader::Binary::kTypeFloat3x4:	case Shader::Binary::kTypeFloat4x4:
		{
			return kParameterTypeF32;
		}
	case Shader::Binary::kTypeHalf1x1:	case Shader::Binary::kTypeHalf2x1:	case Shader::Binary::kTypeHalf3x1:	case Shader::Binary::kTypeHalf4x1:
	case Shader::Binary::kTypeHalf1x2:	case Shader::Binary::kTypeHalf2x2:	case Shader::Binary::kTypeHalf3x2:	case Shader::Binary::kTypeHalf4x2:
	case Shader::Binary::kTypeHalf1x3:	case Shader::Binary::kTypeHalf2x3:	case Shader::Binary::kTypeHalf3x3:	case Shader::Binary::kTypeHalf4x3:
	case Shader::Binary::kTypeHalf1x4:	case Shader::Binary::kTypeHalf2x4:	case Shader::Binary::kTypeHalf3x4:	case Shader::Binary::kTypeHalf4x4:
		{
			return kParameterTypeF16;
		}
	case Shader::Binary::kTypeInt1x1:	case Shader::Binary::kTypeInt2x1:	case Shader::Binary::kTypeInt3x1:	case Shader::Binary::kTypeInt4x1:
	case Shader::Binary::kTypeInt1x2:	case Shader::Binary::kTypeInt2x2:	case Shader::Binary::kTypeInt3x2:	case Shader::Binary::kTypeInt4x2:
	case Shader::Binary::kTypeInt1x3:	case Shader::Binary::kTypeInt2x3:	case Shader::Binary::kTypeInt3x3:	case Shader::Binary::kTypeInt4x3:
	case Shader::Binary::kTypeInt1x4:	case Shader::Binary::kTypeInt2x4:	case Shader::Binary::kTypeInt3x4:	case Shader::Binary::kTypeInt4x4:
		{
			return kParameterTypeS32;
		}
	case Shader::Binary::kTypeUint1x1:	case Shader::Binary::kTypeUint2x1:	case Shader::Binary::kTypeUint3x1:	case Shader::Binary::kTypeUint4x1:
	case Shader::Binary::kTypeUint1x2:	case Shader::Binary::kTypeUint2x2:	case Shader::Binary::kTypeUint3x2:	case Shader::Binary::kTypeUint4x2:
	case Shader::Binary::kTypeUint1x3:	case Shader::Binary::kTypeUint2x3:	case Shader::Binary::kTypeUint3x3:	case Shader::Binary::kTypeUint4x3:
	case Shader::Binary::kTypeUint1x4:	case Shader::Binary::kTypeUint2x4:	case Shader::Binary::kTypeUint3x4:	case Shader::Binary::kTypeUint4x4:
		{
			return kParameterTypeU32;
		}
	default : 
		{
			return kParameterTypeUnknown;
		}
	}

}



ssg::ParameterSemantic ssg::GnmParameter::getSemantic(std::string semantic)
{
	if(semantic ==  std::string("BINORMAL")){
		return 	kParameterSemanticBinormal;
	}else if(std::string(semantic) == std::string("COLOR")){
		return kParameterSemanticColor;
	}else if(semantic == std::string("NORMAL")){
		return kParameterSemanticNormal;
	}else if(semantic == std::string("POINTSIZE")){
		return kParameterSemanticPointsize;
	}else if(semantic == std::string("POSITION")){
		return kParameterSemanticPosition;
	}else if(semantic == std::string("TANGENT")){
		return kParameterSemanticTangent;
	}else if(semantic == std::string("TEXCOORD")){
		return kParameterSemanticTexcoord;
	}else{
		return kParameterSemanticNone;
	}
}
std::string ssg::GnmParameter::getParamName(const std::string sbparam)
{
	size_t n = sbparam.find_first_of('[');
	if(n == std::string::npos){
		return sbparam;
	}
	return sbparam.substr(0, n);
}

void ssg::GnmParameter::initializeAsUniform(const sce::Shader::Binary::Element *e, uint32_t containerIndex)
{
	m_category = kParameterCategoryUniform;
	m_name = getParamName(e->getName());
	m_size = e->m_size; 
	m_semanticIndex = 0;
	m_resourceIndex = e->m_byteOffset;
	m_containerIndex = containerIndex; 
	m_componentCount = sce::Shader::Binary::getPsslTypeElementCount((sce::Shader::Binary::PsslType)e->m_type) * (e->m_arraySize ? e->m_arraySize : 1);
	m_offset = e->m_byteOffset;
	m_elementType = e->m_type;
}

void ssg::GnmParameter::initializeAsAttribute(const sce::Shader::Binary::Attribute *a)
{
	m_category = kParameterCategoryAttribute;
	m_name = getParamName(a->getName());
	m_size = Impl::getPSSLTypeSize((sce::Shader::Binary::PsslType)a->m_type);
	m_semanticIndex = a->m_semanticIndex;
	m_semantic = getSemantic(std::string(a->getSemanticName()));
	m_resourceIndex = a->m_resourceIndex;
	m_componentCount = sce::Shader::Binary::getPsslTypeElementCount((sce::Shader::Binary::PsslType)a->m_type);
	m_offset = 0;
	m_elementType = a->m_type;
}

void ssg::GnmParameter::initializeAsSampler(const sce::Shader::Binary::Buffer *b)
{
	m_category = kParameterCategorySampler;
	m_name = getParamName(b->getName());
	m_size = 0;
	m_resourceIndex = b->m_resourceIndex;
	m_elementType = b->m_type;
};

#pragma endregion // GnmParameter



/* --------------------------------------------------------- */

#pragma region GnmProgram

int ssg::GnmProgram::initParams(void)
{
	std::vector< GnmParameter> gnmAttrParams;
	sce::SampleUtil::Map<std::string, GnmParameter>::Type gnmConstParams;

	// init attribute parameters
	for(uint32_t i=0; i<m_shaderBinaryProgram.m_numInputAttributes; i++){
		GnmParameter gp;
		//printf("[attr] [%d] name=%s\n", i, (char*)m_shaderBinaryProgram.m_inputAttributes[i].m_name);
		gp.initializeAsAttribute(&m_shaderBinaryProgram.m_inputAttributes[i]);
		gnmAttrParams.push_back(gp);
	}
	
	// init buffer parameters
	m_defaultUniformBufferSize = 0;
	for(uint32_t i=0; i<m_shaderBinaryProgram.m_numBuffers; i++){
		sce::Shader::Binary::Buffer *b = &m_shaderBinaryProgram.m_buffers[i];
		//printf("%s langtype=%d, inttype=%d\n", (char*)b->m_name, b->m_langType, b->m_internalType);
		if(b->m_langType == sce::Shader::Binary::kBufferTypeTexture2d){ // TODO
			GnmParameter gnmParam;
			gnmParam.initializeAsSampler(b);
			gnmConstParams[gnmParam.m_name] = gnmParam;
		}
		else if(b->m_langType == sce::Shader::Binary::kBufferTypeConstantBuffer){
			if(b->m_resourceIndex == 0) // global constant buffer for uniform
				m_defaultUniformBufferSize = b->m_strideSize;
			// init constant buffer elements
			for(uint32_t j = 0; j < b->m_numElements; j++){
				sce::Shader::Binary::Element *e = &m_shaderBinaryProgram.m_elements[b->m_elementOffset + j];
				GnmParameter gnmParam;
				gnmParam.initializeAsUniform(e, b->m_resourceIndex);
				gnmConstParams[gnmParam.m_name] = gnmParam;
			}
		}
	}

	//printf("[dump parmas]\n");
	sce::SampleUtil::Map<std::string, GnmParameter>::Type::iterator it;
	int index = 0;
	for(it = gnmConstParams.begin(); it != gnmConstParams.end(); it++){
		GnmParameter &p = it->second;
		//p.dump();
		m_params.push_back(p);
		index++;
	}
	for(int i=0; i<gnmAttrParams.size(); i++){
		GnmParameter &p = gnmAttrParams.at(i);
		m_params.push_back(p);
		index++;
	}

	//printf("m_defaultUniformBufferSize=%d\n", m_defaultUniformBufferSize);
	return SCE_OK;
}


int ssg::GnmProgram::initializeVertexShader(Impl::GraphicsLoaderImpl* loader)
{

	int ret = loadAndAllocateVsShaderFromMemory(loader, m_vsShader, m_memblk,
		m_binary,
		m_binarySize);
	if(ret < SCE_OK){
		return ret;
	}

	m_type = TYPE_VERTEX;

	
	uint32_t fssz = sce::Gnmx::computeVsFetchShaderSize(m_vsShader);

	loader->getHeap()->allocateVideoMemoryBlock(&m_fetchShader, fssz, sce::Gnm::kAlignmentOfFetchShaderInBytes);

	sce::Gnmx::generateVsFetchShader(m_fetchShader.m_ptr, &m_shaderModifier, m_vsShader);

	
	initParams();
	
	//dump();
	return SCE_OK;
}

int ssg::GnmProgram::initializePixelShader(Impl::GraphicsLoaderImpl* loader)
{

	int ret = loadAndAllocatePsShaderFromMemory(loader, 
		m_psShader,
		m_memblk,
		m_binary,
		m_binarySize);
	if(ret < SCE_OK){
		return ret;
	}
	m_type = TYPE_PIXEL;
	
	initParams();
	
	//dump();
	return SCE_OK;
}

int ssg::GnmProgram::initializeComputeShader(Impl::GraphicsLoaderImpl* loader)
{
	int ret = loadAndAllocateCsShaderFromMemory(loader, 
		m_csShader,
		m_memblk,
		m_binary,
		m_binarySize);
	if(ret < SCE_OK){
		return ret;
	}
	m_type = TYPE_COMPUTE;
	
	initParams();
	
	//dump();
	return SCE_OK;
}


int ssg::GnmProgram::initialize(Impl::GraphicsLoaderImpl* loader, const void *binary, uint32_t binarySize)
{
	m_loader = loader;
	m_binary = binary;
	m_binarySize = binarySize;

	int ret = m_shaderBinaryProgram.loadFromMemory(binary, binarySize);
	if(ret < 0){
		return ret;
	}

	switch(m_shaderBinaryProgram.m_header->m_shaderType)
	{
	case sce::Shader::Binary::kShaderTypeVsShader:
		{
			ret = initializeVertexShader(loader);
			return ret;
		}
	case sce::Shader::Binary::kShaderTypeFsShader:
		{
			ret = initializePixelShader(loader);
			return ret;
		}
	case sce::Shader::Binary::kShaderTypeCsShader:
		{
			ret = initializeComputeShader(loader);
			return ret;
		}
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
		return -1;
	};
}

int ssg::GnmProgram::finalize(void)
{
	if(m_memblk.m_ptr != NULL){
		m_loader->getHeap()->releaseMemoryBlock(&m_memblk);
	}
	switch(m_type)
	{
	case TYPE_VERTEX:
		free(m_vsShader);
		m_loader->getHeap()->releaseMemoryBlock(&m_fetchShader);
		break;
	case TYPE_PIXEL:
		free(m_psShader);
		break;
	default:
		break;
	};
	return SCE_OK;
}

bool ssg::GnmProgram::isValid(void) const
{
	return m_binary != NULL;
}

uint32_t ssg::GnmProgram::getDefaultBufferSize(void) const
{
	return m_defaultUniformBufferSize;
}

const ssg::Parameter* ssg::GnmProgram::findParameterByName(const char *_name) const
{
	std::string name(_name);
	for(uint32_t i=0; i<m_params.size(); i++){
		if(m_params.at(i).getName() == name){
			const ssg::Parameter &param = m_params.at(i);
			return &param;
		}
	}
	return NULL;
}


uint32_t ssg::GnmProgram::getParameterCount(void) const
{
	return m_params.size();
}

const ssg::Parameter* ssg::GnmProgram::getParameter(uint32_t index) const
{
	if(index >= m_params.size()){
		return NULL;
	}
	const ssg::Parameter &param = m_params.at(index);
	return &param;
}

const ssg::Parameter* ssg::GnmProgram::findParameterBySemantic(ssg::ParameterSemantic semantic, uint32_t semanticIndex) const
{
	for(uint32_t i=0; i<m_params.size(); i++){
		const ssg::Parameter &param = m_params.at(i);
		if((param.getSemantic() == semantic) && param.getSemanticIndex() == semanticIndex){
			return &param;
		}
	}
	return NULL;
}


void ssg::GnmProgram::getAllParameters(std::vector<const ssg::Parameter*> &params) const
{
	params.clear();
	for(uint32_t i=0; i<m_params.size(); i++){
		const GnmParameter &param = m_params.at(i);
		params.push_back(&param);
	}
}

#pragma endregion // GnmProgram


#pragma region VertexProgramImpl

ssgi::VertexProgramImpl::VertexProgramImpl(void)
{
	m_numAttrs = 0;
	m_attributes = NULL;
	
	m_numStreams = 0;
	m_streams = NULL;
}

ssgi::VertexProgramImpl::~VertexProgramImpl(void)
{
	if(m_attributes != NULL){
		delete [] m_attributes;
		m_attributes = NULL;
	}
	if(m_streams != NULL){
		delete [] m_streams;
		m_streams = NULL;
	}
}

#pragma endregion // VertexProgramImpl

#endif //_SCE_TARGET_OS_ORBIS
