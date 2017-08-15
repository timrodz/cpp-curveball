/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/graphics/context.h>
#include "constant_internal_gxm.h"
#include "context_internal_gxm.h"
namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;



ssgi::FrameBufferImpl::Resolutions::Resolutions(void)
{
	m_mainRenderTargets[0] = NULL;
	m_mainRenderTargets[1] = NULL;
	m_mainRenderTargets[2] = NULL;
	m_mainRenderTargets[3] = NULL;
	m_depthStencilSurface[0] = NULL;
	m_depthStencilSurface[1] = NULL;
	m_depthStencilSurface[2] = NULL;
	m_depthStencilSurface[3] = NULL;
	m_syncObjects = NULL;
	m_numBufferPerResolution = 0;
}
	
int ssgi::FrameBufferImpl::Resolutions::initialize(GraphicsLoaderImpl *loader, 
												   uint32_t numBufferPerResolution,
												   MultisampleMode multiSampleMode)
{
	int ret;
	m_numBufferPerResolution = numBufferPerResolution;
	m_mainRenderTargets[0] = new RenderTargetImpl[numBufferPerResolution];
	m_mainRenderTargets[1] = new RenderTargetImpl[numBufferPerResolution];
	m_mainRenderTargets[2] = new RenderTargetImpl[numBufferPerResolution];
	m_mainRenderTargets[3] = new RenderTargetImpl[numBufferPerResolution];
	m_syncObjects = new SceGxmSyncObject*[numBufferPerResolution];

	for(int i=0; i<m_numBufferPerResolution; i++){
		ret = sceGxmSyncObjectCreate(&m_syncObjects[i]);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		
		// [0]960x544 [1]720x408 [2]640x368 [3]480x272
		ret = m_mainRenderTargets[0][i].initialize(loader,
			960, 544,
			SCE_GXM_COLOR_SURFACE_LINEAR,
			SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
			0,
			1,
			convertMultiSampleMode(multiSampleMode),
			0,
			NULL,
			m_syncObjects[i]);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
#if 0
		ret = m_mainRenderTargets[1][i].initialize(loader,
			720, 408,
			SCE_GXM_COLOR_SURFACE_LINEAR,
			SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
			0,
			1,
			convertMultiSampleMode(multiSampleMode),
			0,
			NULL,
			m_syncObjects[i]);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		ret = m_mainRenderTargets[2][i].initialize(loader,
			640, 368,
			SCE_GXM_COLOR_SURFACE_LINEAR,
			SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
			0,
			1,
			convertMultiSampleMode(multiSampleMode),
			0,
			NULL,
			m_syncObjects[i]);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		ret = m_mainRenderTargets[3][i].initialize(loader,
			480, 272,
			SCE_GXM_COLOR_SURFACE_LINEAR,
			SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
			0,
			1,
			convertMultiSampleMode(multiSampleMode),
			0,
			NULL,
			m_syncObjects[i]);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
#endif
	}
	
	// [0]960x544 [1]720x408 [2]640x368 [3]480x272
	m_depthStencilSurface[0] = new DepthStencilSurfaceImpl;
	m_depthStencilSurface[0]->initialize(
		loader, 960,544,
		SCE_GXM_DEPTH_STENCIL_FORMAT_DF32,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		convertMultiSampleMode(multiSampleMode),
		false, false
		);
#if 0
	m_depthStencilSurface[1] = new DepthStencilSurfaceImpl;
	m_depthStencilSurface[1]->initialize(
		loader, 720,408,
		SCE_GXM_DEPTH_STENCIL_FORMAT_DF32,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		convertMultiSampleMode(multiSampleMode),
		false, false
		);
	m_depthStencilSurface[2] = new DepthStencilSurfaceImpl;
	m_depthStencilSurface[2]->initialize(
		loader, 640,368,
		SCE_GXM_DEPTH_STENCIL_FORMAT_DF32,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		convertMultiSampleMode(multiSampleMode),
		false, false
		);
	m_depthStencilSurface[3] = new DepthStencilSurfaceImpl;
	m_depthStencilSurface[3]->initialize(
		loader, 
		480, 272, 
		SCE_GXM_DEPTH_STENCIL_FORMAT_DF32,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		convertMultiSampleMode(multiSampleMode),
		false, false
		);
#endif

	(void)ret;
	return SCE_OK;
}
int ssgi::FrameBufferImpl::Resolutions::finalize(void)
{
	m_depthStencilSurface[0]->finalize();
	delete m_depthStencilSurface[0];
	m_depthStencilSurface[0] = NULL;
#if 0
	m_depthStencilSurface[1]->finalize();
	delete m_depthStencilSurface[1];
	m_depthStencilSurface[1] = NULL;
	m_depthStencilSurface[2]->finalize();
	delete m_depthStencilSurface[2];
	m_depthStencilSurface[2] = NULL;
	m_depthStencilSurface[3]->finalize();
	delete m_depthStencilSurface[3];
	m_depthStencilSurface[3] = NULL;
#endif
	for(int i=0; i<m_numBufferPerResolution; i++){
		sceGxmSyncObjectDestroy(m_syncObjects[i]);
		m_syncObjects[i] = NULL;
	}
	delete [] m_syncObjects;

	for(int i=0; i<m_numBufferPerResolution; i++){
		m_mainRenderTargets[0][i].finalize();
		m_mainRenderTargets[1][i].finalize();
		m_mainRenderTargets[2][i].finalize();
		m_mainRenderTargets[3][i].finalize();
	}
	delete [] m_mainRenderTargets[0];
	m_mainRenderTargets[0] = NULL;
#if 0
	delete [] m_mainRenderTargets[1];
	m_mainRenderTargets[1] = NULL;
	delete [] m_mainRenderTargets[2];
	m_mainRenderTargets[2] = NULL;
	delete [] m_mainRenderTargets[3];
	m_mainRenderTargets[3] = NULL;
#endif
	return SCE_OK;
}


