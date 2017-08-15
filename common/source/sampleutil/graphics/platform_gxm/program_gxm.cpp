/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2


#include <sampleutil/graphics.h>
#include "../graphics_internal.h"
#include "../constant_internal.h"
#include "util/util.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;


ssg::ParameterSemantic ssgi::ParameterImpl::getSemantic(void) const
{
	switch(sceGxmProgramParameterGetSemantic(m_gxmParam))
	{
	case SCE_GXM_PARAMETER_SEMANTIC_ATTR: return kParameterSemanticAttr         ;
	case SCE_GXM_PARAMETER_SEMANTIC_BCOL: return kParameterSemanticBcol         ;
	case SCE_GXM_PARAMETER_SEMANTIC_BINORMAL: return kParameterSemanticBinormal      ;
	case SCE_GXM_PARAMETER_SEMANTIC_BLENDINDICES: return kParameterSemanticBlendindices  ;
	case SCE_GXM_PARAMETER_SEMANTIC_BLENDWEIGHT: return kParameterSemanticBlendweight   ;
	case SCE_GXM_PARAMETER_SEMANTIC_COLOR: return kParameterSemanticColor         ;
	case SCE_GXM_PARAMETER_SEMANTIC_DIFFUSE: return kParameterSemanticDiffuse       ; 
	case SCE_GXM_PARAMETER_SEMANTIC_FOGCOORD: return kParameterSemanticFogcoord      ; 
	case SCE_GXM_PARAMETER_SEMANTIC_NORMAL: return kParameterSemanticNormal        ; 
	case SCE_GXM_PARAMETER_SEMANTIC_POINTSIZE: return kParameterSemanticPointsize     ; 
	case SCE_GXM_PARAMETER_SEMANTIC_POSITION: return kParameterSemanticPosition      ; 
	case SCE_GXM_PARAMETER_SEMANTIC_SPECULAR: return kParameterSemanticSpecular      ; 
	case SCE_GXM_PARAMETER_SEMANTIC_TANGENT: return kParameterSemanticTangent       ;
	case SCE_GXM_PARAMETER_SEMANTIC_TEXCOORD: return kParameterSemanticTexcoord      ; 
	default: return kParameterSemanticNone;
	}
}


uint32_t ssgi::ParameterImpl::getComponentCount(void) const
{
	return sceGxmProgramParameterGetComponentCount(m_gxmParam);
}
uint32_t ssgi::ParameterImpl::getArraySize(void) const
{
	return sceGxmProgramParameterGetArraySize(m_gxmParam);
}
uint32_t ssgi::ParameterImpl::getComponentCountAll(void) const
{
	return getComponentCount() * getArraySize();
}


bool ssgi::ParameterImpl::isSampler(void) const
{

	SceGxmParameterCategory category = sceGxmProgramParameterGetCategory(m_gxmParam);
	return (category == SCE_GXM_PARAMETER_CATEGORY_SAMPLER);
}


bool ssgi::ParameterImpl::isUniform(void) const
{
	SceGxmParameterCategory category = sceGxmProgramParameterGetCategory(m_gxmParam);
	return (category == SCE_GXM_PARAMETER_CATEGORY_UNIFORM);
}

bool ssgi::ParameterImpl::isAttribute(void) const
{
	SceGxmParameterCategory category = sceGxmProgramParameterGetCategory(m_gxmParam);
	return (category == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE);
}

bool ssgi::ParameterImpl::isUseDefaultBuf(void) const
{
	SceGxmParameterCategory category = sceGxmProgramParameterGetCategory(m_gxmParam);
	uint32_t containerIndex = sceGxmProgramParameterGetContainerIndex(m_gxmParam);
	return (containerIndex >= SCE_GXM_MAX_UNIFORM_BUFFERS) && (category == SCE_GXM_PARAMETER_CATEGORY_UNIFORM);
}

uint32_t ssgi::ParameterImpl::getContainerIndex(void) const
{
	return sceGxmProgramParameterGetContainerIndex(m_gxmParam);
}

uint32_t ssgi::ParameterImpl::getResourceIndex(void) const
{
	return sceGxmProgramParameterGetResourceIndex(m_gxmParam);
}


uint32_t ssgi::ParameterImpl::getSemanticIndex(void) const
{
	return sceGxmProgramParameterGetSemanticIndex(m_gxmParam);
}

int ssgi::ParameterImpl::setUniformDataF(void *uniformBuffer, const float *sourceData) const
{
	uint32_t componentCount = getComponentCount() * getArraySize();
	return sceGxmSetUniformDataF(uniformBuffer,m_gxmParam,0, componentCount,sourceData);
}

const char *ssgi::ParameterImpl::getName(void) const
{
	return sceGxmProgramParameterGetName(m_gxmParam);
}

ssg::ParameterCategory ssgi::ParameterImpl::getCategory(void) const
{
	switch(sceGxmProgramParameterGetCategory(m_gxmParam))
	{
	case SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE: return kParameterCategoryAttribute;
	case SCE_GXM_PARAMETER_CATEGORY_UNIFORM:  return kParameterCategoryUniform; 
	case SCE_GXM_PARAMETER_CATEGORY_SAMPLER:  return kParameterCategorySampler;
	default: return kParameterCategoryUnknown;
	}
}

