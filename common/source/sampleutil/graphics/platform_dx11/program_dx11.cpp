/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <sampleutil/graphics.h>
#include "util/util.h"

#if _SCE_TARGET_OS_WINDOWS

#include "program_internal_dx11.h"
#include "buffer_internal_dx11.h"
#include <dxgi.h>
#include <D3D11.h>
#include <D3D11Shader.h>

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace util = sce::SampleUtil::Internal::Util;

//-----------------------------------------------------------------------
// For Cg
//-----------------------------------------------------------------------
static ssg::ParameterCategory utilCgParamGetCategory(CGparameter cgParam) 
{
	util::checkForCgError("precheck");
	CGtype  type = cgGetParameterType(cgParam);
	if(type == CG_INVALID_PARAM_HANDLE_ERROR){
		cgGetError();
		return ssg::kParameterCategoryUnknown;
	}
	if(type == CG_STRUCT){
		return ssg::kParameterCategoryUnknown;
	}
	if(cgGetParameterClass(cgParam) == CG_PARAMETERCLASS_SAMPLER ){
		return ssg::kParameterCategorySampler;
	}
	if(cgGetParameterVariability(cgParam) == CG_UNIFORM){
		return ssg::kParameterCategoryUniform;
	}

	bool isInput = (cgGetParameterDirection(cgParam) == CG_IN) || (cgGetParameterDirection(cgParam) == CG_INOUT);
	if((cgGetParameterVariability(cgParam) == CG_VARYING) && isInput ){
		return ssg::kParameterCategoryAttribute;
	}
	return ssg::kParameterCategoryUnknown;
	
}


static ssg::ParameterSemantic utilCgParamGetSemantic(CGparameter cgParam)
{
	ssg::ParameterCategory c = utilCgParamGetCategory(cgParam);
	if(c != ssg::kParameterCategoryAttribute){
		return ssg::kParameterSemanticNone;
	}
	
	util::checkForCgError("getSemantic");
	const char *sem = cgGetParameterSemantic(cgParam);
	if(strncmp(sem, "BLENDINDICES", strlen("BLENDINDICES")) == 0){
		util::checkForCgError("getSemantic");
		return ssg::kParameterSemanticBlendindices;
	}
	if(strncmp(sem, "BLENDWEIGHT", strlen("BLENDWEIGHT")) == 0){
		util::checkForCgError("getSemantic");
		return ssg::kParameterSemanticBlendweight;
	}
	if(strncmp(sem, "BCOL", strlen("BCOL")) == 0){
		util::checkForCgError("getSemantic");
		return ssg::kParameterSemanticBcol;
	}
	if(strncmp(sem, "FOGCOORD", strlen("FOGCOORD")) == 0){
		util::checkForCgError("getSemantic");
		return ssg::kParameterSemanticFogcoord;
	}
	if(strncmp(sem, "POINTSIZE", strlen("POINTSIZE")) == 0){
		util::checkForCgError("getSemantic");
		return ssg::kParameterSemanticPointsize;
	}

	switch(cgGetParameterBaseResource(cgParam))
	{
	case CG_ATTR0:         util::checkForCgError("getSemantic");return ssg::kParameterSemanticAttr          ;
	case CG_BINORMAL0:     util::checkForCgError("getSemantic");return ssg::kParameterSemanticBinormal      ; 
	case CG_BLENDINDICES0: util::checkForCgError("getSemantic");return ssg::kParameterSemanticBlendindices  ; 
	case CG_BLENDWEIGHT0:  util::checkForCgError("getSemantic");return ssg::kParameterSemanticBlendweight   ; 
	case CG_COLOR0:        util::checkForCgError("getSemantic");return ssg::kParameterSemanticColor         ; 
	case CG_DIFFUSE0:      util::checkForCgError("getSemantic");return ssg::kParameterSemanticDiffuse       ; 
	case CG_NORMAL0:       util::checkForCgError("getSemantic");return ssg::kParameterSemanticNormal        ; 
	case CG_POSITION0:     util::checkForCgError("getSemantic");return ssg::kParameterSemanticPosition      ; 
	case CG_SPECULAR0:     util::checkForCgError("getSemantic");return ssg::kParameterSemanticSpecular      ; 
	case CG_TANGENT0:      util::checkForCgError("getSemantic");return ssg::kParameterSemanticTangent       ; 
	case CG_TEXCOORD0:     util::checkForCgError("getSemantic");return ssg::kParameterSemanticTexcoord      ; 
	default: {
		util::checkForCgError("getSemantic");
		return ssg::kParameterSemanticNone;
			 }
	};
}

