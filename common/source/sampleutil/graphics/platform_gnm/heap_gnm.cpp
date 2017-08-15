/*                 -*- mode: c; tab-width: 4; indent-tabs-mode: t; -*- */
/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 *
 *      Copyright (C) 2012 Sony Interactive Entertainment Inc.
 *                        All Rights Reserved.
 *
 */
#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#include <sampleutil/graphics.h>
#include <gnm.h>
#include <scebase.h>
#include <scebase_common.h>
#include <stdlib.h>
#include <string.h>
#define GNM_MEMORY_STORE_INHIB_MACRO
#include <gnm/gpumem.h>
#include <gnm/platform.h>
#include <stdio.h>
#include <mspace.h>
#include <vector>
#include "heap_gnm.h"

namespace ssgi = sce::SampleUtil::Graphics::Impl;

ssgi::Heap::Heap(void)
{
	m_systemHeap = NULL;
	m_videoSharedHeap = NULL;
}
ssgi::Heap::~Heap(void)
{
	finalize();
}



int ssgi::Heap::initialize2(
						   void *system,	    uint32_t systemSize,
						   void *gpuShared, 	uint32_t gpuSharedSize)
{
	if(systemSize)	{
		m_systemHeap = sceLibcMspaceCreate("sampleUtilSys", system, systemSize, 0);
	}

	if(gpuSharedSize)	{
		m_videoSharedHeap = sceLibcMspaceCreate("sampleUtilVideoShared", gpuShared, gpuSharedSize, 0);
	}

	m_system        = system;
	m_systemSize    = systemSize;
	m_gpuShared     = gpuShared;
	m_gpuSharedSize = gpuSharedSize;

	return SCE_OK;

}



int ssgi::Heap::finalize(void)
{
	if(m_systemHeap != NULL){
		sceLibcMspaceDestroy(m_systemHeap);
		m_systemHeap = NULL;
	}
	if(m_videoSharedHeap != NULL){
		sceLibcMspaceDestroy(m_videoSharedHeap);
		m_videoSharedHeap = NULL;
	}
	return SCE_OK;
}


void *ssgi::Heap::allocateSystemShared(size_t alignment, size_t size)
{
	m_mutex.lock();
	_mm_sfence(); //callins sfence since heap may reside on WC_GARLIC.
	void *ptr = sceLibcMspaceMemalign(m_systemHeap, alignment, size);
	SCE_SAMPLE_UTIL_ASSERT(isSystemMem(ptr));
	_mm_sfence();
	m_mutex.unlock();
	return ptr;
}
void ssgi::Heap::releaseSystemShared(void *ptr)
{
	m_mutex.lock();
	_mm_sfence();
	sceLibcMspaceFree(m_systemHeap, ptr);
	_mm_sfence();
	m_mutex.unlock();
}

static uint32_t largeCount = 0;

void *ssgi::Heap::allocateVideoShared(size_t alignment, size_t size)
{
	m_mutex.lock();
	_mm_sfence();
	void *ret = sceLibcMspaceMemalign(m_videoSharedHeap, alignment, size);
	SCE_SAMPLE_UTIL_ASSERT(isGpuSharedMem(ret));
	_mm_sfence();
	m_mutex.unlock();
	return ret;

}

void ssgi::Heap::dump(void)
{
	int ret;
	SceLibcMallocManagedSize sysMsize;
	SCE_LIBC_INIT_MALLOC_MANAGED_SIZE(sysMsize);
	ret = sceLibcMspaceMallocStats(m_systemHeap, &sysMsize);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SceLibcMallocManagedSize videoMsize;
	SCE_LIBC_INIT_MALLOC_MANAGED_SIZE(videoMsize);
	ret = sceLibcMspaceMallocStats(m_videoSharedHeap, &videoMsize);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	printf("[mspace, dump %d] %#x, %#x\n", largeCount, (uint32_t)sysMsize.currentInuseSize, (uint32_t)videoMsize.currentInuseSize);
}


void ssgi::Heap::releaseVideoShared(void *ptr)
{
	m_mutex.lock();
	_mm_sfence();
	sceLibcMspaceFree(m_videoSharedHeap, ptr);
	_mm_sfence();
	m_mutex.unlock();
}


uint64_t ssgi::Heap::allocateSystemMemoryBlock(ssgi::MemBlock *pOutput, uint32_t size, sce::Gnm::AlignmentType align)
{
	SCE_GNM_ASSERT(pOutput);
	pOutput->m_ptr = static_cast<uint32_t*>(this->allocateSystemShared(align, size));
	SCE_GNM_ASSERT(pOutput->m_ptr);
	pOutput->m_addr = pOutput->m_ptr ? (uintptr_t)pOutput->m_ptr : 0;
	pOutput->m_size     = pOutput->m_ptr ? size : 0;
	return pOutput->m_addr;
}

uint64_t ssgi::Heap::allocateVideoMemoryBlock(ssgi::MemBlock *pOutput, uint32_t size, sce::Gnm::AlignmentType align)
{
	SCE_GNM_ASSERT(pOutput);
	pOutput->m_ptr = static_cast<uint32_t*>(this->allocateVideoShared(align, size));
	SCE_GNM_ASSERT(pOutput->m_ptr);
	pOutput->m_addr = pOutput->m_ptr ? (uintptr_t)pOutput->m_ptr : 0;
	pOutput->m_size     = pOutput->m_ptr ? size : 0;
	return pOutput->m_addr;
}

void ssgi::Heap::releaseMemoryBlock(ssgi::MemBlock *pBlock)
{
	if (pBlock == NULL || pBlock->m_ptr == 0){
		return;
	}
	bool free = false;
	if(this->isSystemMem(pBlock->m_ptr)){
		this->releaseSystemShared(pBlock->m_ptr);
		free = true;
	}
	if(this->isGpuSharedMem(pBlock->m_ptr)){
		this->releaseVideoShared(pBlock->m_ptr);
		free = true;
	}

	pBlock->m_ptr	 = 0;
	pBlock->m_addr = 0;
	pBlock->m_size     = 0;
	SCE_SAMPLE_UTIL_ASSERT(free);
}

#endif //_SCE_TARGET_OS_ORBIS
