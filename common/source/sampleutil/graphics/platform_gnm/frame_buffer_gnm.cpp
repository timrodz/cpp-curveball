/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#define TILED

#pragma comment(lib,"libSceGpuAddress.a")

#include <gnm.h>
#include <gnmx.h>
#include <shader.h>


#include <sampleutil/graphics.h>
#include <gpu_address.h>
#include <assert.h>
#include <video_out.h>
#include "context_internal_gnm.h"
#include <queue>
#include <algorithm>

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

/* ------------------------------------------------------------------------------------------------------- */

#pragma region FrameBuffer_VideoInfo

bool ssgi::FrameBuffer::isDisplaymodeSupported(ssg::GraphicsContext::DisplayMode mode)
{
	if(mode == GraphicsContext::kDisplayModeNormal){
		return true;
	}
	return m_videoInfo.isDisplaymodeSupported(mode);
}
int ssgi::FrameBuffer::setDisplaymode(ssg::GraphicsContext::DisplayMode m)
{
	return m_videoInfo.setDisplaymode(m);
}

/* ------------------------------------------------------------------------------------------------------- */

bool ssgi::FrameBuffer::VideoInfo::isDisplaymodeSupported(ssg::GraphicsContext::DisplayMode mode)
{
	if(mode == GraphicsContext::kDisplayModeNormal){
		return true;
	}

	SceVideoOutDeviceCapabilityInfo info;
	int ret = sceVideoOutGetDeviceCapabilityInfo(m_handle, &info);
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
	
	if(mode == GraphicsContext::kDisplayModeS3dView){
		if (info.capability & SCE_VIDEO_OUT_DEVICE_CAPABILITY_S3D_FRAME_PACKING_59_94HZ) {
			return true;
	    }
		return false;
	}else if(mode == GraphicsContext::kDisplayModeVrView90Hz){
		if (info.capability & SCE_VIDEO_OUT_DEVICE_CAPABILITY_VR_VIEW_89_91HZ) {
			return true;
	    }
		return false;
	}else if(mode == GraphicsContext::kDisplayModeVrView120Hz){
		if (info.capability & SCE_VIDEO_OUT_DEVICE_CAPABILITY_VR_VIEW_119_88HZ) {
			return true;
	    }
		return false;
	}
	
	return false;
}
int ssgi::FrameBuffer::VideoInfo::setDisplaymode(ssg::GraphicsContext::DisplayMode m)
{
	if(!isDisplaymodeSupported(m)){
		return -1;
	}

	int ret;
	if(m_displayMode == m){
		return SCE_OK;
	}
	
	{
		ret	= sceVideoOutIsFlipPending( m_handle );
		while( 0 < ret )
		{
			ret	= sceVideoOutIsFlipPending( m_handle );
		}
	}
	ret	= sceVideoOutSubmitFlip( m_handle, SCE_VIDEO_OUT_BUFFER_INDEX_BLANK, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0 );

	if( SCE_OK != ret )
	{
		return	ret;
	}
	
	if(m == GraphicsContext::kDisplayModeS3dView){
		SceVideoOutMode mode;
		sceVideoOutModeSetAny(&mode);
		mode.resolution  = SCE_VIDEO_OUT_RESOLUTION_ANY_S3D;
		ret = sceVideoOutConfigureOutputMode(m_handle,
											 /*option*/ 0,
											 &mode,
											 /*pColor*/NULL);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		m_displayMode = m;
		return SCE_OK;
	}else if(m == GraphicsContext::kDisplayModeVrView90Hz){
		SceVideoOutMode mode;
		sceVideoOutModeSetAny(&mode);
		mode.resolution = SCE_VIDEO_OUT_RESOLUTION_ANY_VR_VIEW;
		mode.refreshRate = SCE_VIDEO_OUT_REFRESH_RATE_89_91HZ;
		ret = sceVideoOutConfigureOutputMode(m_handle,
											 /*option*/ 0,
											 &mode,
											 /*pColor*/NULL);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		m_displayMode = m;
		return SCE_OK;
	}else if(m == GraphicsContext::kDisplayModeVrView120Hz){
		SceVideoOutMode mode;
		sceVideoOutModeSetAny(&mode);
		mode.resolution = SCE_VIDEO_OUT_RESOLUTION_ANY_VR_VIEW;
		mode.refreshRate = SCE_VIDEO_OUT_REFRESH_RATE_119_88HZ;
		ret = sceVideoOutConfigureOutputMode(m_handle,
											 /*option*/ 0,
											 &mode,
											 /*pColor*/NULL);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		m_displayMode = m;
		return SCE_OK;
	}else{
		SceVideoOutMode mode;
		sceVideoOutModeSetAny(&mode);
		ret = sceVideoOutConfigureOutputMode(m_handle,
											 /*option*/ 0,
											 &mode,
											 /*pColor*/NULL);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		m_displayMode = m;
		return SCE_OK;
	}
}


