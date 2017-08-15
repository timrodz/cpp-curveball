/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2


#include "ngs_internal.h"

namespace ssai = sce::SampleUtil::Audio::Internal;

int sce::SampleUtil::Audio::Internal::setPatchVolume(SceNgsHPatch patch, const Volume *volInfo)
{
	Volume v = {1.0f, 1.0f };
	if(volInfo == NULL){
		volInfo = &v;
	}

	// Set volumes on patch
	SceNgsPatchRouteInfo patchRouteInfo;
	int returnCode = sceNgsPatchGetInfo( patch, &patchRouteInfo, NULL );
	if( returnCode != SCE_NGS_OK )
	{
		fprintf(stderr, "connectRacks: sceNgsPatchGetInfo() failed: 0x%08X\n", returnCode );
		return returnCode;
	}
	if( patchRouteInfo.nOutputChannels == 1 ){
		if( patchRouteInfo.nInputChannels == 1 ){
			patchRouteInfo.vols.m[0][0] = volInfo->left; // left to left
		}else{
			patchRouteInfo.vols.m[0][0] = volInfo->left; // left to left
			patchRouteInfo.vols.m[0][1] = volInfo->right; // left to right
		}
	}else{
		if( patchRouteInfo.nInputChannels == 1 ){
			patchRouteInfo.vols.m[0][0] = volInfo->left; // left to left
			patchRouteInfo.vols.m[1][0] = volInfo->right; // right to l
		}else{
			patchRouteInfo.vols.m[0][0] = volInfo->left; // left to left
			patchRouteInfo.vols.m[0][1] = 0.0f; // left to right
			patchRouteInfo.vols.m[1][0] = 0.0f; // right to l
			patchRouteInfo.vols.m[1][1] = volInfo->right; // right to r
		}
	}

	returnCode = sceNgsVoicePatchSetVolumesMatrix( patch, &patchRouteInfo.vols );
	if( returnCode != SCE_NGS_OK )
	{

		printf( "connectRacks: sceNgsVoicePatchSetVolumesMatrix() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	return SCE_OK;

}

int sce::SampleUtil::Audio::Internal::connectRacks(
												   SceNgsHVoice hVoiceSource, 
												   unsigned int nSourceOutputIndex, 
												   SceNgsHVoice hVoiceDestination, 
												   unsigned int nTargetInputIndex, 
												   SceNgsHPatch* pPatch)
{
	int                  returnCode;

	// Create patch
	SceNgsPatchSetupInfo patchInfo;
	patchInfo.hVoiceSource           = hVoiceSource;
	patchInfo.nSourceOutputIndex     = nSourceOutputIndex;
	patchInfo.nSourceOutputSubIndex  = SCE_NGS_VOICE_PATCH_AUTO_SUBINDEX;
	patchInfo.hVoiceDestination      = hVoiceDestination;
	patchInfo.nTargetInputIndex      = nTargetInputIndex;

	SceNgsHPatch         patch;
	returnCode = sceNgsPatchCreateRouting( &patchInfo, &patch );
	if( returnCode != SCE_NGS_OK ){
		fprintf(stderr,  "connectRacks: sceNgsPatchCreateRouting() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	returnCode = setPatchVolume(patch, NULL);
	if( returnCode != SCE_NGS_OK ){
		fprintf(stderr, "connectRacks: sceNgsVoicePatchSetVolumesMatrix() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	*pPatch = patch;

	return SCE_OK;
}





#endif /* _SCE_TARGET_OS_PSP2 */