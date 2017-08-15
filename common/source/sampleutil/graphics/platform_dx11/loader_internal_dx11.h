/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_DX11_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_DX11_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include "buffer_internal_dx11.h"
#include <dxgi.h>
#include "D3D11.h"
#include "util/util.h"
namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class GraphicsLoaderImpl : public Dx11::GraphicsLoaderDx11, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					sce::SampleUtil::Internal::Util::Mutex m_mutex;
					GraphicsLoaderImpl(void){}
					~GraphicsLoaderImpl(void){}

					int initialize(void);

					int registerVertexProgram(Program** outProgram, const void *program, uint32_t size, ShaderFormat shaderFormat=kShaderFormatDefault);
					int registerFragmentProgram(Program** outProgram, const void *program, uint32_t size, ShaderFormat shaderFormat=kShaderFormatDefault);
					int registerComputeProgram(Program** outProgram, const void *binary, uint32_t binarySize, ShaderFormat shaderFormat=kShaderFormatDefault);


					Program* registerVertexProgramFromSourceFile(const char *path, const char *entry);
					Program* registerFragmentProgramFromSourceFile(const char *path, const char *entry);
					Program* registerVertexProgramFromSource(const char *programSource, const char *entry);
					Program* registerFragmentProgramFromSource(const char *programSource, const char *entry);

					Program* registerHlslComputetProgramFromSource(SCE_SAMPLE_LPCTSTR program, const char* entry);


					int createVertexProgram(VertexProgram **vertexProgram, const Program* programId, 
						const VertexAttribute *attrs, uint32_t numAttrs,
						const VertexStream *streams, uint32_t numStreams);

					int createFragmentProgram(FragmentProgram **fragmentProgram, 
						const Program *programId, 
						const Program *vertexProgramId, const BlendInfo *blendInfo);

					int createComputeProgram(ComputeProgram **computeProgram, const Program* programId);

					int cloneVertexProgram(VertexProgram **vertexProgram, const VertexProgram *from);
					int cloneFragmentProgram(FragmentProgram **fragmentProgram, const FragmentProgram *from);
					int cloneComputeProgram(ComputeProgram **computeProgram, const FragmentProgram *from);

					int finalize(void);

					int initialize(ID3D11Device *device, CGprofile cgVertexProfile=CG_PROFILE_VS_4_0, CGprofile cgFragmentProfile=CG_PROFILE_PS_4_0);

					ID3D11Device *getDevice(void) const;

					int createIndexBuffer(IndexBuffer **outIndexBuffer, uint32_t size);
					int createVertexBuffer(VertexBuffer **outVertexBuffer, uint32_t size);
					int createUniformBuffer(UniformBuffer **outVertexBuffer, uint32_t size);
					int createStructuredBuffer(StructuredBuffer **outStructuredBuffer, uint32_t stride, uint32_t numElements);
					int createRwStructuredBuffer(RwStructuredBuffer **outStructuredBuffer, uint32_t stride, uint32_t numElements);

					int createBuffer(Buffer **outDataBuffer,
											 size_t              size,
											 BufferAccessMode    accessMode,
											 uint32_t            bufferBindFlags,
											 size_t              elementSize);

					int createTexture2dBuffer(Texture2dBuffer **outTexture2dBuffer,
												  BufferFormat format,
												  uint32_t width,
												  uint32_t height,
												  uint32_t mipCount,
												  BufferAccessMode accessMode,
												  uint32_t bufferBindFlags, /* BufferBindFlag */
												  MultisampleMode multiSampleMode
												  );

					int createRenderTarget(RenderTarget **outRenderTarget, 	uint32_t width,  uint32_t height);
					int createDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface, uint32_t width, uint32_t height);

					int createTextureFromFile(Texture **outTexture, const char *filename);
					int createTextureFromMemory(Texture **outTexture, const void *image, uint32_t gxtImageSize);
					int createTexture(Texture **outTexture, TextureFormat texFormat, uint32_t width, uint32_t height, uint32_t mipCount);

					int saveBackBufferAsBmp(const char* path)
					{
						printf("saveBackBufferAsBmp is not yet implemented\n");
						return SCE_OK;
					}

					std::string getResourceType(void) const
					{
						return "Graphics::GraphicsLoader";
					}

				private:
					ID3D11Device *m_device;
					CGcontext m_cgContext;
					CGprofile m_cgVertexProfile;
					CGprofile m_cgFragmentProfile;
				};


				DXGI_SAMPLE_DESC getSampleDesc(sce::SampleUtil::Graphics::Impl::GraphicsLoaderImpl *loader, sce::SampleUtil::Graphics::MultisampleMode multiSampleMode);
			}
		}
	}

}


#endif // _SCE_TARGET_OS_WINDOWS
#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_DX11_H */

