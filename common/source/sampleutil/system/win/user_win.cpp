/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <sampleutil/sampleutil_common.h>
#include <sampleutil/system.h>
#include <sampleutil/input.h>

#if defined _SCE_TARGET_OS_WINDOWS &&  _SCE_TARGET_OS_WINDOWS

#if defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS
#include <pad.h>
#else/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800 /* DIRECTINPUT_HEADER_VERSION */
#endif/*DIRECTINPUT_VERSION*/
#include <windows.h>
#include <dinput.h>
#include <dinputd.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "DXGUID.LIB")

#endif/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */

/* ------------------------------------------------------------------------- */
namespace sce
{
	namespace SampleUtil
	{
		namespace System
		{
			namespace Impl
			{
				class UserIdManagerImplForWindows : public UserIdManager
				{
				public:
					UserIdManagerImplForWindows();
					~UserIdManagerImplForWindows();

					int initialize(UserIdManagerOption *option);

					int update(void);
					int getLoginUserIdList(UserIdList *list);
					int getUserLoginStatusChangedEventsOfLastUpdate(UserLoginStatusChangedEvents *events);
					int getInitialUser(UserId *userId);
					int getUserName(const UserId userId, char *userName, const size_t size);
					int getUserColor(const UserId userId, UserColor *userColor);

				private:
					sce::SampleUtil::System::UserIdList						userIdList;
					sce::SampleUtil::System::UserLoginStatusChangedEvents	loginLogoutUserIdList;

					/* ------ For binding UserID and GamePad ------ */
					class PadList{
					public:
						PadList(){psButtonPressed = false;};
						~PadList(){};

						bool					psButtonPressed;
						DIDEVICEINSTANCE		deviceInstance;
						LPDIRECTINPUTDEVICE8	pJoystick;
					};
					PadList m_padList[SCE_SAMPLE_UTIL_MAX_LOGIN_USERS];
					int m_numOfPads;
				};
			}
		}
	}
}

/* ------------------------------------------------------------------------- */
namespace sss  = sce::SampleUtil::System;
namespace sssi = sce::SampleUtil::System::Impl;

/* ------------------------------------------------------------------------- */
static int _idListContainsId(sss::UserIdList *list, sss::UserId id)
{
	if (list == NULL) {
		return -1;
	}

	for (int i=0; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++i) {
		if (list->userId[i] == id) {
			return i;
		}
	}

	return -1;
}

/* ------------------------------------------------------------------------- */
static void _clearIdList(sss::UserIdList *list)
{
	if (list == NULL) {
		return;
	}

	for (int i=0; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++i) {
		list->userId[i] = SCE_SAMPLE_UTIL_USER_ID_INVALID;
	}

	return;
}

/* ------------------------------------------------------------------------- */
static void _addIdListId(sss::UserIdList *list, sss::UserId id)
{
	if (list == NULL) {
		return;
	}

	// search empty slot
	for (int i=0 ; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++i) {
		// write ID to empty slot
		if (list->userId[i] == SCE_SAMPLE_UTIL_USER_ID_INVALID) {
			list->userId[i] = id;
			return;
		}
	}

	return;
}

/* ------------------------------------------------------------------------- */
static void _removeIdListId(sss::UserIdList *list, sss::UserId id)
{
	if (list == NULL) {
		return;
	}

	// search ID slot
	int i;
	for (i=0; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++i) {
		// write ID to empty slot
		if (list->userId[i] == id) {
			list->userId[i] = SCE_SAMPLE_UTIL_USER_ID_INVALID;
			break;
		}
	}

	// remove empty slot
	for (i+=1; i < SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++i) {
		list->userId[i-1] = list->userId[i];
		list->userId[i]   = SCE_SAMPLE_UTIL_USER_ID_INVALID;
	}

	return;
}

