/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <sampleutil/sampleutil_common.h>
#include <sampleutil/system.h>
#include <sampleutil/input.h>


#if defined _SCE_TARGET_OS_PSP2 &&  _SCE_TARGET_OS_PSP2
namespace sce
{
	namespace SampleUtil
	{
		namespace System
		{
			namespace Impl
			{
				class UserIdManagerImplForPsp2 : public UserIdManager
				{
				public:
					UserIdManagerImplForPsp2();
					~UserIdManagerImplForPsp2();

					int initialize(UserIdManagerOption *option);

					int update(void);
					int getLoginUserIdList(UserIdList *list);
					int getUserLoginStatusChangedEventsOfLastUpdate(UserLoginStatusChangedEvents *events);
					int getInitialUser(UserId *userId);
					int getUserName(const UserId userId, char *userName, const size_t size);
					int getUserColor(const UserId userId, UserColor *userColor);
				};
			}
		}
	}
}

/* ------------------------------------------------------------------------- */
namespace sss  = sce::SampleUtil::System;
namespace sssi = sce::SampleUtil::System::Impl;


/* ------------------------------------------------------------------------- */
int sss::createUserIdManager(sss::UserIdManager **outUserIdManager, UserIdManagerOption *option)
{
	if (outUserIdManager == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	sssi::UserIdManagerImplForPsp2 *userIdManager = new sssi::UserIdManagerImplForPsp2();
	int ret = userIdManager->initialize(option);
	if (ret != SCE_OK) {
		delete userIdManager;
		return ret;
	}

	*outUserIdManager = userIdManager;

	return SCE_OK;
}


/* ------------------------------------------------------------------------- */
sssi::UserIdManagerImplForPsp2::UserIdManagerImplForPsp2()
{

}

/* ------------------------------------------------------------------------- */
sssi::UserIdManagerImplForPsp2::~UserIdManagerImplForPsp2()
{
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForPsp2::initialize(sss::UserIdManagerOption *option)
{
	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForPsp2::update(void)
{
	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForPsp2::getLoginUserIdList(sss::UserIdList *list)
{
	if (list == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	list->userId[0] = SCE_SAMPLE_UTIL_USER_ID_DEFAULT;

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForPsp2::getUserLoginStatusChangedEventsOfLastUpdate(sss::UserLoginStatusChangedEvents *events)
{
	if (events == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	for (int i=0 ; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++i) {
		events->joinedUserIdList.userId[i] = SCE_SAMPLE_UTIL_USER_ID_INVALID;
		events->leftUserIdList.userId[i]   = SCE_SAMPLE_UTIL_USER_ID_INVALID;
	}

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForPsp2::getInitialUser(sss::UserId *userId)
{
	if (userId == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	*userId = SCE_SAMPLE_UTIL_USER_ID_DEFAULT;

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForPsp2::getUserName(const sss::UserId userId, char *userName, const size_t size)
{
	if (userName == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (userId != SCE_SAMPLE_UTIL_USER_ID_DEFAULT) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	strncpy(userName, "User1", size);

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int sssi::UserIdManagerImplForPsp2::getUserColor(const UserId userId, UserColor *userColor)
{
	if (userColor == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (userId != SCE_SAMPLE_UTIL_USER_ID_DEFAULT) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	*userColor = kUserColorBlue;

	return SCE_OK;
}

#endif//defined _SCE_TARGET_OS_PSP2 &&  _SCE_TARGET_OS_PSP2