ssgi::FrameBufferImpl::FrameBufferImpl(void)
{

	m_onSetFrameBufCallback.reserve(2);

	m_displayFrontBufferIndex = 0;
	m_displayBackBufferIndex = 0;
}

int ssgi::FrameBufferImpl::initialize(GraphicsLoaderImpl *loader, 
									  uint32_t width, uint32_t height,
									  MultisampleMode multiSampleMode,
									  uint32_t numBuffer)
{
	int ret;
	
	ret = m_resolutions.initialize(loader, numBuffer, multiSampleMode);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_resolutions.setNextResolution(width, height);

	m_referringResources = new ReferringResources[numBuffer];
	
	m_onSetFrameBufCallback.reserve(2);
	
	m_displayBackBufferIndex = 0;
	m_displayFrontBufferIndex = 0;
	
	
	(void)ret;
	return SCE_OK;
}

int ssgi::FrameBufferImpl::finalize(void)
{
	int ret;
	
	ret = m_resolutions.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	delete [] m_referringResources;
	(void)ret;
	return SCE_OK;
}


ssg::RenderTarget *ssgi::FrameBufferImpl::getNextRenderTarget(void)
{
	return m_resolutions.getNextRenderTarget(m_displayBackBufferIndex);
}

uint32_t ssgi::FrameBufferImpl::getNextRenderTargetIndex(void) const
{
	return m_displayBackBufferIndex;
}

ssg::DepthStencilSurface *ssgi::FrameBufferImpl::getDepthStencilSurface(void)
{
	return m_resolutions.getDepthStencilSurface();
}


