/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#include "constant_internal_gnm.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;



sce::Gnm::BlendMultiplier ssgi::convertBlendOp(ssg::BlendFactor f)
{
	switch(f)
	{
	case ssg::kBlendFactorZero:                return sce::Gnm::kBlendMultiplierZero;
	case ssg::kBlendFactorOne:                 return sce::Gnm::kBlendMultiplierOne;
	case ssg::kBlendFactorSrcColor:           return sce::Gnm::kBlendMultiplierSrcColor;
	case ssg::kBlendFactorOneMinusSrcColor: return sce::Gnm::kBlendMultiplierOneMinusSrcColor;
	case ssg::kBlendFactorSrcAlpha:           return sce::Gnm::kBlendMultiplierSrcAlpha;
	case ssg::kBlendFactorOneMinusSrcAlpha: return sce::Gnm::kBlendMultiplierOneMinusSrcAlpha;
	case ssg::kBlendFactorDstColor:           return sce::Gnm::kBlendMultiplierDestColor;
	case ssg::kBlendFactorOneMinusDstColor: return sce::Gnm::kBlendMultiplierOneMinusDestColor;
	case ssg::kBlendFactorDstAlpha:           return sce::Gnm::kBlendMultiplierDestAlpha;
	case ssg::kBlendFactorOneMinusDstAlpha: return sce::Gnm::kBlendMultiplierOneMinusDestAlpha;
	default:                                    return sce::Gnm::kBlendMultiplierOne;
	}
}


sce::Gnm::BlendFunc ssgi::convertBlendFunc(ssg::BlendFunc f)
{
	switch(f)
	{
	case ssg::kBlendFuncNone:              return sce::Gnm::kBlendFuncAdd;
	case ssg::kBlendFuncAdd:               return sce::Gnm::kBlendFuncAdd;
	case ssg::kBlendFuncSubtract:          return sce::Gnm::kBlendFuncSubtract;
	case ssg::kBlendFuncReverseSubtract:  return sce::Gnm::kBlendFuncReverseSubtract;
	case ssg::kBlendFuncMin:               return sce::Gnm::kBlendFuncMin;
	case ssg::kBlendFuncMax:               return sce::Gnm::kBlendFuncMax;
	default :                               return sce::Gnm::kBlendFuncAdd;
	}
}




sce::Gnm::DataFormat ssgi::convertTextureFormat(ssg::TextureFormat f)
{
	switch(f)
	{
	case ssg::kTextureFormatA8B8G8R8Unorm:
		{
			return sce::Gnm::kDataFormatR8G8B8A8Unorm;
		}
	case ssg::kTextureFormatG8R8Unorm:
		{
			return sce::Gnm::kDataFormatR8G8Unorm;
		}
	case ssg::kTextureFormatR8Unorm:
		{
			return sce::Gnm::kDataFormatR8Unorm;
		}
	case ssg::kTextureFormatA16B16G16R16Unorm:
		{
			return sce::Gnm::kDataFormatR16G16B16A16Unorm;
		}
	case ssg::kTextureFormatG16R16Unorm:
		{
			return sce::Gnm::kDataFormatR16G16Unorm;
		}
	case ssg::kTextureFormatR16Unorm:
		{
			return sce::Gnm::kDataFormatR16Unorm;
		}

	default:
		{
			return sce::Gnm::kDataFormatR8G8B8A8Unorm;
		}
	}
}

sce::SampleUtil::Graphics::BufferFormat ssgi::getBufferFormat(TextureFormat f)
{

	switch(f)
	{
	case ssg::kTextureFormatA8B8G8R8Unorm:
		{
			return kBufferFormatX8X8X8X8;
		}
	case ssg::kTextureFormatG8R8Unorm:
		{
			return kBufferFormatX8X8;
		}
	case ssg::kTextureFormatR8Unorm:
		{
			return kBufferFormatX8;
		}
	case ssg::kTextureFormatA16B16G16R16Unorm:
		{
			return kBufferFormatX16X16X16X16;
		}
	case ssg::kTextureFormatG16R16Unorm:
		{
			return kBufferFormatX16X16;
		}
	case ssg::kTextureFormatR16Unorm:
		{
			return kBufferFormatX16;
		}

	default:
		{
			return kBufferFormatUnknown;
		}
	}
}

