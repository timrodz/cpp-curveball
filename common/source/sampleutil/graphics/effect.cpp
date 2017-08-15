/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"
#include <sampleutil/graphics/effect.h>
#include "util/util.h"
#include <vector>
#include "graphics_internal.h"


using namespace sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

#pragma region ParameterValue



Effect::ParameterValue::ParameterValue(void)
{
	m_category   = kParameterCategoryUnknown;
	m_count  = 0;
	m_texture    = NULL;
	m_array = NULL;
}

Effect::ParameterValue::~ParameterValue(void)
{
	finalize();
}


static uint32_t getParamTypeSize(ParameterType type)
{
	switch(type)
	{
	case kParameterTypeF32: return 4;
	case kParameterTypeF16: return 2;
	case kParameterTypeU32: return 4;
	case kParameterTypeS32:  return 4;
	case kParameterTypeU16: return 2;
	case kParameterTypeS16: return 2;
	case kParameterTypeU8: return 1;
	case kParameterTypeS8: return 1;
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
		// sorry C10 is not supported yet.
		//case PARAMETER_TYPE_C10 
		return 0;
	}
}

int Effect::ParameterValue::initializeAsArray(ParameterType type, uint32_t count, const void *initData)
{
	if(m_category  != kParameterCategoryUnknown){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	m_category = kParameterCategoryUniform;
	m_parameterType = type;
	m_count = count;
	uint32_t size = count * getParamTypeSize(type);
	m_array = malloc(size);
	if(initData != NULL){
		memcpy(m_array, initData, size);
	}else{
		memset(m_array, 0, size);
	}
	return 0;
}


int Effect::ParameterValue::initializeAsTexture(Texture *texture)
{
	if(m_category  != kParameterCategoryUnknown){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	m_category = kParameterCategorySampler;
	m_parameterType = kParameterTypeSampler;
	m_texture = texture;
	m_count = 0;
	return SCE_OK;
}


int Effect::ParameterValue::finalize(void)
{
	if(m_category == kParameterCategoryUniform){
		free(m_array);
		m_array = NULL;
	}
	m_category = kParameterCategoryUnknown;
	return SCE_OK;
}


int Effect::ParameterValue::copyFrom(const ParameterValue *from)
{
	if(from == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	finalize();
	m_category = from->m_category;
	m_parameterType = from->m_parameterType;
	m_count    = from->m_count;

	if(from->m_category == kParameterCategoryUniform){
		uint32_t size = m_count * getParamTypeSize(m_parameterType);	
		m_array = malloc(size);
		memcpy(m_array, from->m_array, size);
	}else if(from->m_category == kParameterCategorySampler){
		m_texture = from->m_texture;
	}
	return SCE_OK;
}

int Effect::ParameterValue::setArrayDataF(const float *value, uint32_t componentCount)
{
	if(value == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(m_category != kParameterCategoryUniform){
		return -1;
	}
	if(componentCount == 0){
		componentCount = m_count;
	}
	if(componentCount > m_count){
		return -1;
	}
	switch(m_parameterType)
	{
	case kParameterTypeF32:
		{
			float *f = (float*)m_array;
			for(uint32_t i=0; i<componentCount; i++){
				f[i] = value[i];
			} 
			return SCE_OK;
		}
#ifdef __psp2__
	case kParameterTypeF16: 
		{
			__fp16 *f = (__fp16*)m_array;
			for(uint32_t i=0; i<componentCount; i++){
				__fp16 v = value[i];
				f[i] = v;
			} 
			return SCE_OK;
		}
#endif
	case kParameterTypeU32: 
		{
			uint32_t *f = (uint32_t*)m_array;
			for(uint32_t i=0; i<componentCount; i++){
				uint32_t v = (uint32_t)value[i];
				f[i] = v;
			} 
			return SCE_OK;
		}
	case kParameterTypeS32:  
		{
			int32_t *f = (int32_t*)m_array;
			for(uint32_t i=0; i<componentCount; i++){
				int32_t v = (int32_t)value[i];
				f[i] = v;
			} 
			return SCE_OK;
		}
	case kParameterTypeU16: 
		{
			uint16_t *f = (uint16_t*)m_array;
			for(uint32_t i=0; i<componentCount; i++){
				uint16_t v = (uint16_t)value[i];
				f[i] = v;
			} 
			return SCE_OK;
		}
	case kParameterTypeS16: 
		{
			int16_t *f = (int16_t*)m_array;
			for(uint32_t i=0; i<componentCount; i++){
				int16_t v = (int16_t)value[i];
				f[i] = v;
			} 
			return SCE_OK;
		}
	case kParameterTypeU8: 
		{
			uint8_t *f = (uint8_t*)m_array;
			for(uint32_t i=0; i<componentCount; i++){
				uint8_t v = (uint8_t)value[i];
				f[i] = v;
			} 
			return SCE_OK;
		}
	case kParameterTypeS8: 
		{
			int8_t *f = (int8_t*)m_array;
			for(uint32_t i=0; i<componentCount; i++){
				int8_t v = (int8_t)value[i];
				f[i] = v;
			} 
			return SCE_OK;
		}
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
		// sorry C10 is not supported yet.
		//case PARAMETER_TYPE_C10 
		return 0;
	}

}

int Effect::ParameterValue::setTexture(Texture *texture)
{
	if(m_category == kParameterCategorySampler){
		m_texture = texture;
		return SCE_OK;
	}else{
		return -1;
	}
}

int Effect::ParameterValue::setArrayData(const void *value, uint32_t numBytes)
{
	if(value == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	uint32_t arraySize = m_count * getParamTypeSize(m_parameterType);	
	if(numBytes==0){
		numBytes = arraySize;
	}
	if(m_category == kParameterCategoryUniform){
		if(numBytes > arraySize){
			return -1;
		}
		memcpy(m_array, value, numBytes);
		return SCE_OK;
	}else{
		return -1;
	}
}

#pragma endregion //ParameterValue

#pragma region EffectParameter

Effect::EffectParameter::EffectParameter(void)
{
	m_param = NULL;
}




int Effect::EffectParameter::initialize(const Parameter *param, ProgramType programType)
{
	if(param == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(m_param != NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	m_param = param;
	m_programType = programType;
	if(m_param->isUseDefaultBuf()){
		m_value.initializeAsArray(m_param->getType(), m_param->getComponentCount() * m_param->getArraySize(), NULL);
	}else if(m_param->getCategory() == kParameterCategorySampler){
		m_value.initializeAsTexture(NULL);
	}
	return SCE_OK;
}

int Effect::EffectParameter::setToUniformBuffer(GraphicsContext *context, void *buffer)
{
	// buffer can be NULL.
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(m_param->isUseDefaultBuf()){
		if(m_value.getCategory() == kParameterCategoryUniform){
			int ret = m_param->setUniformDataF(buffer, (const float*)m_value.getData());
			if(ret != SCE_OK){
				return ret;
			}
		}
	}

	if(m_param->getCategory() == kParameterCategorySampler){
		if(m_value.getCategory() == kParameterCategorySampler){
			int ret = context->setTexture(m_programType, m_value.getTexture(), m_param->getResourceIndex());
			if(ret != SCE_OK){
				return ret;
			}
		}
	}
	return SCE_OK;
}

int Effect::EffectParameter::finalize(void)
{
	int ret = m_value.finalize();
	m_param = NULL;
	return ret;
}
Effect::EffectParameter::~EffectParameter(void)
{
	finalize();
}

ProgramType    Effect::EffectParameter::getProgramType(void) const
{
	return m_programType;
}


int Effect::EffectParameter::copyFrom(const EffectParameter* from)
{
	int ret;

	if(from == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	finalize();
	m_param = from->m_param;
	m_programType = from->m_programType;

	ret = m_value.copyFrom(&from->m_value);

	return ret;
}

#pragma endregion //  EffectParameter


/* ------------------------------------------------------------------------------------ */

#pragma region EffectShader

int Effect::EffectShader::initialize(const Program* programId, ProgramType programType)
{
	if(programId == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(m_programId != NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	std::vector<const Parameter*> params;
	for(uint32_t i=0; i<programId->getParameterCount(); i++){
		params.push_back(programId->getParameter(i));
	}

	m_programId = programId;
	m_numParameters = 0;
	m_programType = programType;
	for(uint32_t i=0; i<params.size(); i++){
		const Parameter* param = params.at(i);
		if(param->isUseDefaultBuf() || (param->getCategory() == kParameterCategorySampler)){
			m_numParameters++;
		}
	}

	m_parameters = new EffectParameter[m_numParameters];

	uint32_t index = 0;
	for(uint32_t i=0; i<params.size(); i++){
		const Parameter* param = params.at(i);
		if(param->isUseDefaultBuf() || (param->getCategory() == kParameterCategorySampler)){
			m_parameters[index].initialize(param, programType);
			index++;
			if(index > m_numParameters){
				SCE_SAMPLE_UTIL_ASSERT(0);
			}
		}
	}

	return 0;
}

uint32_t Effect::EffectShader::getNumParams(void) const
{
	return m_numParameters;
}

int Effect::EffectShader::getParamIndex(std::string name) const
{
	for(uint32_t i=0; i<m_numParameters; i++){
		if(strcmp(m_parameters[i].getParameter()->getName(), name.c_str()) == 0){
			return i;
		}
	}
	return -1;
}

const Effect::EffectParameter *Effect::EffectShader::getParamByIndex(uint32_t index) const
{
	if(index >= m_numParameters){
		return NULL;
	}
	return &m_parameters[index];
}

const Effect::EffectParameter *Effect::EffectShader::getParamByName(std::string name) const
{
	int index = getParamIndex(name);
	if(index< 0){
		return NULL;
	}
	return getParamByIndex(index);
}


Effect::EffectParameter *Effect::EffectShader::getParamByIndex(uint32_t index) 
{
	if(index >= m_numParameters){
		return NULL;
	}
	return &m_parameters[index];
}

Effect::EffectParameter *Effect::EffectShader::getParamByName(std::string name) 
{
	int index = getParamIndex(name);
	if(index< 0){
		return NULL;
	}
	return getParamByIndex(index);
}


int Effect::EffectShader::finalize(void)
{
	if(m_parameters != NULL){
		delete [] m_parameters;
		m_parameters = NULL;
	}
	m_programId = NULL;
	m_numParameters = 0;
	return SCE_OK;
}

Effect::EffectShader::EffectShader(void)
{
	m_programId = NULL;
	m_numParameters = 0;
	m_parameters = NULL;
}
Effect::EffectShader::~EffectShader(void)
{
	finalize();
}

int Effect::EffectShader::copyFrom(const EffectShader *from)
{
	if(from == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	finalize();
	m_programId = from->m_programId;
	m_programType = from->m_programType;
	m_numParameters = from->m_numParameters;
	m_parameters = new EffectParameter[from->m_numParameters];
	for(uint32_t i=0; i<m_numParameters; i++){
		m_parameters[i].copyFrom(&from->m_parameters[i]);
	}
	return SCE_OK;


}

#pragma endregion // EffectShader 

/* ------------------------------------------------------------------------------------ */

#pragma region EffectData

int Effect::EffectData::initialize(
			   const Program* vertexProgram, 
			   const Program* fragmentProgram, 
			   BlendInfo *blendInfo)
{
	int ret;
	if(vertexProgram==NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(fragmentProgram==NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ret = m_vertexShader.initialize(vertexProgram, kProgramTypeVertex);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	ret = m_fragmentShader.initialize(fragmentProgram, kProgramTypeFragment);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	if(blendInfo){
		m_blendInfo = *blendInfo;
	}else{
		m_blendInfo.initializeAsNoBlend();
	}


	m_numAttributeParams = 0;

	std::vector<const Parameter*> params;
	for(uint32_t i=0; i<vertexProgram->getParameterCount(); i++){
		params.push_back(vertexProgram->getParameter(i));
	}
	for(uint32_t i=0; i<params.size(); i++){
		const Parameter *param = params.at(i);
		if(param->getCategory() == kParameterCategoryAttribute){	
			m_numAttributeParams++;
		}
	}

	m_attributeParams = new const Parameter*[m_numAttributeParams];
	int index=0;
	for(uint32_t i=0; i<params.size(); i++){
		const Parameter *param = params.at(i);
		if(param->getCategory() == kParameterCategoryAttribute){	
			m_attributeParams[index] = param; 
			index++;	
		}
	}

	(void)ret;
	return SCE_OK;
}

int Effect::EffectData::finalize(void)
{
	int ret;
	ret = m_vertexShader.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	ret = m_fragmentShader.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	if(m_attributeParams != NULL){
		delete [] m_attributeParams;
		m_attributeParams = NULL;
	}
	m_numAttributeParams = 0;
	(void)ret;
	return SCE_OK;
}

int Effect::EffectData::copyFrom(const EffectData *from)
{
	if(from==NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	finalize();
	m_vertexShader.copyFrom(&from->m_vertexShader);
	m_fragmentShader.copyFrom(&from->m_fragmentShader);
	m_blendInfo = from->m_blendInfo;
	m_numAttributeParams = from->m_numAttributeParams;
	m_attributeParams = new const Parameter*[m_numAttributeParams];
	for(uint32_t i=0; i<m_numAttributeParams; i++){
		m_attributeParams[i] = from->m_attributeParams[i];
	}
	return SCE_OK;

}

Effect::EffectData::EffectData(void)
{
	m_numAttributeParams = 0;
	m_attributeParams = NULL;

}
Effect::EffectData::~EffectData(void)
{
	finalize();
}

#pragma endregion  //EffectData




/* ------------------------------------------------------------------------------------ */

#pragma region  EffectInstance

void Effect::EffectInstance::ValueRef::setToDefaultUniformBuffer(GraphicsContext *context, void *buffer)
{
	const Parameter *param = m_param->getParameter();
	if(param->isUseDefaultBuf()){
		if(m_value->getCategory() == kParameterCategoryUniform){
			param->setUniformDataF(buffer, (const float*)m_value->getData() );
		}
	}
	if(param->getCategory() == kParameterCategorySampler){
		if(m_value->getCategory() == kParameterCategorySampler){
			context->setTexture(m_param->getProgramType(), m_value->getTexture(), param->getResourceIndex());
		}
	}
}


Effect::EffectInstance::EffectInstance(void)
{
	m_graphicsLoader = NULL;
	m_vertexProgram = NULL;
	m_fragmentProgram = NULL;

	m_effect = NULL;

	m_vertexValueRefs = NULL;
	m_fragmentValueRefs = NULL;
}


Effect::EffectInstance::~EffectInstance(void)
{
	finalize();
}

int Effect::EffectInstance::initialize(GraphicsLoader *graphicsLoader, 
									   const sce::SampleUtil::Graphics::Effect::EffectData *effect,
									   const VertexAttribute *attrs, 
									   uint32_t numAttrs,

									   const VertexStream *streams, 
									   uint32_t numStreams)
{
	if(graphicsLoader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(effect == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(attrs == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(streams == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(m_effect != NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	m_graphicsLoader = graphicsLoader;

	m_effect = effect;

	int ret = graphicsLoader->createVertexProgram(&m_vertexProgram, 
		m_effect->m_vertexShader.getProgramId(),
		attrs,
		numAttrs,
		streams, 
		numStreams);
	if(ret != SCE_OK){
		printf(__FILE__"(%d) Error %#x\n", __LINE__, ret);
	}
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	ret = graphicsLoader->createFragmentProgram(&m_fragmentProgram,
		m_effect->m_fragmentShader.getProgramId(), 
		m_effect->m_vertexShader.getProgramId(), 
		&m_effect->m_blendInfo
/*
		,
		SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4
		, SCE_GXM_MULTISAMPLE_4X*/);
	if(ret != SCE_OK){
		printf(__FILE__"(%d) Error %#x\n", __LINE__, ret);
	}
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	m_vertexValueRefs = new ValueRef[m_effect->m_vertexShader.getNumParams()];
	for(uint32_t i=0; i<m_effect->m_vertexShader.getNumParams(); i++){
		const EffectParameter *bu = m_effect->m_vertexShader.getParamByIndex(i);
		m_vertexValueRefs[i].m_param = bu;//m_vertexProgram.m_programId.findParameterByName(bu->getParameter().getName());
		m_vertexValueRefs[i].m_value = bu->getValue();
	}

	m_fragmentValueRefs = new ValueRef[m_effect->m_fragmentShader.getNumParams()];
	for(uint32_t i=0; i<m_effect->m_fragmentShader.getNumParams(); i++){
		const EffectParameter *bu = m_effect->m_fragmentShader.getParamByIndex(i);
		m_fragmentValueRefs[i].m_param = bu;//m_fragmentProgram.m_programId.findParameterByName(bu->getParameter().getName());
		m_fragmentValueRefs[i].m_value = bu->getValue();
	}



	return 0;
}

int Effect::EffectInstance::finalize(void)
{
	int ret;
	if(m_fragmentProgram != NULL){
		ret = sce::SampleUtil::destroy( m_fragmentProgram );
		SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
		m_fragmentProgram = NULL;
	}
	if(m_vertexProgram != NULL){
		ret = sce::SampleUtil::destroy( m_vertexProgram );
		SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
		m_vertexProgram = NULL;
	}
	if(m_vertexValueRefs != NULL){
		delete [] m_vertexValueRefs;
		m_vertexValueRefs = NULL;
	}
	if(m_fragmentValueRefs != NULL){
		delete [] m_fragmentValueRefs;
		m_fragmentValueRefs = NULL;
	}
	m_effect = NULL;
	m_graphicsLoader = NULL;
	(void)ret;
	return SCE_OK;
}

int Effect::EffectInstance::setParamValueReferenceByName(ProgramType programType, const char* name, const ParameterValue *value)
{
	if(name == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(value == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if(programType == kProgramTypeVertex){
		for(uint32_t i=0; i<m_effect->m_vertexShader.getNumParams(); i++){
			if(strcmp(m_vertexValueRefs[i].m_param->getParameter()->getName(), name) == 0){
				m_vertexValueRefs[i].m_value = value;

				return SCE_OK;
			}
		}
	}else{
		for(uint32_t i=0; i<m_effect->m_fragmentShader.getNumParams(); i++){
			if(strcmp(m_fragmentValueRefs[i].m_param->getParameter()->getName(), name) == 0){
				m_fragmentValueRefs[i].m_value = value;

				return SCE_OK;
			}
		}
	}
	return -1;
	
}

int Effect::EffectInstance::apply(GraphicsContext *context,
						   VertexBuffer *const *vertexBuffer, uint32_t numVertexBuffers
						   )
{
	if(context == NULL || vertexBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	for(uint32_t i=0; i<numVertexBuffers; i++){
		context->setVertexBuffer(vertexBuffer[i], i);
	}
	context->setVertexProgram( m_vertexProgram );
	context->setFragmentProgram( m_fragmentProgram );

	void *vertexDefaultBuffer = context->reserveDefaultUniformBuffer(kProgramTypeVertex);
	void *fragmentDefaultBuffer = context->reserveDefaultUniformBuffer(kProgramTypeFragment);

	for(uint32_t i=0; i<m_effect->m_vertexShader.getNumParams(); i++){
		m_vertexValueRefs[i].setToDefaultUniformBuffer(context, vertexDefaultBuffer);
	}
	for(uint32_t i=0; i<m_effect->m_fragmentShader.getNumParams(); i++){
		m_fragmentValueRefs[i].setToDefaultUniformBuffer(context, fragmentDefaultBuffer);
	}

	return SCE_OK;
}



void Effect::EffectInstance::cloneFrom(const EffectInstance &from)
{
	this->m_graphicsLoader = from.m_graphicsLoader;
	m_graphicsLoader->cloneVertexProgram(&this->m_vertexProgram, from.m_vertexProgram);
	m_graphicsLoader->cloneFragmentProgram(&this->m_fragmentProgram, from.m_fragmentProgram);

	this->m_effect = from.m_effect;

	this->m_vertexValueRefs = new ValueRef[m_effect->m_vertexShader.getNumParams()];
	for(uint32_t i=0; i<m_effect->m_vertexShader.getNumParams(); i++){	
		this->m_vertexValueRefs[i] = from.m_vertexValueRefs[i];
	}

	this->m_fragmentValueRefs  = new ValueRef[m_effect->m_fragmentShader.getNumParams()];
	for(uint32_t i=0; i<m_effect->m_fragmentShader.getNumParams(); i++){
		this->m_fragmentValueRefs[i] = from.m_fragmentValueRefs[i];
	}
}


#pragma endregion // EffectInstance