static uint32_t utilCgParamGetComponentCount(CGparameter cgParam)
{
	util::checkForCgError("getComponentCount");
	int nrows = cgGetParameterRows(cgParam);
	int ncols = cgGetParameterColumns(cgParam);
	return nrows*ncols;
}
static uint32_t utilCgParamGetArraySize(CGparameter cgParam)
{
	int asize = cgGetArrayTotalSize(cgParam);
	if(asize <= 0){
		asize = 1;
	}
	return asize;
}



static ssg::ParameterType utilCgParamGetType(CGparameter cgParam)
{
	ssg::ParameterCategory c = utilCgParamGetCategory(cgParam);
	if(c == ssg::kParameterCategorySampler){
		return ssg::kParameterTypeSampler;
	}else if(c == ssg::kParameterCategoryUnknown){
		return ssg::kParameterTypeUnknown;
	}
	CGtype  ct = cgGetParameterBaseType(cgParam);
	switch(ct)
	{
	case CG_FLOAT   : return ssg::kParameterTypeF32;
	case CG_HALF    : return ssg::kParameterTypeF16;
	case CG_FIXED   : return ssg::kParameterTypeC10;
	case CG_UINT    : return ssg::kParameterTypeU32;
	case CG_INT     : return ssg::kParameterTypeS32;
	case CG_USHORT  : return ssg::kParameterTypeU16;
	case CG_SHORT   : return ssg::kParameterTypeS16;
	case CG_UCHAR   : return ssg::kParameterTypeU8;
	case CG_CHAR    : return ssg::kParameterTypeS8;
	default : return ssg::kParameterTypeUnknown;
	}

}

static uint32_t utilCgParamGetSemanticIndex(CGparameter cgParam)
{
	util::checkForCgError("check");
	ssg::ParameterCategory c = utilCgParamGetCategory(cgParam);
	if(c != ssg::kParameterCategoryAttribute){
		return 0;
	}
	util::checkForCgError("getSemanticIndex");
	const char *sem = cgGetParameterSemantic(cgParam);
	if(strncmp(sem, "BLENDINDICES", strlen("BLENDINDICES")) == 0){
		return atoi(&sem[strlen("BLENDINDICES")]);
	}
	if(strncmp(sem, "BLENDWEIGHT", strlen("BLENDWEIGHT")) == 0){
		return atoi(&sem[strlen("BLENDWEIGHT")]);
	}
	return cgGetParameterResourceIndex(cgParam);
	
}

static uint32_t utilCgParamGetContainerIndex(CGparameter cgParam) 
{
	ssg::ParameterCategory c = utilCgParamGetCategory(cgParam);
	if(c != ssg::kParameterCategoryUniform){
		return 0;
	}
	return cgGetParameterBufferIndex(cgParam);
}


static uint32_t utilCgParamGetResourceIndex(CGparameter cgParam) 
{
	ssg::ParameterCategory c = utilCgParamGetCategory(cgParam);
	util::checkForCgError("getBufferIndex");
	if(c == ssg::kParameterCategoryUniform){
		int offset = cgGetParameterBufferOffset(cgParam);
		if(offset < 0){
			//printf(__FILE__"(%d) %s : %d\n", __LINE__, cgGetParameterName(cgParam), offset);
		}
		return offset;
	}else if((c == ssg::kParameterCategorySampler) || (c == ssg::kParameterCategoryAttribute)){
		return cgGetParameterResourceIndex(cgParam);
	}
	return 0;
}

