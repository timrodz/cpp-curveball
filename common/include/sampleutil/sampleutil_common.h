/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */



#ifndef _SCE_SAMPLE_UTIL_COMMON_H
#define _SCE_SAMPLE_UTIL_COMMON_H


#include <scebase_common.h>


#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __ORBIS__
   #define SCE_SAMPLE_UTIL_API
	#include <hash_map>

#elif defined(__psp2__)
   #ifdef SCE_SAMPLE_UTIL_PRX_BUILD
      #ifdef SCE_SAMPLE_UTIL_BUILD
         #define SCE_SAMPLE_UTIL_API  __declspec(dllexport)
      #else
         #define SCE_SAMPLE_UTIL_API  __declspec(dllimport)
      #endif

      /* warning suppress for __declspec */
      #pragma diag_suppress=1403
      #pragma diag_suppress=1409

   #else
      #define SCE_SAMPLE_UTIL_API 
   #endif /* SCE_SAMPLE_UTIL_PRX_BUILD */
	#include <hash_map>
#else /* __psp2__ */
   #ifdef SCE_SAMPLE_UTIL_BUILD
      #define SCE_SAMPLE_UTIL_API  __declspec(dllexport)
   #else
      #define SCE_SAMPLE_UTIL_API  __declspec(dllimport)
   #endif 
	#if _SCE_TARGET_OS_WINDOWS && _MSC_VER >= 1900
	#include <unordered_map>
	#else
	#include <hash_map>
	#endif

#endif /* __psp2__ */


namespace sce
{
	/*!
	 * @~English
	 * @brief Sample utility 
	 * @details These are the sample utility definitions. 
	 * @~Japanese
	 * @brief サンプルユーティリティ 
	 * @details サンプルユーティリティの定義です。 
	 */
	namespace SampleUtil
	{
		namespace Internal
		{
			SCE_SAMPLE_UTIL_API void Assert(const char* file, int line, bool test, const char* format, ...);
			SCE_SAMPLE_UTIL_API void AssertEqual32(const char* file, int line, int32_t value, int32_t expected, const char* format, ...);

		}

		/*!
		 * @~English
		 * @brief Resource base class 
		 * @details This is the base class for resources generated within the library. 
		 * @~Japanese
		 * @brief リソースの基底クラス 
		 * @details ライブラリ内で生成されるリソースの基底クラスです。 
		 */
		class SCE_SAMPLE_UTIL_API Resource 
		{
		public:
			virtual ~Resource(void){}

		};


		/*!
		 * @~English
		 * @brief Discard resources 
		 * @details This discards an instance to free resources. 
		 * @param resource Resource object
		 * @retval SCE_OK Success
		 * @retval (<0) Error code
		 * @~Japanese
		 * @brief リソースの破棄 
		 * @details インスタンスを破棄し、リソースを解放します。 
		 * @param resource リソースオブジェクト
		 * @retval SCE_OK 成功
		 * @retval (<0) エラーコード
		 */
		SCE_SAMPLE_UTIL_API int destroy(Resource *resource);
	}

	namespace SampleUtil
	{
	#if _SCE_TARGET_OS_WINDOWS && _MSC_VER >= 1900
		template<class _Kty, class _Ty, class _Hasher = std::hash<_Kty> >
		struct Map
		{
			typedef	std::unordered_map<_Kty, _Ty, _Hasher> Type;
		};
	#else
		template<class _Kty, class _Ty, class _Tr = std::hash_compare<_Kty, std::less<_Kty> > >
		struct Map
		{
			typedef	std::hash_map<_Kty, _Ty, _Tr> Type;
		};
	#endif
		static inline float fmax(float a, float b)
		{
			return (a>b) ? a : b;
		}
	}
}

#ifndef SCE_SAMPLE_UTIL_ASSERTS_ENABLED
#define SCE_SAMPLE_UTIL_ASSERTS_ENABLED 0
#endif	/* SCE_SAMPLE_UTIL_ASSERTS_ENABLED */

#if SCE_SAMPLE_UTIL_ASSERTS_ENABLED

#define SCE_SAMPLE_UTIL_ASSERT(test)			        sce::SampleUtil::Internal::Assert(__FILE__, __LINE__, (bool)(test), "Assertion failed: %s\n",#test)
#define SCE_SAMPLE_UTIL_ASSERT_MSG(test, msg, ...)	    sce::SampleUtil::Internal::Assert(__FILE__, __LINE__, (bool)(test), msg, ##__VA_ARGS__)
#define SCE_SAMPLE_UTIL_ASSERT_EQUAL(value, expected)	sce::SampleUtil::Internal::AssertEqual32(__FILE__, __LINE__, value, expected, "Assertion failed, values not equal. actual value=%#x, expected value=%#x\n", value, expected)

#else

#define SCE_SAMPLE_UTIL_ASSERT(test)			        
#define SCE_SAMPLE_UTIL_ASSERT_MSG(test, msg, ...)	    
#define SCE_SAMPLE_UTIL_ASSERT_EQUAL(value, expected)	

#endif

//#define SCE_SAMPLE_UTIL_GNM_0820

#if _SCE_TARGET_OS_ORBIS || _SCE_TARGET_OS_WINDOWS
#define _SCE_SAMPLE_UTIL_ENABLE_COMPUTE_SHADER 1
#endif

#if defined(_SCE_TARGET_OS_WINDOWS) &&  _SCE_TARGET_OS_WINDOWS
#define _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS 1
#endif/* defined(_SCE_TARGET_OS_WINDOWS) &&  _SCE_TARGET_OS_WINDOWS */


#if _SCE_TARGET_OS_WINDOWS
#include <tchar.h>
#define SCE_SAMPLE_LPCTSTR LPCTSTR
#define SCE_SAMPLE_LPTSTR LPTSTR
#define SCE_SAMPLE_TCHAR  TCHAR
#else
typedef const char* SCE_SAMPLE_LPCTSTR;
typedef char* SCE_SAMPLE_LPTSTR;
typedef char SCE_SAMPLE_TCHAR;
#endif

#endif // _SCE_SAMPLE_UTIL_COMMON_H_



