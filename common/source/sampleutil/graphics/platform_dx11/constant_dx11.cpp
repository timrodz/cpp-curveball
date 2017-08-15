/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil/graphics.h>
#include "constant_internal_dx11.h"



namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

D3D11_STENCIL_OP ssgi::convertStencilOp(ssg::StencilOp o)
{
	switch(o)
	{
	case kStencilOpKeep:		return D3D11_STENCIL_OP_KEEP;
	case kStencilOpZero:		return D3D11_STENCIL_OP_ZERO;
	case kStencilOpReplace:		return D3D11_STENCIL_OP_REPLACE;
	case kStencilOpIncr:		return D3D11_STENCIL_OP_INCR;
	case kStencilOpDecr:		return D3D11_STENCIL_OP_DECR;
	case kStencilOpInvert:		return D3D11_STENCIL_OP_INVERT;
	case kStencilOpIncrWrap:	return D3D11_STENCIL_OP_INCR_SAT;
	case kStencilOpDecrWrap:	return D3D11_STENCIL_OP_DECR_SAT;
	default:                    return D3D11_STENCIL_OP_KEEP;
	}
}

D3D11_COMPARISON_FUNC ssgi::convertStencilFunc(ssg::StencilFunc s)
{
	switch(s)
	{
	case kStencilFuncNever			: return D3D11_COMPARISON_NEVER; 
	case kStencilFuncLess			: return D3D11_COMPARISON_LESS; 
	case kStencilFuncEqual			: return D3D11_COMPARISON_EQUAL; 
	case kStencilFuncLessEqual		: return D3D11_COMPARISON_LESS_EQUAL; 
	case kStencilFuncGreater		: return D3D11_COMPARISON_GREATER; 
	case kStencilFuncNotEqual		: return D3D11_COMPARISON_NOT_EQUAL; 
	case kStencilFuncGreaterEqual	: return D3D11_COMPARISON_GREATER_EQUAL; 
	case kStencilFuncAlways			: return D3D11_COMPARISON_ALWAYS; 
	default							: return D3D11_COMPARISON_ALWAYS;
	}
}