int ssgi::FrameBuffer::VideoInfo::initialize(uint32_t width, uint32_t height, uint32_t pitch, 
										   uint32_t numBuffers, 
										   void* const * normalBuffers,
										   const SceVideoOutStereoBuffers *buffers,
										   void* const * vrBuffers, 
										   bool enableVrView, bool enableS3dView
										   )
{
	int ret;
	const uint32_t kPlayerId = 0;
	m_handle = sceVideoOutOpen(kPlayerId, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);
	SCE_SAMPLE_UTIL_ASSERT(m_handle >= 0);

	ret = sceKernelCreateEqueue(&m_eq, __FUNCTION__);
	SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
	ret=sceVideoOutAddFlipEvent(m_eq, m_handle, NULL);
	SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
	
	m_numBuffers = numBuffers;
	m_enableVrView = enableVrView;
	m_enableS3dView = enableS3dView;

	m_flipCount = 1;
	{
		SceVideoOutBufferAttribute attribute;
		sceVideoOutSetBufferAttribute(&attribute,
			SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
#ifdef TILED
			SCE_VIDEO_OUT_TILING_MODE_TILE,
#else
			SCE_VIDEO_OUT_TILING_MODE_LINEAR,
#endif
			SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
			width, height, pitch
			);

		m_firstNormalBufferSet = sceVideoOutRegisterBuffers(m_handle, 0, normalBuffers, numBuffers, &attribute);
		SCE_SAMPLE_UTIL_ASSERT(m_firstNormalBufferSet >= 0);
		m_firstNormalBufferIndex = 0;
		m_useFirstNormalBuffer = true;
	}

	{
		m_secondNormalBufferSet = -1;
		m_secondNormalBufferIndex = numBuffers;
	}

	if(m_enableS3dView){
		SceVideoOutBufferAttribute attribute;
		sceVideoOutSetBufferAttribute(&attribute,
			SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
#ifdef TILED
			SCE_VIDEO_OUT_TILING_MODE_TILE,
#else
			SCE_VIDEO_OUT_TILING_MODE_LINEAR,
#endif
			SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
			1280, 720, 1280
			);
		
		ret= sceVideoOutRegisterStereoBuffers(m_handle, numBuffers * 3, buffers, numBuffers, &attribute );
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		m_S3dBufferIndexStart = numBuffers * 3;
	}
	
	if(m_enableVrView){
		SceVideoOutBufferAttribute attribute;
		sceVideoOutSetBufferAttribute(&attribute,
			SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
#ifdef TILED
			SCE_VIDEO_OUT_TILING_MODE_TILE,
#else
			SCE_VIDEO_OUT_TILING_MODE_LINEAR,
#endif
			SCE_VIDEO_OUT_ASPECT_RATIO_16_9,

			1920, 1080, 1920
		);

		attribute.option = SCE_VIDEO_OUT_BUFFER_ATTRIBUTE_OPTION_VR;
		
		ret= sceVideoOutRegisterBuffers(m_handle, numBuffers * 2, vrBuffers, numBuffers, &attribute );
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		m_VrBufferIndexStart = numBuffers * 2;
	}

	SceVideoOutFlipStatus status;
	ret = sceVideoOutGetFlipStatus(m_handle, &status);
	SCE_SAMPLE_UTIL_ASSERT(ret >= 0);

	m_displayMode = GraphicsContext::kDisplayModeNormal;

	return SCE_OK;
}
int ssgi::FrameBuffer::VideoInfo::unregisterBackFrameBuffers(void)
{
	int ret;
	if(m_useFirstNormalBuffer){
		ret = sceVideoOutUnregisterBuffers(m_handle, m_secondNormalBufferSet);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		m_secondNormalBufferSet = -1;
	}else {
		ret = sceVideoOutUnregisterBuffers(m_handle, m_firstNormalBufferSet);
		m_firstNormalBufferSet = -1;
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
	}
	return 0;
}
int ssgi::FrameBuffer::VideoInfo::registerBackFrameBuffers(uint32_t width, uint32_t height, uint32_t pitch, void*const * buffers)
{
	if(m_useFirstNormalBuffer){
		SCE_SAMPLE_UTIL_ASSERT(m_secondNormalBufferSet < 0);
	}else {
		SCE_SAMPLE_UTIL_ASSERT(m_firstNormalBufferSet < 0);
	}

	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute,
		SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
#ifdef TILED
		SCE_VIDEO_OUT_TILING_MODE_TILE,
#else
		SCE_VIDEO_OUT_TILING_MODE_LINEAR,
#endif
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		width, height, pitch
		);
	int bufferSet = sceVideoOutRegisterBuffers(m_handle,
		m_useFirstNormalBuffer ? m_secondNormalBufferIndex : m_firstNormalBufferIndex, 
		buffers, m_numBuffers, &attribute);
	SCE_SAMPLE_UTIL_ASSERT(bufferSet >= 0);

	if(m_useFirstNormalBuffer){
		m_secondNormalBufferSet = bufferSet;
	}else{
		m_firstNormalBufferSet = bufferSet;
	}
	return 0;
}

