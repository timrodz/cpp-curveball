/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _COLLADA_RENDER_UTIL_DAE_ANIMATION_H
#define _COLLADA_RENDER_UTIL_DAE_ANIMATION_H

#include <graphics/collada/dae/dae_common.h>
#include "dae_data_flow.h"

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
	namespace Collada
	{
		namespace Dae
		{
			namespace Animation
			{
				struct Sampler : public Dae::Common::IInputContainer
				{
					std::string m_id;
					std::vector<Dae::DataFlow::Input*> m_inputs;
					virtual const std::vector<Dae::DataFlow::Input*> &getInputs(void) const { return m_inputs; }
					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

					void dispose(void)
					{
						for(unsigned int i=0; i<m_inputs.size(); i++){
							delete m_inputs.at(i);
						}
						m_inputs.clear();
						m_id = "";
					}

					virtual ~Sampler(void)
					{
						dispose();
					}

				};

				struct Channel
				{
					std::string m_source;
					std::string m_target;

					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
					{
						m_source = elem->Attribute("source");
						m_target = elem->Attribute("target");
						return SCE_OK;
					}
					void dispose(void)
					{
						m_source = "";
						m_target = "";
					}
					~Channel(void)
					{
						dispose();
					}
				};

				struct Animation : public Dae::Common::ISourceContainer
				{
					std::string m_id;
					std::vector<Dae::DataFlow::Source*> m_sources;
					std::vector<Sampler*> m_samplers;
					std::vector<Channel*> m_channels;
					virtual const std::vector<Dae::DataFlow::Source*> &getSources(void) const { return m_sources; }

					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

					void dispose(void)
					{
						m_id = "";
						for(unsigned int i=0; i<m_sources.size(); i++){
							delete m_sources.at(i);
						}
						for(unsigned int i=0; i<m_samplers.size(); i++){
							delete m_samplers.at(i);
						}
						for(unsigned int i=0; i<m_channels.size(); i++){
							delete m_channels.at(i);
						}
					}

					virtual ~Animation(void)
					{
						dispose();
					}


					Sampler *findSampler(std::string sampler_id)
					{
						for(unsigned int i=0; i<m_samplers.size(); i++){
							if(m_samplers.at(i)->m_id == sampler_id){
								return m_samplers.at(i);
							}
						}
						return NULL;
					}

				};

				struct LibraryAnimations
				{
					std::vector<Animation*> m_animations;

					void dispose(void)
					{
						for(unsigned int i=0; i<m_animations.size(); i++){
							delete m_animations.at(i);
						}
						m_animations.clear();
					}

					virtual ~LibraryAnimations(void)
					{
						dispose();
					}

					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

				};

			}

		}
	}
}
}
}


#endif /* _COLLADA_RENDER_UTIL_DAE_ANIMATION_H */