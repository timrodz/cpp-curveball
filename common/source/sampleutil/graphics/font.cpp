/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <scebase_common.h>

#include <graphics/graphics_internal.h>
#include "font_internal.h"
#include <string.h>
#include <stdlib.h>
#include <sampleutil_internal.h>
#include <util/util.h>

namespace ssg  = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;

/* ----------------------------------------------------------------------------------- */


ssg::FontParam::FontParam(void)
{
	setDefaults(  );
}

void ssg::FontParam::setDefaults( void )
{
		
	this->language			= kFontLanguageLatin;

	this->family			= kFontFamilySansSerif;

	this->weight               = kFontWeightNormal;
	this->isItalic             = false;

	//this->pixelFormat			= SCE_PVF_USERIMAGE_DIRECT8;
	this->fontSizePixelH		= 64.0f;
	this->fontSizePixelV		= 64.0f;
	//this->emValue				= 72.0f / ( 10.125f * 128.0f );
	//this->resolutionH			= 220.0f;
	//this->resolutionV			= 220.0f;
	//this->emboldenRate			= 0.0f;
	//this->skewValueAngleX		= 0.0f;
	//this->skewValueAngleY		= 0.0f;

	this->baselineOffsetY		= 0.0f;
	//this->advanceOffsetX		= 0.0f;
	//this->advanceOffsetY		= 0.0f;
}

/* ---------------------------------------------------------------------------------------------- */


#pragma region FontTextureAtlas

ssgi::FontTextureAtlas::FontTextureAtlas(void)
{
	m_texture = NULL;
}
ssgi::FontTextureAtlas::~FontTextureAtlas(void)
{
	finalize();
}

uint32_t ssgi::FontTextureAtlas::getWidth(void) const{	return m_textureWidth;	}
uint32_t ssgi::FontTextureAtlas::getHeight(void) const{	return m_textureHeight;	}
uint32_t ssgi::FontTextureAtlas::getStride(void) const{	return m_textureWidth;	}


#define TEX_WIDTH_MAX 4096
//#define TEX_WIDTH_MAX 1024

