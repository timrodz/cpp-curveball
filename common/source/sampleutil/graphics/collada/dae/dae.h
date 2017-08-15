/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _COLLADA_RENDER_UTIL_DAE_H
#define _COLLADA_RENDER_UTIL_DAE_H

#include <tinyxml.h>
#include <string>
#include <vector>


#include <graphics/collada/dae/dae_common.h>
#include <graphics/collada/dae/dae_animation.h>
#include <graphics/collada/dae/dae_control.h>
#include <graphics/collada/dae/dae_transform.h>
#include <graphics/collada/dae/dae_effect.h>
#include <graphics/collada/dae/dae_geometry.h>
#include <graphics/collada/dae/dae_scene.h>


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
		struct Asset
		{
			std::string m_upAxis;

			void parse(TiXmlElement* elem, const sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig)
			{
				(void)loaderConfig;
				TiXmlElement* celem = elem->FirstChildElement("up_axis");
				SCE_SAMPLE_UTIL_ASSERT(celem != NULL);
				m_upAxis = celem->GetText();
			}

			void dispose(void)
			{
				m_upAxis = "";
			}
			~Asset(void)
			{
				dispose();
			}
		};


		namespace Image
		{
			struct Image
			{
				std::string m_id;
				std::string m_name;
				std::string m_init_from;
				void parse(TiXmlElement* elem)
				{
					m_id   = elem->Attribute("id");
					const char* name = elem->Attribute("name");
					m_name = (name==NULL)?"":name;
					m_init_from = elem->FirstChildElement("init_from")->GetText();
				}
				void dispose(void)
				{
					m_id = "";
					m_name = "";
					m_init_from = "";
				}
				~Image(void)
				{
					dispose();
				}

			};

			struct LibraryImages
			{
				std::vector<Image*> m_images;

				void dispose(void)
				{
					for(unsigned int i=0;i<m_images.size();i++){
						delete m_images.at(i);
					}
					m_images.clear();
				}

				virtual ~LibraryImages(void)
				{
					dispose();
				}

				void parse(TiXmlElement* elem)
				{
					if(elem == NULL){
						return;
					}
					SCE_SAMPLE_UTIL_ASSERT(elem != NULL);
					for(TiXmlElement* celem=elem->FirstChildElement("image");
						celem != NULL;
						celem = celem->NextSiblingElement("image"))
					{
						Image *i = new Image;
						i->parse(celem);
						m_images.push_back(i);
					}
				}
			};

		}
		namespace material
		{
			struct Material
			{
				std::string m_id;
				std::string m_name;
				std::string m_instance_effect_url;
				void parse(TiXmlElement* elem)
				{
					m_id   = elem->Attribute("id");
					const char *name = elem->Attribute("name");
					m_name = (name==NULL)?"":name;
					m_instance_effect_url = elem->FirstChildElement("instance_effect")->Attribute("url");
				};
				void dispose(void)
				{
					m_id = "";
					m_name = "";
					m_instance_effect_url = "";
				}
				~Material(void)
				{
					dispose();
				}
			};

			struct LibraryMaterials
			{
				std::vector<Material*> m_materials;
				void parse(TiXmlElement* elem)
				{
					for(TiXmlElement* celem=elem->FirstChildElement("material");
						celem != NULL;
						celem = celem->NextSiblingElement("material"))
					{
						Material *e = new Material;
						e->parse(celem);
						m_materials.push_back(e);
					}
				}


				Material *findMaterialById(std::string id)
				{
					for(unsigned int i=0; i<m_materials.size(); i++){
						if(m_materials.at(i)->m_id == id){
							return m_materials.at(i);
						}
					}
					return NULL;
				}
				void dispose(void)
				{
					for(unsigned int i=0; i<m_materials.size(); i++){
						delete m_materials.at(i);
					}
					m_materials.clear();
				}

				~LibraryMaterials(void)
				{
					dispose();
				}
			};
		}

		namespace Scene
		{
			struct InstanceVisualScene
			{
				std::string m_url;

				int parse(TiXmlElement* elem)
				{
					m_url = elem->Attribute("url");
					return 0;
				}

				void dispose(void)
				{
					m_url = "";
				}
				~InstanceVisualScene(void)
				{
					dispose();
				}
			};

			struct Scene
			{
				InstanceVisualScene *m_instanceVisualScene;

				Scene(void)
				{
					m_instanceVisualScene = NULL;
				}

				int parse(TiXmlElement* elem)
				{
					if(elem == NULL){
						return 0;
					}
					if(elem->FirstChildElement("instance_visual_scene") != NULL){
						m_instanceVisualScene = new InstanceVisualScene();
						m_instanceVisualScene->parse(elem->FirstChildElement("instance_visual_scene"));
					}
					return 0;
				}

				void dispose(void)
				{
					if(m_instanceVisualScene != NULL){
						delete m_instanceVisualScene;
						m_instanceVisualScene = NULL;
					}
				}
				~Scene(void)
				{
					dispose();
				}

			};
		}

		struct Collada
		{
			Asset *m_asset;
			Image::LibraryImages                 *m_libraryImages;
			Dae::Geometry::LibraryGeometries     *m_libraryGeometries;
			Dae::Animation::LibraryAnimations    *m_libraryAnimations;
			Dae::Controller::LibraryControllers  *m_libraryControllers;
			Dae::Scene::LibraryVisualScenes      *m_libraryVisualScenes;
			Dae::effect::LibraryEffects          *m_libraryEffects;
			Dae::material::LibraryMaterials      *m_libraryMaterials;
			Dae::Scene::Scene                    *m_scene;
			Collada(void)
			{
				m_asset = NULL;
				m_libraryImages       = NULL;
				m_libraryGeometries   = NULL;
				m_libraryAnimations   = NULL;
				m_libraryControllers  = NULL;
				m_libraryVisualScenes = NULL;
				m_libraryEffects      = NULL;
				m_libraryMaterials    = NULL;
				m_scene               = NULL;
			}

			void dispose(void)
			{
				if(m_libraryImages != NULL){
					delete m_libraryImages;
					m_libraryImages = NULL;
				}
				if(m_libraryGeometries != NULL){
					delete m_libraryGeometries;
					m_libraryGeometries = NULL;
				}
				if(m_libraryAnimations != NULL){
					delete m_libraryAnimations;
					m_libraryAnimations = NULL;
				}
				if(m_libraryControllers != NULL){
					delete m_libraryControllers;
					m_libraryControllers = NULL;
				}
				if(m_libraryVisualScenes != NULL){
					delete m_libraryVisualScenes;
					m_libraryVisualScenes = NULL;
				}
				if(m_libraryEffects != NULL){
					delete m_libraryEffects;
					m_libraryEffects = NULL;
				}
				if(m_libraryMaterials != NULL){
					delete m_libraryMaterials;
					m_libraryMaterials = NULL;
				}
				if(m_scene != NULL){
					delete m_scene;
					m_scene = NULL;
				}
			}

			virtual ~Collada(void)
			{
				dispose();
			}

			int load(const char *filename, sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig)
			{
				//loaderConfig->onMessage("Loading %s\n", filename);
				TiXmlDocument	xml(filename);

				bool bret = xml.LoadFile(); 
				if(! bret){
					fprintf(stderr, __FILE__"(%d) Error when loading XML file : %s\n", __LINE__, filename);
					//printf("Error desc = %s\n", xml.ErrorDesc());
					return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_PARSE;
				}

				SCE_SAMPLE_UTIL_ASSERT(bret);
				SCE_SAMPLE_UTIL_ASSERT(isValid(xml));
				parse(loaderConfig, xml);
				xml.Clear();

				return 0;
			}

			bool isValid(TiXmlDocument	&xml)
			{
				// validate file
				TiXmlHandle colladaHandle = TiXmlHandle(&xml).FirstChild("COLLADA");
				TiXmlElement* element = colladaHandle.ToElement();
				if (element == NULL) {
					return false;
				}
				return true;
			}

			void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlDocument &xml)
			{
				TiXmlHandle colladaHandle = TiXmlHandle(&xml).FirstChild("COLLADA");
				TiXmlElement* element = colladaHandle.ToElement();


				m_asset = new Asset;
				m_asset->parse(element->FirstChildElement("asset"), loaderConfig);

				m_libraryGeometries = new Dae::Geometry::LibraryGeometries();
				m_libraryGeometries->parse(loaderConfig, element->FirstChildElement("library_geometries"));

				m_libraryAnimations = new Dae::Animation::LibraryAnimations;
				m_libraryAnimations->parse(loaderConfig, element->FirstChildElement("library_animations"));

				m_libraryControllers = new Dae::Controller::LibraryControllers();
				m_libraryControllers->parse(loaderConfig, element->FirstChildElement("library_controllers"));

				m_libraryVisualScenes = new Dae::Scene::LibraryVisualScenes();
				m_libraryVisualScenes->parse(loaderConfig, element->FirstChildElement("library_visual_scenes"));

				m_libraryImages = new Image::LibraryImages();
				m_libraryImages->parse(element->FirstChildElement("library_images"));

				m_libraryEffects = new Dae::effect::LibraryEffects();
				m_libraryEffects->parse(loaderConfig, element->FirstChildElement("library_effects"));
				m_libraryMaterials = new material::LibraryMaterials();
				m_libraryMaterials->parse(element->FirstChildElement("library_materials"));
				m_scene = new Dae::Scene::Scene();
				m_scene->parse(element->FirstChildElement("scene"));

			}

		};

	}
}
}
}
}


#endif /* _COLLADA_RENDER_UTIL_DAE_H */
