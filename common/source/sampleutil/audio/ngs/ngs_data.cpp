/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h" 

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2
#include <libdbg.h>

#include <stdio.h>
#include <string.h>
#include "ngs_internal.h"
#include "util/util.h"
#include "audio/audio_internal.h"

namespace ssiu = sce::SampleUtil::Internal::Util;


namespace ssa    = sce::SampleUtil::Audio;
namespace ssai   = sce::SampleUtil::Audio::Internal;
namespace ssaid  = sce::SampleUtil::Audio::Internal::Data;


int sce::SampleUtil::Audio::Internal::VoiceDataImpl::initializeAsStreaming(const char *path)
{
	m_type = TYPE_STREAMING;

	sce::SampleUtil::Internal::Util::Reader reader;
	int ret = reader.initializeFromFile(path);
	if(ret != SCE_OK){
		return ret;
	}
	ret = m_audioFile.parse(&reader);
	if(ret != SCE_OK){
		return ret;
	}

	m_streamingFilePath = path;
	return SCE_OK;
}

int ssai::VoiceDataImpl::initializeAsOnMemoryData(const char *path)
{
	m_type = TYPE_ON_MEMORY;

	sce::SampleUtil::Internal::Util::Reader reader;
	int ret = reader.initializeFromFile(path);
	if(ret != SCE_OK){
		return ret;
	}
	ret = m_audioFile.parse(&reader);
	if(ret != SCE_OK){
		return ret;
	}

	size_t size;
	m_fileImage = ssiu::malloc_and_load(path, &size);
	if(m_fileImage == NULL){
		return -1;
	}

	return SCE_OK;
}

int ssai::VoiceDataImpl::initializeAsGenerator(const GeneratorSettings *setting)
{
	if(setting == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_type = TYPE_GENERATOR;
	m_generatorInfo = *setting;

	return SCE_OK;
}


int ssai::VoiceDataImpl::finalize(void)
{
	if(m_type == TYPE_ON_MEMORY){
		free(m_fileImage);
		m_fileImage = NULL;
	}
	m_type = TYPE_GENERATOR;
	return SCE_OK;
}


int ssa::createVoiceDataFromFile(VoiceData **outVoiceData, const char* path, bool isStreaming)
{
	ssai::VoiceDataImpl *impl = new ssai::VoiceDataImpl;
	int ret = SCE_OK;
	if(isStreaming){
		ret = impl->initializeAsStreaming(path);
	}else{
		ret = impl->initializeAsOnMemoryData(path);
	}
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outVoiceData = impl;
	return SCE_OK;
}

int ssa::createVoiceDataFromGeneratorSetting(VoiceData **outVoiceData, const GeneratorSettings *setting)
{
	ssai::VoiceDataImpl *impl = new ssai::VoiceDataImpl;
	int ret = SCE_OK;
	ret = impl->initializeAsGenerator(setting);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outVoiceData = impl;
	return SCE_OK;
}

#endif /* _SCE_TARGET_OS_PSP2 */
