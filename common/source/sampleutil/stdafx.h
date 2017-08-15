/*
 * SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */
// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include <scebase_common.h>

#if _SCE_TARGET_OS_WINDOWS

#define NOMINMAX 

//#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>



#include <stdio.h>
#include <stdlib.h>
// operator new must return 16-byte aligned buffer, because of vectormath.


#ifdef __cplusplus
#include <new>

inline void* operator new (size_t size)
{
	void *p;
	p = _aligned_malloc(size, 16);
	if (p==0) { throw std::bad_alloc(); } // for ANSI/ISO compliant
	return p;
}

inline void* operator new[] (size_t size)
{
	void *p;
	p = _aligned_malloc(size, 16);
	if (p==0) { throw std::bad_alloc(); } // for ANSI/ISO compliant
	//printf("align new %#x\n", (uint32_t)p);
	return p;
}

inline void operator delete (void *p)
{
	_aligned_free(p);
}

inline void operator delete[] (void *p)
{
	//printf("align delete %#x\n", (uint32_t)p);
	_aligned_free(p);
}
#endif /* __cplusplus */

#endif /* _SCE_TARGET_OS_WINDOWS */


#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vectormath.h>
#include <vector>

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