sce::Gnm::DataFormat                 ssgi::convertBufferFormat(ssg::BufferFormat f)
{
	switch(f)
	{
	case ssg::kBufferFormatX8X8X8X8:
		return sce::Gnm::kDataFormatR8G8B8A8Unorm;
	case ssg::kBufferFormatX8X8:
		return sce::Gnm::kDataFormatR8G8Unorm;
	case ssg::kBufferFormatX8:
		return sce::Gnm::kDataFormatR8Unorm;
	case ssg::kBufferFormatX16X16X16X16:
		return sce::Gnm::kDataFormatR16G16B16A16Unorm;
	case ssg::kBufferFormatX16X16:
		return sce::Gnm::kDataFormatR16G16Unorm;
	case ssg::kBufferFormatX16:
		return sce::Gnm::kDataFormatR16Unorm;
	case ssg::kBufferFormatX32X8X24:
	case ssg::kBufferFormatX32:
	case ssg::kBufferFormatX24X8:
	default:
		abort();
	}
}
uint32_t                             ssgi::getDataSize(ssg::BufferFormat f)
{
	switch(f)
	{
	case ssg::kBufferFormatX8X8X8X8: return 4;
	case ssg::kBufferFormatX8X8: return 2;
	case ssg::kBufferFormatX8: return 1;
	case ssg::kBufferFormatX16X16X16X16: return 8;
	case ssg::kBufferFormatX16X16: return 4;
	case ssg::kBufferFormatX16: return 2;
	case ssg::kBufferFormatX32X8X24: return 8;
	case ssg::kBufferFormatX32: return 4;
	case ssg::kBufferFormatX24X8: return 4;
	default:
		abort();
	}
}




sce::Gnm::CompareFunc ssgi::convertStencilFunc(const sce::SampleUtil::Graphics::StencilFunc f)
{
	switch(f)
	{
	case sce::SampleUtil::Graphics::kStencilFuncNever:         return sce::Gnm::kCompareFuncNever;
	case sce::SampleUtil::Graphics::kStencilFuncLess:          return sce::Gnm::kCompareFuncLess;
	case sce::SampleUtil::Graphics::kStencilFuncEqual:         return sce::Gnm::kCompareFuncEqual;
	case sce::SampleUtil::Graphics::kStencilFuncLessEqual:    return sce::Gnm::kCompareFuncLessEqual;
	case sce::SampleUtil::Graphics::kStencilFuncGreater:       return sce::Gnm::kCompareFuncGreater;
	case sce::SampleUtil::Graphics::kStencilFuncNotEqual:     return sce::Gnm::kCompareFuncNotEqual;
	case sce::SampleUtil::Graphics::kStencilFuncGreaterEqual: return sce::Gnm::kCompareFuncGreaterEqual;
	case sce::SampleUtil::Graphics::kStencilFuncAlways:        return sce::Gnm::kCompareFuncAlways;
	default:                         return sce::Gnm::kCompareFuncNever;
	}
}