D3D11_CULL_MODE  ssgi::convertCullMode(ssg::CullMode c)
{
	switch(c)
	{
	case kCullCw  :  D3D11_CULL_FRONT;
	case kCullCcw :  D3D11_CULL_BACK;
	default : return D3D11_CULL_NONE;
	}
}
D3D_PRIMITIVE_TOPOLOGY  ssgi::convertPrimitive(ssg::Primitive p)
{
	switch(p)
	{
	case kPrimitivePoints			: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case kPrimitiveLines			: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case kPrimitiveTriangles		: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case kPrimitiveTriangleStrip	: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	default							: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

}

D3D11_COMPARISON_FUNC  ssgi::convertDepthFunc(ssg::DepthFunc d)
{
	switch(d)
	{
	case kDepthFuncNever		: return D3D11_COMPARISON_NEVER;
	case kDepthFuncLess			: return D3D11_COMPARISON_LESS;
	case kDepthFuncEqual		: return D3D11_COMPARISON_EQUAL;
	case kDepthFuncLessEqual	: return D3D11_COMPARISON_LESS_EQUAL;
	case kDepthFuncGreater		: return D3D11_COMPARISON_GREATER;
	case kDepthFuncNotEqual		: return D3D11_COMPARISON_NOT_EQUAL;
	case kDepthFuncGreaterEqual	: return D3D11_COMPARISON_GREATER_EQUAL;
	case kDepthFuncAlways		: return D3D11_COMPARISON_ALWAYS;
	default						: return D3D11_COMPARISON_NEVER;
	}
}

D3D11_TEXTURE_ADDRESS_MODE ssgi::convertTextureAddrMode(ssg::TextureAddrMode a)
{
	switch(a)
	{
	case kTextureAddrModeRepeat:            return D3D11_TEXTURE_ADDRESS_WRAP;
	case kTextureAddrModeMirror:            return D3D11_TEXTURE_ADDRESS_MIRROR;
	case kTextureAddrModeClamp:             return D3D11_TEXTURE_ADDRESS_CLAMP;
	case kTextureAddrModeMirrorClamp:		return D3D11_TEXTURE_ADDRESS_BORDER;
	case kTextureAddrModeClampFullBorder:   return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	default:                                return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
}

D3D11_FILTER ssgi::convertTextureFilter(ssg::TextureFilter minFilter,
												 ssg::TextureFilter magFilter,
												 ssg::TextureMipFilter mipFilter)
{
	
	if(minFilter == kTextureFilterPoint){
		if(magFilter == kTextureFilterPoint){
			if(mipFilter == kTextureMipFilterDisabled){
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			}else{
				return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			}
		}else{
			if(mipFilter == kTextureMipFilterDisabled){
				return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			}else{
				return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			}
		}
	}else{
		if(magFilter == kTextureFilterPoint){
			if(mipFilter == kTextureMipFilterDisabled){
				return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			}else{
				return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			}
		}else{
			if(mipFilter == kTextureMipFilterDisabled){
				return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			}else{
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}
	}
}

static uint8_t convertColorMask(uint8_t c)
{
	uint8_t ret = 0;
	ret |= (c & ssg::kColorMaskA) ? D3D11_COLOR_WRITE_ENABLE_ALPHA  : 0;
	ret |= (c & ssg::kColorMaskR) ? D3D11_COLOR_WRITE_ENABLE_RED    : 0;
	ret |= (c & ssg::kColorMaskG) ? D3D11_COLOR_WRITE_ENABLE_GREEN  : 0;
	ret |= (c & ssg::kColorMaskB) ? D3D11_COLOR_WRITE_ENABLE_BLUE   : 0;
	return ret;
}
static D3D11_BLEND_OP convertBlendFunc(uint8_t b)
{
	switch(b)
	{
	//case ssg::BLEND_FUNC_NONE:             return 0;
	case ssg::kBlendFuncAdd:				return D3D11_BLEND_OP_ADD;
	case ssg::kBlendFuncSubtract:			return D3D11_BLEND_OP_SUBTRACT; 
	case ssg::kBlendFuncReverseSubtract:	return D3D11_BLEND_OP_REV_SUBTRACT ;
	case ssg::kBlendFuncMin:				return D3D11_BLEND_OP_MIN;
	case ssg::kBlendFuncMax:				return D3D11_BLEND_OP_MAX ;
	default :								return D3D11_BLEND_OP_ADD;
	}
}
static D3D11_BLEND  convertBlendFactor(uint8_t b)
{
	switch(b)
	{
	case ssg::kBlendFactorZero:					return D3D11_BLEND_ZERO;
	case ssg::kBlendFactorOne:					return D3D11_BLEND_ONE;
	case ssg::kBlendFactorSrcColor:				return D3D11_BLEND_SRC_COLOR;
	case ssg::kBlendFactorOneMinusSrcColor:		return D3D11_BLEND_INV_SRC_COLOR;
	case ssg::kBlendFactorSrcAlpha:				return D3D11_BLEND_SRC_ALPHA;
	case ssg::kBlendFactorOneMinusSrcAlpha:		return D3D11_BLEND_INV_SRC_ALPHA;
	case ssg::kBlendFactorDstColor:				return D3D11_BLEND_DEST_COLOR;
	case ssg::kBlendFactorOneMinusDstColor:		return D3D11_BLEND_INV_DEST_COLOR;
	case ssg::kBlendFactorDstAlpha:				return D3D11_BLEND_DEST_ALPHA;
	case ssg::kBlendFactorOneMinusDstAlpha:		return D3D11_BLEND_INV_DEST_ALPHA;
	default :return D3D11_BLEND_ZERO;
	}
}

D3D11_BLEND_DESC  ssgi::convertBlendInfo(const ssg::BlendInfo &b)
{
	D3D11_BLEND_DESC  ret;
	ZeroMemory( &ret, sizeof( D3D11_BLEND_DESC ) );
	ret.IndependentBlendEnable          = FALSE;
	ret.RenderTarget[0].RenderTargetWriteMask = convertColorMask(b.colorMask);
	ret.RenderTarget[0].BlendEnable     = (b.colorFunc != kBlendFuncNone) || (b.alphaFunc != kBlendFuncNone);
	if(b.colorFunc != kBlendFuncNone){
		ret.RenderTarget[0].SrcBlend	      = convertBlendFactor(b.colorSrc);
		ret.RenderTarget[0].DestBlend	      = convertBlendFactor(b.colorDst);
		ret.RenderTarget[0].BlendOp	          = convertBlendFunc(b.colorFunc);
	}else{
		ret.RenderTarget[0].SrcBlend	      = D3D11_BLEND_ONE;
		ret.RenderTarget[0].DestBlend	      = D3D11_BLEND_ZERO;
		ret.RenderTarget[0].BlendOp	          = D3D11_BLEND_OP_ADD;
	}
	if(b.alphaFunc != kBlendFuncNone){
		ret.RenderTarget[0].SrcBlendAlpha     =	convertBlendFactor(b.alphaSrc);
		ret.RenderTarget[0].DestBlendAlpha    = convertBlendFactor(b.alphaSrc);
		ret.RenderTarget[0].BlendOpAlpha	  = convertBlendFunc(b.alphaFunc);
	}else{
		ret.RenderTarget[0].SrcBlendAlpha     =	D3D11_BLEND_ONE;
		ret.RenderTarget[0].DestBlendAlpha    = D3D11_BLEND_ZERO;
		ret.RenderTarget[0].BlendOpAlpha	  = D3D11_BLEND_OP_ADD;
	}
	return ret;
}



#endif /* _SCE_TARGET_OS_WINDOWS */


