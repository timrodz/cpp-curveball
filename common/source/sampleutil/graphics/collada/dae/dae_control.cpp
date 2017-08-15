/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <graphics/collada/dae/dae_control.h> 
#include "dae_data_flow.h"
#include <scebase_common.h>

using namespace sce::SampleUtil::Graphics::Collada;
using namespace sce::SampleUtil::Graphics::Collada::Dae;
using namespace sce::SampleUtil::Graphics::Collada::Dae::Controller;



int Joints::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	for(TiXmlElement* celem= elem->FirstChildElement("input"); 
		celem != NULL;
		celem = celem->NextSiblingElement("input"))
	{
		Dae::DataFlow::Input *i = new Dae::DataFlow::Input;
		ret = i->parse(loaderConfig, celem);
		if(ret == SCE_OK){
			m_inputs.push_back(i);
		}else{
			delete i;
			if(ret == SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
				dispose();
				return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
			}
		}
	}
	return SCE_OK;
}


int VertexWeights::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;

	ret = elem->QueryIntAttribute("count", &m_count);
	if(ret != TIXML_SUCCESS){
		ret = fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

	for(TiXmlElement* celem= elem->FirstChildElement("input"); 
		celem != NULL;
		celem = celem->NextSiblingElement("input"))
	{
		Dae::DataFlow::Input *i = new Dae::DataFlow::Input;
		ret = i->parse(loaderConfig, celem);
		if(ret == SCE_OK){
			m_inputs.push_back(i);
		}else{
			delete i;
			if(ret == SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
				dispose();
				return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
			}
		}
	}

	m_vcount = new int[m_count];
	ret = Internal::Util::parseIntArray(m_vcount, m_count, elem->FirstChildElement("vcount")->GetText());
	if(ret != SCE_OK){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return ret;
	}

	int sum_vcount=0;
	for(int i=0; i<m_count; i++){
		sum_vcount += m_vcount[i];
	}

	int num_v = sum_vcount * m_inputs.size();
	if(num_v > 0){
		m_v = new int[num_v];
		ret = Internal::Util::parseIntArray(m_v, num_v, elem->FirstChildElement("v")->GetText());
		if(ret != SCE_OK){
			fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
			dispose();
			return ret;
		}
	}else{
		m_v = NULL;
	}
	return SCE_OK;
}


int Skin::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	if(elem->Attribute("source") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}
	m_source = elem->Attribute("source");

	if(elem->FirstChildElement("bind_shape_matrix") != NULL){
		float ary[16];
		ret = Internal::Util::parseFloatArray(ary, 16, elem->FirstChildElement("bind_shape_matrix")->GetText());
		if(ret != 0){
			fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
			dispose();
			return -1;
		}
		m_bindShapeMatrix = Internal::Util::fromFloatArray(ary);
	}else{
		m_bindShapeMatrix = Matrix4::identity();
	}

	for(TiXmlElement* celem=elem->FirstChildElement("source");
		celem != NULL;
		celem = celem->NextSiblingElement("source"))
	{
		Dae::DataFlow::Source *s = new Dae::DataFlow::Source;
		ret = s->parse(loaderConfig, celem);
		if(ret == SCE_OK){
			m_sources.push_back(s);
		}else{
			delete s;
			if(ret == SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
				dispose();
				return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
			}
		}
	}
	if(m_sources.size() < 3){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}

	if(elem->FirstChildElement("joints") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}

	m_joints = new Joints();
	m_joints->parse(loaderConfig, elem->FirstChildElement("joints"));

	if(elem->FirstChildElement("vertex_weights") == NULL){
		fprintf(stderr, "Parse error: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}
	m_vertexWeights = new VertexWeights;
	m_vertexWeights->parse(loaderConfig, elem->FirstChildElement("vertex_weights"));
	return SCE_OK;
}




int Dae::Controller::Controller::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	SCE_SAMPLE_UTIL_ASSERT(elem != NULL);
	m_id = elem->Attribute("id");

	TiXmlElement* skinElem = elem->FirstChildElement("skin");
	if(skinElem == NULL){
		fprintf(stderr, "Non-skin controller is not supported: %s\n", Internal::Util::getXmlContext(elem).c_str());
		dispose();
		return -1;
	}

	m_skin = new Skin;
	ret = m_skin->parse(loaderConfig, skinElem);
	if(ret != 0){
		dispose();
		return ret;
	}

	return SCE_OK;
}


int Dae::Controller::LibraryControllers::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	int ret;
	if(elem == NULL){
		return 0;
	}
	for(TiXmlElement* celem=elem->FirstChildElement("controller");
		celem != NULL;
		celem = celem->NextSiblingElement("controller"))
	{
		Controller* controller = new Controller;
		ret = controller->parse(loaderConfig, celem);

		if(ret == SCE_OK){
			m_controllers.push_back(controller);
		}else{
			delete controller;
			if(ret == SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL){
				dispose();
				return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_FATAL;
			}
		}
	}
	return SCE_OK;
}



