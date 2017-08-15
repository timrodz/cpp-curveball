/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/graphics/platform.h>
#include "gxm_internal.h"

#include "../graphics_internal.h"
#include "util/util.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

ssg::Gxm::BufferGxm* ssg::Gxm::BufferGxm::downcast(ssg::BufferInterface *buffer)
{
	return dynamic_cast<Gxm::BufferGxm*>(buffer);
}
const ssg::Gxm::BufferGxm* ssg::Gxm::BufferGxm::downcast(const ssg::BufferInterface *buffer)
{
	return dynamic_cast<const Gxm::BufferGxm*>(buffer);
}


ssgi::BufferImpl::BufferImpl(void)
{
	m_data = NULL;
	m_loader = NULL;
}
int ssgi::BufferImpl::initialize(GraphicsLoaderImpl *loader, uint32_t size, 
								 BufferAccessMode    accessMode,
								 uint32_t            bufferBindFlags,
								 size_t              elementSize,
								 uint32_t bufferAlign)
{
	SCE_SAMPLE_UTIL_ASSERT(loader != NULL);
	m_loader = loader;
	m_data = loader->memalign(loader->MEMTYPE_CDRAM, bufferAlign, size);
	if(m_data == NULL){
		return -1;
	}
	m_size = size;

	m_elementSize = elementSize;
	
	m_accessMode = accessMode;
	m_bufferBindFlags = bufferBindFlags;

	return 0;
}

int ssgi::BufferImpl::finalize(void)
{
	Gxm::GraphicsLoaderGxm *loader = dynamic_cast<Gxm::GraphicsLoaderGxm *>(m_loader);
	if(m_data != NULL){
		loader->free(m_data);
		m_data = NULL;
	}
	return 0;
}
void *ssgi::BufferImpl::getData(void)
{
	return m_data;
}
const void *ssgi::BufferImpl::getData(void) const
{
	return m_data;
}

void *ssgi::BufferImpl::beginWrite(void)
{
	return m_data;
}
void ssgi::BufferImpl::endWrite(void){}


ssgi::BufferImpl::~BufferImpl(void)
{
	finalize();
}

/* ---------------------------------------------------------------------------------------- */



ssgi::Texture2dBufferImpl::Texture2dBufferImpl(void)
{
	m_data = NULL;
}

ssgi::Texture2dBufferImpl::~Texture2dBufferImpl(void)
{
	finalize();
}

int ssgi::Texture2dBufferImpl::finalize(void)
{
	if((m_data != NULL) && (m_loader!=NULL)){
		if(m_freeOnFinalize){
			m_loader->free(m_data);
		}
	}
	m_data = NULL;
	m_loader = NULL;
	m_freeOnFinalize = false;

	return SCE_OK;
}

static uint32_t getBufferFormatSize( ssg::BufferFormat format)
{
	switch(format)
	{
	case ssg::kBufferFormatUnknown: return 0;
	case ssg::kBufferFormatX8X8X8X8: return 4;
	case ssg::kBufferFormatX8X8: return 2;
	case ssg::kBufferFormatX8 : return 1;
	case ssg::kBufferFormatX16X16X16X16: return 8;
		case ssg::kBufferFormatX16X16: return 4;
		case ssg::kBufferFormatX16: return 2;
		case ssg::kBufferFormatX32X8X24 : return 8;
		case ssg::kBufferFormatX32 : return 4;
		case ssg::kBufferFormatX24X8 : return 4;
		default: abort();
	}
}



int ssgi::Texture2dBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader,
										  ssg::BufferFormat format,
										  uint32_t width,
										  uint32_t height,
										  uint32_t mipCount,
										  ssg::BufferAccessMode accessMode,
										  uint32_t bufferBindFlags, /* BufferBindFlag */
										  ssg::MultisampleMode multiSampleMode,
										  uint32_t bufferAlign
								   )
{
	m_width = width;
	m_height = height;
	m_mipCount = mipCount;
	m_stride = width * getBufferFormatSize(format);
	m_type = SCE_GXM_TEXTURE_LINEAR;

	m_format = format;
	m_accessMode = accessMode;
	m_bufferBindFlags = bufferBindFlags;
	m_multiSampleMode = multiSampleMode;

	size_t size = m_stride * m_height; // TODO: mipCount

	m_size = size;
	bufferAlign = maxUint32(16, bufferAlign);
	m_data = loader->memalign(loader->MEMTYPE_CDRAM, bufferAlign, size);
	m_loader = loader;
	m_freeOnFinalize = true;
	return SCE_OK;
	
}

