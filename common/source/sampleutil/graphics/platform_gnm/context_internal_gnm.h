/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_H

#include <sampleutil/graphics.h>
#include "program_internal_gnm.h"
#include "buffer_internal_gnm.h"
#include "loader_internal_gnm.h"
#ifdef SCE_GNM_INITIAL1
#include <display.h>
#else
#include <video_out.h>
#endif /* SCE_GNM_INITIAL1 */

#include <queue>


namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class ReferringResources
				{
				private:
					sce::SampleUtil::Impl::ResourceImpl** m_resources;
					uint32_t m_arraySize;
					uint32_t m_count;
				public:
					ReferringResources(void)
					{
						m_arraySize = 1024;
						m_resources = new sce::SampleUtil::Impl::ResourceImpl*[m_arraySize];
						m_count = 0;
					}
					~ReferringResources(void)
					{
						delete [] m_resources;
					}
				
					void add(sce::SampleUtil::Impl::ResourceImpl* impl){
						if(m_count >= m_arraySize){
							sce::SampleUtil::Impl::ResourceImpl** newArray = new sce::SampleUtil::Impl::ResourceImpl*[m_arraySize*2];
							for(uint32_t i=0; i<m_count; i++){
								newArray[i] = m_resources[i];
							}
							delete [] m_resources;
							m_arraySize *= 2;
							m_resources = newArray;
						}
						impl->refer();
						m_resources[m_count] = impl;
						m_count++;
					}
					void destroyAll(void)
					{
						for(uint32_t i=0; i<m_count; i++){
							sce::SampleUtil::Impl::destroyImpl(m_resources[i]);
						}
						m_count = 0;
					}
				};


				class FrameBuffer
				{
					friend class GraphicsContextImpl;
				public:
					typedef void (*OnSetFrameBufCallback)(uint32_t framebufIndex, void *arg);

					class DisplayBuffer
					{
					public:
						RenderTargetImpl m_renderTargetLeft;
						RenderTargetImpl m_renderTargetRight;
					};
					uint32_t m_numBuffers;
					DisplayBuffer* m_firstNormalDisplayBuffers;
					DisplayBuffer* m_secondNormalDisplayBuffers;
					DisplayBuffer* m_s3dDisplayBuffers;
					DisplayBuffer* m_vrDisplayBuffers;

					class VideoInfo
					{
					private:
						int32_t m_handle;
						SceKernelEqueue m_eq;
						uint8_t m_flipCount;
						bool m_enableVrView;
						bool m_enableS3dView;
						int32_t m_firstNormalBufferIndex;
						int32_t m_firstNormalBufferSet;
						int32_t m_secondNormalBufferIndex;
						int32_t m_secondNormalBufferSet;
						int32_t m_VrBufferIndexStart;
						int32_t m_S3dBufferIndexStart;
						GraphicsContext::DisplayMode m_displayMode;
						std::vector<int> m_pendingFlipIndices;
					public:
						uint32_t m_numBuffers;
						bool m_useFirstNormalBuffer;
						VideoInfo(void)
						{
							m_eq = NULL;
							m_handle = -1;

						}
						~VideoInfo(void)
						{
							finalize();
						}
						int initialize(uint32_t width, uint32_t height, uint32_t pitch, 
									   uint32_t numBuffers, 
									   void* const * normalBuffers,
									   const SceVideoOutStereoBuffers *buffers,
									    void* const *vrBuffers, bool isVrView, bool isStereoView );
						int finalize(void);

						void dump(void);
						void pendingCheck(uint32_t count);
						void waitUntilSafeForRenderingByCpu(uint32_t bufferIndex);

						void submitFlip(sce::Gnmx::GfxContext *m_gfxc, uint32_t bufferIdex);
						void submit(sce::Gnmx::GfxContext *m_gfxc);
						void waitUntilSafeForRendering(sce::Gnmx::GfxContext *m_gfxc, uint32_t bufferIdex);

						bool isDisplaymodeSupported(GraphicsContext::DisplayMode mode);
						int setDisplaymode(GraphicsContext::DisplayMode m);
						bool isVrDisplaymode() const
						{
							if((m_displayMode == GraphicsContext::kDisplayModeVrView90Hz) || (m_displayMode == GraphicsContext::kDisplayModeVrView120Hz)){
								return true;
							}
							
							return false;
						}
						bool isS3dDisplaymode() const
						{
							if(m_displayMode == GraphicsContext::kDisplayModeS3dView){
								return true;
							}
							
							return false;
						}

						int32_t getVideoOutHandle(void) const
						{
							return m_handle;
						}
						
						bool isVrViewEnabled() const
						{
							return m_enableVrView;
						}
						
						bool isS3dViewEnabled() const
						{
							return m_enableS3dView;
						}
						int unregisterBackFrameBuffers(void);
						int registerBackFrameBuffers(uint32_t width, uint32_t height, uint32_t pitch, void* const* buffer);
						int switchNormalBuffer(void)
						{
							m_useFirstNormalBuffer = !m_useFirstNormalBuffer;
							return 0;
						}
					};
					VideoInfo m_videoInfo;

					GraphicsLoaderImpl *m_loader;

					uint32_t m_width;
					uint32_t m_height;
					bool m_useStencil;
					bool m_useHtile;
					sce::Gnm::TileMode m_tileMode;
					bool m_enableCmask;
					bool m_enableFmask;
					int32_t m_lastFlipIndex;
					uint32_t m_pendingResizeFrameBufferCountDown;
					bool m_hasPendingResize;
					uint32_t m_pendingWidth;
					uint32_t m_pendingHeight;
					
					DepthStencilSurfaceImpl m_depthTarget;
					MultisampleMode m_multiSample;
					RenderTargetImpl m_middleRenderTarget;
					class OnSetFrameBufCallbackJob
					{
					public:
						OnSetFrameBufCallbackJob(OnSetFrameBufCallback callback, void *arg)
						{
							m_callbackFunction = callback;
							m_callbackArg = arg;
						}
						bool equals(OnSetFrameBufCallback callback, void *arg)
						{
							return (m_callbackFunction==callback) && (m_callbackArg==arg);
						}

						OnSetFrameBufCallback m_callbackFunction;
						void *m_callbackArg;
					};

					std::vector<OnSetFrameBufCallbackJob> m_onSetFrameBufCallback;


					ReferringResources *m_referringResources;
				public:
					FrameBuffer(void)
					{
						m_firstNormalDisplayBuffers = NULL;
						m_secondNormalDisplayBuffers = NULL;
						m_s3dDisplayBuffers = NULL;
						m_vrDisplayBuffers = NULL;
						m_lastFlipIndex = -1;
						m_pendingResizeFrameBufferCountDown = 0;
						m_hasPendingResize = false;
					}

					void submitFlip(sce::Gnmx::GfxContext *m_gfxc, uint32_t bufferIdex)
					{
						m_videoInfo.submitFlip(m_gfxc, bufferIdex);
						m_lastFlipIndex = bufferIdex;
					}
					void submit(sce::Gnmx::GfxContext *m_gfxc)
					{
						m_videoInfo.submit(m_gfxc);
					}

					int32_t getVideoOutHandle(void) const
					{
						return m_videoInfo.getVideoOutHandle();
					}


					void waitUntilSafeForRendering(sce::Gnmx::GfxContext *m_gfxc, uint32_t bufferIdex)
					{
						m_videoInfo.waitUntilSafeForRendering(m_gfxc, bufferIdex);
					}
					void checkPendingResizeFrameBuffer(void);
					void freeReferringResources(uint32_t bufferIndex)
					{
						for(uint32_t i=0; i<m_onSetFrameBufCallback.size(); i++){
							OnSetFrameBufCallbackJob job = m_onSetFrameBufCallback.at(i);
							job.m_callbackFunction(bufferIndex, job.m_callbackArg);
						}
						m_referringResources[bufferIndex].destroyAll();
						if (m_pendingResizeFrameBufferCountDown > 0){
							m_pendingResizeFrameBufferCountDown--;
							if (m_pendingResizeFrameBufferCountDown == 0) {
								checkPendingResizeFrameBuffer();
							}
						}


					}

					RenderTarget* getRenderTarget(uint32_t bufferIndex, GraphicsContext::FrameBufferSide side)
					{
						if( m_videoInfo.isVrDisplaymode() ){
							return &m_vrDisplayBuffers[bufferIndex].m_renderTargetLeft;
						}
						if (m_videoInfo.isS3dDisplaymode()) {
							if (side == GraphicsContext::kFrameBufferSideLeft){
								return &m_s3dDisplayBuffers[bufferIndex].m_renderTargetLeft;
							}else{
								return &m_s3dDisplayBuffers[bufferIndex].m_renderTargetRight;
							}
						}
						if(side == GraphicsContext::kFrameBufferSideLeft){
							if(m_videoInfo.m_useFirstNormalBuffer){
								return &m_firstNormalDisplayBuffers[bufferIndex].m_renderTargetLeft;
							}else{
								return &m_secondNormalDisplayBuffers[bufferIndex].m_renderTargetLeft;
							}
						}else{
							return nullptr;
						}
					}


					bool isDisplaymodeSupported(GraphicsContext::DisplayMode mode);
					int setDisplaymode(GraphicsContext::DisplayMode mode);

					
					int initialize(Impl::GraphicsLoaderImpl *loader, uint32_t width, uint32_t height, uint32_t numBuffers, 
								   bool useStencil, bool useHtile, MultisampleMode multiSample,
								   bool isVrView, bool isStereoView);
					int finalize(void);
					virtual ~FrameBuffer(void)
					{
						finalize();
					}
					DepthStencilSurface *getDepthStencilSurface(void)
					{
						DepthStencilSurface *ret = &m_depthTarget;
						SCE_SAMPLE_UTIL_ASSERT(dynamic_cast<Impl::DepthStencilSurfaceImpl*>(ret) != NULL);
						return ret;
					}
					void addOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg);
					void removeOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg);


					ReferringResources &beginAddingReferringResources(uint32_t bufferIndex)
					{
						return m_referringResources[bufferIndex];
					}

					void endAddingReferringResources(void)
					{
						//printf("rr[%d] size=%d\n", m_displayBackBufferIndex, m_referringResources[m_displayBackBufferIndex].size());
					}


					void dump(void)
					{
						m_videoInfo.dump();
					}
					void pendingCheck(uint32_t count)
					{
						m_videoInfo.pendingCheck(count);
					}
					void waitUntilSafeForRenderingByCpu(uint32_t bufferIndex)
					{
						m_videoInfo.waitUntilSafeForRenderingByCpu(bufferIndex);
					}

					int resizeFrameBuffer(uint32_t width, uint32_t height);
				};

				/* ------------------------------------------------------------------------------------------------------------------ */

				class GpuControl
				{
				private:
					sce::Gnmx::GfxContext *m_gfxc;

					sce::Gnm::DepthStencilControl m_depthStencilControl;

					sce::Gnm::PrimitiveSetup m_primitiveSetup;
					sce::Gnm::StencilOpControl m_stencilOpControl;

					// depth
					sce::Gnm::DepthControlZWrite m_zWrite;
					sce::Gnm::CompareFunc        m_zFunc;

					// cull 
					sce::Gnm::PrimitiveSetupCullFaceMode m_cullFaceMode;

					// ScanModeControl setting
					sce::Gnm::ScanModeControlAa				m_scanModeControlAa;	//msaa
					sce::Gnm::ScanModeControlViewportScissor	m_scanModeControlViewportScissor;

					// stencil
					uint8_t m_testVal;
					uint8_t m_mask;
					uint8_t m_writeMask;
					uint8_t m_opVal;
					sce::Gnm::CompareFunc m_stencilFunc;
					sce::Gnm::CompareFunc m_stencilFuncBack;
					sce::Gnm::StencilOp m_stencilFail;
					sce::Gnm::StencilOp m_stencilZPass;
					sce::Gnm::StencilOp m_stencilZfail;
					sce::Gnm::StencilOp m_stencilFailBack;
					sce::Gnm::StencilOp m_stencilZPassBack;
					sce::Gnm::StencilOp m_stencilZfailBack;

					uint32_t m_lineWidth;
					uint16_t m_pointSizeHalfWidth;
					uint16_t m_pointSizeHalfHeight;

					struct ViewPort
					{
						float minDepth;
						float maxDepth;
						float viewportScale[3];
						float viewportOffset[3];
					} m_viewPort;

					struct RegionClip
					{
						uint32_t topLeftX;	
						uint32_t topLeftY;
						uint32_t width;
						uint32_t height;
					} m_regionClip;
				public:
					int initialize(void);
					void setContext(sce::Gnmx::GfxContext *gfxc);

					void setDepthWriteEnable(bool enable);
					void setDepthFunc(DepthFunc depthFunc);

					void setStencilFunc(uint8_t compareMask,
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
										);

					void setFillMode(bool isFill);
					void setCullMode(CullMode cullMode);
					void setLineWidth(uint32_t width);
					void setPointSize(uint16_t halfWidth, uint16_t halfHeight);
					void setViewPort(uint32_t topLeftX,  uint32_t topLeftY,	 uint32_t width, uint32_t height,	 float minDepth, float maxDepth);
					void setRegionClip(uint32_t topLeftX,   uint32_t topLeftY,   uint32_t width,	   uint32_t height);

				};

				/* ------------------------------------------------------------------------------------------------------------------ */

				class GpuBufferState
				{
				public:
					sce::Gnmx::GfxContext *m_gfxc;
					RenderTargetImpl        *m_currentRenderTarget;
					DepthStencilSurfaceImpl *m_currentDepthStencilSurface;

					sce::SampleUtil::Impl::ResourceImpl* m_vertexBuffers[sce::Gnm::kSlotCountVertexBuffer];
					uint32_t m_vertexBuffersUsedMask;

					struct ShaderResource
					{
						sce::SampleUtil::Impl::ResourceImpl *resource;
						sce::Gnm::Texture* texture;
						sce::Gnm::Sampler* sampler;
						const sce::Gnm::Buffer *buffer;
						void clear(void)
						{
							resource = NULL;
							texture = NULL;
							sampler = NULL;
							buffer = NULL;
						}
					};
					ShaderResource m_vertexShaderResources[sce::Gnm::kSlotCountResource];
					unsigned __int128 m_vertexShaderResourcesUsedMask;
					ShaderResource m_fragmentShaderResources[sce::Gnm::kSlotCountResource];
					unsigned __int128 m_fragmentShaderResourcesUsedMask;
					ShaderResource m_computeShaderResources[sce::Gnm::kSlotCountResource];
					unsigned __int128 m_computeShaderResourcesUsedMask;

					sce::SampleUtil::Graphics::Impl::RwStructuredBufferImpl* m_vertexShaderRwResources[sce::Gnm::kSlotCountRwResource];
					uint32_t m_vertexShaderRwResourcesUsedMask;
					sce::SampleUtil::Graphics::Impl::RwStructuredBufferImpl* m_fragmentShaderRwResources[sce::Gnm::kSlotCountRwResource];
					uint32_t m_fragmentShaderRwResourcesUsedMask;
					sce::SampleUtil::Graphics::Impl::RwStructuredBufferImpl* m_computeShaderRwResources[sce::Gnm::kSlotCountRwResource];
					uint32_t m_computeShaderRwResourcesUsedMask;
					
					sce::SampleUtil::Impl::ResourceImpl *m_vertexUniformBuffers[sce::Gnm::kSlotCountConstantBuffer];
					uint32_t m_vertexUniformBuffersUsedMask;
					sce::SampleUtil::Impl::ResourceImpl *m_fragmentUniformBuffers[sce::Gnm::kSlotCountConstantBuffer];
					uint32_t m_fragmentUniformBuffersUsedMask;
					sce::SampleUtil::Impl::ResourceImpl *m_computeUniformBuffers[sce::Gnm::kSlotCountConstantBuffer];
					uint32_t m_computeUniformBuffersUsedMask;
				public:
					int initialize(void);
					void setContext(sce::Gnmx::GfxContext *gfxc);
					int finalize(void);
					
					int setVertexBuffer(VertexBuffer *_vertexBuffer, uint32_t streamIndex);					

					int setRenderTarget(RenderTarget* renderTarget, DepthStencilSurface *depthStencil);
					int setTexture(ProgramType programType, Texture *texture, uint32_t index);
					int setUniformBuffer(ProgramType programType, UniformBuffer *buffer, uint32_t bufferIndex);
					int setResource(ProgramType programType, Texture *texture, uint32_t index);
					int setResource(ProgramType programType, StructuredBuffer *buffer, uint32_t index);
					int setRwResource(ProgramType programType, RwStructuredBuffer *buffer, uint32_t index);

					RenderTarget *getCurrentRenderTarget(void) const;
					DepthStencilSurface *getCurrentDepthStencilSurface(void) const;
					void setUpVertexBuffers(const VertexProgramImpl *vertexProgram);
					void addToReferringResources(ReferringResources &rr);
				};

				/* ------------------------------------------------------------------------------------------------------------------ */

				class GraphicsContextImpl : public Gnm::GraphicsContextGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					GraphicsLoaderImpl m_loader;
					uint32_t m_nextRenderTargetIndex;
				public:
					GraphicsContextImpl();
					virtual ~GraphicsContextImpl();
					int initialize(uint32_t width, uint32_t height, 
								   GraphicsContextOption* option=NULL );
					int finalize(void);

					sce::Gnmx::GfxContext* getGfxContext(void);


					int beginScene(RenderTarget* renderTarget, DepthStencilSurface *depthStencil);
					int endScene(void);
					int flip( uint32_t numVsync);
					int submitOnly(void);
					int prepareNextFrame(void);

					int setVertexProgram(VertexProgram *vertexProgram);
					int setFragmentProgram(FragmentProgram *fragmentProgram);
					int setComputeProgram(ComputeProgram *computeProgram);

					int draw(Primitive primitive, IndexBuffer *indexBuffer, uint32_t count);
					int drawIndexAuto(Primitive primitive, uint32_t count);
					int dispatch(uint32_t threadGroupX,  uint32_t threadGroupY,	 uint32_t threadGroupZ);

					void *reserveDefaultUniformBuffer(ProgramType programType);
					void waitEndOfComputeShader(void);
					int saveBackBufferAsBmp(const char* path);

					void clearRenderTarget(uint32_t color);
					int gpuMemcopy(uint64_t dst, void* src, uint64_t size);
					int readGpuData(void *dst, void* src, uint64_t size);
					int readBuffer(void *dst, const BufferInterface* from, uint64_t offset, uint64_t numBytes);

					// ------------------------------------------------------------------------------------------------------
					// FrameBuffer
					// ------------------------------------------------------------------------------------------------------
					void addOnSetFrameBufCallback(FrameBuffer::OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg)
					{
						m_frameBuffer->addOnSetFrameBufCallback(onSetFrameBufCallback, onSetFrameBufCallbackArg);
					}
					void removeOnSetFrameBufCallback(FrameBuffer::OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg)
					{
						m_frameBuffer->removeOnSetFrameBufCallback(onSetFrameBufCallback, onSetFrameBufCallbackArg);
					}
					RenderTarget* getNextRenderTarget(GraphicsContext::FrameBufferSide size=GraphicsContext::kFrameBufferSideLeft);
					uint32_t getNextRenderTargetIndex(void) const
					{
						return m_nextRenderTargetIndex;
					}

					DepthStencilSurface *getDepthStencilSurface(void);

					bool isDisplaymodeSupported(DisplayMode mode)
					{
						return m_frameBuffer->isDisplaymodeSupported(mode);
					}
					int setDisplaymode(DisplayMode mode)
					{
						return m_frameBuffer->setDisplaymode(mode);
					}

					int32_t getVideoOutHandle(void) const
					{
						return m_frameBuffer->getVideoOutHandle();
					}
					int resizeFrameBuffer(uint32_t width, uint32_t height)
					{
						return m_frameBuffer->resizeFrameBuffer(width, height);
					}

					// ------------------------------------------------------------------------------------------------------
					// Buffers
					// ------------------------------------------------------------------------------------------------------
					int setResource(ProgramType programType, Texture *texture, uint32_t index)                 { return m_bufferState.setResource(programType, texture, index); } 
					int setResource(ProgramType programType, StructuredBuffer *buffer, uint32_t index)        { return m_bufferState.setResource(programType, buffer, index); }
					int setRwResource(ProgramType programType, RwStructuredBuffer *buffer, uint32_t index)    { return m_bufferState.setRwResource(programType, buffer, index); }
					int setUniformBuffer(ProgramType programType, UniformBuffer *buffer, uint32_t bufferIndex) { return m_bufferState.setUniformBuffer(programType, buffer, bufferIndex); }
					int setTexture(ProgramType programType, Texture *texture, uint32_t index)                  { return m_bufferState.setTexture(programType, texture, index); }
					int setVertexBuffer(VertexBuffer *vertexBuffer, uint32_t streamIndex)                      { return m_bufferState.setVertexBuffer(vertexBuffer, streamIndex); }
					RenderTarget *getCurrentRenderTarget(void) const                                                 { return m_bufferState.getCurrentRenderTarget();	}		
					DepthStencilSurface *getCurrentDepthStencilSurface(void) const                                   { return m_bufferState.getCurrentDepthStencilSurface();	}

					// ------------------------------------------------------------------------------------------------------
					// Control
					// ------------------------------------------------------------------------------------------------------
					void setDepthFunc(DepthFunc depthFunc){ m_control.setDepthFunc(depthFunc);     }
					void setDepthWriteEnable(bool enable) { m_control.setDepthWriteEnable(enable); }
					void setFillMode(bool isFill        ) { m_control.setFillMode(isFill);         }
					void setCullMode(CullMode cullMode)   { m_control.setCullMode(cullMode);       }
					void setStencilFunc(uint8_t compareMask, uint8_t writeMask,
										StencilFunc frontFaceFunc,	StencilOp   frontFaceStencilFail,	StencilOp   frontFaceDepthFail,	StencilOp   frontFaceDepthPass,
										StencilFunc backFaceFunc,	StencilOp   backFaceStencilFail,	StencilOp   backFaceDepthFail,	StencilOp   backFaceDepthPass,
										uint8_t stencilRef)	{
						m_control.setStencilFunc(compareMask, writeMask,
												 frontFaceFunc,	frontFaceStencilFail, frontFaceDepthFail, frontFaceDepthPass,
												 backFaceFunc,	backFaceStencilFail,   backFaceDepthFail, backFaceDepthPass,
												 stencilRef );
					}
					void setLineWidth(uint32_t width)    { m_control.setLineWidth(width); }
					void setPointSize(uint16_t halfWidth, uint16_t halfHeight){ m_control.setPointSize(halfWidth, halfHeight); }
					void setViewPort(uint32_t topLeftX,  uint32_t topLeftY,	 uint32_t width, uint32_t height,	 float minDepth, float maxDepth)
					{ m_control.setViewPort(topLeftX,  topLeftY, width, height, minDepth, maxDepth); }
					void setRegionClip(uint32_t topLeftX,   uint32_t topLeftY,   uint32_t width,	   uint32_t height)
					{ m_control.setRegionClip(topLeftX, topLeftY, width, height); }


					// ------------------------------------------------------------------------------------------------------
					// Loader methods
					// ------------------------------------------------------------------------------------------------------
