/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2


#include "gxm_internal.h"
#include "texture_util_gxm.h"
#include "../constant_internal.h"
#include "../graphics_internal.h"
#include "../image.h"
#include "util/util.h"
#include <sampleutil/graphics.h>

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

ssg::Gxm::TextureGxm *ssg::Gxm::TextureGxm::downcast(ssg::Texture *buffer)
{
	return dynamic_cast<ssg::Gxm::TextureGxm *>(buffer);
}

const ssg::Gxm::TextureGxm *ssg::Gxm::TextureGxm::downcast(const ssg::Texture *buffer)
{
	return dynamic_cast<const ssg::Gxm::TextureGxm *>(buffer);
}



static int initializeTextureFromImageFile(SceGxmTexture *texture, ssgi::ImageFile &imageFile, ssgi::GraphicsLoaderImpl *loader)
{
	size_t size = ssgi::getTextureSize(imageFile.getGxmType(),
									   imageFile.getGxmFormat(),
									   imageFile.getWidth(),
									   imageFile.getHeight(),
									   imageFile.getStride(),
									   imageFile.getMipCount());

	void *textureData = loader->memalign(loader->MEMTYPE_CDRAM, 256, size); // Minimum-alignment for texture is 16, but jpeg decode requires 256-byte aligned.

	int ret = imageFile.readData(textureData);
	if(ret != SCE_OK){
		loader->free(textureData);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}

	switch(imageFile.getGxmType()){
	case SCE_GXM_TEXTURE_SWIZZLED:
		{
			ret = sceGxmTextureInitSwizzled(texture,textureData,	imageFile.getGxmFormat(),	imageFile.getWidth(),	imageFile.getHeight(),	imageFile.getMipCount());
			break;
		}
	case SCE_GXM_TEXTURE_CUBE:
		{
			ret = sceGxmTextureInitCube(texture,	textureData,	imageFile.getGxmFormat(),	imageFile.getWidth(),	imageFile.getHeight(),	imageFile.getMipCount()	);
			break;
		}
	case SCE_GXM_TEXTURE_LINEAR :
		{
			ret = sceGxmTextureInitLinear(texture,  textureData,  imageFile.getGxmFormat(),	  imageFile.getWidth(),	  imageFile.getHeight(),  imageFile.getMipCount() );
			break;
		}
	case SCE_GXM_TEXTURE_TILED:
		{
			ret = sceGxmTextureInitTiled( texture,  textureData,  imageFile.getGxmFormat(),   imageFile.getWidth(),   imageFile.getHeight(),  imageFile.getMipCount()   );
			break;
		}
    case SCE_GXM_TEXTURE_SWIZZLED_ARBITRARY :
		{
			ret = sceGxmTextureInitSwizzledArbitrary(texture, textureData, imageFile.getGxmFormat(), imageFile.getWidth(), imageFile.getHeight(), imageFile.getMipCount() );
			break;
		}
    case SCE_GXM_TEXTURE_LINEAR_STRIDED :
		{
			ret = sceGxmTextureInitLinearStrided(texture, textureData, imageFile.getGxmFormat(), imageFile.getWidth(),imageFile.getHeight(), imageFile.getStride() );
			break;
		}
    case SCE_GXM_TEXTURE_CUBE_ARBITRARY :
		{
			ret = sceGxmTextureInitCubeArbitrary(texture, textureData, imageFile.getGxmFormat(),imageFile.getWidth(),imageFile.getHeight(), imageFile.getMipCount() );
			break;
		}
	default: SCE_SAMPLE_UTIL_ASSERT(0);
	}
	if(ret != SCE_OK){
		loader->free(textureData);
		return ret;
	}
	
	return SCE_OK;
}

static int _sceSampleUtilLoadImage( SceGxmTexture *texture, const char *filename, ssgi::GraphicsLoaderImpl *loader)
{
	ssgi::ImageFile imageFile;
	int ret = imageFile.open(loader, loader->getJpegDecoder(), filename);
	if(ret != SCE_OK){
		return ret;
	}
	ret = initializeTextureFromImageFile(texture, imageFile, loader);
	imageFile.close();
	return ret;
}

static int _sceSampleUtilLoadImageFromMemory( SceGxmTexture *texture, const void *image, size_t imageSize, ssgi::GraphicsLoaderImpl *loader)
{
	ssgi::ImageFile imageFile;
	int ret = imageFile.openFromMemory(loader->getJpegDecoder(), image, imageSize);
	if(ret != SCE_OK){
		printf(__FILE__"(%d) check ret=%#x\n", __LINE__, ret);
		return ret;
	}
	ret = initializeTextureFromImageFile(texture, imageFile, loader);
	imageFile.close();
	return ret; 
}





ssgi::TextureImpl::TextureImpl(void)
{
	memset(&m_texture, 0, sizeof(m_texture));
	m_buffer = NULL;
}

