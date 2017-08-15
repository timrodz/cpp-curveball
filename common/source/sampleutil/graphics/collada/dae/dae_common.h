/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _COLLADA_LOADER_UTIL_DAE_COMMON_H
#define _COLLADA_LOADER_UTIL_DAE_COMMON_H

#include <string>
#include <vector>

#include <tinyxml.h>
#include "dae_data_flow.h"
#include <sampleutil/graphics/collada/collada_base.h>
#include <graphics/collada/collada_util.h>
#if _SCE_TARGET_OS_ORBIS
#pragma comment(lib,"tinyxml")
#endif



using namespace sce::Vectormath::Simd::Aos;

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

		namespace Common
		{

			struct Bind
			{
				std::string m_semantic;
				std::string m_target;

				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("semantic")){
						m_semantic = elem->Attribute("semantic");
					}

					if(elem->Attribute("target")){
						m_target = elem->Attribute("target");
					}
				}
				void dispose(void)
				{
				}
				~Bind(void)
				{
					dispose();
				}

			};


			struct BindVertexInput
			{
				std::string m_semantic;
				std::string m_inputSemantic;
				int m_inputSet;
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					m_semantic = elem->Attribute("semantic");
					m_inputSemantic = elem->Attribute("input_semantic");
					if(elem->Attribute("input_set") != NULL){
						elem->QueryIntAttribute("input_set", &m_inputSet);
					}else{
						m_inputSet = -1;
					}
				}
			};


			struct InstanceMaterial
			{
				std::string m_symbol;
				std::string m_target;

				std::vector<Bind*> m_binds;
				std::vector<BindVertexInput*> m_bindVertexInputs;

				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					m_symbol = elem->Attribute("symbol");
					m_target = elem->Attribute("target");

					for(TiXmlElement* celem=elem->FirstChildElement("bind"); 
						celem != NULL;
						celem = celem->NextSiblingElement("bind"))
					{
						Bind *b = new Bind;
						b->parse(loaderConfig, celem);
						m_binds.push_back(b);
					}


					for(TiXmlElement* celem=elem->FirstChildElement("bind_vertex_input"); 
						celem != NULL;
						celem = celem->NextSiblingElement("bind_vertex_input"))
					{
						BindVertexInput *b = new BindVertexInput;
						b->parse(loaderConfig, celem);
						m_bindVertexInputs.push_back(b);
					}

				}


				BindVertexInput* findBindVertexInput(std::string semantic)
				{
					for(unsigned int i=0; i< m_bindVertexInputs.size(); i++){
						if(m_bindVertexInputs.at(i)->m_semantic == semantic){
							return m_bindVertexInputs.at(i);
						}
					}
					return NULL;
				}

				virtual ~InstanceMaterial(void)
				{
					for(unsigned int i=0; i< m_binds.size(); i++){
						delete m_binds.at(i);
					}
					m_binds.clear();
					for(unsigned int i=0; i< m_bindVertexInputs.size(); i++){
						delete m_bindVertexInputs.at(i);
					}
					m_bindVertexInputs.clear();
				}
			};

			struct TechniqueCommon 
			{
				Dae::DataFlow::Accessor* m_accessor;
				std::vector<InstanceMaterial*> m_instanceMaterials;

				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					m_accessor = NULL;
					if(elem->FirstChildElement("accessor") != NULL){
						m_accessor = new Dae::DataFlow::Accessor();
						m_accessor->parse(loaderConfig, elem->FirstChildElement("accessor"));
					}

					for(TiXmlElement* celem=elem->FirstChildElement("instance_material"); 
						celem != NULL;
						celem = celem->NextSiblingElement("instance_material"))
					{
						InstanceMaterial* im = new InstanceMaterial();
						im->parse(loaderConfig, celem);
						m_instanceMaterials.push_back(im);
					}
					return SCE_OK;
				}
				InstanceMaterial *findInstanceMaterialBySymbol(std::string symbol)
				{
					for(unsigned int i=0; i<m_instanceMaterials.size(); i++){
						if(m_instanceMaterials.at(i)->m_symbol == symbol){
							return m_instanceMaterials.at(i);
						}
					}
					return NULL;
				}
			};




			struct ISourceContainer
			{
				virtual const std::vector<Dae::DataFlow::Source*> &getSources(void) const = 0;
				Dae::DataFlow::Source* findSource(std::string id) const
				{
					const std::vector<Dae::DataFlow::Source*> &sources = getSources();
					for(unsigned int i=0; i<sources.size(); i++){
						if(sources.at(i)->m_id == id){
							return sources.at(i);
						}
					}
					return NULL;
				}

				virtual ~ISourceContainer(void)
				{}

			};




			struct IInputContainer
			{
				virtual const std::vector<Dae::DataFlow::Input*> &getInputs(void) const = 0;
				Dae::DataFlow::Input* findInput(Dae::VertexSemantic semantic) const
				{
					const std::vector<Dae::DataFlow::Input*> &inputs = getInputs();
					for(unsigned int i=0; i<inputs.size(); i++){
						if(inputs.at(i)->m_semantic == semantic){
							return inputs.at(i);
						}
					}
					return NULL;
				}

				virtual ~IInputContainer(void)
				{}

			};


		}
	}
}
}
}
}

#endif /* _DAE_COMMON_H */
