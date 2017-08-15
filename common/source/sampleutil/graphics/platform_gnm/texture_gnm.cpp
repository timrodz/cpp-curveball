/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#include <sampleutil/graphics.h>
#include "image_gnm.h"
#include "constant_internal_gnm.h"
#include "buffer_internal_gnm.h"
#include "loader_internal_gnm.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

static sce::Gnm::WrapMode convertAddrMode(ssg::TextureAddrMode addrMode)
{
	switch(addrMode)
	{
	case ssg::kTextureAddrModeRepeat:
		{
			return sce::Gnm::kWrapModeWrap;
		}
	case ssg::kTextureAddrModeMirror:
		{
			return sce::Gnm::kWrapModeMirror;
		}
	case ssg::kTextureAddrModeClamp:
		{
			return sce::Gnm::kWrapModeClampLastTexel;
		}
	case ssg::kTextureAddrModeMirrorClamp:
		{
			return sce::Gnm::kWrapModeMirrorOnceBorder;
		}
	case ssg::kTextureAddrModeClampFullBorder:
		{
			return sce::Gnm::kWrapModeClampBorder;
		}
	default:
		{
			return sce::Gnm::kWrapModeWrap;
		}
	}
}

int ssgi::TextureImpl::initializeFromMemory(ssg::GraphicsLoader *loader, const void *data, uint32_t size)
{
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(data == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssgi::ImageFile imageFile;
	int ret = imageFile.openFromMemory(data, size);
	if(ret != SCE_OK){
		return ret;
	}

	m_textureBuffer = new Texture2dBufferImpl();
	if(m_textureBuffer == NULL){
		imageFile.close();
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}
	
	ret = m_textureBuffer->initialize(dynamic_cast<ssgi::GraphicsLoaderImpl*>(loader), kBufferFormatX8X8X8X8, imageFile.getWidth(), imageFile.getHeight(), imageFile.getMipCount(),
										  kBufferAccessModeGpuReadCpuWrite, kBufferBindFlagShaderResource, kMultisampleNone);
	if(ret != SCE_OK){
		delete m_textureBuffer;
		imageFile.close();
		return ret;
	}
	void *bufptr = m_textureBuffer->beginWrite();
	ret = imageFile.readData(bufptr);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	m_textureBuffer->endWrite();
	
	uint32_t miplevel = (imageFile.getMipCount() == 0) ? 1: imageFile.getMipCount();

	sce::Gnm::TextureSpec spec;
	spec.init();
	spec.m_textureType = sce::Gnm::kTextureType2d;
	spec.m_width = m_textureBuffer->getWidth();
	spec.m_height = m_textureBuffer->getHeight();
	spec.m_depth = 1;
	spec.m_pitch = 0;
	spec.m_numMipLevels = miplevel;
	spec.m_numSlices = 1;
	spec.m_format = sce::Gnm::kDataFormatR8G8B8A8Unorm;
	spec.m_tileModeHint = sce::Gnm::kTileModeDisplay_LinearAligned;
	spec.m_minGpuMode = sce::Gnm::kGpuModeBase;
	spec.m_numFragments = sce::Gnm::kNumFragments1;
	int32_t status = m_texture.init(&spec);
	if (status != SCE_GNM_OK)
		return status;
	sce::Gnm::SizeAlign sizeAlign = m_texture.getSizeAlign();
	
	(void)sizeAlign;
	SCE_SAMPLE_UTIL_ASSERT(sizeAlign.m_size == m_textureBuffer->getSize());
	m_texture.setBaseAddress(m_textureBuffer->getData());

	m_sampler.init();
	m_sampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
	m_sampler.setXyFilterMode(sce::Gnm::kFilterModeBilinear, sce::Gnm::kFilterModeBilinear);

	m_freeTexture = true;
	imageFile.close();

	return ret;
}





int ssgi::TextureImpl::initializeFromFile(ssg::GraphicsLoader *loader, const char *path)
{
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(path == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}


	ssgi::ImageFile imageFile;
	int ret = imageFile.open(path);
	if(ret != SCE_OK){
		return ret;
	}


	m_textureBuffer = new Texture2dBufferImpl();
	if(m_textureBuffer == NULL){
		imageFile.close();
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}
	
	if(imageFile.getFileFormat() != ssgi::ImageFile::kGnf){
		ret = m_textureBuffer->initialize(dynamic_cast<ssgi::GraphicsLoaderImpl*>(loader), kBufferFormatX8X8X8X8, imageFile.getWidth(), imageFile.getHeight(), imageFile.getMipCount(),
			kBufferAccessModeGpuReadCpuWrite, kBufferBindFlagShaderResource, kMultisampleNone);
	}else{
		ssgi::GnfFile *gnfFile = imageFile.getGnf();
		SCE_SAMPLE_UTIL_ASSERT(gnfFile != NULL);
		ret = m_textureBuffer->initializeAsUnknownBuffer(dynamic_cast<ssgi::GraphicsLoaderImpl*>(loader), 
			gnfFile->getGnmSize(), 0, 
			imageFile.getWidth(), imageFile.getHeight(), imageFile.getMipCount(),
			kBufferAccessModeGpuReadCpuWrite, kBufferBindFlagShaderResource, kMultisampleNone);
		
	}
	if(ret != SCE_OK){
		delete m_textureBuffer;
		imageFile.close();
		return ret;
	}
	void *bufptr = m_textureBuffer->beginWrite();
	ret = imageFile.readData(bufptr);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	m_textureBuffer->endWrite();

	uint32_t miplevel = (imageFile.getMipCount() == 0) ? 1: imageFile.getMipCount();
	sce::Gnm::SizeAlign sizeAlign;
	if(imageFile.getFileFormat() != ssgi::ImageFile::kGnf){

		sce::Gnm::TextureSpec spec;
		spec.init();
		spec.m_textureType = sce::Gnm::kTextureType2d;
		spec.m_width = m_textureBuffer->getWidth();
		spec.m_height = m_textureBuffer->getHeight();
		spec.m_depth = 1;
		spec.m_pitch = 0;
		spec.m_numMipLevels = miplevel;
		spec.m_numSlices = 1;
		spec.m_format = sce::Gnm::kDataFormatR8G8B8A8Unorm;
		spec.m_tileModeHint = sce::Gnm::kTileModeDisplay_LinearAligned;
		spec.m_minGpuMode = sce::Gnm::kGpuModeBase;
		spec.m_numFragments = sce::Gnm::kNumFragments1;
		int32_t status = m_texture.init(&spec);
		if (status != SCE_GNM_OK)
			return status;
		sizeAlign = m_texture.getSizeAlign();
	}else{
		ssgi::GnfFile *gnfFile = imageFile.getGnf();
		SCE_SAMPLE_UTIL_ASSERT(gnfFile != NULL);
		sizeAlign = gnfFile->getGnmSizeAlign();
		m_texture = gnfFile->getGnmTexture();
	}
	(void)sizeAlign;
	//printf(__FILE__"(%d) sizeAlign.m_size=%d, m_textureBuffer->getSize()=%d\n", __LINE__, sizeAlign.m_size, m_textureBuffer->getSize());
	SCE_SAMPLE_UTIL_ASSERT(sizeAlign.m_size == m_textureBuffer->getSize());
	m_texture.setBaseAddress(m_textureBuffer->getData());
	

	m_sampler.init();
	m_sampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
	m_sampler.setXyFilterMode(sce::Gnm::kFilterModeBilinear, sce::Gnm::kFilterModeBilinear);

	m_freeTexture = true;
	imageFile.close();
	return ret;
}


int ssgi::TextureImpl::initialize(GraphicsLoader *loader, 
	uint32_t width, uint32_t height,
	uint32_t mipCount,
	TextureFormat format)
{
	GraphicsLoaderImpl *_loader = dynamic_cast<GraphicsLoaderImpl *>(loader);
	if(_loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_textureBuffer = new Texture2dBufferImpl();

	BufferFormat bformat = getBufferFormat(format);

	int ret = m_textureBuffer->initialize(_loader, bformat, width, height, mipCount,
		ssg::kBufferAccessModeGpuReadWrite, ssg::kBufferBindFlagShaderResource,  ssg::kMultisampleNone);
	if(ret != SCE_OK){
		delete 	m_textureBuffer;
		m_textureBuffer = NULL;
		return ret;
	}
		
	sce::Gnm::TextureSpec spec;
	spec.init();
	spec.m_textureType = sce::Gnm::kTextureType2d;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depth = 1;
	spec.m_pitch = 0;
	spec.m_numMipLevels = (mipCount==0)? 1: mipCount;
	spec.m_numSlices = 1;
	spec.m_format = convertTextureFormat(format);
	spec.m_tileModeHint = sce::Gnm::kTileModeDisplay_LinearAligned;
	spec.m_minGpuMode = sce::Gnm::kGpuModeBase;
	spec.m_numFragments = sce::Gnm::kNumFragments1;
	int32_t status = m_texture.init(&spec);
	if (status != SCE_GNM_OK) {
		return -1;
	}
	sce::Gnm::SizeAlign sizeAlign = m_texture.getSizeAlign();
	(void)sizeAlign;
	SCE_SAMPLE_UTIL_ASSERT(sizeAlign.m_size == m_textureBuffer->getSize());
#ifdef SCE_GNM_INITIAL1
	m_texture.setBaseAddress256ByteBlocks( static_cast<uint32_t>(m_textureBuffer->getAddress()>>8));
#else
	m_texture.setBaseAddress(m_textureBuffer->getData());
#endif

	m_sampler.init();
	m_sampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
	m_sampler.setXyFilterMode(sce::Gnm::kFilterModeBilinear, sce::Gnm::kFilterModeBilinear);

	m_freeTexture = true;

	return SCE_OK;
}

int ssgi::TextureImpl::initializeFromBuffer(GraphicsLoader *loader, Texture2dBufferImpl *buffer, sce::Gnm::DataFormat format, sce::Gnm::TileMode tileMode)
{
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(buffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_textureBuffer = buffer;
	//printf("m_textureBuffer=%p\n", m_textureBuffer);
	
	uint32_t mipCount = buffer->getMipCount();
	sce::Gnm::TextureSpec spec;
	spec.init();
	spec.m_textureType = sce::Gnm::kTextureType2d;
	spec.m_width = buffer->getWidth();
	spec.m_height = buffer->getHeight();
	spec.m_depth = 1;
	spec.m_pitch = 0;
	spec.m_numMipLevels = (mipCount==0)? 1: mipCount;
	spec.m_numSlices = 1;
	spec.m_format = format;
	spec.m_tileModeHint = sce::Gnm::kTileModeDisplay_LinearAligned;
	spec.m_minGpuMode = sce::Gnm::kGpuModeBase;
	spec.m_numFragments = sce::Gnm::kNumFragments1;
	int32_t status = m_texture.init(&spec);
	if (status != SCE_GNM_OK) {
		return -1;
	}
	sce::Gnm::SizeAlign sizeAlign = m_texture.getSizeAlign();

	m_texture.setBaseAddress(m_textureBuffer->getData());
	(void)sizeAlign;

	m_sampler.init();
	m_sampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
	m_sampler.setXyFilterMode(sce::Gnm::kFilterModeBilinear, sce::Gnm::kFilterModeBilinear);

	m_freeTexture = false;
	return SCE_OK;
	
}


int ssgi::TextureImpl::finalize(void)
{
	if(m_freeTexture){
		if(m_textureBuffer != NULL){
			delete m_textureBuffer;
			m_textureBuffer = NULL;
		}
		m_freeTexture = false;
	}
	return SCE_OK;
}




int ssgi::TextureImpl::setAddrMode(TextureAddrMode uAddrMode,
	TextureAddrMode vAddrMode)
{
	m_sampler.setWrapMode(
		convertAddrMode(uAddrMode), 
		convertAddrMode(vAddrMode),
		sce::Gnm::kWrapModeWrap);
	return SCE_OK;
}

static sce::Gnm::FilterMode convertFilter(ssg::TextureFilter f)
{
	switch(f)
	{
	case ssg::kTextureFilterPoint:
		{
			return sce::Gnm::kFilterModePoint;
		}
	case ssg::kTextureFilterLinear:
		{
			return sce::Gnm::kFilterModeBilinear;
		}
	default:
		{
			return sce::Gnm::kFilterModePoint;
		}
	}

}

static sce::Gnm::MipFilterMode convertMipFilter(ssg::TextureMipFilter f)
{
	switch(f)
	{
	case ssg::kTextureMipFilterDisabled:
		{
			return sce::Gnm::kMipFilterModeNone;
		}
	case ssg::kTextureMipFilterEnabled:
		{
			return sce::Gnm::kMipFilterModeLinear;
		}
	default:
		{
			return sce::Gnm::kMipFilterModeNone;
		}
	}
}

int ssgi::TextureImpl::setFilter(
	TextureFilter minFilter,
	TextureFilter magFilter,
	TextureMipFilter mipFilter)
{
	m_sampler.setXyFilterMode( convertFilter(magFilter), convertFilter(minFilter));
	m_sampler.setMipFilterMode(convertMipFilter(mipFilter));
	return SCE_OK;
}


uint32_t ssgi::TextureImpl::getWidth(void) const
{
	return m_texture.getWidth();
}
uint32_t ssgi::TextureImpl::getHeight(void) const
{
	return m_texture.getHeight();
}
uint32_t ssgi::TextureImpl::getStride(void) const
{
	return m_texture.getPitch();
}


#endif /* _SCE_TARGET_OS_ORBIS */
