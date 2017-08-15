/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include "font_win.h"
#include "graphics/font_internal.h"
#include "util/util.h"

namespace ssg  = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;

#  if _MSC_VER < 1800

static inline float fmaxf(float a, float b)
{
	return (a>b) ? a : b;
}
#  endif 


ssgi::FontData::FontData(void)
{
	m_font = NULL;
}

ssgi::FontData::~FontData(void)
{
	finalize();
}

void ssgi::FontData::estimateMaxCharSize(void)
{
	float maxHeight = 0.0f;
	float maxDepth  = 0.0f;
	float maxWidth  = 0.0f;
	// Estimate texture size
	char alphabet[] = "abcdefghijklnmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	{
		char *code = &alphabet[0];
		while( *code != 0){
			uint16_t c = (uint16_t)(*code);
			GLYPHMETRICS glyphMetrics;
			DWORD ret = getGlyph(&glyphMetrics, c, NULL, 0);
			if(ret == GDI_ERROR){
				return;
			}

			float height = fabs((float)glyphMetrics.gmptGlyphOrigin.y);
			float depth  = fabs((float)glyphMetrics.gmBlackBoxY - (float)glyphMetrics.gmptGlyphOrigin.y);
			float width  = fabs((float)glyphMetrics.gmBlackBoxX);

			maxHeight = sce::SampleUtil::fmax(maxHeight, height);
			maxDepth  = sce::SampleUtil::fmax(maxDepth, depth);
			maxWidth  = sce::SampleUtil::fmax(maxWidth, width);
			code++;
		}
	}

	uint16_t ucs2aiueo[] = { 0x3042, // a
		0x3044, //i 
		0x3046, // u
		0x3048, //e
		0x304a, //o
		0x30DD, //po
		0 };
	{
		uint16_t *code = &ucs2aiueo[0];
		while( *code != 0){
			uint16_t c = *code;

			GLYPHMETRICS glyphMetrics;
			DWORD ret = getGlyph(&glyphMetrics, c, NULL, 0);
			if(ret == GDI_ERROR){
				return;
			}

			float height = fabs((float)glyphMetrics.gmptGlyphOrigin.y);
			float depth  = fabs((float)glyphMetrics.gmBlackBoxY - (float)glyphMetrics.gmptGlyphOrigin.y);
			float width  = fabs((float)glyphMetrics.gmBlackBoxX);

			maxHeight = sce::SampleUtil::fmax(maxHeight, height);
			maxDepth  = sce::SampleUtil::fmax(maxDepth, depth);
			maxWidth  = sce::SampleUtil::fmax(maxWidth, width);

			code++;
		}
	}
	float fontHeight  = maxHeight + maxDepth;

	DeleteObject(m_font);


	int fnWeight;
	switch(m_fontParam.weight)
	{
	case kFontWeightNarrow: { fnWeight = FW_THIN; break; }
	case kFontWeightBold: { fnWeight = FW_BOLD; break; }
	case kFontWeightBlack:{ fnWeight = FW_BLACK; break; }
	default:  { fnWeight = FW_NORMAL; break;} 
	}

	DWORD family;
	switch(m_fontParam.weight)
	{
	case kFontFamilySerif: { family = FF_ROMAN; break; }
	case kFontFamilyRounded: {family = FF_SCRIPT; break; }
	default:  { family = FF_SWISS;break; }
	}

	m_font =  CreateFont(
		m_fontParam.fontSizePixelH * m_fontParam.fontSizePixelH / fontHeight, // int nHeight,
		0.0, // int nWidth,
		0, // left to right, int nEscapement
		0, // int nOrientation,
		fnWeight, //int fnWeight,
		m_fontParam.isItalic, // DWORD fdwItalic,
		false, // DWORD fdwUnderline,
		false, // DWORD fdwStrikeOut,
		DEFAULT_CHARSET, //DWORD fdwCharSet,
		OUT_DEFAULT_PRECIS, // DWORD fdwOutputPrecision,
		CLIP_DEFAULT_PRECIS, // DWORD fdwClipPrecision,
		DEFAULT_QUALITY, // DWORD fdwQuality,
		DEFAULT_PITCH | family, // DWORD fdwPitchAndFamily,
		NULL // LPCTSTR lpszFace
		);


	if( m_fontParam.baselineOffsetY == 0.0f ){
		m_baselineOffset =maxHeight * (m_fontParam.fontSizePixelH / fontHeight);
	}else{
		m_baselineOffset = m_fontParam.baselineOffsetY;
	}

	//sce::SampleUtil::Impl::debugPrintfln("m_fontParam.fontSizePixelH=%f, m_baselineOffset=%f\n", m_fontParam.fontSizePixelH, m_baselineOffset);
	m_fontHeight = fontHeight;


}

