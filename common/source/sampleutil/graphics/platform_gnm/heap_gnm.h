/* SIE CONFIDENTIAL
   PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
   *
   *      Copyright (C) 2013 Sony Interactive Entertainment Inc.
   *                        All Rights Reserved.
   */

#include <stdint.h>
#include <mspace.h>
#include <gnm.h>
#include <gnmx.h>
#include "../../util/util.h"

#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_HEAP_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_HEAP_H

#include <shader.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class GraphicsLoaderImpl;

				// Helper structure to encapsulate a block of CPU/GPU shared memory
				struct MemBlock
				{
					uint32_t *m_ptr; //! CPU-addressible pointer
					uint32_t  m_size;  //! Size of the block (in bytes)
					uint64_t  m_addr; //! 40-bit GPU-addressible address

					uint32_t getLoAddr() const { return (uint32_t)(m_addr & 0xFFFFFFFF); }
					uint32_t getHiAddr() const { return (uint32_t)(m_addr >> 32); }

					uint32_t getLoAddr256() const { return (uint32_t)((m_addr>>8) & 0xFFFFFFFF); }
					uint32_t getHiAddr256() const { return (uint32_t)(m_addr >> 40); }

					/* Returns m_addr shifted right by 8 bits, to fit in a single 32-bit field.	 */
					uint32_t  getAddress256ByteBlocks() const
					{
						SCE_GNM_ASSERT((m_addr&0xFF)==0);
						return static_cast<uint32_t>(m_addr>>8);
					}
				};

				class Heap
				{
				public:
					SceLibcMspace m_systemHeap;
					SceLibcMspace m_videoSharedHeap;

					void *m_system;
					uint32_t m_systemSize;
					void *m_gpuShared;
					uint32_t m_gpuSharedSize;
					Internal::Util::Mutex m_mutex;
				public:
					Heap(void);
					~Heap(void);
					int initialize2( void *system,	    uint32_t systemSize,
									 void *gpuShared, 	uint32_t gpuSharedSize);

					bool isSystemMem(void *p)
					{
						uintptr_t sp = (uintptr_t)m_system;
						uintptr_t up = (uintptr_t)p;
						return (sp <= up) && (up < (sp+m_systemSize));
					}
					bool isGpuSharedMem(void *p)
					{
						uintptr_t sp = (uintptr_t)m_gpuShared;
						uintptr_t up = (uintptr_t)p;
						return (sp <= up) && (up < (sp+m_gpuSharedSize));
					}

					int finalize(void);


					void *allocateSystemShared(size_t alignment, size_t size);
					void releaseSystemShared(void *ptr);
					void *allocateVideoShared(size_t alignment, size_t size);
					void releaseVideoShared(void *ptr);

					uint64_t allocateSystemMemoryBlock(MemBlock *pOutput, uint32_t size, sce::Gnm::AlignmentType align);
					uint64_t allocateVideoMemoryBlock(MemBlock *pOutput, uint32_t size, sce::Gnm::AlignmentType align);
					void releaseMemoryBlock(MemBlock *pBlock);
					void dump(void);
				};


			

				int loadAndAllocateVsShaderFromMemory(Impl::GraphicsLoaderImpl *loader,
													  sce::Gnmx::VsShader* &vsShader,
													  MemBlock &memBlock,
													  const void *sbFileContents, uint32_t sbFileSizeInBytes);

				int loadAndAllocatePsShaderFromMemory(Impl::GraphicsLoaderImpl *loader,
													  sce::Gnmx::PsShader* &psShader, 
													  MemBlock &memBlock, 
													  const void *sbFileContents, uint32_t sbFileSizeInBytes);

				int loadAndAllocateCsShaderFromMemory(Impl::GraphicsLoaderImpl *loader,
													  sce::Gnmx::CsShader * &csShader, 
													  MemBlock &memBlock, 
													  const void *sbFileContents, uint32_t sbFileSizeInBytes);

				int loadAndAllocateLsShaderFromMemory(Impl::GraphicsLoaderImpl *loader,
													  sce::Gnmx::LsShader *& lsShader, 
													  MemBlock &memBlock, 
													  const void *sbFileContents, uint32_t sbFileSizeInBytes);

				int loadAndAllocateHsShaderFromMemory(Impl::GraphicsLoaderImpl *loader,
													  sce::Gnmx::HsShader *& hsShader, 
													  MemBlock &memBlock, 
													  const void *sbFileContents, uint32_t sbFileSizeInBytes);

				int loadAndAllocateEsShaderFromMemory(Impl::GraphicsLoaderImpl *loader,
													  sce::Gnmx::EsShader *&esShader, MemBlock &memBlock, const void *sbFileContents, uint32_t sbFileSizeInBytes);

				int loadAndAllocateGsShaderFromMemory(Impl::GraphicsLoaderImpl *loader,
													  sce::Gnmx::GsShader *&gsShader, MemBlock &gsMemBlock, 
													  MemBlock &vsMemBlock, const void *sbFileContents, uint32_t sbFileSizeInBytes);

				////////////////////////////////////////////////////////////////////////////////
				//PSSL LINKAGE (with metadata)
				void ReleaseShaderProgram(sce::Shader::Binary::Program *sbp);


			};
		}
	}
}


#endif // _SCE_SAMPLE_UTIL_GRAPHICS_HEAP_H
#endif // defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