#pragma region laoder_wrapper
					int registerVertexProgram(Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat=kShaderFormatDefault)	{
						return m_loader.registerVertexProgram(outProgram, binary, programSize, shaderFormat);
					}
					int registerFragmentProgram(Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat=kShaderFormatDefault){
						return m_loader.registerFragmentProgram(outProgram, binary, programSize, shaderFormat);
					}
					int registerComputeProgram(Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat=kShaderFormatDefault){
						return m_loader.registerComputeProgram(outProgram, binary, programSize, shaderFormat);
					}
					const Program* registerProgramFromMemory(const void *binary, uint32_t binarySize){
						return m_loader.registerProgramFromMemory(binary, binarySize);
					}
					int unregisterProgram(Program* programId){
						return m_loader.unregisterProgram(programId);
					}
					int createVertexProgram(VertexProgram **vertexProgram, const Program* programId,
											const VertexAttribute *attrs, uint32_t numAttrs,
											const VertexStream *streams, uint32_t numStreams)	{
						return m_loader.createVertexProgram(vertexProgram, programId, attrs, numAttrs,  streams, numStreams);
					}
					int releaseVertexProgram(VertexProgram *vertexProgram){
						return m_loader.releaseVertexProgram(vertexProgram);
					}
					int createFragmentProgram(FragmentProgram **fragmentProgram, const Program *programId, const Program* vertexProgramId, const BlendInfo *blendInfo){
						return m_loader.createFragmentProgram(fragmentProgram, programId, vertexProgramId, blendInfo);
					}
					int releaseFragmentProgram(FragmentProgram *fragmentProgram)	{
						return m_loader.releaseFragmentProgram(fragmentProgram);
					}
					int createComputeProgram(ComputeProgram **computeProgram, const Program* programId){
						return m_loader.createComputeProgram(computeProgram, programId);
					}
					int cloneVertexProgram(VertexProgram **vertexProgram, const VertexProgram *from)	{
						return m_loader.cloneVertexProgram(vertexProgram, from);
					}
					int cloneFragmentProgram(FragmentProgram **fragmentProgram, const FragmentProgram *from)	{
						return m_loader.cloneFragmentProgram(fragmentProgram, from);
					}
					Heap* getHeap(void)	{
						return m_loader.getHeap();
					}
					int createTexture(Texture **outTexture,  TextureFormat texFormat,  uint32_t width,  uint32_t height,  uint32_t mipCount){
						return m_loader.createTexture(outTexture, texFormat, width,  height, mipCount);
					}
					int createTextureFromFile(Texture **outTexture, const char *filename)	{
						return m_loader.createTextureFromFile(outTexture, filename);
					}
					int createTextureFromMemory(Texture **outTexture, const void *data, uint32_t size)	{
						return m_loader.createTextureFromMemory(outTexture, data, size);
					}
					int createIndexBuffer(IndexBuffer **outIndexBuffer, uint32_t size)	{
						return m_loader.createIndexBuffer(outIndexBuffer, size);
					}
					int createVertexBuffer(VertexBuffer **outVertexBuffer, uint32_t size){
						return m_loader.createVertexBuffer(outVertexBuffer, size);
					}
					int createUniformBuffer(UniformBuffer **outVertexBuffer, uint32_t size)	{
						return m_loader.createUniformBuffer(outVertexBuffer, size);
					}
					int createStructuredBuffer(StructuredBuffer **outStructuredBuffer, uint32_t stride, uint32_t numElements){
						return m_loader.createStructuredBuffer(outStructuredBuffer, stride, numElements);
					}
					int createRwStructuredBuffer(RwStructuredBuffer * *outStructuredBuffer, uint32_t stride, uint32_t numElements){
						return m_loader.createRwStructuredBuffer(outStructuredBuffer, stride, numElements);
					}
					int createBuffer(Buffer **outDataBuffer, size_t size, BufferAccessMode accessMode, uint32_t bufferBindFlags, size_t elementSize){
						return m_loader.createBuffer(outDataBuffer, size, accessMode, bufferBindFlags, elementSize);
					}
					int createTexture2dBuffer(Texture2dBuffer **outTexture2dBuffer,  BufferFormat format,  uint32_t width,	  uint32_t height,
											  uint32_t mipCount,  BufferAccessMode accessMode,  uint32_t bufferBindFlags,   MultisampleMode multiSampleMode ){
						return m_loader.createTexture2dBuffer(outTexture2dBuffer,  format, width, height,
															  mipCount,  accessMode, bufferBindFlags,   multiSampleMode );
					}

					int createTexture2dBufferUsingExistentBuffer(Texture2dBuffer **outTexture2dBuffer,
						BufferFormat format, uint32_t width, uint32_t height,
						uint32_t mipCount,	BufferAccessMode accessMode,  uint32_t bufferBindFlags, 
						MultisampleMode multiSampleMode,	void *buffer)
					{
						return m_loader.createTexture2dBufferUsingExistentBuffer(outTexture2dBuffer,  format, width, height,
															  mipCount,  accessMode, bufferBindFlags,   multiSampleMode, buffer );
					}
					int createTextureFromBuffer(Texture **outTexture, Texture2dBuffer *buffer, TextureFormat texFormat)
					{
						return m_loader.createTextureFromBuffer(outTexture, buffer, texFormat);
					}


					void* memalignSystemSharedMemory(size_t boundary, size_t size)	{
						return m_loader.memalignSystemSharedMemory(boundary, size);
					}
					void  freeSystemSharedMemory(void* ptr)	{
						m_loader.freeSystemSharedMemory(ptr);
					}
					void* memalignVideoSharedMemory(size_t boundary, size_t size)	{
						return m_loader.memalignVideoSharedMemory(boundary, size);
					}
					void  freeVideoSharedMemory(void* ptr)	{
						m_loader.freeVideoSharedMemory(ptr);
					}
					int createRenderTarget(RenderTarget **outRenderTarget,  uint32_t width, uint32_t height){
						return m_loader.createRenderTarget(outRenderTarget, width, height);
					}
					int createDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface,  uint32_t width, uint32_t height){
						return m_loader.createDepthStencilSurface(outDepthStencilSurface, width, height);
					}
