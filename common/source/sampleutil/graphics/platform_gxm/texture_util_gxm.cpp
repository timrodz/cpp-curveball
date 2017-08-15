/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include "util/util.h"
#include "texture_util_gxm.h"

namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;

uint32_t ssgi::getBitPerPixel(const SceGxmTextureFormat texFormat)
{
	uint32_t baseTexFormat = (uint32_t)texFormat & SCE_GXM_TEXTURE_BASE_FORMAT_MASK;
	uint32_t ret = 64;

	switch(baseTexFormat){
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U8	:	ret = 8;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U16	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_F16	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_F32M	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U8U8	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_F16F16	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U5U6U5	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U4U4U4U4	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U1U5U5U5	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_PVRT2BPP	:	ret = 2;	break;	/*J 8x4ピクセルブロックごとに8バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_PVRT4BPP	:	ret = 4;	break;	/*J 4x4ピクセルブロックごとに8バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_PVRTII2BPP	:	ret = 2;	break;	/*J 8x4ピクセルブロックごとに8バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_PVRTII4BPP	:	ret = 4;	break;	/*J 4x4ピクセルブロックごとに8バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_UBC1	:	ret = 4;	break;	/*J 4x4ピクセルブロックごとに8バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_UBC2	:	ret = 8;	break;	/*J 4x4ピクセルブロックごとに16バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_UBC3	:	ret = 8;	break;	/*J 4x4ピクセルブロックごとに16バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_UBC4	:	ret = 4;	break;	/*J 4x4ピクセルブロックごとに8バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_SBC4	:	ret = 4;	break;	/*J 4x4ピクセルブロックごとに8バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_UBC5	:	ret = 8;	break;	/*J 4x4ピクセルブロックごとに16バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_SBC5	:	ret = 8;	break;	/*J 4x4ピクセルブロックごとに16バイトのメモリが占有されます */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_YUV422	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S8	:	ret = 8;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S16	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U32	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S32	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_F32	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_X8U24	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S8S8	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U16U16	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S16S16	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_F32F32	:	ret = 64;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U32U32	:	ret = 64;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S5S5U6	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_SE5M9M9M9	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_F11F11F10	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_X8S8S8U8	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8	:	ret = 24;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8	:	ret = 24;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U8U3U3U2	:	ret = 16;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8S8	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U16U16U16U16	:	ret = 64;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_S16S16S16S16	:	ret = 64;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_F16F16F16F16	:	ret = 64;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U2U10U10U10	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_U2F10F10F10	:	ret = 32;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_P4	:	ret = 4;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_P8	:	ret = 8;	break;
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_YUV420P2	:	ret = 24;	break; /* 8+16/4 +alpha TODO */
	case 	SCE_GXM_TEXTURE_BASE_FORMAT_YUV420P3	:	ret = 24;	break; /* 8+8/4+8/4 +alpha TODO */
	default:	break;
	}

	return ret;
}

uint32_t ssgi::getTextureLinearSize(SceGxmTextureFormat texFormat,  uint32_t width,  uint32_t height,  uint32_t mipCount)
{
	uint32_t size = 0;
	if(mipCount == 0){
		size = ssiu::ceil(width, SCE_GXM_TEXTURE_IMPLICIT_STRIDE_ALIGNMENT)
			* height * ssgi::getBitPerPixel(texFormat) / 8;
	}else{
		// get size for mipmap
		uint32_t widthForMip = ceilPowersOf2(width);
		uint32_t heightForMip = ceilPowersOf2(height);
		uint32_t pixel = ssiu::ceil(widthForMip, SCE_GXM_TEXTURE_IMPLICIT_STRIDE_ALIGNMENT) * heightForMip;

		for(int i=0; i<mipCount; i++){
			widthForMip /= 2;
			heightForMip /= 2;
			if(widthForMip == 0){
				widthForMip = 1;
			}
			if(heightForMip == 0){
				heightForMip = 1;
			}
			pixel += ssiu::ceil(widthForMip, SCE_GXM_TEXTURE_IMPLICIT_STRIDE_ALIGNMENT) * heightForMip;
		}
		size = pixel * ssgi::getBitPerPixel(texFormat) / 8;
	}
	return size;
}

uint32_t ssgi::getTextureTiledSize(SceGxmTextureFormat texFormat,  uint32_t width,  uint32_t height,  uint32_t mipCount)
{
	uint32_t size = 0;

	if(mipCount == 0){
		size = ssiu::ceil(width, SCE_GXM_TILE_SIZEX) * ssiu::ceil(height, SCE_GXM_TILE_SIZEY) * ssgi::getBitPerPixel(texFormat) / 8;
	}else{
		// get size for mipmap
		uint32_t widthForMip = ceilPowersOf2(ssiu::ceil(width, SCE_GXM_TILE_SIZEX));
		uint32_t heightForMip = ceilPowersOf2(ssiu::ceil(height, SCE_GXM_TILE_SIZEY));
		uint32_t pixel = widthForMip * heightForMip;

		for(int i=0; i<mipCount; i++){
			widthForMip /= 2;
			heightForMip /= 2;
			if(widthForMip < 64){
				widthForMip = 64;
			}
			if(heightForMip < 64){
				heightForMip = 64;
			}
			pixel += widthForMip * heightForMip;
		}
		size = pixel * ssgi::getBitPerPixel(texFormat) / 8;
	}

	return size;
}

