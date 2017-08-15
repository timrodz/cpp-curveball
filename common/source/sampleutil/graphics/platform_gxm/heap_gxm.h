/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 *
 *      Copyright (C) 2012 Sony Interactive Entertainment Inc.
 *                        All Rights Reserved.
 */


#ifndef _SCE_SAMPLE_UTIL_CPP_HEAP_H
#define _SCE_SAMPLE_UTIL_CPP_HEAP_H

#ifdef __psp2__

#include <scebase_common.h>
#include <string>
#include <mspace.h>
#include <kernel.h>
#include <vector>

#include <sampleutil/sampleutil_common.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class SCE_SAMPLE_UTIL_API Heap 
		{
		public:
			static const int FLAG_NONE=0;
			static const int FLAG_AUTO_EXTEND=1;

			struct Usage
			{
				uint32_t totalSize;
				uint32_t maxTotalSize;
				uint32_t memblockTotalSize;
			};

			Heap(void) : m_mutex(NULL) {}
			~Heap(void){}

			/*E Create heap memory */
			int initialize(const char *name, uint32_t initSize, 
				            SceKernelMemBlockType memblockType,
                            bool mapGpu=true,
							bool usseMemory=false,
							int flags=FLAG_AUTO_EXTEND);

			/*E Delete heap memory */
			int finalize(void);

			/*E Allocate memory from heap memory */
			void *malloc(uint32_t size);
			void *memalign(uint32_t align, uint32_t size);

			/*E Release memory back to heap memory */
			void free(void *ptr);

			uint32_t convertToVertexUsseOffset(void *ptr) const;
			uint32_t convertToFragmentUsseOffset(void *ptr) const;

			bool isInside(void *ptr) const;

			void getUsage(Usage *usage);


		private:
			struct Block
			{
				SceUID	m_uid;
				void *m_base;
				SceSize	m_size;
				mspace	m_mspace;
				bool m_usse;
				uint32_t m_vertexUsseOffset;
				uint32_t m_fragmentUsseOffset;
				bool m_mapGpu;
				int initialize(const std::string &name,
					           uint32_t size, 
					           SceKernelMemBlockType memblockType,
							   bool mapGpu,
							   bool usseMemory);
				int finalize(void);

				void *malloc(uint32_t size);
				void *memalign(uint32_t align, uint32_t size);
				void free(void *ptr);
				bool isInside(void *ptr) const;

				uint32_t convertToVertexUsseOffset(void *ptr) const;
				uint32_t convertToFragmentUsseOffset(void *ptr) const;
			};

			std::string m_name;
			SceKernelMemBlockType m_memblockType;
			bool m_mapGpu;
			bool m_usse;
			int m_flags;
			uint32_t m_minBlockSize;
			SceKernelLwMutexWork *m_mutex;

			std::vector<Block> m_blocks;

			void lock(void) const;
			void unlock(void) const;

			Usage m_usage;

		};


			}
		}
	}
}




#endif /* __psp2__ */

#endif	/* _SCE_SAMPLE_UTIL_CPP_HEAP_H */


