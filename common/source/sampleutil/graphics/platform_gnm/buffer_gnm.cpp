/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#pragma comment(lib,"libSceVideoOut_stub_weak.a")

#include <sampleutil/graphics.h>
#include "util/util.h"
#include "image_gnm.h"
#include "buffer_internal_gnm.h"
#include "loader_internal_gnm.h"
#include "constant_internal_gnm.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;

int ssgi::BufferImpl::initialize(GraphicsLoaderImpl *loader, uint32_t size, 
								 BufferAccessMode    accessMode,
								 uint32_t            bufferBindFlags,
								 size_t              elementSize,
								 uint32_t alignment)
{
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_loader = loader;
	size = ssiu::ceil(size, alignment);

	uint64_t addr = m_loader->getHeap()->allocateVideoMemoryBlock(&m_buffer, size, alignment);

	m_elementSize = elementSize;
	m_accessMode = accessMode;
	m_bufferBindFlags = bufferBindFlags;
	
	return (addr!=0) ? SCE_OK : SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	
}
int ssgi::BufferImpl::finalize(void)
{
	if(m_buffer.m_ptr != NULL){
		m_loader->getHeap()->releaseMemoryBlock(&m_buffer);
	}
	return SCE_OK;
}


ssg::Gnm::BufferGnm *ssg::Gnm::BufferGnm::cast(ssg::BufferInterface *buffer)
{
	return dynamic_cast<ssg::Gnm::BufferGnm *>(buffer);
}
const ssg::Gnm::BufferGnm *ssg::Gnm::BufferGnm::cast(const ssg::BufferInterface *buffer)
{
	return dynamic_cast<const ssg::Gnm::BufferGnm *>(buffer);
}



int ssgi::IndexBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	
	int ret = m_dataBuffer.initialize(loader, size, 
									  kBufferAccessModeGpuReadCpuWrite,
									  kBufferBindFlagIndexBuffer, 0);
	if(ret != SCE_OK){
		return ret;
	}
	return ret;
}

int ssgi::VertexBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	int ret = m_dataBuffer.initialize(loader, size,
								   kBufferAccessModeGpuReadCpuWrite,
								   kBufferBindFlagVertexBuffer, 0);
	return ret;
}

int ssgi::UniformBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	int ret = m_dataBuffer.initialize(loader, size,
		kBufferAccessModeGpuReadCpuWrite,
		kBufferBindFlagConstantBuffer, 0);
	return ret;
}


/* ------------------------------------------------------------------------------------------------------------------ */




ssgi::Texture2dBufferImpl::Texture2dBufferImpl(void)
{

}
ssgi::Texture2dBufferImpl::~Texture2dBufferImpl(void)
{
	finalize();
}

int ssgi::Texture2dBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader,
									ssg::BufferFormat format,
									uint32_t width,
									uint32_t height,
									uint32_t mipCount,
									ssg::BufferAccessMode accessMode,
									uint32_t bufferBindFlags, /* BufferBindFlag */
									ssg::MultisampleMode multiSampleMode
									)
{
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_width = width;
	m_height = height;
	m_mipCount = mipCount;
	
	m_format = format;
	m_accessMode = accessMode;
	m_bufferBindFlags = bufferBindFlags; 
	m_multiSampleMode = multiSampleMode;

	m_loader = loader;

	sce::Gnm::DataFormat dataformat = ssgi::convertBufferFormat(format);
	uint32_t pixSize                = ssgi::getDataSize(format);
	(void)pixSize;

	sce::Gnm::TextureSpec spec;
	spec.init();
	spec.m_textureType = sce::Gnm::kTextureType2d;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depth = 1;
	spec.m_pitch = 0;
	spec.m_numMipLevels = 1;
	spec.m_numSlices = 1;
	spec.m_format = dataformat;
	spec.m_tileModeHint = sce::Gnm::kTileModeDisplay_LinearAligned;
	spec.m_minGpuMode = sce::Gnm::kGpuModeBase;
	spec.m_numFragments = sce::Gnm::kNumFragments1;
	int32_t status = m_texture.init(&spec);
	if (status != 0) {
		return -1;
	}
	sce::Gnm::SizeAlign sizeAlign = m_texture.getSizeAlign();
	uint32_t alignedImageSize = sizeAlign.m_size;
	uint64_t addr = m_loader->getHeap()->allocateVideoMemoryBlock(&m_buffer, alignedImageSize, sizeAlign.m_align);
	SCE_SAMPLE_UTIL_ASSERT(getData() != NULL);

	m_freeBuffer = true;
	return (addr!=0) ? SCE_OK : SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
}