static const std::string utilCgParamGetName(CGparameter cgParam) 
{
	return cgGetParameterName(cgParam);
}


//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// For Hlsl
//-----------------------------------------------------------------------

static ssgi::ParameterImpl utilHlslInitAttributeParam(const D3D11_SIGNATURE_PARAMETER_DESC &input_desc)
{
	ssg::ParameterSemantic semantic = ssg::kParameterSemanticNone;
	if(std::string(input_desc.SemanticName) == std::string("BLENDINDICES")){ semantic = ssg::kParameterSemanticBlendindices  ; }
	if(std::string(input_desc.SemanticName) == std::string("BLENDWEIGHT" )){ semantic = ssg::kParameterSemanticBlendweight   ; }
	if(std::string(input_desc.SemanticName) == std::string("BCOL"        )){ semantic = ssg::kParameterSemanticBcol          ; }
	if(std::string(input_desc.SemanticName) == std::string("FOGCOORD"    )){ semantic = ssg::kParameterSemanticFogcoord      ; }
	if(std::string(input_desc.SemanticName) == std::string("POINTSIZE"   )){ semantic = ssg::kParameterSemanticPointsize     ; }
	if(std::string(input_desc.SemanticName) == std::string("ATTR"        )){ semantic = ssg::kParameterSemanticAttr          ; }
	if(std::string(input_desc.SemanticName) == std::string("BINORMAL"    )){ semantic = ssg::kParameterSemanticBinormal      ; }
	if(std::string(input_desc.SemanticName) == std::string("BLENDINDICES")){ semantic = ssg::kParameterSemanticBlendindices  ; }
	if(std::string(input_desc.SemanticName) == std::string("BLENDWEIGHT" )){ semantic = ssg::kParameterSemanticBlendweight   ; }
	if(std::string(input_desc.SemanticName) == std::string("COLOR"       )){ semantic = ssg::kParameterSemanticColor         ; }
	if(std::string(input_desc.SemanticName) == std::string("DIFFUSE"     )){ semantic = ssg::kParameterSemanticDiffuse       ; }
	if(std::string(input_desc.SemanticName) == std::string("NORMAL"      )){ semantic = ssg::kParameterSemanticNormal        ; }
	if(std::string(input_desc.SemanticName) == std::string("POSITION"    )){ semantic = ssg::kParameterSemanticPosition      ; }
	if(std::string(input_desc.SemanticName) == std::string("SPECULAR"    )){ semantic = ssg::kParameterSemanticSpecular      ; }
	if(std::string(input_desc.SemanticName) == std::string("TANGENT"     )){ semantic = ssg::kParameterSemanticTangent       ; }
	if(std::string(input_desc.SemanticName) == std::string("TEXCOORD"    )){ semantic = ssg::kParameterSemanticTexcoord      ; }

	ssg::ParameterType type = ssg::kParameterTypeUnknown;
	switch(input_desc.ComponentType)
		{
		case D3D10_REGISTER_COMPONENT_UINT32:  type = ssg::kParameterTypeU32; break; 
		case D3D10_REGISTER_COMPONENT_SINT32:  type = ssg::kParameterTypeS32; break; 
		case D3D10_REGISTER_COMPONENT_FLOAT32: type = ssg::kParameterTypeF32; break; 
		default: type = ssg::kParameterTypeUnknown; break;
		}

	return ssgi::ParameterImpl(
							   "",                       //std::string       m_name,
							   semantic,                 //ParameterSemantic m_semantic,
							   input_desc.SemanticIndex, // uint32_t          m_semanticIndex,
							   4,                        //uint32_t          m_componentCount,
							   0,                        //uint32_t          m_arraySize,
							   ssg::kParameterCategoryAttribute, //ParameterCategory m_category,
							   type, //ParameterType     m_type,
							   0, //uint32_t          m_containerIndex,
							   input_desc.Register //  uint32_t          m_resourceIndex
							   );
	
	
}


