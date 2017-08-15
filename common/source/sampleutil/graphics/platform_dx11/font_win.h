/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_FONT_WIN_H
#define _SCE_SAMPLE_UTIL_FONT_WIN_H

#include <sampleutil.h>
#include <sampleutil_internal.h>

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class FontLoaderImpl;


				class FontData
				{
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

					void estimateMaxCharSize(void);
				private:
					FontLoaderImpl* m_fontLoader;
					FontParam		m_fontParam;
					HFONT m_font;

					DWORD getGlyph(GLYPHMETRICS *gm, UINT uChar, uint8_t *buffer, uint32_t bufferSize) const; 

					float m_fontHeight;
					float m_baselineOffset;
				};



				class FontLoaderImpl : public FontLoader, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					FontLoaderImpl()
					{
						m_loader = NULL;
					}
					virtual ~FontLoaderImpl(void)
					{
						finalize();
					}
					int initialize(GraphicsLoader *loader)
					{
						m_loader = loader;
						return SCE_OK;
					}
					int finalize(void)
					{
						m_loader = NULL;
						return SCE_OK;
					}

					GraphicsLoader *getGraphicsLoader(void)
					{
						return m_loader;
					}

					int createFont( Font **outFont, const FontParam* param = NULL, uint32_t numCache=1024);

					std::string getResourceType(void) const
					{
						return "Graphics::FontLoader";
					}

					HDC getDisplayDeviceContext(void) const;
				private:
					GraphicsLoader *m_loader;
				};					


			}
		}
	}
}



#endif /* _SCE_TARGET_OS_WINDOWS */

#endif /* _SCE_SAMPLE_UTIL_FONT_WIN_H */