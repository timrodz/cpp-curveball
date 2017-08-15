/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */



#ifndef _SCE_SAMPLE_UTI_SAMPLE_UTILITIES_CPP_INTERNAL_H
#define _SCE_SAMPLE_UTI_SAMPLE_UTILITIES_CPP_INTERNAL_H

#include <stdint.h>
#include <string>
#include <sampleutil/sampleutil_common.h>
#include <sce_atomic.h>

#if _SCE_TARGET_OS_PSP2
#endif

#if _SCE_TARGET_OS_WINDOWS
#endif

#if _SCE_TARGET_OS_ORBIS
#include <x86intrin.h>
#endif

#if _SCE_TARGET_OS_WINDOWS
#pragma comment(lib,"tinyxml")
#elif _SCE_TARGET_OS_ORBIS
//#pragma comment(lib,"tinyxml")
#elif _SCE_TARGET_OS_PSP2
#pragma comment(lib,"tinyxml")
#endif 


#define METHOD_FORWARD_VOID_1(foward, f, t1, a1)  void f(t1 a1) { foward.f(a1); }
#define METHOD_FORWARD_VOID_2(foward, f, t1, a1, t2, a2)  void f(t1 a1, t2 a2) { foward.f(a1, a2); }
#define METHOD_FORWARD_VOID_4(foward, f, t1, a1, t2, a2, t3, a3, t4, a4)  void f(t1 a1, t2 a2, t3 a3, t4 a4) { foward.f(a1, a2, a3, a4); }
#define METHOD_FORWARD_VOID_6(foward, f, t1, a1, t2, a2, t3, a3, t4, a4, t5, a5, t6, a6)  void f(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6) { foward.f(a1, a2, a3, a4, a5, a6); }
#define METHOD_FORWARD_0(foward, ret, f)  ret f(void) { return foward.f(); }
#define METHOD_FORWARD_0_CONST(foward, ret, f)  ret f(void) const { return foward.f(); }
#define METHOD_FORWARD_1(foward, ret, f, t1, a1)  ret f(t1 a1) { return foward.f(a1); }
#define METHOD_FORWARD_1_D1(foward, ret, f, t1, a1, d1)  ret f(t1 a1=d1) { return foward.f(a1); }
#define METHOD_FORWARD_2(foward, ret, f, t1, a1, t2,a2)  ret f(t1 a1, t2 a2) { return foward.f(a1,a2); }
#define METHOD_FORWARD_3(foward, ret, f, t1, a1, t2,a2, t3,a3)  ret f(t1 a1, t2 a2, t3 a3) { return foward.f(a1,a2, a3); }
#define METHOD_FORWARD_4(foward, ret, f, t1, a1, t2,a2, t3,a3, t4,a4)  ret f(t1 a1, t2 a2, t3 a3, t4 a4) { return foward.f(a1,a2, a3, a4); }
#define METHOD_FORWARD_5(foward, ret, f, t1, a1, t2,a2, t3,a3, t4,a4, t5,a5)  ret f(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5) { return foward.f(a1,a2, a3, a4, a5); }
#define METHOD_FORWARD_6(foward, ret, f, t1, a1, t2,a2, t3,a3, t4,a4, t5,a5, t6,a6)  ret f(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6) { return foward.f(a1,a2, a3, a4, a5, a6); }
#define METHOD_FORWARD_6_D1(foward, ret, f, t1, a1, t2,a2, t3,a3, t4,a4, t5,a5, t6,a6,d6)  ret f(t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6=d6) { return foward.f(a1,a2, a3, a4, a5, a6); }


//#define ENABLE_REFCOUNT_CHECK 1

namespace sce
{
	namespace SampleUtil
	{
		namespace Impl
		{
			class ResourceImpl : public Resource
			{
			private:
				volatile int32_t m_refCount;
				int32_t m_resourceIndex;
				void memBarrier(void)
				{
#if _SCE_TARGET_OS_WINDOWS
					_mm_mfence();
#endif
#if _SCE_TARGET_OS_PSP2
					__builtin_dmb();
#endif
#if _SCE_TARGET_OS_ORBIS
					_mm_mfence();
#endif
				}
			protected:
				ResourceImpl(void);
			public:
#ifdef ENABLE_REFCOUNT_CHECK
				uint32_t refer(void);
				uint32_t release(void);
				virtual ~ResourceImpl(void);
#else
				inline uint32_t refer(void)
				{
					uint32_t ret = (uint32_t)sceAtomicIncrement32(&m_refCount);
					memBarrier();
					return ret;
				}
				inline uint32_t release(void)
				{
					memBarrier();
					return (uint32_t)sceAtomicDecrement32(&m_refCount);
				}
				inline virtual ~ResourceImpl(void)
				{
					SCE_SAMPLE_UTIL_ASSERT(m_refCount <= 1);
				}

#endif
				virtual std::string getResourceType(void) const = 0;
				
				uint32_t getIndex(void) const
				{ 
					return (uint32_t)m_resourceIndex; 
				}
				uint32_t getCount(void) const
				{
					return (uint32_t)m_refCount;
				}
			};

			void debugPrintfln(SCE_SAMPLE_LPCTSTR format, ...);

			static inline int destroyImpl(ResourceImpl *resourceImpl)
			{
				uint32_t oldCount = resourceImpl->release();
				if(oldCount <= 1){
					delete resourceImpl;
				}
				return SCE_OK;
			}

		}
	}
}

#if _SCE_TARGET_OS_WINDOWS
  #ifdef  _UNICODE
    #define SNTPRINTF _snwprintf
  #else
    #define SNTPRINTF _snprintf
  #endif
  #define FOPEN _tfopen
  #define VSNPRINTF _vsntprintf
#else
  #define SNTPRINTF snprintf
  #define FOPEN fopen
  #define VSNPRINTF vsnprintf
  #define _TEXT(a) a
#endif

#endif /* _SCE_SAMPLE_UTI_SAMPLE_UTILITIES_CPP_INTERNAL_H */

