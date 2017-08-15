/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <scebase_common.h>
#include "sampleutil_internal.h"
#if _SCE_TARGET_OS_ORBIS

#include <sampleutil/input.h>
#include <sampleutil/sampleutil_error.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <kernel.h>
#include <user_service.h>

#include "../pad_common.h"

namespace sce
{
	namespace SampleUtil
	{
		namespace Input
		{
			namespace Impl
			{
				typedef int (*EraseCallBack)(void *caller, void *arg);

				class PadContextForOrbisImpl : public PadContextImpl
				{
				public:

					PadContextForOrbisImpl(void)
					{
						m_handle = -1;
						m_currentPadData  = NULL;
						m_previousPadData = NULL;
						m_currentData = NULL;
					}
					~PadContextForOrbisImpl(void)
					{
						finalize();
					}

					int initialize(System::UserId userId, int32_t type, int32_t index, const PadContextOption* option=NULL);

					int finalize(void);

					int update(void); 

					int enableMotionSensor(bool bEnable);
					int enableTiltCorrection(bool bEnable);
					int enableAngularVelocityDeadband(bool bEnable);
					int resetOrientation();
					int setVibration(float largeMotorSpeed, float smallMotorSpeed, uint32_t vibrationTimeInMs = 2500);
					int setLightBar(const ScePadLightBarParam *pParam);
					int resetLightBar(void);

					int getPadHandle(void);
					float getTouchPadPixelDensity(void);
					float getTouchPadAspectRatio(void);
					int getConnectionType(void);

				private:
					ScePadData* m_currentData;

					SceKernelTimespec m_previousTime;

					float   m_touchPadPixelDensity;
					float   m_touchPadAspectRatio;
					int     m_connectionType;

					int m_currentConnectedCound;

					int m_touchPadWidth;
					int m_touchPadHeight;

					int32_t m_handle;

					bool m_isTimeSpecifiedVibing;
					int	m_vibrationTimeInMs;
					uint64_t m_startVibrationTime;

					int updatePadData(void);
					int open(System::UserId userId, int32_t type, int32_t index, const ScePadOpenParam* pParam);
					int setScePadDataForUserLogoutCase(ScePadData* data);
				};
			}
		}
	}
}

#pragma comment(lib,"libScePad_stub_weak.a")


namespace ssi = sce::SampleUtil::Input;
namespace ssii = sce::SampleUtil::Input::Impl;
using namespace sce::Vectormath::Simd::Aos;

static bool	m_usedPadInit = false;

static ssi::PadData convert(const ScePadData &pad, int touchPadWidth, int touchPadHeight)
{
	ssi::PadData ctrl;
	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.connected = pad.connected;

	if (!pad.connected) {
		ctrl.lx = 127;
		ctrl.ly = 127;
		ctrl.rx = 127;
		ctrl.ry = 127;

		ctrl.l2 = 0;
		ctrl.r2 = 0;
		return ctrl;
	}
	ctrl.timeStamp = pad.timestamp;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_L3    ) ? ssi::kButtonL3       : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_R3    ) ? ssi::kButtonR3       : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_OPTIONS ) ? ssi::kButtonStart  : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_UP    ) ? ssi::kButtonUp       : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_RIGHT ) ? ssi::kButtonRight    : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_DOWN  ) ? ssi::kButtonDown     : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_LEFT  ) ? ssi::kButtonLeft     : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_L2    ) ? ssi::kButtonL2       : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_R2    ) ? ssi::kButtonR2       : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_L1    ) ? ssi::kButtonL1       : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_R1    ) ? ssi::kButtonR1       : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_TRIANGLE ) ? ssi::kButtonTriangle : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_CIRCLE   ) ? ssi::kButtonCircle   : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_CROSS    ) ? ssi::kButtonCross    : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_SQUARE   ) ? ssi::kButtonSquare   : 0;
	ctrl.buttons |= (pad.buttons & SCE_PAD_BUTTON_TOUCH_PAD   ) ? ssi::kButtonTouchPad   : 0;
	ctrl.lx = pad.leftStick.x;
	ctrl.ly	= pad.leftStick.y;
	ctrl.rx	= pad.rightStick.x;
	ctrl.ry = pad.rightStick.y;

	ctrl.l2	= pad.analogButtons.l2;
	ctrl.r2 = pad.analogButtons.r2;

	ctrl.motionData.acceleration.setX(pad.acceleration.x);
	ctrl.motionData.acceleration.setY(pad.acceleration.y);
	ctrl.motionData.acceleration.setZ(pad.acceleration.z);
	ctrl.motionData.angularVelocity.setX(pad.angularVelocity.x);
	ctrl.motionData.angularVelocity.setY(pad.angularVelocity.y);
	ctrl.motionData.angularVelocity.setZ(pad.angularVelocity.z);
	ctrl.motionData.orientation.setW(pad.orientation.w);
	ctrl.motionData.orientation.setX(pad.orientation.x);
	ctrl.motionData.orientation.setY(pad.orientation.y);
	ctrl.motionData.orientation.setZ(pad.orientation.z);

	ctrl.touchNumber = pad.touchData.touchNum;

	for (int j=0;j<SCE_PAD_MAX_TOUCH_NUM;j++) {
		ctrl.touchPadData[j].id = pad.touchData.touch[j].id;
		ctrl.touchPadData[j].x  = (float)pad.touchData.touch[j].x / touchPadWidth;
		ctrl.touchPadData[j].y  = (float)pad.touchData.touch[j].y / touchPadHeight;
	}

	ctrl.intercepted = (pad.buttons & SCE_PAD_BUTTON_INTERCEPTED) != 0;
	ctrl.connectedCount = pad.connectedCount;

	return ctrl;
}


