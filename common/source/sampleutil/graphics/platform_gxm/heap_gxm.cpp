/*                 -*- mode: c; tab-width: 4; indent-tabs-mode: t; -*- */
/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 *
 *      Copyright (C) 2012 Sony Interactive Entertainment Inc.
 *                        All Rights Reserved.
 *
 */

#ifdef __psp2__

#include "stdafx.h"
#include <string.h>
#include <mspace.h>
#include <sampleutil/sampleutil_common.h>
#include <sampleutil/sampleutil_error.h>
#include "heap_gxm.h"
#include <kernel.h>
#include <sceerror.h>
#include <gxm.h>

using namespace sce::SampleUtil::Graphics::Impl;

static inline unsigned int ceil(uint32_t v, uint32_t align)
{
	return ((v+ (align-1)) / align ) * align;
}


int Heap::Block::initialize(const std::string &name,
							uint32_t size, 
							SceKernelMemBlockType memblockType,
							bool mapGpu,
							bool usseMemory)
{
	int ret;
	m_size = size;
	m_uid = sceKernelAllocMemBlock(name.c_str(), memblockType, size, SCE_NULL);
	if (m_uid < 0) {
		return m_uid;
	}
	ret = sceKernelGetMemBlockBase(m_uid, &m_base);
	if (ret < 0) {
		sceKernelFreeMemBlock(m_uid);
		return ret;
	}

	m_mapGpu = mapGpu;
	if(mapGpu){
		ret = sceGxmMapMemory(m_base, m_size, SCE_GXM_MEMORY_ATTRIB_READ |  SCE_GXM_MEMORY_ATTRIB_WRITE );
		if (ret < 0) {
			sceKernelFreeMemBlock(m_uid);
			return ret;
		}
	}

	m_usse = usseMemory;
	if(usseMemory){
		ret = sceGxmMapVertexUsseMemory(m_base, m_size, &m_vertexUsseOffset);
		if(ret != SCE_OK){
			sceGxmUnmapMemory(m_base);
			sceKernelFreeMemBlock(m_uid);
			return ret;
		}
		ret = sceGxmMapFragmentUsseMemory(m_base, m_size, &m_fragmentUsseOffset);
		if(ret != SCE_OK){
			sceGxmUnmapVertexUsseMemory(m_base);
			sceGxmUnmapMemory(m_base);
			sceKernelFreeMemBlock(m_uid);
			return ret;
		}
	}

	m_mspace = mspace_create(m_base, size);

	return SCE_OK;
}


int Heap::Block::finalize(void)
{
	int ret;
	if(m_mspace != NULL){
		mspace_destroy(m_mspace);
		m_mspace = NULL;
	}

	if(m_usse){
		ret = sceGxmUnmapFragmentUsseMemory(m_base);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		ret = sceGxmUnmapVertexUsseMemory(m_base);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_usse = false;
	}

	if(m_mapGpu){
		ret = sceGxmUnmapMemory(m_base);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_mapGpu = false;
	}
	if(m_uid != SCE_UID_INVALID_UID){
		sceKernelFreeMemBlock(m_uid);
		m_uid = SCE_UID_INVALID_UID;
	}
	(void)ret;
	return SCE_OK;
}

void *Heap::Block::malloc(uint32_t size)
{
	return mspace_malloc(m_mspace, size);
}
void *Heap::Block::memalign(uint32_t align, uint32_t size)
{
	return mspace_memalign(m_mspace, align, size);
}

bool Heap::Block::isInside(void *ptr) const
{
	uintptr_t uiptr = (uintptr_t)ptr;
	uintptr_t uibase = (uintptr_t)m_base;
	if(uiptr < uibase){
		return false;
	}
	if(uiptr >= (uibase+m_size)){
		return false;
	}
	return true;
}

void Heap::Block::free(void *ptr)
{
	mspace_free(m_mspace, ptr);
}

uint32_t Heap::Block::convertToVertexUsseOffset(void *ptr) const
{
	if((!m_usse) || !isInside(ptr)){
		return 0;
	}
	return m_vertexUsseOffset + ((uintptr_t)ptr) - ((uintptr_t)m_base);
}
uint32_t Heap::Block::convertToFragmentUsseOffset(void *ptr) const
{
	if((!m_usse) || !isInside(ptr)){
		return 0;
	}
	return m_fragmentUsseOffset + ((uintptr_t)ptr) - ((uintptr_t)m_base);
}


//E Create heap memory
int Heap::initialize(const char *name, uint32_t initSize, 
					 SceKernelMemBlockType memblockType,
					 bool mapGpu,
					 bool usseMemory,
					 int flags)
{
	int ret;
	if(m_mutex != NULL){
		finalize();
	}

	if(name == NULL){
		return -1;
	}

	m_minBlockSize = 0;


	if(memblockType == SCE_KERNEL_MEMBLOCK_TYPE_USER_RW){
		m_minBlockSize = 4096;
	}else if(memblockType == SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE){
		m_minBlockSize = 4096;
	}else if(memblockType == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW){
		m_minBlockSize = 1024*1024;
	}else{
		return -1;
	}

	initSize = ceil(initSize, m_minBlockSize);

	if(mapGpu && usseMemory){
		return -1;
	}

	m_name = name;
	m_memblockType = memblockType;
	m_mapGpu = mapGpu;
	m_usse = usseMemory;
	m_flags = flags;

	m_mutex = new SceKernelLwMutexWork;

	ret = sceKernelCreateLwMutex(m_mutex, name, SCE_KERNEL_ATTR_TH_PRIO, 0, SCE_NULL);
	if (ret < 0) {
		return ret;
	}


	Block initBlock;
	ret = initBlock.initialize(m_name, initSize, m_memblockType, m_mapGpu, m_usse);
	if (ret < 0) {
		return ret;
	}
	m_blocks.push_back(initBlock);

	m_usage.totalSize = 0;
	m_usage.maxTotalSize   = 0;
	m_usage.memblockTotalSize = initSize;

	return SCE_OK;
}



//E Delete heap memory
int Heap::finalize(void)
{
	if(m_mutex == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	lock();

	for(uint32_t i=0; i<m_blocks.size(); i++){
		m_blocks.at(i).finalize();
	}
	m_blocks.clear();

	unlock();

	delete m_mutex; 
	m_mutex = NULL;
	return SCE_OK;
}

void Heap::lock(void) const
{
	int ret = sceKernelLockLwMutex(m_mutex, 1, SCE_NULL);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}
void Heap::unlock(void) const
{
	int ret = sceKernelUnlockLwMutex(m_mutex, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}


/*E Allocate memory from heap memory */
void *Heap::malloc(uint32_t size)
{
	if(m_mutex == NULL){
		return NULL;
	}

	lock();

	void *p = NULL;
	for(uint32_t i=0; i<m_blocks.size(); i++){
		p = m_blocks.at(i).malloc(size);
		if(p != NULL){
			break;
		}
	}

	if(p != NULL){
		unlock();
		return p;
	}

	if((m_flags & FLAG_AUTO_EXTEND) == 0){
		unlock();
		return NULL;
	}

	uint32_t bsize = ceil(size*2, m_minBlockSize);

	Block newBlock;
	int ret = newBlock.initialize(m_name, bsize, m_memblockType, m_mapGpu, m_usse);
	if (ret < 0) {
		unlock();
		return NULL;
	}
	m_blocks.push_back(newBlock);
	p = newBlock.malloc(size);

	m_usage.memblockTotalSize += bsize;

	unlock();

	return p;

}
void *Heap::memalign(uint32_t align, uint32_t size)
{
	if(m_mutex == NULL){
		return NULL;
	}

	lock();

	void *p = NULL;
	for(uint32_t i=0; i<m_blocks.size(); i++){
		p = m_blocks.at(i).memalign(align, size);
		if(p != NULL){
			break;
		}
	}

	if(p != NULL){
		unlock();
		return p;
	}

	if((m_flags & FLAG_AUTO_EXTEND) == 0){
		unlock();
		return NULL;
	}

	uint32_t bsize = ceil(size*2, m_minBlockSize);

	Block newBlock;
	int ret = newBlock.initialize(m_name, bsize, m_memblockType, m_mapGpu, m_usse);
	if (ret < 0) {
		unlock();
		return NULL;
	}

	m_blocks.push_back(newBlock);
	p = newBlock.memalign(align, size);

	m_usage.memblockTotalSize += bsize;

	unlock();

	return p;

}


/*E Release memory back to heap memory */
void Heap::free(void *ptr)
{
	if(m_mutex == NULL){
		return;
	}

	lock();
	for(uint32_t i=0; i<m_blocks.size(); i++){
		if(m_blocks.at(i).isInside(ptr)){
			m_blocks.at(i).free(ptr);
			unlock();
			return;
		}
	}
	abort();
	unlock();

}

bool Heap::isInside(void *ptr) const
{
	if(m_mutex == NULL){
		return false;
	}

	lock();
	for(uint32_t i=0; i<m_blocks.size(); i++){
		if(m_blocks.at(i).isInside(ptr)){
			unlock();
			return true;
		}
	}
	unlock();
	return false;
}
 
uint32_t Heap::convertToVertexUsseOffset(void *ptr) const
{
	if(m_mutex == NULL){
		return 0;
	}

	lock();
	for(uint32_t i=0; i<m_blocks.size(); i++){
		if(m_blocks.at(i).isInside(ptr)){
			uint32_t ret = m_blocks.at(i).convertToVertexUsseOffset(ptr);
			unlock();
			return ret;
		}
	}
	unlock();
	return 0;
}
uint32_t Heap::convertToFragmentUsseOffset(void *ptr) const
{
	if(m_mutex == NULL){
		return 0;
	}

	lock();
	for(uint32_t i=0; i<m_blocks.size(); i++){
		if(m_blocks.at(i).isInside(ptr)){
			uint32_t ret = m_blocks.at(i).convertToFragmentUsseOffset(ptr);
			unlock();
			return ret;
		}
	}
	unlock();
	return 0;

}

void Heap::getUsage(Usage *usage)
{
	if(m_mutex == NULL){
		return;
	}

	if(usage == NULL){
		return;
	}
	lock();
	usage->totalSize = 0;
	usage->maxTotalSize = 0;
	usage->memblockTotalSize = m_usage.memblockTotalSize;

	for(uint32_t i=0; i<m_blocks.size(); i++){
		malloc_managed_size mmsize;
		int ret = mspace_malloc_stats(m_blocks.at(i).m_mspace, &mmsize);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		usage->totalSize    += mmsize.current_inuse_size;
		usage->maxTotalSize += mmsize.max_inuse_size;
	}
	unlock();
}


#endif /* __psp2__ */