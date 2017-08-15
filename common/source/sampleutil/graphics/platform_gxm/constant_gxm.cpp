/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/graphics.h>
#include "constant_internal_gxm.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

static SceGxmTextureFormat ssgi::convertTextureFormat(ssg::TextureFormat texFormat)
{
	switch(texFormat)
	{
	case ssg::kTextureFormatA8B8G8R8Unorm: return SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR;
	case ssg::kTextureFormatR8Unorm: return SCE_GXM_TEXTURE_FORMAT_U8_000R;
	default: abort();
	}
}



SceGxmMultisampleMode ssgi::convertMultiSampleMode(ssg::MultisampleMode m)
{
	switch(m)
	{
	case kMultisampleNone:  return SCE_GXM_MULTISAMPLE_NONE;
	case kMultisample2x:    return SCE_GXM_MULTISAMPLE_2X;
	case kMultisample4x:    return SCE_GXM_MULTISAMPLE_4X;
	default:               return SCE_GXM_MULTISAMPLE_NONE; 
	}

}

SceGxmStencilFunc ssgi::convertStencilFunc(StencilFunc s)
{
	switch(s)
	{
	case kStencilFuncNever        : return SCE_GXM_STENCIL_FUNC_NEVER; 
	case kStencilFuncLess         : return SCE_GXM_STENCIL_FUNC_LESS; 
	case kStencilFuncEqual        : return SCE_GXM_STENCIL_FUNC_EQUAL; 
	case kStencilFuncLessEqual   : return SCE_GXM_STENCIL_FUNC_LESS_EQUAL; 
	case kStencilFuncGreater      : return SCE_GXM_STENCIL_FUNC_GREATER; 
	case kStencilFuncNotEqual    : return SCE_GXM_STENCIL_FUNC_NOT_EQUAL; 
	case kStencilFuncGreaterEqual: return SCE_GXM_STENCIL_FUNC_GREATER_EQUAL; 
	case kStencilFuncAlways       : return SCE_GXM_STENCIL_FUNC_ALWAYS; 
	default:  return SCE_GXM_STENCIL_FUNC_ALWAYS;
	}
}
SceGxmStencilOp ssgi::convertStencilOp(StencilOp o)
{
	switch(o)
	{
	case kStencilOpKeep: return SCE_GXM_STENCIL_OP_KEEP;
	case kStencilOpZero:  return SCE_GXM_STENCIL_OP_ZERO;
	case kStencilOpReplace:  return SCE_GXM_STENCIL_OP_REPLACE;
	case kStencilOpIncr:  return SCE_GXM_STENCIL_OP_INCR;
	case kStencilOpDecr:  return SCE_GXM_STENCIL_OP_DECR;
	case kStencilOpInvert:  return SCE_GXM_STENCIL_OP_INVERT;
	case kStencilOpIncrWrap:  return SCE_GXM_STENCIL_OP_INCR_WRAP;
	case kStencilOpDecrWrap:  return SCE_GXM_STENCIL_OP_DECR_WRAP;
	default: return SCE_GXM_STENCIL_OP_KEEP;
	}
}


SceGxmCullMode  ssgi::convertCullMode(CullMode c)
{
	switch(c)
	{
	case kCullCw :  return SCE_GXM_CULL_CW;
	case kCullCcw : return SCE_GXM_CULL_CCW;
	default:        return SCE_GXM_CULL_NONE;
	}

}
SceGxmPrimitiveType  ssgi::convertPrimitive(Primitive p)
{
	switch(p)
	{
	case kPrimitivePoints          : return SCE_GXM_PRIMITIVE_POINTS;
	case kPrimitiveLines           : return SCE_GXM_PRIMITIVE_LINES;
	case kPrimitiveTriangles       : return SCE_GXM_PRIMITIVE_TRIANGLES;
	case kPrimitiveTriangleStrip  : return SCE_GXM_PRIMITIVE_TRIANGLE_STRIP;
	default                       : return SCE_GXM_PRIMITIVE_TRIANGLES;
	}
}

SceGxmDepthFunc  ssgi::convertDepthFunc(DepthFunc d)
{
	switch(d)
	{
	case kDepthFuncNever        : return SCE_GXM_DEPTH_FUNC_NEVER;
	case kDepthFuncLess          : return SCE_GXM_DEPTH_FUNC_LESS;
	case kDepthFuncEqual         : return SCE_GXM_DEPTH_FUNC_EQUAL;
	case kDepthFuncLessEqual    : return SCE_GXM_DEPTH_FUNC_LESS_EQUAL;
	case kDepthFuncGreater       : return SCE_GXM_DEPTH_FUNC_GREATER;
	case kDepthFuncNotEqual     : return SCE_GXM_DEPTH_FUNC_NOT_EQUAL;
	case kDepthFuncGreaterEqual : return SCE_GXM_DEPTH_FUNC_GREATER_EQUAL;
	case kDepthFuncAlways        : return SCE_GXM_DEPTH_FUNC_ALWAYS;
	default : return SCE_GXM_DEPTH_FUNC_NEVER;
	}
}

