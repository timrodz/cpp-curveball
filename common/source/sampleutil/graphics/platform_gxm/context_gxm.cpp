/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include "gxm_internal.h"
#include "../constant_internal.h"
#include "../graphics_internal.h"
#include "util/util.h"
#include "../image.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;

extern const SceGxmProgram _binary_sampleutil_clear_v_gxp_start;
extern const SceGxmProgram _binary_sampleutil_clear_f_gxp_start;

extern const char _binary_sampleutil_clear_v_gxp_size[];
extern const char _binary_sampleutil_clear_f_gxp_size[];


#define SCE_SAMPLEUTIL_GRAPHICS_DISPLAY_MAX_PENDING_SWAPS		2

#pragma region GpuControl

int ssgi::GpuControl::initialize(SceGxmContext *context)
{
	m_context = context;
	m_isFill = true;
	return SCE_OK;
}

void ssgi::GpuControl::setDepthFunc(DepthFunc depthFunc)
{
	sceGxmSetFrontDepthFunc(m_context, ssgi::convertDepthFunc(depthFunc));
	sceGxmSetBackDepthFunc(m_context, ssgi::convertDepthFunc(depthFunc));
}

void ssgi::GpuControl::setDepthWriteEnable(bool enable)
{
	sceGxmSetFrontDepthWriteEnable(m_context, enable? SCE_GXM_DEPTH_WRITE_ENABLED : SCE_GXM_DEPTH_WRITE_DISABLED);
	sceGxmSetBackDepthWriteEnable(m_context, enable? SCE_GXM_DEPTH_WRITE_ENABLED : SCE_GXM_DEPTH_WRITE_DISABLED);
}



void ssgi::GpuControl::setFillMode(bool isFill)
{
	m_isFill = isFill;
}

void ssgi::GpuControl::setCullMode(CullMode cullMode)
{
	sceGxmSetCullMode(m_context, ssgi::convertCullMode(cullMode));
}

void ssgi::GpuControl::Region::setAndAlign(uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height)
{
	m_enable = true;
	m_topLeftX = topLeftX;
	m_topLeftY = topLeftY;
	m_bottomRightX = topLeftX+width;
	m_bottomRightY = topLeftY+height;

	if(m_topLeftX%SCE_GXM_TILE_SIZEX != 0){
		m_topLeftX = (m_topLeftX/SCE_GXM_TILE_SIZEX + 1)*SCE_GXM_TILE_SIZEX;
	}

	if(m_topLeftY%SCE_GXM_TILE_SIZEY != 0){
		m_topLeftY = (m_topLeftY/SCE_GXM_TILE_SIZEY + 1)*SCE_GXM_TILE_SIZEY;
	}

	if(m_bottomRightX%SCE_GXM_TILE_SIZEX != 0){
		m_bottomRightX = (m_bottomRightX/SCE_GXM_TILE_SIZEX)*SCE_GXM_TILE_SIZEX;
	}

	if(m_bottomRightY%SCE_GXM_TILE_SIZEY != 0){
		m_bottomRightY = (m_bottomRightY/SCE_GXM_TILE_SIZEY)*SCE_GXM_TILE_SIZEY;
	}
}

void ssgi::GpuControl::setViewPort(uint32_t topLeftX,
								  uint32_t topLeftY,
								  uint32_t width,
								  uint32_t height,
								  float minDepth,
								  float maxDepth)
{
	float scale[4] =  { width*0.5f, height*-0.5f, (maxDepth-minDepth)*0.5f, 0.0f };
	float offset[4] = { topLeftX + scale[0], height + topLeftY + scale[1], (maxDepth+minDepth)*0.5f, 0.0f };

	m_viewPortRegion.setAndAlign(topLeftX, topLeftY, width, height);
	setClip();

	sceGxmSetViewportEnable(m_context, SCE_GXM_VIEWPORT_ENABLED);
	sceGxmSetViewport(m_context, offset[0], scale[0], offset[1], scale[1], offset[2], scale[2]);
}

void ssgi::GpuControl::setRegionClip(uint32_t topLeftX,
									uint32_t topLeftY,
									uint32_t width,
									uint32_t height)
{
	m_clipRegion.setAndAlign(topLeftX, topLeftY, width, height);
	setClip();
}

static void getDuplicatedRegion(uint32_t top1, uint32_t bottom1, uint32_t top2, uint32_t bottom2, uint32_t *outTop, uint32_t* outBottom)
{
	if((top1 <= top2) && (top2 <= bottom1)){
		*outTop = top2;
		if(bottom1 < bottom2){
			*outBottom = bottom1;
		}else{
			*outBottom = bottom2;
		}
	}else if ((top2 <= top1) && (top1 <= bottom2)){
		*outTop = top1;
		if(bottom1 < bottom2){
			*outBottom = bottom1;
		}else{
			*outBottom = bottom2;
		}
	}else{
		*outTop = 0;
		*outBottom = 0;
	}
}

