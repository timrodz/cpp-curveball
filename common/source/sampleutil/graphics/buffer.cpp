/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <sampleutil/graphics/buffer.h>
#include "graphics_internal.h"
#include "image.h"
#include <util/util.h>

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
ssg::GraphicsLoader* ssgi::BufferImpl::getLoader(void) const
{
	return m_loader;
}
