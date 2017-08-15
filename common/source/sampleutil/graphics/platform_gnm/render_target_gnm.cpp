/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#include <gnm.h>
#include <gnmx.h>

#include <sampleutil/graphics.h>
#include <gpu_address.h>
#include <assert.h>
#include <video_out.h>
#include "context_internal_gnm.h"


namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;


#pragma region RenderTargetImpl

ssgi::RenderTargetImpl::RenderTargetImpl(void)
{
	m_textureBuffer = NULL;
	m_freeTextureBuffer = false;
	m_initialized = false;
}


int ssgi::RenderTargetImpl::initialize(
	GraphicsLoaderImpl *loader,
	uint32_t width, uint32_t height, 
	sce::Gnm::DataFormat colorFormat, 
	sce::Gnm::TileMode tileMode,
	sce::Gnm::NumSamples numSamples, sce::Gnm::NumFragments numFragments,
	bool cmask, bool fmask)
{
	// TODO: 
	//colorFormat  = sce::Gnm::kDataFormatB8G8R8A8Unorm;
	//tileMode     = sce::Gnm::kTileModeDisplay_LinearAligned;
	//numSamples   = sce::Gnm::kNumSamples1;
	//numFragments = sce::Gnm::kNumFragments1;
	//cmask        = false;
	//fmask        = false;
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_width = width;
	m_height = height;
	//sce::Gnm::TileMode tileMode;
	sce::Gnm::SizeAlign cmaskSizeAlign;
	sce::Gnm::SizeAlign fmaskSizeAlign;

	sce::Gnm::RenderTargetSpec spec;
	spec.init();
	spec.m_width = width;
	spec.m_height = height;
	spec.m_pitch = 0;
	spec.m_numSlices = 1;
	spec.m_colorFormat = colorFormat;
	spec.m_colorTileModeHint = tileMode;
	spec.m_minGpuMode = sce::Gnm::getGpuMode();
	spec.m_numSamples = numSamples;
	spec.m_numFragments = numFragments;
	spec.m_flags.enableCmaskFastClear = 0;
	spec.m_flags.enableFmaskCompression = 0;
	int32_t status = m_renderTarget.init(&spec);
	if (status != SCE_GNM_OK)
		return -1;
	m_renderTarget.setAddresses((void*)NULL, (void*)NULL, (void*)NULL);
	cmaskSizeAlign = m_renderTarget.getCmaskSizeAlign();
	fmaskSizeAlign = m_renderTarget.getFmaskSizeAlign();
	sce::Gnm::SizeAlign sizeAlign = m_renderTarget.getColorSizeAlign();
	m_textureBuffer = new Texture2dBufferImpl;
	if(m_textureBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}

	int ret = m_textureBuffer->initializeAsUnknownBuffer(loader, 
		sizeAlign.m_size,
		sizeAlign.m_align,
		width, height, 1,
		ssg::kBufferAccessModeGpuReadWrite,  
		ssg::kBufferBindFlagShaderResource  | ssg::kBufferBindFlagRenderTarget,  
		ssg::kMultisampleNone/*todo*/);
	if(ret != SCE_OK){
		delete m_textureBuffer;
		return ret;
	}

	// TODO: tilemode texture is not implemented yet.
	SCE_SAMPLE_UTIL_ASSERT(sizeAlign.m_size == m_textureBuffer->getSize());

	m_freeTextureBuffer = true;

	void* renderTargetAddr = m_textureBuffer->getData();
	if(renderTargetAddr == 0){
		return SCE_SAMPLE_UTIL_ERROR_BUSY;
	}

	//printf(__FILE__"(%d) init cmask fmask %ld, %ld\n", __LINE__, cmaskSizeAlign.m_size, fmaskSizeAlign.m_size);
	void *cmaskAddr = 0;
	if(cmaskSizeAlign.m_size > 0){
		//printf(__FILE__"(%d) init cmask\n", __LINE__);
		cmaskAddr = loader->getHeap()->allocateVideoShared(cmaskSizeAlign.m_align, cmaskSizeAlign.m_size);
		if( cmaskAddr == NULL){
			loader->getHeap()->releaseVideoShared(renderTargetAddr);
			return SCE_SAMPLE_UTIL_ERROR_BUSY;
		}
	}

	void *fmaskAddr = NULL;
	if(	(fmaskSizeAlign.m_size > 0) && fmask){
		printf(__FILE__"(%d) init fmask\n", __LINE__);
		fmaskAddr = loader->getHeap()->allocateVideoShared(fmaskSizeAlign.m_align, fmaskSizeAlign.m_size);
		if(fmaskAddr == NULL){
			loader->getHeap()->releaseVideoShared(renderTargetAddr);
			if(cmaskAddr != NULL){
				loader->getHeap()->releaseVideoShared(cmaskAddr);
			}
			return SCE_SAMPLE_UTIL_ERROR_BUSY;
		}
	}
	//printf(__FILE__"(%d) m_renderTarget.setAddresses( %p, %p, %p )\n", __LINE__, renderTargetAddr, cmaskAddr, fmaskAddr);
	m_renderTarget.setAddresses( renderTargetAddr, cmaskAddr, fmaskAddr );

	m_loader = loader;
	
	ret = m_texture.initializeFromBuffer(loader, m_textureBuffer, colorFormat);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SCE_SAMPLE_UTIL_ASSERT(m_texture.m_textureBuffer != NULL);

	//m_texture.m_sampler.init();
	//m_texture.m_texture.initFromRenderTarget(&m_renderTarget, false);
	//m_texture.m_textureBuffer = NULL;

	m_initialized = true;


	return SCE_OK;
}
int ssgi::RenderTargetImpl::finalize(void)
{
	if(!m_initialized){
		return SCE_OK;
	}
	m_initialized = false;
	void* cmaskAddr = m_renderTarget.getCmaskAddress();
	if(cmaskAddr != NULL){
		m_loader->getHeap()->releaseVideoShared(cmaskAddr);
	}
	void* fmaskAddr = m_renderTarget.getFmaskAddress();
	if(fmaskAddr != NULL){
		m_loader->getHeap()->releaseVideoShared(fmaskAddr);
	}
	m_renderTarget.setAddresses((void*)NULL,(void*)NULL,(void*)NULL);

	if(m_freeTextureBuffer){
		if(m_textureBuffer != NULL){
			delete m_textureBuffer;
			m_textureBuffer = NULL;
		}
		m_freeTextureBuffer = false;

	}
	return SCE_OK;
}