void ssgi::GpuControl::setClip()
{
	uint32_t topLeftX = 0;
	uint32_t topLeftY = 0;
	uint32_t bottomRightX = 0;
	uint32_t bottomRightY = 0;

	if(m_viewPortRegion.m_enable && m_clipRegion.m_enable){
		getDuplicatedRegion(m_viewPortRegion.m_topLeftX, m_viewPortRegion.m_bottomRightX, m_clipRegion.m_topLeftX, m_clipRegion.m_bottomRightX, &topLeftX, &bottomRightX);
		getDuplicatedRegion(m_viewPortRegion.m_topLeftY, m_viewPortRegion.m_bottomRightY, m_clipRegion.m_topLeftY, m_clipRegion.m_bottomRightY, &topLeftY, &bottomRightY);
	}else if(m_viewPortRegion.m_enable){
		topLeftX = m_viewPortRegion.m_topLeftX;
		topLeftY = m_viewPortRegion.m_topLeftY;
		bottomRightX = m_viewPortRegion.m_bottomRightX;
		bottomRightY = m_viewPortRegion.m_bottomRightY;
	}else if(m_clipRegion.m_enable){
		topLeftX = m_clipRegion.m_topLeftX;
		topLeftY = m_clipRegion.m_topLeftY;
		bottomRightX = m_clipRegion.m_bottomRightX;
		bottomRightY = m_clipRegion.m_bottomRightY;
	}

	sceGxmSetRegionClip(m_context, SCE_GXM_REGION_CLIP_OUTSIDE, topLeftX, topLeftY, bottomRightX-1, bottomRightY-1);
}

void ssgi::GpuControl::setStencilFunc(
	uint8_t compareMask,
	uint8_t writeMask,
	StencilFunc frontFaceFunc,
	StencilOp   frontFaceStencilFail,
	StencilOp   frontFaceDepthFail,
	StencilOp   frontFaceDepthPass,
	StencilFunc backFaceFunc,
	StencilOp   backFaceStencilFail,
	StencilOp   backFaceDepthFail,
	StencilOp   backFaceDepthPass,
	uint8_t stencilRef
	)
{
	sceGxmSetBackStencilFunc(m_context,
		ssgi::convertStencilFunc(backFaceFunc),
		ssgi::convertStencilOp(backFaceStencilFail),
		ssgi::convertStencilOp(backFaceDepthFail),
		ssgi::convertStencilOp(backFaceDepthPass),
		compareMask,writeMask);
	sceGxmSetBackStencilRef(m_context,stencilRef);
	sceGxmSetFrontStencilFunc(m_context,
		ssgi::convertStencilFunc(frontFaceFunc),
		ssgi::convertStencilOp(frontFaceStencilFail),
		ssgi::convertStencilOp(frontFaceDepthFail),
		ssgi::convertStencilOp(frontFaceDepthPass),
		compareMask,	writeMask);
	sceGxmSetFrontStencilRef(m_context,	stencilRef);
}


#pragma endregion

#pragma region GpuBufferState

static inline uint32_t setUsedBit(uint32_t usedBufferMask, uint32_t index)
{
	uint32_t mask = 1<<(31-index);
	return usedBufferMask | mask;
}

static inline uint32_t unsetUsedBit(uint32_t usedBufferMask, uint32_t index)
{
	uint32_t mask = 1<<(31-index);
	return usedBufferMask & ~mask; 
}

static inline void referBuffers(ssgi::ReferringResources &rr, 
	                      sce::SampleUtil::Impl::ResourceImpl** buffers, 
						  uint32_t usedBufferMask)
{
	/*
	printf("* usedBufferMask=%#x\n", usedBufferMask);
	for(uint32_t i=0; i< 16; i++){
		printf("%#x ", (uintptr_t)buffers[i]);
	}
	printf("\n");
	*/
	uint32_t index = 0;
	while(index < 32){
		uint8_t cz = __clz(usedBufferMask);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 32){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(buffers[index+cz] != NULL);
		rr.add(buffers[index+cz]);

		usedBufferMask <<= (cz+1);
		index += cz+1;
	}
}


