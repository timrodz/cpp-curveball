/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS


#pragma comment(lib,"libSceGnmDriver_stub_weak.a")
#pragma comment(lib,"libSceGnm.a")
#pragma comment(lib,"libSceGnmx.a")


#include <sampleutil/graphics.h>
#include "image_gnm.h"
#include "constant_internal_gnm.h" 
#include "program_internal_gnm.h" 
#include "loader_internal_gnm.h"
#include "context_internal_gnm.h"
#include <gpu_address.h>
#include <shader/binary.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <algorithm>
#include <string>
#include <vectormath.h>

#undef min
#undef max

using namespace sce::Vectormath::Simd::Aos;
namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

extern const char _binary_sampleutil_clear_vv_sb_start[];
extern const char _binary_sampleutil_clear_vv_sb_size[];
extern const char _binary_sampleutil_clear_p_sb_start[];
extern const char _binary_sampleutil_clear_p_sb_size[];


#pragma region GpuControl

int ssgi::GpuControl::initialize(void)
{
	m_gfxc = NULL;
	m_depthStencilControl.init();
	m_depthStencilControl.setDepthEnable(true);
	m_zWrite = sce::Gnm::kDepthControlZWriteEnable;
	m_zFunc  = sce::Gnm::kCompareFuncAlways;
	m_depthStencilControl.setDepthControl(m_zWrite, m_zFunc);


	m_primitiveSetup.init();
	m_cullFaceMode = sce::Gnm::kPrimitiveSetupCullFaceNone;
	m_primitiveSetup.setCullFace(m_cullFaceMode);
	m_primitiveSetup.setFrontFace(sce::Gnm::kPrimitiveSetupFrontFaceCw);

	m_scanModeControlAa = sce::Gnm::ScanModeControlAa::kScanModeControlAaDisable;
	m_scanModeControlViewportScissor = sce::Gnm::ScanModeControlViewportScissor::kScanModeControlViewportScissorDisable;

	m_stencilOpControl.init();


	memset(&m_viewPort, 0, sizeof(m_viewPort));
	memset(&m_regionClip,0,  sizeof(m_regionClip));

	return SCE_OK;
}

void ssgi::GpuControl::setContext(sce::Gnmx::GfxContext *gfxc)
{
	m_gfxc = gfxc;

	sce::Gnm::StencilControl stencilControl;
	stencilControl.m_testVal   = m_testVal;
	stencilControl.m_mask      = m_mask;
	stencilControl.m_writeMask = m_writeMask;
	stencilControl.m_opVal     = m_opVal;
	m_gfxc->setStencil(stencilControl);
	m_gfxc->setStencilOpControl(m_stencilOpControl);
	m_gfxc->setDepthStencilControl(m_depthStencilControl);

	m_gfxc->setPrimitiveSetup(m_primitiveSetup);
	m_gfxc->setLineWidth(m_lineWidth);
	m_gfxc->setPointSize(m_pointSizeHalfWidth, m_pointSizeHalfHeight);

	m_gfxc->setViewport(0, m_viewPort.minDepth, m_viewPort.maxDepth, m_viewPort.viewportScale, m_viewPort.viewportOffset);
	m_gfxc->setScreenScissor(m_regionClip.topLeftX, m_regionClip.topLeftY, 
		m_regionClip.topLeftX + m_regionClip.width, m_regionClip.topLeftY + m_regionClip.height);
}


void ssgi::GpuControl::setDepthWriteEnable(bool enable)
{
	m_depthStencilControl.setDepthEnable(enable);
	m_gfxc->setDepthStencilControl(m_depthStencilControl);
}


void ssgi::GpuControl::setDepthFunc(DepthFunc depthFunc)
{
	m_zFunc = convertDepthFunc(depthFunc);
	m_depthStencilControl.setDepthControl(m_zWrite, m_zFunc);
	m_gfxc->setDepthStencilControl(m_depthStencilControl);
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
	m_testVal   = stencilRef;
	m_mask      = compareMask;
	m_writeMask = writeMask;
	m_opVal     = 0x1;
	m_stencilFunc = convertStencilFunc(frontFaceFunc);
	m_stencilFail = convertStencilOp(frontFaceStencilFail);
	m_stencilZPass= convertStencilOp(frontFaceDepthPass);
	m_stencilZfail= convertStencilOp(frontFaceDepthFail);

	m_stencilFuncBack = convertStencilFunc(backFaceFunc);
	m_stencilFailBack = convertStencilOp(backFaceStencilFail);
	m_stencilZPassBack= convertStencilOp(backFaceDepthPass);
	m_stencilZfailBack= convertStencilOp(backFaceDepthFail);

	m_depthStencilControl.setStencilEnable(true);
	m_depthStencilControl.setStencilFunction(m_stencilFunc);
	m_depthStencilControl.setStencilFunctionBack(m_stencilFuncBack);

	m_stencilOpControl.setStencilOps(m_stencilFail, m_stencilZPass, m_stencilZfail);
	m_stencilOpControl.setStencilOpsBack(m_stencilFailBack, m_stencilZPassBack, m_stencilZfailBack);

	sce::Gnm::StencilControl stencilControl;
	stencilControl.init();
	stencilControl.m_testVal   = m_testVal;
	stencilControl.m_mask      = m_mask;
	stencilControl.m_writeMask = m_writeMask;
	stencilControl.m_opVal     = m_opVal;
	m_gfxc->setStencil(stencilControl);
	m_gfxc->setStencilOpControl(m_stencilOpControl);
	m_gfxc->setDepthStencilControl(m_depthStencilControl);

}


void ssgi::GpuControl::setFillMode(bool fill)
{
	if(fill){
		m_primitiveSetup.setPolygonMode(sce::Gnm::kPrimitiveSetupPolygonModeFill, sce::Gnm::kPrimitiveSetupPolygonModeFill);
	}else{
		m_primitiveSetup.setPolygonMode(sce::Gnm::kPrimitiveSetupPolygonModeLine, sce::Gnm::kPrimitiveSetupPolygonModeLine);
	}
	m_primitiveSetup.setCullFace(m_cullFaceMode);
	m_gfxc->setPrimitiveSetup(m_primitiveSetup);
}



void ssgi::GpuControl::setCullMode(CullMode cullMode)
{
	m_cullFaceMode = convertCullMode(cullMode);
	m_primitiveSetup.setCullFace(m_cullFaceMode);
	m_gfxc->setPrimitiveSetup(m_primitiveSetup);

}

void ssgi::GpuControl::setLineWidth(uint32_t width)
{
	m_lineWidth = width;
	m_gfxc->setLineWidth(width);
}

void ssgi::GpuControl::setPointSize(uint16_t halfWidth, uint16_t halfHeight)
{
	m_pointSizeHalfWidth = halfWidth;
	m_pointSizeHalfHeight = halfHeight;
	m_gfxc->setPointSize(halfWidth, halfHeight);
}



void ssgi::GpuControl::setViewPort(uint32_t topLeftX, uint32_t topLeftY,	uint32_t width,	uint32_t height,
											float minDepth,	float maxDepth)
{
	m_viewPort.viewportScale[0] = (float)width*0.5f;
	m_viewPort.viewportScale[1] = -(float)height*0.5f;
	m_viewPort.viewportScale[2] = 0.5f;

	m_viewPort.viewportOffset[0] = (float)topLeftX + (float)width*0.5f;
	m_viewPort.viewportOffset[1] = (float)topLeftY + (float)height*0.5f;
	m_viewPort.viewportOffset[2] = 0.5f;

	m_viewPort.minDepth = minDepth;
	m_viewPort.maxDepth = maxDepth;
	m_gfxc->setViewport(0, m_viewPort.minDepth, m_viewPort.maxDepth, m_viewPort.viewportScale, m_viewPort.viewportOffset);
}

void ssgi::GpuControl::setRegionClip(uint32_t topLeftX,	uint32_t topLeftY,	uint32_t width,	uint32_t height)
{
	m_regionClip.topLeftX = topLeftX;
	m_regionClip.topLeftY = topLeftY;
	m_regionClip.width    = width;
	m_regionClip.height   = height;
	m_gfxc->setScreenScissor(topLeftX, topLeftY, topLeftX + width, topLeftY + height);
}



#pragma endregion

