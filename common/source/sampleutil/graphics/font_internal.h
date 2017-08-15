/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#ifndef _SCE_SAMPLE_UTIL_FONT_INTERNAL_H
#define _SCE_SAMPLE_UTIL_FONT_INTERNAL_H

#include "graphics_internal.h"
#include "platform_dx11/font_win.h"
#include "platform_gnm/font_orbis.h"
#include "platform_gxm/font_psp2.h"



namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{


			namespace Impl
			{

				class FontTextureAtlas
				{
				public:
					Texture *m_texture;
					uint32_t m_charWidth;
					uint32_t m_charHeight;
					uint32_t m_numCharsPerLine;
					uint32_t m_numLines;
					uint32_t m_textureWidth;
					uint32_t m_textureHeight;


				public:
					FontTextureAtlas(void);
					~FontTextureAtlas(void);

					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const;

					int initialize(GraphicsLoader *loader, 
						           uint32_t charWidth, uint32_t charHeight,
                                   uint32_t numCache);
					int finalize(void);
					Texture *getTexture(void) const;
					int getInfo(uint32_t *outX, uint32_t *outY, float *outU, float *outV, uint32_t index);

					uint8_t *m_cpuBuffer;
				};


				struct FontCharInfo
				{
					uint32_t x;
					uint32_t y;
					float u;
					float v;
					int32_t index;

					float bearingX;
					float advanceX;
					bool isBlank;
					float width;
				};


				class FontCache
				{
				private:
					FontData m_fontData;
					FontTextureAtlas m_textureAtlas;
					sce::SampleUtil::Map<uint16_t, FontCharInfo>::Type m_charMap;
					uint32_t m_numChars;
					uint32_t m_maxChars;

					uint32_t getNewIndex(void);

					int cacheChar(uint16_t ucs2code);
				public:
					FontCache(void)
					{
						m_numChars = 0;
						m_maxChars = 0;
					}
					~FontCache(void);

					int initialize(FontLoaderImpl* fontLoader, const FontParam* param, uint32_t maxChars);

					int finalize(void);

					int getCharInfo(FontCharInfo *charInfo, uint16_t ucs2code);

					int cacheCharacters( const uint16_t* ucs2Charcode );

					void clearCache(void);

					Texture *getTexture(void) const{ return m_textureAtlas.getTexture(); }

					float getCharHeight(void) const
					{
						return m_fontData.getFontHeight();
					}

#if _SCE_TARGET_OS_ORBIS
					float getAlphanumericCharactersHeight(void) const
					{
						return m_fontData.getAlphanumericCharactersHeight();
					}

					float getAlphanumericCharactersBaselineOffset(void) const
					{
						return m_fontData.getAlphanumericCharactersBaselineOffset();
					}
#endif
				};


				class FontImpl2 : public Font, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					FontCache m_fontCache;
				public:
					FontImpl2(void){}

					virtual ~FontImpl2(void)
					{
						finalize();
					}

					int initialize( FontLoaderImpl* fontLoader, const FontParam* param = NULL, uint32_t numMaxCache=1024)
					{
						return m_fontCache.initialize(fontLoader, param, numMaxCache);
					}

					int finalize(void)
					{
						return m_fontCache.finalize();
					}

					int cacheCharacters( const uint16_t* ucs2Charcode )
					{
						return m_fontCache.cacheCharacters( ucs2Charcode );
					}

					void clearCache()
					{
						m_fontCache.clearCache();
					}

					std::string getResourceType(void) const
					{
						return "Graphics::Font";
					}

					Texture *getTexture(void) const{ return m_fontCache.getTexture(); }
					int getCharInfo(FontCharInfo *charInfo, uint16_t ucs2code)
					{
						//printf(__FILE__"(%d) get char info\n", __LINE__);
						return m_fontCache.getCharInfo(charInfo, ucs2code);
					}

					float getCharHeight(void) const
					{
						return m_fontCache.getCharHeight();
					}

#if _SCE_TARGET_OS_ORBIS
					float getAlphanumericCharactersHeight(void) const
					{
						return m_fontCache.getAlphanumericCharactersHeight();
					}

					float getAlphanumericCharactersBaselineOffset(void) const
					{
						return m_fontCache.getAlphanumericCharactersBaselineOffset();
					}
#endif
				};



			}
		}
	}
}




#endif // _SCE_SAMPLE_UTIL_FONT_INTERNAL_H