uint32_t ssgi::getTextureSwizzledSize(SceGxmTextureFormat texFormat, uint32_t width, uint32_t height,uint32_t mipCount)
{
	uint32_t size = 0;
	uint32_t widthForSwiz = ceilPowersOf2(width);
	uint32_t heightForSwiz = ceilPowersOf2(height);

	if(mipCount == 0){
		size = width * height * ssgi::getBitPerPixel(texFormat) / 8;
	}else{
		// get size for mipmap
		uint32_t widthForMip = widthForSwiz;
		uint32_t heightForMip = heightForSwiz;
		uint32_t pixel = widthForMip * heightForMip;

		for(int i=0; i<mipCount; i++){
			widthForMip /= 2;
			heightForMip /= 2;
			if(widthForMip == 0){
				widthForMip = 1;
			}
			if(heightForMip == 0){
				heightForMip = 1;
			}
			pixel += widthForMip * heightForMip;
		}
		size = pixel * ssgi::getBitPerPixel(texFormat) / 8;
	}
	return size;
}

uint32_t ssgi::getTextureCubeSize(SceGxmTextureFormat texFormat, uint32_t width, uint32_t height, uint32_t mipCount)
{
	uint32_t size = 0;
	uint32_t widthForSwiz = ceilPowersOf2(width);
	uint32_t heightForSwiz = ceilPowersOf2(height);

	if(mipCount == 0){
		size = width * height * ssgi::getBitPerPixel(texFormat) / 8 * 6;
	}else{
		// get size for mipmap
		uint32_t widthForMip = widthForSwiz;
		uint32_t heightForMip = heightForSwiz;
		uint32_t pixel = widthForMip * heightForMip;

		for(int i=0; i<mipCount; i++){
			widthForMip /= 2;
			heightForMip /= 2;
			if(widthForMip == 0){
				widthForMip = 1;
			}
			if(heightForMip == 0){
				heightForMip = 1;
			}
			pixel += widthForMip * heightForMip;
		}
		size = ssiu::ceil(pixel * ssgi::getBitPerPixel(texFormat) / 8, SCE_GXM_TEXTURE_FACE_ALIGNMENT) * 6;	//todo
	}
	return size;
}

uint32_t ssgi::getTextureSize(SceGxmTextureType type, SceGxmTextureFormat texFormat, uint32_t width, uint32_t height, uint32_t stride, uint32_t mipCount)
{
	switch(type)
		{
		case SCE_GXM_TEXTURE_SWIZZLED:
			{
				uint32_t widthForSwiz = ceilPowersOf2(width);
				uint32_t heightForSwiz = ceilPowersOf2(height);
				if((widthForSwiz != width) ||  (heightForSwiz != height)){
					return 0;
				}
				if(stride != 0){
					return 0;
				}
				return ssgi::getTextureSwizzledSize(texFormat, width, height, mipCount);
			}
		case SCE_GXM_TEXTURE_CUBE:
			{
				uint32_t widthForSwiz = ceilPowersOf2(width);
				uint32_t heightForSwiz = ceilPowersOf2(height);
				if((widthForSwiz != width) ||  (heightForSwiz != height)){
					return 0;
				}
				if(stride != 0){
					return 0;
				}
				return ssgi::getTextureCubeSize(texFormat, width, height, mipCount);
			}
		case SCE_GXM_TEXTURE_LINEAR:
			{
				if(stride != 0){
					return 0;
				}
				return  ssgi::getTextureLinearSize(texFormat,  width,   height,   mipCount);
			}
		case SCE_GXM_TEXTURE_TILED:					
			{
				return ssgi::getTextureTiledSize(texFormat, width, height, mipCount);
			}
		case SCE_GXM_TEXTURE_SWIZZLED_ARBITRARY:		
			{
				if(stride != 0){
					return 0;
				}
				return ssgi::getTextureSwizzledSize(texFormat, width, height, mipCount)	;
			}
		case SCE_GXM_TEXTURE_LINEAR_STRIDED:
			{
				return height * stride;
			}
		case SCE_GXM_TEXTURE_CUBE_ARBITRARY:			
			{
				if(stride != 0){
					return 0;
				}
				return ssgi::getTextureCubeSize(texFormat, width, height, mipCount);
			}
		default:
			return 0;
		}
}

uint32_t ssgi::getTextureSize(SceGxmTexture *tex)
{
	return ssgi::getTextureSize(sceGxmTextureGetType(tex), 
								sceGxmTextureGetFormat(tex),
								sceGxmTextureGetWidth(tex), 
								sceGxmTextureGetHeight(tex),
								sceGxmTextureGetStride(tex), 
								sceGxmTextureGetMipmapCount(tex));
}

#endif /* _SCE_TARGET_OS_PSP2 */
