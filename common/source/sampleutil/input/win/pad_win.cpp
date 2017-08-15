/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil/input/controller.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil_internal.h>

#include "../pad_common.h"
/* ------------------------------------------------------------------------- */
namespace ssi  = sce::SampleUtil::Input;
namespace sss  = sce::SampleUtil::System;

#if defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS

#include "mmsystem.h"
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "libScePad.lib")

namespace sce
{
	namespace SampleUtil
	{
		namespace Input
		{
			namespace Impl
			{
				typedef int (*EraseCallBack)(void *caller, void *arg);

				class PadContextForWindowsImpl : public PadContextImpl
				{
				public:

					PadContextForWindowsImpl(void)
					{
						m_handle = -1;
						m_currentPadData  = NULL;
						m_previousPadData = NULL;
						m_currentData = NULL;
						m_previousData = NULL;
					}
					~PadContextForWindowsImpl(void)
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
					ScePadData* m_previousData;

					DWORD m_previousTime;

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
	ctrl.connectedCount = pad.connectedCount;

	return ctrl;
}


int ssii::PadContextForWindowsImpl::initialize(sss::UserId userId, int32_t type, int32_t index, const ssi::PadContextOption* option)
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

	m_previousData = (ScePadData*)malloc(sizeof(ScePadData) * m_numPadData);
	if (m_previousData == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}
	for (uint32_t i = 0; i < m_numPadData; i++) {
	   memset((void*)&m_previousData[i], 0, sizeof(m_previousData[i]));
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

	timeBeginPeriod(1);

	return ret;
}

int ssii::PadContextForWindowsImpl::finalize(void)
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
	if (m_previousData) {
		free(m_previousData);
	}

	setVibration(0.0f, 0.0f, 0);	

