/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_DX11_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_DX11_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil_internal.h>
#include "program_internal_dx11.h"
#include "buffer_internal_dx11.h"
#include "loader_internal_dx11.h"

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class FrameBufferImpl 
				{
				public:

					FrameBufferImpl(void);
					virtual ~FrameBufferImpl(void){
						finalize();
					}

					int initialize(GraphicsLoaderImpl *loader, 
						uint32_t width, uint32_t height, 
						MultisampleMode multiSampleMode,
						uint32_t numBuffer);

					virtual int finalize(void);

					RenderTarget *getNextRenderTarget(sce::SampleUtil::Graphics::GraphicsContext::FrameBufferSide side);
					DepthStencilSurface *getDepthStencilSurface(void);
					int flip(uint32_t numVSync);

					HWND getWindowHandle(void) const;

					HDC getDisplayDeviceContext(void) const;

				private:
					HWND m_windowHandle;
					IDXGISwapChain *m_swapChain;
					Texture2dBufferImpl *m_renderTargetTextureBuffer;
					Texture2dBufferImpl *m_depthTargetTextureBuffer;
					RenderTargetImpl *m_frameBufferRenderTarget;
					DepthStencilSurfaceImpl *m_depthStencilSurface;
					HDC m_displayDeviceContext;
				};

				/* ------------------------------------------------------------------------------------------------------------------ */

				class GpuControl
				{
				private:
					ID3D11Device *m_device;
					ID3D11DeviceContext *m_context;
					ID3D11DepthStencilState *m_depthStencilstate;
					D3D11_DEPTH_STENCIL_DESC m_currentDepthStencilDesc;
					ID3D11RasterizerState *m_rasterizerState;
					D3D11_RASTERIZER_DESC m_currentRasterizeDesc;
					sce::SampleUtil::Internal::Util::Mutex *m_mutex;
				public:
					int initialize(
						sce::SampleUtil::Internal::Util::Mutex *mutex,
						ID3D11Device *device, ID3D11DeviceContext *context);
					int finalize(void);

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

					void setViewPort(uint32_t topLeftX, uint32_t topLeftY,
									 uint32_t width,	uint32_t height,
									 float minDepth,	float maxDepth);
					
					void setRegionClip(uint32_t topLeftX,		uint32_t topLeftY,
									   uint32_t width,		uint32_t height);


					void setFillMode(bool isFill);
					void setCullMode(CullMode cullMode);

					void setLineWidth(uint32_t width);
					void setPointSize(uint16_t,uint16_t);
				};

				/* ------------------------------------------------------------------------------------------------------------------ */

				class GpuBufferState
				{
				private:
					ID3D11DeviceContext *m_context;
					sce::SampleUtil::Internal::Util::Mutex *m_mutex;
					ID3D11Buffer *m_vertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

					UniformBufferImpl *m_vertexParameterBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
					UniformBufferImpl *m_fragmentParameterBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

				public:
					int initialize(sce::SampleUtil::Internal::Util::Mutex *mutex, ID3D11DeviceContext *context);
					int setVertexBuffer(VertexBuffer *_vertexBuffer, uint32_t streamIndex);					
					int setUniformBuffer(ProgramType programType, UniformBuffer *buffer, uint32_t bufferIndex);

					void setUpVertexBuffers(const VertexProgramImpl *vertexProgram);
					void setUpUniformBuffers(void);

					int setTexture(ProgramType programType, Texture *texture, uint32_t index);
					int setResource(ProgramType programType, Texture *buffer, uint32_t index);
					int setResource(ProgramType programType, StructuredBuffer *buffer, uint32_t index);
					int setRwResource(ProgramType programType, RwStructuredBuffer *buffer, uint32_t index);
					
					int setRenderTarget(RenderTarget* renderTarget, DepthStencilSurface *depthStencil);

					RenderTarget *getCurrentRenderTarget(void) const;
					DepthStencilSurface *getCurrentDepthStencilSurface(void) const;
				};

				/* ------------------------------------------------------------------------------------------------------------------ */

				class GraphicsContextImpl : public Dx11::GraphicsContextDx11, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					GraphicsContextImpl(void);
					~GraphicsContextImpl(void)
					{
						finalize();
					}

					int initialize(uint32_t width, uint32_t height, GraphicsContextOption* option=NULL);
					int finalize(void);

					void clearRenderTarget(uint32_t color);

					int beginScene(RenderTarget *renderTarget, DepthStencilSurface *depthStencilSurface);
					int endScene(void);

					int setVertexProgram(VertexProgram *vertexProgram);
					int setFragmentProgram(FragmentProgram *fragmentProgram);
					int setComputeProgram(ComputeProgram *computeProgram);

					int draw(Primitive primitive, IndexBuffer *indexBuffer, uint32_t count);
					int dispatch(uint32_t threadGroupX,	 uint32_t threadGroupY,	 uint32_t threadGroupZ);

					void *reserveDefaultUniformBuffer(ProgramType programType);

					ID3D11DeviceContext *getDeviceContext(void) const;
					ID3D11Device *getDevice(void) const;
					HWND getWindowHandle(void) const;

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

					RenderTarget *getCurrentRenderTarget(void) const
					{
						return m_state.m_currentRenderTarget;
					}		
					DepthStencilSurface *getCurrentDepthStencilSurface(void) const
					{
						return m_state.m_currentDepthStencilSurface;
					}
					int readBuffer(void *dst, const BufferInterface* from, uint64_t offset, uint64_t numBytes);

					// ------------------------------------------------------------------------------
					// Frame Buffer
					// ------------------------------------------------------------------------------
					METHOD_FORWARD_1_D1(m_frameBuffer, RenderTarget *, getNextRenderTarget, FrameBufferSide, size, kFrameBufferSideLeft);
					METHOD_FORWARD_0(m_frameBuffer, DepthStencilSurface *, getDepthStencilSurface);
					METHOD_FORWARD_1(m_frameBuffer, int, flip, uint32_t, numVSync);
					METHOD_FORWARD_0(m_frameBuffer, HDC, getDisplayDeviceContext);
					// ------------------------------------------------------------------------------
					// Gpu control
					// ------------------------------------------------------------------------------
					METHOD_FORWARD_VOID_1(m_gpuControl, setDepthFunc,       DepthFunc, depthFunc);
					METHOD_FORWARD_VOID_1(m_gpuControl, setDepthWriteEnable,bool,      enable);
					METHOD_FORWARD_VOID_1(m_gpuControl, setFillMode,        bool,     isFill);
					METHOD_FORWARD_VOID_1(m_gpuControl, setCullMode,         CullMode,  cullMode);
					METHOD_FORWARD_VOID_4(m_gpuControl, setRegionClip, uint32_t, topLeftX, uint32_t, topLeftY, uint32_t, width,	uint32_t, height);
					METHOD_FORWARD_VOID_6(m_gpuControl, setViewPort,   uint32_t, topLeftX, uint32_t, topLeftY, uint32_t, width, uint32_t, height, float, minDepth, float, maxDepth);
					METHOD_FORWARD_VOID_1(m_gpuControl, setLineWidth, uint32_t, width);
					METHOD_FORWARD_VOID_2(m_gpuControl, setPointSize, uint16_t, w, uint16_t, h);
					void setStencilFunc(uint8_t compareMask, uint8_t writeMask, 
										StencilFunc frontFaceFunc, StencilOp frontFaceStencilFail, StencilOp frontFaceDepthFail, StencilOp frontFaceDepthPass,	
										StencilFunc backFaceFunc,  StencilOp backFaceStencilFail,  StencilOp backFaceDepthFail,  StencilOp backFaceDepthPass,  uint8_t stencilRef)
					{
						m_gpuControl.setStencilFunc(compareMask, writeMask, 
							frontFaceFunc, frontFaceStencilFail, frontFaceDepthFail, frontFaceDepthPass,
							backFaceFunc,  backFaceStencilFail,  backFaceDepthFail,  backFaceDepthPass,  stencilRef);
					}

					// ------------------------------------------------------------------------------
					// Buffer managerment
					// ------------------------------------------------------------------------------
					METHOD_FORWARD_2(m_gpuBufferState, int, setVertexBuffer,  VertexBuffer *, vertexBuffer, uint32_t, streamIndex);
					METHOD_FORWARD_3(m_gpuBufferState, int, setUniformBuffer, ProgramType, programType, UniformBuffer *,     buffer,  uint32_t, bufferIndex);
					METHOD_FORWARD_3(m_gpuBufferState, int, setTexture,       ProgramType, programType, Texture *,           texture, uint32_t, index);
					METHOD_FORWARD_3(m_gpuBufferState, int, setResource,      ProgramType, programType, Texture *,           texture, uint32_t, index);
					METHOD_FORWARD_3(m_gpuBufferState, int, setResource,      ProgramType, programType, StructuredBuffer *,  texture, uint32_t, index);
					METHOD_FORWARD_3(m_gpuBufferState, int, setRwResource,    ProgramType, programType, RwStructuredBuffer *,texture, uint32_t, index);


					// ------------------------------------------------------------------------------
					// Loader 
					// ------------------------------------------------------------------------------
					METHOD_FORWARD_4(m_loader, int, registerVertexProgram, Program**, outProgram, const void *, program, uint32_t, size, ShaderFormat, shaderFormat=kShaderFormatDefault);
					METHOD_FORWARD_4(m_loader, int, registerFragmentProgram, Program**, outProgram, const void *, program, uint32_t, size, ShaderFormat, shaderFormat=kShaderFormatDefault);
					METHOD_FORWARD_4(m_loader, int, registerComputeProgram, Program**, outProgram, const void *, program, uint32_t, size, ShaderFormat, shaderFormat=kShaderFormatDefault);
					int createVertexProgram(VertexProgram **vertexProgram, const Program* programId, 
						                    const VertexAttribute *attrs, uint32_t numAttrs,
											const VertexStream *streams, uint32_t numStreams)
					{
						return m_loader.createVertexProgram(vertexProgram, programId, 
							attrs,  numAttrs,
							streams, numStreams);
					}
					METHOD_FORWARD_4(m_loader, int, createFragmentProgram, FragmentProgram **, fragmentProgram, const Program*, programId, const Program*, vertexProgramId, const BlendInfo *,blendInfo);
					METHOD_FORWARD_2(m_loader, int, createComputeProgram, ComputeProgram **, computeProgram, const Program*, programId);
					METHOD_FORWARD_2(m_loader, int, cloneVertexProgram,   VertexProgram **,  vertexProgram,  const VertexProgram *, from);
					METHOD_FORWARD_2(m_loader, int, cloneFragmentProgram, FragmentProgram **, fragmentProgram, const FragmentProgram *,from);
					METHOD_FORWARD_2(m_loader, int, createIndexBuffer,    IndexBuffer **,  outIndexBuffer, uint32_t, size);
					METHOD_FORWARD_2(m_loader, int, createVertexBuffer,   VertexBuffer **, outVertexBuffer, uint32_t, size);
					METHOD_FORWARD_2(m_loader, int, createUniformBuffer,  UniformBuffer **, outVertexBuffer, uint32_t, size);
					METHOD_FORWARD_3(m_loader, int, createStructuredBuffer, StructuredBuffer **, outStructuredBuffer, uint32_t, stride, uint32_t, numElements);
					METHOD_FORWARD_3(m_loader, int, createRwStructuredBuffer, RwStructuredBuffer **, outStructuredBuffer, uint32_t, stride, uint32_t, numElements);
					METHOD_FORWARD_5(m_loader, int, createBuffer,             Buffer **, outDataBuffer, size_t, size, BufferAccessMode, accessMode, uint32_t, bufferBindFlags, size_t, elementSize);
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

					METHOD_FORWARD_3(m_loader, int, createRenderTarget, RenderTarget **, outRenderTarget, uint32_t, width, uint32_t, height);
					METHOD_FORWARD_3(m_loader, int, createDepthStencilSurface, DepthStencilSurface **, outDepthStencilSurface, 	uint32_t, width, uint32_t, height);
					METHOD_FORWARD_2(m_loader, int, createTextureFromFile, Texture **, outTexture, const char *, filename);
					METHOD_FORWARD_3(m_loader, int, createTextureFromMemory, Texture **, outTexture, const void *, image, uint32_t, gxtImageSize);
					METHOD_FORWARD_5(m_loader, int, createTexture, 	Texture **, outTexture, TextureFormat, texFormat, uint32_t, width, uint32_t, height, uint32_t, mipCount);
					METHOD_FORWARD_2(m_loader, Program*, registerVertexProgramFromSourceFile, const char *, path, const char *, entry);
					METHOD_FORWARD_2(m_loader, Program*, registerFragmentProgramFromSourceFile, const char *, path,          const char *, entry);
					METHOD_FORWARD_2(m_loader, Program*, registerVertexProgramFromSource,       const char *, programSource, const char *, entry);
					METHOD_FORWARD_2(m_loader, Program*, registerFragmentProgramFromSource,     const char *, programSource, const char *, entry);
					METHOD_FORWARD_1(m_loader, int,saveBackBufferAsBmp, const char*, path);

					std::string getResourceType(void) const
					{
						return "Graphics::GraphicsContext";
					}

				private: 
					GraphicsLoaderImpl m_loader;
					struct State
					{
						RenderTargetImpl *m_currentRenderTarget;
						DepthStencilSurfaceImpl *m_currentDepthStencilSurface;
						VertexProgramImpl *m_vertexProgram;
						FragmentProgramImpl *m_fragmentProgram;
					} m_state;

					GpuControl m_gpuControl;
					GpuBufferState m_gpuBufferState;

					static const MultisampleMode	m_defaultMultisampleMode	= kMultisampleNone;
					static const uint32_t			m_defaultNumFrameBuffer		= 3;

					ID3D11Device *              m_pDevice;
					ID3D11DeviceContext *       m_pDeviceContext;
					D3D_DRIVER_TYPE             m_driverType;
					FrameBufferImpl m_frameBuffer;


					UniformBufferImpl *m_reservedVertexUniformBuffer;
					UniformBufferImpl *m_reservedFragmentUniformBuffer;

				};

			}
		}
	}
}




#endif /* _SCE_TARGET_OS_WINDOWS */

#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_CONTEXT_INTERNAL_DX11_H */