sce::Gnm::StencilOp ssgi::convertStencilOp(const sce::SampleUtil::Graphics::StencilOp op)
{
	switch(op)
	{
	case sce::SampleUtil::Graphics::kStencilOpKeep:       return sce::Gnm::kStencilOpKeep;
	case sce::SampleUtil::Graphics::kStencilOpZero:       return sce::Gnm::kStencilOpZero;
	case sce::SampleUtil::Graphics::kStencilOpReplace:    return sce::Gnm::kStencilOpReplaceTest;
	case sce::SampleUtil::Graphics::kStencilOpIncr:       return sce::Gnm::kStencilOpAddClamp;
	case sce::SampleUtil::Graphics::kStencilOpDecr:       return sce::Gnm::kStencilOpSubClamp;
	case sce::SampleUtil::Graphics::kStencilOpInvert:     return sce::Gnm::kStencilOpInvert;
	case sce::SampleUtil::Graphics::kStencilOpIncrWrap:  return sce::Gnm::kStencilOpAddWrap;
	case sce::SampleUtil::Graphics::kStencilOpDecrWrap:  return sce::Gnm::kStencilOpSubWrap;
	default:                    return sce::Gnm::kStencilOpKeep;
	}
}


sce::Gnm::DataFormat ssgi::getFormat(ssg::AttributeFormat format, uint8_t componentCount)
{
	//printf(__FILE__"(%d) format=%d, count=%d\n", __LINE__, format, componentCount);
	if(componentCount > 4){
		return 	sce::Gnm::kDataFormatInvalid;
	}
	switch(format)
	{
	case ssg::kAttributeFormatF32:
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR32Float,
				sce::Gnm::kDataFormatR32G32Float,
				sce::Gnm::kDataFormatR32G32B32Float,
				sce::Gnm::kDataFormatR32G32B32A32Float
			};
			return f[componentCount];
			
			break;
		}
	case ssg::kAttributeFormatU16:
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR16Uint,
				sce::Gnm::kDataFormatR16G16Uint,
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR16G16B16A16Uint
			};
			return f[componentCount];
			break;
		}
	case ssg::kAttributeFormatU8:
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR8Uint,
				sce::Gnm::kDataFormatR8G8Uint,
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR8G8B8A8Uint
			};
			return f[componentCount];
			break;
		}
	case ssg::kAttributeFormatS8:
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR8Sint,
				sce::Gnm::kDataFormatR8G8Sint,
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR8G8B8A8Sint
			};
			return f[componentCount];
		}
	case ssg::kAttributeFormatS16:  
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR16Sint,
				sce::Gnm::kDataFormatR16G16Sint,
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR16G16B16A16Sint
			};
			return f[componentCount];
		}
	case ssg::kAttributeFormatU8N:  
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR8Unorm,
				sce::Gnm::kDataFormatR8G8Unorm,
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR8G8B8A8Unorm
			};
			return f[componentCount];
		}
	case ssg::kAttributeFormatS8N:  
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR8Snorm,
				sce::Gnm::kDataFormatR8G8Snorm,
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR8G8B8A8Snorm
			};
			return f[componentCount];
		}
	case ssg::kAttributeFormatU16N: 
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR16Unorm,
				sce::Gnm::kDataFormatR16G16Unorm,
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR16G16B16A16Unorm
			};
			return f[componentCount];
		}
	case ssg::kAttributeFormatF16:  
		{
			static sce::Gnm::DataFormat f[5] = {
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR16Float,
				sce::Gnm::kDataFormatR16G16Float,
				sce::Gnm::kDataFormatInvalid,
				sce::Gnm::kDataFormatR16G16B16A16Float
			};
			return f[componentCount];
		}

	default:
		// TODO: not supported yet.
		SCE_SAMPLE_UTIL_ASSERT(0);
		return sce::Gnm::kDataFormatInvalid;
	}
}


sce::Gnm::CompareFunc ssgi::convertDepthFunc(ssg::DepthFunc depthFunc)
{
	switch(depthFunc)
	{
	case kDepthFuncNever:
		{
			return sce::Gnm::kCompareFuncNever;
		}
	case kDepthFuncLess:
		{
			return sce::Gnm::kCompareFuncLess;
		}
	case kDepthFuncEqual:
		{
			return sce::Gnm::kCompareFuncEqual;
		}
	case kDepthFuncLessEqual:
		{
			return sce::Gnm::kCompareFuncLessEqual;
		}
	case kDepthFuncGreater:
		{
			return sce::Gnm::kCompareFuncGreater;
		}
	case kDepthFuncNotEqual:
		{
			return  sce::Gnm::kCompareFuncNotEqual;
		}
	case kDepthFuncGreaterEqual:
		{
			return sce::Gnm::kCompareFuncGreaterEqual;
		}
	case kDepthFuncAlways:
		{
			return sce::Gnm::kCompareFuncAlways;
		}
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
	}
}


