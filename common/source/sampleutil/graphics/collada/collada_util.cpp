/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"


#include <graphics/collada/collada_util.h>
#include <stdio.h>
#include <errno.h>


std::string sce::SampleUtil::Graphics::Collada::Internal::Util::getExtention(const std::string &path)
{
	int p = path.find_last_of(".");
	if(p == path.npos){
		return "";
	}
	std::string ext = path.substr(p+1, path.npos);
	return ext;
}


void* sce::SampleUtil::Graphics::Collada::Internal::Util::malloc_and_load(const char* path, uint32_t *pSize)
{
	int ret;

	FILE *fp = fopen(path, "rb");
	if(fp == NULL){
		fprintf(stderr, "Error : Opening %s failed.\n", path);
		return NULL;
	}
	ret = fseek(fp, 0, SEEK_END);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	int32_t size = ftell(fp);
	SCE_SAMPLE_UTIL_ASSERT(size > 0);

	ret = fseek(fp, 0, SEEK_SET);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	void *elf = malloc(size);
	if(elf == NULL){
		fprintf(stderr, "Error : No memory. %s couldn't be loaded.\n", path);
		return NULL;
	}

	ret = fread(elf, 1, size, fp);
	SCE_SAMPLE_UTIL_ASSERT(ret == size);

	ret = fclose(fp);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	if(pSize != NULL){
		*pSize = size;
	}
	
	(void)ret;
	return elf;
}


using namespace sce::Vectormath::Simd::Aos;

void sce::SampleUtil::Graphics::Collada::Internal::Util::tokenize(std::vector<std::string>& tokens, 
									   const std::string& str,
									   const std::string& delimiters)
{

	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		std::string ss = str.substr(lastPos, pos - lastPos);
		tokens.push_back(ss);
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}


int sce::SampleUtil::Graphics::Collada::Internal::Util::parseIntArray(int *array, unsigned int arrayLength, std::string arrayStr)
{
#if 0
	std::vector<std::string> tokens;
	tokenize(tokens, arrayStr.c_str());
	if(tokens.size() != arrayLength){
		return -1;
	}
	for(unsigned int i=0; i<tokens.size(); i++){
		char *e;
		array[i] = strtol(tokens.at(i).c_str(), &e, 10);
		if (errno != 0) {
			return -1;
		}
	}
#else
	const char *str = arrayStr.c_str();
	for(uint32_t i=0; i<arrayLength; i++){
		while(str[0] == ' '){ 
			str++;
		}
		char *e;
		errno = 0;
		array[i] = strtol(str, &e, 10);
		if (errno != 0) {
			return -1;
		}
		str = e;
	}

#endif
	return 0;
}

int sce::SampleUtil::Graphics::Collada::Internal::Util::parseFloatArray(float *array, unsigned int arrayLength, std::string arrayStr)
{
#if 0
	std::vector<std::string> tokens;
	tokenize(tokens, arrayStr.c_str());
	if(tokens.size() != arrayLength){
		return -1;
	}
	for(unsigned int i=0; i<tokens.size(); i++){
		char *e;
		errno = 0;
		array[i] = (float)strtod(tokens.at(i).c_str(), &e);
		if ((errno != 0) || (array[i] == HUGE_VAL)) {
			return -1;
		}
	}
#elif 0
	std::string &str = arrayStr;
	const std::string delimiters = " ";
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	for(uint32_t i=0; i<arrayLength; i++){
		std::string ss = str.substr(lastPos, pos - lastPos);
		char *e;
		array[i] = (float)strtod(ss.c_str(), &e);
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
#else 
	const char *str = arrayStr.c_str();

	for(uint32_t i=0; i<arrayLength; i++){
		while(str[0] == ' '){ 
			str++;
		}
		char *e;
		array[i] = (float)strtod(str, &e);
		str = e;
	}


#endif
	return SCE_OK;
}

Matrix4 sce::SampleUtil::Graphics::Collada::Internal::Util::fromFloatArray(const float *ary) 
{
	Vector4 v0 = Vector4( ary[0], ary[4], ary[8], ary[12]);
	Vector4 v1 = Vector4( ary[1], ary[5], ary[9], ary[13]);
	Vector4 v2 = Vector4( ary[2], ary[6], ary[10], ary[14]);
	Vector4 v3 = Vector4( ary[3], ary[7], ary[11], ary[15]);
	Matrix4 ret =  Matrix4(v0, v1, v2, v3);
	return ret;
}

Matrix4 sce::SampleUtil::Graphics::Collada::Internal::Util::fromElementText(TiXmlElement* elem)
{
	float ary[16];
	parseFloatArray(ary, 16, elem->GetText());
	return fromFloatArray(ary);
}