bool ssgi::ParameterImpl::isValid(void) const
{
	return m_gxmParam != NULL;
}

ssg::ParameterType ssgi::ParameterImpl::getType(void) const
{
	ParameterCategory c = getCategory();
	if(c == kParameterCategorySampler){
		return kParameterTypeSampler;
	}else if(c == kParameterCategoryUnknown){
		return kParameterTypeUnknown;
	}
	SceGxmParameterType gt = sceGxmProgramParameterGetType(m_gxmParam);
	switch(gt)
	{
	case SCE_GXM_PARAMETER_TYPE_F32 : return kParameterTypeF32;
	case SCE_GXM_PARAMETER_TYPE_F16  : return kParameterTypeF16;
	case SCE_GXM_PARAMETER_TYPE_C10  : return kParameterTypeC10;
	case SCE_GXM_PARAMETER_TYPE_U32  : return kParameterTypeU32;
	case SCE_GXM_PARAMETER_TYPE_S32  : return kParameterTypeS32;
	case SCE_GXM_PARAMETER_TYPE_U16  : return kParameterTypeU16;
	case SCE_GXM_PARAMETER_TYPE_S16  : return kParameterTypeS16;
	case SCE_GXM_PARAMETER_TYPE_U8  : return kParameterTypeU8;
	case SCE_GXM_PARAMETER_TYPE_S8  : return kParameterTypeS8;
	default : return kParameterTypeUnknown;
	}

}

const ssg::Gxm::ParameterGxm *ssg::Gxm::ParameterGxm::downcast(const ssg::Parameter* param)
{
	return dynamic_cast<const ssg::Gxm::ParameterGxm*>(param);
}





ssgi::ProgramImpl::ProgramImpl(void) : 
m_numParameters(0),
m_parameters(NULL),
m_patcherId(NULL)
{

}

ssgi::ProgramImpl::ProgramImpl(SceGxmShaderPatcherId patcherId, ssgi::GraphicsLoaderImpl *loader) : 
m_numParameters(0),
m_parameters(NULL),
m_patcherId(patcherId),
m_loader(loader)
{
	std::vector<ParameterImpl> params;
	_getAllParameters(params);
	m_numParameters = params.size();
	if(m_numParameters > 0){
		m_parameters = new ParameterImpl[m_numParameters];
		for(uint32_t i=0; i<m_numParameters; i++){
			m_parameters[i] = params.at(i);
			SCE_SAMPLE_UTIL_ASSERT( m_parameters[i].isValid() );
		}
	}


}

ssgi::ProgramImpl::ProgramImpl(const ProgramImpl& from):
m_numParameters(0),
m_parameters(NULL),
m_patcherId(from.m_patcherId),
m_loader(from.m_loader)
{
	m_parameters = NULL;
	m_numParameters = from.m_numParameters;
	if(m_numParameters > 0){
		m_parameters = new ParameterImpl[m_numParameters];
		for(uint32_t i=0; i<m_numParameters; i++){
			m_parameters[i] = from.m_parameters[i];
			SCE_SAMPLE_UTIL_ASSERT( m_parameters[i].isValid() );
		}
	}
	m_loader->_referProgram(getGxmProgram());
}

ssgi::ProgramImpl &ssgi::ProgramImpl::operator=(const ssgi::ProgramImpl &from)
{
	if(m_parameters != NULL){
		delete [] m_parameters;
	}
	m_patcherId = from.m_patcherId;
	m_numParameters = from.m_numParameters;
	m_loader = from.m_loader;
	if(m_numParameters > 0){
		m_parameters = new ParameterImpl[m_numParameters];
		for(uint32_t i=0; i<m_numParameters; i++){
			m_parameters[i] = from.m_parameters[i];
			SCE_SAMPLE_UTIL_ASSERT( m_parameters[i].isValid() );
		}
	}
	m_loader->_referProgram(getGxmProgram());

	return *this;
}


ssgi::ProgramImpl::~ProgramImpl(void)
{
	if(m_parameters != NULL){
		delete [] m_parameters;
	}
	m_parameters = NULL;
	m_numParameters = 0;
	m_loader->_unregisterProgram(getGxmProgram());
}





void ssgi::ProgramImpl::_getAllParameters(std::vector<ssgi::ParameterImpl> &params) const
{
	params.clear();
	const SceGxmProgram *program = sceGxmShaderPatcherGetProgramFromId(m_patcherId);
	if(program == NULL){
		return;
	}
	uint32_t count = sceGxmProgramGetParameterCount(program);

	for(uint32_t i=0; i<count; i++){
		ParameterImpl p = ssgi::ParameterImpl(sceGxmProgramGetParameter(program, i));
		SCE_SAMPLE_UTIL_ASSERT( p.isValid() );
		params.push_back(p);
	}

}


const ssg::Gxm::ProgramGxm *ssg::Gxm::ProgramGxm::downcast(const ssg::Program* param)
{
	return dynamic_cast<const ssg::Gxm::ProgramGxm *>(param);
}



#endif /* _SCE_TARGET_OS_PSP2 */