#pragma endregion
					std::string getResourceType(void) const
					{
						return "Graphics::GraphicsContext";
					}

					void waitVBlank(void)
					{
						sceVideoOutWaitVblank(m_frameBuffer->getVideoOutHandle());
					}
				private:
					GraphicsContextOption m_graphicsOption;

					GpuControl m_control;
					GpuBufferState m_bufferState;


					void *m_vertexDefaultUniformBuffer;
					void *m_fragmentDefaultUniformBuffer;
					void *m_computeDefaultUniformBuffer;

					uint32_t          m_frameCount;

					// Buffers for gfxc
					struct CommandContext
					{
						void *m_cueHeapBuffer;
						void *m_dcbBuffer;
						void *m_ccbBuffer;
						sce::Gnmx::GfxContext m_gfxc;
					};
					CommandContext *m_commandContexts;
					sce::Gnmx::GfxContext *m_currentGfxc;
					
					Impl::FrameBuffer* m_frameBuffer;

					VertexProgramImpl *m_vertexProgram;
					FragmentProgramImpl *m_fragmentProgram;
					ComputeProgramImpl *m_computeProgram;
					

					class UtilShader
					{
					public:
						Program* m_vertexProgramId;
						Program* m_pixelProgramId;
						VertexProgram *m_vertexProgram;
						FragmentProgram *m_fragmentProgram;
						GraphicsLoaderImpl *m_loader;
						int initialize(GraphicsLoaderImpl *loader);
						int finalize(void);
					};

					
					UtilShader m_clearShader;

					void *m_copyBuffer;
					size_t m_copyBufferSize;


					void KickAndStall(void);

					volatile uint64_t *m_labels;
					uint32_t *m_expectedLabels;
					void notifyDrawFinish(uint32_t bufferIndex);
					void waitDrawFinish(uint32_t bufferIndex);

					void setUpGraphicsShaders(void);
					void setUpDrawBuffers(void);
					void setUpComputeBuffers(void);

				};

			}				
		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_H */

