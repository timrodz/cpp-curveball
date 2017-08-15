/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2
#pragma comment(lib, "ScePvf_stub")

#include "font_psp2.h"
#include "graphics/font_internal.h"
#include "util/util.h"

namespace ssg  = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;

static bool isLatin(uint16_t ucs2Charcode)
{
	return (ucs2Charcode <= 0x00ff) || ((0x0400 <= ucs2Charcode) && (ucs2Charcode <= 0x04ff));
}



#pragma region PvfFont

int ssgi::PvfFont::initialize(ScePvf_t_initRec	*pvfInitParams, ScePvf_t_fontStyleInfo* styleInfo, uint32_t accessMode)
{
	int ret;
	ScePvf_t_libId libId = scePvfNewLib( pvfInitParams, &ret );
	if( ret != SCE_OK ){
		return ret;
	}

	ScePvf_t_fontIndex fontIndex = scePvfFindOptimumFont( libId, styleInfo, &ret );
    if( ret != SCE_OK ){
		scePvfDoneLib( libId );
		return ret;
    }

	ScePvf_t_fontId fontId = scePvfOpen( libId, fontIndex, accessMode, &ret );
	if( ret != SCE_OK ){
		scePvfDoneLib( libId );
		return ret;
	}

	m_libId = libId;
	m_fontId = fontId;
	return SCE_OK;

}

void ssgi::PvfFont::setup(float	fontSizePixelH,
					float	fontSizePixelV)
{
	int	ret	= SCE_OK;

	float emValue				= 72.0f / ( 10.125f * 128.0f );
	float resolutionH			= 220.0f;
	float resolutionV			= 220.0f;
	float emboldenRate			= 0.0f;
	float skewValueAngleX		= 0.0f;
	float skewValueAngleY		= 0.0f;
	

	ret = scePvfSetResolution( m_libId, resolutionH, resolutionV );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = scePvfSetEM( m_libId, emValue );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	float pvfFontSizePointH	= scePvfPixelToPointH( m_libId, fontSizePixelH, &ret );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	float pvfFontSizePointV	= scePvfPixelToPointV( m_libId, fontSizePixelV, &ret );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = scePvfSetCharSize( m_fontId, pvfFontSizePointH, pvfFontSizePointV );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	// scePvfSetEmboldenRate および scePvfSetSkewValue を使用した場合、
	// glyphMetrics の値は変更されないため、設定値に従って下記の値を算出する必要あり。
	// （pvfGetCharGlyphImage内の算出部分を改修）
	//   - width64
	//   - height64
	//   - horizontalBearingX64
	//   - horizontalBearingY64

	ret = scePvfSetEmboldenRate( m_fontId, emboldenRate );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = scePvfSetSkewValue( m_fontId, skewValueAngleX, skewValueAngleY );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	(void)ret;
}

void ssgi::PvfFont::setup(const ssg::FontParam *fontParam)
{
	setup(
		fontParam->fontSizePixelH,
		fontParam->fontSizePixelV);
		//fontParam->emValue,
		//fontParam->resolutionH,
		//fontParam->resolutionV,
		//fontParam->emboldenRate,
		//fontParam->skewValueAngleX,
		//fontParam->skewValueAngleY);
}



void ssgi::PvfFont::getCharInfo(ScePvf_t_charInfo* charInfo, uint16_t ucs2Charcode)
{
	int ret = scePvfGetCharInfo( m_fontId, ucs2Charcode, charInfo );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}

int ssgi::PvfFont::getGlyphImage(ScePvf_t_userImageBufferRec *buffer, uint16_t ucs2Charcode)
{
	return scePvfGetCharGlyphImage(  m_fontId, ucs2Charcode, buffer );
}

int ssgi::PvfFont::getGlyphImage(ScePvf_t_userImageBufferRec *buffer, uint16_t ucs2Charcode,
	int32_t clipX,
	int32_t clipY,
	uint32_t clipWidth,
	uint32_t clipHeight)
{
	return scePvfGetCharGlyphImageClip(  m_fontId, ucs2Charcode, buffer ,
		 clipX,
		 clipY,
		 clipWidth,
		clipHeight		);
	
}

