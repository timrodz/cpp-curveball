/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#if _SCE_TARGET_OS_PSP2

#include <sampleutil/input/controller.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil_internal.h>

#include "../pad_common.h"
/* ------------------------------------------------------------------------- */
namespace ssi  = sce::SampleUtil::Input;
namespace ssii = sce::SampleUtil::Input::Impl;

#include <ctrl.h>
#pragma comment(lib, "SceCtrl_stub")
#pragma comment(lib, "SceMotion_stub")
#include <alloca.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Input
		{
			namespace Impl
			{
				class PadContextForVitaImpl : public PadContextImpl
				{
				public:
					PadContextForVitaImpl(void){ m_motionEnabled = false; return;}
					~PadContextForVitaImpl(){return;}

					/* ------------ Motion Sensor Methods ------------ */
					virtual int enableMotionSensor(bool bEnable);
					virtual int enableMagnetometer(bool bEnable);
					virtual int enableTiltCorrection(bool bEnable);
					virtual int enableAngularVelocityDeadband(bool bEnable);
					virtual int resetOrientation(void);

				private:
					void updateController(PadData *data, uint32_t numPadData);
					void updateTouchDevice(PadData *data, uint32_t numPadData);
					void updateMotion(PadData *data, uint32_t numPadData);

					int initializeController(const PadContextOption* pOption);
					int initializeTouchDevice(const PadContextOption* pOption);
					int initializeMotion(const PadContextOption* pOption);

					int finalizeController(void);
					int finalizeTouchDevice(void);
					int finalizeMotion(void);

					/* ------------ Motion Sensor Local Data ------------ */
					bool m_motionEnabled;
					float m_radiansThreshold;
				};
			}
		}
	}
}