int ssgi::FrameBuffer::VideoInfo::finalize(void)
{
	int ret;
	if(m_handle >= 0){
		sceVideoOutClose(m_handle);
		m_handle = -1;
	}
	if(m_eq != NULL){
		ret = sceKernelDeleteEqueue(m_eq);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		m_eq = NULL;
	}
	(void)ret;
	return SCE_OK;
}



void ssgi::FrameBuffer::VideoInfo::dump(void)
{
	printf(" ------------ dump -----------\n");

	sce::Gnm::debugHardwareStatus(sce::Gnm::kHardwareStatusDump);
	SceVideoOutFlipStatus status;
	int ret = sceVideoOutGetFlipStatus(m_handle, &status);
	SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
	printf("status.count = %ld\n", status.count);
	printf("status.processTime = %ld\n", status.processTime);
	printf("status.tsc = %ld\n", status.tsc);
	printf("status.flipArg = %#lx\n", status.flipArg);
	printf("status.gcQueueNum = %d\n", status.gcQueueNum);
	printf("status.flipPendingNum = %d\n", status.flipPendingNum);
	printf("status.currentBuffer = %d\n", status.currentBuffer);

	(void)ret;
}

void ssgi::FrameBuffer::VideoInfo::pendingCheck(uint32_t count)
{
	while(1){
		SceVideoOutFlipStatus status;
		int ret = sceVideoOutGetFlipStatus(m_handle, &status);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		if(status.flipPendingNum <= count){
			break;
		}
		dump();
		(void)ret;
	}
}

void ssgi::FrameBuffer::VideoInfo::waitUntilSafeForRenderingByCpu(uint32_t bufferIndex)
{
	while (1){
		SceVideoOutFlipStatus status;
		int ret = sceVideoOutGetFlipStatus(m_handle, &status);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		
		std::vector<int>::iterator it = std::find(m_pendingFlipIndices.begin(), m_pendingFlipIndices.end(), status.currentBuffer);
		if (it != m_pendingFlipIndices.end()){
			// found
			++it;
			m_pendingFlipIndices.erase(m_pendingFlipIndices.begin(), it);
		}

		if ((bufferIndex != status.currentBuffer) && 
			(std::find(m_pendingFlipIndices.begin(), m_pendingFlipIndices.end(), bufferIndex)
			== m_pendingFlipIndices.end()))
		{
			return;
		}

		SceKernelEvent ev;
		int numEvent;
		ret = sceKernelWaitEqueue(m_eq, &ev, 1, &numEvent, NULL);
	}
}

void ssgi::FrameBuffer::VideoInfo::submitFlip(sce::Gnmx::GfxContext *gfxc, uint32_t bufferIndex)
{
	uint32_t index = 0;
	if (m_displayMode == GraphicsContext::kDisplayModeS3dView)
	{
		index = bufferIndex + m_S3dBufferIndexStart;
	}else if ((m_displayMode == GraphicsContext::kDisplayModeVrView90Hz) || (m_displayMode == GraphicsContext::kDisplayModeVrView120Hz)){
		index = bufferIndex + m_VrBufferIndexStart;
	}else {
		index = (m_useFirstNormalBuffer ? m_firstNormalBufferIndex : m_secondNormalBufferIndex )
			+ bufferIndex;
	}
	m_flipCount++;
	gfxc->submitAndFlip(m_handle, 
		index, SCE_VIDEO_OUT_FLIP_MODE_VSYNC,
		m_flipCount);
	m_pendingFlipIndices.push_back(index);

}