int ssgi::PvfFont::finalize(void)
{
	int ret;
	if(m_fontId != NULL){
		ret = scePvfClose( m_fontId );
		SCE_SAMPLE_UTIL_ASSERT(ret >= SCE_OK);
		m_fontId = NULL;
	}
	if(m_libId!= NULL){
		ret = scePvfDoneLib( m_libId );
		SCE_SAMPLE_UTIL_ASSERT(ret >= SCE_OK);
		m_libId = NULL;
	}
	(void)ret;
	return SCE_OK;
}

#pragma endregion

#pragma region FontLoaderImpl
 
int ssgi::FontLoaderImpl::initialize(GraphicsLoader *loader)
{
	clear();
	m_loader = loader;
	return SCE_OK;
}

ssg::GraphicsLoader *ssgi::FontLoaderImpl::getGraphicsLoader(void)
{
	return m_loader;
}

int
ssgi::FontLoaderImpl::finalize()
{
	clear();
	return SCE_OK;
}


ssgi::PvfFont *ssgi::FontLoaderImpl::openFontFile( ScePvf_t_fontStyleInfo* styleInfo, uint32_t accessMode, int* errorCode )
{
	if( styleInfo == NULL || errorCode == NULL ){
		if( errorCode ){
			*errorCode = SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
		}
		return NULL;
	}

	*errorCode		= SCE_OK;

	FontTableKey key;
	key.m_styleInfo = *styleInfo;
	key.accessMode  = accessMode;
	if(m_fontTable.find( key ) != m_fontTable.end() ){
		FontTableValue val = m_fontTable[key];
		val.referenceCount++;
		m_fontTable[key] = val;
		return val.fontInfo;
	}


    ScePvf_t_initRec	pvfInitParams;
	memset( &pvfInitParams, 0, sizeof( pvfInitParams ) );

	pvfInitParams.userData			= NULL;
	pvfInitParams.maxNumFonts		= 1;
	pvfInitParams.cache				= NULL;
	pvfInitParams.allocFunc			= cbAllocFunc;
	pvfInitParams.reallocFunc		= cbReallocFunc;
	pvfInitParams.freeFunc			= cbFreeFunc;

	ssgi::PvfFont *pvfFont = new ssgi::PvfFont;
	int ret = pvfFont->initialize(&pvfInitParams, styleInfo, accessMode);
	if(ret != SCE_OK){
		delete pvfFont;
		return NULL;
	}

	FontTableValue val;
	val.fontInfo = pvfFont;
	val.referenceCount	= 1;
	m_fontTable[ key ]	= val;

	return val.fontInfo;
}


int ssgi::FontLoaderImpl::closeFontFile( ssgi::PvfFont *fontInfo )
{
	FontTable::iterator it = m_fontTable.begin();
	while(it != m_fontTable.end())
	{
		FontTableValue val = it->second;
		if( val.fontInfo == fontInfo ){
			val.referenceCount--;
			if(val.referenceCount > 0){
				m_fontTable[it->first] = val;
			}else{
				val.fontInfo->finalize();
				m_fontTable.erase(it);
				delete val.fontInfo;
			}
			return SCE_OK;
		}
		++it;
	}
	return -1;
}

void ssgi::FontLoaderImpl::clear(void)
{
	FontTable::iterator it = m_fontTable.begin();
	while(it != m_fontTable.end())
	{
		FontTableValue val = it->second;
		val.fontInfo->finalize();
		delete val.fontInfo;
		++it;
	}
	m_fontTable.clear();
}



/* for libpvf callback function */
void* ssgi::FontLoaderImpl::cbAllocFunc( void *userData, uint32_t size )
{
	(void)userData;
	return malloc(size);
}

void ssgi::FontLoaderImpl::cbFreeFunc( void *userData, void *p )
{
	(void)userData;
	free(p);
}

void* ssgi::FontLoaderImpl::cbReallocFunc( void *userData, void *p, uint32_t size )
{
	(void)userData;
	return realloc(p, size);
}

