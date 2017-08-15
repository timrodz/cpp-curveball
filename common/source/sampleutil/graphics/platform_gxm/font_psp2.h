/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_FONT_PSP2_H
#define _SCE_SAMPLE_UTIL_FONT_PSP2_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2
#include <sampleutil/graphics/font.h>
#include <font/libpvf.h>
#include <sampleutil_internal.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class FontLoaderImpl;

				class PvfFont
				{
				public:
					ScePvf_t_libId	m_libId;

					ScePvf_t_fontId	m_fontId;

					void setup(
						float	fontSizePixelH,
						float	fontSizePixelV);


				public:
					PvfFont(void)
					{
						m_libId = NULL;
						m_fontId = NULL;
					}
					~PvfFont(void)
					{
						finalize();
					}

					ScePvf_t_fontId	getFontId(void) const
					{
						return m_fontId;
					}

					int initialize(ScePvf_t_initRec	*pvfInitParams, ScePvf_t_fontStyleInfo* styleInfo, uint32_t accessMode);

					int finalize(void);

					void setup(const FontParam *fontParam);

					void getCharInfo(ScePvf_t_charInfo* charInfo, uint16_t ucs2Charcode);

					int getGlyphImage(ScePvf_t_userImageBufferRec *buffer, uint16_t ucs2Charcode);
					int getGlyphImage(ScePvf_t_userImageBufferRec *buffer, uint16_t ucs2Charcode,     
						int32_t clipX,
						int32_t clipY,
						uint32_t clipWidth,
						uint32_t clipHeight);
				};

				class FontData
				{
				private:
					FontLoaderImpl* m_fontLoader;
					FontParam		m_fontParam;
					PvfFont*		m_pvfFontJapanese;
					PvfFont*		m_pvfFontKorean;
					PvfFont*		m_pvfFontChinese;
					PvfFont*		m_pvfFontLatin;

					//ScePvf_t_fontInfo m_fontInfo;

					PvfFont *chooseLang(uint16_t ucs2code) const;

					PvfFont *getFont(uint16_t ucs2code) const;

					void estimateMaxCharSize(void);

					float m_fontHeight;
					float m_baselineOffset;

					float m_pvfFontSizePointH;
					float m_pvfFontSizePointV;

				public:
					FontData(void);

					~FontData(void);

					int initialize(FontLoaderImpl* fontLoader, const FontParam* param );
					int finalize(void);
					uint32_t getFontHeight(void) const;

					float getBaselineOffset(void) const;

					bool isBlank(uint16_t ucs2Charcode) const;

					float getCharBearingX(uint16_t ucs2Charcode) const;

					uint32_t getCharAdvanceX(uint16_t ucs2Charcode) const;

					int rasterize(uint8_t *image, uint32_t imageWidth, uint32_t imageHeight, uint32_t imageStride, 
                                   uint32_t x, uint32_t y, uint16_t ucs2Charcode,
								   uint32_t clipWidth,
								   uint32_t clipHeight );
					float getCharWidth(uint16_t ucs2Charcode) const;
				};



				class FontLoaderImpl : public FontLoader, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					FontLoaderImpl(){}
					virtual ~FontLoaderImpl(void)
					{
						finalize();
					}

					int initialize(GraphicsLoader *loader);
					int finalize();

					GraphicsLoader *getGraphicsLoader(void);

					int createFont( Font **outFont, const FontParam* param = NULL, uint32_t numCache=1024);

					std::string getResourceType(void) const
					{
						return "Graphics::FontLoader";
					}

					/* for libpvf callback function */
					static void* cbAllocFunc(void *userData, uint32_t size);
					static void  cbFreeFunc(void *userData, void *p);
					static void* cbReallocFunc(void *userData, void *q, uint32_t size);

					/* for Font class */
					PvfFont *openFontFile( ScePvf_t_fontStyleInfo* styleInfo, uint32_t mode, int* errorCode );
					int closeFontFile( PvfFont* fontId );

				private:
					void clear();

					struct FontTableKey
					{
						ScePvf_t_fontStyleInfo m_styleInfo;
						uint32_t  accessMode;

						size_t getHash(void) const
						{
							size_t ret = 0;
							uint8_t *v = (uint8_t*)this;
							for(uint16_t i=0; i<sizeof(FontTableKey); i++){
								ret = ret *3 + v[i];
							}
							return ret;
						}

						bool operator < (const FontTableKey& b) const
						{
							return memcmp(this, &b, sizeof(FontTableKey)) < 0;
						}

					};

					class FontTableKeyHashFunction
					{
					public:
						enum
						{	
							bucket_size = 4,	
							min_buckets = 8
						};	
						size_t operator( )( const FontTableKey& indices ) const
						{
							return indices.getHash();
						}
						bool operator( )( 
							const FontTableKey& _Key1,
							const FontTableKey& _Key2
							) const
						{
							return _Key1 < _Key2;
						}
					};

					struct FontTableValue
					{
						PvfFont *fontInfo;
						uint32_t referenceCount;
					};

					typedef sce::SampleUtil::Map< FontTableKey, FontTableValue, FontTableKeyHashFunction>::Type	FontTable;
					FontTable	m_fontTable;

					GraphicsLoader *m_loader;

				};

			}
		}
	}
}


#endif /* _SCE_TARGET_OS_PSP2 */

#endif /* _SCE_SAMPLE_UTIL_FONT_PSP2_H */
