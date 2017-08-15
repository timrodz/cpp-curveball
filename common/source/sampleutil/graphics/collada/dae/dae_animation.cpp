/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */
#include "stdafx.h"


#include <graphics/collada/dae/dae_animation.h>

using namespace sce::SampleUtil::Graphics::Collada;

int Dae::Animation::Sampler::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
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


int Dae::Animation::Animation::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					m_id = elem->Attribute("id");
					//printf("Parsing animation : %s\n", m_id.c_str());

					for(TiXmlElement* celem=elem->FirstChildElement("source");
						celem != NULL;
						celem = celem->NextSiblingElement("source"))
					{
						Dae::DataFlow::Source *s = new Dae::DataFlow::Source;
						s->parse(loaderConfig, celem);
						m_sources.push_back(s);
					}

					for(TiXmlElement* celem=elem->FirstChildElement("sampler");
						celem != NULL;
						celem = celem->NextSiblingElement("sampler"))
					{
						Sampler *s = new Sampler;
						s->parse(loaderConfig, celem);
						m_samplers.push_back(s);
					}

					for(TiXmlElement* celem=elem->FirstChildElement("channel");
						celem != NULL;
						celem = celem->NextSiblingElement("channel"))
					{
						Channel *c = new Channel;
						c->parse(loaderConfig, celem);
						m_channels.push_back(c);
					}
					return SCE_OK;
				}


int Dae::Animation::LibraryAnimations::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					if(elem == NULL){
						return 0;
					}
					for(TiXmlElement* celem=elem->FirstChildElement("animation");
						celem != NULL;
						celem = celem->NextSiblingElement("animation"))
					{
						Animation *a = new Animation;
						a->parse(loaderConfig, celem);
						m_animations.push_back(a);
					}
					return SCE_OK;
				}