static ssgi::ParameterImpl utilHlslInitUniformParam(uint32_t bufferIndex, const D3D11_SHADER_VARIABLE_DESC &varDesc)
{


	std::string name = varDesc.Name;
	if(bufferIndex == 0){
		if(name[0] == '_'){
			name.erase(0,1);
		}
	}
	//printf(__FILE__"(%d) name=%s, size=%d\n", __LINE__, name.c_str(), varDesc.Size);

	SCE_SAMPLE_UTIL_ASSERT( (varDesc.Size %  sizeof(float) ) == 0); // TODO. Types other than float can be stored to uniform param, but this was not considered when defining interface.
	return ssgi::ParameterImpl(
							   name,                   // std::string       m_name,
							   ssg::kParameterSemanticNone,    // ParameterSemantic m_semantic,
							   0,                              // uint32_t          m_semanticIndex,
							   varDesc.Size / sizeof(float),                   // uint32_t          m_componentCount,
							   1,                              // uint32_t          m_arraySize,
							   ssg::kParameterCategoryUniform, // ParameterCategory m_category,
							   ssg::kParameterTypeF32,          // ParameterType     m_type,
							   bufferIndex,                    // uint32_t          m_containerIndex,
							   varDesc.StartOffset             // uint32_t          m_resourceIndex
							   );

}

static ssgi::ParameterImpl utilHlslInitTextureParam(uint32_t bufferIndex, const D3D11_SHADER_INPUT_BIND_DESC &bindDesc)
{
	std::string name = bindDesc.Name;
	if((name[0] == '$') && (name[1] == '_')){
		name.erase(0,2);
		}

	//printf(__FILE__"(%d) name=%s BindPoint=%d, bindCount=%d\n", __LINE__, name.c_str(), bindDesc.BindPoint, bindDesc.BindCount );
	return ssgi::ParameterImpl(
							   name,                             // std::string       m_name,
							   ssg::kParameterSemanticNone,    // ParameterSemantic m_semantic,
							   0,                              // uint32_t          m_semanticIndex,
							   1,                               // uint32_t          m_componentCount,
							   bindDesc.BindCount,                              // uint32_t          m_arraySize,
							   ssg::kParameterCategorySampler, // ParameterCategory m_category,
							   ssg::kParameterTypeSampler,     // ParameterType     m_type,
							   0,                              // uint32_t          m_containerIndex,
							   bindDesc.BindPoint             // uint32_t          m_resourceIndex
							   );

}


//-----------------------------------------------------------------------



/*
ssgi::ParameterImpl::ParameterImpl(CGparameter cgParam) 
: m_cgParam(cgParam)
{
	checkForCgError("check");
	m_semantic       = utilCgParamGetSemantic(cgParam);
	checkForCgError("check2");
	m_semanticIndex  = utilCgParamGetSemanticIndex(cgParam);
	m_componentCount = utilCgParamGetComponentCount(cgParam);
	m_arraySize      = utilCgParamGetArraySize(cgParam);

	m_category       = utilCgParamGetCategory(cgParam);
	m_type           = utilCgParamGetType(cgParam);
	m_name           = utilCgParamGetName(cgParam);
	m_containerIndex = utilCgParamGetContainerIndex(cgParam);
	m_resourceIndex  = utilCgParamGetResourceIndex(cgParam);
	checkForCgError("check");
}
*/
ssgi::ParameterImpl::ParameterImpl(std::string       name,
								   ssg::ParameterSemantic semantic,
								   uint32_t          semanticIndex,
								   uint32_t          componentCount,
								   uint32_t          arraySize,
								   ssg::ParameterCategory category,
								   ssg::ParameterType     type,
								   uint32_t          containerIndex,
								   uint32_t          resourceIndex)
  : 
	m_name(name),
	m_semantic(semantic),
	m_semanticIndex(semanticIndex),
	m_componentCount(componentCount),
	m_arraySize(arraySize),
	m_category(category),
	m_type(type),
	m_containerIndex(containerIndex),
	m_resourceIndex(resourceIndex)
{

}


 