int ssgi::FontData::initialize(ssgi::FontLoaderImpl* fontLoader, const ssg::FontParam* param )
{
	m_fontLoader = fontLoader;
	m_fontParam.setDefaults();
	if(param != NULL){
		m_fontParam = *param;
	}

	int fnWeight;
	switch(param->weight)
	{
	case kFontWeightNarrow: { fnWeight = FW_THIN; break; }
	case kFontWeightBold: { fnWeight = FW_BOLD; break; }
	case kFontWeightBlack:{ fnWeight = FW_BLACK; break; }
	default:  { fnWeight = FW_NORMAL; break;} 
	}

	DWORD family;
	switch(param->weight)
	{
	case kFontFamilySerif: { family = FF_ROMAN; break; }
	case kFontFamilyRounded: {family = FF_SCRIPT; break; }
	default:  { family = FF_SWISS;break; }
	}


	m_font =  CreateFont(
		m_fontParam.fontSizePixelH, // int nHeight,
		0.0, // int nWidth,
		0, // left to right, int nEscapement
		0, // int nOrientation,
		fnWeight, //int fnWeight,
		param->isItalic, // DWORD fdwItalic,
		false, // DWORD fdwUnderline,
		false, // DWORD fdwStrikeOut,
		DEFAULT_CHARSET, //DWORD fdwCharSet,
		OUT_DEFAULT_PRECIS, // DWORD fdwOutputPrecision,
		CLIP_DEFAULT_PRECIS, // DWORD fdwClipPrecision,
		DEFAULT_QUALITY, // DWORD fdwQuality,
		DEFAULT_PITCH | family, // DWORD fdwPitchAndFamily,
		NULL // LPCTSTR lpszFace
		);

	if(m_font == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	estimateMaxCharSize();
	return SCE_OK;
}

int ssgi::FontData::finalize(void)
{
	if(m_font != NULL){
		DeleteObject(m_font);
		m_font = NULL;
	}
	return SCE_OK;
}
uint32_t ssgi::FontData::getFontHeight(void) const
{
	return m_fontParam.fontSizePixelH;
}

float ssgi::FontData::getBaselineOffset(void) const
{
	return m_baselineOffset;
}

DWORD ssgi::FontData::getGlyph(GLYPHMETRICS *gm, UINT uChar, uint8_t *buffer, uint32_t bufferSize) const
{
	HDC displayDeviceContext = m_fontLoader->getDisplayDeviceContext();
	HFONT font = (HFONT)SelectObject(displayDeviceContext, m_font);
	MAT2 mat = {{0,1}, {0,0}, {0,0}, {0,1}};

	memset(gm, 0, sizeof(GLYPHMETRICS));
	if((buffer != NULL) && (bufferSize > 0)){
		memset(buffer, 0, bufferSize);
	}
	return GetGlyphOutlineW(
		displayDeviceContext,             
		uChar    ,
		GGO_GRAY8_BITMAP, // UINT uFormat,
		gm, //PGLYPHMETRICS lpgm,
		bufferSize, // DWORD cbBuffer,
		buffer, //LPVOID lpvBuffer,
		&mat //CONST MAT2 *lpmat2
		);

}

bool ssgi::FontData::isBlank(uint16_t ucs2Charcode) const
{
	GLYPHMETRICS glyphMetrics;
	DWORD ret = getGlyph(&glyphMetrics, ucs2Charcode, NULL, 0);
	if(ret == GDI_ERROR){
		return true;
	}
	return glyphMetrics.gmBlackBoxX == 0;
}

float ssgi::FontData::getCharBearingX(uint16_t ucs2Charcode) const
{
	GLYPHMETRICS glyphMetrics;
	DWORD ret = getGlyph(&glyphMetrics, ucs2Charcode, NULL, 0);
	if(ret == GDI_ERROR){
		return 0.0f;
	}
	return glyphMetrics.gmptGlyphOrigin.x;
}

uint32_t ssgi::FontData::getCharAdvanceX(uint16_t ucs2Charcode) const
{
	GLYPHMETRICS glyphMetrics;
	DWORD ret = getGlyph(&glyphMetrics, ucs2Charcode, NULL, 0);
	if(ret == GDI_ERROR){
		return 0;
	}
	return glyphMetrics.gmCellIncX;
}

static uint8_t getFontValue(const GLYPHMETRICS *gm, const uint8_t *buf, uint32_t x, uint32_t y)
{
	if(gm->gmBlackBoxX < x){
		return 0;
	}
	if(gm->gmBlackBoxY < y){
		return 0;
	}
	uint8_t v = buf[ ssiu::ceil(gm->gmBlackBoxX, 4) * y + x];
	return (v * 255) / 64;
}

int ssgi::FontData::rasterize(uint8_t *image, 
	uint32_t imageWidth, uint32_t imageHeight, uint32_t imageStride,
	uint32_t x, uint32_t y, uint16_t ucs2Charcode,
	uint32_t clipWidth,
	uint32_t clipHeight	)
{
	// Clear clip rect.
	for(uint32_t fy=0; fy<clipHeight; fy++){
		for(uint32_t fx=0; fx<clipWidth; fx++){
			uint32_t iy = y+fy;
			uint32_t ix = x+fx;
			if(iy >= imageHeight){
				continue;
			}
			if(ix >= imageWidth){
				continue;
			}
			image[iy*imageStride + ix] = 0;
		}
	}

	if(ucs2Charcode == 0x20){ // space character
		return SCE_OK; // workaround
	}
	if(ucs2Charcode == 0x3000){ // two-byte space character
		return SCE_OK; // workaround
	}


	// Get Glyph
	GLYPHMETRICS glyphMetrics;
	DWORD size = getGlyph(&glyphMetrics, ucs2Charcode, NULL, 0);
	if(size == GDI_ERROR){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	uint8_t *buf = (uint8_t*)malloc(size);
	memset(buf, 0, size);
	
	DWORD res = getGlyph(&glyphMetrics, ucs2Charcode, buf, size);
	if(size == GDI_ERROR){
		free(buf);
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	// Copy glyph
	for(uint32_t fy=0; fy<glyphMetrics.gmBlackBoxY; fy++){
		if(fy>clipHeight){
			continue;
		}
		for(uint32_t fx=0; fx<glyphMetrics.gmBlackBoxX; fx++){
			if(fx>clipWidth){
				continue;
			}

			uint32_t ix = x + fx;
			uint32_t iy = y + fy + getBaselineOffset() - glyphMetrics.gmptGlyphOrigin.y;
			if(ix >= imageWidth){
				continue;
			}
			if(iy >= imageHeight){
				continue;
			}
			image[iy*imageStride + ix] = getFontValue(&glyphMetrics, buf, fx, fy);
		}
	}
	free(buf);
	return SCE_OK;
}
float ssgi::FontData::getCharWidth(uint16_t ucs2Charcode) const
{
	GLYPHMETRICS glyphMetrics;
	DWORD ret = getGlyph(&glyphMetrics, ucs2Charcode, NULL, 0);
	if(ret == GDI_ERROR){
		return true;
	}
	return glyphMetrics.gmBlackBoxX;
}


/* ----------------------------------------- */




int ssgi::FontLoaderImpl::createFont( ssg::Font **outFont, const ssg::FontParam* param, uint32_t numCache)
{
	if(outFont == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	FontImpl2 *impl = new FontImpl2;
	int ret = impl->initialize(this, param, numCache);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outFont = impl;
	return SCE_OK;
}


HDC ssgi::FontLoaderImpl::getDisplayDeviceContext(void) const
{
	sce::SampleUtil::Graphics::Impl::GraphicsContextImpl *context = dynamic_cast<sce::SampleUtil::Graphics::Impl::GraphicsContextImpl *>(m_loader);
	if(context == NULL){
		return NULL;
	}
	return context->getDisplayDeviceContext();
}


#endif

