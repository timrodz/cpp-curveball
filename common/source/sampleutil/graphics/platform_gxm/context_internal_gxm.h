/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_GXM_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_GXM_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2


#include <sampleutil_internal.h>
#include "loader_internal_gxm.h"
#include "buffer_internal_gxm.h"

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
				
				class FrameBufferImpl 
				{
				public:

					FrameBufferImpl(void);
					virtual ~FrameBufferImpl(void){}

					int initialize(GraphicsLoaderImpl *loader, 
								   uint32_t width, uint32_t height, 
								   MultisampleMode multiSampleMode,
								   uint32_t numBuffer);

					virtual int finalize(void);

					RenderTarget *getNextRenderTarget(void);
					uint32_t getNextRenderTargetIndex(void) const;
					DepthStencilSurface *getDepthStencilSurface(void);
					int flip(uint32_t numVSync);


					typedef void (*OnSetFrameBufCallback)(const SceDisplayFrameBuf *framebuf, uint32_t framebufIndex, void *arg);
					static void displayCallback( const void *callbackData );
					void addOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg);
					void removeOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg);

					ReferringResources &beginAddingReferringResources(void)
					{
						return m_referringResources[m_displayBackBufferIndex];
					}

					void endAddingReferringResources(void)
					{
						//printf("rr[%d] size=%d\n", m_displayBackBufferIndex, m_referringResources[m_displayBackBufferIndex].size());
					}
				private:


					struct Resolutions
					{
						uint32_t m_numBufferPerResolution;
						SceGxmSyncObject** m_syncObjects;

						// [0]960x544 [1]720x408 [2]640x368 [3]480x272
						RenderTargetImpl *m_mainRenderTargets[4];
						DepthStencilSurfaceImpl *m_depthStencilSurface[4];
						Resolutions(void);
						int initialize(GraphicsLoaderImpl *loader, 
							uint32_t numBufferPerResolution,
							MultisampleMode multiSampleMode);
						int finalize(void);
						void setNextResolution(uint32_t width, uint32_t height)
						{
						}

						RenderTarget *getNextRenderTarget(int index)
						{
							return &m_mainRenderTargets[0][index];
						}
						DepthStencilSurface *getDepthStencilSurface(void)
						{
							return m_depthStencilSurface[0];
						}
					};

					Resolutions m_resolutions;


					ReferringResources *m_referringResources;

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

					uint32_t m_displayFrontBufferIndex;
					uint32_t m_displayBackBufferIndex;
					void waitVblankStart(void *backBuffer, uint32_t frameBufferIndex, uint32_t numVSync, RenderTargetImpl *renderTarget);
				};

				class GpuControl
				{
				private:
					SceGxmContext *m_context;

					class Region
					{
					public:
						bool m_enable;
						uint32_t m_topLeftX;
						uint32_t m_topLeftY;
						uint32_t m_bottomRightX;
						uint32_t m_bottomRightY;

						Region(){
							m_enable = false;
							m_topLeftX = 0;
							m_topLeftY = 0;
							m_bottomRightX = 0;
							m_bottomRightY = 0;
						}

						void setAndAlign(
							uint32_t topLeftX, 
							uint32_t topLeftY, 
							uint32_t width, 
							uint32_t height);
					};
					Region m_viewPortRegion;
					Region m_clipRegion;

				public:
					bool m_isFill;

				public:
					int initialize(SceGxmContext *context);
					void setDepthFunc(DepthFunc depthFunc);
					void setDepthWriteEnable(bool enable);
					void setFillMode(bool isFill);
					void setCullMode(CullMode cullMode);
					void setViewPort(uint32_t topLeftX,
									 uint32_t topLeftY,
									 uint32_t width,
									 uint32_t height,
									 float minDepth,
									 float maxDepth);

					void setRegionClip(uint32_t topLeftX,
									   uint32_t topLeftY,
									   uint32_t width,
									   uint32_t height);

					void setStencilFunc(uint8_t compareMask, uint8_t writeMask,	
						StencilFunc frontFaceFunc,	StencilOp   frontFaceStencilFail,StencilOp   frontFaceDepthFail,StencilOp   frontFaceDepthPass,
						StencilFunc backFaceFunc,	StencilOp   backFaceStencilFail, StencilOp   backFaceDepthFail,	StencilOp   backFaceDepthPass,
						uint8_t stencilRef);
				private:
					void setClip();
				};


				class GpuBufferState
				{
				private:
					SceGxmContext *m_context;
					RenderTargetImpl *m_currentRenderTarget;
					DepthStencilSurfaceImpl *m_currentDepthStencilSurface;

					uint32_t m_usedVertexBufferMask;
					sce::SampleUtil::Impl::ResourceImpl* m_vertexBuffers[SCE_GXM_MAX_VERTEX_STREAMS];
					
					uint32_t m_usedVertexUniformBufferMask;
					sce::SampleUtil::Impl::ResourceImpl* m_vertexUniformBuffers[SCE_GXM_MAX_UNIFORM_BUFFERS];
					uint32_t m_usedFragmentUniformBufferMask;
					sce::SampleUtil::Impl::ResourceImpl* m_fragmentUniformBuffers[SCE_GXM_MAX_UNIFORM_BUFFERS];
					uint32_t m_usedVertexTextureMask;
					sce::SampleUtil::Impl::ResourceImpl* m_vertexTextures[SCE_GXM_MAX_TEXTURE_UNITS];
					uint32_t m_usedFragmentTextureMask;
					sce::SampleUtil::Impl::ResourceImpl* m_fragmentTextures[SCE_GXM_MAX_TEXTURE_UNITS];
				public:
					GpuBufferState(void);
					int initialize(SceGxmContext *context);
					int finalize(void);
					int setVertexBuffer(VertexBuffer *_vertexBuffer, uint32_t streamIndex);
					int setUniformBuffer(ProgramType programType, UniformBuffer *buffer, uint32_t bufferIndex);
					int setTexture(ProgramType programType, Texture *texture, uint32_t index);
					int setRenderTarget(RenderTarget* renderTarget, DepthStencilSurface *depthStencil);

					RenderTarget *getCurrentRenderTarget(void) const
					{
						return m_currentRenderTarget;
					}		

					DepthStencilSurface *getCurrentDepthStencilSurface(void) const
					{
						return m_currentDepthStencilSurface;
					}

					void addToReferringResources(ReferringResources &rr);

				};

				class GraphicsContextImpl : public Gxm::GraphicsContextGxm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					GraphicsContextImpl(void);
					~GraphicsContextImpl(void){ finalize(); }

					int initialize(uint32_t width, uint32_t height, 
								   GraphicsContextOption* option=NULL);
					int finalize(void);

					void clearRenderTarget(uint32_t color);
					RenderTarget *getNextRenderTarget(FrameBufferSide side);
					uint32_t getNextRenderTargetIndex(void)
					{
						return m_frameBuffer.getNextRenderTargetIndex();
					}
					DepthStencilSurface *getDepthStencilSurface(void);

					int beginScene(RenderTarget *renderTarget, DepthStencilSurface *depthStencilSurface);

					int endScene(void);

					int flip(uint32_t numVSync);

					int setVertexProgram(VertexProgram *vertexProgram);

					int setFragmentProgram(FragmentProgram *fragmentProgram);

					int draw(Primitive primitive, IndexBuffer *indexBuffer, uint32_t count);
					void *reserveDefaultUniformBuffer(ProgramType programType);


					bool isDisplaymodeSupported(DisplayMode mode)
					{
						if(mode == kDisplayModeNormal){
							return true;
						}else{
							return false;
						}
					}
					int setDisplaymode(DisplayMode mode)
					{
						if(mode == kDisplayModeNormal){
							return SCE_OK;
						}else{
							return -1;
						}
					}
					void setLineWidth(uint32_t width)
					{
						sceGxmSetFrontPointLineWidth(getGxmContext(), width);
					}
					void setPointSize(uint16_t halfWidth, uint16_t halfHeight)
					{
						sceGxmSetFrontPointLineWidth(getGxmContext(), halfWidth*2);
					}


					int initialize(SceGxmShaderPatcher *patcher, 
								   SceGxmContext *context,
								   uint32_t width,
								   uint32_t height, 
								   GraphicsContextOption* option=NULL,
								   bool initializeGxm=true);

					int endSceneGxm(const SceGxmNotification *vertexNotification, 
									const SceGxmNotification *fragmentNotification);

					SceGxmContext *getGxmContext(void) const;


					void addOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg);
					void removeOnSetFrameBufCallback(OnSetFrameBufCallback onSetFrameBufCallback, void *onSetFrameBufCallbackArg);

					int saveBackBufferAsBmp(const char* path);

					// ------------------------------------------------------------------------------
					// Buffer managerment
					// ------------------------------------------------------------------------------
					METHOD_FORWARD_2(m_gpuBufferState, int, setVertexBuffer,  VertexBuffer *, vertexBuffer, uint32_t, streamIndex);
					METHOD_FORWARD_3(m_gpuBufferState, int, setUniformBuffer, ProgramType, programType, UniformBuffer *,     buffer,  uint32_t, bufferIndex);
					METHOD_FORWARD_3(m_gpuBufferState, int, setTexture,       ProgramType, programType, Texture *,           texture, uint32_t, index);

					METHOD_FORWARD_0_CONST(m_gpuBufferState, RenderTarget *,        getCurrentRenderTarget);
					METHOD_FORWARD_0_CONST(m_gpuBufferState, DepthStencilSurface *, getCurrentDepthStencilSurface);


                    #pragma region GpuControl
					METHOD_FORWARD_VOID_1(m_gpuControl, setDepthFunc, DepthFunc, depthFunc);
					METHOD_FORWARD_VOID_1(m_gpuControl, setDepthWriteEnable, bool, enable);
					METHOD_FORWARD_VOID_1(m_gpuControl, setFillMode, bool, isFill);
					METHOD_FORWARD_VOID_1(m_gpuControl, setCullMode, CullMode, cullMode);

					METHOD_FORWARD_VOID_6(m_gpuControl, setViewPort, uint32_t, topLeftX, uint32_t, topLeftY,
									 uint32_t, width, uint32_t, height,	 float, minDepth, float, maxDepth);
					METHOD_FORWARD_VOID_4(m_gpuControl, setRegionClip, uint32_t, topLeftX, uint32_t, topLeftY, uint32_t, width, uint32_t, height);

					void setStencilFunc(uint8_t compareMask, uint8_t writeMask,
										StencilFunc frontFaceFunc, StencilOp   frontFaceStencilFail,StencilOp   frontFaceDepthFail,	StencilOp   frontFaceDepthPass,	
										StencilFunc backFaceFunc,StencilOp   backFaceStencilFail,StencilOp   backFaceDepthFail,	StencilOp   backFaceDepthPass,
										uint8_t stencilRef)
					{
						m_gpuControl.setStencilFunc(compareMask, writeMask,
										frontFaceFunc,  frontFaceStencilFail, frontFaceDepthFail,	frontFaceDepthPass,	
										backFaceFunc,   backFaceStencilFail,   backFaceDepthFail,	   backFaceDepthPass,
										stencilRef);
					}

                    #pragma endregion

                   #pragma region loaderWrapper

					int registerVertexProgram(Program** outProgram, const void *binary, uint32_t binarySize, ShaderFormat shaderFormat=kShaderFormatDefault)
					{
						return m_loader.registerVertexProgram(outProgram, binary, binarySize);
					}

					int registerFragmentProgram(Program** outProgram, const void *binary, uint32_t binarySize, ShaderFormat shaderFormat=kShaderFormatDefault)
					{
						return m_loader.registerFragmentProgram(outProgram, binary, binarySize);
					}


					METHOD_FORWARD_1(m_loader, int, unregisterProgram, Program*, programId);
					METHOD_FORWARD_6(m_loader, int, createVertexProgram, VertexProgram **, vertexProgram, const Program*, programId, 
									const VertexAttribute *, attrs, uint32_t, numAttrs,	const VertexStream *, streams, uint32_t, numStreams);
					METHOD_FORWARD_4(m_loader, int, createFragmentProgram, FragmentProgram **, fragmentProgram, 
									 const Program*, programId, const Program*, vertexProgramId,  const BlendInfo *, blendInfo);
					METHOD_FORWARD_6(m_loader, int, createFragmentProgramGxm, FragmentProgram **, fragmentProgram, const Program*, programId, const Program*, vertexProgramId, 
									 const BlendInfo *, blendInfo, SceGxmOutputRegisterFormat, outputFormat32BIT, SceGxmOutputRegisterFormat, outputFormat64BIT);
					METHOD_FORWARD_2(m_loader, int, cloneVertexProgram, VertexProgram **, vertexProgram, const VertexProgram *, from);
					METHOD_FORWARD_2(m_loader, int, cloneFragmentProgram, FragmentProgram **, fragmentProgram, const FragmentProgram *, from);
					METHOD_FORWARD_2(m_loader, int, createIndexBuffer, IndexBuffer **, outIndexBuffer, uint32_t, size);
					METHOD_FORWARD_2(m_loader, int, createVertexBuffer, VertexBuffer **, outVertexBuffer, uint32_t, size);
					METHOD_FORWARD_2(m_loader, int, createUniformBuffer, UniformBuffer **, outVertexBuffer, uint32_t, size);
					METHOD_FORWARD_5(m_loader, int, createBuffer, Buffer **, outDataBuffer, size_t, size, BufferAccessMode, accessMode, uint32_t, bufferBindFlags, size_t, elementSize);

					int createTexture2dBuffer(Texture2dBuffer **outTexture2dBuffer,
											  BufferFormat format,  uint32_t width,	  uint32_t height,
											  uint32_t mipCount,  BufferAccessMode accessMode,
											  uint32_t bufferBindFlags,   MultisampleMode multiSampleMode )
					{
						return m_loader.createTexture2dBuffer(outTexture2dBuffer,
															  format, width, height,
															  mipCount,  accessMode,
															  bufferBindFlags,   multiSampleMode );
					}


					int createRenderTarget(RenderTarget **outRenderTarget, uint32_t width, uint32_t height)
					{
						return m_loader.createRenderTarget(outRenderTarget, width, height);
					}


					int createDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface, 	uint32_t width, uint32_t height)
					{
						return m_loader.createDepthStencilSurface(outDepthStencilSurface, width, height);
					}


					int createTextureFromFile(Texture **outTexture, const char *filename)
					{
						return m_loader.createTextureFromFile(outTexture, filename);
					}
					int createTextureFromMemory(Texture **outTexture, const void *image, uint32_t gxtImageSize)
					{
						return m_loader.createTextureFromMemory(outTexture, image, gxtImageSize);
					}
					int createTexture(	Texture **outTexture, 		TextureFormat texFormat, 
										uint32_t width,	uint32_t height,	uint32_t mipCount)
					{
						return m_loader.createTexture(	outTexture,  texFormat, width, height, mipCount);
					}

					int createGxmRenderTarget(RenderTarget **outRenderTarget, 
											  uint32_t width,
											  uint32_t height,
											  SceGxmColorSurfaceType type /*=SCE_GXM_COLOR_SURFACE_LINEAR*/,
											  SceGxmColorFormat format=SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR,
											  SceGxmOutputRegisterSize outputRegisterSize = SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,

											  uint32_t renderTargetFlags=NULL,
											  uint16_t scenesPerFrame=1,
											  SceGxmMultisampleMode msaaMode=SCE_GXM_MULTISAMPLE_NONE,
											  uint32_t multisampleLocations=0,

											  SceGxmSyncObject*  vertexSyncObject=NULL,
											  SceGxmSyncObject*  fragmentSyncObject=NULL)
					{
						return m_loader.createGxmRenderTarget( outRenderTarget, width, height,
															  type /*=SCE_GXM_COLOR_SURFACE_LINEAR*/, format, outputRegisterSize,
															  renderTargetFlags, scenesPerFrame, msaaMode,  multisampleLocations,
															  vertexSyncObject,  fragmentSyncObject);
					}
					int createGxmDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface, uint32_t width, uint32_t height,  SceGxmDepthStencilFormat fort,
													 SceGxmDepthStencilSurfaceType type = SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
													 SceGxmMultisampleMode multisampleMode=SCE_GXM_MULTISAMPLE_NONE,
													 bool enableForceStore=false,
													 bool enableForceLoad=false
													 )
					{
						return m_loader.createGxmDepthStencilSurface(outDepthStencilSurface, width,	 height, fort, type,
																	 multisampleMode, enableForceStore, enableForceLoad);

					}
					METHOD_FORWARD_6_D1(m_loader, int, createTextureGxmLinear, Texture **, outTexture, SceGxmTextureFormat, texFormat,
										uint32_t, width, uint32_t, height, uint32_t, mipCount, uint32_t, bufferAlign, 16);
					METHOD_FORWARD_6_D1(m_loader, int, createTextureGxmLinearStrided,  Texture **, outTexture,  SceGxmTextureFormat, texFormat,
						                uint32_t, width, uint32_t, height, uint32_t, byteStride, uint32_t, bufferAlign, 16);
					METHOD_FORWARD_6_D1(m_loader, int, createTextureGxmTiled, Texture **,outTexture, SceGxmTextureFormat, texFormat, \
						               uint32_t, width,  uint32_t, height,  uint32_t, mipCount,  uint32_t, bufferAlign, 16);
					METHOD_FORWARD_6_D1(m_loader, int, createTextureGxmSwizzled, Texture **,outTexture, SceGxmTextureFormat, texFormat,\
						             uint32_t, width, uint32_t, height,	 uint32_t, mipCount, uint32_t, bufferAlign, 16);
					METHOD_FORWARD_6_D1(m_loader, int, createTextureGxmCube, Texture **, outTexture, SceGxmTextureFormat, texFormat,\
						            uint32_t, width,	 uint32_t, height, uint32_t, mipCount, uint32_t, bufferAlign, 16);
					METHOD_FORWARD_0_CONST(m_loader, SceGxmShaderPatcher *, getGxmShaderPatcher);
					METHOD_FORWARD_3(m_loader, void *, memalign, MemType, memtype, uint32_t, boundary, uint32_t, size);
					METHOD_FORWARD_VOID_1(m_loader, free, void *, ptr);
					METHOD_FORWARD_VOID_1(m_loader, getMemoryUsage, MemoryUsage *, usage);

				  #pragma endregion
					typedef void (*graphicsContextImplOnSetFrameBufferCallback)(void *arg);
					int addOnSetFrameBufferCallback   (graphicsContextImplOnSetFrameBufferCallback callback, void *param);
					int removeOnSetFrameBufferCallback(graphicsContextImplOnSetFrameBufferCallback callback, void *param);


					std::string getResourceType(void) const
					{
						return "Graphics::GraphicsContext";
					}

				private: 
					SceGxmContextParams m_contextParams;
					GraphicsLoaderImpl m_loader;								
					GpuControl m_gpuControl;
					GpuBufferState m_gpuBufferState;

					static const MultisampleMode	m_defaultMultisampleMode	= kMultisampleNone;
					static const uint32_t			m_defaultNumFrameBuffer		= 3;

					bool m_initializeGxm;
					SceGxmContext *createGxmContext(void);
					SceGxmContext *m_gxmContext;
					bool m_freeContext;

					Program* m_clearV;
					Program* m_clearF;
					Effect::EffectData *m_clearEffect;
					Effect::EffectInstance *m_clearEffectInstance;
					VertexBuffer *m_clearVertexBuffer;
					IndexBuffer *m_clearIndexBuffer;
					Effect::EffectParameter *m_clearColor;
					FrameBufferImpl m_frameBuffer;
				};

			}
		}
	}
}


#endif // _SCE_TARGET_OS_PSP2
#endif //_SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_GXM_H