ssgi::GpuBufferState::GpuBufferState(void)
{
	m_currentRenderTarget = NULL;
	m_currentDepthStencilSurface = NULL;
	for(uint32_t i=0; i<SCE_GXM_MAX_VERTEX_STREAMS; i++){
		m_vertexBuffers[i] = NULL;
	}
	m_usedVertexBufferMask = 0;
	
	for(uint32_t i=0; i<SCE_GXM_MAX_UNIFORM_BUFFERS; i++){
		m_vertexUniformBuffers[i] = NULL;
		m_fragmentUniformBuffers[i] = NULL;
	}
	m_usedVertexUniformBufferMask = 0;
	m_usedFragmentUniformBufferMask = 0;

	for(uint32_t i=0; i<SCE_GXM_MAX_TEXTURE_UNITS; i++){
		m_vertexTextures[i] = NULL;
		m_fragmentTextures[i] = NULL;
	}
	m_usedVertexTextureMask = 0;
	m_usedFragmentTextureMask = 0;
	
	
}
int ssgi::GpuBufferState::initialize(SceGxmContext *context)
{
	m_context = context;
	return SCE_OK;
}
int ssgi::GpuBufferState::finalize(void)
{
	for(uint32_t i=0; i<SCE_GXM_MAX_VERTEX_STREAMS; i++){
		if(m_vertexBuffers[i] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_vertexBuffers[i]));
			m_vertexBuffers[i] = NULL;
		}
	}
	for(uint32_t i=0; i<SCE_GXM_MAX_UNIFORM_BUFFERS; i++){
		if(m_vertexUniformBuffers[i] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_vertexUniformBuffers[i]));
			m_vertexUniformBuffers[i] = NULL;
		}
		if(m_fragmentUniformBuffers[i] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_fragmentUniformBuffers[i]));
			m_fragmentUniformBuffers[i] = NULL;
		}
	}

	for(uint32_t i=0; i<SCE_GXM_MAX_TEXTURE_UNITS; i++){
		if(m_vertexTextures[i] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_vertexTextures[i]));
			m_vertexTextures[i] = NULL;
		}
		if(m_fragmentTextures[i] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_fragmentTextures[i]));
			m_fragmentTextures[i] = NULL;
		}
	}

	if(m_currentRenderTarget != NULL){
		sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_currentRenderTarget));
		m_currentRenderTarget = NULL;
	}
	if(m_currentDepthStencilSurface != NULL){
		sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_currentDepthStencilSurface));
		m_currentDepthStencilSurface = NULL;
	}


	return SCE_OK;
}


int ssgi::GpuBufferState::setVertexBuffer(VertexBuffer *ivertexBuffer, uint32_t streamIndex)
{
	if(streamIndex >= SCE_GXM_MAX_VERTEX_STREAMS){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	VertexBufferImpl *vertexBuffer = dynamic_cast<VertexBufferImpl*>(ivertexBuffer);
	if(m_vertexBuffers[streamIndex] == vertexBuffer){
		return SCE_OK;
	}
	if(m_vertexBuffers[streamIndex] != NULL){
		sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_vertexBuffers[streamIndex]));
	}
	m_vertexBuffers[streamIndex] = vertexBuffer;
	if(m_vertexBuffers[streamIndex] == NULL){
		m_usedVertexBufferMask = unsetUsedBit(m_usedVertexBufferMask, streamIndex);
		return SCE_OK;
	}else{
		m_usedVertexBufferMask = setUsedBit(m_usedVertexBufferMask, streamIndex);
		m_vertexBuffers[streamIndex]->refer();
		return sceGxmSetVertexStream( m_context, streamIndex, vertexBuffer->getData() );
	}
}


int ssgi::GpuBufferState::setUniformBuffer(ProgramType programType, UniformBuffer *buffer, uint32_t bufferIndex)
{
	if(bufferIndex >= SCE_GXM_MAX_UNIFORM_BUFFERS){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	UniformBufferImpl *buf = dynamic_cast<UniformBufferImpl*>(buffer);
	if(programType == kProgramTypeVertex){
		if(m_vertexUniformBuffers[bufferIndex] == buf){
			return SCE_OK;
		}			
		if(m_vertexUniformBuffers[bufferIndex] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_vertexUniformBuffers[bufferIndex]));
		}
		m_vertexUniformBuffers[bufferIndex] = buf;
		if(m_vertexUniformBuffers[bufferIndex] == NULL){
			m_usedVertexUniformBufferMask = unsetUsedBit(m_usedVertexUniformBufferMask, bufferIndex);
			return SCE_OK;
		}else{
			m_usedVertexUniformBufferMask = setUsedBit(m_usedVertexUniformBufferMask, bufferIndex);
			m_vertexUniformBuffers[bufferIndex]->refer();
			return sceGxmSetVertexUniformBuffer(m_context, bufferIndex, buf->getData());
		}
	}else{
		if(m_fragmentUniformBuffers[bufferIndex] == buf){
			return SCE_OK;
		}			
		if(m_fragmentUniformBuffers[bufferIndex] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_fragmentUniformBuffers[bufferIndex]));
		}
		m_fragmentUniformBuffers[bufferIndex] = buf;
		if(m_fragmentUniformBuffers[bufferIndex] == NULL){
			m_usedFragmentUniformBufferMask = unsetUsedBit(m_usedFragmentUniformBufferMask, bufferIndex);
			return SCE_OK;
		}else{
			m_usedFragmentUniformBufferMask = setUsedBit(m_usedFragmentUniformBufferMask, bufferIndex);
			m_fragmentUniformBuffers[bufferIndex]->refer();
			return sceGxmSetFragmentUniformBuffer(m_context, bufferIndex, buf->getData());
		}
	}
}


