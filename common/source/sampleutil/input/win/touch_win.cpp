/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <sampleutil/input/touch.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil/sampleutil_common.h>
#include <sampleutil_internal.h>

#if _SCE_TARGET_OS_WINDOWS

#define DIRECTINPUT_VERSION 0x0800 /* DIRECTINPUT_HEADER_VERSION */
#include <windows.h>
#include <dinput.h>
#include <dinputd.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "DXGUID.LIB")

namespace sce
{
	namespace SampleUtil
	{
		namespace Input
		{
			namespace Impl
			{
				class TouchContextImpl : public TouchContext
				{
				public:
					TouchContextImpl(void){}
					virtual ~TouchContextImpl(void)
					{
						finalize();
					}
					int initialize(HWND hWnd);
					int finalize();
					int update();

					int getNumberOfPrimitiveTouchEvents() const;
					const PrimitiveTouchEvent* getPrimitiveTouchEvents() const;

				private:
					LPDIRECTINPUT8			m_pDI;
					LPDIRECTINPUTDEVICE8	m_pMouse;
					HWND					m_hWnd;

					struct MouseData {
						POINT	pos;
						bool	isButtonDown[2];
					};
					MouseData				m_mouseData;

					PrimitiveTouchEvent m_primitiveTouchEvents[1];
				};
			}
		}
	}
}

namespace ssi = sce::SampleUtil::Input;
namespace ssii = sce::SampleUtil::Input::Impl;

int ssii::TouchContextImpl::initialize(HWND hWnd)
{
	m_hWnd = hWnd;

	// Setup DirectInput
	int ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDI, NULL);
	if (ret < 0) {
		return ret;
	}

	ret = m_pDI->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	if (ret < 0) {
		return ret;
	}

	ret = m_pMouse->SetDataFormat(&c_dfDIMouse);
	if (ret < 0) {
		return ret;
	}

	ret = m_pMouse->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (ret < 0) {
		return ret;
	}

	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = 16;

	ret = m_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if (ret < 0) {
		return ret;
	}

	memset(&m_mouseData, 0, sizeof(MouseData));

	return SCE_OK;
}

int ssii::TouchContextImpl::finalize()
{
	if (m_pMouse) {
		m_pMouse->Release();
		m_pMouse = NULL;
	}

	if (m_pDI) {
		m_pDI->Release();
		m_pDI = NULL;
	}

	return SCE_OK;
}

int ssii::TouchContextImpl::update()
{
	int ret = 0;

	if (m_pMouse == NULL) {
		return -1;
	}
	m_pMouse->Acquire();

	DIDEVICEOBJECTDATA od;
	DWORD dwElements = 1;

	while (dwElements != 0) {
		dwElements = 1;
		ret = m_pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);

		switch (od.dwOfs) {
		case DIMOFS_BUTTON0:
			if (od.dwData & 0x80) {
				m_mouseData.isButtonDown[0] = true;
			} else {
				m_mouseData.isButtonDown[0] = false;
			}
			break;
		case DIMOFS_BUTTON1:
			if (od.dwData & 0x80) {
				m_mouseData.isButtonDown[1] = true;
			} else {
				m_mouseData.isButtonDown[1] = false;
			}
			break;
		}
	}

	GetCursorPos(&m_mouseData.pos);
	ScreenToClient(m_hWnd, &m_mouseData.pos);

	if (m_mouseData.isButtonDown[0]) {
		m_primitiveTouchEvents[0].eventState = 2;
	} else {
		m_primitiveTouchEvents[0].eventState = 0;
	}
	m_primitiveTouchEvents[0].currentPosition.x = m_mouseData.pos.x * 2;
	m_primitiveTouchEvents[0].currentPosition.y = m_mouseData.pos.y * 2;

	return SCE_OK;
}

int ssii::TouchContextImpl::getNumberOfPrimitiveTouchEvents() const
{
	if (m_mouseData.isButtonDown[0]) {
		return 1;
	} else {
		return 0;
	}
}

const ssii::TouchContextImpl::PrimitiveTouchEvent* ssii::TouchContextImpl::getPrimitiveTouchEvents() const
{
	return m_primitiveTouchEvents;
}

int ssi::createTouchContext(ssi::TouchContext **outTouchContext, HWND hWnd)
{
	if (outTouchContext == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssii::TouchContextImpl *t = new ssii::TouchContextImpl;
	int ret = t->initialize(hWnd);
	if (ret != SCE_OK) {
		delete t;
		return ret;
	}
	*outTouchContext = t;
	return ret;
}

#endif	/* _SCE_TARGET_OS_WINDOWS */