void ssgi::FrameBuffer::VideoInfo::submit(sce::Gnmx::GfxContext *gfxc)
{
	gfxc->submit();
}

void ssgi::FrameBuffer::VideoInfo::waitUntilSafeForRendering(sce::Gnmx::GfxContext *gfxc, uint32_t bufferIndex)
{
	if ((m_displayMode == GraphicsContext::kDisplayModeVrView90Hz) || (m_displayMode == GraphicsContext::kDisplayModeVrView120Hz)){
		return;
	}

	uint32_t index = 0;
	if (m_displayMode == GraphicsContext::kDisplayModeS3dView)
	{
		index = bufferIndex + m_S3dBufferIndexStart;
	} else {
		index = (m_useFirstNormalBuffer ? m_firstNormalBufferIndex : m_secondNormalBufferIndex )
			+ bufferIndex;
	}
	gfxc->waitUntilSafeForRendering(m_handle, index);
}
#pragma endregion // FrameBuffer_VideoInfo

/* ------------------------------------------------------------------------------------------------------- */
#pragma region FrameBuffer

int ssgi::FrameBuffer::initialize(GraphicsLoaderImpl *loader,
	uint32_t width, uint32_t height,
	uint32_t numBuffers,
	bool useStencil, bool useHtile,
	MultisampleMode multiSample,
	bool isVrView, bool isS3dView)
{
	int ret;
	m_loader = loader;
	m_width = width;
	m_height = height;
	m_useStencil = useStencil;
	m_useHtile = useHtile;
	m_multiSample = multiSample;

	m_numBuffers = numBuffers;

#ifdef TILED
	m_tileMode = sce::Gnm::kTileModeDisplay_2dThin;
#else
	m_tileMode = sce::Gnm::kTileModeDisplay_LinearAligned;
#endif
	m_enableCmask = m_multiSample != sce::SampleUtil::Graphics::kMultisampleNone;
	m_enableFmask = m_multiSample != sce::SampleUtil::Graphics::kMultisampleNone;

	m_firstNormalDisplayBuffers = NULL;
	m_secondNormalDisplayBuffers = NULL;
	m_s3dDisplayBuffers = NULL;
	m_vrDisplayBuffers = NULL;

	void **videoNormalBuffers = (void**)alloca(sizeof(void*) * numBuffers);
	SceVideoOutStereoBuffers *videoS3dBuffers = (SceVideoOutStereoBuffers *)alloca(sizeof(SceVideoOutStereoBuffers) * numBuffers);
	void **videoVrBuffers = (void**)alloca(sizeof(void*) * numBuffers);


	m_firstNormalDisplayBuffers = new DisplayBuffer[m_numBuffers];
	for (uint32_t buffer = 0; buffer < m_numBuffers; ++buffer)
	{
		ret = m_firstNormalDisplayBuffers[buffer].m_renderTargetLeft.initialize(loader, width, height,
			sce::Gnm::kDataFormatB8G8R8A8Unorm,
			m_tileMode, //sce::Gnm::kTileModeDisplay_LinearAligned,
			sce::Gnm::kNumSamples1, sce::Gnm::kNumFragments1,
			m_enableCmask, m_enableFmask);
 		if(ret != SCE_OK){
			printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
			finalize();
			return ret;
		}
		videoNormalBuffers[buffer] = (void*)m_firstNormalDisplayBuffers[buffer].m_renderTargetLeft.m_renderTarget.getBaseAddress();
	}

	m_s3dDisplayBuffers = new DisplayBuffer[m_numBuffers];
	if (isS3dView) {
		for (uint32_t buffer = 0; buffer < m_numBuffers; ++buffer) {
			ret = m_s3dDisplayBuffers[buffer].m_renderTargetLeft.initialize(loader, 1280, 720,
				sce::Gnm::kDataFormatB8G8R8A8Unorm,
				m_tileMode, //sce::Gnm::kTileModeDisplay_LinearAligned,
				sce::Gnm::kNumSamples1, sce::Gnm::kNumFragments1,
				m_enableCmask, m_enableFmask);
			if (ret != SCE_OK){
				printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
				finalize();
				return ret;
			}

			ret = m_s3dDisplayBuffers[buffer].m_renderTargetRight.initialize(loader, 1280, 720,
				sce::Gnm::kDataFormatB8G8R8A8Unorm,
				m_tileMode, //sce::Gnm::kTileModeDisplay_LinearAligned,
				sce::Gnm::kNumSamples1, sce::Gnm::kNumFragments1,
				m_enableCmask, m_enableFmask);
			if (ret != SCE_OK){
				printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
				finalize();
				return ret;
			}
			videoS3dBuffers[buffer].left = (void*)m_s3dDisplayBuffers[buffer].m_renderTargetLeft.m_renderTarget.getBaseAddress();
			videoS3dBuffers[buffer].right = (void*)m_s3dDisplayBuffers[buffer].m_renderTargetRight.m_renderTarget.getBaseAddress();
		}
	}

	m_vrDisplayBuffers = new DisplayBuffer[m_numBuffers];
	if (isVrView)
	{
		for (uint32_t buffer = 0; buffer < m_numBuffers; ++buffer)
		{
			ret = m_vrDisplayBuffers[buffer].m_renderTargetLeft.initialize(loader, 1920, 1080,
				sce::Gnm::kDataFormatB8G8R8A8Unorm,
				m_tileMode, //sce::Gnm::kTileModeDisplay_LinearAligned,
				sce::Gnm::kNumSamples1, sce::Gnm::kNumFragments1,
				m_enableCmask, m_enableFmask);
			if (ret != SCE_OK){
				printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
				finalize();
				return ret;
			}
			videoVrBuffers[buffer] = (void*)m_vrDisplayBuffers[buffer].m_renderTargetLeft.m_renderTarget.getBaseAddress();
		}

	}
	m_referringResources = new ReferringResources[m_numBuffers];

	ret = m_videoInfo.initialize(m_firstNormalDisplayBuffers[0].m_renderTargetLeft.m_renderTarget.getWidth(),
								 m_firstNormalDisplayBuffers[0].m_renderTargetLeft.m_renderTarget.getHeight(),
								 m_firstNormalDisplayBuffers[0].m_renderTargetLeft.m_renderTarget.getPitch(),
								 numBuffers, 
								 videoNormalBuffers,
								 videoS3dBuffers,
								 videoVrBuffers,
								 isVrView, isS3dView);
	SCE_SAMPLE_UTIL_ASSERT(ret >= 0);


	ret = m_depthTarget.initialize(m_loader, width, height, useStencil, useHtile);
	if(ret != SCE_OK){
		printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
		finalize();
		return ret;
	}

	return SCE_OK;
}