int ssgi::Texture2dBufferImpl::initializeFromTexture(ssgi::GraphicsLoaderImpl *loader,  SceGxmTexture *texture)
{
	m_width = sceGxmTextureGetWidth(texture);
	m_height = sceGxmTextureGetHeight(texture);
	m_mipCount = sceGxmTextureGetMipmapCount(texture);
	m_stride = sceGxmTextureGetStride(texture);
	m_type = sceGxmTextureGetType(texture);
					
	m_format = ssg::kBufferFormatUnknown;
	m_accessMode = ssg::kBufferAccessModeGpuReadCpuWrite;
	m_bufferBindFlags = ssg::kBufferBindFlagShaderResource;
	m_multiSampleMode = ssg::kMultisampleNone;

	m_size = ssgi::getTextureSize(texture);
	m_data = sceGxmTextureGetData(texture);
	m_loader = loader;
	m_freeOnFinalize = false;
	return SCE_OK;
	
}

int ssgi::Texture2dBufferImpl::initializeFromExistentBuffer(ssgi::GraphicsLoaderImpl *loader,
															void *data,
															BufferFormat format,
															uint32_t width,
															uint32_t height,
															uint32_t stride,
															BufferAccessMode accessMode,
															uint32_t bufferBindFlags, /* BufferBindFlag */
															MultisampleMode multiSampleMode,
															uint32_t bufferAlign
															)
{
	m_width = width;
	m_height = height;
	m_mipCount = 0;
	m_stride = stride;
	m_type = SCE_GXM_TEXTURE_LINEAR_STRIDED;

	m_format = format;
	m_accessMode = accessMode;
	m_bufferBindFlags = bufferBindFlags;
	m_multiSampleMode = multiSampleMode;

	size_t size = m_stride * m_height; // This is ok, because LINEAR_STRIDED and mip=0 are assumed.

	m_size = size;
	bufferAlign = maxUint32(16, bufferAlign);
	m_data = data;
	m_loader = loader;
	m_freeOnFinalize = false;
	return SCE_OK;
}



uint32_t ssgi::Texture2dBufferImpl::getWidth(void) const
{
	return m_width;
}
uint32_t ssgi::Texture2dBufferImpl::getHeight(void) const
{
	return m_height;
}
uint32_t ssgi::Texture2dBufferImpl::getStride(void) const
{
	return m_stride;
}
uint32_t ssgi::Texture2dBufferImpl::getMipCount(void) const
{
	return m_mipCount;
}

ssg::GraphicsLoader* ssgi::Texture2dBufferImpl::getLoader(void) const 
{ 
	return m_loader; 
}


/* ---------------------------------------------------------------------------------------- */

int ssgi::VertexBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	return m_dataBuffer.initialize(loader, size, 
								   kBufferAccessModeGpuReadCpuWrite,
								   kBufferBindFlagVertexBuffer, 0);
}
 
int ssgi::IndexBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	return m_dataBuffer.initialize(loader, size, 
								   kBufferAccessModeGpuReadCpuWrite,
								   kBufferBindFlagIndexBuffer, 0);
}

int ssgi::UniformBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	return m_dataBuffer.initialize(loader, size, 
								   kBufferAccessModeGpuReadCpuWrite,
								   kBufferBindFlagConstantBuffer, 0);
}

/* ---------------------------------------------------------------------------------------- */






/* ---------------------------------------------------------------------------------------- */
int ssg::Gxm::CommonDialogUpdateParam::initialize(RenderTarget        *renderTarget,
											DepthStencilSurface *depthStencilSurface)
{
	if(renderTarget ==NULL){
		return -1;
	}
	m_renderTarget = renderTarget;
	ssgi::RenderTargetImpl *rt = dynamic_cast<ssgi::RenderTargetImpl *>(renderTarget);
	m_depthStencilSurface = depthStencilSurface;
	ssgi::DepthStencilSurfaceImpl *ds = dynamic_cast<ssgi::DepthStencilSurfaceImpl *>(m_depthStencilSurface);


	this->renderTarget.colorFormat		= sceGxmColorSurfaceGetFormat(rt->getGxmColorSurface());
	this->renderTarget.surfaceType		= sceGxmColorSurfaceGetType(rt->getGxmColorSurface());
	this->renderTarget.width			= rt->getWidth();
	this->renderTarget.height			= rt->getHeight();
	this->renderTarget.strideInPixels	= sceGxmColorSurfaceGetStrideInPixels(rt->getGxmColorSurface());
	this->renderTarget.colorSurfaceData	= sceGxmColorSurfaceGetData(rt->getGxmColorSurface());
	this->renderTarget.depthSurfaceData	= (ds != NULL) ? ds->getGxmDepthStencilSurface()->depthData : NULL;
	this->displaySyncObject				= rt->getGxmFragmentSyncObject();
	return SCE_OK;
}

uint32_t ssgi::RenderTargetImpl::getWidth(void) const
{
	return m_colorTexture.getWidth();
}

uint32_t ssgi::RenderTargetImpl::getHeight(void) const
{
	return m_colorTexture.getHeight();
}


#endif /* _SCE_TARGET_OS_PSP2 */


