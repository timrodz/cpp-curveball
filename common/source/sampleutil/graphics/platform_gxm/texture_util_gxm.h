/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <stdint.h>
#include <gxm.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				uint32_t getBitPerPixel(const SceGxmTextureFormat texFormat);
				
				uint32_t getTextureLinearSize(SceGxmTextureFormat texFormat,  uint32_t width,  uint32_t height,  uint32_t mipCount);

				uint32_t getTextureTiledSize(SceGxmTextureFormat texFormat,  uint32_t width,  uint32_t height,  uint32_t mipCount);

				uint32_t getTextureSwizzledSize(SceGxmTextureFormat texFormat, uint32_t width, uint32_t height,uint32_t mipCount);
				
				uint32_t getTextureCubeSize(SceGxmTextureFormat texFormat, uint32_t width, uint32_t height, uint32_t mipCount);
				
				uint32_t getTextureSize(SceGxmTextureType type, SceGxmTextureFormat texFormat, uint32_t width, uint32_t height, uint32_t stride, uint32_t mipCount);
				
				uint32_t getTextureSize(SceGxmTexture *tex);
			}
		}
	}
}

#endif /* _SCE_TARGET_OS_PSP2 */