int ssii::PadContextForOrbisImpl::initialize(System::UserId userId, int32_t type, int32_t index, const ssi::PadContextOption* option)
{
	int ret = SCE_OK;

	if (option == NULL)	{
		m_numPadData = kDefaultNumPadDataBufs;
	} else {
		if (option->numBufs < 1 || kMaxNumPadDataBufs < option->numBufs) {
			return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
		}
		m_numPadData = option->numBufs;
	}
	m_numCurrentPadData = 0;
	m_numPreviousPadData = 0;

	m_userId = userId;
	m_type = type;
	m_index = index;

	m_pressedButtonData = 0;
	m_releasedButtonData = 0;

	m_isTimeSpecifiedVibing = false;

	if (m_usedPadInit == false) {
		ret = scePadInit();
		m_usedPadInit = true;
	}
	if (ret != SCE_OK) {
		return ret;
	}

	m_handle = scePadOpen(userId, type, index, (option != NULL)?option->padOpenParam:NULL);
	if (m_handle < 0) {
		return m_handle;
	}

	m_defaultRightAnalogStickDeadZone = 0.06666667;
	m_defaultLeftAnalogStickDeadZone  = 0.06666667;
	m_touchPadWidth        = 0x00000780;
	m_touchPadHeight       = 0x000002f2;
	m_touchPadPixelDensity = 42.2000008;
	m_connectionType       = -1;

	m_touchPadAspectRatio  = (float)m_touchPadWidth / m_touchPadHeight;

	m_currentConnectedCound = 0;

	m_rightAnalogStickDeadZone = m_defaultRightAnalogStickDeadZone;
	m_leftAnalogStickDeadZone  = m_defaultLeftAnalogStickDeadZone;

	m_currentPadData = (PadData*)malloc(sizeof(PadData) * m_numPadData);
	if (m_currentPadData == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}
	for (uint32_t i = 0; i < m_numPadData; i++) {
		memset((void*)&m_currentPadData[i], 0, sizeof(m_currentPadData[i]));
	}

	m_previousPadData = (PadData*)malloc(sizeof(PadData) * m_numPadData);
	if (m_previousPadData == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}
	for (uint32_t i = 0; i < m_numPadData; i++) {
		memset((void*)&m_previousPadData[i], 0, sizeof(m_previousPadData[i]));
	}

	m_currentData = (ScePadData*)malloc(sizeof(ScePadData) * m_numPadData);
	if (m_currentData == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}
	for (uint32_t i = 0; i < m_numPadData; i++) {
		memset((void*)&m_currentData[i], 0, sizeof(m_currentData[i]));
	}

	m_leftStickValue.setX(0.0f);
	m_leftStickValue.setY(0.0f);
	m_rightStickValue.setX(0.0f);
	m_rightStickValue.setY(0.0f);

	return ret;
}

int ssii::PadContextForOrbisImpl::finalize(void)
{
	if (m_handle > 0) {
		int ret = scePadClose(m_handle);
		if (ret < 0) {
			return ret;
		}
	}
	if (m_currentPadData) {
		free(m_currentPadData);
	}
	if (m_previousPadData) {
		free(m_previousPadData);
	}
	if (m_currentData) {
		free(m_currentData);
	}

	setVibration(0.0f, 0.0f, 0);	

	return SCE_OK;
}