int ssgi::GpuBufferState::setTexture(ProgramType programType, Texture *texture, uint32_t index)
{
	if(index >= SCE_GXM_MAX_TEXTURE_UNITS){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	TextureImpl *tex = dynamic_cast<TextureImpl*>(texture);
	if(programType == kProgramTypeVertex){
		if(m_vertexTextures[index] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_vertexTextures[index]));
		}
		m_vertexTextures[index] = tex;
		if(m_vertexTextures[index] == NULL){
			m_usedVertexTextureMask = unsetUsedBit(m_usedVertexTextureMask, index);
			sceGxmSetVertexTexture(m_context, index, NULL);
			return SCE_OK;
		}else{
			m_usedVertexTextureMask = setUsedBit(m_usedVertexTextureMask, index);
			m_vertexTextures[index]->refer();
			return sceGxmSetVertexTexture(m_context, index, tex->getGxmTexture());
		}
	}else{
		if(m_fragmentTextures[index] != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_fragmentTextures[index]));
		}
		m_fragmentTextures[index] = tex;
		if(m_fragmentTextures[index] == NULL){
			m_usedFragmentTextureMask = unsetUsedBit(m_usedFragmentTextureMask, index);
			sceGxmSetFragmentTexture(m_context, index, NULL);
			return SCE_OK;
		}else{
			m_usedFragmentTextureMask = setUsedBit(m_usedFragmentTextureMask, index);
			m_fragmentTextures[index]->refer();
			return sceGxmSetFragmentTexture(m_context, index, tex->getGxmTexture());
		}
	}
}


int ssgi::GpuBufferState::setRenderTarget(RenderTarget* renderTarget, DepthStencilSurface *depthStencil)
{
	RenderTargetImpl *rt = dynamic_cast<RenderTargetImpl *>(renderTarget);
	DepthStencilSurfaceImpl *ds = dynamic_cast<DepthStencilSurfaceImpl *>(depthStencil);
	if(m_currentRenderTarget !=  rt){
		if(m_currentRenderTarget != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_currentRenderTarget));
		}
		m_currentRenderTarget =  rt;
		if(m_currentRenderTarget != NULL){
			m_currentRenderTarget->refer();
		}
	}
	if(m_currentDepthStencilSurface != ds){
		if(m_currentDepthStencilSurface != NULL){
			sce::SampleUtil::destroy(dynamic_cast<sce::SampleUtil::Impl::ResourceImpl*>(m_currentDepthStencilSurface));
		}
		m_currentDepthStencilSurface =  ds;
		if(m_currentDepthStencilSurface != NULL){
			m_currentDepthStencilSurface->refer();
		}
	}
	return SCE_OK;
}

void ssgi::GpuBufferState::addToReferringResources(ReferringResources &rr)
{
	referBuffers(rr, m_vertexBuffers, m_usedVertexBufferMask);
	referBuffers(rr, m_vertexUniformBuffers, m_usedVertexUniformBufferMask);
	referBuffers(rr, m_fragmentUniformBuffers, m_usedFragmentUniformBufferMask);
	referBuffers(rr, m_vertexTextures, m_usedVertexTextureMask);
	referBuffers(rr, m_fragmentTextures, m_usedFragmentTextureMask);

	if(m_currentRenderTarget != NULL){
		rr.add(m_currentRenderTarget);
	}
	if(m_currentDepthStencilSurface != NULL){
		rr.add(m_currentDepthStencilSurface);
	}
}



#pragma endregion //GpuBufferState


ssgi::GraphicsContextImpl::GraphicsContextImpl(void)
{
}

