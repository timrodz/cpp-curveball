/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_GNM_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_GNM_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS


#include <sampleutil/graphics.h>
#include <gnm.h>
#include <gnmx.h>
#include <shader.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				sce::Gnm::BlendMultiplier            convertBlendOp(BlendFactor f);
				sce::Gnm::BlendFunc                  convertBlendFunc(BlendFunc f);
				sce::Gnm::DataFormat                 convertTextureFormat(TextureFormat f);
				sce::Gnm::CompareFunc                convertStencilFunc(const StencilFunc f);
				sce::Gnm::StencilOp                  convertStencilOp(const StencilOp op);
				sce::Gnm::DataFormat                 getFormat(AttributeFormat format, uint8_t componentCount);
				sce::Gnm::CompareFunc                convertDepthFunc(DepthFunc depthFunc);
				sce::Gnm::PrimitiveSetupCullFaceMode convertCullMode(CullMode cullMode);
				sce::Gnm::PrimitiveType              convertPrimitive(Primitive primitive);
				sce::Gnm::ShaderStage                getShaderStage(ProgramType programType);
				uint32_t                             getPSSLTypeSize(sce::Shader::Binary::PsslType type);
				

				BufferFormat                         getBufferFormat(TextureFormat f);
				sce::Gnm::DataFormat                 convertBufferFormat(BufferFormat f);
				uint32_t                             getDataSize(BufferFormat f);
			}
		}
	}
}

#endif // _SCE_TARGET_OS_ORBIS
#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_CONSTANT_INTERNAL_GNM_H */