int ssii::PadContextForOrbisImpl::update(void)
{
	int ret;
	
	if (m_isTimeSpecifiedVibing) {		
		if (sceKernelGetProcessTime() - m_startVibrationTime >= m_vibrationTimeInMs * 1000) {			
			setVibration(0.0f, 0.0f, 0);	
			m_isTimeSpecifiedVibing = false;			
		}
	}	

	ret = updatePadData();

	float lX=0.0f, lY=0.0f, rX=0.0f, rY=0.0f;
	m_pressedButtonData  = 0;
	m_releasedButtonData = 0;
	
	if(m_numCurrentPadData>0){
		m_pressedButtonData  = (m_currentPadData[m_numCurrentPadData-1].buttons & ~m_previousPadData[0].buttons);
		m_releasedButtonData = (~m_currentPadData[m_numCurrentPadData-1].buttons & m_previousPadData[0].buttons);		///< released button event data
		for(int i=(m_numCurrentPadData-1);i>0;i--){
			m_pressedButtonData  |= (m_currentPadData[i-1].buttons & ~m_currentPadData[i].buttons);
			m_releasedButtonData |= (~m_currentPadData[i-1].buttons & m_currentPadData[i].buttons);
		}

		// Get the analogue stick values
		// Remap ranges from 0-255 to -1 > +1
		lX = (float)((int32_t)m_currentPadData[0].lx * 2 -255) * m_recipMaxByteAsFloat;
		lY = (float)((int32_t)m_currentPadData[0].ly * 2 -255) * m_recipMaxByteAsFloat;
		rX = (float)((int32_t)m_currentPadData[0].rx * 2 -255) * m_recipMaxByteAsFloat;
		rY = (float)((int32_t)m_currentPadData[0].ry * 2 -255) * m_recipMaxByteAsFloat;
	
		// store stick values adjusting for deadzone
		m_leftStickValue.setX((fabsf(lX) < m_leftAnalogStickDeadZone) ? 0.0f : lX);
		m_leftStickValue.setY((fabsf(lY) < m_leftAnalogStickDeadZone) ? 0.0f : lY);
		m_rightStickValue.setX((fabsf(rX) < m_rightAnalogStickDeadZone) ? 0.0f : rX);
		m_rightStickValue.setY((fabsf(rY) < m_rightAnalogStickDeadZone) ? 0.0f : rY);

	}

	return ret;
}

int ssii::PadContextForOrbisImpl::enableMotionSensor(bool bEnable)
{
	int ret = scePadSetMotionSensorState(m_handle, bEnable);

	if (ret == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
		return SCE_OK;
	} else {
		return ret;
	}
}

int ssii::PadContextForOrbisImpl::enableTiltCorrection(bool bEnable)
{
	int ret = scePadSetTiltCorrectionState(m_handle, bEnable);

	if (ret == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
		return SCE_OK;
	} else {
		return ret;
	}
}

int ssii::PadContextForOrbisImpl::enableAngularVelocityDeadband(bool bEnable)
{
	int ret = scePadSetAngularVelocityDeadbandState(m_handle, bEnable);

	if (ret == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
		return SCE_OK;
	} else {
		return ret;
	}
}

int ssii::PadContextForOrbisImpl::resetOrientation()
{
	int ret = scePadResetOrientation(m_handle);

	if (ret == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
		return SCE_OK;
	} else {
		return ret;
	}
}

int ssii::PadContextForOrbisImpl::setVibration(float largeMotorSpeed, float smallMotorSpeed, uint32_t vibrationTimeInMs)
{
	ScePadVibrationParam param;
				
	if (largeMotorSpeed < 0.0f || largeMotorSpeed > 1.0f || smallMotorSpeed < 0.0f || smallMotorSpeed > 1.0f) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if (vibrationTimeInMs > 2500) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	
	if ((largeMotorSpeed == 0.0f && smallMotorSpeed == 0.0f) || vibrationTimeInMs == 0) {	
		param.largeMotor = 0;
		param.smallMotor = 0;
		m_isTimeSpecifiedVibing = false;
	} else {
		param.largeMotor = 255 * largeMotorSpeed;
		param.smallMotor = 255 * smallMotorSpeed;
		m_vibrationTimeInMs = vibrationTimeInMs;
		m_startVibrationTime = sceKernelGetProcessTime();
		m_isTimeSpecifiedVibing = true;
	}

	int ret = scePadSetVibration(m_handle, &param);

	if (ret == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
		return SCE_OK;
	} else {
		return ret;
	}
}