int ssgi::Texture2dBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader,
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
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_width = width;
	m_height = height;
	m_mipCount = mipCount;
	
	m_format = format;
	m_accessMode = accessMode;
	m_bufferBindFlags = bufferBindFlags; 
	m_multiSampleMode = multiSampleMode;

	m_loader = loader;

	sce::Gnm::DataFormat dataformat = ssgi::convertBufferFormat(format);
	uint32_t pixSize                = ssgi::getDataSize(format);
	(void)pixSize;

	sce::Gnm::TextureSpec spec;
	spec.init();
	spec.m_textureType = sce::Gnm::kTextureType2d;
	spec.m_width = width;
	spec.m_height = height;
	spec.m_depth = 1;
	spec.m_pitch = 0;
	spec.m_numMipLevels = 1;
	spec.m_numSlices = 1;
	spec.m_format = dataformat;
	spec.m_tileModeHint = sce::Gnm::kTileModeDisplay_LinearAligned;
	spec.m_minGpuMode = sce::Gnm::kGpuModeBase;
	spec.m_numFragments = sce::Gnm::kNumFragments1;
	int32_t status = m_texture.init(&spec);
	if (status != 0) {
		return -1;
	}
	sce::Gnm::SizeAlign sizeAlign = m_texture.getSizeAlign();

	m_buffer.m_ptr  = (uint32_t*)buffer;
	m_buffer.m_addr = (uintptr_t)buffer;
	m_buffer.m_size = sizeAlign.m_size;
	m_freeBuffer = false;
	return SCE_OK;
}


int ssgi::Texture2dBufferImpl::initializeAsUnknownBuffer(ssgi::GraphicsLoaderImpl *loader,
	size_t bufferSize,
	size_t alignment,
	uint32_t width,
	uint32_t height,
	uint32_t mipCount,
	
	ssg::BufferAccessMode accessMode,
	uint32_t bufferBindFlags, /* BufferBindFlag */
	ssg::MultisampleMode multiSampleMode

	)
{
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_width = width;
	m_height = height;
	m_mipCount = mipCount;
	
	m_format = ssg::kBufferFormatUnknown;
	m_accessMode = accessMode;
	m_bufferBindFlags = bufferBindFlags; 
	m_multiSampleMode = multiSampleMode;

	m_loader = loader;

	uint32_t alignedImageSize = bufferSize;
	
	uint64_t addr = m_loader->getHeap()->allocateVideoMemoryBlock(&m_buffer, alignedImageSize, 
		(alignment==0) ? sce::Gnm::kAlignmentOfShaderInBytes : alignment);
	SCE_SAMPLE_UTIL_ASSERT(getData() != NULL);
	m_freeBuffer = true;
	return (addr!=0) ? SCE_OK : SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
}



ssg::GraphicsLoader* ssgi::Texture2dBufferImpl::getLoader(void) const
{ 
	return m_loader; 
}



int ssgi::Texture2dBufferImpl::finalize(void)
{
	if(m_freeBuffer){
		if(m_buffer.m_ptr != NULL){
			m_loader->getHeap()->releaseMemoryBlock(&m_buffer);
		}
	}
	m_buffer.m_ptr = NULL;
	return SCE_OK;	
}


#endif //_SCE_TARGET_OS_ORBIS