ssg::ParameterSemantic ssgi::ParameterImpl::getSemantic(void) const
{
	return m_semantic;
}

uint32_t ssgi::ParameterImpl::getComponentCount(void) const
{
	return m_componentCount;
}
uint32_t ssgi::ParameterImpl::getArraySize(void) const
{
	return m_arraySize;
}

uint32_t ssgi::ParameterImpl::getComponentCountAll(void) const
{
	return m_arraySize * m_componentCount;
}

bool ssgi::ParameterImpl::isSampler(void) const
{
	return getCategory() == kParameterCategorySampler;
}


bool ssgi::ParameterImpl::isUniform(void) const
{
	return getCategory() == kParameterCategoryUniform;
}

bool ssgi::ParameterImpl::isAttribute(void) const
{
	return getCategory() == kParameterCategoryAttribute;
}

bool ssgi::ParameterImpl::isUseDefaultBuf(void) const
{
	if(!isUniform()){
		return false;
	}
	util::checkForCgError("isUseDefaultBuf");
	return m_containerIndex == 0;
	//return (cgGetParameterBufferIndex(m_cgParam) == -1);
}

bool ssgi::ParameterImpl::isValid(void) const
{
	return getCategory() != ssg::kParameterCategoryUnknown;
	util::checkForCgError("isUseDefaultBuf");
	//return m_cgParam != NULL;
}

uint32_t ssgi::ParameterImpl::getContainerIndex(void) const
{
	return m_containerIndex;
}


uint32_t ssgi::ParameterImpl::getResourceIndex(void) const
{
	return m_resourceIndex;
}

ssg::ParameterCategory ssgi::ParameterImpl::getCategory(void) const
{
	return m_category;
}

uint32_t ssgi::ParameterImpl::getSemanticIndex(void) const
{
	return m_semanticIndex;
}

static uint32_t getParamTypeSize(ssg::ParameterType type)
{
	switch(type)
	{
	case ssg::kParameterTypeF32: return 4;
	case ssg::kParameterTypeF16: return 2;
	case ssg::kParameterTypeU32: return 4;
	case ssg::kParameterTypeS32:  return 4;
	case ssg::kParameterTypeU16: return 2;
	case ssg::kParameterTypeS16: return 2;
	case ssg::kParameterTypeU8: return 1;
	case ssg::kParameterTypeS8: return 1;
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
		// sorry C10 is not supported yet.
		//case PARAMETER_TYPE_C10 
		return 0;
	}
}


int ssgi::ParameterImpl::setUniformDataF(void *uniformBuffer, const float *sourceData) const
{
	if(!isUniform()){
		return SCE_OK;
	}


	util::checkForCgError("setUniformDataF");

	uint32_t componentCount = getComponentCount() * getArraySize();
	//cgSetParameterValuefr( m_cgParam, componentCount, sourceData);
	util::checkForCgError("getting latest profile options");
	
	uint32_t size = getParamTypeSize(getType()) * componentCount;
	uintptr_t b = (uintptr_t)uniformBuffer;
	memcpy((void*)(b + getResourceIndex()), sourceData, 
		getParamTypeSize(getType()) * componentCount);
	


	return 0;
}

const char *ssgi::ParameterImpl::getName(void) const
{
	return m_name.c_str();
}


ssg::ParameterType ssgi::ParameterImpl::getType(void) const
{
	return m_type;
}



ssgi::ProgramImpl::ProgramImpl(void) : 
	m_loader(NULL),
	m_numParameters(0),
	m_parameters(NULL),
	m_cgProgram(NULL),
	m_pBlob(NULL),
	m_defaultUniformBuffer(NULL),
	m_defaultUniformBufferSize(0)
{
}