SceGxmContext *ssgi::GraphicsContextImpl::createGxmContext(void)
{
	SceGxmContext *context;

	void *vdmRingBuffer      = m_loader.memalign(GraphicsLoaderImpl::MEMTYPE_LPDDR_UC, 4, SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE);
	SCE_SAMPLE_UTIL_ASSERT(vdmRingBuffer != NULL);
	void *vertexRingBuffer   = m_loader.memalign(GraphicsLoaderImpl::MEMTYPE_LPDDR_UC, 4, SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE);
	SCE_SAMPLE_UTIL_ASSERT(vertexRingBuffer != NULL);
	void *fragmentRingBuffer = m_loader.memalign(GraphicsLoaderImpl::MEMTYPE_LPDDR_UC, 4, SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE);
	SCE_SAMPLE_UTIL_ASSERT(fragmentRingBuffer != NULL);
	void *fragmentUsseRingBuffer = m_loader.memalign(GraphicsLoaderImpl::MEMTYPE_USSE, 4, SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE);
	SCE_SAMPLE_UTIL_ASSERT(fragmentUsseRingBuffer != NULL);
	uint32_t fragmentUsseRingBufferOffset = m_loader.convertToFragmentUsseOffset(fragmentUsseRingBuffer);

	memset(&m_contextParams, 0, sizeof(SceGxmContextParams));
	m_contextParams.hostMem							= ::malloc(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE); // TODO: to be freed
	m_contextParams.hostMemSize						= SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE;
	m_contextParams.vdmRingBufferMem				= vdmRingBuffer;
	m_contextParams.vdmRingBufferMemSize			= SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;
	m_contextParams.vertexRingBufferMem			    = vertexRingBuffer;
	m_contextParams.vertexRingBufferMemSize			= SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;
	m_contextParams.fragmentRingBufferMem			= fragmentRingBuffer;
	m_contextParams.fragmentRingBufferMemSize		= SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;
	m_contextParams.fragmentUsseRingBufferMem		= fragmentUsseRingBuffer;
	m_contextParams.fragmentUsseRingBufferMemSize	= SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;
	m_contextParams.fragmentUsseRingBufferOffset	= fragmentUsseRingBufferOffset;

	int ret = sceGxmCreateContext(&m_contextParams, &context);
	if(ret < 0){
		return NULL;
	}
	return context;

}

int ssgi::GraphicsContextImpl::initialize(uint32_t width, 
								 uint32_t height, 
								 GraphicsContextOption* option)
{
	return initialize(NULL, NULL, width, height, option);

}



int ssgi::GraphicsContextImpl::initialize(SceGxmShaderPatcher *patcher, 
					            SceGxmContext *context,
								uint32_t width,
								uint32_t height, 
								GraphicsContextOption* option,
								bool initializeGxm)
{
	int ret;

	MultisampleMode multisampleMode;
	uint32_t numFrameBuffer;

	if (option == NULL)	{
		multisampleMode = m_defaultMultisampleMode;
		numFrameBuffer = m_defaultNumFrameBuffer;
	}else{
		multisampleMode = option->multisampleMode;
		numFrameBuffer = option->numFrameBuffer;
	}

	m_initializeGxm = initializeGxm;
	if(initializeGxm){
		SceGxmInitializeParams params;
		params.flags							= 0;
		params.parameterBufferSize			= SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;
		params.displayQueueMaxPendingCount	= SCE_SAMPLEUTIL_GRAPHICS_DISPLAY_MAX_PENDING_SWAPS;
		params.displayQueueCallback         = FrameBufferImpl::displayCallback;
		params.displayQueueCallbackDataSize = sizeof(DisplayCallbackArg);
		ret = sceGxmInitialize(&params);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	}

	m_loader.initialize(patcher);
	memset(&m_contextParams, 0, sizeof(m_contextParams));
	if(context != NULL){
		m_gxmContext = context;
		m_freeContext = false;
	}else{
		m_freeContext = true;
		m_gxmContext = createGxmContext();
		SCE_SAMPLE_UTIL_ASSERT(m_gxmContext != NULL);
	}

	ret = m_gpuBufferState.initialize(m_gxmContext);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	ret = m_frameBuffer.initialize(&m_loader, width, height, multisampleMode, numFrameBuffer);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	ret = m_gpuControl.initialize(m_gxmContext);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	
	ret = m_loader.registerVertexProgram(&m_clearV, &_binary_sampleutil_clear_v_gxp_start, (uintptr_t)_binary_sampleutil_clear_v_gxp_size);
	SCE_SAMPLE_UTIL_ASSERT(m_clearV != NULL);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	ret = m_loader.registerFragmentProgram(&m_clearF, &_binary_sampleutil_clear_f_gxp_start, (uintptr_t)_binary_sampleutil_clear_f_gxp_size);
	SCE_SAMPLE_UTIL_ASSERT(m_clearF != NULL);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_clearEffect = new Effect::EffectData;
	const Program* cv = m_clearV;

	ret = m_clearEffect->initialize(cv, m_clearF, NULL);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_clearEffectInstance = new Effect::EffectInstance;
	{
		VertexAttribute vattr;
		vattr.streamIndex = 0;		
		vattr.offset = 0;			
		vattr.format = kAttributeFormatF32;
		vattr.componentCount = 2;		
		vattr.semantic = kParameterSemanticPosition;
		vattr.semanticIndex = 0;

		VertexStream stream;
		stream.stride = 2 * sizeof(float);
		stream.indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

		GraphicsContext *igc = this;
		ret = m_clearEffectInstance->initialize(igc, m_clearEffect, &vattr, 1, &stream, 1);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	}

	ret = m_loader.createVertexBuffer(&m_clearVertexBuffer, sizeof(float)*6);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = m_loader.createIndexBuffer(&m_clearIndexBuffer, sizeof(uint16_t)*3);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		float *v = (float*)m_clearVertexBuffer->beginWrite();
		uint16_t *i = (uint16_t*)m_clearIndexBuffer->beginWrite();

		v[0] = -1.0f;
		v[1] = -1.0f;
		v[2] =  3.0f;
		v[3] = -1.0f;
		v[4] = -1.0f;
		v[5] =  3.0f;

		i[0] = 0;
		i[1] = 1;
		i[2] = 2;

		m_clearIndexBuffer->endWrite();
		m_clearVertexBuffer->endWrite();
	}

	m_clearColor = m_clearEffect->getFragmentShader()->getParamByName("clearColor");
	SCE_SAMPLE_UTIL_ASSERT(m_clearColor != NULL);
	(void)ret;
	return SCE_OK;
}



