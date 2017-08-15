/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_GXM_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_GXM_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/graphics.h>
#include "program_internal_gxm.h"

#include <scepng.h>
#include <scejpeg.h>
#include <gxt.h>
#include "heap_gxm.h"
namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class _JpegDecoder
				{
				public:
					_JpegDecoder();
					~_JpegDecoder();

					int initialize(void);
					
					SceUID allocateDecodeMemblock(const uint32_t imageWidth, const uint32_t imageHeight,
												  void **outFileBuf,   uint32_t *outFileBufSize, 
												  void **outDecodeBuf, uint32_t *outDecodeBufSize);


					int getOutputInfo(SceJpegOutputInfo *outputInfo, int *decodeMode, const void *fileImage, uint32_t fileImageSize);

					int decodeJpeg(uint32_t *imageData, const void *fileImage, uint32_t fileSize, unsigned char *decodeBuffer, uint32_t decodeBufferSize, const SceJpegOutputInfo *outputInfo, int decodeMode);
					
					int finalize(void);

				private:
					static const bool m_useLPDDR2Memory = false;
					bool m_isInitialized;
				};


				class GraphicsLoaderImpl : public Gxm::GraphicsLoaderGxm, public sce::SampleUtil::Impl::ResourceImpl
				{
					friend class ProgramImpl;
				public:
					GraphicsLoaderImpl(void){}
					~GraphicsLoaderImpl(void){}

					int initialize(void);

					int registerVertexProgram(Program** outProgram, const void *binary, uint32_t binarySize, ShaderFormat shaderFormat=kShaderFormatDefault);
					int registerFragmentProgram(Program** outProgram, const void *binary, uint32_t binarySize, ShaderFormat shaderFormat=kShaderFormatDefault);


					int unregisterProgram(Program* programId);


					int createVertexProgram(VertexProgram **vertexProgram, const Program* programId, 
											const VertexAttribute *attrs, uint32_t numAttrs,
											const VertexStream *streams, uint32_t numStreams);

					int createFragmentProgram(FragmentProgram **fragmentProgram, 
											  const Program *programId, 
											  const Program *vertexProgramId, 
											  const BlendInfo *blendInfo);

					int cloneVertexProgram(VertexProgram **vertexProgram, const VertexProgram *from);

					int cloneFragmentProgram(FragmentProgram **fragmentProgram, const FragmentProgram *from);

					int finalize(void);


					int initialize(SceGxmShaderPatcher *patcher);

					int createFragmentProgramGxm(FragmentProgram **fragmentProgram, 
												 const Program* programId, 
												 const Program* vertexProgramId, 
												 const BlendInfo *blendInfo,
												 SceGxmOutputRegisterFormat outputFormat32BIT,
												 SceGxmOutputRegisterFormat outputFormat64BIT);


					SceGxmShaderPatcher *getGxmShaderPatcher(void) const;

					_JpegDecoder *getJpegDecoder();

					void *memalign(MemType memtype, uint32_t boundary, uint32_t size);

					void free(void *ptr);

					uint32_t convertToVertexUsseOffset(void *ptr) const;

					uint32_t convertToFragmentUsseOffset(void *ptr) const;


					void getMemoryUsage(MemoryUsage *usage);

					int createIndexBuffer(IndexBuffer **outIndexBuffer, uint32_t size);
					int createVertexBuffer(VertexBuffer **outVertexBuffer, uint32_t size);
					int createUniformBuffer(UniformBuffer **outVertexBuffer, uint32_t size);

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

					int createRenderTarget(RenderTarget **outRenderTarget, 
										   uint32_t width, 
										   uint32_t height);


					int createDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface, 
												  uint32_t width, 
												  uint32_t height);


					int createTextureFromFile(Texture **outTexture, const char *filename);
					int createTextureFromMemory(Texture **outTexture, const void *image, uint32_t gxtImageSize);
					int createTexture(	Texture **outTexture, 		TextureFormat texFormat, 
										uint32_t width,	uint32_t height,	uint32_t mipCount);


					int createTextureGxmLinear(
											   Texture **outTexture,
											   SceGxmTextureFormat texFormat,
											   uint32_t width,
											   uint32_t height,
											   uint32_t mipCount,
											   uint32_t bufferAlign=16);

					int createTextureGxmLinearStrided(
													  Texture **outTexture,
													  SceGxmTextureFormat texFormat,
													  uint32_t width,
													  uint32_t height,
													  uint32_t byteStride,
													  uint32_t bufferAlign=16);

					int createTextureGxmTiled(
											  Texture **outTexture,
											  SceGxmTextureFormat texFormat,
											  uint32_t width,
											  uint32_t height,
											  uint32_t mipCount,
											  uint32_t bufferAlign=16);

					int createTextureGxmSwizzled(
												 Texture **outTexture,
												 SceGxmTextureFormat texFormat,
												 uint32_t width,
												 uint32_t height,
												 uint32_t mipCount,
												 uint32_t bufferAlign=16);

					int createTextureGxmCube(
											 Texture **outTexture,
											 SceGxmTextureFormat texFormat,
											 uint32_t width,
											 uint32_t height,
											 uint32_t mipCount,
											 uint32_t bufferAlign=16);


					int createGxmRenderTarget(
											  RenderTarget **outRenderTarget, 
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
											  SceGxmSyncObject*  fragmentSyncObject=NULL);



					int createGxmDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface, 
													 uint32_t width,
													 uint32_t height, 
													 SceGxmDepthStencilFormat fort=SCE_GXM_DEPTH_STENCIL_FORMAT_D16, /*= SCE_GXM_DEPTH_STENCIL_FORMAT_DF32*/
													 SceGxmDepthStencilSurfaceType type = SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
													 SceGxmMultisampleMode multisampleMode=SCE_GXM_MULTISAMPLE_NONE,
													 bool enableForceStore=false,
													 bool enableForceLoad=false
													 );
					std::string getResourceType(void) const
					{
						return "Graphics::GraphicsLoader";
					}

				private:
					bool m_destroyShaderPatcher;
					SceGxmShaderPatcher *m_gxmShaderPatcher;

					SceGxmShaderPatcher *createGxmShaderPatcher(void);

					static void *patcherHostAlloc(void *userData, uint32_t size);
					static void patcherHostFree(void *userData, void *mem);
					static void *patcherBufferAlloc(void *userData, uint32_t size);
					static void patcherBufferFree(void *userData, void *pMem);
					static void *patcherVertexUsseAlloc(void *userData, uint32_t size, uint32_t *usseOffset);
					static void patcherVertexUsseFree(void *userData, void *pMem);
					static void *patcherFragmentUsseAlloc(void *userData, uint32_t size, uint32_t *usseOffset);
					static void patcherFragmentUsseFree(void *userData, void *pMem);

					Heap m_usseHeap;
					Heap m_lpddrUcRwHeap;
					Heap m_cdramRwHeap;

					_JpegDecoder m_jpegDecoder;

					struct ProgramInfo
					{
						ProgramImpl programId;
						const SceGxmProgram *program;
						uint32_t count;
					};
					sce::SampleUtil::Map<const Program*, ProgramInfo*>::Type m_id2ProgramInfoMap;
					sce::SampleUtil::Map<const SceGxmProgram *,  ProgramInfo*>::Type m_program2ProgramInfoMap;


					struct ProgramInfo2
					{
						SceGxmShaderPatcherId patcherId;
						const SceGxmProgram *program;
						uint32_t count;
					};
					sce::SampleUtil::Map<const SceGxmProgram*, ProgramInfo2>::Type m_programInfoMap;
					SceGxmShaderPatcherId _registerProgram(const SceGxmProgram* program);
					int _unregisterProgram(const SceGxmProgram* program);
					int _referProgram(const SceGxmProgram* program);

				};


			}
		}
	}
}

#endif // _SCE_TARGET_OS_PSP2
#endif //_SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_GXM_H
