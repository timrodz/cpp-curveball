/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _COLLADA_RENDER_UTIL_DAE_TRANSFORM_H
#define _COLLADA_RENDER_UTIL_DAE_TRANSFORM_H


#include "graphics/collada/collada_transform.h"
#include <graphics/collada/dae/dae_common.h>
#include <graphics/collada/dae/dae.h>
#include "dae_control.h"
#include <libdbg.h>

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
		namespace Transform
		{
			struct Matrix : public ::sce::SampleUtil::Graphics::Collada::Internal::Matrix
			{
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("sid") != NULL){
						m_sid = elem->Attribute("sid");
					}
					m_matrix = Internal::Util::fromElementText(elem);
				};

				virtual ~Matrix(void){}

			};

			struct Rotate : public ::sce::SampleUtil::Graphics::Collada::Internal::Rotate
			{
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("sid") != NULL){
						m_sid = elem->Attribute("sid");
					}
					float ary[4];
					Internal::Util::parseFloatArray(ary, 4, elem->GetText());
					//printf("sid=%s, axis=%.1f, %.1f %.1f \n", m_sid.c_str(), ary[0], ary[1], ary[2]);
					m_axis = Vector3(ary[0], ary[1], ary[2]);
					m_angle_degree = ary[3];
				};

				virtual ~Rotate(void){}

			};

			struct Translate : public ::sce::SampleUtil::Graphics::Collada::Internal::Translate
			{
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("sid") != NULL){
						m_sid = elem->Attribute("sid");
					}
					float ary[3];
					Internal::Util::parseFloatArray(ary, 3, elem->GetText());
					m_offset = Vector3(ary[0], ary[1], ary[2]);


				};
				virtual ~Translate(void) {}

			};

			struct Scale  : public ::sce::SampleUtil::Graphics::Collada::Internal::Scale
			{
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("sid") != NULL){
						m_sid = elem->Attribute("sid");
					}
					float ary[3];
					Internal::Util::parseFloatArray(ary, 3, elem->GetText());
					m_scale = Vector3(ary[0], ary[1], ary[2]);
				};
				virtual ~Scale(void) {}
			};

			struct Skew  : public ::sce::SampleUtil::Graphics::Collada::Internal::Skew
			{
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("sid") != NULL){
						m_sid = elem->Attribute("sid");
					}
					float ary[7];
					Internal::Util::parseFloatArray(ary, 7, elem->GetText());
					m_angle_degree = ary[0];
					m_vector_a = Vector3(ary[1], ary[2], ary[3]);
					m_vector_b = Vector3(ary[4], ary[5], ary[6]);
				};
				virtual ~Skew(void)
				{}
			};




}
}
	}
}

}
}
#endif /* _COLLADA_RENDER_UTIL_DAE_TRANSFORM_H */