ssgi::ProgramImpl::ProgramImpl(ssg::GraphicsLoader *loader, CGprogram cgProgram, ID3D10Blob* pBlob) : 
	m_loader(loader),
	m_numParameters(0),
	m_parameters(NULL),
	m_cgProgram(cgProgram),
	m_pBlob(pBlob)
{
	//printf(__FILE__"(%d) m_loader=%#x\n", __LINE__, m_loader);
	SCE_SAMPLE_UTIL_ASSERT(m_loader != NULL);
	std::vector<ParameterImpl> params;
	_getAllParameters(params);
	m_numParameters = params.size();
	if(m_numParameters > 0){
		m_parameters = new ParameterImpl[m_numParameters];
		for(uint32_t i=0; i<m_numParameters; i++){
			m_parameters[i] = params.at(i);
		}
	}

	m_defaultUniformBuffer = NULL;
	m_defaultUniformBufferSize = 0;

	ID3D11ShaderReflection* pReflector = NULL; 
	D3DReflect( m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(),	IID_ID3D11ShaderReflection, (void**) &pReflector);     
	
	ID3D11ShaderReflectionConstantBuffer* cb = pReflector->GetConstantBufferByIndex(0);
	if(cb != NULL){
		D3D11_SHADER_BUFFER_DESC cb_desc;
		cb->GetDesc(&cb_desc);
		m_defaultUniformBufferSize = cb_desc.Size;
		UniformBuffer *ub;
		int ret = loader->createUniformBuffer(&ub, m_defaultUniformBufferSize);
		//printf("ret = %#x, m_defaultUniformBufferSize=%d\n", ret, m_defaultUniformBufferSize);
		m_defaultUniformBuffer = dynamic_cast<UniformBufferImpl*>(ub);
		//printf("(%d) m_defaultUniformBuffer=%p\n", __LINE__, m_defaultUniformBuffer);
		//void * p= m_defaultUniformBuffer->beginWrite();
		//printf("(%d) p=%#x\n", __LINE__, p);
		//m_defaultUniformBuffer->endWrite();
	}



}

ssgi::ProgramImpl::ProgramImpl(const ProgramImpl& from)
{
	m_loader = from.m_loader;
	//printf(__FILE__"(%d) m_loader=%#x\n", __LINE__, m_loader);
	SCE_SAMPLE_UTIL_ASSERT(m_loader != NULL);
	if(m_parameters != NULL){
		delete [] m_parameters;
	}
	m_parameters = NULL;
	m_numParameters = from.m_numParameters;
	if(m_numParameters > 0){
		m_parameters = new ParameterImpl[m_numParameters];
		for(uint32_t i=0; i<m_numParameters; i++){
			m_parameters[i] = from.m_parameters[i];
		}
	}
	if(from.m_cgProgram != NULL){
		m_cgProgram = cgCopyProgram(from.m_cgProgram);
		m_pBlob =     cgD3D11GetCompiledProgram(from.m_cgProgram);
	}else{
		m_pBlob = from.m_pBlob;
		m_pBlob->AddRef();
	}
	
	m_defaultUniformBufferSize = from.m_defaultUniformBufferSize;
	if(m_defaultUniformBufferSize == 0){
		m_defaultUniformBuffer = NULL;
	}else{
		UniformBuffer *ub;
		m_loader->createUniformBuffer(&ub, m_defaultUniformBufferSize);
		m_defaultUniformBuffer = dynamic_cast<UniformBufferImpl*>(ub);
	}
}

ssgi::ProgramImpl::~ProgramImpl(void)
{
	if(m_parameters != NULL){
		delete [] m_parameters;
	}
	m_parameters = NULL;
	m_numParameters = 0;

	if(m_cgProgram != NULL){
		cgD3D11UnloadProgram(m_cgProgram);
		cgDestroyProgram(m_cgProgram);
		m_cgProgram = NULL;
		m_pBlob;
	}else if(m_pBlob != NULL){
		m_pBlob->Release();
	}

	if(m_defaultUniformBuffer){
		sce::SampleUtil::destroy(dynamic_cast<UniformBuffer*>(m_defaultUniformBuffer));
	}
	m_defaultUniformBuffer = NULL;
	m_defaultUniformBufferSize;

}