sce::Gnm::PrimitiveSetupCullFaceMode ssgi::convertCullMode(ssg::CullMode cullMode)
{
	sce::Gnm::PrimitiveSetupCullFaceMode mode = sce::Gnm::kPrimitiveSetupCullFaceNone;;
	switch(cullMode)
	{
	case kCullNone:
		{
			mode = sce::Gnm::kPrimitiveSetupCullFaceNone;
			break;
		}
	case kCullCw:
		{
			mode = sce::Gnm::kPrimitiveSetupCullFaceFront;
			break;
		}
	case kCullCcw:
		{
			mode = sce::Gnm::kPrimitiveSetupCullFaceBack;
			break;
		}
	}
	return mode;
}


sce::Gnm::PrimitiveType ssgi::convertPrimitive(ssg::Primitive primitive)
{
	sce::Gnm::PrimitiveType pt = sce::Gnm::kPrimitiveTypeNone;
	switch(primitive)
	{
	case ssg::kPrimitivePoints:
		{
			pt = sce::Gnm::kPrimitiveTypePointList;
			break;
		}
	case ssg::kPrimitiveLines:
		{
			pt = sce::Gnm::kPrimitiveTypeLineList;
			break;
		}
	case ssg::kPrimitiveTriangles:
		{
			pt = sce::Gnm::kPrimitiveTypeTriList;
			break;
		}
	case ssg::kPrimitiveTriangleStrip:
		{
			pt = sce::Gnm::kPrimitiveTypeTriStrip;
			break;
		}
	}
	return pt;
}


sce::Gnm::ShaderStage ssgi::getShaderStage(ssg::ProgramType programType)
{
	switch(programType){
	case ssg::kProgramTypeVertex:   { return sce::Gnm::kShaderStageVs; }
	case ssg::kProgramTypeFragment: { return sce::Gnm::kShaderStagePs; }
	case ssg::kProgramTypeCompute:  { return sce::Gnm::kShaderStageCs; }
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
	}	
}


