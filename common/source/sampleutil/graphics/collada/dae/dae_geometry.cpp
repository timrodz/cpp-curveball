/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"
#include "dae_geometry.h"
#include <errno.h>

 
using namespace sce::SampleUtil::Graphics::Collada;


int Dae::Geometry::Vertices::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	m_id = elem->Attribute("id");

	for(TiXmlElement* celem = elem->FirstChildElement("input");
		celem != NULL;
		celem = celem->NextSiblingElement("input"))
	{
		Dae::DataFlow::Input *i = new Dae::DataFlow::Input;
		i->parse(loaderConfig, celem);			 
		m_inputs.push_back(i);
	}
	return SCE_OK;
}



int Dae::Geometry::Triangles::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	if(elem->Attribute("material") != NULL){
		m_matrial = elem->Attribute("material");
	}
	elem->QueryIntAttribute("count", &m_count);

	uint32_t maxOffset = 0;

	for(TiXmlElement* celem = elem->FirstChildElement("input");
		celem != NULL;
		celem = celem->NextSiblingElement("input"))
	{
		Dae::DataFlow::Input *i = new Dae::DataFlow::Input;
		i->parse(loaderConfig, celem);
		m_inputs.push_back(i);
		if(i->m_offset >= maxOffset){
			maxOffset = i->m_offset;
		}
	}

	const char *p_text = elem->FirstChildElement("p")->GetText();
#if 0
	std::vector<std::string> tokens;
	Util::tokenize(tokens, p_text);
	p = new uint16_t[tokens.size()];

	//printf("@@@@@ [%s], %d, %d, count=%d, maxOffset=%d\n",  m_matrial.c_str(), tokens.size(), arrayLength, m_count, maxOffset);
		
	for(unsigned int i=0; i<tokens.size(); i++){
		p[i] = atoi(tokens.at(i).c_str());
	}
#else
	uint32_t arrayLength = 3 * (maxOffset+1) * m_count;
	p = new uint16_t[arrayLength];

	const char *str = p_text;
	for(uint32_t i=0; i<arrayLength; i++){
		while(str[0] == ' '){ 
			str++;
		}
		char *e;
		errno = 0;
		p[i] = (uint16_t)strtol(str, &e, 10);
		if (errno != 0) {
			return -1;
		}
		str = e;
	}

#endif
	return SCE_OK;

}



void Dae::Geometry::Triangles::dispose(void)
			  {
				  m_matrial = "";
				  m_count = 0;
				  if(p != NULL){
					  delete [] p;
				  }
				  p = NULL;
				  for(unsigned int input_idx=0; input_idx< m_inputs.size(); input_idx++){
					  delete m_inputs.at(input_idx);
				  }
				  m_inputs.clear();
			  }

int Dae::Geometry::Polylist::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	fprintf(stderr, "warning: polylist is not supported\n");
	return SCE_OK;
#if 0
	if(elem->Attribute("material") != NULL){
		m_matrial = elem->Attribute("material");
	}
	elem->QueryIntAttribute("count", &m_count);

	for(TiXmlElement* celem = elem->FirstChildElement("input");
		celem != NULL;
		celem = celem->NextSiblingElement("input"))
	{
		Dae::DataFlow::Input *i = new Dae::DataFlow::Input;
		i->parse(loaderConfig, celem);
		m_inputs.push_back(i);
	}

	const char *vcount_text = elem->FirstChildElement("vcount")->GetText();
	std::vector<std::string> vcount_tokens;
	Util::tokenize(vcount_tokens, vcount_text);
	printf(__FILE__"(%d) check\n", __LINE__);
	vcount = new uint16_t[vcount_tokens.size()];
	printf(__FILE__"(%d) check\n", __LINE__);
	for(unsigned int i=0; i<vcount_tokens.size(); i++){
		vcount[i] = atoi(vcount_tokens.at(i).c_str());
	}
	printf(__FILE__"(%d) check\n", __LINE__);
		  
	const char *p_text = elem->FirstChildElement("p")->GetText();
	std::vector<std::string> p_tokens;
	Util::tokenize(p_tokens, p_text);
	p = new uint16_t[p_tokens.size()];
	for(unsigned int i=0; i<p_tokens.size(); i++){
		p[i] = atoi(p_tokens.at(i).c_str());
	}
	return SCE_OK;
