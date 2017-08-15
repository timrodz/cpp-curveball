/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _COLLADA_RENDER_UTIL_DAE_CONTROL_H
#define _COLLADA_RENDER_UTIL_DAE_CONTROL_H


#include <graphics/collada/dae/dae_common.h>
#include "dae_data_flow.h"
#include "dae_effect.h"

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
		namespace Controller
		{
			struct Joints : public Dae::Common::IInputContainer
			{
				std::vector<Dae::DataFlow::Input*> m_inputs;

				void dispose(void)
				{
					for(unsigned int i=0; i<m_inputs.size(); i++){
						delete m_inputs.at(i);
					}
					m_inputs.clear();
				}

				virtual ~Joints(void)
					{
						dispose();
					}

				virtual const std::vector<Dae::DataFlow::Input*> & getInputs(void) const { return m_inputs; }
				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);
			};


			struct VertexWeights : public Dae::Common::IInputContainer
			{
				int m_count;
				std::vector<Dae::DataFlow::Input*> m_inputs;
				int *m_vcount;
				int *m_v;

				virtual const std::vector<Dae::DataFlow::Input*> &getInputs(void) const { return m_inputs; }

				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

				VertexWeights(void)
				{
					m_count = 0;
					m_vcount = NULL;
					m_v = NULL;
				}

				void dispose(void)
				{
					for(unsigned int i=0; i<m_inputs.size(); i++){
						delete m_inputs.at(i);
					}
					m_inputs.clear();
					if(m_vcount != NULL){
						delete [] m_vcount;
						m_vcount = NULL;
					}
					if(m_v != NULL){
						delete [] m_v;
						m_v = NULL;
					}
				}

				virtual ~VertexWeights(void)
					{
						dispose();
					}
			};

			struct Skin
			{
				std::string          m_source;
				Matrix4              m_bindShapeMatrix;
				std::vector<Dae::DataFlow::Source*> m_sources;
				Joints*              m_joints;
				VertexWeights*       m_vertexWeights;

				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

				Dae::DataFlow::Source *findSource(std::string id) const
					{
						for(unsigned int i=0; i<m_sources.size(); i++){
							Dae::DataFlow::Source *s = m_sources.at(i);
							if(s->m_id == id){
								return s;
							}
						}
						return NULL;
					}
				void dispose(void)
				{
					delete m_joints;
					delete m_vertexWeights;
					for(unsigned int i=0; i<m_sources.size(); i++){
						delete m_sources.at(i);
					}
					m_sources.clear();
					m_source = "";
				}
				virtual ~Skin(void)
					{
						dispose();
					}
			};

			struct Controller
			{
				std::string m_id;
				Skin *m_skin;
				Controller(void) : m_skin(NULL)
					{
					}

				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem); 

				void dispose(void)
				{
					m_id = "";

					if(m_skin != NULL){
						delete m_skin;
						m_skin = NULL;
					}
				}

				virtual ~Controller(void)
					{
						dispose();
					}
			};

			struct LibraryControllers
			{
				std::vector<Controller*> m_controllers;

				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

				void dispose(void)
				{
					for(unsigned int i=0; i<m_controllers.size(); i++){
						delete m_controllers.at(i);
					}
					m_controllers.clear();
				}

				virtual ~LibraryControllers(void)
					{
						dispose();
					}

			};


			struct InstanceController
			{
				std::string m_url;
				std::vector<std::string> m_skeletons;
				Dae::effect::BindMaterial *m_bindMaterial;

				InstanceController(void) : m_bindMaterial(NULL)
				{}

				void dispose(void)
				{
					m_url = "";
					m_skeletons.clear();
					if(m_bindMaterial != NULL){
						delete m_bindMaterial;
						m_bindMaterial = NULL;
					}
				}

				virtual ~InstanceController(void)
				{
					if(m_bindMaterial != NULL){
						delete m_bindMaterial;
						m_bindMaterial = NULL;
					}
				}


				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					m_url = elem->Attribute("url");

					for(TiXmlElement* celem = elem->FirstChildElement("skeleton");
						celem != NULL;
						celem = celem->NextSiblingElement("skeleton"))
					{
						std::string s = celem->GetText();
						m_skeletons.push_back(s);
					}

					m_bindMaterial = NULL;
					if(elem->FirstChildElement("bind_material") != NULL){
						m_bindMaterial = new Dae::effect::BindMaterial;
						m_bindMaterial->parse(loaderConfig, elem->FirstChildElement("bind_material") );
					}

				}

				Dae::Common::InstanceMaterial* findInstanceMaterialBySymbol(std::string symbol)
				{
					return m_bindMaterial->m_techniqueCommon->findInstanceMaterialBySymbol(symbol);
				}
			};

		}
	}
}
}
}
}

#endif /* _COLLADA_LOADER_UTIL_DAE_CONTROL_H */