int ssgi::TextureImpl::initializeFromFile(GraphicsLoaderImpl *loader, const char *filename)
{
	if(loader == NULL || filename == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	int ret = _sceSampleUtilLoadImage(&m_texture, filename, loader);
	if(ret == 0){
		m_buffer = new Texture2dBufferImpl;
		ret = m_buffer->initializeFromTexture(loader,
											 &m_texture);
		if(ret != SCE_OK){
			loader->free(sceGxmTextureGetData(&m_texture));
			delete m_buffer;
			m_buffer = NULL;
			return ret;
		}
		m_freeBufferOnFinalize = true;
	}
	return ret;
}

int ssgi::TextureImpl::initializeFromMemory(GraphicsLoaderImpl *loader, const void *image, uint32_t fileSize)
{
	if(loader == NULL || image == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	int ret = _sceSampleUtilLoadImageFromMemory(&m_texture, image, fileSize, loader);
	if(ret == 0){
		m_buffer = new Texture2dBufferImpl;
		ret = m_buffer->initializeFromTexture(loader,
											 &m_texture);
		if(ret != SCE_OK){
			loader->free(sceGxmTextureGetData(&m_texture));
			delete m_buffer;
			m_buffer = NULL;
			return ret;
		}
		m_freeBufferOnFinalize = true;
	}
	return ret;
}

int ssgi::TextureImpl::initializeGxm(GraphicsLoaderImpl *loader, 
									 SceGxmTextureType type, SceGxmTextureFormat texFormat, 
									 uint32_t width, uint32_t height, uint32_t stride, uint32_t mipCount,
									 uint32_t bufferAlign)
{
	int ret;
	uint32_t size = getTextureSize(type, texFormat, width, height, stride, mipCount);
	if(size == 0){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	void *textureData = loader->memalign(loader->MEMTYPE_CDRAM, bufferAlign, size);
	switch(type)
		{
		case SCE_GXM_TEXTURE_SWIZZLED:
			{
				ret = sceGxmTextureInitSwizzled(&m_texture,	textureData,texFormat,width,height,mipCount);
				break;
			}
		case SCE_GXM_TEXTURE_CUBE:
			{
				ret = sceGxmTextureInitCube(&m_texture,	textureData,texFormat,width,height,mipCount);
				break;
			}
		case SCE_GXM_TEXTURE_LINEAR:
			{
				ret = sceGxmTextureInitLinear(&m_texture, textureData, texFormat,	width,	height,	mipCount);
				break;
			}
		case SCE_GXM_TEXTURE_TILED:					
			{
				ret = sceGxmTextureInitTiled(&m_texture, textureData, texFormat, width, height, mipCount );
				break;
			}
		case SCE_GXM_TEXTURE_SWIZZLED_ARBITRARY:		
			{
				ret = sceGxmTextureInitSwizzledArbitrary(&m_texture, textureData,	texFormat,	width,	height,	mipCount);
				break;
			}
		case SCE_GXM_TEXTURE_LINEAR_STRIDED:
			{
				ret = sceGxmTextureInitLinearStrided(&m_texture,textureData,texFormat,width,height, stride	);
				break;
			}
		case SCE_GXM_TEXTURE_CUBE_ARBITRARY:			
			{
				ret = sceGxmTextureInitCubeArbitrary(&m_texture,textureData,texFormat,width,height,	mipCount);
				break;
			}
		default:
			SCE_SAMPLE_UTIL_ASSERT(0);
			return -1;
		}
	
	if(ret != SCE_OK){
		finalize();
		return ret;
	}
	m_buffer = new Texture2dBufferImpl;
	ret = m_buffer->initializeFromTexture(loader,
										 &m_texture);
	if(ret != SCE_OK){
		loader->free(sceGxmTextureGetData(&m_texture));
		delete m_buffer;
		m_buffer = NULL;
		return ret;
	}
	m_buffer->m_freeOnFinalize = true;

	sceGxmTextureSetVAddrMode(&m_texture, SCE_GXM_TEXTURE_ADDR_CLAMP);
	sceGxmTextureSetUAddrMode(&m_texture, SCE_GXM_TEXTURE_ADDR_CLAMP);
	return SCE_OK;
}


int ssgi::TextureImpl::initializeGxmLinear( GraphicsLoaderImpl *loader,  SceGxmTextureFormat texFormat,
											uint32_t width,	  uint32_t height,  uint32_t mipCount,  uint32_t bufferAlign)
{
	return initializeGxm(loader, SCE_GXM_TEXTURE_LINEAR, texFormat, width, height, 0, mipCount, bufferAlign);
}

int ssgi::TextureImpl::initializeGxmLinearStrided( GraphicsLoaderImpl *loader, 	 SceGxmTextureFormat texFormat,
												   uint32_t width, uint32_t height, uint32_t byteStride, uint32_t bufferAlign)

{
	return initializeGxm(loader, SCE_GXM_TEXTURE_LINEAR_STRIDED, texFormat, width, height, byteStride, 0, bufferAlign);
}


int ssgi::TextureImpl::initializeGxmTiled(GraphicsLoaderImpl *loader,  SceGxmTextureFormat texFormat, uint32_t width,
										  uint32_t height,	 uint32_t mipCount,	 uint32_t bufferAlign)
{
	return initializeGxm(loader, SCE_GXM_TEXTURE_TILED, texFormat, width, height, 0, mipCount, bufferAlign);
}

int ssgi::TextureImpl::initializeGxmSwizzled(GraphicsLoaderImpl *loader, 	SceGxmTextureFormat texFormat,
											 uint32_t width, uint32_t height, uint32_t mipCount, uint32_t bufferAlign)
{
	if((width == ceilPowersOf2(width)) && (height == ceilPowersOf2(height))){
		return initializeGxm(loader, SCE_GXM_TEXTURE_SWIZZLED, texFormat, width, height, 0, mipCount, bufferAlign);
	}else{
		return initializeGxm(loader, SCE_GXM_TEXTURE_SWIZZLED_ARBITRARY, texFormat, width, height, 0, mipCount, bufferAlign);
	}
}

int ssgi::TextureImpl::initializeGxmCube(GraphicsLoaderImpl *loader, 	SceGxmTextureFormat texFormat,	uint32_t width,
										 uint32_t height,	uint32_t mipCount,	uint32_t bufferAlign)
{
	if((width == ceilPowersOf2(width)) && (height == ceilPowersOf2(height))){
		return initializeGxm(loader, SCE_GXM_TEXTURE_CUBE, texFormat, width, height, 0, mipCount, bufferAlign);
	}else{
		return initializeGxm(loader, SCE_GXM_TEXTURE_CUBE_ARBITRARY, texFormat, width, height, 0, mipCount, bufferAlign);
	}
}

static ssg::TextureFormat convertBufferFormatToTextureFormat(ssg::BufferFormat b)
{
	switch(b)
	{
	case ssg::kBufferFormatX8X8X8X8:
		return ssg::kTextureFormatA8B8G8R8Unorm;
	case ssg::kBufferFormatX8:
		return ssg::kTextureFormatR8Unorm;
	default:
		abort();
	}
}

int ssgi::TextureImpl::initializeFromTexture2dBuffer(Texture2dBufferImpl *buffer)
{
	int ret;
	const void *textureData = buffer->getData();

	//TODO: Assuming linear strided
	ret = sceGxmTextureInitLinearStrided(&m_texture,textureData,
		convertTextureFormat(convertBufferFormatToTextureFormat(buffer->getFormat())),
		buffer->getWidth(),buffer->getHeight(), buffer->getStride());
	if(ret != SCE_OK){
		finalize();
		return ret;
	}
	m_buffer = buffer;
	m_freeBufferOnFinalize = false;

	sceGxmTextureSetVAddrMode(&m_texture, SCE_GXM_TEXTURE_ADDR_CLAMP);
	sceGxmTextureSetUAddrMode(&m_texture, SCE_GXM_TEXTURE_ADDR_CLAMP);

	return SCE_OK;
}


int ssgi::TextureImpl::initialize(GraphicsLoaderImpl *loader,  TextureFormat texFormat,	 uint32_t width, uint32_t height, uint32_t mipCount)
{
	return initializeGxm(loader, SCE_GXM_TEXTURE_LINEAR, convertTextureFormat(texFormat), width, height, 0, mipCount);
}

int ssgi::TextureImpl::setFilter(TextureFilter minFilter,
							TextureFilter magFilter,
							TextureMipFilter mipFilter)
{

	int ret;
	ret = sceGxmTextureSetMagFilter(getGxmTexture(), ssgi::convertTextureFilter(magFilter));
	if(ret != SCE_OK){
		return  ret;
	}
	ret = sceGxmTextureSetMinFilter(getGxmTexture(), ssgi::convertTextureFilter(minFilter));
	if(ret != SCE_OK){
		return  ret;
	}
	ret = sceGxmTextureSetMipFilter(getGxmTexture(), ssgi::convertTextureMipFilter(mipFilter));
	if(ret != SCE_OK){
		return  ret;
	}
	return SCE_OK;
}

int ssgi::TextureImpl::setAddrMode(TextureAddrMode uAddrMode, 
                                TextureAddrMode vAddrMode)
{
	int ret = sceGxmTextureSetUAddrMode(getGxmTexture(), ssgi::convertTextureAddrMode(uAddrMode));
	if(ret < 0){
		return ret;
	}
	return sceGxmTextureSetVAddrMode(getGxmTexture(), ssgi::convertTextureAddrMode(vAddrMode));
}



uint32_t ssgi::TextureImpl::getWidth(void) const
{
	return sceGxmTextureGetWidth(&m_texture);
}
uint32_t ssgi::TextureImpl::getHeight(void) const
{
	uint32_t h = sceGxmTextureGetHeight(&m_texture);
	return h;
}
uint32_t ssgi::TextureImpl::getStride(void) const
{
	return sceGxmTextureGetStride(&m_texture);
}


int ssgi::TextureImpl::finalize(void)
{
	if(m_buffer != NULL){
		if(m_freeBufferOnFinalize){
			delete m_buffer;
		}
	}
	m_buffer =NULL;
	return SCE_OK;
}


#endif /* #if _SCE_TARGET_OS_PSP2 */