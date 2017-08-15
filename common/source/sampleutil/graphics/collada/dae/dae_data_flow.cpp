/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include "dae_common.h"
#include "dae_data_flow.h"
#include <errno.h>


using namespace sce::SampleUtil::Graphics::Collada;
using namespace sce::SampleUtil::Graphics::Collada::Dae::DataFlow;



int FloatArray::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	if(elem->Attribute("id") != NULL){
		m_id = elem->Attribute("id");
	}else{
		m_id = "";
	}

	int count;
	ret = elem->QueryIntAttribute("count", &count);
	m_count = count;
	if(ret != TIXML_SUCCESS){
		ret = fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

	if(elem->GetText() == NULL){
		ret = fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

#if 0
	std::string dataString ( (char*)elem->GetText());
	std::vector<std::string> tokens;
	Util::tokenize(tokens, dataString.c_str());

	if(m_count != tokens.size()){
		ret = loaderConfig->onError(SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_PARSE, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

	m_data = new float[m_count];
	for(uint32_t i=0; i<m_count; i++){
		std::string floatStr = tokens.at(i);
		errno = 0;
		char *e;
		m_data[i] = (float)strtod(floatStr.c_str(), &e);
		if ((errno != 0) || (m_data[i] == HUGE_VAL)) {
			ret = loaderConfig->onError(SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_PARSE, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
			dispose();
			return ret;
		}
		//m_data[i] = (float)atof(floatStr.c_str());
	}
#else
	m_data = new float[m_count];
	const char *str = elem->GetText();

	for(uint32_t i=0; i<m_count; i++){
		while(str[0] == ' '){ 
			str++;
		}
		char *e;
		m_data[i] = (float)strtod(str, &e);
		str = e;
	}


#endif

	return 0;
}



int Param::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	//int ret;
	SCE_SAMPLE_UTIL_ASSERT(elem != NULL);
	const char* c_name = elem->Attribute("name");
	m_name = c_name != NULL ? c_name : "";

	const char* c_sid = elem->Attribute("sid");
	m_sid = c_sid != NULL ? c_sid : "";

	const char* n_type = elem->Attribute("type");
	if(n_type == NULL){
		//ret = loaderConfig->onError(SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_PARSE, "Parse error: %s has no \"type\" attribute.\n", getXmlContext(elem).c_str());
		//dispose();
		//return ret;
	}else{
		m_type = elem->Attribute("type");
	}

	const char* c_semantic = elem->Attribute("semantic");
	m_semantic = c_semantic != NULL ? c_semantic : "";

	if(elem->Attribute("ref") != NULL){
		m_ref = elem->Attribute("ref");
	}

	return SCE_OK;
}


int Accessor::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	SCE_SAMPLE_UTIL_ASSERT(elem != NULL);

	ret = elem->QueryIntAttribute("count", &m_count);
	if(ret != TIXML_SUCCESS){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

	if(elem->Attribute("source") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}
	m_source = elem->Attribute("source");

	ret = elem->QueryIntAttribute("stride", &m_stride);
	if(ret != TIXML_SUCCESS){
		m_stride = 1;
	}

	for(TiXmlElement* celem=elem->FirstChildElement("param"); 
		celem != NULL;
		celem = celem->NextSiblingElement("param"))
	{
		Dae::DataFlow::Param *p = new Dae::DataFlow::Param;
		int ret2 = p->parse(loaderConfig, celem);
		if(ret2 == SCE_OK){
			params.push_back(p);
		}else{
			delete p;
			if(ret2 == SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
				dispose();
				return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
			}
		}
	}
	return 0;
}




int NameArray::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	if(elem->Attribute("id") != NULL){
		m_id = elem->Attribute("id");
	}else{
		m_id = "";
	}

	ret = elem->QueryIntAttribute("count", &m_count);
	if(ret != TIXML_SUCCESS){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

	if(elem->GetText() == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

	char *dataString = (char*)elem->GetText();
#if 0
	std::vector<std::string> tokens;

	Util::tokenize(tokens, dataString);

	if(m_count != tokens.size()){
		ret = loaderConfig->onError(SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_PARSE, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

	m_data = new std::string[m_count];
	for(int i=0; i<m_count; i++){
		m_data[i] = tokens.at(i);
	}
#else
	m_data = new std::string[m_count];
	std::string delimiters = " ";
	std::string str = dataString;
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	for(int i=0; i<m_count; i++){
		m_data[i]  = str.substr(lastPos, pos - lastPos);
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}

#endif
	return 0;

}



int TechniqueCommonOfSource::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	m_accessor = NULL;
	if(elem->FirstChildElement("accessor") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}
	m_accessor = new Dae::DataFlow::Accessor();
	ret = m_accessor->parse(loaderConfig, elem->FirstChildElement("accessor"));
	if(ret != SCE_OK){
		dispose();
		if(ret ==  SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
			return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
		}
	}
	return SCE_OK;
}




int Source::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig,TiXmlElement* elem)
{
	int ret;
	if(elem->Attribute("id") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}
	m_id = elem->Attribute("id");

	m_name = (elem->Attribute("name") == NULL) ? "" : elem->Attribute("name");

	TiXmlElement* floatArrayElem = elem->FirstChildElement("float_array");
	if(floatArrayElem != NULL){
		if(m_arrayElementType != ARRAY_ELEMENT_TYPE_NONE){
			fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
			dispose();
			return -1;
		}
		m_floatArray = new Dae::DataFlow::FloatArray();
		ret = m_floatArray->parse(loaderConfig, floatArrayElem);
		if(ret != SCE_OK){
			dispose();
			if(ret ==  SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
				return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
			}
		}
		m_arrayElementType = ARRAY_ELEMENT_TYPE_FLOAT;
	}

	bool isNameArray = true;
	TiXmlElement* nameArrayElem = elem->FirstChildElement("Name_array");
	if(nameArrayElem  == NULL){
		isNameArray = false;
		nameArrayElem = elem->FirstChildElement("IDREF_array");
	}
	if(nameArrayElem != NULL){
		if(m_arrayElementType != ARRAY_ELEMENT_TYPE_NONE){
			fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
			dispose();
			return -1;
		}
		m_nameArray = new Dae::DataFlow::NameArray(isNameArray ? Dae::DataFlow::NameArray::TYPE_NAME : Dae::DataFlow::NameArray::TYPE_IDREF);
		ret = m_nameArray->parse(loaderConfig, nameArrayElem);
		if(ret != SCE_OK){
			dispose();
			if(ret ==  SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
				return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
			}
		}
		m_arrayElementType = ARRAY_ELEMENT_TYPE_NAME;
	}

	m_techniqueCommon = new TechniqueCommonOfSource();
	if(elem->FirstChildElement("technique_common") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}
 	ret = m_techniqueCommon->parse(loaderConfig, elem->FirstChild("technique_common")->ToElement());
	if(ret != SCE_OK){
		dispose();
		if(ret ==  SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
			return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
		}
	}

	return SCE_OK;
}


void Source::dispose(void)
{
	m_id = "";
	m_name= "";
	if(m_techniqueCommon != NULL){
		delete m_techniqueCommon;
	}

	switch(m_arrayElementType)
	{
	case ARRAY_ELEMENT_TYPE_NONE:
		{
			break;
		}
	case ARRAY_ELEMENT_TYPE_FLOAT: 
		{
			SCE_SAMPLE_UTIL_ASSERT(m_floatArray != NULL);
			delete m_floatArray;
			m_floatArray = NULL;
			break;
		}
	case ARRAY_ELEMENT_TYPE_NAME:
		{
			SCE_SAMPLE_UTIL_ASSERT(m_nameArray != NULL);
			delete m_nameArray;
			m_nameArray = NULL;
			break;
		}
	default:
		{
			SCE_SAMPLE_UTIL_ASSERT(0);
		}
	}

}


Dae::VertexSemantic Internal::Util::strToSemantic(std::string strSemantic)
{
	if(strSemantic == "BINORMAL"){
		return Dae::kSemanticBinormal;
	}else if(strSemantic == "COLOR"){
		return Dae::kSemanticColor;
	}else if(strSemantic == "CONTINUITY"){
		return Dae::kSemanticContinuity;
	}else if(strSemantic == "IMAGE"){
		return Dae::kSemanticImage;
	}else if(strSemantic == "INPUT"){
		return Dae::kSemanticInput;
	}else if(strSemantic == "IN_TANGENT"){
		return Dae::kSemanticInTangent;
	}else if(strSemantic == "INTERPOLATION"){
		return Dae::kSemanticInterpolation;
	}else if(strSemantic == "INV_BIND_MATRIX"){
		return Dae::kSemanticInvBindMatrix;
	}else if(strSemantic == "JOINT"){
		return Dae::kSemanticJoint;
	}else if(strSemantic == "LINEAR_STEPS"){
		return Dae::kSemanticLinearSteps;
	}else if(strSemantic == "MORPH_TARGETMORPH_WEIGHT"){
		return Dae::kSemanticMorphTargetmorphWeight;
	}else if(strSemantic == "NORMAL"){
		return Dae::kSemanticNormal;
	}else if(strSemantic == "OUTPUT"){
		return Dae::kSemanticOutput;
	}else if(strSemantic == "OUT_TANGENT"){
		return Dae::kSemanticOutTangent;
	}else if(strSemantic == "POSITION"){
		return Dae::kSemanticPosition;
	}else if(strSemantic == "TANGENT"){
		return Dae::kSemanticTangent;
	}else if(strSemantic == "TEXBINORMAL"){
		return Dae::kSemanticTexbinormal;
	}else if(strSemantic == "TEXCOORD"){
		return Dae::kSemanticTexcoord;
	}else if(strSemantic == "TEXTANGENT"){
		return Dae::kSemanticTextangent;
	}else if(strSemantic == "UV"){
		return Dae::kSemanticUv;
	}else if(strSemantic == "VERTEX"){
		return Dae::kSemanticVertex;
	}else if(strSemantic == "WEIGHT"){
		return Dae::kSemanticWeight;
	}else{
		return Dae::kSemanticInvalid;
	}
}



int Input::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	m_offset = 0;
	if(elem->Attribute("offset") != NULL){
		int offset;
		ret = elem->QueryIntAttribute("offset", &offset);
		m_offset = offset;
		if(ret != TIXML_SUCCESS){
			fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
			dispose();
			return ret;
		}
	}
	if(elem->Attribute("semantic") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}
	m_semanticStr = std::string( elem->Attribute("semantic") );
	m_semantic = Internal::Util::strToSemantic(m_semanticStr);
	if(m_semantic == Dae::kSemanticInvalid){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		printf("m_semanticStr=%s\n", m_semanticStr.c_str());
		SCE_SAMPLE_UTIL_ASSERT(0);
		dispose();
		return -1;
	}

	if(elem->Attribute("source") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}
	m_source = std::string( elem->Attribute("source") );

	m_set = 0;
	if(elem->Attribute("set") != NULL){
		ret = elem->QueryIntAttribute("set", &m_set);
		if(ret != TIXML_SUCCESS){
			fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
			dispose();
			return ret;
		}
	}
	return SCE_OK;
} 