/* ------------------------------------------------------------------------- */
int sss::createUserIdManager(sss::UserIdManager **outUserIdManager, UserIdManagerOption *option)
{
	if (outUserIdManager == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	sssi::UserIdManagerImplForWindows *userIdManager = new sssi::UserIdManagerImplForWindows();
	int ret = userIdManager->initialize(option);
	if (ret != SCE_OK) {
		delete userIdManager;
		return ret;
	}

	*outUserIdManager = userIdManager;

	return SCE_OK;
}


/* ------------------------------------------------------------------------- */
sssi::UserIdManagerImplForWindows::UserIdManagerImplForWindows()
{

}

/* ------------------------------------------------------------------------- */
sssi::UserIdManagerImplForWindows::~UserIdManagerImplForWindows()
{
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForWindows::initialize(sss::UserIdManagerOption *option)
{
	int ret = SCE_OK;

#if defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS

	userIdList.userId[0] = SCE_USER_SERVICE_STATIC_USER_ID_1;
	userIdList.userId[1] = SCE_USER_SERVICE_STATIC_USER_ID_2;
	userIdList.userId[2] = SCE_USER_SERVICE_STATIC_USER_ID_3;
	userIdList.userId[3] = SCE_USER_SERVICE_STATIC_USER_ID_4;

#else/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */

	DIDEVICEINSTANCE list[SCE_SAMPLE_UTIL_MAX_LOGIN_USERS];
	sce::SampleUtil::Input::getPadList(list, SCE_SAMPLE_UTIL_MAX_LOGIN_USERS, &this->m_numOfPads);

	for (int i=0; i<this->m_numOfPads; ++i) {
		this->m_padList[i].deviceInstance = list[i];

		ret = sce::SampleUtil::Input::createDirectInputDevice(
			this->m_padList[i].deviceInstance.guidInstance,
			&this->m_padList[i].pJoystick);
		if (0 <= ret) {
			this->m_padList[i].pJoystick->SetDataFormat(&c_dfDIJoystick2);
		}
	}

	// Initialzie User Lists
	this->getInitialUser(&this->userIdList.userId[0]);
	userIdList.userId[1] = SCE_SAMPLE_UTIL_USER_ID_INVALID;
	userIdList.userId[2] = SCE_SAMPLE_UTIL_USER_ID_INVALID;
	userIdList.userId[3] = SCE_SAMPLE_UTIL_USER_ID_INVALID;

#endif/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */

	for (int i=0; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++i) {
		this->loginLogoutUserIdList.joinedUserIdList.userId[i] = SCE_SAMPLE_UTIL_USER_ID_INVALID;
		this->loginLogoutUserIdList.leftUserIdList.userId[i]   = SCE_SAMPLE_UTIL_USER_ID_INVALID;
	}

	return ret;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForWindows::update(void)
{
	int ret = SCE_OK;

#if !(defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS)

	// Clear Logint/Logout List
	_clearIdList(&this->loginLogoutUserIdList.joinedUserIdList);
	_clearIdList(&this->loginLogoutUserIdList.leftUserIdList);

	// Check PS Button
	const BYTE bPRESSED			= 0x80;
	const BYTE PS_BUTTON_IDX	= 12;
	
	for (int i=1; i<this->m_numOfPads; ++i) {
		if (this->m_padList[i].pJoystick != NULL) {
			this->m_padList[i].pJoystick->Acquire();
			ret = this->m_padList[i].pJoystick->Poll();
			if (ret < DI_OK) {
				continue;
			}

			DIJOYSTATE2 joystate;
			ret = this->m_padList[i].pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &joystate);
			if (ret < 0) {
				continue;
			}

			// Handle PS Button.
			if (joystate.rgbButtons[PS_BUTTON_IDX] & bPRESSED && this->m_padList[i].psButtonPressed == false) {
				this->m_padList[i].psButtonPressed = true;

				int itemIndex = _idListContainsId(&this->userIdList, &this->m_padList[i].deviceInstance);
				if (itemIndex >= 0) {
					//remove from UserIDList
					_removeIdListId(&this->userIdList, &this->m_padList[i].deviceInstance);

					//Add this ID to LogoutList
					_addIdListId(&this->loginLogoutUserIdList.leftUserIdList, &this->m_padList[i].deviceInstance);
				} else {
					//Add this ID to UserIDList
					_addIdListId(&this->userIdList, &this->m_padList[i].deviceInstance);

					//Add this ID to LoginList
					_addIdListId(&this->loginLogoutUserIdList.joinedUserIdList, &this->m_padList[i].deviceInstance);

				}
			} else if (0 == (joystate.rgbButtons[PS_BUTTON_IDX] & bPRESSED) && this->m_padList[i].psButtonPressed == true) {
				this->m_padList[i].psButtonPressed = false;
			}
		}
	}

#endif/* !(defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) */

	return ret;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForWindows::getLoginUserIdList(sss::UserIdList *list)
{
	if (list == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	memcpy(list, &this->userIdList, sizeof(sss::UserIdList));

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForWindows::getUserLoginStatusChangedEventsOfLastUpdate(sss::UserLoginStatusChangedEvents *events)
{
	if (events == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	memcpy(events, &this->loginLogoutUserIdList, sizeof(sss::UserLoginStatusChangedEvents));

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForWindows::getInitialUser(sss::UserId *userId)
{
	if (userId == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

#if defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS
	*userId = userIdList.userId[0];
#else/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */
	if (this->m_numOfPads > 0 && this->m_padList[0].deviceInstance.dwSize != 0) {
		*userId = &this->m_padList[0].deviceInstance;
	} else {
		*userId = NULL;
	}
#endif/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForWindows::getUserName(const sss::UserId userId, char *userName, const size_t size)
{
	if (userName == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (userId == SCE_SAMPLE_UTIL_USER_ID_INVALID) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	for (int i=0;i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS;i++) {
		if (userIdList.userId[i] == userId) {
			switch (i) {
			case 0:
				strncpy(userName, "User1", size);
				break;
			case 1:
				strncpy(userName, "User2", size);
				break;
			case 2:
				strncpy(userName, "User3", size);
				break;
			case 3:
				strncpy(userName, "User4", size);
				break;
			default:
				return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
				break;
			}
			return SCE_OK;
		}
	}

	return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForWindows::getUserColor(const sss::UserId userId, sss::UserColor *userColor)
{
	if (userColor == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (userId == SCE_SAMPLE_UTIL_USER_ID_INVALID) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	for (int i=0;i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS;i++) {
		if (userIdList.userId[i] == userId) {
			switch (i) {
			case 0:
				*userColor = kUserColorBlue;
				break;
			case 1:
				*userColor = kUserColorRed;
				break;
			case 2:
				*userColor = kUserColorGreen;
				break;
			case 3:
				*userColor = kUserColorPink;
				break;
			default:
				return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
				break;
			}
			return SCE_OK;
		}
	}

	return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
}

#endif //defined _SCE_TARGET_OS_WINDOWS &&  _SCE_TARGET_OS_WINDOWS