uint32_t ssgi::RenderTargetImpl::getWidth(void) const
{
	return m_width;
}

uint32_t ssgi::RenderTargetImpl::getHeight(void) const
{
	return m_height;
}


ssgi::RenderTargetImpl::~RenderTargetImpl(void)
{
	finalize();
}

#pragma endregion // RenderTargetImpl
/* ------------------------------------------------------------------------------------------------- */
#pragma region DepthStencilSurfaceImpl

int ssgi::DepthStencilSurfaceImpl::initialize(GraphicsLoaderImpl *loader,
	                                    uint32_t width,
										uint32_t height,
	                                    bool useStencil,
										bool useHtile	)
{
	if(loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_loader = loader;
	m_width = width;
	m_height = height;

	sce::Gnm::SizeAlign stencilSizeAlign;
	sce::Gnm::SizeAlign htileSizeAlign;
	const sce::Gnm::StencilFormat stencilFormat = (useStencil) ? sce::Gnm::kStencil8 : sce::Gnm::kStencilInvalid;
	sce::Gnm::TileMode depthTileMode;
	sce::Gnm::DataFormat depthFormat = sce::Gnm::DataFormat::build(sce::Gnm::kZFormat32Float);

	int ret  = sce::GpuAddress::computeSurfaceTileMode(sce::Gnm::getGpuMode(), &depthTileMode, sce::GpuAddress::kSurfaceTypeDepthOnlyTarget, depthFormat, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, sce::GpuAddress::kStatusSuccess);
	
	sce::Gnm::DepthRenderTargetSpec spec;
	spec.init();
	spec.m_width = width;
	spec.m_height = height;
	spec.m_pitch = 0;
	spec.m_numSlices = 1;
	spec.m_zFormat = depthFormat.getZFormat();
	spec.m_stencilFormat = stencilFormat;
	spec.m_tileModeHint = depthTileMode;
	spec.m_minGpuMode = sce::Gnm::getGpuMode();
	spec.m_numFragments = sce::Gnm::kNumFragments1;
	spec.m_flags.enableHtileAcceleration = useHtile ? 1 : 0;
	int32_t status = m_depthTarget.init(&spec);
	if (status != SCE_GNM_OK)
		return status;
	if (useStencil){
		stencilSizeAlign = m_depthTarget.getStencilSizeAlign();
	}
	if (useHtile) {
		htileSizeAlign = m_depthTarget.getHtileSizeAlign();
	}
	const sce::Gnm::SizeAlign depthTargetSizeAlign = m_depthTarget.getZSizeAlign();

	void * stencilAddr = NULL;
	if( useStencil ){
		stencilAddr = loader->getHeap()->allocateVideoShared(stencilSizeAlign.m_align, stencilSizeAlign.m_size);
		if(stencilAddr == NULL){
			return SCE_SAMPLE_UTIL_ERROR_BUSY;
		}
	}
	void * htileAddr = 0;
	if( useHtile ){
		htileAddr = loader->getHeap()->allocateVideoShared(htileSizeAlign.m_align, htileSizeAlign.m_size);
		if(htileAddr == NULL){
			if(stencilAddr != NULL){
				loader->getHeap()->releaseVideoShared(stencilAddr);
			}
			return SCE_SAMPLE_UTIL_ERROR_BUSY;
		}
		m_depthTarget.setHtileAddress(htileAddr);
	}




	m_textureBuffer = new Texture2dBufferImpl;
	if(m_textureBuffer == NULL){
		if(stencilAddr != NULL){
			loader->getHeap()->releaseVideoShared(stencilAddr);
		}
		if(htileAddr != NULL){
			loader->getHeap()->releaseVideoShared(htileAddr);
		}
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}
	ret = m_textureBuffer->initializeAsUnknownBuffer(loader,
		depthTargetSizeAlign.m_size,
		depthTargetSizeAlign.m_align,
		width, height, 1,
		ssg::kBufferAccessModeGpuReadWrite,  
		ssg::kBufferBindFlagShaderResource  | ssg::kBufferBindFlagDepthStencil,
		ssg::kMultisampleNone/*todo*/);
	if(ret != SCE_OK){
		if(stencilAddr != NULL){
			loader->getHeap()->releaseVideoShared(stencilAddr);
		}
		if(htileAddr != NULL){
			loader->getHeap()->releaseVideoShared(htileAddr);
		}
		delete m_textureBuffer;
		return ret;
	}
	// TODO: tilemode texture is not implemented yet.
	SCE_SAMPLE_UTIL_ASSERT(depthTargetSizeAlign.m_size == m_textureBuffer->getSize());
	m_freeTextureBuffer = true;

	m_depthTarget.setAddresses( m_textureBuffer->getData(), stencilAddr );

	ret = m_texture.initializeFromBuffer(loader, m_textureBuffer, depthFormat, depthTileMode);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SCE_SAMPLE_UTIL_ASSERT(m_texture.m_textureBuffer != NULL);

	return SCE_OK;

}


int ssgi::DepthStencilSurfaceImpl::finalize(void)
{
	if(m_depthTarget.getStencilReadAddress() != 0){
		m_loader->getHeap()->releaseVideoShared(m_depthTarget.getStencilReadAddress());
	}
	m_depthTarget.setAddresses((void *)NULL,(void *)NULL);

	if(m_depthTarget.getHtileAddress() != 0){
		m_loader->getHeap()->releaseVideoShared(m_depthTarget.getHtileAddress());
		m_depthTarget.setHtileAddress256ByteBlocks(0);
	}

	if(m_freeTextureBuffer){
		if(m_textureBuffer != NULL){
			delete m_textureBuffer;
			m_textureBuffer = NULL;
		}
		m_freeTextureBuffer = false;
	}

	return SCE_OK;
}

ssgi::DepthStencilSurfaceImpl::~DepthStencilSurfaceImpl(void)
{
	finalize();
}

#pragma endregion // DepthStencilSurfaceImpl

#endif // _SCE_TARGET_OS_ORBIS