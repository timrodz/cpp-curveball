/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/



#ifndef _COLLADA_RENDER_UTIL_COLLADA_SCENE_H
#define _COLLADA_RENDER_UTIL_COLLADA_SCENE_H

#include <sampleutil/graphics/collada.h>
#include <sampleutil/graphics/collada/collada_geometry.h>
namespace sce
{
namespace SampleUtil
{
namespace Graphics
{

	namespace Collada
	{
	namespace Internal
	{
		struct Bind
		{
			std::string m_semantic;
			std::string m_target;

			int initialize(Dae::Common::Bind *bind)
			{
				m_semantic      = bind->m_semantic;
				m_target        = bind->m_target;
				return SCE_OK;
			}
			int finalize(void)
			{
				return SCE_OK;
			}
		};


		class InstanceMaterial
		{
		public:
			std::string m_symbol;
			std::string m_target;


			uint32_t m_numBinds;
			Bind* m_binds;

			int initialize(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, 
				Dae::Common::InstanceMaterial *instanceMaterial)
			{
				m_symbol = instanceMaterial->m_symbol;
				m_target = instanceMaterial->m_target;

				m_numBinds = instanceMaterial->m_binds.size();
				m_binds = new Bind[m_numBinds];
				for(unsigned int i=0; i<m_numBinds; i++){
					m_binds[i].initialize(instanceMaterial->m_binds.at(i));
				}

				return SCE_OK;
			}

			int finalize(void)
			{
				for(unsigned int i=0; i<m_numBinds; i++){
					m_binds[i].finalize();
				}
				delete [] m_binds;
				m_binds = NULL;

				return SCE_OK;
			}

			std::string getTargetId(void)
			{
				return sce::SampleUtil::Graphics::Collada::Internal::Util::sourceToId(m_target);
			}

			Bind* findBind(const std::string& semantic) const
			{
				for(unsigned int i=0; i< m_numBinds; i++){
					if(m_binds[i].m_semantic == semantic){
						return &m_binds[i];
					}
				}
				return NULL;
			}
		};

	}
	}
}
}
}




#endif /* _COLLADA_RENDER_UTIL_COLLADA_SCENE_H */