int ssgi::FontLoaderImpl::createFont( Font **outFont, const FontParam* param, uint32_t numMaxCache)
{
	if(outFont == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	FontImpl2 *impl = new FontImpl2;
	int ret = impl->initialize(this, param, numMaxCache);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outFont = impl;
	return SCE_OK;

}


#pragma endregion

#pragma region FontData

static inline bool between(uint16_t low, uint16_t c, uint16_t high)
{
	return (low<=c) && (c <= high);
}

static bool isJapanese(uint16_t c)
{
	//Number Forms (U+2150~U+218F)
	if(between(0x2150,c, 0x218F)) { return true; }
	//Arrows (U+2190~U+21FF)
	if(between(0x2190,c, 0x21FF)) { return true; }
	//Mathematical Operators (U+2200~U+22FF)
	if(between(0x2200,c, 0x22FF)) { return true; }
	//Miscellaneous Technical (U+2300~U+23FF)
	if(between(0x2300,c, 0x23FF)) { return true; }
	//Enclosed Alphanumerics (U+2460~U+24FF)
	if(between(0x2460,c, 0x24FF)) { return true; }
	//Box Drawing (U+2500~U+257F)
	if(between(0x2500,c, 0x257F)) { return true; }
	//Block Elements (U+2580~U+259F)
	if(between(0x2580,c, 0x259F)) { return true; }
	//Geometric Shapes (U+25A0~U+25FF)
	if(between(0x25A0,c, 0x25FF)) { return true; }
	//Miscellaneous Symbols (U+2600~U+26FF)
	if(between(0x2600, c, 0x26FF)) { return true; }
	//Dingbats (U+2700~U+27BF)
	if(between(0x2700, c, 0x27BF)) { return true; }
	//Kangxi Radicals (U+2F00~U+2FDF)
	if(between(0x2700, c, 0x27BF)) { return true; }
	//CJK Symbols and Punctuation (U+3000~U+303F)
	if(between(0x3000, c, 0x303F)) { return true; }
	//Hiragana (U+3040~U+309F)
	if(between(0x3040, c, 0x309F)) { return true; }
	//Katakana (U+30A0~U+30FF)
	if(between(0x3040, c, 0x309F)) { return true; }
	//Enclosed CJK Letters and Months (U+3200~U+32FF)
	if(between(0x3200, c, 0x32FF)) { return true; }
	//CJK Compatibility (U+3300~U+33FF)
	if(between(0x3300, c, 0x33FF)) { return true; }
	//CJK Unified Ideographs U+4E00˜U+9FFF
	if(between(0x4E00, c, 0x9FFF)) { return true; }
	//CJK Compatibility Forms (U+FE30~U+FE4F)
	if(between(0xFE30, c, 0xFE4F)) { return true; }
	//Halfwidth and Fullwidth Forms (U+FF00~U+FFEF)
	if(between(0xFF00, c, 0xFFEF)) { return true; }
	return false;
}

static bool isKorean(uint16_t c)
{
	//Hangul Compatibility Jamo (U+3130~U+318F)
	if(between(0x3130, c, 0x318F)) { return true; }
	//Enclosed CJK Letters and Months (U+3200~U+32FF)
	if(between(0x3200, c, 0x32FF)) { return true; }
	//Hangul Syllables (U+AC00~˜U+D7AF)
	if(between(0xAC00, c, 0xD7AF)) { return true; }
	//Greek and Coptic (U+0370~U+03FF)
	if(between(0x0370, c, 0x03FF)) { return true; }
	//Cyrillic (U+0400~U+04FF)
	if(between(0x0400, c, 0x04FF)) { return true; }
	return false;
}

static bool isChinese(uint16_t c)
{
	//Basic Latin (U+0000~U+007F)
	if(between(0x0000, c, 0x007F)) { return true; }
	//Latin-1 Supplement (U+0080~U+00FF)
	if(between(0x0080, c, 0x00FF)) { return true; }
	//Latin Extended-A (U+0100~U+017F)
	if(between(0x0100, c, 0x017F)) { return true; }
	//Latin Extended-B (U+0180~U+024F)
	if(between(0x0180, c, 0x024F)) { return true; }
	//Arabic (U+0600~U+06FF)
	if(between(0x0600, c, 0x06FF)) { return true; }
	//Tibetan (U+0F00~U+0FFF)
	if(between(0x0F00, c, 0x0FFF)) { return true; }

	//Mongolian (U+1800~U+18AF)
	if(between(0x1800, c, 0x18AF)) { return true; }
	//General Punctuation (U+2000~U+206F)
	if(between(0x2000, c, 0x206F)) { return true; }
	//Letterlike Symbols (U+2100~U+214F)
	if(between(0x2100, c, 0x214F)) { return true; }

	//Number Forms (U+2150~U+218F)
	if(between(0x2150, c, 0x218F)) { return true; }
	//Arrows (U+2190~U+21FF)
	if(between(0x2190, c, 0x21FF)) { return true; }
	//Mathematical Operators (U+2200~U+22FF)
	if(between(0x2200, c, 0x22FF)) { return true; }

	//Enclosed Alphanumerics (U+2460~U+24FF)
	if(between(0x2460, c, 0x24FF)) { return true; }
	//Box Drawing (U+2500~U+257F)
	if(between(0x2500, c, 0x257F)) { return true; }

	//Block Elements (U+2580~U+259F)
	if(between(0x2580, c, 0x259F)) { return true; }

	//Geometric Shapes (U+25A0~U+25FF)
	if(between(0x25A0, c, 0x25FF)) { return true; }
	//Miscellaneous Symbols (U+2600~U+26FF)
	if(between(0x2600, c, 0x26FF)) { return true; }

	//CJK Radicals Supplement (U+2E80~U+2EFF)
	if(between(0x2E80, c, 0x2EFF)) { return true; }
	//Kangxi Radicals (U+2F00~U+2FDF)
	if(between(0x2F00, c, 0x2FDF)) { return true; }
	//Ideographic Description Characters (U+2FF0~U+2FFF)
	if(between(0x2FF0, c, 0x2FFF)) { return true; }
	//CJK Symbols and Punctuation (U+3000~U+303F)
	if(between(0x3000, c, 0x303F)) { return true; }
	//Hiragana (U+3040~U+309F)
	if(between(0x3040, c, 0x309F)) { return true; }
	//Katakana (U+30A0~U+30FF)
	if(between(0x30A0, c, 0x30FF)) { return true; }

	//Bopomofo (U+3100~U+312F)
	if(between(0x3100, c, 0x312F)) { return true; }
	//Kanbun (U+3190~U+319F)
	if(between(0x3190, c, 0x319F)) { return true; }
	//Enclosed CJK Letters and Months (U+3200~U+32FF)
	if(between(0x3200, c, 0x32FF)) { return true; }
	//CJK Compatibility (U+3300~U+33FF)
	if(between(0x3300, c, 0x33FF)) { return true; }
	//CJK Unified Ideographs Extension A (U+3400~U+4DBF)
	if(between(0x3400, c, 0x4DBF)) { return true; }
	
	//CJK Unified Ideographs (U+4E00~U+4FFF)
	if(between(0x4E00, c, 0x9FFF)) { return true; }

	//Yi Syllables (U+A000~U+A48F)
	if(between(0xA000, c, 0xA48F)) { return true; }
	//Yi Radicals (U+A490~U+A4CF)
	if(between(0xA490, c, 0xA4CF)) { return true; }
	//Private Use Area (U+E000~U+F8FF)
	if(between(0xE000, c, 0xF8FF)) { return true; }
	//CJK Compatibility Ideographs (U+F900~U+FAFF)
	if(between(0xF900, c, 0xFAFF)) { return true; }
	//CJK Compatibility Forms (U+FE30~U+FE4F)
	if(between(0xFE30, c, 0xFE4F)) { return true; }
	//Small Form Variants (U+FE50~U+FE6F)
	if(between(0xFE50, c, 0xFE6F)) { return true; }
	//Halfwidth and Fullwidth Forms (U+FF00~U+FFEF)
	if(between(0xFF00, c, 0xFFEF)) { return true; }
	return false;
}


ssgi::PvfFont *ssgi::FontData::chooseLang(uint16_t ucs2code) const
{
	if(m_fontParam.language == kFontLanguageLatin){
		if(isLatin(ucs2code)){
			return m_pvfFontLatin;
		}
	}else if(m_fontParam.language == kFontLanguageJapanese){
		if(isJapanese(ucs2code)){
			return m_pvfFontJapanese;
		}
	}else if(m_fontParam.language == kFontLanguageChinese){
		if(isChinese(ucs2code)){
			return m_pvfFontChinese;
		}
	}else if(m_fontParam.language == kFontLanguageKorean){
		if(isKorean(ucs2code)){
			return m_pvfFontKorean;
		}
	}


	if(isJapanese(ucs2code)){
		return m_pvfFontJapanese;
	}
	if(isKorean(ucs2code)){
		return m_pvfFontKorean;
	}
	if(isChinese(ucs2code)){
		return m_pvfFontChinese;
	}	
	return m_pvfFontLatin;
}

ssgi::PvfFont *ssgi::FontData::getFont(uint16_t ucs2code) const
{
	PvfFont* pvfFont = chooseLang(ucs2code);
	pvfFont->setup(&m_fontParam);

	int ret = scePvfSetCharSize( pvfFont->m_fontId, m_pvfFontSizePointH, m_pvfFontSizePointV );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	return pvfFont;
}

ssgi::FontData::FontData(void)
{
	m_fontLoader = NULL;
	m_pvfFontJapanese = NULL;
	m_pvfFontKorean = NULL;
	m_pvfFontChinese = NULL;
	m_pvfFontLatin = NULL;
	
}

ssgi::FontData::~FontData(void)
{
	finalize();
}

void ssgi::FontData::estimateMaxCharSize(void)
{
	int ret;

	float maxHeight = 0.0f;
	float maxDepth  = 0.0f;
	float maxWidth  = 0.0f;

	PvfFont* font = NULL;
	if(m_fontParam.language == kFontLanguageJapanese){
		font = m_pvfFontJapanese;
	}else if(m_fontParam.language == kFontLanguageChinese){
		font = m_pvfFontChinese;
	}else if(m_fontParam.language == kFontLanguageKorean){
		font = m_pvfFontKorean;
	}else{
		font = m_pvfFontLatin;
	}


	// Estimate texture size
	char alphabet[] = "abcdefghijklnmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	{
		char *code = &alphabet[0];
		while( *code != 0){
			uint16_t c = (uint16_t)(*code);
			
			//PvfFont* font = chooseLang(c);
			font->setup(&m_fontParam);

			ScePvf_t_charInfo charInfo;
			font->getCharInfo(&charInfo, c);

			float height = fabs(charInfo.glyphMetrics.horizontalBearingY64/64.0f);
			float depth  = fabs(charInfo.glyphMetrics.height64 / 64.0f) - fabs(charInfo.glyphMetrics.horizontalBearingY64/64.0f);
			float width  = fabs(charInfo.glyphMetrics.width64 / 64.0f);

			maxHeight = fmaxf(maxHeight, height);
			maxDepth  = fmaxf(maxDepth, depth);
			maxWidth  = fmaxf(maxWidth, width);
			/*
			printf("%c %#x (%.1f, %.1f) (%.1f, %.1f)\n", c, c, charInfo.glyphMetrics.width64 / 64.0f, charInfo.glyphMetrics.height64 / 64.0f,
				charInfo.glyphMetrics.ascender64/ 64.0f, charInfo.glyphMetrics.descender64/ 64.0f);
				*/
			code++;
		}
	}

	uint16_t ucs2aiueo[] = { 0x3042, // a
		0x3044, //i 
		0x3046, // u
		0x3048, //e
		0x304a, //o
		0x30DD, //po

		0x4E00,
		0x4E01,
		0x4E02,
		0x4E03,
		0x4E04,
		0x4E05,
		0x4E06,
		0x4E07,

		0xAC00,
		0xAC01,
		0xAC02,
		0xAC03,
		0xAC04,
		0xAC05,
		0xAC06,
		0xAC07,

		0 };
	{
		uint16_t *code = &ucs2aiueo[0];
		while( *code != 0){
			uint16_t c = *code;

			//PvfFont* font = chooseLang(c);
			font->setup(&m_fontParam);

			ScePvf_t_charInfo charInfo;
			font->getCharInfo(&charInfo, c);

			float height = fabs(charInfo.glyphMetrics.horizontalBearingY64/64.0f);
			float depth  = fabs(charInfo.glyphMetrics.height64 / 64.0f) - fabs(charInfo.glyphMetrics.horizontalBearingY64/64.0f);
			float width  = fabs(charInfo.glyphMetrics.width64 / 64.0f);

			maxHeight = fmaxf(maxHeight, height);
			maxDepth  = fmaxf(maxDepth, depth);
			maxWidth  = fmaxf(maxWidth, width);
			/*
			printf("%c %#x (%.1f, %.1f) (%.1f, %.1f)\n", c, c, charInfo.glyphMetrics.width64 / 64.0f, charInfo.glyphMetrics.height64 / 64.0f,
				charInfo.glyphMetrics.ascender64/ 64.0f, charInfo.glyphMetrics.descender64/ 64.0f);
*/
			code++;
		}
	}
	m_fontHeight  = maxHeight + maxDepth;

	float pvfFontSizePointH	= scePvfPixelToPointH( font->m_libId, m_fontParam.fontSizePixelH, &ret );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	float pvfFontSizePointV	= scePvfPixelToPointV( font->m_libId, m_fontParam.fontSizePixelV, &ret );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_pvfFontSizePointH = pvfFontSizePointH * (m_fontParam.fontSizePixelH / m_fontHeight);
	m_pvfFontSizePointV = pvfFontSizePointV * (m_fontParam.fontSizePixelH / m_fontHeight);

	if( m_fontParam.baselineOffsetY == 0.0f ){
		m_baselineOffset =maxHeight * (m_fontParam.fontSizePixelH / m_fontHeight);
	}else{
		m_baselineOffset = m_fontParam.baselineOffsetY;
	}
	

}