	return SCE_OK;
}

int ssii::PadContextForWindowsImpl::update(void)
{
	int ret;
	
	if (m_isTimeSpecifiedVibing) {		
		if (timeGetTime() - m_startVibrationTime >= m_vibrationTimeInMs * 1000) {			
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

int ssii::PadContextForWindowsImpl::enableMotionSensor(bool bEnable)
{
	int ret = scePadSetMotionSensorState(m_handle, bEnable);
	return ret;
}

int ssii::PadContextForWindowsImpl::enableTiltCorrection(bool bEnable)
{
	int ret = scePadSetTiltCorrectionState(m_handle, bEnable);
	return ret;
}

int ssii::PadContextForWindowsImpl::enableAngularVelocityDeadband(bool bEnable)
{
	int ret = scePadSetAngularVelocityDeadbandState(m_handle, bEnable);
	return ret;
}

int ssii::PadContextForWindowsImpl::resetOrientation()
{
	int ret = scePadResetOrientation(m_handle);
	return ret;
}

int ssii::PadContextForWindowsImpl::setVibration(float largeMotorSpeed, float smallMotorSpeed, uint32_t vibrationTimeInMs)
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
		m_startVibrationTime = timeGetTime();
		m_isTimeSpecifiedVibing = true;
	}

	int ret = scePadSetVibration(m_handle, &param);
	return ret;
}

int ssii::PadContextForWindowsImpl::setLightBar(const ScePadLightBarParam *pParam)
{
	int ret = scePadSetLightBar(m_handle, pParam);
	return ret;
}

int ssii::PadContextForWindowsImpl::resetLightBar(void)
{
	int ret = scePadResetLightBar(m_handle);
	return ret;
}

float ssii::PadContextForWindowsImpl::getTouchPadPixelDensity(void)
{
	return m_touchPadPixelDensity;
}

float ssii::PadContextForWindowsImpl::getTouchPadAspectRatio(void)
{
	return m_touchPadAspectRatio;
}

int ssii::PadContextForWindowsImpl::getConnectionType(void)
{
	return m_connectionType;
}

int ssii::PadContextForWindowsImpl::getPadHandle(void)
{
	return m_handle;
}

int ssii::PadContextForWindowsImpl::updatePadData(void)
{
	int ret = 0;
	bool logout = false;

	ret = scePadRead(m_handle, m_currentData, m_numPadData);

	if (ret < SCE_OK) {
		return ret;
	}

	if (m_currentData->connected && m_currentData->connectedCount > m_currentConnectedCound) {
		ScePadControllerInformation pInfo;
		int ret2 = scePadGetControllerInformation(m_handle, &pInfo);
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

int ssii::PadContextForWindowsImpl::setScePadDataForUserLogoutCase(ScePadData* data)
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

#else/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800 /* DIRECTINPUT_HEADER_VERSION */
#endif/*DIRECTINPUT_VERSION*/

#include <windows.h>
#include <dinput.h>
#include <dinputd.h>
#include <Dbt.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "DXGUID.LIB")

namespace ssii = sce::SampleUtil::Input::Impl;

namespace sce
{
	namespace SampleUtil
	{
		namespace Input
		{
			namespace Impl
			{
				/* ------------------------------------------------------------------------- */
				const int SCE_SAMPLE_UTIL_MAX_PADS = 4;
				const GUID GUID_DEVINTERFACE_USB_DEVICE = { 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F,  0x00,  0xC0,  0x4F,  0xB9,  0x51,  0xED } };
				LRESULT CALLBACK DeviceStateProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

				class JoystickPool
				{
				public:
					static JoystickPool &getInstance(void)
					{
						static JoystickPool singlton;
						return singlton;
					}

					int getPadList(DIDEVICEINSTANCE *list, int listLength, int *size);
					int getDirextInputInstance(LPDIRECTINPUT8 *pDirectInput);
					int enumerateJoystick(void);
				private:
					DIDEVICEINSTANCE m_joystickDeviceInstances[SCE_SAMPLE_UTIL_MAX_LOGIN_USERS];
					int m_numJoyStick;

					DIDEVICEINSTANCE m_enumeratedJoystickDeviceInstances[SCE_SAMPLE_UTIL_MAX_LOGIN_USERS];
					int m_numEnumeratedJoyStick;

					LPDIRECTINPUT8			m_pDI;
					static BOOL CALLBACK enumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pvRef);

					JoystickPool();
				};

				/* ------------------------------------------------------------------------- */
				class PadContextForWindowsImpl : public PadContextImpl
				{
				public:
					PadContextForWindowsImpl(void);
					~PadContextForWindowsImpl(){return;}

					void setPadGUID(const GUID &guid);

					/* ------------ Motion Sensor Methods ------------ */
					int enableMotionSensor(bool bEnable);
					int enableTiltCorrection(bool bEnable);
					int enableAngularVelocityDeadband(bool bEnable);
					int resetOrientation();

				private:
					/* ------------ From Super Class ------------ */
					void updateController(ssi::PadData *data, uint32_t numPadData);
					void updateTouchDevice(ssi::PadData *data, uint32_t numPadData);
					void updateMotion(ssi::PadData *data, uint32_t numPadData);

					int initializeController(const PadContextOption* pOption);
					int initializeTouchDevice(const PadContextOption* pOption);
					int initializeMotion(const PadContextOption* pOption);

					int finalizeController(void);
					int finalizeTouchDevice(void);
					int finalizeMotion(void);

					/* ------------ For Joystick(Direct X) ------------ */
					GUID					m_padGuid;
					LPDIRECTINPUTDEVICE8	m_pJoystick;
					static BOOL CALLBACK enumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
					int readBufferPositive(ssi::PadData *ctrlData, int numData);

					/* ------------- For Dual Shock 3/4  -------------- */
					GUID					DS4Guid;
					GUID					DS3Guid;

					/* ------------------------------------------------ */
					enum PAD_TYPE{
						UNKNOWN_PAD,
						DUAL_SHOCK_3,
						DUAL_SHOCK_4
					};

					PAD_TYPE				m_padType;

					/* ------------------------------------------------ */
					class DualShockButtonIndex{
					public:
						int iSelect;
						int iStart;
						int iL1;
						int iR1;
						int iL2;
						int iR2;
						int iTriangle;
						int iCircle;
						int iCross;
						int iSquare;

						DualShockButtonIndex(
							int _iSelect, int _iStart, 
							int _iL1, int _iL2, int _iR1, int _iR2,
							int _iTriangle, int _iCircle, int _iCross, int _iSquare)
						{
							this->iSelect = _iSelect;
							this->iStart  = _iStart;

							this->iL1   = _iL1;
							this->iL2   = _iL2;
							this->iR1   = _iR1;
							this->iR2   = _iR2;

							this->iTriangle = _iTriangle;
							this->iCircle   = _iCircle;
							this->iCross    = _iCross;
							this->iSquare   = _iSquare;

							return;
						}
					};

					const DualShockButtonIndex m_indexForDS3;
					const DualShockButtonIndex m_indexForDS4;

					DualShockButtonIndex m_padButtonIndex;
				};
			}
		}
	}
}

/* ------------------------------------------------------------------------- */
ssii::PadContextForWindowsImpl::PadContextForWindowsImpl(void)
	: m_indexForDS3(9,8,6,4,7,5,0,1,2,3), m_indexForDS4(8,9,4,6,5,7,3,2,1,0), m_padButtonIndex(0,0,0,0,0,0,0,0,0,0)
{
	// Set DualShock4 GUID
	{
		DS4Guid.Data1 = 0x05C4054C;
		DS4Guid.Data2 = 0;
		DS4Guid.Data3 = 0;
		DS4Guid.Data4[0] = 0;
		DS4Guid.Data4[1] = 0;
		DS4Guid.Data4[2] = 'P';
		DS4Guid.Data4[3] = 'I';
		DS4Guid.Data4[4] = 'D';
		DS4Guid.Data4[5] = 'V';
		DS4Guid.Data4[6] = 'I';
		DS4Guid.Data4[7] = 'D';
	}

	// Set DualShock3 GUID
	{
		DS3Guid.Data1 = 0x0268054C;
		DS3Guid.Data2 = 0;
		DS3Guid.Data3 = 0;
		DS3Guid.Data4[0] = 0;
		DS3Guid.Data4[1] = 0;
		DS3Guid.Data4[2] = 'P';
		DS3Guid.Data4[3] = 'I';
		DS3Guid.Data4[4] = 'D';
		DS3Guid.Data4[5] = 'V';
		DS3Guid.Data4[6] = 'I';
		DS3Guid.Data4[7] = 'D';
	}

	this->m_padType = UNKNOWN_PAD;

	return;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::enableMotionSensor(bool bEnable)
{
	int ret = SCE_OK;

	return ret;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::enableTiltCorrection(bool bEnable)
{
	int ret = SCE_OK;

	return ret;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::enableAngularVelocityDeadband(bool bEnable)
{
	int ret = SCE_OK;

	return ret;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::resetOrientation(void)
{
	int ret = SCE_OK;

	return ret;
}


/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::initializeController(const ssi::PadContextOption* pOption)
{
	int ret = SCE_OK;

	// Create Device
	m_pJoystick = NULL;
	ret = sce::SampleUtil::Input::createDirectInputDevice(m_userId->guidInstance, &m_pJoystick);
	if (ret < SCE_OK) {
		return ret;
	}

	this->setPadGUID(m_userId->guidProduct);
	if (m_pJoystick) {
		ret = m_pJoystick->SetDataFormat(&c_dfDIJoystick2);
		if (ret < SCE_OK) {
			return ret;
		}

		ret = m_pJoystick->EnumObjects(enumObjectsCallback, this, DIDFT_ALL);
		if (ret < SCE_OK) {
			return ret;
		}
	}

	return ret;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::finalizeController(void)
{
	int ret = SCE_OK;

	return ret;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextForWindowsImpl::updateController(ssi::PadData *data, uint32_t numPadData)
{
	int returnBufs = readBufferPositive(data, numPadData);

	return;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::initializeTouchDevice(const ssi::PadContextOption* pOption)
{
	int ret = SCE_OK;

	return ret;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::finalizeTouchDevice(void)
{
	int ret = SCE_OK;

	return ret;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextForWindowsImpl::updateTouchDevice(ssi::PadData *data, uint32_t numPadData)
{
	return;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::initializeMotion(const ssi::PadContextOption* pOption)
{
	int ret = SCE_OK;

	return ret;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::finalizeMotion(void)
{
	int ret = SCE_OK;

	return ret;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextForWindowsImpl::updateMotion(ssi::PadData *data, uint32_t numPadData)
{
	return;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextForWindowsImpl::setPadGUID(const GUID &guid)
{
	this->m_padGuid = guid;

	if (guid.Data1 == DS4Guid.Data1 && 0 == memcmp((void *)(guid.Data4), "\0\0PIDVID", 8)) {
		this->m_padType = ssii::PadContextForWindowsImpl::DUAL_SHOCK_4;
		this->m_padButtonIndex = this->m_indexForDS4;
	} else if ( guid.Data1 == DS3Guid.Data1 && 0 == memcmp((void *)(guid.Data4), "\0\0PIDVID", 8)) {
		this->m_padType = ssii::PadContextForWindowsImpl::DUAL_SHOCK_3;
		this->m_padButtonIndex = this->m_indexForDS3;
	} else {
		this->m_padType = ssii::PadContextForWindowsImpl::UNKNOWN_PAD;
	}

	return;
}

/* ------------------------------------------------------------------------- */
BOOL CALLBACK ssii::PadContextForWindowsImpl::enumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pvRef)
{
	ssii::PadContextForWindowsImpl *context = (ssii::PadContextForWindowsImpl *) pvRef;

	if (pdidoi->dwType & DIDFT_AXIS) {
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof( DIPROPRANGE );
		diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
		diprg.diph.dwObj = pdidoi->dwType;
		diprg.diph.dwHow = DIPH_BYID;
		diprg.lMin = 0;
		diprg.lMax = 0xff;

		if (0 > context->m_pJoystick->SetProperty(DIPROP_RANGE, &diprg.diph)) {
			return DIENUM_STOP;
		}
	}

	return DIENUM_CONTINUE;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextForWindowsImpl::readBufferPositive(ssi::PadData *ctrlData, int numData)
{
	if (m_pJoystick==NULL) {
		return -1;
	}

	HRESULT result = m_pJoystick->Acquire();
		
	int ret = m_pJoystick->Poll();
	if (ret < DI_OK) {
		return ret;
	}

	DIJOYSTATE2 joystate;
	ret = m_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &joystate);
	if (ret < 0) {
		return ret;
	}

	memset(ctrlData, 0, sizeof(ssi::ControllerData));

	ctrlData[0].lx = (uint8_t)joystate.lX;
	ctrlData[0].ly = (uint8_t)joystate.lY;
	ctrlData[0].rx = (uint8_t)joystate.lZ;
	ctrlData[0].ry = (uint8_t)joystate.lRz;

	DualShockButtonIndex padButtonIndex = this->m_indexForDS3;
	BYTE bPRESSED = 0x80;

	if (31500 == joystate.rgdwPOV[0] ||     0 == joystate.rgdwPOV[0] ||  4500 == joystate.rgdwPOV[0]) {
		ctrlData->buttons |= kButtonUp;
	}
	if ( 4500 == joystate.rgdwPOV[0] ||  9000 == joystate.rgdwPOV[0] || 13500 == joystate.rgdwPOV[0]) {
		ctrlData->buttons |= kButtonRight;
	}
	if (13500 == joystate.rgdwPOV[0] || 18000 == joystate.rgdwPOV[0] || 22500 == joystate.rgdwPOV[0]) {
		ctrlData->buttons |= kButtonDown;
	}
	if (22500 == joystate.rgdwPOV[0] || 27000 == joystate.rgdwPOV[0] || 31500 == joystate.rgdwPOV[0]) {
		ctrlData->buttons |= kButtonLeft;
	}

	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iSelect]) {
		ctrlData->buttons |= kButtonSelect;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iStart]) {
		ctrlData->buttons |= kButtonStart;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iL1]) {
		ctrlData->buttons |= kButtonL1;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iL2]) {
		ctrlData->buttons |= kButtonL2;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iR1]) {
		ctrlData->buttons |= kButtonR1;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iR2]) {
		ctrlData->buttons |= kButtonR2;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iTriangle]) {
		ctrlData->buttons |= kButtonTriangle;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iCircle]) {
		ctrlData->buttons |= kButtonCircle;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iCross]) {
		ctrlData->buttons |= kButtonCross;
	}
	if (bPRESSED & joystate.rgbButtons[this->m_padButtonIndex.iSquare]) {
		ctrlData->buttons |= kButtonSquare;
	}

	return 1;
}

/* ------------------------------------------------------------------------- */
ssii::JoystickPool::JoystickPool()
{
	// Setup DirectInput: To share the Direct Input Instance, Initialzie the instance here.
	int returnCode = DirectInput8Create(
						GetModuleHandle(NULL),
						DIRECTINPUT_VERSION, 
						IID_IDirectInput8, 
						( VOID** )&this->m_pDI, 
						NULL);
	if (returnCode < 0) {
		m_pDI = NULL;
		return;
	}

	// Clear Game Pad List
	memset((void *)this->m_joystickDeviceInstances, 0x00, sizeof(DIDEVICEINSTANCE) * SCE_SAMPLE_UTIL_MAX_LOGIN_USERS);
	this->m_numJoyStick = 0;

	memset((void *)this->m_enumeratedJoystickDeviceInstances, 0x00, sizeof(DIDEVICEINSTANCE) * SCE_SAMPLE_UTIL_MAX_LOGIN_USERS);
	this->m_numEnumeratedJoyStick = 0;

	// Enumerate Game Pad
	this->enumerateJoystick();

	return;
}

/* ------------------------------------------------------------------------- */
int ssii::JoystickPool::getPadList(DIDEVICEINSTANCE *list, int listLength, int *size)
{
	*size = this->m_numJoyStick;

	for (int i=0 ; i<listLength && i < *size ; ++i) {
		list[i] = this->m_joystickDeviceInstances[i];
	}

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int ssii::JoystickPool::getDirextInputInstance(LPDIRECTINPUT8 *pDirectInput)
{
	*pDirectInput = this->m_pDI;

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int ssii::JoystickPool::enumerateJoystick(void)
{
	// initialize Enumeration Valiables
	memset((void *)this->m_enumeratedJoystickDeviceInstances, 0x00, sizeof(DIDEVICEINSTANCE) * SCE_SAMPLE_UTIL_MAX_LOGIN_USERS);
	this->m_numEnumeratedJoyStick = 0;
	
	// Enumerate Joysticks
	int result = this->m_pDI->EnumDevices(
						DI8DEVCLASS_GAMECTRL,
						ssii::JoystickPool::enumJoysticksCallback,
						this,
						DIEDFL_ATTACHEDONLY);

	// Copy Enumerated Pad
	memcpy((void *)this->m_joystickDeviceInstances,
		   (void *)this->m_enumeratedJoystickDeviceInstances,
		   sizeof(DIDEVICEINSTANCE) * SCE_SAMPLE_UTIL_MAX_LOGIN_USERS);
	this->m_numJoyStick = this->m_numEnumeratedJoyStick;

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
BOOL CALLBACK ssii::JoystickPool::enumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pvRef)
{
	// Queue Device Instance
	ssii::JoystickPool *pool = (ssii::JoystickPool *)pvRef;

	if (pool->m_numJoyStick < SCE_SAMPLE_UTIL_MAX_LOGIN_USERS) {
		memcpy((void*)&pool->m_enumeratedJoystickDeviceInstances[pool->m_numEnumeratedJoyStick++],
			   (void*)(DIDEVICEINSTANCE*)pdidInstance,
			   sizeof(DIDEVICEINSTANCE));
	}

	return DIENUM_CONTINUE;
}

/* ------------------------------------------------------------------------- */
int ssi::getPadList(DIDEVICEINSTANCE *list, int listLength, int *size)
{
	return ssii::JoystickPool::getInstance().getPadList(list, listLength, size);
}

/* ------------------------------------------------------------------------- */
int ssi::createDirectInputDevice(GUID deviceInstanceGUID, LPDIRECTINPUTDEVICE8 *joystick)
{
	LPDIRECTINPUT8 dI;
	ssii::JoystickPool::getInstance().getDirextInputInstance(&dI);

	return dI->CreateDevice(deviceInstanceGUID, joystick, NULL);
}

/* ------------------------------------------------------------------------- */
int ssi::enableDyanmicPadEnumeration(void)
{
	static const TCHAR * myProgramName = __TEXT("Invisible Window for Joystick Enumeration");
	HINSTANCE g_hInst = GetModuleHandle(NULL);

	// Register class
	WNDCLASSEX wcex;
	{
		wcex.cbSize         = sizeof(WNDCLASSEX); 
		wcex.style          = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc    = ssii::DeviceStateProc;
		wcex.cbClsExtra     = 0;
		wcex.cbWndExtra     = 0;
		wcex.hInstance      = g_hInst;
		wcex.hIcon          = NULL;
		wcex.hCursor        = LoadCursor( NULL, IDC_ARROW );
		wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName   = NULL;
		wcex.lpszClassName  = myProgramName;
		wcex.hIconSm        = NULL;
	}
	if (!RegisterClassEx(&wcex)) {
		return E_FAIL;
	}

	HWND _hWnd = CreateWindow( myProgramName,          // Class name
		myProgramName,          // Window name
		0,					    // Style
		CW_USEDEFAULT,          // X position
		CW_USEDEFAULT,          // Y position
		0,						// Width
		0,					    // Height
		NULL,                   // Parent HWND
		NULL,                   // Menu
		g_hInst,                // Instance
		NULL                    // Param
		);
	if (!_hWnd) {
		return E_FAIL;
	}

	DEV_BROADCAST_DEVICEINTERFACE devif;
	{
		ZeroMemory(&devif, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
		devif.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		devif.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		devif.dbcc_classguid = ssii::GUID_DEVINTERFACE_USB_DEVICE;
	}	
	RegisterDeviceNotification(_hWnd, &devif, DEVICE_NOTIFY_WINDOW_HANDLE);

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
LRESULT CALLBACK ssii::DeviceStateProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			exit(0);
			break;
		}
		break;
	case WM_DEVICECHANGE:
		{
			if (wParam == DBT_DEVICEARRIVAL || wParam == DBT_DEVICEREMOVECOMPLETE) {
				ssii::JoystickPool::getInstance().enumerateJoystick();
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

#endif/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */

int ssii::createWindowsPadContext(ssi::PadContext **outPadContext,
								  const sss::UserId userId,
								  const int32_t type,
								  const int32_t index,
								  const ssi::PadContextOption* option)
{
	if (outPadContext == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssii::PadContextForWindowsImpl *t = new ssii::PadContextForWindowsImpl;
	int ret = t->initialize(userId, type, index, option);
	if (ret != SCE_OK) {
		delete t;
		return ret;
	}
	*outPadContext = (ssi::PadContext *)t;

	return SCE_OK;
}

#endif /*_SCE_TARGET_OS_WINDOWS*/
