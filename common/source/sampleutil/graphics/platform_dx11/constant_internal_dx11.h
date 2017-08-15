/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_DX11_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_DX11_H

#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS


#include <sampleutil/graphics.h>
namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{


				D3D11_STENCIL_OP        convertStencilOp(StencilOp o);
				D3D11_COMPARISON_FUNC   convertStencilFunc(StencilFunc s);
				D3D11_CULL_MODE         convertCullMode(CullMode c);
				D3D_PRIMITIVE_TOPOLOGY  convertPrimitive(Primitive p);
				D3D11_COMPARISON_FUNC   convertDepthFunc(DepthFunc d);
				D3D11_TEXTURE_ADDRESS_MODE convertTextureAddrMode(TextureAddrMode a);
				D3D11_FILTER            convertTextureFilter(TextureFilter minFilter,
															 TextureFilter magFilter,
					                                         TextureMipFilter mipFilter);
				D3D11_BLEND_DESC        convertBlendInfo(const BlendInfo &b);
			}
		}
	}
}

#endif // _SCE_TARGET_OS_WINDOWS
#endif // _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_DX11_H