int ssgi::FrameBuffer::finalize(void)
{
	if(m_loader == NULL){
		return SCE_OK;
	}
	m_depthTarget.finalize();

	if(m_referringResources != NULL){
		delete [] m_referringResources;
		m_referringResources = NULL;
	}

	if (m_firstNormalDisplayBuffers != NULL){
		for (uint32_t buffer = 0; buffer < m_numBuffers; ++buffer)	{
			m_firstNormalDisplayBuffers[buffer].m_renderTargetLeft.finalize();
			m_firstNormalDisplayBuffers[buffer].m_renderTargetRight.finalize();
		}
		delete[] m_firstNormalDisplayBuffers;
		m_firstNormalDisplayBuffers = NULL;
	}

	if (m_secondNormalDisplayBuffers != NULL){
		for (uint32_t buffer = 0; buffer < m_numBuffers; ++buffer)	{
			m_secondNormalDisplayBuffers[buffer].m_renderTargetLeft.finalize();
			m_secondNormalDisplayBuffers[buffer].m_renderTargetRight.finalize();
		}
		delete[] m_secondNormalDisplayBuffers;
		m_secondNormalDisplayBuffers = NULL;
	}

	if (m_s3dDisplayBuffers) {
		for (uint32_t buffer = 0; buffer < m_numBuffers; ++buffer)	{
			m_s3dDisplayBuffers[buffer].m_renderTargetLeft.finalize();
			m_s3dDisplayBuffers[buffer].m_renderTargetRight.finalize();
		}
		delete[] m_s3dDisplayBuffers;
		m_s3dDisplayBuffers = NULL;
	}

	if (m_vrDisplayBuffers) {
		for (uint32_t buffer = 0; buffer < m_numBuffers; ++buffer)	{
			m_vrDisplayBuffers[buffer].m_renderTargetLeft.finalize();
			m_vrDisplayBuffers[buffer].m_renderTargetRight.finalize();
		}
		delete[] m_vrDisplayBuffers;
		m_vrDisplayBuffers = NULL;
	}

	m_videoInfo.finalize();

	if(m_multiSample != sce::SampleUtil::Graphics::kMultisampleNone){
		m_middleRenderTarget.finalize();
	}

	m_loader = NULL;
	return SCE_OK;

}