/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::initializeController(const ssi::PadContextOption* pOption)
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITALANALOG);

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextForVitaImpl::updateController(ssi::PadData *data, uint32_t numPadData)
{
	if (data == NULL) {
		return;
	}

	SceCtrlData* ctrlData = (SceCtrlData*)alloca(sizeof(SceCtrlData) * numPadData);
	int returnBufs = sceCtrlReadBufferPositive(0, ctrlData, numPadData);

	for (uint32_t i=0 ; i<numPadData ; ++i) {
		memset(&data[i], 0, sizeof(ssi::PadData));
		data[i].timeStamp = ctrlData[i].timeStamp;
		data[i].connected = true;
		data[i].buttons   = ctrlData[i].buttons;
		data[i].lx        = ctrlData[i].lx;
		data[i].ly        = ctrlData[i].ly;
		data[i].rx        = ctrlData[i].rx;
		data[i].ry        = ctrlData[i].ry;
		data[i].l2        = 0;
		data[i].r2        = 0;
		data[i].intercepted = (ctrlData[i].buttons & SCE_CTRL_INTERCEPTED) != 0;
	}

	return;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::finalizeController(void)
{
	return SCE_OK;
}


/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::initializeTouchDevice(const ssi::PadContextOption* pOption)
{
	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextForVitaImpl::updateTouchDevice(ssi::PadData *data, uint32_t numPadData)
{
	return;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::finalizeTouchDevice(void)
{
	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::initializeMotion(const ssi::PadContextOption* pOption)
{
	this->m_radiansThreshold = sceMotionGetAngleThreshold();

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextForVitaImpl::updateMotion(ssi::PadData *data, uint32_t numPadData)
{
	int ret = SCE_OK;
	(void)ret;

	// Always update this parameter
	this->m_radiansThreshold = sceMotionGetAngleThreshold();

	if (!(this->m_motionEnabled)) {
		return;
	}

	SceMotionState motionState;
	ret = sceMotionGetState(&motionState);
	SCE_SAMPLE_UTIL_ASSERT_MSG((ret == SCE_OK || ret == SCE_MOTION_ERROR_NOT_SAMPLING), 
								"sceMotionMagnetometerOff() failed with: 0x%x\n",
								ret);

	// Orientation / Quaternion
	{
		data[0].motionData.orientation.setW(motionState.deviceQuat.w);
		data[0].motionData.orientation.setX(motionState.deviceQuat.x);
		data[0].motionData.orientation.setY(motionState.deviceQuat.y);
		data[0].motionData.orientation.setZ(motionState.deviceQuat.z);
	}

	// Acceleration / Vector 3D
	{
		data[0].motionData.acceleration.setX(motionState.acceleration.x);
		data[0].motionData.acceleration.setY(motionState.acceleration.y);
		data[0].motionData.acceleration.setZ(motionState.acceleration.z);
	}

	// Angular Velocity / Vector 3D
	{
		data[0].motionData.angularVelocity.setX(motionState.angularVelocity.x);
		data[0].motionData.angularVelocity.setY(motionState.angularVelocity.y);
		data[0].motionData.angularVelocity.setZ(motionState.angularVelocity.z);
	}

	// Gravity based orientation / Vector 3D
	{
		data[0].motionData.userInterfaceOrientation.setX(motionState.basicOrientation.x);
		data[0].motionData.userInterfaceOrientation.setY(motionState.basicOrientation.y);
		data[0].motionData.userInterfaceOrientation.setZ(motionState.basicOrientation.z);
	}
	
	// Magnetometer / 
	// Note. Magnetometer is off -> The matrix is unit matrix. The sensibility is zero.
	{
		data[0].motionData.magnetometer.magneticFieldStability = motionState.magnFieldStability;

		for (int i=0 ; i<4 ; ++i) {
			for (int j=0 ; j<4 ; ++j) {
				data[0].motionData.magnetometer.northEastDownOrientation.setElem(i,j, motionState.nedMatrix.f[i][j]);
			}
		}
	}
	return;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::finalizeMotion(void)
{
	int ret = SCE_OK;

	sceMotionMagnetometerOff();
	sceMotionStopSampling();

	return ret;
}

/* ------------------------------------------------------------------------- */
/* Motion Methods                                                            */
/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::enableMotionSensor(bool bEnable)
{
	int ret = SCE_OK;
	this->m_motionEnabled = bEnable;

	if (bEnable) {
		ret = sceMotionStartSampling();
	} else {
		ret = sceMotionMagnetometerOff();
		SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionMagnetometerOff() failed with: 0x%x\n", ret);

		ret = sceMotionStopSampling();
	}

	return ret;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::enableMagnetometer(bool bEnable)
{
	if (this->m_motionEnabled) {
		if (bEnable) {
			return sceMotionMagnetometerOn();
		} else {
			return sceMotionMagnetometerOff();
		}
	} else {
		if (bEnable) {
			return SCE_SAMPLE_UTIL_ERROR_NOT_ACTIVE;
		} else {
			return sceMotionMagnetometerOff();
		}
	}
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::enableTiltCorrection(bool bEnable)
{
	return sceMotionSetTiltCorrection((int)bEnable);
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::enableAngularVelocityDeadband(bool bEnable)
{
	return sceMotionSetDeadband((int)bEnable);
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForVitaImpl::resetOrientation(void)
{
	return sceMotionReset();
}

/* ------------------------------------------------------------------------- */
int ssii::createVitaPadContext(sce::SampleUtil::Input::PadContext **outPadContext,
							   const System::UserId userId,
							   const int32_t type,
							   const int32_t index,
							   const PadContextOption* option)
{
	if (outPadContext == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssii::PadContextForVitaImpl *t = new ssii::PadContextForVitaImpl;
	int ret = t->initialize(userId, type, index, option);
	if (ret != SCE_OK) {
		delete t;
		return ret;
	}
	*outPadContext = (ssi::PadContext *)t;

	return SCE_OK;
}
#endif/*_SCE_TARGET_OS_PSP2*/