int ssgi::FrameBufferImpl::flip(uint32_t numVSync)
{
	RenderTargetImpl *backRenderTarget = dynamic_cast<RenderTargetImpl*>(&m_resolutions.m_mainRenderTargets[0][m_displayBackBufferIndex]);
	RenderTargetImpl *frontRenderTarget = dynamic_cast<RenderTargetImpl*>(&m_resolutions.m_mainRenderTargets[0][m_displayFrontBufferIndex]);

	ssgi::TextureImpl *textureImpl = dynamic_cast<ssgi::TextureImpl*>(backRenderTarget->getTexture());
	void *backBuffer                         = textureImpl->getData();
	const SceGxmColorSurface *displaySurface = backRenderTarget->getGxmColorSurface();
	SceGxmSyncObject *oldBuffer  =  frontRenderTarget->getGxmFragmentSyncObject();
	SceGxmSyncObject *newBuffer  =  backRenderTarget->getGxmFragmentSyncObject();


	DisplayCallbackArg arg;
	arg.backBuffer = backBuffer;
	arg.frameBufferIndex = m_displayBackBufferIndex;
	arg.frameBuffer = this;
	arg.numVSync = numVSync;
	arg.renderTarget = backRenderTarget;

	int32_t ret = SCE_OK;

	ret = sceGxmPadHeartbeat(displaySurface, newBuffer);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = sceGxmDisplayQueueAddEntry(oldBuffer,		// front buffer is OLD buffer
		newBuffer,		// back buffer is NEW buffer
		&arg);

	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	// update buffer indices
	m_displayFrontBufferIndex = m_displayBackBufferIndex;
	m_displayBackBufferIndex = (m_displayBackBufferIndex + 1) % m_resolutions.m_numBufferPerResolution;



	return ret;
}


void ssgi::FrameBufferImpl::displayCallback( const void *callbackData )
{
	// cast the parameters back
	const DisplayCallbackArg* arg =(const DisplayCallbackArg*)callbackData;
	arg->frameBuffer->waitVblankStart(arg->backBuffer, arg->frameBufferIndex, arg->numVSync, arg->renderTarget);
}



// Wait for vblank start
void ssgi::FrameBufferImpl::waitVblankStart(void *backBuffer, uint32_t frameBufferIndex, uint32_t numVSync, RenderTargetImpl *renderTarget)
{
	int32_t returnCode = SCE_OK;
	SceDisplayFrameBuf framebuf;

	/* swap to the new buffer on the next VSYNC */
	memset(&framebuf, 0x00, sizeof(SceDisplayFrameBuf));
	framebuf.size        = sizeof(SceDisplayFrameBuf);
	framebuf.base        = backBuffer;
	framebuf.pitch       = sceGxmColorSurfaceGetStrideInPixels(renderTarget->getGxmColorSurface());
	framebuf.pixelformat = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
	framebuf.width       = renderTarget->getTexture()->getWidth(); 
	framebuf.height      = renderTarget->getTexture()->getHeight(); 
	/*
	printf("framebuf.width = %d\n", framebuf.width);
	printf("framebuf.height = %d\n", framebuf.height);
	printf("framebuf.pitch=%#d\n", framebuf.pitch);
	*/
	for(int i=0 ; i<m_onSetFrameBufCallback.size(); ++i){
		m_onSetFrameBufCallback[i].m_callbackFunction(&framebuf, frameBufferIndex, m_onSetFrameBufCallback[i].m_callbackArg);
	}

	// Destroying frame-base objects.
	ReferringResources &rr = m_referringResources[frameBufferIndex];
	rr.destroyAll();

	if (numVSync>0) {
		returnCode = sceDisplayWaitSetFrameBufMulti(numVSync);
	}

	returnCode = sceDisplaySetFrameBuf( &framebuf, SCE_DISPLAY_UPDATETIMING_NEXTVSYNC );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_OK);

	returnCode = sceDisplayWaitSetFrameBuf();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(returnCode, SCE_OK);

	(void)returnCode;
}

void ssgi::FrameBufferImpl::addOnSetFrameBufCallback(
	OnSetFrameBufCallback onSetFrameBufCallback,
	void *onSetFrameBufCallbackArg)
{
	m_onSetFrameBufCallback.push_back(OnSetFrameBufCallbackJob(onSetFrameBufCallback, onSetFrameBufCallbackArg));

	return;
}

void ssgi::FrameBufferImpl::removeOnSetFrameBufCallback(
	OnSetFrameBufCallback onSetFrameBufCallback,
	void *onSetFrameBufCallbackArg)
{
	std::vector<OnSetFrameBufCallbackJob>::iterator itr;

	for(itr=m_onSetFrameBufCallback.begin() ; itr != m_onSetFrameBufCallback.end() ; ++itr){
		if((*itr).equals(onSetFrameBufCallback, onSetFrameBufCallbackArg)) {
			m_onSetFrameBufCallback.erase(itr);
			break;
		}
	}
	return;
}




#endif /* _SCE_TARGET_OS_PSP2 */