int ssgi::FontTextureAtlas::initialize(ssg::GraphicsLoader *loader, 
	uint32_t charWidth, uint32_t charHeight,
	uint32_t numCache)
{
	int ret;
	if(charWidth > TEX_WIDTH_MAX){
		m_cpuBuffer = NULL;
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	uint32_t numCharsPerLine = TEX_WIDTH_MAX / charWidth;
	uint32_t numLines = (numCache + numCharsPerLine - 1) / numCharsPerLine;
	if(numLines * charHeight > TEX_WIDTH_MAX){
		m_cpuBuffer = NULL;
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

#if _SCE_TARGET_OS_PSP2
	uint32_t textureWidth  = ssiu::ceil(numCharsPerLine * charWidth, SCE_GXM_TEXTURE_IMPLICIT_STRIDE_ALIGNMENT);
	uint32_t textureHeight = ssiu::ceil(numLines * charHeight, SCE_GXM_TEXTURE_IMPLICIT_STRIDE_ALIGNMENT);
#else
	uint32_t textureWidth  = ssiu::ceil(numCharsPerLine * charWidth, 64);
	uint32_t textureHeight = ssiu::ceil(numLines * charHeight, 64);
#endif

	ret = loader->createTexture(
		&m_texture,
		ssg::kTextureFormatR8Unorm,
		textureWidth,
		textureHeight, 0);
	if(ret != SCE_OK){
		return ret;
	}
	m_texture->setFilter(kTextureFilterLinear, kTextureFilterLinear, kTextureMipFilterDisabled);
	void *p = m_texture->beginWrite();
	memset(p,0, textureWidth*textureHeight);
	m_texture->endWrite();

	m_charWidth = charWidth;
	m_charHeight = charHeight;
	m_numCharsPerLine = numCharsPerLine;
	m_numLines = numLines;
	m_textureWidth = textureWidth;
	m_textureHeight = textureHeight;

#if _SCE_TARGET_OS_WINDOWS
	m_cpuBuffer = (uint8_t*)malloc(textureWidth * textureHeight);
	memset(m_cpuBuffer, 0, textureWidth * textureHeight);
#else
	m_cpuBuffer = NULL;
#endif

	return SCE_OK;
}
int ssgi::FontTextureAtlas::finalize(void)
{
	if(m_texture != NULL){
		sce::SampleUtil::destroy(m_texture);
		m_texture = NULL;
	}
	if(m_cpuBuffer != NULL){
		free(m_cpuBuffer);
		m_cpuBuffer = NULL;
	}
	return SCE_OK;
}
ssg::Texture *ssgi::FontTextureAtlas::getTexture(void) const
{
	return m_texture;
}
int ssgi::FontTextureAtlas::getInfo(uint32_t *outX, uint32_t *outY, float *outU, float *outV, uint32_t index)
{
	if(m_numCharsPerLine * m_numLines <= index){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	uint32_t xIndex = index % m_numCharsPerLine;
	uint32_t yIndex = index / m_numCharsPerLine;
	uint32_t x = xIndex * m_charWidth;
	uint32_t y = yIndex * m_charHeight;
	*outX = x;
	*outY = y;
	*outU = ((float)x)/ m_textureWidth;
	*outV = ((float)y)/ m_textureHeight;
	return SCE_OK;
}

#pragma endregion


#pragma region FontCache

uint32_t ssgi::FontCache::getNewIndex(void)
{
	SCE_SAMPLE_UTIL_ASSERT(m_numChars< m_maxChars);
	uint32_t idx = m_numChars;
	m_numChars++;
	return idx;
}

int ssgi::FontCache::cacheChar(uint16_t ucs2code)
{
	int ret;
	FontCharInfo charInfo;

	

	if(m_charMap.find(ucs2code) != m_charMap.end()){
		// alread cached.
		//printf("- already cached\n");
		return SCE_OK;
	}

	charInfo.bearingX = m_fontData.getCharBearingX(ucs2code);
	charInfo.advanceX = m_fontData.getCharAdvanceX(ucs2code);
	charInfo.isBlank = m_fontData.isBlank(ucs2code);
	charInfo.width    = m_fontData.getCharWidth(ucs2code);

	if(m_fontData.isBlank(ucs2code)){
		charInfo.x = 0;
		charInfo.y = 0;
		charInfo.u = 0.0f;
		charInfo.v = 0.0f;
		charInfo.index = -1;
		m_charMap[ucs2code] = charInfo;
		//printf("- blank\n");
		return SCE_OK;

	}else{
		if(m_numChars >= m_maxChars){
			//printf("- invalide state numChars=%d, maxChars=%d\n", m_numChars, m_maxChars);
			return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
		}
		//printf("cache %c %#x\n", (char)ucs2code, ucs2code);
		uint32_t index = getNewIndex();
		charInfo.index = index;
		
		ret = m_textureAtlas.getInfo(&charInfo.x, &charInfo.y, &charInfo.u, &charInfo.v, index);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		
		if(m_textureAtlas.m_cpuBuffer != NULL){
			ret = m_fontData.rasterize(m_textureAtlas.m_cpuBuffer,
				m_textureAtlas.getWidth(),
				m_textureAtlas.getHeight(),
				m_textureAtlas.getStride(),
				charInfo.x, charInfo.y, ucs2code,
				m_textureAtlas.m_charWidth, m_textureAtlas.m_charHeight);
			uint8_t *image = (uint8_t*)m_textureAtlas.getTexture()->beginWrite();
			memcpy(image, m_textureAtlas.m_cpuBuffer, m_textureAtlas.getHeight()*m_textureAtlas.getStride());
			m_textureAtlas.getTexture()->endWrite();
		}else{
			uint8_t *image = (uint8_t*)m_textureAtlas.getTexture()->beginWrite();
			ret = m_fontData.rasterize(image,
				m_textureAtlas.getWidth(),
				m_textureAtlas.getHeight(),
				m_textureAtlas.getStride(),
				charInfo.x, charInfo.y, ucs2code,
				m_textureAtlas.m_charWidth, m_textureAtlas.m_charHeight		);
			
			m_textureAtlas.getTexture()->endWrite();
		}
		
		m_charMap[ucs2code] = charInfo;
		(void)ret;
		return SCE_OK;
	}
}

ssgi::FontCache::~FontCache(void)
{
	finalize();
}
int ssgi::FontCache::initialize(ssgi::FontLoaderImpl* fontLoader, const ssg::FontParam* param, uint32_t maxChars)
{
	int ret;
	ssg::FontParam defaultParam;
	if(param == NULL){
		param = &defaultParam;
	}

	ret = m_fontData.initialize(fontLoader, param);
	if(ret != SCE_OK){
		return ret;
	}

	ret = m_textureAtlas.initialize(fontLoader->getGraphicsLoader(),
		(uint32_t)param->fontSizePixelH,
		(uint32_t)param->fontSizePixelV,
		maxChars);
	if(ret != SCE_OK){
		m_fontData.finalize();

		return ret;
	}

	m_numChars = 0;
	m_maxChars = maxChars;
	return SCE_OK;
	
}
int ssgi::FontCache::finalize(void)
{
	m_fontData.finalize();
	m_textureAtlas.finalize();
	return SCE_OK;
}

int ssgi::FontCache::getCharInfo(FontCharInfo *charInfo, uint16_t ucs2code)
{
	int ret = cacheChar(ucs2code);
	if(ret != SCE_OK){
		return ret;
	}
	*charInfo = m_charMap[ucs2code];
	return SCE_OK;
}

int ssgi::FontCache::cacheCharacters( const uint16_t* ucs2Charcode )
{
	if( ucs2Charcode == NULL ) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	while( (*ucs2Charcode) != 0 ){
		uint16_t code = *ucs2Charcode;
		int ret = cacheChar(code);
		if(ret != SCE_OK){
			return ret;
		}
		ucs2Charcode++;
	}
	return SCE_OK;
}

void ssgi::FontCache::clearCache(void)
{
	m_numChars = 0;
	m_charMap.clear();
}



#pragma endregion


int ssg::createFontLoader(FontLoader **outFontLoader, GraphicsLoader *loader)
{
	if(outFontLoader == NULL || loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssgi::FontLoaderImpl *fontLoader = new ssgi::FontLoaderImpl;
	int ret = fontLoader->initialize(loader);
	if(ret != SCE_OK){
		delete fontLoader;
		return ret;
	}
	*outFontLoader = fontLoader;
	return SCE_OK;

}