int ssgi::FontData::initialize(FontLoaderImpl* fontLoader, const FontParam* param )
{
	if( fontLoader == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	int	ret	= SCE_OK;
	FontParam defaultParam;
	if(param == NULL){
		param = &defaultParam;
	}
	m_fontParam = *param;
	
	m_fontLoader = fontLoader;

	ScePvfStyleCode pvfStyle;
	if(param->isItalic){
		switch(param->weight){
		case kFontWeightNarrow: { pvfStyle = SCE_PVF_STYLE_NARROW_OBLIQUE; break; }
		case kFontWeightBold:   { pvfStyle = SCE_PVF_STYLE_BOLD_OBLIQUE; break; }
		case kFontWeightBlack:  { pvfStyle = SCE_PVF_STYLE_BLACK_OBLIQUE; break; }
		default:                { pvfStyle = SCE_PVF_STYLE_OBLIQUE; break; }
		}
	}else{
		switch(param->weight){
		case kFontWeightNarrow: { pvfStyle = SCE_PVF_STYLE_NARROW; break; }
		case kFontWeightBold:   { pvfStyle = SCE_PVF_STYLE_BOLD; break; }
		case kFontWeightBlack:  { pvfStyle = SCE_PVF_STYLE_BLACK; break; }
		default:                { pvfStyle = SCE_PVF_STYLE_REGULAR; break; }
		}
	}

	ScePvfFamilyCode family;
	switch(param->family){
	case kFontFamilySerif   : { family = SCE_PVF_FAMILY_SERIF; break; }
	case kFontFamilyRounded : { family = SCE_PVF_FAMILY_ROUNDED; break; }
	default                 : { family = SCE_PVF_FAMILY_SANSERIF; break; }
	}

	{
		ScePvf_t_fontStyleInfo		pvfTargetStyle;
		memset( &pvfTargetStyle, 0, sizeof(pvfTargetStyle) );
		pvfTargetStyle.languageCode		= SCE_PVF_LANGUAGE_J;
		pvfTargetStyle.familyCode		= family;
		pvfTargetStyle.style			= pvfStyle;
		m_pvfFontJapanese = m_fontLoader->openFontFile( &pvfTargetStyle, SCE_PVF_FILEBASEDSTREAM, &ret );
		if( ret != SCE_OK ){
			return ret;
		}
	}
	{
		ScePvf_t_fontStyleInfo		pvfTargetStyle;
		memset( &pvfTargetStyle, 0, sizeof(pvfTargetStyle) );
		pvfTargetStyle.languageCode		= SCE_PVF_LANGUAGE_K;
		pvfTargetStyle.familyCode		= family;
		pvfTargetStyle.style			= pvfStyle;
		m_pvfFontKorean = m_fontLoader->openFontFile( &pvfTargetStyle, SCE_PVF_FILEBASEDSTREAM, &ret );
		if( ret != SCE_OK ){
			m_fontLoader->closeFontFile(m_pvfFontJapanese);
			m_pvfFontJapanese = NULL;
			return ret;
		}
	}
	{
		ScePvf_t_fontStyleInfo		pvfTargetStyle;
		memset( &pvfTargetStyle, 0, sizeof(pvfTargetStyle) );
		pvfTargetStyle.languageCode		= SCE_PVF_LANGUAGE_C;
		pvfTargetStyle.familyCode		=  family;
		pvfTargetStyle.style			= pvfStyle;
		m_pvfFontChinese = m_fontLoader->openFontFile( &pvfTargetStyle, SCE_PVF_FILEBASEDSTREAM, &ret );
		if( ret != SCE_OK ){
			m_fontLoader->closeFontFile(m_pvfFontJapanese);
			m_pvfFontJapanese = NULL;
			m_fontLoader->closeFontFile(m_pvfFontKorean);
			m_pvfFontKorean = NULL;
			return ret;
		}
	}
	
	{
		ScePvf_t_fontStyleInfo		pvfTargetStyle;
		memset( &pvfTargetStyle, 0, sizeof(pvfTargetStyle) );
		pvfTargetStyle.languageCode		= SCE_PVF_LANGUAGE_LATIN;
		pvfTargetStyle.familyCode		= family;
		pvfTargetStyle.style			= pvfStyle;
		m_pvfFontLatin = m_fontLoader->openFontFile( &pvfTargetStyle, SCE_PVF_FILEBASEDSTREAM, &ret );
		if( ret != SCE_OK ){
			m_fontLoader->closeFontFile(m_pvfFontJapanese);
			m_pvfFontJapanese = NULL;
			m_fontLoader->closeFontFile(m_pvfFontKorean);
			m_pvfFontKorean = NULL;
			m_fontLoader->closeFontFile(m_pvfFontChinese);
			m_pvfFontChinese = NULL;
			return ret;
		}
	}

	estimateMaxCharSize();

	return SCE_OK;
}

int ssgi::FontData::finalize(void)
{
	int ret;
	if(m_fontLoader != NULL){
		if(m_pvfFontJapanese != NULL){
			ret = m_fontLoader->closeFontFile( m_pvfFontJapanese );
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
			m_pvfFontJapanese = NULL;
		}
		if(m_pvfFontKorean != NULL){
			ret = m_fontLoader->closeFontFile( m_pvfFontKorean );
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
			m_pvfFontKorean = NULL;
		}
		if(m_pvfFontChinese != NULL){
			ret = m_fontLoader->closeFontFile( m_pvfFontChinese );
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
			m_pvfFontChinese = NULL;
		}
		if(m_pvfFontLatin != NULL){
			ret = m_fontLoader->closeFontFile( m_pvfFontLatin );
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
			m_pvfFontLatin = NULL;
		}
		m_fontLoader = NULL;
	}
	(void)ret;
	return SCE_OK;
}

uint32_t ssgi::FontData::getFontHeight(void) const
{
	return m_fontHeight;
	//return (uint32_t)m_fontParam.fontSizePixelH;
}
float ssgi::FontData::getBaselineOffset(void) const
{
	return m_baselineOffset;
/*
	if( m_fontParam.baselineOffsetY == 0.0f ){
		float a = fabs(m_fontInfo.maxFGlyphMetrics.ascender);
		float d = fabs(m_fontInfo.maxFGlyphMetrics.descender);
		return m_fontParam.fontSizePixelH * a / (a+d);
	} else {
		return m_fontParam.baselineOffsetY;
	}
*/
}

float ssgi::FontData::getCharWidth(uint16_t ucs2Charcode) const
{
	PvfFont* pvfFont = getFont(ucs2Charcode);
	ScePvf_t_charInfo charInfo;
	memset(&charInfo, 0, sizeof(charInfo));
	pvfFont->getCharInfo( &charInfo, ucs2Charcode);
	//printf("char width %#x(%c) = %f\n", ucs2Charcode,  ucs2Charcode, charInfo.glyphMetrics.width64 / 64.0f);
	return charInfo.glyphMetrics.width64 / 64.0f;
}

bool ssgi::FontData::isBlank(uint16_t ucs2Charcode) const
{
	PvfFont* pvfFont = getFont(ucs2Charcode);
	ScePvf_t_charInfo charInfo;
	memset(&charInfo, 0, sizeof(charInfo));
	pvfFont->getCharInfo( &charInfo, ucs2Charcode);

	return (charInfo.glyphMetrics.width64 == 0) || (charInfo.glyphMetrics.height64 == 0);
}

float ssgi::FontData::getCharBearingX(uint16_t ucs2Charcode) const
{
	PvfFont* pvfFont = getFont(ucs2Charcode);
	ScePvf_t_charInfo charInfo;
	memset(&charInfo, 0, sizeof(charInfo));
	pvfFont->getCharInfo( &charInfo, ucs2Charcode);
	return charInfo.glyphMetrics.horizontalBearingX64 / 64.0f;

}
uint32_t ssgi::FontData::getCharAdvanceX(uint16_t ucs2Charcode) const
{
	PvfFont* pvfFont = getFont(ucs2Charcode);
	ScePvf_t_charInfo charInfo;
	memset(&charInfo, 0, sizeof(charInfo));
	pvfFont->getCharInfo( &charInfo, ucs2Charcode);
	return charInfo.glyphMetrics.horizontalAdvance64 / 64.0f;
}

int ssgi::FontData::rasterize(uint8_t *image, uint32_t imageWidth, uint32_t imageHeight, uint32_t imageStride, 
	uint32_t x, uint32_t y, uint16_t ucs2Charcode,
	uint32_t clipWidth,
	uint32_t clipHeight	)
{
	for(uint32_t j=0; j<clipHeight; j++){
		for(uint32_t i=0; i<clipWidth; i++){
			uint32_t ix = x + i;
			uint32_t iy = y + j;
			image[iy*imageStride + ix] = 0;
		}
	}
	PvfFont* pvfFont = getFont(ucs2Charcode);
	ScePvf_t_userImageBufferRec		pvfImageBuffer;
	memset( &pvfImageBuffer, 0, sizeof(pvfImageBuffer) );
	pvfImageBuffer.pixelFormat			= SCE_PVF_USERIMAGE_DIRECT8;
	pvfImageBuffer.xPos64				= (int32_t)(x*64 - getCharBearingX(ucs2Charcode) * 64.0f);
	pvfImageBuffer.yPos64				= (int32_t)(y*64 + getBaselineOffset() * 64.0f);

	pvfImageBuffer.rect.width			= imageWidth;
	pvfImageBuffer.rect.height			= imageHeight;
	pvfImageBuffer.bytesPerLine			= imageStride;
	pvfImageBuffer.buffer				= (uint8_t*)image;
	int ret = pvfFont->getGlyphImage(&pvfImageBuffer, ucs2Charcode, x, y, clipWidth, clipHeight );
	return ret;
}

#pragma endregion





#endif // _SCE_TARGET_OS_PSP2