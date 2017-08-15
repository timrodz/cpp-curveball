/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_DX11_INTERNAL_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_DX11_INTERNAL_H


#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil/graphics/loader.h>
#include <sampleutil/graphics/context.h>
#include <sampleutil/graphics/platform.h>
#include <sampleutil/graphics/effect.h>
#include <dxgi.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>


#include "buffer_internal_dx11.h"
#include "program_internal_dx11.h"
#include "loader_internal_dx11.h"
#include "context_internal_dx11.h"

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{


			namespace Impl
			{
				class UniformBufferImpl;
				class GraphicsLoaderImpl;
			}


		}
	}
}


#endif /* _SCE_TARGET_OS_WINDOWS */


#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_DX11_INTERNAL_H */

