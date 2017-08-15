/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS


#include <sampleutil/sampleutil_common.h>
#include <sampleutil/sampleutil_error.h>
#include "ngs2_internal.h"

namespace ssa = sce::SampleUtil::Audio;
namespace ssai = sce::SampleUtil::Audio::Internal;

static int32_t ioRead(uintptr_t userData, uint32_t offset, void *data, size_t size){
	FILE *fp = (FILE*)userData;
	int32_t ret;

	ret = fseek( fp, offset, SEEK_SET);
	if(ret < 0){
		return -1;
	}
	
	ret = fread( data, 1, size, fp);
	if(ret != size){
		return -1;
	}
	return ret;
}

int ssa::Data::FileInfo::initFromPath(const char *path)
{
	FILE *fp =  fopen(path, "rb");
	if(fp == NULL){
		return -1;
	}
	int ret = sceNgs2ParseWaveformUser( ioRead, (uintptr_t)fp, &waveformInfo);

	if(ret != SCE_OK){
		printf(__FILE__"(%d) Error : sceNgs2ParseWaveformFile( %s, 0, &waveformInfo ) failed;\n", __LINE__, path);
		printf(__FILE__"(%d) ret = %#x\n", __LINE__, ret);
	}
	fclose(fp);

	return ret;
}



int ssai::VoiceDataImpl::initializeAsStreaming(const char *path)
{
	m_type = TYPE_STREAMING;

	int ret = m_fileInfo.initFromPath(path);
	if(ret < SCE_OK){
		m_type = TYPE_UNKNOWN;
		return ret;
	}

	m_streamingSoundInfo.m_fileInfo = m_fileInfo;
	m_streamingSoundInfo.m_filePath = path;
	return SCE_OK;
}

int ssai::VoiceDataImpl::initializeAsOnMemoryData(const char *path)
{
	m_type = TYPE_ON_MEMORY;
	int ret = m_fileInfo.initFromPath(path);
	if(ret < SCE_OK){
		m_type = TYPE_UNKNOWN;
		return ret;
	}
	m_onMemorySoundInfo.setFileInfo(&m_fileInfo);
	m_onMemorySoundInfo.m_data = memalign(4, m_fileInfo.waveformInfo.dataSize);
	if(m_onMemorySoundInfo.m_data == NULL){
		m_type = TYPE_UNKNOWN;
		return -1;
	}

	FILE *file = fopen(path, "rb");
	if(file == NULL){
		free(m_onMemorySoundInfo.m_data);
		m_type = TYPE_UNKNOWN;
		return -1;
	}

	ret = fseek(file, m_fileInfo.waveformInfo.dataOffset, SEEK_SET);
	if(ret < 0){
		fclose(file);
		free(m_onMemorySoundInfo.m_data);
		m_type = TYPE_UNKNOWN;
		return -1;
	}

	ret = fread(m_onMemorySoundInfo.m_data, 1, m_fileInfo.waveformInfo.dataSize, file);
	if(ret != m_fileInfo.waveformInfo.dataSize){
		fclose(file);
		free(m_onMemorySoundInfo.m_data);
		m_type = TYPE_UNKNOWN;
		return -1;
	}
	for(uint32_t i=0; i<m_fileInfo.waveformInfo.numBlocks; i++){
		m_fileInfo.waveformInfo.aBlock[i].dataOffset -= m_fileInfo.waveformInfo.dataOffset;
	}

	m_fileInfo.waveformInfo.dataOffset = 0;
	fclose(file);

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
		free(m_onMemorySoundInfo.m_data);
	}
	m_type = TYPE_UNKNOWN;
	return SCE_OK;
}

//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------

int ssa::createVoiceDataFromFile(VoiceData **outVoiceData, const char* path, bool isStreaming)
{
	if( outVoiceData == NULL ) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	ssai::VoiceDataImpl *impl = new ssai::VoiceDataImpl;
	int ret = SCE_OK;
	if(isStreaming){
		if(!sceFiosIsInitialized(NULL)){
			delete impl;
			return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
		}
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
	if( outVoiceData == NULL ) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
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



#endif // defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS