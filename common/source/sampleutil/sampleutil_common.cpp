/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <sampleutil/sampleutil_common.h>
#include "sampleutil_internal.h"
#include <scebase_common.h>
#include <util/util.h>
#include <sce_atomic.h>

#if _SCE_TARGET_OS_PSP2
#include <kernel.h>
#endif

#if _SCE_TARGET_OS_WINDOWS
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

#if _SCE_TARGET_OS_ORBIS
#include <stdarg.h>
#endif

namespace ssi = sce::SampleUtil::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;

static void vdebugPrintfln(SCE_SAMPLE_LPCTSTR format, va_list ap)
{
	SCE_SAMPLE_TCHAR buf[1024];
	VSNPRINTF(buf, 1024, format, ap);
	buf[1023] = 0;
#if _SCE_TARGET_OS_WINDOWS
	OutputDebugString(buf);
#endif
	printf("%s", buf);
}

void ssi::debugPrintfln(SCE_SAMPLE_LPCTSTR format, ...)
{
	va_list ap;
	va_start(ap, format);
	vdebugPrintfln(format, ap);
	va_end(ap);
}


void sce::SampleUtil::Internal::Assert(const char* file, int line, bool test, const char* format, ...)
{
	if(!test){
		SCE_SAMPLE_TCHAR buf[1024];
		SNTPRINTF(buf, 1024, _TEXT("Sample Util ASSERT, FILE:%s, LINE:%d, %s"), file, line, format);
		buf[1023] = 0;
		va_list argList;
		va_start(argList, format);
		vdebugPrintfln(buf, argList);
		va_end(argList);
#ifdef SCE_SAMPLE_UTIL_RELEASE
   #ifdef __psp2__
		while(1){
			sceKernelDelayThread(1000*1000*100);
		}
   #elif _SCE_TARGET_OS_ORBIS
		while(1){
			sceKernelUsleep(1000*1000*100);
		}
   #else 
		abort();
   #endif /* __psp2__ */
#else /* SCE_SAMPLE_UTIL_RELEASE */
			abort();
#endif /* SCE_SAMPLE_UTIL_RELEASE */
	}

}


void sce::SampleUtil::Internal::AssertEqual32(const char* file, int line, int32_t value, int32_t expected, const char* format, ...)
{
	if(value != expected){
		SCE_SAMPLE_TCHAR buf[1024];
		SNTPRINTF(buf, 1024, _TEXT("Sample Util ASSERT, FILE:%s, LINE:%d, %s:  Actual value is %#x, expected value is %#x"), file, line, format, value, expected);
		buf[1023] = 0;
		va_list argList;
		va_start(argList, format);
		vdebugPrintfln(buf, argList);
		va_end(argList);
#ifdef SCE_SAMPLE_UTIL_RELEASE
   #ifdef __psp2__
		while(1){
			sceKernelDelayThread(1000*1000*100);
		}
   #elif _SCE_TARGET_OS_ORBIS
		while(1){
			sceKernelUsleep(1000*1000*100);
		}
   #else 
		abort();
   #endif /* __psp2__ */
#else /* SCE_SAMPLE_UTIL_RELEASE */
		abort();
#endif /* SCE_SAMPLE_UTIL_RELEASE */
	}

}


static volatile int32_t gResourceIndex = 0;


#ifdef ENABLE_REFCOUNT_CHECK
static ssiu::Mutex g_refCountLock("RefcountLock");
static uint32_t gResourceCount = 0;

std::vector<ssi::ResourceImpl*> g_resources;

static void checkCount(void)
{
	std::vector<ssi::ResourceImpl*>::iterator it;
	uint32_t count = 0;

	for(uint32_t i=0; i<g_resources.size(); i++){
		ssi::ResourceImpl* impl = g_resources.at(i);
		count += impl->getCount();
	}
	if(count != gResourceCount){
		printf("count = %d\n", count);
		printf("gResourceCount = %d\n", gResourceCount);
		for(it = g_resources.begin(); it != g_resources.end(); it++){
			printf("%s : %d\n", (*it)->getResourceType().c_str(), (*it)->getCount());
		}
	}
	SCE_SAMPLE_UTIL_ASSERT(count == gResourceCount);
}


ssi::ResourceImpl::ResourceImpl(void)
{
	ssiu::FocusLock focusLock(&g_refCountLock);
	checkCount();
	m_refCount = 1;
	gResourceCount++;
	m_resourceIndex = gResourceIndex;
	gResourceIndex++;
	g_resources.push_back(this);
	checkCount();
}

uint32_t ssi::ResourceImpl::refer(void)
{
	ssiu::FocusLock focusLock(&g_refCountLock);
	checkCount();
	SCE_SAMPLE_UTIL_ASSERT(m_refCount > 0);
	uint32_t retval = m_refCount;
	m_refCount++;
	gResourceCount++;
	checkCount();
	return retval;
}

uint32_t ssi::ResourceImpl::release(void)
{
	ssiu::FocusLock focusLock(&g_refCountLock);
	checkCount();
	SCE_SAMPLE_UTIL_ASSERT(m_refCount > 0);
	uint32_t retval = m_refCount;
	m_refCount--;
	gResourceCount--;
	checkCount();
	return retval;
}

ssi::ResourceImpl::~ResourceImpl(void)
{
	ssiu::FocusLock focusLock(&g_refCountLock);
	checkCount();
	SCE_SAMPLE_UTIL_ASSERT(m_refCount <= 1);
	gResourceCount -= m_refCount;

	std::vector<ResourceImpl*>::iterator it;
	bool found = false;
	for(it = g_resources.begin(); it != g_resources.end(); it++){
		if(*it == this){
			g_resources.erase(it);
			found = true;
			break;
		}
	}
	SCE_SAMPLE_UTIL_ASSERT(found);
	checkCount();
}
#else
ssi::ResourceImpl::ResourceImpl(void)
{
	m_refCount = 1;
	m_resourceIndex = sceAtomicIncrement32(&gResourceIndex);
	memBarrier();
}

#endif

int sce::SampleUtil::destroy(sce::SampleUtil::Resource *resource)
{
	ssi::ResourceImpl* impl = dynamic_cast<ssi::ResourceImpl*>(resource);
	if(impl == NULL){
		//ssi::debugPrintfln(__FILE__"(%d) Cast to ssi::ResourceImpl failed ", __LINE__);
		//_SCE_BREAK();
		//SCE_SAMPLE_UTIL_ASSERT(0);
		delete resource;
		return SCE_OK;
	}else{
#ifdef ENABLE_REFCOUNT_CHECK
		g_refCountLock.lock();
		uint32_t oldCount = impl->release();
		checkCount();
		g_refCountLock.unlock();
#else
		uint32_t oldCount = impl->release();
#endif

		if(oldCount <= 1){
			delete resource;
		}
		return SCE_OK;
	}
}


#ifdef ENABLE_REFCOUNT_CHECK

void dumpResources(void)
{
	ssiu::FocusLock focusLock(&g_refCountLock);
	checkCount();
	ssi::debugPrintfln("[ Resources ] gResourceCount=%d\n", gResourceCount);
	std::vector<ssi::ResourceImpl*>::iterator it;
	for(it = g_resources.begin(); it != g_resources.end(); it++){
		std::string name = (*it)->getResourceType();
		ssi::debugPrintfln("%s [idx=%d]\n", name.c_str(), (*it)->getIndex());
	}
	SCE_SAMPLE_UTIL_ASSERT(gResourceCount == 0);
}
#endif