int ssgi::GraphicsContextImpl::finalize(void)
{
	sceGxmFinish(m_gxmContext);
	sceGxmDisplayQueueFinish();
	int ret;
	if(m_clearVertexBuffer != NULL){
		sce::SampleUtil::destroy(m_clearVertexBuffer);
		m_clearVertexBuffer = NULL;
	}

	if(m_clearIndexBuffer != NULL){
		sce::SampleUtil::destroy(m_clearIndexBuffer);
		m_clearIndexBuffer = NULL;
	}


	if(m_clearEffectInstance != NULL){
		m_clearEffectInstance->finalize();
		delete m_clearEffectInstance;
		m_clearEffectInstance = NULL;
	}
	if(m_clearEffect != NULL){
		m_clearEffect->finalize();
		delete m_clearEffect;
		m_clearEffect = NULL;
	}

	if(m_clearV != NULL){
		ret = m_loader.unregisterProgram(m_clearV);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_clearV = NULL;
	}
	if(m_clearF != NULL){
		ret = m_loader.unregisterProgram(m_clearF);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_clearF = NULL;
	}

	m_frameBuffer.finalize();
	m_gpuBufferState.finalize();
	
	if(m_freeContext){
		m_freeContext = false;
		sceGxmDestroyContext(m_gxmContext);
	}

	if(m_contextParams.hostMem != NULL){
		::free(m_contextParams.hostMem);
	}
	if(m_contextParams.vdmRingBufferMem		!= NULL){
		m_loader.free(m_contextParams.vdmRingBufferMem);
	}
	if(m_contextParams.vertexRingBufferMem!= NULL){
		m_loader.free(m_contextParams.vertexRingBufferMem);
	}
	if(m_contextParams.fragmentRingBufferMem!= NULL){
		m_loader.free(m_contextParams.fragmentRingBufferMem);
	}
	if(m_contextParams.fragmentUsseRingBufferMem!= NULL){
		m_loader.free(m_contextParams.fragmentUsseRingBufferMem);
	}


	memset(&m_contextParams, 0, sizeof(m_contextParams));


	m_loader.finalize();

	if(m_initializeGxm){
		ret = sceGxmTerminate();
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_initializeGxm = false;
	}

	(void)ret;
	return SCE_OK;
}


void ssgi::GraphicsContextImpl::clearRenderTarget(uint32_t color)
{
	setDepthWriteEnable(true);
	setDepthFunc(kDepthFuncAlways);
	const float floatCoeff = 1.0f / 255.0f;
	float fcolor[] = 
	{
		(float)((color >> 24) & 0x000000FF) * floatCoeff,
		(float)((color >> 16) & 0x000000FF) * floatCoeff,
		(float)((color >> 8) & 0x000000FF) * floatCoeff,
		(float)((color) & 0x000000FF) * floatCoeff
	};

	m_clearColor->getValue()->setArrayDataF(fcolor, 4);

	VertexBuffer *buffers[] = { m_clearVertexBuffer };
	m_clearEffectInstance->apply(this, buffers, 1);

	m_gpuControl.setFillMode(true);
	draw(kPrimitiveTriangles, m_clearIndexBuffer, 3);
}



                                    

int ssgi::GraphicsContextImpl::saveBackBufferAsBmp(const char* path)
{
	if(path == NULL){
		printf("graphicsUtilSaveDisplayAsBmp: path is NULL\n");
		return SCE_ERROR_ERRNO_EFAULT;
	}

	//int32_t returnCode = SCE_OK;

	RenderTargetImpl *backBuffer = dynamic_cast<RenderTargetImpl*>(m_frameBuffer.getNextRenderTarget());

	int stride = ssiu::ceil(backBuffer->getWidth(), SCE_GXM_TEXTURE_IMPLICIT_STRIDE_ALIGNMENT);
	int width = backBuffer->getWidth();
	int height = backBuffer->getHeight();

	ssgi::TextureImpl *texture = dynamic_cast<ssgi::TextureImpl *>(backBuffer->getTexture());

	const char * buf = (const char*)texture->getData();

	return BmpFile::saveImage(path, stride, width, height, buf);
}