//------------------------------------------------------------------------------------------------------------------------

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
	uint32_t index = 0;
	while(index < 32){
		uint8_t cz = __builtin_clz(usedBufferMask);
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

static inline void referBuffersRw(ssgi::ReferringResources &rr, 
	ssgi::RwStructuredBufferImpl** buffers,
						  uint32_t usedBufferMask)
{
	uint32_t index = 0;
	while(index < 32){
		uint8_t cz = __builtin_clz(usedBufferMask);
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


static inline unsigned __int128  setUsedBit128(unsigned __int128 usedBufferMask, uint32_t index)
{
	unsigned __int128  mask = 0x1;
	return usedBufferMask | (mask <<(127-index));
}

static inline unsigned __int128  unsetUsedBit128(unsigned __int128 usedBufferMask, uint32_t index)
{
	unsigned __int128  mask = 1;
	return usedBufferMask & ~(mask<<(127-index)); 
}
#if 0
static inline void referBuffers128(ssgi::ReferringResources &rr, 
	                      sce::SampleUtil::Impl::ResourceImpl** buffers, 
						  unsigned __int128 usedBufferMask)
{
	uint64_t usedBufferMaskHigh = (uint64_t )(usedBufferMask>>64);
	uint64_t usedBufferMaskLow = (uint64_t )(usedBufferMask);
	//printf("%#lx %#lx\n", usedBufferMaskHigh, usedBufferMaskLow);
	uint32_t index = 0;
	while(index < 64){
		uint8_t cz = __builtin_clzll(usedBufferMaskHigh);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 64){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(buffers[index+cz] != NULL);
		rr.add(buffers[index+cz]);

		usedBufferMaskHigh <<= (cz+1);
		index += cz+1;
	}

	index = 64;
	while(index < 128){
		uint8_t cz = __builtin_clzll(usedBufferMaskLow);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 128){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(buffers[index+cz] != NULL);
		rr.add(buffers[index+cz]);

		usedBufferMaskLow <<= (cz+1);
		index += cz+1;
	}

}

#endif
static inline void referBuffers128_2(ssgi::ReferringResources &rr, 
	                      ssgi::GpuBufferState::ShaderResource* resources, 
						  unsigned __int128 usedBufferMask)
{
	uint64_t usedBufferMaskHigh = (uint64_t )(usedBufferMask>>64);
	uint64_t usedBufferMaskLow = (uint64_t )(usedBufferMask);
	//printf("%#lx %#lx\n", usedBufferMaskHigh, usedBufferMaskLow);
	uint32_t index = 0;
	while(index < 64){
		uint8_t cz = __builtin_clzll(usedBufferMaskHigh);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 64){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(resources[index+cz].resource != NULL);
		rr.add(resources[index+cz].resource);

		usedBufferMaskHigh <<= (cz+1);
		index += cz+1;
	}

	index = 64;
	while(index < 128){
		uint8_t cz = __builtin_clzll(usedBufferMaskLow);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 128){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(resources[index+cz].resource != NULL);
		rr.add(resources[index+cz].resource);

		usedBufferMaskLow <<= (cz+1);
		index += cz+1;
	}

}





int ssgi::GpuBufferState::initialize(void)
{
	m_currentRenderTarget = NULL;
	m_currentDepthStencilSurface = NULL;

	for(uint32_t i=0; i<sce::Gnm::kSlotCountVertexBuffer; i++){
		m_vertexBuffers[i] = NULL;
	}
	for(uint32_t i=0; i<sce::Gnm::kSlotCountResource; i++){
		m_vertexShaderResources[i].clear();
		m_fragmentShaderResources[i].clear();
		m_computeShaderResources[i].clear();
	}

	for(uint32_t i=0; i<sce::Gnm::kSlotCountRwResource; i++){
		m_vertexShaderRwResources[i] = NULL;
		m_fragmentShaderRwResources[i] = NULL;
		m_computeShaderRwResources[i] = NULL;
	}
	
	for(uint32_t i=0; i<sce::Gnm::kSlotCountConstantBuffer; i++){
		m_vertexUniformBuffers[i] = NULL;
		m_fragmentUniformBuffers[i] = NULL;
		m_computeUniformBuffers[i] = NULL;
	}		

	m_vertexBuffersUsedMask = 0;
	m_vertexShaderResourcesUsedMask = 0;
	m_fragmentShaderResourcesUsedMask = 0;
	m_computeShaderResourcesUsedMask = 0;
	
	m_vertexShaderRwResourcesUsedMask = 0;
	m_fragmentShaderRwResourcesUsedMask = 0;
	m_computeShaderRwResourcesUsedMask = 0;
	
	m_vertexUniformBuffersUsedMask = 0;
	m_fragmentUniformBuffersUsedMask = 0;
	m_computeUniformBuffersUsedMask = 0;


	return SCE_OK;
}

void ssgi::GpuBufferState::setContext(sce::Gnmx::GfxContext *gfxc)
{
	finalize();
	m_gfxc = gfxc;
}

int ssgi::GpuBufferState::finalize(void)
{
	if(m_currentRenderTarget != NULL){
		sce::SampleUtil::Impl::destroyImpl(m_currentRenderTarget);
		m_currentRenderTarget = NULL;
	}
	if(m_currentDepthStencilSurface != NULL){
		sce::SampleUtil::Impl::destroyImpl(m_currentDepthStencilSurface);
		m_currentDepthStencilSurface = NULL;
	}

	for(uint32_t i=0; i<sce::Gnm::kSlotCountVertexBuffer; i++){
		if(m_vertexBuffers[i] != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_vertexBuffers[i]);
			m_vertexBuffers[i] = NULL;
		}
	}
	for(uint32_t i=0; i<sce::Gnm::kSlotCountResource; i++){
		if(m_vertexShaderResources[i].resource != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_vertexShaderResources[i].resource);
			m_vertexShaderResources[i].clear();
		}
		if(	m_fragmentShaderResources[i].resource != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_fragmentShaderResources[i].resource);
			m_fragmentShaderResources[i].clear();
		}
		if(m_computeShaderResources[i].resource != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_computeShaderResources[i].resource);
			m_computeShaderResources[i].clear();
		}
	}

	for(uint32_t i=0; i<sce::Gnm::kSlotCountRwResource; i++){
		if(m_vertexShaderRwResources[i] != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_vertexShaderRwResources[i]);
			m_vertexShaderRwResources[i] = NULL;
		}
		if(m_fragmentShaderRwResources[i] != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_fragmentShaderRwResources[i]);
			m_fragmentShaderRwResources[i] = NULL;
		}
		if(m_computeShaderRwResources[i] != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_fragmentShaderRwResources[i]);
			m_fragmentShaderRwResources[i] = NULL;
		}
	}
	
	for(uint32_t i=0; i<sce::Gnm::kSlotCountConstantBuffer; i++){
		if(m_vertexUniformBuffers[i] != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_vertexUniformBuffers[i]);
			m_vertexUniformBuffers[i] = NULL;
		}
		if(m_fragmentUniformBuffers[i] != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_fragmentUniformBuffers[i]);
			m_fragmentUniformBuffers[i] = NULL;
		}
		if(m_computeUniformBuffers[i] != NULL){
			sce::SampleUtil::Impl::destroyImpl(m_fragmentUniformBuffers[i]);
			m_fragmentUniformBuffers[i] = NULL;
		}
	}		

	m_vertexBuffersUsedMask = 0;
	m_vertexShaderResourcesUsedMask = 0;
	m_fragmentShaderResourcesUsedMask = 0;
	m_computeShaderResourcesUsedMask = 0;
	
	m_vertexShaderRwResourcesUsedMask = 0;
	m_fragmentShaderRwResourcesUsedMask = 0;
	m_computeShaderRwResourcesUsedMask = 0;
	
	m_vertexUniformBuffersUsedMask = 0;
	m_fragmentUniformBuffersUsedMask = 0;
	m_computeUniformBuffersUsedMask = 0;

	m_gfxc = NULL;
	return SCE_OK;
}

