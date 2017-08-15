/* SIE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_INTERNAL_RESOURCE_H
#define _SCE_SAMPLE_UTIL_INTERNAL_RESOURCE_H

#include <sampleutil.h>
#include <scebase_common.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Internal
		{
			class Resource
			{
			public:
#if _SCE_TARGET_OS_WINDOWS
				static void initialize(HINSTANCE moduleId);
#endif

				enum VertexShaderId
				{
					SPRITE_2D_V,
					SPRITE_2D_2_V,
					SPRITE_2D_YUY2_V,
					SPRITE_2D_YCBCR_V,
					FILL_2D_V,
					FILL_3D_V,
					POINT_SPRITE_2D_V,
					COLLADA_SKINNING_PHONG_V,
					WIREFRAME_V
				};

				static sce::SampleUtil::Graphics::Program* loadVertexShader(sce::SampleUtil::Graphics::GraphicsLoader *loader,
					                                                                VertexShaderId shader);

				enum FragmentShaderId
				{
					SPRITE_2D_F,
					SPRITE_2D_2_F,
					SPRITE_2D_YUY2_F,
					SPRITE_2D_YCBCR_F,
					FILL_2D_F,
					FILL_3D_F,
					POINT_SPRITE_2D_F,
					COLLADA_SKINNING_PHONG_F,
					WIREFRAME_F
				};

				static sce::SampleUtil::Graphics::Program* loadFragmentShader(sce::SampleUtil::Graphics::GraphicsLoader *loader,
					                                                                FragmentShaderId shader);

				enum TextureId
				{
					ASCII,
				};

				static sce::SampleUtil::Graphics::Texture* loadTexture(sce::SampleUtil::Graphics::GraphicsLoader *loader,
					                                                           TextureId texture);

			};
		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_GNM_INTERNAL_H */