ssg::RenderTarget *ssgi::GraphicsContextImpl::getNextRenderTarget(FrameBufferSide side)
{
	(void)side;
	return m_frameBuffer.getNextRenderTarget();
}

ssg::DepthStencilSurface *ssgi::GraphicsContextImpl::getDepthStencilSurface(void)
{
	return m_frameBuffer.getDepthStencilSurface();
}



int ssgi::GraphicsContextImpl::flip(uint32_t numVSync)
{
	int ret = m_frameBuffer.flip(numVSync);

	return ret;
}






int ssgi::GraphicsContextImpl::draw(Primitive _primitive, IndexBuffer *indexBuffer, uint32_t count)
{
	if(indexBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	SceGxmPrimitiveType primitive = ssgi::convertPrimitive(_primitive);
	switch(primitive)
	{
	case SCE_GXM_PRIMITIVE_TRIANGLES			:
	case SCE_GXM_PRIMITIVE_TRIANGLE_STRIP	:
	case SCE_GXM_PRIMITIVE_TRIANGLE_FAN		:
	case SCE_GXM_PRIMITIVE_TRIANGLE_EDGES	:
		if(m_gpuControl.m_isFill){
			sceGxmSetFrontPolygonMode(getGxmContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
			sceGxmSetBackPolygonMode(getGxmContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
		}else{
			sceGxmSetFrontPolygonMode(getGxmContext(), SCE_GXM_POLYGON_MODE_LINE);
			sceGxmSetBackPolygonMode(getGxmContext(), SCE_GXM_POLYGON_MODE_LINE);
		}
		break;
	case SCE_GXM_PRIMITIVE_LINES				:
		sceGxmSetFrontPolygonMode(getGxmContext(), SCE_GXM_POLYGON_MODE_LINE);
		break;
	case SCE_GXM_PRIMITIVE_POINTS		:	
		sceGxmSetFrontPolygonMode(getGxmContext(), SCE_GXM_POLYGON_MODE_POINT_01UV);
		break;
	}

	IndexBufferImpl *ib = (IndexBufferImpl*)indexBuffer;
	int ret = sceGxmDraw( getGxmContext(), 
		primitive,
		SCE_GXM_INDEX_FORMAT_U16, 
		ib->getData(),
		count);

	ReferringResources &rr = m_frameBuffer.beginAddingReferringResources();
	rr.add(ib);
	m_gpuBufferState.addToReferringResources(rr);
	m_frameBuffer.endAddingReferringResources();

	return ret;

}

void ssgi::GraphicsContextImpl::addOnSetFrameBufCallback(ssgi::GraphicsContextImpl::OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg)
{
	m_frameBuffer.addOnSetFrameBufCallback(onSetFrameBufCallback, onSetFrameBufCallbackArg);
}

void ssgi::GraphicsContextImpl::removeOnSetFrameBufCallback(ssgi::GraphicsContextImpl::OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg)
{
	m_frameBuffer.removeOnSetFrameBufCallback(onSetFrameBufCallback, onSetFrameBufCallbackArg);
}


SceGxmContext *ssgi::GraphicsContextImpl::getGxmContext(void) const
{
	return m_gxmContext;
}


sce::Vectormath::Simd::Aos::Vector2 ssg::Gxm::GraphicsContextGxm::getDisplaySize() 
{
	return sce::Vectormath::Simd::Aos::Vector2(960,544);
}






int ssgi::GraphicsContextImpl::setVertexProgram(VertexProgram *vertexProgram)
{
	if(vertexProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	sceGxmSetVertexProgram( getGxmContext(), dynamic_cast<VertexProgramImpl*>(vertexProgram)->m_gxmVertexProgram );
	return SCE_OK;
}
int ssgi::GraphicsContextImpl::setFragmentProgram(FragmentProgram *_fragmentProgram)
{
	if(_fragmentProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	FragmentProgramImpl *fragmentProgram = dynamic_cast<FragmentProgramImpl *>(_fragmentProgram);

	SceGxmOutputRegisterSize s = SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT;
	SceGxmMultisampleMode    m = SCE_GXM_MULTISAMPLE_NONE;
	RenderTargetImpl            *r = dynamic_cast<RenderTargetImpl *>(m_gpuBufferState.getCurrentRenderTarget());
	if(r != NULL){
		s = r->getGxmOutputRegisterSize();
		m = r->getGxmMultisampleMode();
	}

	SceGxmFragmentProgram* p = fragmentProgram->m_gxmFragmentPrograms[ s ][ m ];
	sceGxmSetFragmentProgram( getGxmContext(), p );
	return SCE_OK;
}

int ssgi::GraphicsContextImpl::beginScene(RenderTarget *iRenderTarget, DepthStencilSurface *idepthStencilSurface)
{
	RenderTargetImpl        *renderTarget        = dynamic_cast<RenderTargetImpl *>(iRenderTarget);
	DepthStencilSurfaceImpl *depthStencilSurface = dynamic_cast<DepthStencilSurfaceImpl *>(idepthStencilSurface);
	int ret = sceGxmBeginScene(getGxmContext(),
		(renderTarget == NULL) ? 0 : renderTarget->m_flags,
		(renderTarget == NULL) ? 0 : renderTarget->m_renderTarget,
		(renderTarget == NULL) ? 0 : renderTarget->m_validRegion,
		(renderTarget == NULL) ? 0 : renderTarget->m_vertexSyncObject,
		(renderTarget == NULL) ? 0 : renderTarget->m_fragmentSyncObject,
		(renderTarget == NULL) ? 0 : &renderTarget->m_colorSurface,
		(depthStencilSurface==NULL)? NULL : depthStencilSurface->getGxmDepthStencilSurface());
	if(ret < 0){
		return ret;
	}

	m_gpuBufferState.setRenderTarget(renderTarget, depthStencilSurface);

	setViewPort(0, 0, 
		    renderTarget->getTexture()->getWidth(), 
			renderTarget->getTexture()->getHeight(), 
			0.0f, 1.0f);
	setRegionClip(0, 0, renderTarget->getTexture()->getWidth(), renderTarget->getTexture()->getHeight());

	return SCE_OK;
}

int ssgi::GraphicsContextImpl::endScene(void)
{
	return endSceneGxm(NULL, NULL);
}


int ssgi::GraphicsContextImpl::endSceneGxm(const SceGxmNotification *vertexNotification, 
									  const SceGxmNotification *fragmentNotification)
{
	if(m_gpuBufferState.getCurrentRenderTarget() == NULL){
		return -1;
	}
	int ret = sceGxmEndScene(getGxmContext(), 
		vertexNotification,
		fragmentNotification);
	if(ret < 0){
		return ret;
	}

	m_gpuBufferState.setRenderTarget(NULL, NULL);
	return SCE_OK;
}

ssg::Gxm::GraphicsContextGxm *ssg::Gxm::GraphicsContextGxm::downcast(GraphicsContext *loader)
{
	return dynamic_cast<ssg::Gxm::GraphicsContextGxm*>(loader);
}
const ssg::Gxm::GraphicsContextGxm* ssg::Gxm::GraphicsContextGxm::downcast(const GraphicsContext *loader)
{
	return dynamic_cast<const ssg::Gxm::GraphicsContextGxm*>(loader);
}






int ssg::Gxm::createGraphicsLoader(GraphicsLoader **outGraphicsLoader, SceGxmShaderPatcher *patcher)
{
	ssgi::GraphicsLoaderImpl *loader = new ssgi::GraphicsLoaderImpl();
	int ret = loader->initialize(patcher);
	if(ret != SCE_OK){
		delete loader;
		return ret;
	}
	*outGraphicsLoader = loader;
	return SCE_OK;
}

int ssg::createGraphicsContext(GraphicsContext **outGraphicsContext, uint32_t width, uint32_t height,
							   GraphicsContextOption* option)
{
	if(outGraphicsContext == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssgi::GraphicsContextImpl *c = new ssgi::GraphicsContextImpl();

	int ret = c->initialize(width, height, option);
	if(ret < SCE_OK){
		return ret;
	}
	*outGraphicsContext = c;
	return ret;
}

/*
int ssg::destroyGraphicsContext(GraphicsContext *igraphicsContext)
{
	ssgi::GraphicsContextImpl *gc = dynamic_cast<ssgi::GraphicsContextImpl *>(igraphicsContext);
	int ret = gc->finalize();
	if(ret < SCE_OK){
		return ret;
	}
	delete igraphicsContext;
	return SCE_OK;
}
*/

void *ssgi::GraphicsContextImpl::reserveDefaultUniformBuffer(ProgramType programType)
{
	int ret;
	void *buffer;
	if(programType == kProgramTypeVertex){
		ret = sceGxmReserveVertexDefaultUniformBuffer( getGxmContext(), &buffer );
	}else{
		ret = sceGxmReserveFragmentDefaultUniformBuffer( getGxmContext(), &buffer );
	}
	if(ret < 0){
		return NULL;
	}
	return buffer;
}


#endif /* _SCE_TARGET_OS_PSP2 */