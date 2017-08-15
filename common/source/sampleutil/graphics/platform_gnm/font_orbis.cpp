/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS
#include "font_orbis.h"

#include "graphics/font_internal.h"
#include "util/util.h"
#include <libsysmodule.h>

#pragma comment(lib,"libSceSysmodule_stub_weak.a")
#pragma comment(lib,"SceFont_stub_weak")
#pragma comment(lib,"SceFontFt_stub_weak")

namespace ssg  = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;


//--------------------------------------------------------------

#pragma region FontData

ssgi::FontData::FontData(void)
{
	m_handle = NULL;
}

ssgi::FontData::~FontData(void)
{
	finalize();
}

int ssgi::FontData::estimateMaxCharSize(void)
{
	int ret;

	SceFontOpenDetail* openDetail = NULL;
	uint32_t openFlag = SCE_FONT_OPEN_MEMORY_STREAM;

	bool isBold = (m_param.weight == ssg::kFontWeightBold) || (m_param.weight == ssg::kFontWeightBlack);

	uint32_t fontset;
	if(m_param.isItalic){
		if(isBold){
			fontset = SCE_FONT_SET_SST_STD_EUROPEAN_JP_W1G_BOLD_ITALIC;
		}else{
			fontset = SCE_FONT_SET_SST_STD_EUROPEAN_JP_W1G_ITALIC;
		}
	}else{
		if(isBold){
			fontset = SCE_FONT_SET_SST_STD_EUROPEAN_JP_W1G_BOLD;
		}else{
			fontset = SCE_FONT_SET_SST_STD_EUROPEAN_JP_W1G;
		}
	}

	SceFontHandle tmpHandle = NULL;
	SceFontLibrary fontLib = m_fontLoader->getLibrary();
	ret = sceFontOpenFontSet( fontLib, fontset, openFlag, openDetail, &tmpHandle);
	if ( ret != SCE_OK ) {
		return ret;
	}
#if 1
	float maxHeight = 0.0f;
	float maxDepth  = 0.0f;
	float maxWidth  = 0.0f;

	ret = sceFontSetScalePixel( tmpHandle, m_param.fontSizePixelH, m_param.fontSizePixelV );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	// Estimate texture size
	char alphanumericCharacters[] = "abcdefghijklnmopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
	{
		char *code = &alphanumericCharacters[0];
		while( *code != 0){
			uint16_t c = (uint16_t)(*code);
			
			SceFontGlyphMetrics gm;
			ret = sceFontGetCharGlyphMetrics(tmpHandle, c, &gm);
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

			float height = gm.Horizontal.bearingY;
			float depth  = gm.height - gm.Horizontal.bearingY;
			float width  = gm.width;

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

	float alphanumericCharactersHeight = maxHeight + maxDepth;
	float maxAlphanumericCharactersHeight = maxHeight;

	uint16_t ucs2aiueo[] = { 0x3042, // a
		0x3044, //i 
		0x3046, // u
		0x3048, //e
		0x304a, //o
		0x30D7, //pu
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

			SceFontGlyphMetrics gm;
			ret = sceFontGetCharGlyphMetrics(tmpHandle, c, &gm);
			if(ret != SCE_OK){
				code++;
				continue;
			}
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

			float height = gm.Horizontal.bearingY;
			float depth  = gm.height - gm.Horizontal.bearingY;
			float width  = gm.width;

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
	float fontHeight  = maxHeight + maxDepth;

	m_fontSize = sce::Vectormath::Simd::Aos::Vector2(m_param.fontSizePixelH * (m_param.fontSizePixelH / fontHeight),
		                                             m_param.fontSizePixelV * (m_param.fontSizePixelH / fontHeight));

	if( m_param.baselineOffsetY == 0.0f ){
		m_baselineOffset = maxHeight * (m_param.fontSizePixelH / fontHeight);
		m_alphanumericCharactersBaselineOffset = maxAlphanumericCharactersHeight * (m_param.fontSizePixelH / fontHeight);
	}else{
		m_baselineOffset = m_param.baselineOffsetY;
		m_alphanumericCharactersBaselineOffset = m_param.baselineOffsetY;
	}

	m_alphanumericCharactersHeightPixel = alphanumericCharactersHeight * m_param.fontSizePixelV / fontHeight;

#endif
	sceFontCloseFont( tmpHandle);

	return SCE_OK;
}

int ssgi::FontData::initialize(ssgi::FontLoaderImpl* fontLoader, const ssg::FontParam* param )
{

	SceFontOpenDetail* openDetail = NULL;
	uint32_t openFlag = SCE_FONT_OPEN_MEMORY_STREAM;
	if(param != NULL){
		m_param = *param;
	}
	m_fontLoader = fontLoader;

	int ret;

	ret = estimateMaxCharSize();
	if(ret != SCE_OK){

		return ret;
	}

	bool isBold = (m_param.weight == ssg::kFontWeightBold) || (m_param.weight == ssg::kFontWeightBlack);

	uint32_t fontset;
	if(m_param.isItalic){
		if(isBold){
			fontset = SCE_FONT_SET_SST_STD_EUROPEAN_JP_W1G_BOLD_ITALIC;
		}else{
			fontset = SCE_FONT_SET_SST_STD_EUROPEAN_JP_W1G_ITALIC;
		}
	}else{
		if(isBold){
			fontset = SCE_FONT_SET_SST_STD_EUROPEAN_JP_W1G_BOLD;
		}else{
			fontset = SCE_FONT_SET_SST_STD_EUROPEAN_JPCJK_AR_W1G;
		}
	}
	//printf("fot debug\n");
	//fontset = SCE_FONT_SET_SST_STD_JAPANESE_JP_W1G;

	ret = sceFontOpenFontSet( m_fontLoader->getLibrary(), fontset, openFlag, openDetail, &m_handle);
	if ( ret != SCE_OK ) {

		return ret;
	}


	//E Set the scale for the font
	//J フォントにスケールを設定
	ret = sceFontSetScalePixel( m_handle, (float)m_fontSize.getX(), (float)m_fontSize.getY() );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	//m_baselineOffset = m_param.fontSizePixelH * baselineRatio;
	//E Obtain the line layout information in the specified scale
	//J 指定したスケールにおける、行のレイアウト情報を取得
	ret = sceFontGetHorizontalLayout( m_handle, &m_horizontalLayout );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	//E Bind the renderer for use
	//J 使用するレンダラーをバインド
	ret = sceFontBindRenderer( m_handle, fontLoader->getRenderer() );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	//ret = sceFontSetupRenderScalePixel( m_handle, 100, 100);
	ret = sceFontSetupRenderScalePixel( m_handle, (float)m_fontSize.getX(), (float)m_fontSize.getY() );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	return 0;
}
int ssgi::FontData::finalize(void)
{ 
	if(m_handle == NULL){
		sceFontUnbindRenderer( m_handle);
		sceFontCloseFont( m_handle );
		m_handle = NULL;
	}
	return 0;
}

uint32_t ssgi::FontData::getFontHeight(void) const
{
	return m_param.fontSizePixelH;
}

float ssgi::FontData::getAlphanumericCharactersHeight(void) const
{
	return m_alphanumericCharactersHeightPixel;
}

float ssgi::FontData::getAlphanumericCharactersBaselineOffset(void) const
{
	return m_alphanumericCharactersBaselineOffset;
}

float ssgi::FontData::getBaselineOffset(void) const
{
	return m_baselineOffset;
}

bool ssgi::FontData::isBlank(uint16_t ucs2Charcode) const 
{
	SceFontGlyphMetrics gm;
	uint32_t code = ucs2Charcode;
	int ret = sceFontGetCharGlyphMetrics(m_handle, code, &gm);
	if(ret != SCE_OK){
		return true;
	}
	return gm.width == 0;
}

float ssgi::FontData::getCharBearingX(uint16_t ucs2Charcode) const 
{
	SceFontGlyphMetrics gm;
	uint32_t code = ucs2Charcode;
	int ret = sceFontGetCharGlyphMetrics(m_handle, code, &gm);
	if(ret != SCE_OK){
		return 0.0f;
	}
	return gm.Horizontal.bearingX;
}

uint32_t ssgi::FontData::getCharAdvanceX(uint16_t ucs2Charcode) const 
{
	SceFontGlyphMetrics gm;
	uint32_t code = ucs2Charcode;
	int ret = sceFontGetCharGlyphMetrics(m_handle, code, &gm);
	if(ret != SCE_OK){
		return 0.0f;
	}
	return gm.Horizontal.advance;
}

int ssgi::FontData::rasterize(uint8_t *image, uint32_t imageWidth, uint32_t imageHeight, uint32_t imageStride,
	uint32_t x, uint32_t y, uint16_t ucs2Charcode,
	uint32_t clipWidth,
	uint32_t clipHeight ) 
{

	SceFontRenderSurface surface;
	sceFontRenderSurfaceInit(&surface, image, imageStride, 1, imageWidth, imageHeight);
	sceFontRenderSurfaceSetScissor(&surface, x, y, clipWidth, clipHeight);

	uint32_t ucode = ucs2Charcode;

	SceFontGlyphMetrics gm;
	int ret = sceFontGetCharGlyphMetrics(m_handle, ucode, &gm);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	(void)ret;

	SceFontGlyphMetrics  metrics;
	SceFontRenderResult  result;
	sceFontRenderCharGlyphImageHorizontal( m_handle, ucode, &surface, x - gm.Horizontal.bearingX, y + m_baselineOffset, &metrics, &result );
	return SCE_OK;
}
float ssgi::FontData::getCharWidth(uint16_t ucs2Charcode) const 
{
	SceFontGlyphMetrics gm;
	uint32_t code = ucs2Charcode;
	int ret = sceFontGetCharGlyphMetrics(m_handle, code, &gm);
	if(ret != SCE_OK){
		return 0.0f;
	}
	return gm.width;
}

#pragma endregion // FontData
					
#pragma region FontLoaderImpl
					


// ---------------------------------------------------------------------

static void* fontMalloc( void* object, uint32_t size )
{
	(void)object;
	void *ptr = malloc( size );
	return ptr;
}
static void fontFree( void* object, void *p )
{
	(void)object;
	free( p );
}
static void* fontRealloc( void* object, void *p, uint32_t size )
{
	(void)object;

	void *ptr = realloc( p, size );
	return ptr;
}
static void* fontCalloc( void* object, uint32_t n, uint32_t size )
{
	(void)object;
	void *ptr = calloc( n, size );
	return ptr;
}

const SceFontMemoryInterface  s_fontLibcMallocInterface = {
	.Malloc  = fontMalloc,
	.Free    = fontFree,
	.Realloc = fontRealloc,
	.Calloc  = fontCalloc,
	.MspaceCreate  = (SceFontMspaceCreateFunction *) 0,
	.MspaceDestroy = (SceFontMspaceDestroyFunction*) 0,
};

static void fontMemoryDestroyCallback( SceFontMemory* fontMemory, void*object, void* destroyArg )
{
	(void)object;
	(void)destroyArg;
	(void)fontMemory;
}


int ssgi::FontLoaderImpl::initialize(ssg::GraphicsLoader *loader)
{
	m_loader = loader;
	int ret;

	// load module
	{
		ret = sceSysmoduleLoadModule( SCE_SYSMODULE_FONT );
		if(ret != SCE_OK ){
			return ret;
		}
		ret = sceSysmoduleLoadModule( SCE_SYSMODULE_FONT_FT );
		if(ret != SCE_OK) {
			sceSysmoduleUnloadModule( SCE_SYSMODULE_FONT );
			return ret;
		}
		ret = sceSysmoduleLoadModule( SCE_SYSMODULE_FREETYPE_OT );
		if (ret!=SCE_OK) {
			sceSysmoduleUnloadModule( SCE_SYSMODULE_FONT_FT );
			sceSysmoduleUnloadModule( SCE_SYSMODULE_FONT );
			return ret;
		}
	}
	// init memory callback
	{
		ret = sceFontMemoryInit( &m_fontMemory, NULL, 0,
			&s_fontLibcMallocInterface, (void*)0,
			fontMemoryDestroyCallback, (void*)0 );
		if(ret != SCE_OK){
			finalize();
			return ret;
		}
		m_hasFontMemory = true;
	}

	// init fontlib
	{
		ret = sceFontCreateLibrary( &m_fontMemory, sceFontSelectLibraryFt(0), &m_fontLib );
		if(ret != SCE_OK){
			finalize();
			return ret;
		}

		//E Processing which should be done only once as necessary just after creating library
		//J ライブラリ作成直後に必要に応じて一度だけ行うべき処理
		ret = sceFontSupportSystemFonts( m_fontLib );
		if(ret != SCE_OK){
			sceFontDestroyLibrary( &m_fontLib );
			finalize();
			return ret;
		}
		//E Have the library support handling of external font
		//J ライブラリに、外部のフォントの取り扱いをサポートさせる。
		ret = sceFontSupportExternalFonts( m_fontLib, 16, SCE_FONT_FORMAT_OPENTYPE );
		if(ret != SCE_OK){
			sceFontDestroyLibrary( &m_fontLib );
			finalize();
			return ret;
		}
		//E Provide the device cache buffer for a library.
		//J ライブラリにデバイスキャッシュバッファーを与える。
		ret = sceFontAttachDeviceCacheBuffer( m_fontLib, NULL, 512*1024 );
		if(ret != SCE_OK){
			sceFontDestroyLibrary( &m_fontLib );
			finalize();
			return ret;
		}
		m_hasFontLib = true;
	}

	// init renderer
	{
		ret = sceFontCreateRenderer( &m_fontMemory, sceFontSelectRendererFt(0), &m_renderer );
		if(ret != SCE_OK){
			finalize();
			return ret;
		}
		m_hasRenderer = true;
	}


	return SCE_OK;
}

int ssgi::FontLoaderImpl::finalize(void)
{
	m_loader = NULL;
	int ret;

	if ( m_hasRenderer ) {
		sceFontDestroyRenderer( &m_renderer );
		m_hasRenderer = false;
	}
	if ( m_hasFontLib ) {
		sceFontDestroyLibrary( &m_fontLib );
		m_hasFontLib = false;
	}

	if(m_hasFontMemory){
		sceFontMemoryTerm( &m_fontMemory );
	}

	ret = sceSysmoduleUnloadModule( SCE_SYSMODULE_FREETYPE_OT );
	if( ret != SCE_OK ){
	   return ret;
	}
	ret = sceSysmoduleUnloadModule( SCE_SYSMODULE_FONT_FT );
	if( ret != SCE_OK ){
	   return ret;
	}
	ret = sceSysmoduleUnloadModule( SCE_SYSMODULE_FONT );
	if( ret != SCE_OK ){
	   return ret;
	}

	return ret;
}


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

#pragma endregion // FontLoaderImpl

#endif // _SCE_TARGET_OS_ORBIS

