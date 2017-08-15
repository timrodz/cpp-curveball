/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 *
 *      Copyright (C) 2012 Sony Interactive Entertainment Inc.
 *                        All Rights Reserved.
 *
 */

#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include "graphics/image.h"
#include "graphics/graphics_internal.h"
#include <arm_neon.h>

#pragma comment(lib, "ScePng")
#pragma comment(lib, "SceDeflt")
#pragma comment(lib, "SceJpeg_stub")

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace util = sce::SampleUtil::Internal::Util;

/* ------------------------------------------------------------------------------------------------- */
#pragma region JpegDecoder

ssgi::_JpegDecoder::_JpegDecoder()
{
	m_isInitialized = false;
}

ssgi::_JpegDecoder::~_JpegDecoder()
{
}

int ssgi::_JpegDecoder::initialize(void)
{
	if(m_isInitialized){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	/*E Initialize JPEG decoder. */
	SceJpegMJpegInitParam initParam;
	initParam.size				= sizeof(SceJpegMJpegInitParam);
	initParam.maxSplitDecoder	= 0;
	if(m_useLPDDR2Memory){
		initParam.option		= SCE_JPEG_MJPEG_INIT_OPTION_LPDDR2_MEMORY;
	}else{
		initParam.option		= SCE_JPEG_MJPEG_INIT_OPTION_NONE;
	}
	int ret = sceJpegInitMJpegWithParam(&initParam);
	if (ret < 0) {
		return ret;
	}

	m_isInitialized = true;

	return ret;
}



SceUID ssgi::_JpegDecoder::allocateDecodeMemblock(const uint32_t imageWidth, const uint32_t imageHeight, 
												  void **outFileBuf,   uint32_t *outFileBufSize, 
												  void **outDecodeBuf, uint32_t *outDecodeBufSize)
{
	if(!m_isInitialized){
		return SCE_SAMPLE_UTIL_ERROR_NOT_INITIALIZED;
	}
	SceKernelMemBlockType memBlockType;
	SceSize memBlockAlign;

	if(m_useLPDDR2Memory){
		/*E In JPEG decoder, SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW is
			faster than SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_RW.
			Also, you can not use SCE_KERNEL_MEMBLOCK_TYPE_USER_RW / USER_NC_RW,
			because JPEG decoder needs consecutive areas in a physical address. */
		memBlockType = SCE_KERNEL_MEMBLOCK_TYPE_USER_MAIN_PHYCONT_NC_RW;
		memBlockAlign = 1024 * 1024;
	}else{
		memBlockType = SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW;
		memBlockAlign = 256 * 1024;
	}

	/*E Allocate buffers for JPEG decoder. */
	SceSize streamBufSize = util::ceil(imageWidth * imageHeight, 256);
	SceSize decodeBufSize = util::ceil(imageWidth * imageHeight * 3 / 2, 256);
	SceSize totalBufSize = util::ceil(streamBufSize + decodeBufSize, memBlockAlign);

	SceUID bufferMemBlock = sceKernelAllocMemBlock("jpeg decode", memBlockType, totalBufSize, SCE_NULL);
	SCE_SAMPLE_UTIL_ASSERT(bufferMemBlock >= 0);

	void *fbuf;
	int ret = sceKernelGetMemBlockBase(bufferMemBlock, &fbuf);
	SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
	*outFileBuf = fbuf;
	void *dbuf = (void*)(((uintptr_t)fbuf) + streamBufSize);
	*outDecodeBuf = dbuf;

	*outFileBufSize = streamBufSize;
	*outDecodeBufSize = decodeBufSize;
	
	
	return bufferMemBlock;
}

int ssgi::_JpegDecoder::getOutputInfo(SceJpegOutputInfo *outputInfo, int *outDecodeMode, const void *fileImage, uint32_t fileImageSize)
{
	int decodeMode = SCE_JPEG_MJPEG_WITH_DHT;
	/*E Get JPEG output information. */
	int ret = sceJpegGetOutputInfo((const unsigned char *)fileImage, fileImageSize,
		SCE_JPEG_NO_CSC_OUTPUT, decodeMode, outputInfo);

	if (ret == SCE_JPEG_ERROR_UNSUPPORT_SAMPLING &&
		outputInfo->colorSpace == (SCE_JPEG_CS_YCBCR | SCE_JPEG_CS_H1V1)) {
		/* Set SCE_JPEG_MJPEG_ANY_SAMPLING for decodeMode and retry sceJpegGetOutputInfo(),
		   if the JPEG's color space is YCbCr 4:4:4. */

		decodeMode = SCE_JPEG_MJPEG_ANY_SAMPLING;
		ret = sceJpegGetOutputInfo((unsigned char *)fileImage, fileImageSize,
			SCE_JPEG_NO_CSC_OUTPUT, decodeMode, outputInfo);
	}
	if(ret != SCE_OK){
		return ret;
	}
	
	*outDecodeMode = decodeMode;
	return SCE_OK;
}



#pragma region CSC

#define CSC_FIX(x)	((int)(x * 1024 + 0.5))

/* CSC coefficients for ITU-R BT.601 full-range */
static const int16x4_t s_cscCoef[] = {
	/* y_offset, uv_lvs, y0, alpha */
	{ 0, -128, CSC_FIX(1.000), 255 },
	/* v0, u1, v1, u2 */
	{ CSC_FIX(1.402), -CSC_FIX(0.344), -CSC_FIX(0.714), CSC_FIX(1.772) }
};

/* chroma selector for bilinear upsampling */
static const uint8x8_t s_selBlChroma[] = {
	{  2, 32,  2, 32,  3, 32,  3, 32 }, // x3 - current samples
	{  2, 32,  3, 32,  2, 32,  4, 32 }, // x1 - left edge
	{  1, 32,  3, 32,  2, 32,  4, 32 }, //      middle
	{  1, 32,  3, 32,  2, 32,  3, 32 }  //      right edge
};

__attribute__((noinline))
static void yuv422ToRgba8888(
	uint8_t * __restrict__ pRGBA,
	const uint8_t * __restrict__ pY,
	const uint8_t * __restrict__ pU,
	const uint8_t * __restrict__ pV,
	unsigned int width, unsigned int height,
	unsigned int chromaPitchDiff, unsigned int pitchDiff
)
{
	int16x8_t sq16_temp;
	int16x4_t y0, u, v, next_y0, next_u, next_v;
	int32x4_t y1;
	uint16x4_t y_offset;
	int16x4_t uv_lvs, y_coef, uv_coef;
	int16x4_t r0, g0, b0, r1, g1, b1;
	uint8x8x4_t rgba;
	uint8x8_t u8_temp0, u8_temp1, u8_temp2;
	int16x4_t s16_temp0, s16_temp1;
	uint8x8_t selChromaMain, selChromaSubL, selChromaSubR;
	int x, matrix = 0;

	pitchDiff <<= 2;
	y_offset	= (uint16x4_t)vdup_lane_s16(s_cscCoef[matrix], 0);
	uv_lvs		= vdup_lane_s16(s_cscCoef[matrix], 1);
	y_coef		= vdup_lane_s16(s_cscCoef[matrix], 2);
	rgba.val[3]	= vdup_lane_u8((uint8x8_t)s_cscCoef[matrix], 6);
	uv_coef		= s_cscCoef[matrix + 1];

	pU -= 2;
	pV -= 2;

	selChromaMain = s_selBlChroma[0];
	do {
		x = width;
		selChromaSubL = s_selBlChroma[1];
		do {
			// load
			u8_temp2 = vld1_u8(pY);
			u8_temp0 = vld1_u8(pU);
			u8_temp1 = vld1_u8(pV);
			pY += 8;
			pU += 4;
			pV += 4;
			sq16_temp = (int16x8_t)vmovl_u8(u8_temp2);
			y0 = vget_low_s16(sq16_temp);
			next_y0 = vget_high_s16(sq16_temp);
			y0 = (int16x4_t)vqsub_u16((uint16x4_t)y0, y_offset);
			next_y0 = (int16x4_t)vqsub_u16((uint16x4_t)next_y0, y_offset);
			if (_SCE_LIKELY(x > 8)) {
				selChromaSubR = s_selBlChroma[2];
			} else {
				selChromaSubR = s_selBlChroma[3];
			}
			// upsample U
			s16_temp0 = (int16x4_t)vtbl1_u8(u8_temp0, selChromaMain);
			s16_temp1 = (int16x4_t)vtbl1_u8(u8_temp0, selChromaSubL);
			s16_temp1 = vmla_n_s16(s16_temp1, s16_temp0, 3);
			u = vrshr_n_s16(s16_temp1, 2);
			u8_temp0 = (uint8x8_t)vshr_n_u64((uint64x1_t)u8_temp0, 16);
			s16_temp0 = (int16x4_t)vtbl1_u8(u8_temp0, selChromaMain);
			s16_temp1 = (int16x4_t)vtbl1_u8(u8_temp0, selChromaSubR);
			s16_temp1 = vmla_n_s16(s16_temp1, s16_temp0, 3);
			next_u = vrshr_n_s16(s16_temp1, 2);
			// upsample V
			s16_temp0 = (int16x4_t)vtbl1_u8(u8_temp1, selChromaMain);
			s16_temp1 = (int16x4_t)vtbl1_u8(u8_temp1, selChromaSubL);
			s16_temp1 = vmla_n_s16(s16_temp1, s16_temp0, 3);
			v = vrshr_n_s16(s16_temp1, 2);
			u8_temp1 = (uint8x8_t)vshr_n_u64((uint64x1_t)u8_temp1, 16);
			s16_temp0 = (int16x4_t)vtbl1_u8(u8_temp1, selChromaMain);
			s16_temp1 = (int16x4_t)vtbl1_u8(u8_temp1, selChromaSubR);
			s16_temp1 = vmla_n_s16(s16_temp1, s16_temp0, 3);
			next_v = vrshr_n_s16(s16_temp1, 2);
			// CSC
			y1 = vmull_s16(y0, y_coef);
			u = vadd_s16(u, uv_lvs);
			v = vadd_s16(v, uv_lvs);
			r0 = vqrshrn_n_s32(vmlal_lane_s16(y1, v, uv_coef, 0), 10);
			g0 = vqrshrn_n_s32(vmlal_lane_s16(vmlal_lane_s16(y1, u, uv_coef, 1), v, uv_coef, 2), 10);
			b0 = vqrshrn_n_s32(vmlal_lane_s16(y1, u, uv_coef, 3), 10);
			y1 = vmull_s16(next_y0, y_coef);
			u = vadd_s16(next_u, uv_lvs);
			v = vadd_s16(next_v, uv_lvs);
			r1 = vqrshrn_n_s32(vmlal_lane_s16(y1, v, uv_coef, 0), 10);
			g1 = vqrshrn_n_s32(vmlal_lane_s16(vmlal_lane_s16(y1, u, uv_coef, 1), v, uv_coef, 2), 10);
			b1 = vqrshrn_n_s32(vmlal_lane_s16(y1, u, uv_coef, 3), 10);
			// store
			rgba.val[0] = vqmovun_s16(vcombine_s16(r0, r1));
			rgba.val[1] = vqmovun_s16(vcombine_s16(g0, g1));
			rgba.val[2] = vqmovun_s16(vcombine_s16(b0, b1));
			vst4_u8(pRGBA, rgba);
			x -= 8;
			pRGBA += 4 * 8;
			selChromaSubL = s_selBlChroma[2];
		} while (x > 0);
		pU += chromaPitchDiff;
		pV += chromaPitchDiff;
		pRGBA += pitchDiff;
	} while (--height);

	return;
}

__attribute__((noinline))
static void yuv420ToRgba8888(
	uint8_t * __restrict__ pRGBA,
	const uint8_t * __restrict__ pY,
	const uint8_t * __restrict__ pU,
	const uint8_t * __restrict__ pV,
	unsigned int width, unsigned int height,
	unsigned int chromaPitchDiff, unsigned int pitchDiff
)
{
	int16x8_t sq16_temp;
	int16x4_t y0, u, v, next_y0, next_u, next_v;
	int32x4_t y1;
	uint16x4_t y_offset;
	int16x4_t uv_lvs, y_coef, uv_coef;
	int16x4_t r0, g0, b0, r1, g1, b1;
	uint8x8x4_t rgba;
	const uint8_t * __restrict__ pLastU;
	const uint8_t * __restrict__ pLastV;
	uint8x8_t u8_temp0, u8_temp1, u8_temp2, u8_temp3, u8_temp4;
	int16x4_t s16_temp0, s16_temp1, s16_temp2, s16_temp3;
	uint8x8_t selChromaMain, selChromaSubL, selChromaSubR;
	intptr_t chromaLineSize, chroma2ndLineOffset;
	int x, matrix = 0;

	pitchDiff <<= 2;
	y_offset	= (uint16x4_t)vdup_lane_s16(s_cscCoef[matrix], 0);
	uv_lvs		= vdup_lane_s16(s_cscCoef[matrix], 1);
	y_coef		= vdup_lane_s16(s_cscCoef[matrix], 2);
	rgba.val[3]	= vdup_lane_u8((uint8x8_t)s_cscCoef[matrix], 6);
	uv_coef		= s_cscCoef[matrix + 1];

	pU -= 2;
	pV -= 2;

	chromaLineSize = (width >> 1) + chromaPitchDiff;
	chroma2ndLineOffset = 0;
	pLastU = pU;
	pLastV = pV;
	selChromaMain = s_selBlChroma[0];
	do {
		// load
		u8_temp4 = vld1_u8(pY);
		u8_temp0 = vld1_u8(pU);
		u8_temp1 = vld1_u8(pU + chroma2ndLineOffset);
		u8_temp2 = vld1_u8(pV);
		u8_temp3 = vld1_u8(pV + chroma2ndLineOffset);
		pY += 8;
		pU += 4;
		pV += 4;
		x = width;
		selChromaSubL = s_selBlChroma[1];
		if (_SCE_LIKELY(x > 8)) {
			selChromaSubR = s_selBlChroma[2];
		} else {
			selChromaSubR = s_selBlChroma[3];
		}
		do {
			sq16_temp = (int16x8_t)vmovl_u8(u8_temp4);
			y0 = vget_low_s16(sq16_temp);
			next_y0 = vget_high_s16(sq16_temp);
			y0 = (int16x4_t)vqsub_u16((uint16x4_t)y0, y_offset);
			next_y0 = (int16x4_t)vqsub_u16((uint16x4_t)next_y0, y_offset);
			// upsample U
			s16_temp1 = (int16x4_t)vtbl1_u8(u8_temp0, selChromaSubL);
			s16_temp2 = (int16x4_t)vtbl1_u8(u8_temp1, selChromaMain);
			s16_temp0 = (int16x4_t)vtbl1_u8(u8_temp0, selChromaMain);
			s16_temp3 = (int16x4_t)vtbl1_u8(u8_temp1, selChromaSubL);
			u8_temp0 = (uint8x8_t)vshr_n_u64((uint64x1_t)u8_temp0, 16);
			u8_temp1 = (uint8x8_t)vshr_n_u64((uint64x1_t)u8_temp1, 16);
			s16_temp1 = vadd_s16(s16_temp1, s16_temp2);
			s16_temp3 = vmla_n_s16(s16_temp3, s16_temp0, 9);
			s16_temp3 = vmla_n_s16(s16_temp3, s16_temp1, 3);
			u = vrshr_n_s16(s16_temp3, 4);
			s16_temp1 = (int16x4_t)vtbl1_u8(u8_temp0, selChromaSubR);
			s16_temp2 = (int16x4_t)vtbl1_u8(u8_temp1, selChromaMain);
			s16_temp0 = (int16x4_t)vtbl1_u8(u8_temp0, selChromaMain);
			s16_temp3 = (int16x4_t)vtbl1_u8(u8_temp1, selChromaSubR);
			s16_temp1 = vadd_s16(s16_temp1, s16_temp2);
			s16_temp3 = vmla_n_s16(s16_temp3, s16_temp0, 9);
			s16_temp3 = vmla_n_s16(s16_temp3, s16_temp1, 3);
			next_u = vrshr_n_s16(s16_temp3, 4);
			// upsample V
			s16_temp1 = (int16x4_t)vtbl1_u8(u8_temp2, selChromaSubL);
			s16_temp2 = (int16x4_t)vtbl1_u8(u8_temp3, selChromaMain);
			s16_temp0 = (int16x4_t)vtbl1_u8(u8_temp2, selChromaMain);
			s16_temp3 = (int16x4_t)vtbl1_u8(u8_temp3, selChromaSubL);
			u8_temp2 = (uint8x8_t)vshr_n_u64((uint64x1_t)u8_temp2, 16);
			u8_temp3 = (uint8x8_t)vshr_n_u64((uint64x1_t)u8_temp3, 16);
			s16_temp1 = vadd_s16(s16_temp1, s16_temp2);
			s16_temp3 = vmla_n_s16(s16_temp3, s16_temp0, 9);
			s16_temp3 = vmla_n_s16(s16_temp3, s16_temp1, 3);
			v = vrshr_n_s16(s16_temp3, 4);
			s16_temp1 = (int16x4_t)vtbl1_u8(u8_temp2, selChromaSubR);
			s16_temp2 = (int16x4_t)vtbl1_u8(u8_temp3, selChromaMain);
			s16_temp0 = (int16x4_t)vtbl1_u8(u8_temp2, selChromaMain);
			s16_temp3 = (int16x4_t)vtbl1_u8(u8_temp3, selChromaSubR);
			s16_temp1 = vadd_s16(s16_temp1, s16_temp2);
			s16_temp3 = vmla_n_s16(s16_temp3, s16_temp0, 9);
			s16_temp3 = vmla_n_s16(s16_temp3, s16_temp1, 3);
			next_v = vrshr_n_s16(s16_temp3, 4);
			selChromaSubL = selChromaSubR;
			// CSC
			y1 = vmull_s16(y0, y_coef);
			u = vadd_s16(u, uv_lvs);
			v = vadd_s16(v, uv_lvs);
			r0 = vqrshrn_n_s32(vmlal_lane_s16(y1, v, uv_coef, 0), 10);
			g0 = vqrshrn_n_s32(vmlal_lane_s16(vmlal_lane_s16(y1, u, uv_coef, 1), v, uv_coef, 2), 10);
			b0 = vqrshrn_n_s32(vmlal_lane_s16(y1, u, uv_coef, 3), 10);
			y1 = vmull_s16(next_y0, y_coef);
			u = vadd_s16(next_u, uv_lvs);
			v = vadd_s16(next_v, uv_lvs);
			r1 = vqrshrn_n_s32(vmlal_lane_s16(y1, v, uv_coef, 0), 10);
			g1 = vqrshrn_n_s32(vmlal_lane_s16(vmlal_lane_s16(y1, u, uv_coef, 1), v, uv_coef, 2), 10);
			// store
			rgba.val[0] = vqmovun_s16(vcombine_s16(r0, r1));
			b1 = vqrshrn_n_s32(vmlal_lane_s16(y1, u, uv_coef, 3), 10);
			rgba.val[1] = vqmovun_s16(vcombine_s16(g0, g1));
			rgba.val[2] = vqmovun_s16(vcombine_s16(b0, b1));
			// load
			if (_SCE_LIKELY(x > 8)) {
				u8_temp4 = vld1_u8(pY);
				u8_temp0 = vld1_u8(pU);
				u8_temp1 = vld1_u8(pU + chroma2ndLineOffset);
				u8_temp2 = vld1_u8(pV);
				u8_temp3 = vld1_u8(pV + chroma2ndLineOffset);
				pY += 8;
				pU += 4;
				pV += 4;
			}
			vst4_u8(pRGBA, rgba);
			x -= 8;
			pRGBA += 4 * 8;
			if (_SCE_UNLIKELY(x <= 8)) {
				selChromaSubR = s_selBlChroma[3];
			}
		} while (x > 0);
		if (pLastU != SCE_NULL) {
			// restore U/V ptr to head of current line
			chroma2ndLineOffset = (height > 2)? chromaLineSize: 0;
			pU = pLastU;
			pV = pLastV;
			pLastU = SCE_NULL;
		} else {
			// forward U/V ptr to next line
			chroma2ndLineOffset = -chromaLineSize;
			pU += chromaPitchDiff;
			pV += chromaPitchDiff;
			pLastU = pU;
			pLastV = pV;
		}
		pRGBA += pitchDiff;
	} while (--height);

	return;
}

static
int csc(void *pRGBA, const unsigned char *pYCbCr, int xysize, int iFrameWidth,
	int colorOption, int sampling)
{
	unsigned int width, height, chromaPitchDiff, pitchDiff, ySize, cSize;

	if (pRGBA == SCE_NULL || pYCbCr == SCE_NULL ||
		(((uintptr_t)pRGBA | (uintptr_t)pYCbCr) & 3u)) {
		return SCE_JPEG_ERROR_INVALID_POINTER;
	}
	if (colorOption != SCE_JPEG_PIXEL_RGBA8888) {
		return SCE_JPEG_ERROR_UNSUPPORT_COLORSPACE;
	}

	width	= (unsigned int)xysize >> 16;
	height	= xysize & 0xFFFF;
	if (width == 0 || height == 0 || (width & 7u) || (unsigned int)iFrameWidth < width) {
		return SCE_JPEG_ERROR_UNSUPPORT_IMAGE_SIZE;
	}

	chromaPitchDiff = (width & 16) >> 1;
	pitchDiff = iFrameWidth - width;
	ySize = width * height;
	switch (sampling & 0xFFFF) {
	case SCE_JPEG_CS_H2V1:
		cSize = util::ceil(width, 32) * height >> 1;
		yuv422ToRgba8888(
			(uint8_t * __restrict__)pRGBA,
			(const uint8_t * __restrict__)pYCbCr,
			(const uint8_t * __restrict__)(pYCbCr + ySize),
			(const uint8_t * __restrict__)(pYCbCr + ySize + cSize),
			width, height, chromaPitchDiff, pitchDiff);
		break;
	case SCE_JPEG_CS_H2V2:
		if ((height & 1u)) {
			return SCE_JPEG_ERROR_UNSUPPORT_IMAGE_SIZE;
		}
		cSize = util::ceil(width, 32) * height >> 2;
		yuv420ToRgba8888(
			(uint8_t * __restrict__)pRGBA,
			(const uint8_t * __restrict__)pYCbCr,
			(const uint8_t * __restrict__)(pYCbCr + ySize),
			(const uint8_t * __restrict__)(pYCbCr + ySize + cSize),
			width, height, chromaPitchDiff, pitchDiff);
		break;
	default:
		return SCE_JPEG_ERROR_UNSUPPORT_SAMPLING;
	}

	return 0;
}

#pragma endregion


int ssgi::_JpegDecoder::decodeJpeg(uint32_t *imageData, const void *fileImage, uint32_t fileSize, unsigned char *decodeBuffer, uint32_t decodeBufferSize, const SceJpegOutputInfo *outputInfo, int decodeMode)
{
	/*E Decode JPEG stream */

	int ret = sceJpegDecodeMJpegYCbCr(
		(const unsigned char *)fileImage, fileSize,
		decodeBuffer, decodeBufferSize, decodeMode,
		NULL, 0);

	if (ret < 0) {
		printf("sceJpegDecodeMJpegYCbCr() 0x%08x\n", ret);
		return ret;
	}
	uint32_t alignedWidth = util::ceil(outputInfo->imageWidth, 16);

	int widthHeight = ret;

	/*E CSC (YCbCr -> RGBA) */
	if ((decodeMode & 3) == SCE_JPEG_MJPEG_WITH_DHT) {
		/*E YCbCr 4:2:0 or YCbCr 4:2:2 */
		if((outputInfo->imageHeight >= 64) && (outputInfo->imageWidth >= 64)){
			/* (fast, processed on dedicated hardware) */
			ret = sceJpegMJpegCsc(
				imageData, decodeBuffer, widthHeight, alignedWidth,
				SCE_JPEG_PIXEL_RGBA8888, outputInfo->colorSpace & 0xFFFF);
			
			if (ret < 0) {
				printf("sceJpegMJpegCsc() 0x%08x\n", ret);
				return ret;
			}
		}else{
			/* (slow, processed on the CPU) */
			ret = csc(
				imageData, decodeBuffer, widthHeight, alignedWidth,
				SCE_JPEG_PIXEL_RGBA8888, outputInfo->colorSpace & 0xFFFF);
			if (ret < 0) {
				//EMSG("csc() 0x%08x\n", ret);
				return ret;
			}

		}
	} else {
		/*E YCbCr 4:4:4 (slow, processed on the codec engine) */
		ret = sceJpegCsc(
			imageData, decodeBuffer, widthHeight, alignedWidth,
			SCE_JPEG_PIXEL_RGBA8888, outputInfo->colorSpace & 0xFFFF);

		if (ret < 0) {
			printf("sceJpegCsc() 0x%08x\n", ret);
			return ret;
		}
	}
	return SCE_OK;
}




int ssgi::_JpegDecoder::finalize(void)
{
	if(!m_isInitialized){
		return SCE_SAMPLE_UTIL_ERROR_NOT_INITIALIZED;
	}

	int ret = sceJpegFinishMJpeg();

	if(ret == SCE_OK){
		m_isInitialized = false;
	}

	return ret;
}


#pragma endregion
/* ------------------------------------------------------------------------------------------------- */

#pragma region PngFile

ssgi::PngFile::PngFile(void)
{
	m_imageData = NULL;
}

uint32_t ssgi::PngFile::getWidth(void) const
{
	return (uint32_t)m_width;
}
uint32_t ssgi::PngFile::getHeight(void) const
{
	return (uint32_t)m_height;
}

int ssgi::PngFile::openFromMemory(const void *fileimage, size_t fileimageSize)
{
	int decodeBufSize = scePngGetOutputInfo( (const unsigned char*)fileimage, fileimageSize,
											 &m_width, &m_height, &m_format, &m_streamFormat);
	void *decodeBuf = malloc(decodeBufSize); //TODO
	SCE_SAMPLE_UTIL_ASSERT_MSG( decodeBuf != NULL, "decodeBuf=%#x \n", decodeBuf );

	/*E Decode PNG stream */
	int ret = scePngDec(decodeBuf, decodeBufSize,
					(const unsigned char*)fileimage, fileimageSize, 
					&m_width, &m_height, &m_format);
	if (ret < 0) {
		printf(__FILE__"(%d) Error ret=%#x\n", __LINE__, ret);
		free(decodeBuf);
		return ret;
	}

	uint32_t bytesPerPixelInBuffer = 4;
	uint32_t imageSize = m_width * m_height * bytesPerPixelInBuffer;                 
	m_imageData = (uint32_t *)malloc(imageSize);
	if(m_imageData == NULL){
		printf(__FILE__"(%d) Allocation failed ret=%#x\n", __LINE__, ret);
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}
		
	if (m_format != SCE_PNG_FORMAT_RGBA8888) {
		ret = scePngConvertToRGBA((uint8_t*)m_imageData, decodeBuf,
								  m_width, m_height, m_format);
		SCE_SAMPLE_UTIL_ASSERT_MSG( ret == SCE_OK, "ret=%#x \n", ret );
	}else{
		memcpy(m_imageData, decodeBuf, imageSize);
	}
	free(decodeBuf);
	return SCE_OK;
}

int ssgi::PngFile::open(const char *filename)
{
	int ret;
	uint32_t fileimageSize = 0;

	void *fileimage = util::malloc_and_load(filename, &fileimageSize);
	if(fileimage == NULL){
		printf("Failed to open file \"%s\"\n", filename);
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}
	ret = ssgi::PngFile::openFromMemory(fileimage, fileimageSize);
	free(fileimage);
	return ret;	
}

int ssgi::PngFile::close(void)
{
	if(m_imageData != NULL){
		free(m_imageData);
		m_imageData = NULL;
	}
	return SCE_OK;
}

int ssgi::PngFile::readData(void *data, uint32_t strideInByte) const
{
	for(uint32_t j=0; j<m_height; j++){
		uint32_t *line = (uint32_t*)(((uintptr_t)data) + j*strideInByte);
		for(uint32_t i=0; i<m_width; i++){
			line[i] = m_imageData[j*m_width+i];
		}
	}
	return SCE_OK;
}



#pragma endregion

/* ------------------------------------------------------------------------------------------------- */

#pragma region JpegFile


uint32_t ssgi::JpegFile::getWidth(void) const
{
	return m_jpegOutInfo.imageWidth; //m_width;
}
uint32_t ssgi::JpegFile::getHeight(void) const
{
	return m_jpegOutInfo.imageHeight;  //m_height;
}
uint32_t ssgi::JpegFile::getStride(void) const
{
	return util::ceil(m_jpegOutInfo.imageWidth,16) * 4;   //  m_stride;
}

ssgi::JpegFile::JpegFile(void)
{
	m_jpegDecoder = NULL;
	m_jpegDecoder = NULL;
	m_fileImageSize = 0;
}

static void *mallocAndLoadToCdram(ssgi::GraphicsLoaderImpl *loader, const char* path, uint32_t *pSize)
{
	int ret;

	FILE *fp = fopen(path, "rb");
	if(fp == NULL){
		fprintf(stderr, "Error : Opening %s failed.\n", path);
		return NULL;
	}
	ret = fseek(fp, 0, SEEK_END);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	int32_t size = ftell(fp);
	SCE_SAMPLE_UTIL_ASSERT(size > 0);

	ret = fseek(fp, 0, SEEK_SET);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	void *elf = loader->memalign(loader->MEMTYPE_CDRAM, 256, util::ceil(size, 256));
	if(elf == NULL){
		fprintf(stderr, "Error : No memory. %s couldn't be loaded.\n", path);
		return NULL;
	}

	ret = fread(elf, 1, size, fp);
	SCE_SAMPLE_UTIL_ASSERT(ret == size);

	ret = fclose(fp);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	if(pSize != NULL){
		*pSize = size;
	}

	(void)ret;
	return elf;

}

int ssgi::JpegFile::open(ssgi::GraphicsLoaderImpl *loader, ssgi::_JpegDecoder *jpegDecoder, const char *path)
{
	m_loader = loader;
	m_jpegDecoder = jpegDecoder;
	m_path = path;

	uint32_t size = 0;
	m_fileImage = mallocAndLoadToCdram(loader, path, &size);
	if(m_fileImage == NULL){
		return -1;
	}
	m_fileImageSize = size;

	int ret = m_jpegDecoder->getOutputInfo(&m_jpegOutInfo, &m_decodeMode, m_fileImage, m_fileImageSize);
	if(ret != SCE_OK){
		loader->free(m_fileImage);
		m_fileImage = NULL;
		return ret;
	}

	return SCE_OK;
}

int ssgi::JpegFile::close(void)
{
	if(m_fileImage != NULL){
		m_loader->free(m_fileImage);
		m_loader = NULL;
		m_fileImage = NULL;
		m_fileImageSize = 0;
	}
	return SCE_OK;
}


int ssgi::JpegFile::allocateDecodeBuffer(DecodeBuffer *buf)
{
	SCE_SAMPLE_UTIL_ASSERT(buf != NULL);
	memset(buf, 0, sizeof(DecodeBuffer));

	uint32_t width  = getWidth();
	uint32_t height = getHeight();
	uint32_t alignedWidth = util::ceil(width ,16);
	uint32_t alignedHeight = util::ceil(width ,16);
	buf->ycbcrSize = m_jpegOutInfo.outputBufferSize;

	buf->ycbcr = m_loader->memalign(m_loader->MEMTYPE_CDRAM, 256, buf->ycbcrSize);
	if(buf->ycbcr == NULL){
		return -1;
	}

	if(height != alignedHeight){
		buf->tmpRgbSize = alignedWidth * alignedHeight * 4;
		buf->tmpRgb = m_loader->memalign(m_loader->MEMTYPE_CDRAM, 256, buf->tmpRgbSize);
		if(buf->tmpRgb == NULL){
			freeDecodeBuffer(buf);
			return -1;
		}
	}else{
		buf->tmpRgbSize = 0;
		buf->tmpRgb     = 0;
	}

	return SCE_OK;
}
int ssgi::JpegFile::freeDecodeBuffer(DecodeBuffer *buf)
{
	if(buf->ycbcr != NULL){
		m_loader->free(buf->ycbcr);
	}
	if(buf->tmpRgb != NULL){
		m_loader->free(buf->tmpRgb);
	}
	memset(buf, 0, sizeof(DecodeBuffer));
	return SCE_OK;
}

int ssgi::JpegFile::readData(void *imageData)
{
	DecodeBuffer buf;
	int ret = allocateDecodeBuffer(&buf);
	if(ret != SCE_OK){
		return ret;
	}

	bool isAligned = (getHeight() % 16) == 0;

	void *outputBuf = isAligned ? imageData : buf.tmpRgb;

	ret = m_jpegDecoder->decodeJpeg((uint32_t*)outputBuf, m_fileImage, m_fileImageSize, 
		(unsigned char *)buf.ycbcr, buf.ycbcrSize, &m_jpegOutInfo, m_decodeMode);
	if (ret < 0) {
		freeDecodeBuffer(&buf);
		return ret;
	}

	if(!isAligned){
		memcpy(imageData, outputBuf, util::ceil(getWidth(), 16) * getHeight() * 4);
	}

	freeDecodeBuffer(&buf);
	return SCE_OK;
}

#pragma endregion

/* ----------------------------------------------------------------------------------------- */

#pragma region GxtFile

ssgi::GxtFile::GxtFile(void)
{
	m_freeImage=false;
	m_image = NULL;
}

ssgi::GxtFile::~GxtFile(void)
{
	close();
}
	
int ssgi::GxtFile::openFromMemory(const void* gxtImage, size_t gxtImageSize)
{
	if( sceGxtCheckData(gxtImage) != SCE_OK ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	m_image = memalign(16, gxtImageSize);
	memcpy(m_image, gxtImage, gxtImageSize);
	m_freeImage = true;

	int ret = sceGxtInitTexture(&m_texture, gxtImage, sceGxtGetDataAddress(m_image), 0);
	if(ret != SCE_OK){
		printf(__FILE__"(%d) Error: ret=%#x\n", __LINE__, ret);
		return ret;
	}
	return SCE_OK;
}

int ssgi::GxtFile::open(const char * filename)
{
	uint32_t size = 0;
	void *image = util::malloc_and_load(filename, &size);
	int ret = openFromMemory(image, size);
	if(ret != SCE_OK){
		free((void*)image);
		return ret;
	}
	m_freeImage = true;
	return SCE_OK;
}

int ssgi::GxtFile::close(void)
{
	if(m_freeImage){
		if(m_image != NULL){
			free((void*)m_image);
			m_image = NULL;
		}
	}else{
		m_image = NULL;
	}
	return SCE_OK;
}

#pragma endregion


#endif /* _SCE_TARGET_OS_PSP2 */