SceGxmTextureAddrMode  ssgi::convertTextureAddrMode(TextureAddrMode a)
{
	switch(a)
	{
	case kTextureAddrModeRepeat              : return SCE_GXM_TEXTURE_ADDR_REPEAT;
	case kTextureAddrModeMirror              : return SCE_GXM_TEXTURE_ADDR_MIRROR;
	case kTextureAddrModeClamp               : return SCE_GXM_TEXTURE_ADDR_CLAMP;
	case kTextureAddrModeMirrorClamp        : return SCE_GXM_TEXTURE_ADDR_MIRROR_CLAMP;
	case kTextureAddrModeClampFullBorder   : return SCE_GXM_TEXTURE_ADDR_CLAMP_FULL_BORDER;
	default:                                    return SCE_GXM_TEXTURE_ADDR_CLAMP;
	}
}

SceGxmTextureFilter     ssgi::convertTextureFilter(TextureFilter f)
{
	switch(f)
	{
	case kTextureFilterPoint : return SCE_GXM_TEXTURE_FILTER_POINT;
	case kTextureFilterLinear : return SCE_GXM_TEXTURE_FILTER_LINEAR;
	default: return SCE_GXM_TEXTURE_FILTER_POINT;

	}
}
SceGxmTextureMipFilter  ssgi::convertTextureMipFilter(TextureMipFilter m)
{
	switch(m)
	{
	case kTextureMipFilterDisabled : return SCE_GXM_TEXTURE_MIP_FILTER_DISABLED;
	case kTextureMipFilterEnabled  : return SCE_GXM_TEXTURE_MIP_FILTER_ENABLED;
	default: return SCE_GXM_TEXTURE_MIP_FILTER_DISABLED;
	}
}

static uint8_t convertColorMask(uint8_t c)
{
	uint8_t ret = 0;
	ret |= (c & ssg::kColorMaskA) ? SCE_GXM_COLOR_MASK_A : 0;
	ret |= (c & ssg::kColorMaskR) ? SCE_GXM_COLOR_MASK_R : 0;
	ret |= (c & ssg::kColorMaskG) ? SCE_GXM_COLOR_MASK_G : 0;
	ret |= (c & ssg::kColorMaskB) ? SCE_GXM_COLOR_MASK_B : 0;
	return ret;
}
static uint8_t convertBlendFunc(uint8_t b)
{
	switch(b)
	{
	case ssg::kBlendFuncNone:             return SCE_GXM_BLEND_FUNC_NONE;
	case ssg::kBlendFuncAdd:              return SCE_GXM_BLEND_FUNC_ADD;
	case ssg::kBlendFuncSubtract:         return SCE_GXM_BLEND_FUNC_SUBTRACT;
	case ssg::kBlendFuncReverseSubtract: return SCE_GXM_BLEND_FUNC_REVERSE_SUBTRACT;
	case ssg::kBlendFuncMin:              return SCE_GXM_BLEND_FUNC_MIN;
	case ssg::kBlendFuncMax:              return SCE_GXM_BLEND_FUNC_MAX;
	default :                                       return SCE_GXM_BLEND_FUNC_NONE;
	}
}
static uint8_t convertBlendFactor(uint8_t b)
{
	switch(b)
	{
	case ssg::kBlendFactorZero:                return SCE_GXM_BLEND_FACTOR_ZERO;
	case ssg::kBlendFactorOne:                 return SCE_GXM_BLEND_FACTOR_ONE;
	case ssg::kBlendFactorSrcColor:           return SCE_GXM_BLEND_FACTOR_SRC_COLOR;
	case ssg::kBlendFactorOneMinusSrcColor: return SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case ssg::kBlendFactorSrcAlpha:           return SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
	case ssg::kBlendFactorOneMinusSrcAlpha: return SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case ssg::kBlendFactorDstColor:           return SCE_GXM_BLEND_FACTOR_DST_COLOR;
	case ssg::kBlendFactorOneMinusDstColor: return SCE_GXM_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case ssg::kBlendFactorDstAlpha:           return SCE_GXM_BLEND_FACTOR_DST_ALPHA;
	case ssg::kBlendFactorOneMinusDstAlpha: return SCE_GXM_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	//case ssg::BLEND_FACTOR_SRC_ALPHA_SATURATE:  return SCE_GXM_BLEND_FACTOR_SRC_ALPHA_SATURATE;
	//case ssg::BLEND_FACTOR_DST_ALPHA_SATURATE:  return SCE_GXM_BLEND_FACTOR_DST_ALPHA_SATURATE;
	default :return SCE_GXM_BLEND_FACTOR_ZERO;
	}
}

SceGxmBlendInfo  ssgi::convertBlendInfo(const BlendInfo &b)
{
	SceGxmBlendInfo ret;
	ret.colorMask = convertColorMask(b.colorMask);
	ret.colorFunc = convertBlendFunc(b.colorFunc);
	ret.alphaFunc = convertBlendFunc(b.alphaFunc);
	ret.colorSrc =  convertBlendFactor(b.colorSrc);
	ret.colorDst =  convertBlendFactor(b.colorDst);
	ret.alphaSrc =  convertBlendFactor(b.alphaSrc);
	ret.alphaDst =  convertBlendFactor(b.alphaDst);
	return ret;
}


#endif /* _SCE_TARGET_OS_PSP2 */


