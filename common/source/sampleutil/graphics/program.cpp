/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#include "stdafx.h"
#include "util/util.h"
#include <sampleutil/graphics/program.h>
#include "graphics_internal.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

#if _SCE_TARGET_OS_PSP2 || _SCE_TARGET_OS_WINDOWS

uint32_t ssgi::ProgramImpl::getParameterCount(void) const
{
	return m_numParameters;
}

const ssgi::ParameterImpl* ssgi::ProgramImpl::findParameterByName(const char *name) const
{
	for(uint32_t i=0; i<m_numParameters; i++){
		if(strcmp(m_parameters[i].getName(), name) == 0){
			return &m_parameters[i];
		}
	}

	return NULL;
}

const ssgi::ParameterImpl* ssgi::ProgramImpl::getParameter(uint32_t index) const
{
	if(index >= m_numParameters){
		return NULL;
	}
	return &m_parameters[index];
}

const ssgi::ParameterImpl* ssgi::ProgramImpl::findParameterBySemantic(ssg::ParameterSemantic semantic, uint32_t semanticIndex) const
{
	for(uint32_t i=0; i<m_numParameters; i++){
		if((m_parameters[i].getSemantic() == semantic) && (m_parameters[i].getSemanticIndex() == semanticIndex)){
			return &m_parameters[i];
		}
	}
	return NULL;
}


void ssgi::ProgramImpl::getAllParameters(std::vector<const ssg::Parameter*> &params) const
{
	params.clear();
	for(uint32_t i=0; i<m_numParameters; i++){
		SCE_SAMPLE_UTIL_ASSERT( m_parameters[i].isValid() );
		params.push_back(&m_parameters[i]);
	}
}



#endif // _SCE_TARGET_OS_PSP2 || _SCE_TARGET_OS_WINDOWS

