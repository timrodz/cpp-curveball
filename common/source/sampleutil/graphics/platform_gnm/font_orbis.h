/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_FONT_ORBIS_H
#define _SCE_SAMPLE_UTIL_FONT_ORBIS_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS
#include <sampleutil/graphics/font.h>
#include <sampleutil_internal.h>
#include <sce_font.h>

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

					float getAlphanumericCharactersHeight(void) const;

					float getAlphanumericCharactersBaselineOffset(void) const;
					
					float getBaselineOffset(void) const;

					bool isBlank(uint16_t ucs2Charcode) const;

					float getCharBearingX(uint16_t ucs2Charcode) const;

					uint32_t getCharAdvanceX(uint16_t ucs2Charcode) const;

					int rasterize(uint8_t *image, uint32_t imageWidth, uint32_t imageHeight, uint32_t imageStride, 
                                   uint32_t x, uint32_t y, uint16_t ucs2Charcode,
								   uint32_t clipWidth,
								   uint32_t clipHeight );
					float getCharWidth(uint16_t ucs2Charcode) const;
				private:
					FontLoaderImpl* m_fontLoader;
					int estimateMaxCharSize(void);
					SceFontHandle m_handle;
					FontParam m_param;
					SceFontHorizontalLayout m_horizontalLayout;
					float m_baselineOffset;
					float m_alphanumericCharactersBaselineOffset;
					float m_alphanumericCharactersHeightPixel;
					sce::Vectormath::Simd::Aos::Vector2 m_fontSize;
				};


				class FontLoaderImpl : public FontLoader, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					FontLoaderImpl(void)
					{
						m_loader = NULL;
						m_fontLib = NULL;
						m_renderer = NULL;
						m_hasFontMemory = false;
						m_hasFontLib    = false;
						m_hasRenderer   = false;

					}
					virtual ~FontLoaderImpl(void)
					{
						finalize();
					}
					int initialize(GraphicsLoader *loader);

					int finalize(void);

					GraphicsLoader *getGraphicsLoader(void)
					{
						return m_loader;
					}

					int createFont( Font **outFont, const FontParam* param = NULL, uint32_t numCache=1024);

					std::string getResourceType(void) const
					{
						return "Graphics::FontLoader";
					}
					SceFontLibrary getLibrary(void) const
					{
						return m_fontLib;
					}
					SceFontRenderer getRenderer(void) const
					{
						return m_renderer;
					}
				private:
					GraphicsLoader *m_loader;

					SceFontMemory m_fontMemory;
					bool m_hasFontMemory;

					SceFontLibrary  m_fontLib;
					bool m_hasFontLib;

					SceFontRenderer m_renderer;
					bool m_hasRenderer;
				};					


			}
		}
	}
}
#endif /* _SCE_TARGET_OS_ORBIS */

#endif /* _SCE_SAMPLE_UTIL_FONT_ORBIS_H */