int ssii::PadContextForOrbisImpl::setLightBar(const ScePadLightBarParam *pParam)
{
	int ret = scePadSetLightBar(m_handle, pParam);

	if (ret == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
		return SCE_OK;
	} else {
		return ret;
	}
}

int ssii::PadContextForOrbisImpl::resetLightBar(void)
{
	int ret = scePadResetLightBar(m_handle);

	if (ret == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
		return SCE_OK;
	} else {
		return ret;
	}
}

float ssii::PadContextForOrbisImpl::getTouchPadPixelDensity(void)
{
	return m_touchPadPixelDensity;
}

float ssii::PadContextForOrbisImpl::getTouchPadAspectRatio(void)
{
	return m_touchPadAspectRatio;
}

int ssii::PadContextForOrbisImpl::getConnectionType(void)
{
	return m_connectionType;
}

int ssii::PadContextForOrbisImpl::getPadHandle(void)
{
	return m_handle;
}

int ssii::PadContextForOrbisImpl::updatePadData(void)
{
	int ret = 0;
	bool logout = false;

	ret = scePadRead(m_handle, m_currentData, m_numPadData);

	if (ret == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
		logout = true;
	} else {
		if (ret < SCE_OK) {
			return ret;
		}
		if (m_currentData->connected && m_currentData->connectedCount > m_currentConnectedCound) {
			ScePadControllerInformation pInfo;
			int ret2 = scePadGetControllerInformation(m_handle, &pInfo);
			if (ret2 == SCE_HID_ERROR_ALREADY_LOGGED_OUT) {
				logout = true;
			} else {
				if (ret2 < SCE_OK) {
					return ret2;
				}
				m_defaultRightAnalogStickDeadZone = (float)pInfo.stickInfo.deadZoneRight/UINT8_MAX;
				m_defaultLeftAnalogStickDeadZone  = (float)pInfo.stickInfo.deadZoneLeft/UINT8_MAX;
				m_touchPadWidth         = pInfo.touchPadInfo.resolution.x;
				m_touchPadHeight        = pInfo.touchPadInfo.resolution.y;
				m_touchPadPixelDensity  = (float)pInfo.touchPadInfo.pixelDensity;
				m_touchPadAspectRatio   = (float)m_touchPadWidth / m_touchPadHeight;
				m_currentConnectedCound = m_currentData->connectedCount;
				m_connectionType        = pInfo.connectionType;
			}
		}
	}

	if (logout) {
		ret = setScePadDataForUserLogoutCase(m_currentData);
	}

	m_numCurrentPadData = (ret >= m_numPadData) ? m_numPadData : ret;
	if(m_numCurrentPadData > 0){
		for(int i = 0; i < m_numPreviousPadData; i++) {
			m_previousPadData[i] = m_currentPadData[i];
		}
		for(int i = 0; i < m_numCurrentPadData; i++) {
			m_currentPadData[i] = convert(m_currentData[m_numCurrentPadData - 1 - i], m_touchPadWidth, m_touchPadHeight);
		}
		for(int i = m_numCurrentPadData; i < m_numPadData; i++) {
			memset((void*)&m_currentPadData[i], 0, sizeof(m_currentPadData[i]));
		}
		m_numPreviousPadData = m_numCurrentPadData;
	}

	return ret;
}

int ssii::PadContextForOrbisImpl::setScePadDataForUserLogoutCase(ScePadData* data)
{
	memset(data, 0x0, sizeof(ScePadData));
	data->connected = false;
	data->leftStick.x = 0x80;
	data->leftStick.y = 0x80;
	data->rightStick.x = 0x80;
	data->rightStick.y = 0x80;
	data->orientation.w = 1.0f;
	data->acceleration.y = 1.0f;

	return 1;
}

int ssii::createOrbisPadContext(ssi::PadContext **outPadContext,
								const System::UserId userId,
								const int32_t type,
								const int32_t index,
								const ssi::PadContextOption* option)
{
	if (outPadContext == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssii::PadContextForOrbisImpl *t = new ssii::PadContextForOrbisImpl;
	int ret = t->initialize(userId, type, index, option);
	if (ret != SCE_OK) {
		delete t;
		return ret;
	}
	*outPadContext = (ssi::PadContext *)t;

	return SCE_OK;
}

#endif //_SCE_TARGET_OS_ORBIS