void ssgi::FrameBuffer::addOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg)
{
	m_onSetFrameBufCallback.push_back(OnSetFrameBufCallbackJob(onSetFrameBufCallback, onSetFrameBufCallbackArg));
}
void ssgi::FrameBuffer::removeOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg)
{
	std::vector<OnSetFrameBufCallbackJob>::iterator itr;
	for(itr=m_onSetFrameBufCallback.begin() ; itr != m_onSetFrameBufCallback.end() ; ++itr){
		if((*itr).equals(onSetFrameBufCallback, onSetFrameBufCallbackArg)) {
			m_onSetFrameBufCallback.erase(itr);
			break;
		}
	}

}

void ssgi::FrameBuffer::checkPendingResizeFrameBuffer(void)
{
	int ret;
	DisplayBuffer* target = 
		m_videoInfo.m_useFirstNormalBuffer ? m_secondNormalDisplayBuffers :
		m_firstNormalDisplayBuffers;
	
	if (target != NULL) {
		for (int i = 0; i < m_numBuffers; i++){
			ret = target[i].m_renderTargetLeft.finalize();
			SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		}
		ret = m_videoInfo.unregisterBackFrameBuffers();
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		delete[] target;
		if(m_videoInfo.m_useFirstNormalBuffer){
			m_secondNormalDisplayBuffers = NULL;
		}else{
			m_firstNormalDisplayBuffers = NULL;
		}
	}
	if (m_hasPendingResize){
		m_hasPendingResize = false;
		ssgi::FrameBuffer::resizeFrameBuffer(m_pendingWidth, m_pendingHeight);
	}

}

int ssgi::FrameBuffer::resizeFrameBuffer(uint32_t width, uint32_t height)
{
	int ret;
 
	DisplayBuffer* target = 
		m_videoInfo.m_useFirstNormalBuffer ? m_secondNormalDisplayBuffers :
		m_firstNormalDisplayBuffers;
	
	if (target != NULL) {
		m_pendingWidth = width;
		m_pendingHeight = height;
		m_hasPendingResize = true;
		return 0;
	}

	m_width = width;
	m_height = height;

	DisplayBuffer* displayBuffers = new DisplayBuffer[m_numBuffers];
	if(m_videoInfo.m_useFirstNormalBuffer){
		m_secondNormalDisplayBuffers = displayBuffers;
	}else{
		m_firstNormalDisplayBuffers = displayBuffers;
	}

	void **buffers = (void**)alloca(m_numBuffers * sizeof(void*));
	for (int i = 0; i < m_numBuffers; i++){
		ret = displayBuffers[i].m_renderTargetLeft.initialize(m_loader, m_width, m_height,
			sce::Gnm::kDataFormatB8G8R8A8Unorm,
			m_tileMode, //sce::Gnm::kTileModeDisplay_LinearAligned,
			sce::Gnm::kNumSamples1, sce::Gnm::kNumFragments1,
			m_enableCmask, m_enableFmask);
		SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
		buffers[i] = displayBuffers[i].m_renderTargetLeft.m_renderTarget.getBaseAddress();
	}
	ret = m_videoInfo.registerBackFrameBuffers(
		displayBuffers[0].m_renderTargetLeft.m_renderTarget.getWidth(),
		displayBuffers[0].m_renderTargetLeft.m_renderTarget.getHeight(),
		displayBuffers[0].m_renderTargetLeft.m_renderTarget.getPitch(),
		buffers);
	SCE_SAMPLE_UTIL_ASSERT(ret >= 0);
	m_pendingResizeFrameBufferCountDown = m_numBuffers + 2;
	m_videoInfo.switchNormalBuffer();
	return 0;
}


#pragma endregion // FrameBuffer

#endif //_SCE_TARGET_OS_ORBIS




