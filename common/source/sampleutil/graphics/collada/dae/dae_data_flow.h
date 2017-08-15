/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _COLLADA_RENDER_UTIL_DAE_DATA_FLOW_H
#define _COLLADA_RENDER_UTIL_DAE_DATA_FLOW_H

#include <string>
#include <graphics/collada/collada_util.h>



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
			namespace DataFlow
			{

				struct NameArray
				{
					enum Type { TYPE_IDREF, TYPE_NAME };
					Type  m_type;
					std::string m_id;
					int m_count;
					std::string* m_data;

					NameArray(Type type) : m_type(type), m_data(NULL){}
					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

					void dispose(void)
					{
						m_id = "";
						m_count = 0;

						if(m_data != NULL){
							delete [] m_data;
							m_data = NULL;
						}
					}

					~NameArray(void)
					{
						dispose();
					}
				};






				struct FloatArray
				{
					std::string m_id;
					uint32_t m_count;
					float *m_data;
					FloatArray(void) : m_data(NULL) {}
					virtual ~FloatArray(void)
					{
						dispose();
					}

					void dispose(void)
					{
						m_id = "";
						m_count = 0;
						if(m_data != NULL){
							delete [] m_data;
							m_data = NULL;
						}
					}

					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);
				};



				struct Param
				{
					std::string m_name;
					std::string m_sid;
					std::string m_type;
					std::string m_semantic;
					std::string m_ref;

					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

					void dispose(void)
					{
						m_name = "";
						m_sid = "";
						m_type ="";
						m_semantic = "";
						m_ref = "";
					}
				};



				struct Accessor
				{
					std::string m_source;
					int m_count;
					int m_stride;
					std::vector<Dae::DataFlow::Param*> params;
					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

					void dispose(void)
					{
						m_source = "";
						m_count = 0;
						m_stride = 0;
						for(unsigned int i=0; i<params.size(); i++){
							delete params.at(i);
						}
						params.clear();
					}

					virtual ~Accessor(void)
					{
						dispose();
					}
				};


				struct TechniqueCommonOfSource
				{
					Dae::DataFlow::Accessor* m_accessor;

					TechniqueCommonOfSource(void)
					{
						m_accessor = NULL;
					}

					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

					void dispose(void)
					{
						if(m_accessor != NULL){
							delete m_accessor;
							m_accessor = NULL;
						}
					}

					~TechniqueCommonOfSource(void)
					{
						dispose();
					}
				};



				struct Source {
					std::string m_id;
					std::string m_name;

					enum { 
						ARRAY_ELEMENT_TYPE_NONE,
						ARRAY_ELEMENT_TYPE_FLOAT, 
						ARRAY_ELEMENT_TYPE_NAME
					} m_arrayElementType;

					union {
						Dae::DataFlow::FloatArray *m_floatArray;
						Dae::DataFlow::NameArray *m_nameArray;
					};

					TechniqueCommonOfSource *m_techniqueCommon;

					Source(void):
					m_techniqueCommon(NULL) 
					{
						m_arrayElementType = ARRAY_ELEMENT_TYPE_NONE;
						m_floatArray = NULL;
					}

					~Source(void)
					{
						dispose();
					}

					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig,TiXmlElement* elem);

					uint32_t getDimension(void) const
					{
						return m_techniqueCommon->m_accessor->params.size();
					}

					void dispose(void);
				};


				struct Input
				{
					uint32_t                 m_offset;
					std::string              m_semanticStr;
					Dae::VertexSemantic                 m_semantic;
					std::string              m_source;
					int              m_set;

					int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem);

					void dispose(void)
					{
						m_offset = 0;
						m_semanticStr = "";
						m_source = "";
						m_set = 0;
					}

					~Input(void){
						dispose();
					}

				};

			};


		}
	}
}
}
}
#endif /* _COLLADA_RENDER_UTIL_DAE_DATA_FLOW_H */