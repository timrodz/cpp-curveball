/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#include "stdafx.h"
#include <sampleutil/graphics/loader.h>
#include "util/util.h"
#include "constant_internal.h"
#include "graphics_internal.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;


int ssgi::GraphicsLoaderImpl::createIndexBuffer(ssg::IndexBuffer **outIndexBuffer, uint32_t size)
{
	if(outIndexBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	IndexBufferImpl *indexBuffer = new IndexBufferImpl();
	int ret = indexBuffer->initialize(this, size);
	if(ret != SCE_OK){
		delete indexBuffer;
		return ret;
	}
	*outIndexBuffer = indexBuffer;
	return SCE_OK;
}
int ssgi::GraphicsLoaderImpl::createVertexBuffer(ssg::VertexBuffer **outVertexBuffer, uint32_t size)
{
	if(outVertexBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	VertexBufferImpl *vertexBuffer = new VertexBufferImpl();
	int ret = vertexBuffer->initialize(this, size);
	if(ret != SCE_OK){
		delete vertexBuffer;
		return ret;
	}
	*outVertexBuffer = vertexBuffer;
	return SCE_OK;
}
int ssgi::GraphicsLoaderImpl::createUniformBuffer(ssg::UniformBuffer **outUniformBuffer, uint32_t size)
{
	if(outUniformBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	UniformBufferImpl *uniformBuffer = new UniformBufferImpl();
	int ret = uniformBuffer->initialize(this, size);
	if(ret != SCE_OK){
		delete uniformBuffer;
		return ret;
	}
	*outUniformBuffer = uniformBuffer;
	return ret;
}

#ifdef _SCE_SAMPLE_UTIL_ENABLE_COMPUTE_SHADER
int ssgi::GraphicsLoaderImpl::createStructuredBuffer(ssg::StructuredBuffer **outStructuredBuffer, uint32_t stride, uint32_t numElements)
{
	if(outStructuredBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	StructuredBufferImpl *structuredBuffer = new StructuredBufferImpl();
	int ret = structuredBuffer->initialize(this, stride, numElements);
	if(ret != SCE_OK){
		delete structuredBuffer;
		return ret;
	}
	*outStructuredBuffer = structuredBuffer;
	return ret;
}
int ssgi::GraphicsLoaderImpl::createRwStructuredBuffer(ssg::RwStructuredBuffer **outStructuredBuffer, uint32_t stride, uint32_t numElements)
{
	if(outStructuredBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	RwStructuredBufferImpl *structuredBuffer = new RwStructuredBufferImpl();
	int ret = structuredBuffer->initialize(this, stride, numElements);
	if(ret != SCE_OK){
		delete structuredBuffer;
		return ret;
	}
	*outStructuredBuffer = structuredBuffer;
	return ret;
}
#endif


#if _SCE_TARGET_OS_WINDOWS || _SCE_TARGET_OS_PSP2

int ssgi::GraphicsLoaderImpl::createTexture(ssg::Texture **outTexture, 		
									   ssg::TextureFormat texFormat, 
									   uint32_t width,	uint32_t height,	uint32_t mipCount)
{
	if(outTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	TextureImpl *tex = new TextureImpl();
#if _SCE_TARGET_OS_WINDOWS
	int ret = tex->initialize(this, texFormat, width, height, mipCount, 
							  kBufferAccessModeGpuReadCpuWrite, kBufferBindFlagShaderResource
							  );
#else
	int ret = tex->initialize(this, texFormat, width, height, mipCount);
#endif
	if(ret != SCE_OK){
		delete tex;
		return ret;
	}
	*outTexture = tex;
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::createRenderTarget(ssg::RenderTarget **outRenderTarget, 
					   uint32_t width, 
					   uint32_t height)
{
	if(outRenderTarget == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	RenderTargetImpl *rt = new RenderTargetImpl();
	int ret = rt->initialize(this, width, height);
	if(ret != SCE_OK){
		delete rt;
		return ret;
	}
	*outRenderTarget = rt;
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::createDepthStencilSurface(ssg::DepthStencilSurface **outDepthStencilSurface, 
							 uint32_t width, 
							 uint32_t height)
{
	if(outDepthStencilSurface == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	DepthStencilSurfaceImpl *ds = new DepthStencilSurfaceImpl();
#if _SCE_TARGET_OS_WINDOWS
	int ret = ds->initialize(this, width, height, DXGI_FORMAT_D16_UNORM, kMultisampleNone);
#elif _SCE_TARGET_OS_PSP2
	int ret = ds->initialize(this, width, height, 
		SCE_GXM_DEPTH_STENCIL_FORMAT_D16,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		SCE_GXM_MULTISAMPLE_NONE,
		true, //force store
		false); // force load

#else
	int ret = ds->initialize(this, width, height);
#endif
	if(ret != SCE_OK){
		delete ds;
		return ret;
	}
	*outDepthStencilSurface = ds;
	return SCE_OK;
}



#endif // _SCE_TARGET_OS_ORBIS




int ssgi::GraphicsLoaderImpl::createTextureFromFile(
					ssg::Texture **outTexture, 
					const char *filename)
{
	if(outTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if(filename == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	TextureImpl *texture = new TextureImpl;
	int ret = texture->initializeFromFile(this, filename);
	if(ret < SCE_OK){
		delete texture;
		return ret;
	}
	*outTexture = texture;
	return ret;
}

int ssgi::GraphicsLoaderImpl::createTextureFromMemory(
	ssg::Texture **outTexture, 
	const void *data, uint32_t size)
{
	if(outTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	TextureImpl *texture = new TextureImpl();
	int ret = texture->initializeFromMemory(this, data, size);
	if(ret < SCE_OK){
		delete texture;
		return ret;
	}
	*outTexture = texture;
	return ret;
}



int ssgi::GraphicsLoaderImpl::createBuffer(ssg::Buffer **outDataBuffer,
											   size_t              size,
											   ssg::BufferAccessMode    accessMode,
											   uint32_t            bufferBindFlags,
											   size_t              elementSize)
{
	if(outDataBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssgi::BufferImpl *impl = new ssgi::BufferImpl();
	int ret = impl->initialize(this, size, accessMode, bufferBindFlags, elementSize);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outDataBuffer = impl;
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::createTexture2dBuffer(ssg::Texture2dBuffer **outTexture2dBuffer,
													ssg::BufferFormat format,
													uint32_t width,
													uint32_t height,
													uint32_t mipCount,
													ssg::BufferAccessMode accessMode,
													uint32_t bufferBindFlags, /* BufferBindFlag */
													ssg::MultisampleMode multiSampleMode
													)
{
	if(outTexture2dBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssgi::Texture2dBufferImpl *impl = new ssgi::Texture2dBufferImpl();
	int ret = impl->initialize(this, format,
							   width,
							   height,
							   mipCount,
							   accessMode,
							   bufferBindFlags, 
							   multiSampleMode);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outTexture2dBuffer = impl;
	return SCE_OK;
}

#if _SCE_TARGET_OS_ORBIS

#include "platform_gnm/constant_internal_gnm.h"

int ssgi::GraphicsLoaderImpl::createTexture2dBufferUsingExistentBuffer(
	                                                ssg::Texture2dBuffer **outTexture2dBuffer,
													ssg::BufferFormat format,
													uint32_t width,
													uint32_t height,
													uint32_t mipCount,
													ssg::BufferAccessMode accessMode,
													uint32_t bufferBindFlags, /* BufferBindFlag */
													ssg::MultisampleMode multiSampleMode,
													void *buffer
													)
{
	if(outTexture2dBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssgi::Texture2dBufferImpl *impl = new ssgi::Texture2dBufferImpl();
	int ret = impl->initialize(this, format,
							   width,
							   height,
							   mipCount,
							   accessMode,
							   bufferBindFlags, 
							   multiSampleMode,
							   buffer);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outTexture2dBuffer = impl;
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::createTextureFromBuffer(Texture **outTexture, Texture2dBuffer *buffer, TextureFormat texFormat)
{
	if(outTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssgi::Texture2dBufferImpl *bufferImpl = dynamic_cast<ssgi::Texture2dBufferImpl*>(buffer);
	if(bufferImpl == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssgi::TextureImpl *impl = new ssgi::TextureImpl();
	int ret = impl->initializeFromBuffer(this, bufferImpl, ssgi::convertTextureFormat(texFormat));
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outTexture = impl;
	return SCE_OK;

}

#endif /* _SCE_TARGET_OS_ORBIS */
