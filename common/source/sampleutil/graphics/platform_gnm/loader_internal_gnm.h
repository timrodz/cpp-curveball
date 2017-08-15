/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_H

#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS


#include "heap_gnm.h"
#include "program_internal_gnm.h"
#include "buffer_internal_gnm.h"
//#include <texture_tool.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{


				class GraphicsLoaderImpl : public Gnm::GraphicsLoaderGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					
					struct MemoryBlock
					{
						void *m_systemPoolPtr;
						size_t m_systemPoolSize;
						off_t m_systemPoolOffset;

						void *m_gpuSharedPtr;
						size_t m_gpuSharedSize;
						off_t m_gpuSharedOffset;

						MemoryBlock(void);
						int initialize(size_t systemHeapSize, size_t videoHeapSize);
						int finalize(void);
					};
					MemoryBlock m_memoryBlock;
					Heap m_heap;
				public:
					GraphicsLoaderImpl(void){}

					int registerVertexProgram(Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat=kShaderFormatDefault);
					int registerFragmentProgram(Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat=kShaderFormatDefault);
					int registerComputeProgram(Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat=kShaderFormatDefault);
					int initialize(size_t systemHeapSize, size_t videoHeapSize);
					int finalize(void);
					Program* registerProgramFromMemory(const void *binary, uint32_t binarySize);
					
					int unregisterProgram(const Program* programId);
					

					int createVertexProgram(VertexProgram **vertexProgram, const Program* programId,
											const VertexAttribute *attrs, uint32_t numAttrs,
											const VertexStream *streams, uint32_t numStreams);
					
					int releaseVertexProgram(VertexProgram *vertexProgram);
					
					int createFragmentProgram(FragmentProgram **fragmentProgram, const Program* programId, const Program* vertexProgramId, const BlendInfo *blendInfo);
					int createComputeProgram(ComputeProgram **computeProgram, const Program* programId);
					
					int releaseFragmentProgram(FragmentProgram *fragmentProgram);
					
					int cloneVertexProgram(VertexProgram **vertexProgram, const VertexProgram *from);
					int cloneFragmentProgram(FragmentProgram **fragmentProgram, const FragmentProgram *from);


					int createRenderTarget(RenderTarget **outRenderTarget,  uint32_t width,   uint32_t height);
					
					int createDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface,  uint32_t width, uint32_t height);

					Heap* getHeap(void)
					{
						return &m_heap;
					}

					virtual ~GraphicsLoaderImpl(void)
					{
						finalize();
					}


					int createTexture(Texture **outTexture,	  TextureFormat texFormat, uint32_t width,  uint32_t height,  uint32_t mipCount);
					int createTextureFromFile(Texture **outTexture, const char *filename);
					int createTextureFromMemory(Texture **outTexture, const void *data, uint32_t size);
					int createIndexBuffer(IndexBuffer **outIndexBuffer, uint32_t size);
					int createVertexBuffer(VertexBuffer **outVertexBuffer, uint32_t size);
					int createUniformBuffer(UniformBuffer **outVertexBuffer, uint32_t size);
					int createStructuredBuffer(StructuredBuffer **outStructuredBuffer, uint32_t stride, uint32_t numElements);
					int createRwStructuredBuffer(RwStructuredBuffer * *outStructuredBuffer, uint32_t stride, uint32_t numElements);

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

					int createTexture2dBufferUsingExistentBuffer(Texture2dBuffer **outTexture2dBuffer,
												  BufferFormat format,
												  uint32_t width,
												  uint32_t height,
												  uint32_t mipCount,
												  BufferAccessMode accessMode,
												  uint32_t bufferBindFlags, /* BufferBindFlag */
												  MultisampleMode multiSampleMode,
												  void *buffer
												  );
					int createTextureFromBuffer(Texture **outTexture, Texture2dBuffer *buffer, TextureFormat texFormat);

					struct ProgramInfo2
					{
						GnmProgram *program;
						const void *binary;
						uint32_t count;
					};
					sce::SampleUtil::Map<const void*, ProgramInfo2>::Type m_programInfoMap;
					GnmProgram* _registerProgram(const void* binary, uint32_t binarySize);
					int _unregisterProgram(const void* binary);
					int _referProgram(const void* binary);


					void* memalignSystemSharedMemory(size_t boundary, size_t size)
					{
						return m_heap.allocateSystemShared(boundary, size);
					}
					void  freeSystemSharedMemory(void* ptr)
					{
						m_heap.releaseSystemShared(ptr);
					}
					void* memalignVideoSharedMemory(size_t boundary, size_t size)
					{
						return m_heap.allocateVideoShared(boundary, size);
					}
					void  freeVideoSharedMemory(void* ptr)
					{
						m_heap.releaseVideoShared(ptr);
					}

					std::string getResourceType(void) const
					{
						return "Graphics::GraphicsLoader";
					}

					
					

				};


			}
		}
	}
}

#endif //defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_LOADER_INTERNAL_H */