uint32_t ssgi::getPSSLTypeSize(sce::Shader::Binary::PsslType type)
			{

				int elem = sce::Shader::Binary::getPsslTypeElementCount(type);
				switch(type){
				case sce::Shader::Binary::kTypeInt1:
				case sce::Shader::Binary::kTypeInt2:
				case sce::Shader::Binary::kTypeInt3:
				case sce::Shader::Binary::kTypeInt4:
				case sce::Shader::Binary::kTypeInt1x1:
				case sce::Shader::Binary::kTypeInt2x1:
				case sce::Shader::Binary::kTypeInt3x1:
				case sce::Shader::Binary::kTypeInt4x1:
				case sce::Shader::Binary::kTypeInt1x2:
				case sce::Shader::Binary::kTypeInt2x2:
				case sce::Shader::Binary::kTypeInt3x2:
				case sce::Shader::Binary::kTypeInt4x2:
				case sce::Shader::Binary::kTypeInt1x3:
				case sce::Shader::Binary::kTypeInt2x3:
				case sce::Shader::Binary::kTypeInt3x3:
				case sce::Shader::Binary::kTypeInt4x3:
				case sce::Shader::Binary::kTypeInt1x4:
				case sce::Shader::Binary::kTypeInt2x4:
				case sce::Shader::Binary::kTypeInt3x4:
				case sce::Shader::Binary::kTypeInt4x4:
				case sce::Shader::Binary::kTypeUint1:
				case sce::Shader::Binary::kTypeUint2:
				case sce::Shader::Binary::kTypeUint3:
				case sce::Shader::Binary::kTypeUint4:
				case sce::Shader::Binary::kTypeUint1x1:
				case sce::Shader::Binary::kTypeUint2x1:
				case sce::Shader::Binary::kTypeUint3x1:
				case sce::Shader::Binary::kTypeUint4x1:
				case sce::Shader::Binary::kTypeUint1x2:
				case sce::Shader::Binary::kTypeUint2x2:
				case sce::Shader::Binary::kTypeUint3x2:
				case sce::Shader::Binary::kTypeUint4x2:
				case sce::Shader::Binary::kTypeUint1x3:
				case sce::Shader::Binary::kTypeUint2x3:
				case sce::Shader::Binary::kTypeUint3x3:
				case sce::Shader::Binary::kTypeUint4x3:
				case sce::Shader::Binary::kTypeUint1x4:
				case sce::Shader::Binary::kTypeUint2x4:
				case sce::Shader::Binary::kTypeUint3x4:
				case sce::Shader::Binary::kTypeUint4x4:
					return elem * sizeof(int);
				case sce::Shader::Binary::kTypeHalf1:
				case sce::Shader::Binary::kTypeHalf2:
				case sce::Shader::Binary::kTypeHalf3:
				case sce::Shader::Binary::kTypeHalf4:
				case sce::Shader::Binary::kTypeHalf1x1:
				case sce::Shader::Binary::kTypeHalf2x1:
				case sce::Shader::Binary::kTypeHalf3x1:
				case sce::Shader::Binary::kTypeHalf4x1:
				case sce::Shader::Binary::kTypeHalf1x2:
				case sce::Shader::Binary::kTypeHalf2x2:
				case sce::Shader::Binary::kTypeHalf3x2:
				case sce::Shader::Binary::kTypeHalf4x2:
				case sce::Shader::Binary::kTypeHalf1x3:
				case sce::Shader::Binary::kTypeHalf2x3:
				case sce::Shader::Binary::kTypeHalf3x3:
				case sce::Shader::Binary::kTypeHalf4x3:
				case sce::Shader::Binary::kTypeHalf1x4:
				case sce::Shader::Binary::kTypeHalf2x4:
				case sce::Shader::Binary::kTypeHalf3x4:
				case sce::Shader::Binary::kTypeHalf4x4:
					return elem * 2;
				case sce::Shader::Binary::kTypeFloat1:
				case sce::Shader::Binary::kTypeFloat2:
				case sce::Shader::Binary::kTypeFloat3:
				case sce::Shader::Binary::kTypeFloat4:
				case sce::Shader::Binary::kTypeFloat1x1:
				case sce::Shader::Binary::kTypeFloat2x1:
				case sce::Shader::Binary::kTypeFloat3x1:
				case sce::Shader::Binary::kTypeFloat4x1:
				case sce::Shader::Binary::kTypeFloat1x2:
				case sce::Shader::Binary::kTypeFloat2x2:
				case sce::Shader::Binary::kTypeFloat3x2:
				case sce::Shader::Binary::kTypeFloat4x2:
				case sce::Shader::Binary::kTypeFloat1x3:
				case sce::Shader::Binary::kTypeFloat2x3:
				case sce::Shader::Binary::kTypeFloat3x3:
				case sce::Shader::Binary::kTypeFloat4x3:
				case sce::Shader::Binary::kTypeFloat1x4:
				case sce::Shader::Binary::kTypeFloat2x4:
				case sce::Shader::Binary::kTypeFloat3x4:
				case sce::Shader::Binary::kTypeFloat4x4:
					return elem * sizeof(float);
				default:
					SCE_SAMPLE_UTIL_ASSERT(0);
					return 0;
				}
			}


#endif // _SCE_TARGET_OS_ORBIS