int ssgi::GpuBufferState::setVertexBuffer(VertexBuffer *_vertexBuffer, uint32_t streamIndex)
{
	if(streamIndex >= sce::Gnm::kSlotCountVertexBuffer){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	Impl::VertexBufferImpl *bufferImpl = dynamic_cast<Impl::VertexBufferImpl *>(_vertexBuffer);
	if(m_vertexBuffers[streamIndex] != NULL){
		sce::SampleUtil::Impl::destroyImpl(m_vertexBuffers[streamIndex]);
	}

	if(bufferImpl != NULL){
		bufferImpl->refer();
		m_vertexBuffersUsedMask = setUsedBit(m_vertexBuffersUsedMask, streamIndex);
	}else{
		m_vertexBuffersUsedMask = unsetUsedBit(m_vertexBuffersUsedMask, streamIndex);
	}

	m_vertexBuffers[streamIndex] = bufferImpl;
	return SCE_OK;
}

int ssgi::GpuBufferState::setRenderTarget(RenderTarget* renderTarget, DepthStencilSurface *depthStencil)
{
	ssgi::RenderTargetImpl        *rtImpl = dynamic_cast<ssgi::RenderTargetImpl *>(renderTarget);
	ssgi::DepthStencilSurfaceImpl *dsImpl = dynamic_cast<ssgi::DepthStencilSurfaceImpl *>(depthStencil);

	if(m_currentRenderTarget != NULL){
		sce::SampleUtil::Impl::destroyImpl(m_currentRenderTarget);
	}
	if(rtImpl != NULL){
		rtImpl->refer();
		m_gfxc->setRenderTarget(0, &rtImpl->m_renderTarget);
	}else{
		m_gfxc->setRenderTarget(0, NULL);
	}
	m_currentRenderTarget = rtImpl;

	///
	if(m_currentDepthStencilSurface != NULL){
		sce::SampleUtil::Impl::destroyImpl(m_currentDepthStencilSurface);
	}
	if(dsImpl != NULL){
		dsImpl->refer();
		m_gfxc->setDepthRenderTarget(dsImpl->getGnmDepthRenderTarget());
	}else{
		m_gfxc->setDepthRenderTarget(NULL);
	}
	m_currentDepthStencilSurface = dsImpl;
	return SCE_OK;
}

ssg::RenderTarget *ssgi::GpuBufferState::getCurrentRenderTarget(void) const
{
	return m_currentRenderTarget;
}
ssg::DepthStencilSurface *ssgi::GpuBufferState::getCurrentDepthStencilSurface(void) const
{
	return m_currentDepthStencilSurface;
}



int ssgi::GpuBufferState::setTexture(ProgramType programType, Texture *texture, uint32_t index)
{
	if(sce::Gnm::kSlotCountResource <= index){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	TextureImpl *impl = dynamic_cast<TextureImpl *>(texture);

	ShaderResource* r = NULL;
	unsigned __int128 *usedMask;
	switch(programType)
	{
	case kProgramTypeVertex:
		{
			r = m_vertexShaderResources;
			usedMask = &m_vertexShaderResourcesUsedMask;
			break;
		}
	case kProgramTypeFragment:
		{
			r = m_fragmentShaderResources;
			usedMask = &m_fragmentShaderResourcesUsedMask;
			break;
		}
	case kProgramTypeCompute:
		{
			r = m_computeShaderResources;
			usedMask = &m_computeShaderResourcesUsedMask;
			break;
		}
	default:
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if(r[index].resource != NULL){
		sce::SampleUtil::Impl::destroyImpl(r[index].resource);
	}
	r[index].clear();

	r[index].resource = impl;
	//printf("set %d=%p\n", index, impl);
	sce::Gnm::ShaderStage stage = getShaderStage(programType);
	if(r[index].resource != NULL){
		r[index].resource->refer();
		r[index].texture = &impl->m_texture;
		r[index].sampler = &impl->m_sampler;
		//m_gfxc->setTextures(stage, index, 1, &impl->m_texture);
		//m_gfxc->setSamplers(stage, index, 1, &impl->m_sampler);
		*usedMask = setUsedBit128(*usedMask, index);
	}else{
		m_gfxc->setTextures(stage, index, 1, NULL);
		m_gfxc->setSamplers(stage, index, 1, NULL);
		*usedMask = unsetUsedBit128(*usedMask, index);
	}

	return SCE_OK;
}


int ssgi::GpuBufferState::setUniformBuffer(ProgramType programType, UniformBuffer *_buffer,  uint32_t index)
{
	if(sce::Gnm::kSlotCountConstantBuffer <= index){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	ssgi::UniformBufferImpl *impl = dynamic_cast<ssgi::UniformBufferImpl *>(_buffer);
	sce::SampleUtil::Impl::ResourceImpl** r = NULL;
	uint32_t *usedMask;
	switch(programType)
	{
	case kProgramTypeVertex:
		{
			r = m_vertexUniformBuffers;
			usedMask = &m_vertexUniformBuffersUsedMask;
			break;
		}
	case kProgramTypeFragment:
		{
			r = m_fragmentUniformBuffers;
			usedMask = &m_fragmentUniformBuffersUsedMask;
			break;
		}
	case kProgramTypeCompute:
		{
			r = m_computeUniformBuffers;
			usedMask = &m_computeUniformBuffersUsedMask;
			break;
		}
	default:
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if(r[index] != NULL){
		sce::SampleUtil::Impl::destroyImpl(r[index]);
	}

	r[index] = impl;

	sce::Gnm::ShaderStage stage = getShaderStage(programType);
	if(r[index] != NULL){
		r[index]->refer();

		sce::Gnm::Buffer constBuffer;
		constBuffer.initAsConstantBuffer( (void*)impl->getData(), impl->getSize()); // todo
		//m_gfxc->setConstantBuffers(stage, index, 1, &constBuffer);
		*usedMask = setUsedBit(*usedMask, index);
	}else{
		m_gfxc->setConstantBuffers(stage, index, 1, NULL);
		*usedMask = unsetUsedBit(*usedMask, index);
	}
	return SCE_OK;
}


int ssgi::GpuBufferState::setResource(ssg::ProgramType programType, ssg::Texture *texture, uint32_t index)
{
	return setTexture(programType, texture, index);
}
int ssgi::GpuBufferState::setResource(ssg::ProgramType programType, ssg::StructuredBuffer *buffer, uint32_t index)
{
	if(sce::Gnm::kSlotCountResource <= index){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	ssgi::StructuredBufferImpl *impl = dynamic_cast<ssgi::StructuredBufferImpl *>(buffer);
	ShaderResource* r = NULL;
	unsigned __int128 *usedMask;
	switch(programType)
	{
	case kProgramTypeVertex:
		{
			r = m_vertexShaderResources;
			usedMask = &m_vertexShaderResourcesUsedMask;
			break;
		}
	case kProgramTypeFragment:
		{
			r = m_fragmentShaderResources;
			usedMask = &m_fragmentShaderResourcesUsedMask;
			break;
		}
	case kProgramTypeCompute:
		{
			r = m_computeShaderResources;
			usedMask = &m_computeShaderResourcesUsedMask;
			break;
		}
	default:
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if(r[index].resource != NULL){
		sce::SampleUtil::Impl::destroyImpl(r[index].resource);
	}
	r[index].clear();
	//printf("set %d=%p\n", index, impl);
	r[index].resource = impl;

	sce::Gnm::ShaderStage stage = getShaderStage(programType);
	if(r[index].resource != NULL){
		r[index].resource->refer();
		r[index].buffer = impl->getGnmBuffer();
		//m_gfxc->setBuffers(stage, index, 1, impl->getGnmBuffer());
		*usedMask = setUsedBit128(*usedMask, index);
	}else{
		m_gfxc->setBuffers(stage, index, 1, NULL);
		*usedMask = unsetUsedBit128(*usedMask, index);
	}
	return SCE_OK;
}
int ssgi::GpuBufferState::setRwResource(ssg::ProgramType programType, ssg::RwStructuredBuffer *buffer, uint32_t index)
{
	if(sce::Gnm::kSlotCountRwResource <= index){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	ssgi::RwStructuredBufferImpl *impl = dynamic_cast<ssgi::RwStructuredBufferImpl *>(buffer);
	ssgi::RwStructuredBufferImpl** r = NULL;
	uint32_t *usedMask;
	switch(programType)
	{
	case kProgramTypeVertex:
		{
			r = m_vertexShaderRwResources;
			usedMask = &m_vertexShaderRwResourcesUsedMask;
			break;
		}
	case kProgramTypeFragment:
		{
			r = m_fragmentShaderRwResources;
			usedMask = &m_fragmentShaderRwResourcesUsedMask;
			break;
		}
	case kProgramTypeCompute:
		{
			r = m_computeShaderRwResources;
			usedMask = &m_computeShaderRwResourcesUsedMask;
			break;
		}
	default:
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if(r[index] != NULL){
		sce::SampleUtil::Impl::destroyImpl(r[index]);
	}

	r[index] = impl;

	sce::Gnm::ShaderStage stage = getShaderStage(programType);
	if(r[index] != NULL){
		r[index]->refer();
		//m_gfxc->setRwBuffers(stage, index, 1, impl->getGnmBuffer());
		*usedMask = setUsedBit(*usedMask, index);
	}else{
		m_gfxc->setRwBuffers(stage, index, 1, NULL);
		*usedMask = unsetUsedBit(*usedMask, index);
	}
	return SCE_OK;
}


void ssgi::GpuBufferState::addToReferringResources(ReferringResources &rr)
{
	if(m_currentRenderTarget != NULL){
		rr.add(m_currentRenderTarget);
	}
	if(m_currentDepthStencilSurface != NULL){
		rr.add(m_currentDepthStencilSurface);
	}

	referBuffers(rr, m_vertexBuffers, m_vertexBuffersUsedMask);

	referBuffers128_2(rr, m_vertexShaderResources, m_vertexShaderResourcesUsedMask);
	referBuffers128_2(rr, m_fragmentShaderResources, m_fragmentShaderResourcesUsedMask);
	referBuffers128_2(rr, m_computeShaderResources,  m_computeShaderResourcesUsedMask);

	referBuffersRw(rr, m_vertexShaderRwResources, m_vertexShaderRwResourcesUsedMask);
	referBuffersRw(rr, m_fragmentShaderRwResources, m_fragmentShaderRwResourcesUsedMask);
	referBuffersRw(rr, m_computeShaderRwResources, m_computeShaderRwResourcesUsedMask);
	
	referBuffers(rr, m_vertexUniformBuffers, m_vertexUniformBuffersUsedMask);
	referBuffers(rr, m_fragmentUniformBuffers, m_fragmentUniformBuffersUsedMask);
	referBuffers(rr, m_computeUniformBuffers, m_computeUniformBuffersUsedMask);

}

#pragma endregion
//------------------------------------------------------------------------------------------------------------------------


void ssgi::GpuBufferState::setUpVertexBuffers(const ssgi::VertexProgramImpl *vertexProgram)
{
	for(uint32_t i=0; i<vertexProgram->m_numAttrs; i++){
		uint32_t streamIndex = vertexProgram->m_attributes[i].attr.streamIndex;
		int reg = vertexProgram->m_attributes[i].reg;
		if(reg<0){
		   continue;
		}
		const VertexBufferImpl *vertexBuffer = dynamic_cast<const VertexBufferImpl *>(m_vertexBuffers[streamIndex]);
		if(vertexBuffer == NULL){
			m_gfxc->setVertexBuffers(sce::Gnm::kShaderStageVs, reg, 1, NULL);
		}else{
			sce::Gnm::Buffer gnmVertexBuffer;
			
			void *vptr  = (void*)(((uintptr_t)vertexBuffer->getData()) + vertexProgram->m_attributes[i].attr.offset);
			gnmVertexBuffer.setBaseAddress(vptr);

			uint32_t stride = vertexProgram->m_streams[streamIndex].stride;
			uint32_t componentCount = vertexProgram->m_attributes[i].attr.componentCount;
			
			sce::Gnm::DataFormat gnmFormat = getFormat(vertexProgram->m_attributes[i].attr.format, componentCount);

			uint32_t numElements = (stride != 0) ? (vertexBuffer->getSize() / vertexProgram->m_streams[streamIndex].stride) : (gnmFormat.getBytesPerElement());
			
			gnmVertexBuffer.initAsVertexBuffer(   vptr,
				gnmFormat, 
				stride,
				numElements);

			m_gfxc->setVertexBuffers(sce::Gnm::kShaderStageVs, reg, 1, &gnmVertexBuffer);
		}
	}

}


#pragma region GraphicsContextImpl

ssg::Gnm::GraphicsContextGnm* ssg::Gnm::GraphicsContextGnm::cast(ssg::GraphicsContext *from)
{
	return dynamic_cast<ssg::Gnm::GraphicsContextGnm*>(from);
}
const ssg::Gnm::GraphicsContextGnm* ssg::Gnm::GraphicsContextGnm::cast(const ssg::GraphicsContext *from)
{
	return dynamic_cast<const ssg::Gnm::GraphicsContextGnm*>(from);
}



int ssgi::GraphicsContextImpl::UtilShader::initialize(ssgi::GraphicsLoaderImpl *loader)
{
	m_loader = loader;

	m_vertexProgramId = loader->registerProgramFromMemory(
		_binary_sampleutil_clear_vv_sb_start,
		(uintptr_t)_binary_sampleutil_clear_vv_sb_size);

	m_pixelProgramId = loader->registerProgramFromMemory(
		_binary_sampleutil_clear_p_sb_start, 
		(uintptr_t)_binary_sampleutil_clear_p_sb_size);


	int ret = loader->createVertexProgram(&m_vertexProgram, m_vertexProgramId,
		NULL, 0,
		NULL, 0);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	ret = loader->createFragmentProgram(&m_fragmentProgram, m_pixelProgramId, NULL, NULL);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	return SCE_OK;
}
int ssgi::GraphicsContextImpl::UtilShader::finalize(void)
{
	if(m_vertexProgram != NULL){
		m_loader->releaseVertexProgram(m_vertexProgram);
		m_vertexProgram = NULL;
	}
	if(m_fragmentProgram != NULL){
		m_loader->releaseFragmentProgram(m_fragmentProgram);
		m_fragmentProgram = NULL;
	}
	if(m_vertexProgramId != NULL){
		m_loader->unregisterProgram(m_vertexProgramId);
		m_vertexProgramId = NULL;
	}
	if(m_pixelProgramId != NULL){
		m_loader->unregisterProgram(m_pixelProgramId);
		m_pixelProgramId = NULL;
	}
	return	 SCE_OK;
}

/* ----------------------------------------------------------------------------------------------------------------------------------------------------*/


ssgi::GraphicsContextImpl::GraphicsContextImpl()
{
	m_frameCount = 0;

	m_vertexDefaultUniformBuffer = NULL; 
	m_fragmentDefaultUniformBuffer = NULL;  

	m_vertexProgram    =NULL;
	m_fragmentProgram  =NULL;
	m_computeProgram   =NULL;
}

ssgi::GraphicsContextImpl::~GraphicsContextImpl()
{
	finalize();
	//SCE_GNM_ASSERT(!m_regionMemPtr);
}

int ssgi::GraphicsContextImpl::initialize(uint32_t width, uint32_t height, GraphicsContextOption* option )
{
	int ret = SCE_OK;

	if(option != NULL){
		m_graphicsOption = *option;
	}
	ret = m_loader.initialize(1024 * 1024 * 32,  
		m_graphicsOption.videoHeapSize!=0 ? m_graphicsOption.videoHeapSize: 1024*1024*1024 );

	if(ret < SCE_OK){
		printf(__FILE__"(%d) Error : ret=%#x\n", __LINE__, ret);
		return ret;
	}

	m_labels = (volatile uint64_t*)getHeap()->allocateVideoShared(sizeof(uint64_t), sizeof(uint64_t) * m_graphicsOption.numFrameBuffer);
	m_expectedLabels = new uint32_t[m_graphicsOption.numFrameBuffer];
	for(uint32_t i=0;i< m_graphicsOption.numFrameBuffer; i++){
		m_labels[i] = 0xffffffff;
		m_expectedLabels[i] = 0xffffffff;
	}

	// Set up gfx context
	const uint32_t kNumRingEntries = 64;
	const uint32_t cueHeapSize        = sce::Gnmx::ConstantUpdateEngine::computeHeapSize(kNumRingEntries);

	m_commandContexts = new CommandContext[m_graphicsOption.numFrameBuffer];
	for(uint32_t i=0; i<m_graphicsOption.numFrameBuffer; i++){
		m_commandContexts[i].m_cueHeapBuffer        = m_loader.getHeap()->allocateVideoShared(sce::Gnm::kAlignmentOfShaderInBytes, cueHeapSize);
		m_commandContexts[i].m_dcbBuffer            = getHeap()->allocateSystemShared(sce::Gnm::kAlignmentOfBufferInBytes, sce::Gnm::kIndirectBufferMaximumSizeInBytes);
		m_commandContexts[i].m_ccbBuffer            = getHeap()->allocateSystemShared(sce::Gnm::kAlignmentOfBufferInBytes, sce::Gnm::kIndirectBufferMaximumSizeInBytes);

#if 0  	
		m_commandContexts[i].m_gfxc.init(m_commandContexts[i].m_cueCpRamShadowBuffer,
			m_commandContexts[i].m_cueHeapBuffer, kNumRingEntries,		// Constant Update Engine
			m_commandContexts[i].m_dcbBuffer,     sce::Gnm::kIndirectBufferMaximumSizeInBytes,		// Draw command buffer
			m_commandContexts[i].m_ccbBuffer,     sce::Gnm::kIndirectBufferMaximumSizeInBytes			// Constant command buffer
			);
#else
		m_commandContexts[i].m_gfxc.init(
			m_commandContexts[i].m_cueHeapBuffer, kNumRingEntries,		// Constant Update Engine
			m_commandContexts[i].m_dcbBuffer, sce::Gnm::kIndirectBufferMaximumSizeInBytes,		// Draw command buffer
			m_commandContexts[i].m_ccbBuffer, sce::Gnm::kIndirectBufferMaximumSizeInBytes			// Constant command buffer
		);
#endif



		
		m_commandContexts[i].m_gfxc.reset();
		m_commandContexts[i].m_gfxc.initializeDefaultHardwareState();
	}
	m_currentGfxc = &m_commandContexts[0].m_gfxc;

	//
	// Setup the rendering:
	//
	m_frameBuffer = new Impl::FrameBuffer;


	bool stencil = false;
	bool htile   = true;

	ret = m_frameBuffer->initialize(&m_loader, width, height, m_graphicsOption.numFrameBuffer,
		stencil,htile,	m_graphicsOption.multisampleMode, m_graphicsOption.enableVrView, m_graphicsOption.enableS3dView );
	if(ret < 0){
		printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
		return ret;
	}
	m_nextRenderTargetIndex = 0;

	/*
	*    init shaders
	*/
	ret = m_clearShader.initialize(&m_loader);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	uint32_t size = 4096 * 2;
	uint32_t align = sce::Gnm::kAlignmentOfBufferInBytes;
	m_copyBuffer      = getHeap()->allocateVideoShared(align, size);
	m_copyBufferSize = size;

	ret = m_control.initialize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	m_control.setContext(m_currentGfxc);

	ret = m_bufferState.initialize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	m_bufferState.setContext(m_currentGfxc);



	// TODO : need to be freed.
	return SCE_OK;
}

int ssgi::GraphicsContextImpl::finalize( )
{
	//printf("finalize m_graphicsOption.numFrameBuffer=%d\n", m_graphicsOption.numFrameBuffer);
	for(uint32_t i=0; i<m_graphicsOption.numFrameBuffer; i++){
		// sweep out all pending draw calls.
		flip(1);
	}
	for(uint32_t i=0; i<m_graphicsOption.numFrameBuffer; i++){
		waitDrawFinish(i);
		m_frameBuffer->waitUntilSafeForRendering(m_currentGfxc, i);
	}

	setVertexProgram(NULL);
	setFragmentProgram(NULL);

	int ret = m_bufferState.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	for(uint32_t i=0; i<m_graphicsOption.numFrameBuffer; i++){
		getHeap()->releaseVideoShared(m_commandContexts[i].m_cueHeapBuffer);
		getHeap()->releaseSystemShared(m_commandContexts[i].m_dcbBuffer);
		getHeap()->releaseSystemShared(m_commandContexts[i].m_ccbBuffer);
	}
	delete [] m_commandContexts;
	m_commandContexts = NULL;

	ret = m_clearShader.finalize();
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_frameBuffer->finalize();
	delete m_frameBuffer;
	m_frameBuffer = NULL;

	if(m_labels != NULL){
		getHeap()->releaseVideoShared((void*)m_labels);
		m_labels = NULL;
	}
	if(m_expectedLabels != NULL){
		delete [] m_expectedLabels;
		m_expectedLabels = NULL;
	}

	m_loader.finalize();
	return SCE_OK;
}





static inline Vector4 convertColor( uint32_t color )
{
	Vector4 dest;
	dest.setX( static_cast<float>( ( color >> 16 ) & 0xFF ) );
	dest.setY( static_cast<float>( ( color >>  8 ) & 0xFF ) );
	dest.setZ( static_cast<float>( ( color >>  0 ) & 0xFF ) );
	dest.setW( static_cast<float>( ( color >> 24 ) & 0xFF ) );
	return dest / 255.f;
}


void ssgi::GraphicsContextImpl::clearRenderTarget(uint32_t color)
{

	setDepthFunc(kDepthFuncAlways);
	setFillMode(true);
	setCullMode(sce::SampleUtil::Graphics::kCullNone);

	m_currentGfxc->setDepthClearValue(1);

	m_currentGfxc->pushMarker("SurfaceUtil::ClearColorDepthStencil");
	//SCE_GNM_ASSERT(s_vertexShaderBinary && s_pixelShaderBinary);

	sce::Gnm::DbRenderControl dbRenderControl;
	dbRenderControl.init();
	dbRenderControl.setDepthClearEnable(true);
	dbRenderControl.setStencilClearEnable(true);
	m_currentGfxc->setDbRenderControl(dbRenderControl);

	sce::Gnm::DepthStencilControl depthControl;
	sce::Gnm::StencilOpControl stencilControl;
	depthControl.init();
	depthControl.setDepthControl( sce::Gnm::kDepthControlZWriteEnable, sce::Gnm::kCompareFuncAlways );
	depthControl.setDepthEnable( true );

	stencilControl.init();
	stencilControl.setStencilOps( sce::Gnm::kStencilOpReplaceTest, sce::Gnm::kStencilOpReplaceTest, sce::Gnm::kStencilOpReplaceTest);
	depthControl.setStencilFunction( sce::Gnm::kCompareFuncAlways );
	m_currentGfxc->setDepthStencilControl( depthControl );
	m_currentGfxc->setStencilOpControl( stencilControl );

	setVertexProgram(m_clearShader.m_vertexProgram);
	setFragmentProgram(m_clearShader.m_fragmentProgram);
	Vector4* constantBuffer = static_cast<Vector4*>( reserveDefaultUniformBuffer(kProgramTypeFragment));
	*constantBuffer = convertColor(color );

	drawIndexAuto(kPrimitiveTriangles, 3);


	dbRenderControl.setDepthClearEnable(false);
	dbRenderControl.setStencilClearEnable(false);
	m_currentGfxc->setDbRenderControl(dbRenderControl);
	m_currentGfxc->popMarker();


}


void ssgi::GraphicsContextImpl::notifyDrawFinish(uint32_t bufferIndex)
{
	m_expectedLabels[bufferIndex] = m_frameCount;
	//printf("notify bufferIndex=%d, m_expectedLabels[bufferIndex]=%d\n", bufferIndex, m_expectedLabels[bufferIndex]);
	++m_frameCount;
	m_commandContexts[bufferIndex].m_gfxc.writeImmediateAtEndOfPipe(sce::Gnm::kEopFlushCbDbCaches,
										(void *)&m_labels[bufferIndex], 
										m_expectedLabels[bufferIndex],
										sce::Gnm::kCacheActionNone);

}
void ssgi::GraphicsContextImpl::waitDrawFinish(uint32_t bufferIndex)
{
	//printf("wait bufferIndex=%d, %lx,%x\n", bufferIndex, m_labels[bufferIndex], m_expectedLabels[bufferIndex]);
	volatile uint32_t wait = 0;
	while(m_labels[bufferIndex] != m_expectedLabels[bufferIndex]){
		wait++;
		if((wait % 1000000) == 0){
			printf("wait bufferIndex=%d, %lx,%x\n", bufferIndex, m_labels[bufferIndex], m_expectedLabels[bufferIndex]);
			m_frameBuffer->dump();
		}
	}
}

int ssgi::GraphicsContextImpl::flip( uint32_t numVsync )
{
	notifyDrawFinish(m_nextRenderTargetIndex);
	m_frameBuffer->submitFlip(m_currentGfxc, m_nextRenderTargetIndex);
	
	if(!m_graphicsOption.disableSubmitDoneInFlip){
		sce::Gnm::submitDone();
	}

	return prepareNextFrame();
}

int ssgi::GraphicsContextImpl::submitOnly(void)
{
	notifyDrawFinish(m_nextRenderTargetIndex);
	m_frameBuffer->submit(m_currentGfxc);
	
	if(!m_graphicsOption.disableSubmitDoneInFlip){
		sce::Gnm::submitDone();
	}

	return SCE_OK;
}

int ssgi::GraphicsContextImpl::prepareNextFrame(void)
{
	uint32_t nextIndex = (m_nextRenderTargetIndex+1) % m_frameBuffer->m_videoInfo.m_numBuffers;
	m_nextRenderTargetIndex = nextIndex;
	m_currentGfxc = &m_commandContexts[nextIndex].m_gfxc;

	waitDrawFinish(nextIndex);
	m_frameBuffer->pendingCheck(6);

	m_frameBuffer->freeReferringResources(nextIndex);

	m_currentGfxc->reset();
	m_control.setContext(m_currentGfxc);
	m_bufferState.setContext(m_currentGfxc);

	m_frameBuffer->waitUntilSafeForRenderingByCpu(nextIndex);
	m_frameBuffer->waitUntilSafeForRendering(m_currentGfxc, nextIndex);
	return SCE_OK;
}



void *ssgi::GraphicsContextImpl::reserveDefaultUniformBuffer(ProgramType programType)
{
	switch(programType){
	case kProgramTypeVertex:
		{
			if(m_vertexProgram == NULL){
				return NULL;
			}
			const ProgramImpl *p = dynamic_cast<const ProgramImpl *>(m_vertexProgram->m_programId);
			if(p->m_program->m_defaultUniformBufferSize == 0){
				return NULL;
			}
			m_vertexDefaultUniformBuffer = m_currentGfxc->allocateFromCommandBuffer(p->m_program->m_defaultUniformBufferSize , sce::Gnm::kEmbeddedDataAlignment16 );
			return m_vertexDefaultUniformBuffer;
		}
	case kProgramTypeFragment:
		{
			if(m_fragmentProgram == NULL){
				return NULL;
			}
			const ProgramImpl *p = dynamic_cast<const ProgramImpl *>(m_fragmentProgram->m_programId);
			if(p->m_program->m_defaultUniformBufferSize == 0){
				return NULL;
			}
			m_fragmentDefaultUniformBuffer = m_currentGfxc->allocateFromCommandBuffer(p->m_program->m_defaultUniformBufferSize, sce::Gnm::kEmbeddedDataAlignment16 );
			return m_fragmentDefaultUniformBuffer;
		}
	case kProgramTypeCompute:
		{
			if(m_computeProgram == NULL){
				return NULL;
			}
			const ProgramImpl *p = dynamic_cast<const ProgramImpl *>(m_computeProgram->m_programId);
			if(p->m_program->m_defaultUniformBufferSize == 0){
				return NULL;
			}
			m_computeDefaultUniformBuffer = m_currentGfxc->allocateFromCommandBuffer(p->m_program->m_defaultUniformBufferSize, sce::Gnm::kEmbeddedDataAlignment16 );
			return m_computeDefaultUniformBuffer;
		}
	default:
		return NULL;
	}
}


void ssgi::GraphicsContextImpl::setUpDrawBuffers(void)
{
	ResourceImpl **r;
	sce::Gnm::ShaderStage stage;

	// Vertex constant

	stage = getShaderStage(kProgramTypeVertex);
	r = m_bufferState.m_vertexUniformBuffers;
	uint32_t index = 0;
	uint32_t usedBufferMask = m_bufferState.m_vertexUniformBuffersUsedMask;
	while(index < 32){
		uint8_t cz = __builtin_clz(usedBufferMask);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 32){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(r[index+cz] != NULL);

		ssgi::UniformBufferImpl *impl = dynamic_cast<ssgi::UniformBufferImpl *>(r[index+cz]);
		sce::Gnm::Buffer constBuffer;
		constBuffer.initAsConstantBuffer((void*)impl->getData(), impl->getSize()); // todo
		m_currentGfxc->setConstantBuffers(stage, index+cz, 1, &constBuffer);

		usedBufferMask <<= (cz+1);
		index += cz+1;
	}
	/*
	for (int i = 0; i < sce::Gnm::kSlotCountConstantBuffer; i++){
		if (r[i] != NULL){
			ssgi::UniformBufferImpl *impl = dynamic_cast<ssgi::UniformBufferImpl *>(r[i]);
			sce::Gnm::Buffer constBuffer;
			constBuffer.initAsConstantBuffer((void*)impl->getData(), impl->getSize()); // todo
			m_currentGfxc->setConstantBuffers(stage, i, 1, &constBuffer);
		}

	}
	*/

	// Fragment constant
	stage = getShaderStage(kProgramTypeFragment);
	r = m_bufferState.m_fragmentUniformBuffers;
	index = 0;
	usedBufferMask = m_bufferState.m_fragmentUniformBuffersUsedMask;
	while(index < 32){
		uint8_t cz = __builtin_clz(usedBufferMask);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 32){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(r[index+cz] != NULL);

		ssgi::UniformBufferImpl *impl = dynamic_cast<ssgi::UniformBufferImpl *>(r[index+cz]);
		sce::Gnm::Buffer constBuffer;
		constBuffer.initAsConstantBuffer((void*)impl->getData(), impl->getSize()); // todo
		m_currentGfxc->setConstantBuffers(stage, index+cz, 1, &constBuffer);

		usedBufferMask <<= (cz+1);
		index += cz+1;
	}
	/*
	for (int i = 0; i < sce::Gnm::kSlotCountConstantBuffer; i++){
		if (r[i] != NULL){
			ssgi::UniformBufferImpl *impl = dynamic_cast<ssgi::UniformBufferImpl *>(r[i]);
			sce::Gnm::Buffer constBuffer;
			constBuffer.initAsConstantBuffer((void*)impl->getData(), impl->getSize()); // todo
			m_currentGfxc->setConstantBuffers(stage, i, 1, &constBuffer);
		}

	}
	*/

	// Vertex Resource
	stage = getShaderStage(kProgramTypeVertex);
	GpuBufferState::ShaderResource *res = m_bufferState.m_vertexShaderResources;
	uint64_t usedBufferMaskHigh = (uint64_t )(m_bufferState.m_vertexShaderResourcesUsedMask>>64);
	uint64_t usedBufferMaskLow = (uint64_t )(m_bufferState.m_vertexShaderResourcesUsedMask);

	index = 0;
	while(index < 64){
		uint8_t cz = __builtin_clzll(usedBufferMaskHigh);
		if(index+cz >= 64){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(res[index+cz].resource != NULL);
		if (res[index + cz].buffer != NULL){
			m_currentGfxc->setBuffers(stage, index + cz, 1, res[index + cz].buffer);
		}
		else{
			m_currentGfxc->setTextures(stage, index + cz, 1, res[index + cz].texture);
			m_currentGfxc->setSamplers(stage, index + cz, 1, res[index + cz].sampler);
		}

		usedBufferMaskHigh <<= (cz+1);
		index += cz+1;
	}

	index = 64;
	while(index < 128){
		uint8_t cz = __builtin_clzll(usedBufferMaskLow);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 128){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(res[index+cz].resource != NULL);
		if (res[index + cz].buffer != NULL){
			m_currentGfxc->setBuffers(stage, index + cz, 1, res[index + cz].buffer);
		}
		else{
			m_currentGfxc->setTextures(stage, index + cz, 1, res[index + cz].texture);
			m_currentGfxc->setSamplers(stage, index + cz, 1, res[index + cz].sampler);
		}

		usedBufferMaskLow <<= (cz+1);
		index += cz+1;
	}

	/*
	for (int i = 0; i < sce::Gnm::kSlotCountResource; i++){
		if (res[i].resource != NULL){
			if (res[i].buffer != NULL){
				m_currentGfxc->setBuffers(stage, i, 1, res[i].buffer);
			}else{
				m_currentGfxc->setTextures(stage, i, 1, res[i].texture);
				m_currentGfxc->setSamplers(stage, i, 1, res[i].sampler);
			}
		}
	}
	*/
	// Fragment Resource
	stage = getShaderStage(kProgramTypeFragment);
	res = m_bufferState.m_fragmentShaderResources;
	usedBufferMaskHigh = (uint64_t)(m_bufferState.m_fragmentShaderResourcesUsedMask >> 64);
	usedBufferMaskLow = (uint64_t )(m_bufferState.m_fragmentShaderResourcesUsedMask);

	index = 0;
	while(index < 64){
		uint8_t cz = __builtin_clzll(usedBufferMaskHigh);
		if(index+cz >= 64){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(res[index+cz].resource != NULL);
		if (res[index + cz].buffer != NULL){
			m_currentGfxc->setBuffers(stage, index + cz, 1, res[index + cz].buffer);
		}
		else{
			m_currentGfxc->setTextures(stage, index + cz, 1, res[index + cz].texture);
			m_currentGfxc->setSamplers(stage, index + cz, 1, res[index + cz].sampler);
		}

		usedBufferMaskHigh <<= (cz+1);
		index += cz+1;
	}

	index = 64;
	while(index < 128){
		uint8_t cz = __builtin_clzll(usedBufferMaskLow);
		if(index+cz >= 128){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(res[index+cz].resource != NULL);
		if (res[index + cz].buffer != NULL){
			m_currentGfxc->setBuffers(stage, index + cz, 1, res[index + cz].buffer);
		}
		else{
			m_currentGfxc->setTextures(stage, index + cz, 1, res[index + cz].texture);
			m_currentGfxc->setSamplers(stage, index + cz, 1, res[index + cz].sampler);
		}

		usedBufferMaskLow <<= (cz+1);
		index += cz+1;
	}

	/*
	for (int i = 0; i < sce::Gnm::kSlotCountResource; i++){
		if (res[i].resource != NULL){
			if (res[i].buffer != NULL){
				m_currentGfxc->setBuffers(stage, i, 1, res[i].buffer);
			}else{
				m_currentGfxc->setTextures(stage, i, 1, res[i].texture);
				m_currentGfxc->setSamplers(stage, i, 1, res[i].sampler);
			}
		}
	}
	*/

	// Vertex RwResource
	stage = getShaderStage(kProgramTypeVertex);
	ssgi::RwStructuredBufferImpl **rw = m_bufferState.m_vertexShaderRwResources;
	index = 0;
	usedBufferMask = m_bufferState.m_vertexShaderRwResourcesUsedMask;
	while(index < 32){
		uint8_t cz = __builtin_clz(usedBufferMask);
		if(index+cz >= 32){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(rw[index+cz] != NULL);
		m_currentGfxc->setRwBuffers(stage, index+cz, 1, rw[index+cz]->getGnmBuffer());

		usedBufferMask <<= (cz+1);
		index += cz+1;
	}

	/*
	for (int i = 0; i < sce::Gnm::kSlotCountRwResource; i++){
		if (rw[i] != NULL){
			m_currentGfxc->setRwBuffers(stage, i, 1, rw[i]->getGnmBuffer());
		}
	}
	*/
	// Fragment RwResource
	stage = getShaderStage(kProgramTypeFragment);
	rw = m_bufferState.m_fragmentShaderRwResources;
	index = 0;
	usedBufferMask = m_bufferState.m_fragmentShaderRwResourcesUsedMask;
	while(index < 32){
		uint8_t cz = __builtin_clz(usedBufferMask);
		if(index+cz >= 32){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(rw[index+cz] != NULL);
		m_currentGfxc->setRwBuffers(stage, index+cz, 1, rw[index+cz]->getGnmBuffer());

		usedBufferMask <<= (cz+1);
		index += cz+1;
	}

	/*
	for (int i = 0; i < sce::Gnm::kSlotCountRwResource; i++){
		if (rw[i] != NULL){
			m_currentGfxc->setRwBuffers(stage, i, 1, rw[i]->getGnmBuffer());
		}
	}
	*/
}


void ssgi::GraphicsContextImpl::setUpGraphicsShaders(void)
{
	const GnmProgram *vp = dynamic_cast<const ProgramImpl *>(m_vertexProgram->m_programId)->m_program;
	const GnmProgram *pp = dynamic_cast<const ProgramImpl *>(m_fragmentProgram->m_programId)->m_program;

	m_currentGfxc->setVsShader(vp->m_vsShader, vp->m_shaderModifier,  vp->m_fetchShader.m_ptr);
	m_currentGfxc->setPsShader(pp->m_psShader);

	// TODO: set buffers
	m_bufferState.setUpVertexBuffers(m_vertexProgram);

	setUpDrawBuffers();

	if(m_vertexDefaultUniformBuffer != NULL){
		sce::Gnm::Buffer constBuffer;

		constBuffer.initAsConstantBuffer( m_vertexDefaultUniformBuffer, vp->m_defaultUniformBufferSize );
		m_currentGfxc->setConstantBuffers(sce::Gnm::kShaderStageVs, 0, 1, &constBuffer);
	}
	if(m_fragmentDefaultUniformBuffer != NULL){
		sce::Gnm::Buffer constBuffer2;

		constBuffer2.initAsConstantBuffer( m_fragmentDefaultUniformBuffer, pp->m_defaultUniformBufferSize );
		m_currentGfxc->setConstantBuffers(sce::Gnm::kShaderStagePs, 0, 1, &constBuffer2);
	}

	m_currentGfxc->setRenderTargetMask(m_fragmentProgram->m_colorMask);
	sce::Gnm::BlendControl bc;
	bc.init();
	bc.setBlendEnable(true);
	bc.setColorEquation(m_fragmentProgram->colorSrc, m_fragmentProgram->colorComb, m_fragmentProgram->colorDest);
	bc.setAlphaEquation(m_fragmentProgram->alphaSrc, m_fragmentProgram->alphaComb, m_fragmentProgram->alphaDest);
	m_currentGfxc->setBlendControl(0, bc);
}


int ssgi::GraphicsContextImpl::draw(Primitive primitive, IndexBuffer *indexBuffer, uint32_t count)
{
	if(indexBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	setUpGraphicsShaders();

	sce::Gnm::PrimitiveType pt = convertPrimitive(primitive);
	m_currentGfxc->setPrimitiveType(pt);
	m_currentGfxc->setIndexSize(sce::Gnm::kIndexSize16);


	Impl::IndexBufferImpl* indexImpl = dynamic_cast<Impl::IndexBufferImpl*>(indexBuffer);

	ReferringResources &rr = m_frameBuffer->beginAddingReferringResources(m_nextRenderTargetIndex);
	rr.add(indexImpl);
	rr.add(m_vertexProgram);
	rr.add(m_fragmentProgram);
	m_bufferState.addToReferringResources(rr);
	m_frameBuffer->endAddingReferringResources();

	m_currentGfxc->drawIndex(count, (void*)indexImpl->getData()); // todo: should be cast to const void *

	return SCE_OK;
}


int ssgi::GraphicsContextImpl::drawIndexAuto(Primitive primitive, uint32_t count)
{
	setUpGraphicsShaders();

	ReferringResources &rr = m_frameBuffer->beginAddingReferringResources(m_nextRenderTargetIndex);
	rr.add(m_vertexProgram);
	rr.add(m_fragmentProgram);
	m_bufferState.addToReferringResources(rr);
	m_frameBuffer->endAddingReferringResources();

	sce::Gnm::PrimitiveType pt = convertPrimitive(primitive);
	m_currentGfxc->setPrimitiveType(pt);
	m_currentGfxc->drawIndexAuto(count);
	return SCE_OK;
}

ssg::RenderTarget* ssgi::GraphicsContextImpl::getNextRenderTarget(GraphicsContext::FrameBufferSide side)
{
	return m_frameBuffer->getRenderTarget(m_nextRenderTargetIndex, side);
}

ssg::DepthStencilSurface *ssgi::GraphicsContextImpl::getDepthStencilSurface(void)
{
	ssg::DepthStencilSurface *ret = m_frameBuffer->getDepthStencilSurface();
	ssgi::DepthStencilSurfaceImpl *depthStencilImpl = dynamic_cast<ssgi::DepthStencilSurfaceImpl *>(ret);
	(void)depthStencilImpl;
	SCE_SAMPLE_UTIL_ASSERT(depthStencilImpl != NULL);
	return ret;
}

int ssgi::GraphicsContextImpl::beginScene(RenderTarget* renderTarget, DepthStencilSurface *depthStencil)
{
	int ret = m_bufferState.setRenderTarget(renderTarget, depthStencil);
	if(ret != SCE_OK){
		return ret;
	}
	ssgi::RenderTargetImpl *renderTargetImpl = dynamic_cast<ssgi::RenderTargetImpl *>(renderTarget);
	m_currentGfxc->setupScreenViewport(0, 0, renderTargetImpl->m_renderTarget.getWidth(), renderTargetImpl->m_renderTarget.getHeight(), 0.5f, 0.5f);

	m_control.setViewPort(0, 0, renderTargetImpl->m_renderTarget.getWidth(), renderTargetImpl->m_renderTarget.getHeight(), 0.0f, 1.0f);
	m_control.setRegionClip(0, 0, renderTargetImpl->m_renderTarget.getWidth(), renderTargetImpl->m_renderTarget.getHeight());


#if 0
	// TODO: this setting should be moved to renderTarget.
	sce::Gnm::ScanModeControlAa              msaa            = sce::Gnm::kScanModeControlAaDisable;
	sce::Gnm::ScanModeControlViewportScissor viewportScissor = sce::Gnm::kScanModeControlViewportScissorDisable;
	sce::Gnm::ScanModeControlLineStipple     lineStipple     = sce::Gnm::kScanModeControlLineStippleDisable;
	sce::Gnm::NumSamples                     numSamples      = sce::Gnm::kNumSamples1;
	switch(m_graphicsOption.multisampleMode){
	case MULTISAMPLE_NONE:
		{
			msaa = sce::Gnm::kScanModeControlAaDisable;
			numSamples      = sce::Gnm::kNumSamples1;
			break;
		}
	case MULTISAMPLE_2X:
		{
			msaa = sce::Gnm::kScanModeControlAaEnable;
			numSamples      = sce::Gnm::kNumSamples2;
			break;
		}
	case MULTISAMPLE_4X:
		{
			msaa = sce::Gnm::kScanModeControlAaEnable;
			numSamples      = sce::Gnm::kNumSamples4;
			break;
		}
	}
	m_currentGfxc->setScanModeControl(msaa, viewportScissor, lineStipple);
	m_currentGfxc->setScanAaConfig(numSamples);
	m_currentGfxc->setAaDefaultSampleLocations(numSamples);
#endif
	return SCE_OK;
}


int ssgi::GraphicsContextImpl::endScene(void)
{
	return m_bufferState.setRenderTarget(NULL, NULL);
}



int ssgi::GraphicsContextImpl::setVertexProgram(VertexProgram *vertexProgram)
{
	VertexProgramImpl* vimpl = dynamic_cast<VertexProgramImpl*>(vertexProgram);
	if(m_vertexProgram != NULL){
		sce::SampleUtil::Impl::destroyImpl(m_vertexProgram);
	}
	if(vimpl != NULL){
		vimpl->refer();
	}
	m_vertexProgram = vimpl;
	return SCE_OK;
}
int ssgi::GraphicsContextImpl::setFragmentProgram(FragmentProgram *fragmentProgram)
{
	FragmentProgramImpl* fimpl = dynamic_cast<FragmentProgramImpl*>(fragmentProgram);
	if(m_fragmentProgram != NULL){
		sce::SampleUtil::Impl::destroyImpl(m_fragmentProgram);
	}
	if(fimpl != NULL){
		fimpl->refer();
	}
	m_fragmentProgram = fimpl;
	return SCE_OK;
}





sce::Gnmx::GfxContext* ssgi::GraphicsContextImpl::getGfxContext(void)
{
	return m_currentGfxc;
}


// TODO
void ssgi::GraphicsContextImpl::KickAndStall(void)
{
	uint64_t labelVal = 0;
	volatile uint64_t* label = static_cast<uint64_t*>(m_currentGfxc->embedData(&labelVal, sizeof(uint64_t)/sizeof(uint32_t), sce::Gnm::kEmbeddedDataAlignment8));

	m_currentGfxc->writeImmediateAtEndOfPipe(sce::Gnm::kEopFlushCbDbCaches, (void*)label, 1, sce::Gnm::kCacheActionNone);
	m_currentGfxc->submit();

	static volatile uint32_t dummy=0;
	while(*label != 1){
		dummy++;
	}

}




void ssgi::GraphicsContextImpl::waitEndOfComputeShader(void) 
{
	uint64_t labelVal = 0;
	volatile uint64_t* label = static_cast<uint64_t*>(m_currentGfxc->embedData(&labelVal, sizeof(uint64_t)/sizeof(uint32_t), sce::Gnm::kEmbeddedDataAlignment8));


	// Flush the L2$:
	m_currentGfxc->writeAtEndOfShader( sce::Gnm::kEosCsDone, (void*)label, 0x2 ); 

	m_currentGfxc->waitOnAddress((void*)label, 0xffffffff, sce::Gnm::kWaitCompareFuncEqual, 0x2);

	m_currentGfxc->flushShaderCachesAndWait(sce::Gnm::kCacheActionWriteBackAndInvalidateL1andL2, 0, sce::Gnm::kStallCommandBufferParserEnable);
	uint64_t one = 1;
	m_currentGfxc->writeDataInline((void*)label, &one, sizeof(uint64_t)/sizeof(uint32_t), sce::Gnm::kWriteDataConfirmEnable);

	m_currentGfxc->submit();

	static volatile uint32_t dummy=0;
	while(*label != 1){
		dummy++;
	}
}

int ssgi::GraphicsContextImpl::readGpuData(void *dst, void *src, uint64_t size)
{

	if(dst == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	waitEndOfComputeShader();
	char* p = (char*)dst;
	uint64_t remain = size;
	uint64_t offset = 0;
	while(remain > 0)
	{
		const uint32_t dmaBytes = (remain < m_copyBufferSize) ? remain : m_copyBufferSize;

		memset(m_copyBuffer, 0, dmaBytes);

		m_currentGfxc->copyData( m_copyBuffer, (void*)(((uintptr_t)src) + offset), dmaBytes, sce::Gnm::kDmaDataBlockingEnable);
		KickAndStall();

		memcpy(p, m_copyBuffer, dmaBytes);
		p += dmaBytes;
		remain -= dmaBytes;
		offset += dmaBytes;
	}
	return SCE_OK;

}

int ssgi::GraphicsContextImpl::readBuffer(void *dst, const ssg::BufferInterface* from, uint64_t offset, uint64_t numBytes)
{
	const ssg::Gnm::BufferGnm *buf = dynamic_cast<const ssg::Gnm::BufferGnm*>(from);
	if(buf == NULL){
		printf(__FILE__"(%d) Error\n", __LINE__);
		return -1;
	}

	return readGpuData(dst, (void *)(((uintptr_t)buf->getData()) + offset), numBytes);
}



int ssgi::GraphicsContextImpl::saveBackBufferAsBmp(const char* path)
{
	int ret;

	ssgi::RenderTargetImpl* renderTarget = dynamic_cast<ssgi::RenderTargetImpl*>(getNextRenderTarget());
	void* addr = renderTarget->m_renderTarget.getBaseAddress();
	uint64_t size = renderTarget->m_renderTarget.getSliceSizeInBytes() * 1; // asssuming num slice of frame buffer is always 1.

	char *img = (char*)malloc(size);
	SCE_SAMPLE_UTIL_ASSERT(img != NULL);

	ret = readGpuData(img, addr, size);
	if(ret != SCE_OK){
		free(img);
		return ret;
	}

	sce::GpuAddress::TilingParameters tileParam;
	tileParam.initFromRenderTarget(&renderTarget->m_renderTarget, 0);

	uint64_t surfaceSize;
	sce::Gnm::AlignmentType surfaceAlign;
	//sce::Gnm::SizeAlign untiledSizeAlign = sce::GpuAddress::computeUntiledSurfaceSize(&tileParam);
	ret = sce::GpuAddress::computeUntiledSurfaceSize(&surfaceSize, &surfaceAlign, &tileParam);
	if(ret < 0){
		free(img);
		return ret;
	}
	char *untiled = (char*)memalign(surfaceAlign, surfaceSize);

	sce::GpuAddress::detileSurface(untiled, img, &tileParam);

	ret = ssgi::saveAsBmp(path,
		renderTarget->m_renderTarget.getPitch(),
		renderTarget->m_renderTarget.getWidth(),
		renderTarget->m_renderTarget.getHeight(),
		(const char *)untiled);
	free(untiled);
	free(img);

	if(ret == SCE_OK){
		printf(" Screenshot taken. Location: %s, Size: (%d, %d)\n", path, renderTarget->getWidth(), renderTarget->getHeight());
	}
	return ret;

}


void ssgi::GraphicsContextImpl::setUpComputeBuffers(void)
{
	sce::Gnm::ShaderStage stage;

	stage = getShaderStage(kProgramTypeCompute);
	ResourceImpl **r = m_bufferState.m_computeUniformBuffers;
	uint32_t index = 0;
	uint32_t usedBufferMask = m_bufferState.m_computeUniformBuffersUsedMask;
	while(index < 32){
		uint8_t cz = __builtin_clz(usedBufferMask);
		if(index+cz >= 32){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(r[index+cz] != NULL);

		ssgi::UniformBufferImpl *impl = dynamic_cast<ssgi::UniformBufferImpl *>(r[index+cz]);
		sce::Gnm::Buffer constBuffer;
		constBuffer.initAsConstantBuffer((void*)impl->getData(), impl->getSize()); // todo
		m_currentGfxc->setConstantBuffers(stage, index+cz, 1, &constBuffer);

		usedBufferMask <<= (cz+1);
		index += cz+1;
	}
	/*
	for (int i = 0; i < sce::Gnm::kSlotCountConstantBuffer; i++){
		if (r[i] != NULL){
			ssgi::UniformBufferImpl *impl = dynamic_cast<ssgi::UniformBufferImpl *>(r[i]);
			sce::Gnm::Buffer constBuffer;
			constBuffer.initAsConstantBuffer((void*)impl->getData(), impl->getSize()); // todo
			m_currentGfxc->setConstantBuffers(stage, i, 1, &constBuffer);
		}
		else{
			m_currentGfxc->setConstantBuffers(stage, i, 1, NULL);
		} 

	}
	*/
	GpuBufferState::ShaderResource *res = m_bufferState.m_computeShaderResources;
	uint64_t usedBufferMaskHigh = (uint64_t )(m_bufferState.m_computeShaderResourcesUsedMask>>64);
	uint64_t usedBufferMaskLow = (uint64_t )(m_bufferState.m_computeShaderResourcesUsedMask);

	index = 0;
	while(index < 64){
		uint8_t cz = __builtin_clzll(usedBufferMaskHigh);
		if(index+cz >= 64){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(res[index+cz].resource != NULL);
		if (res[index + cz].buffer != NULL){
			m_currentGfxc->setBuffers(stage, index + cz, 1, res[index + cz].buffer);
		}
		else{
			m_currentGfxc->setTextures(stage, index + cz, 1, res[index + cz].texture);
			m_currentGfxc->setSamplers(stage, index + cz, 1, res[index + cz].sampler);
		}

		usedBufferMaskHigh <<= (cz+1);
		index += cz+1;
	}

	index = 64;
	while(index < 128){
		uint8_t cz = __builtin_clzll(usedBufferMaskLow);
		//printf("index=%d, usedBufferMask=%#x, cz=%d\n", index, usedBufferMask, cz);
		if(index+cz >= 128){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(res[index+cz].resource != NULL);
		if (res[index + cz].buffer != NULL){
			m_currentGfxc->setBuffers(stage, index + cz, 1, res[index + cz].buffer);
		}
		else{
			m_currentGfxc->setTextures(stage, index + cz, 1, res[index + cz].texture);
			m_currentGfxc->setSamplers(stage, index + cz, 1, res[index + cz].sampler);
		}

		usedBufferMaskLow <<= (cz+1);
		index += cz+1;
	}
	/*
	for (int i = 0; i < sce::Gnm::kSlotCountResource; i++){
		if (res[i].resource != NULL){
			if (res[i].buffer != NULL){
				m_currentGfxc->setBuffers(stage, i, 1, res[i].buffer);
			}else{
				m_currentGfxc->setTextures(stage, i, 1, res[i].texture);
				m_currentGfxc->setSamplers(stage, i, 1, res[i].sampler);
			}
		}else{
			m_currentGfxc->setBuffers(stage, i, 1, NULL);
			m_currentGfxc->setTextures(stage, i, 1, NULL);
			m_currentGfxc->setSamplers(stage, i, 1, NULL);
		}
	}
	*/
	// Compute RwResource
	ssgi::RwStructuredBufferImpl **rw = m_bufferState.m_computeShaderRwResources;
	index = 0;
	usedBufferMask = m_bufferState.m_computeShaderRwResourcesUsedMask;
	while(index < 32){
		uint8_t cz = __builtin_clz(usedBufferMask);
		if(index+cz >= 32){
			break;
		}
		SCE_SAMPLE_UTIL_ASSERT(rw[index+cz] != NULL);
		m_currentGfxc->setRwBuffers(stage, index+cz, 1, rw[index+cz]->getGnmBuffer());

		usedBufferMask <<= (cz+1);
		index += cz+1;
	}

	/*
	for (int i = 0; i < sce::Gnm::kSlotCountRwResource; i++){
		if (rw[i] != NULL){
			m_currentGfxc->setRwBuffers(stage, i, 1, rw[i]->getGnmBuffer());
		}else{
		}
	}
	*/
}

int ssgi::GraphicsContextImpl::dispatch(uint32_t threadGroupX,
										uint32_t threadGroupY,
										uint32_t threadGroupZ)
{
	m_currentGfxc->setCsShader(m_computeProgram->m_csShader);

	setUpComputeBuffers();

	m_currentGfxc->dispatch(threadGroupX, threadGroupY, threadGroupZ);

	

	return SCE_OK;
}

int ssgi::GraphicsContextImpl::setComputeProgram(ssg::ComputeProgram *computeProgram)
{
	ssgi::ComputeProgramImpl *_computeProgram = dynamic_cast<ssgi::ComputeProgramImpl *>(computeProgram);
	m_computeProgram = _computeProgram;
	return SCE_OK;
}

#pragma endregion // GraphicsContextImpl



// ---------------------------------------------------------------------------------------------------

int ssg::createGraphicsContext(ssg::GraphicsContext **outGraphicsContext, uint32_t width, uint32_t height,
				                                           ssg::GraphicsContextOption* option)
{
	if(outGraphicsContext == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssgi::GraphicsContextImpl *impl = new ssgi::GraphicsContextImpl();
	int ret = impl->initialize(width, height, option);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	SCE_SAMPLE_UTIL_ASSERT(impl != NULL);
	SCE_SAMPLE_UTIL_ASSERT(dynamic_cast<ssg::Gnm::GraphicsContextGnm*>(impl) != NULL);
	*outGraphicsContext = impl;
	return ret;

}

#endif //_SCE_TARGET_OS_ORBIS