static void pushStructParameters(std::vector<ssgi::ParameterImpl> &params, CGparameter structParam)
{
	CGparameter cgparam = cgGetFirstStructParameter( structParam );
	while ( cgparam ){
		cgparam = cgGetNextParameter( cgparam );
	}
}


void ssgi::ProgramImpl::_getAllParameters(std::vector<ParameterImpl> &params) const
{
	HRESULT hr;
	if(!isValid()){
		return;
	}
	params.clear();

	ID3D11ShaderReflection* pReflector = NULL; 
	D3DReflect( m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(),	IID_ID3D11ShaderReflection, (void**) &pReflector);     
	D3D11_SHADER_DESC desc;
	pReflector->GetDesc(&desc);
	for(uint32_t i=0; i<desc.InputParameters; i++){
		D3D11_SIGNATURE_PARAMETER_DESC input_desc;
		pReflector->GetInputParameterDesc(i, &input_desc);
		//printf("[Input (%d/%d)] %s:%d\n", i, desc.InputParameters, input_desc.SemanticName, input_desc.SemanticIndex);

		ssgi::ParameterImpl param = utilHlslInitAttributeParam(input_desc);
		
		params.push_back(param);
	}
	for(uint32_t i=0; i<desc.OutputParameters; i++){
		D3D11_SIGNATURE_PARAMETER_DESC output_desc;
		pReflector->GetOutputParameterDesc(i, &output_desc);
		//printf("[Output] %s:%d\n", output_desc.SemanticName, output_desc.SemanticIndex);
	}

	for(uint32_t c=0; c<desc.ConstantBuffers; c++){
		ID3D11ShaderReflectionConstantBuffer *constBuffer = pReflector->GetConstantBufferByIndex(c);
		D3D11_SHADER_BUFFER_DESC constBufferDesc;
		constBuffer->GetDesc(&constBufferDesc);
		for(uint32_t v=0; v<constBufferDesc.Variables; v++){
			ID3D11ShaderReflectionVariable* var = constBuffer->GetVariableByIndex(v);
			D3D11_SHADER_VARIABLE_DESC varDesc;
			var->GetDesc(&varDesc);
			//printf("[Buf:%d][var] %s.%s off=%d, size=%d\n", c, constBufferDesc.Name, varDesc.Name, varDesc.StartOffset, varDesc.Size);
			ssgi::ParameterImpl param = utilHlslInitUniformParam(c, varDesc);
			params.push_back(param);
		}
		
	}

	for(uint32_t b=0; b<desc.BoundResources; b++){
		D3D11_SHADER_INPUT_BIND_DESC desc;
		hr = pReflector->GetResourceBindingDesc(b, &desc);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(hr, SCE_OK);
		if(desc.Type == D3D10_SIT_SAMPLER){
			//printf("Bound resource : %s, uFlags=%#x, type=%d, isTexture=%d\n", desc.Name, desc.uFlags, desc.Type, desc.Type == D3D10_SIT_TEXTURE);
			ssgi::ParameterImpl param = utilHlslInitTextureParam(b, desc);
			params.push_back(param);
		}

	}
	

	if(m_cgProgram != NULL){
		CGparameter cgparam = cgGetFirstParameter( m_cgProgram, CG_GLOBAL );
		while ( cgparam )
		{
			if(cgGetParameterType(cgparam) == CG_STRUCT ){
				pushStructParameters(params, cgparam);
			}
			cgparam = cgGetNextParameter( cgparam );
		}
		
		cgparam = cgGetFirstParameter( m_cgProgram, CG_PROGRAM );
		while ( cgparam )
		{
			if(cgGetParameterType(cgparam) == CG_STRUCT ){
				pushStructParameters(params, cgparam);
			}
			cgparam = cgGetNextParameter( cgparam );
		}
	}

}

const ssg::Dx11::ParameterCg *ssg::Dx11::ParameterCg::downcast(const ssg::Parameter* param)
{
	return dynamic_cast<const ssg::Dx11::ParameterCg*>(param);
}




#endif /* _SCE_TARGET_OS_WINDOWS */


