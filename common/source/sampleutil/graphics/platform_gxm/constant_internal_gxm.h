/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_GXM_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_GXM_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/graphics.h>
#include <gxm.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class FrameBufferImpl;
				class RenderTargetImpl;

				struct DisplayCallbackArg
				{
					void *backBuffer;
					uint32_t frameBufferIndex;
					FrameBufferImpl *frameBuffer;
					uint32_t numVSync;
					RenderTargetImpl *renderTarget;
				};

				SceGxmTextureFormat     convertTextureFormat(TextureFormat texFormat);
				SceGxmMultisampleMode   convertMultiSampleMode(MultisampleMode m);
				SceGxmStencilFunc       convertStencilFunc(StencilFunc s);
				SceGxmStencilOp         convertStencilOp(StencilOp o);
				SceGxmCullMode          convertCullMode(CullMode c);
				SceGxmPrimitiveType     convertPrimitive(Primitive p);
				SceGxmDepthFunc         convertDepthFunc(DepthFunc d);
				SceGxmTextureAddrMode   convertTextureAddrMode(TextureAddrMode a);

				SceGxmTextureFilter     convertTextureFilter(TextureFilter f);
				SceGxmTextureMipFilter  convertTextureMipFilter(TextureMipFilter m);

				SceGxmBlendInfo         convertBlendInfo(const BlendInfo &b);


			}
		}
	}
}


#endif // _SCE_TARGET_OS_PSP2
#endif //_SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_GXM_H
