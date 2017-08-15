/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"
#include <scebase_common.h>


#if _SCE_TARGET_OS_WINDOWS


#define NOMINMAX 
#include "stdafx.h"
#include <windows.h>
#include <graphics/collada/collada_internal.h>
#include "resource/resource_manager.h"

static HINSTANCE gModuleId = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			// printf may not be available at this point.
			gModuleId = hModule;
			sce::SampleUtil::Internal::Resource::initialize(gModuleId);
			break;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


#endif /* __psp2__ */