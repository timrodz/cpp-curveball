/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _COLLADA_RENDER_UTIL_DAE_SCENE_H
#define _COLLADA_RENDER_UTIL_DAE_SCENE_H

#include "graphics/collada/collada_transform.h"
#include <graphics/collada/dae/dae_common.h>
#include "dae_geometry.h"
#include "dae_control.h"

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
		namespace Scene
		{


			struct Node
			{
				std::string m_id;
				std::string m_sid;
				std::string m_type;
				std::string m_name;

				std::vector<sce::SampleUtil::Graphics::Collada::Internal::TransformationElement*> m_matrices;
				std::vector<Node*> m_nodes;
				std::vector<Dae::Controller::InstanceController*> m_instanceController;
				std::vector<Dae::Geometry::InstanceGeometry*> m_instanceGeometry;

				const Node *findNodeById(std::string id) const
				{
					if(m_id == id){
						return this;
					}
					for(unsigned int i=0; i<m_nodes.size(); i++){
						const Node *ret = m_nodes.at(i)->findNodeById(id);
						if(ret != NULL){
							return ret;
						}
					}
					return NULL;
				}

				void dispose(void);

				~Node(void)
				{
					dispose();
				}

				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

			};

			static inline Dae::Scene::Node* findJointNode(Dae::Scene::Node* node)
			{
				if(node->m_type == "JOINT"){
					return node;
				}
				for(unsigned int i=0; i<node->m_nodes.size(); i++){
					Dae::Scene::Node* ret= findJointNode(node->m_nodes.at(i));
					if(ret != NULL){
						return ret;
					}
				}
				return NULL;
			}



			struct VisualScene
			{
				std::string m_id;
				std::string m_name;
				Node m_rootNode;
				//std::vector<Node*> m_nodes;
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					m_id = elem->Attribute("id");
					m_name = elem->Attribute("name");
					for(TiXmlElement* celem=elem->FirstChildElement("node");
						celem != NULL;
						celem = celem->NextSiblingElement("node"))
					{
						Node *n = new Node;
						n->parse(loaderConfig, celem);
						m_rootNode.m_nodes.push_back(n);
					}

				}

				const Node *findNodeById(std::string id) const
				{
					return m_rootNode.findNodeById(id);
/*
					for(unsigned int i=0; m_nodes.size(); i++){
						const Node *ret = m_nodes.at(i)->findNodeById(id);
						if(ret != NULL){
							return ret;
						}
					}
					return NULL;
*/
				}

			};

			struct LibraryVisualScenes
			{
				std::vector<VisualScene*> m_visualScenes;

				virtual ~LibraryVisualScenes(void)
				{
					for(unsigned int i=0; i<m_visualScenes.size(); i++){
						delete m_visualScenes.at(i);
					}
				}

				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					for(TiXmlElement* celem=elem->FirstChildElement("visual_scene");
						celem != NULL;
						celem = celem->NextSiblingElement("visual_scene"))
					{
						VisualScene *visualScene = new VisualScene;
						visualScene->parse(loaderConfig, celem);
						m_visualScenes.push_back(visualScene);
					}
				}
			};

		};

	}
}
}
}
}

#endif /* _COLLADA_RENDER_UTIL_DAE_SCENE_H */

