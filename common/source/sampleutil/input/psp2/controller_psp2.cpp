/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#if _SCE_TARGET_OS_PSP2

#include "../controller_common.h"

namespace ss = sce::SampleUtil;
namespace ssi = sce::SampleUtil::Input;
namespace ssii = sce::SampleUtil::Input::Impl;

int ssii::ControllerBase::readBufferPositive(ControllerData *ctrlData, uint32_t numBufs)
{
	SceCtrlData* vitaCtrlData = (SceCtrlData*)alloca(sizeof(SceCtrlData) * numBufs);
	int returnBufs = sceCtrlReadBufferPositive(0, vitaCtrlData, numBufs);
	if (returnBufs < 0) {
		return returnBufs;
	}
	for (uint32_t i=0; i<returnBufs; i++) {
		ctrlData[i].timeStamp = vitaCtrlData[i].timeStamp;
		ctrlData[i].buttons   = vitaCtrlData[i].buttons;
		ctrlData[i].lx        = vitaCtrlData[i].lx;
		ctrlData[i].ly        = vitaCtrlData[i].ly;
		ctrlData[i].rx        = vitaCtrlData[i].rx;
		ctrlData[i].ry        = vitaCtrlData[i].ry;
	}
	
	SCE_SAMPLE_UTIL_ASSERT_MSG(returnBufs >= SCE_OK, "sceCtrlReadBufferPositive() failed with: 0x%x\n", returnBufs);
	return returnBufs;
}

int ssii::ControllerBase::initialize(void)
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG);
	return SCE_OK;
}

int ssii::ControllerBase::finalize(void)
{
	return SCE_OK;
}

#endif // _SCE_TARGET_OS_PSP2
