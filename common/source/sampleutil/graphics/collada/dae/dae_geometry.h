/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _COLLADA_LOADER_UTIL_DAE_GEOMETRY_H
#define _COLLADA_LOADER_UTIL_DAE_GEOMETRY_H

#define NOMINMAX 
#include <algorithm>
#include <sampleutil/graphics/collada/collada_base.h>
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

	  namespace Geometry
	  {
		  struct Vertices : public Dae::Common::IInputContainer
		  {
			  std::string m_id;
			  std::vector<Dae::DataFlow::Input*> m_inputs;

			  int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

			  virtual const std::vector<Dae::DataFlow::Input*> &getInputs(void) const { return m_inputs; }

			  void dispose(void)
			  {
				  for(unsigned int input_idx=0; input_idx< m_inputs.size(); input_idx++){
					  delete m_inputs.at(input_idx);
				  }
				  m_inputs.clear();
				  m_id = "";
			  }

			  virtual ~Vertices(void)
			  {
				  dispose();
			  }
		  };

		  struct Triangles : public Dae::Common::IInputContainer
		  {
			  std::string m_matrial;
			  int m_count;
			  std::vector<Dae::DataFlow::Input*> m_inputs;
			  uint16_t *p;
			  Triangles(void)
			  {
				  p = NULL;
			  }
			  int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

			  virtual const std::vector<Dae::DataFlow::Input*> &getInputs(void) const { return m_inputs; }
			  int getPStride(void) const
			  {
				  uint32_t max_offset = 0;
				  for(unsigned int input_idx=0; input_idx< m_inputs.size(); input_idx++){
					  Dae::DataFlow::Input *ci = m_inputs.at(input_idx);
					  max_offset = std::max(max_offset, ci->m_offset);
				  }
				  return max_offset+1;
			  }
			  void dispose(void);


			  virtual ~Triangles(void)
			  {
				  dispose();
			  }
		  };




	  struct Polylist : public Dae::Common::IInputContainer
	  {
		std::string m_matrial;
		int m_count;
		std::vector<Dae::DataFlow::Input*> m_inputs;
		uint16_t *vcount;
		uint16_t *p;
		Polylist(void)
		{
			vcount = NULL;
			p = NULL;
		}
		int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

		void dispose(void);
	

		~Polylist(void)
		{
			dispose();
		}
		
		
		virtual const std::vector<Dae::DataFlow::Input*> &getInputs(void) const { return m_inputs; }
		
		int getPStride(void) const
		{
			uint32_t max_offset = 0;
			for(unsigned int input_idx=0; input_idx< m_inputs.size(); input_idx++){
				Dae::DataFlow::Input *ci = m_inputs.at(input_idx);
				max_offset = std::max(max_offset, ci->m_offset);
			}
			return max_offset+1;
		}
		
		
	  };

	  struct Mesh : public Dae::Common::ISourceContainer
	  {
		std::vector<Dae::DataFlow::Source*> m_sources;
		std::vector<Vertices*> m_verticess;
		std::vector<Triangles*> m_triangless;
		std::vector<Polylist*> m_polylists;
			  
		virtual ~Mesh(void)
		{
			dispose();
		}

		void dispose(void);

		int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

		const Vertices *findVertices(std::string id) const
		{
		  for(unsigned int i=0; i<m_verticess.size(); i++){
			Vertices *v = m_verticess.at(i);
			if(v->m_id == id){
			  return v;
			}
		  }
		  return NULL;
		}
		virtual const std::vector<Dae::DataFlow::Source*> &getSources(void) const { return m_sources; }
	  };

	  struct Geometry
	  {
		std::string m_id;
		std::string m_name;
		Mesh *m_mesh;

		Geometry(void) : m_mesh(NULL)
		{
		}

		void dispose(void)
		{
			m_id = "";
			m_name = "";
			if(m_mesh != NULL){
				delete m_mesh;
				m_mesh = NULL;
			}
		}
		virtual ~Geometry(void)
		{
			dispose();
		}
		int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlHandle handle)
		{
			TiXmlElement* elem = handle.ToElement();
			SCE_SAMPLE_UTIL_ASSERT(elem != NULL);
			m_id = elem->Attribute("id");
			const char* name = elem->Attribute("name");
			m_name = (name==NULL)? "" : name;

			TiXmlElement *child_elem = elem->FirstChildElement("mesh");
			if(child_elem != NULL){
				m_mesh = new Mesh();
				m_mesh->parse(loaderConfig, child_elem);
			}else{
				fprintf(stderr, "mesh is not found in geometry %s\n", m_id.c_str());
			}
			return 0;
		}
	  };

	  struct LibraryGeometries
	  {
		  std::vector<Geometry*> m_geometries;
		  virtual ~LibraryGeometries(void)
		  {
			  for(unsigned int i=0; i<m_geometries.size(); i++){
				  delete m_geometries.at(i);
			  }
		  }

		  void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
		  {

			  for(TiXmlElement* celem = elem->FirstChildElement("geometry");
				  celem != NULL;
				  celem = celem->NextSiblingElement("geometry"))
			  {
				  Geometry *geometry = new Geometry();
				  int ret = geometry->parse(loaderConfig, celem);
				  if(ret == 0){
					  m_geometries.push_back(geometry);
				  }else{
					  delete geometry;
				  }
			  }

		  }
	  };

			struct InstanceGeometry
			{
				std::string m_url;
				Dae::effect::BindMaterial *m_bindMaterial;
				InstanceGeometry(void) : m_bindMaterial(NULL)
				{}

				void dispose(void)
				{
					if(m_bindMaterial != NULL){
						delete m_bindMaterial;
						m_bindMaterial = NULL;
					}
				}

				virtual ~InstanceGeometry(void)
				{
					dispose();
				}

				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					m_url = elem->Attribute("url");
					m_bindMaterial = NULL;
					if(elem->FirstChildElement("bind_material") != NULL){
						m_bindMaterial = new Dae::effect::BindMaterial;
						m_bindMaterial->parse(loaderConfig, elem->FirstChildElement("bind_material") );
					}
					return SCE_OK;
				}
			};


	}
  }
}
}
}
}


#endif /* _COLLADA_LOADER_UTIL_DAE_GEOMETRY_H */
