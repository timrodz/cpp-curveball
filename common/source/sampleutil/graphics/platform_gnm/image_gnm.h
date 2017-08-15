/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_IMAGE_GNM_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_IMAGE_GNM_H

//#include <gnm.h>
//#include <gnmx.h>
#include <sceerror.h>
#include <sampleutil/graphics.h>
//#include "gnm_internal.h"
#include "graphics/image.h"

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				int saveAsBmp(const char* path,  int stride,  int width,  int height,  const char * buf);				
			}

		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_IMAGE_GNM_H */
