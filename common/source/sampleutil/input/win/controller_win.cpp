/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#if _SCE_TARGET_OS_WINDOWS 

#include "../controller_common.h"

namespace ss = sce::SampleUtil;
namespace ssi = sce::SampleUtil::Input;
namespace ssii = sce::SampleUtil::Input::Impl;

BOOL CALLBACK ssii::ControllerBase::enumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pvRef)
{
	ssii::ControllerBase *context = (ssii::ControllerBase *) pvRef;

	if (0 <= context->m_pDI->CreateDevice(pdidInstance->guidInstance, &context->m_pJoystick, NULL)) {
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

BOOL CALLBACK ssii::ControllerBase::enumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pvRef)
{
	ssii::ControllerBase *context = (ssii::ControllerBase *) pvRef;

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

int ssii::ControllerBase::initialize(void)
{
	m_pJoystick = NULL;
	
	// Setup DirectInput
	int ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDI, NULL);
	if (ret < 0) {
		return ret;
	}
	// Find a Joystick Device
	IDirectInputJoyConfig8* pJoyConfig = NULL;
	ret = m_pDI->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyConfig);
	if (ret < 0) {
		return ret;
	}
	if (pJoyConfig) {
		pJoyConfig->Release();
		pJoyConfig = NULL;
	}
	ret =  m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
	if (ret < 0) {
		return ret;
	}
	
	if (m_pJoystick) {
		m_pJoystick->SetDataFormat( &c_dfDIJoystick2 );
		m_pJoystick->EnumObjects( enumObjectsCallback, this, DIDFT_ALL );
	}
	return SCE_OK;
}

int ssii::ControllerBase::finalize(void)
{
	if (m_pJoystick) {
		m_pJoystick->Unacquire();
		m_pJoystick->Release();
		m_pJoystick = NULL;
	}
	if (m_pDI) {
		m_pDI->Release();
		m_pDI = NULL;
	}
	return SCE_OK;
}

int ssii::ControllerBase::readBufferPositive(ssi::ControllerData *ctrlData, uint32_t numBufs)
{
	if (m_pJoystick==NULL) {
		return -1;
	}
	m_pJoystick->Acquire();
	int ret = m_pJoystick->Poll();
	if (ret < 0) {
		return ret;
	}

	DIJOYSTATE2 joystate;
	ret = m_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &joystate);
	if (ret < 0) {
		return ret;
	}

	memset(ctrlData, 0, sizeof(ssi::ControllerData));

	ctrlData->lx = (uint8_t)joystate.lX;
	ctrlData->ly = (uint8_t)joystate.lY;
	ctrlData->rx = (uint8_t)joystate.lZ;
	ctrlData->ry = (uint8_t)joystate.lRz;

	if (0x00 != ( 0x80 & joystate.rgbButtons[9])) {
		ctrlData->buttons |= kButtonSelect;
	}
	if (0x00 != ( 0x80 & joystate.rgbButtons[8])) {
		ctrlData->buttons |= kButtonStart;
	}
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
	if (0x00 != ( 0x80 & ( joystate.rgbButtons[4] | joystate.rgbButtons[6] ) )) {
		ctrlData->buttons |= kButtonL1;
	}
	if (0x00 != ( 0x80 & ( joystate.rgbButtons[5] | joystate.rgbButtons[7] ) )) {
		ctrlData->buttons |= kButtonR1;
	}
	if (0x00 != ( 0x80 & joystate.rgbButtons[0] )) {
		ctrlData->buttons |= kButtonTriangle;
	}
	if (0x00 != ( 0x80 & joystate.rgbButtons[1] )) {
		ctrlData->buttons |= kButtonCircle;
	}
	if (0x00 != ( 0x80 & joystate.rgbButtons[2] )) {
		ctrlData->buttons |= kButtonCross;
	}
	if (0x00 != ( 0x80 & joystate.rgbButtons[3] )) {
		ctrlData->buttons |= kButtonSquare;
	}

	(void)numBufs;
	return 1;
}

#endif// _SCE_TARGET_OS_WINDOWS 