#endif
}


void Dae::Geometry::Polylist::dispose(void)
	{
			m_matrial = "";
			m_count = 0;
			for(unsigned int i=0; i<m_inputs.size(); i++){
				delete m_inputs.at(i);
			}
			m_inputs.clear();
			if(vcount != NULL){
				delete [] vcount;
			}
			vcount = NULL;
			if(p != NULL){
				delete [] p;
			}
			p = NULL;
		}



void Dae::Geometry::Mesh::dispose(void)
		{
			for(unsigned int i=0; i<m_sources.size(); i++){
				delete m_sources.at(i);
			}
			m_sources.clear();
			for(unsigned int i=0; i<m_verticess.size(); i++){
				delete m_verticess.at(i);
			}
			m_verticess.clear();
			for(unsigned int i=0; i<m_triangless.size(); i++){
				delete m_triangless.at(i);
			}
			m_triangless.clear();
			for(unsigned int i=0; i<m_polylists.size(); i++){
				delete m_polylists.at(i);
			}
			m_polylists.clear();
		}




int Dae::Geometry::Mesh::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	//printf(__FILE__":%d:parse mesh\n", __LINE__);
	TiXmlElement* meshElem = elem;
	for(TiXmlNode *sourceNode = meshElem->FirstChild("source");
		sourceNode != NULL;
		sourceNode = sourceNode->NextSibling("source") )
	{
		TiXmlElement* sourceElem = sourceNode->ToElement();
		Dae::DataFlow::Source *s = new Dae::DataFlow::Source();
		s->parse(loaderConfig, sourceElem);
		m_sources.push_back(s);
	}

	for(TiXmlNode *node = meshElem->FirstChild("vertices");
		node != NULL;
		node = node->NextSibling("vertices") )
	{
		TiXmlElement* elem = node->ToElement();
		Vertices *v = new Vertices();
		v->parse(loaderConfig, elem);
		m_verticess.push_back(v);
	}

	//printf(__FILE__":%d:parse mesh\n", __LINE__);
	for(TiXmlNode *node = meshElem->FirstChild("triangles");
		node != NULL;
		node = node->NextSibling("triangles") )
	{
		TiXmlElement* elem = node->ToElement();
		SCE_SAMPLE_UTIL_ASSERT(elem != NULL);
		Triangles *v = new Triangles();
		v->parse(loaderConfig, elem);
		m_triangless.push_back(v);
	}

	//  printf(__FILE__":%d:parse mesh\n", __LINE__);
	for(TiXmlNode *node = meshElem->FirstChild("polylist");
		node != NULL;
		node = node->NextSibling("polylist") )
	{
		//loaderConfig->onError(0, "warning: polylist is not supported\n");

		TiXmlElement* elem = node->ToElement();
		SCE_SAMPLE_UTIL_ASSERT(elem != NULL);

		const char *vcount_text = elem->FirstChildElement("vcount")->GetText();
		std::vector<std::string> vcount_tokens;
		Internal::Util::tokenize(vcount_tokens, vcount_text);
		bool isTrianglated = true;
		for(unsigned int i=0; i<vcount_tokens.size(); i++){
			uint16_t vc = atoi(vcount_tokens.at(i).c_str());
			if(vc != 3){
				fprintf(stderr, "warning: non-trianglated polylist is not supported\n");
				isTrianglated = false;
				break;
			}
		}
		if(!isTrianglated){
			break;
		}

		Triangles *v = new Triangles();
		v->parse(loaderConfig, elem);
		m_triangless.push_back(v);
		/*
		TiXmlElement* elem = node->ToElement();
		SCE_SAMPLE_UTIL_ASSERT(elem != NULL);
		Polylist *v = new Polylist();
		v->parse(loaderConfig, elem);
		m_polylists.push_back(v);
		*/
	}
	//printf(__FILE__":%d:parse mesh\n", __LINE__);
	return SCE_OK;
}
