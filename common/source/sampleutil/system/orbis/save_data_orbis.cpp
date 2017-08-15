/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <scebase_common.h>
#include "sampleutil_internal.h"
#include <sampleutil/sampleutil_error.h>
#include <sampleutil/system.h>
#include <util/util.h>

#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <kernel.h>
#include <user_service.h>
#include <save_data.h>

#pragma comment(lib,"libSceSaveData_stub_weak.a")
#pragma comment(lib,"libSceRtc_stub_weak.a")

#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX		"slot"
#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH	8
#define _SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH			255
#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_CREATE_MINIMUM_SIZE 10240

namespace sce
{
	namespace SampleUtil
	{
		namespace System
		{
			namespace Impl
			{
				class SaveDataImpl : public SaveData, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					SaveDataImpl(void);
					~SaveDataImpl(void)
					{
						finalize();
					}

					int initialize(const SaveDataOption *option);
					int finalize(void);

					int slotCreate(const UserId userId, const SaveDataSlotId slotId, const SaveDataSlotParam *param, const size_t sizeKiB);
					int slotDelete(const UserId userId, const SaveDataSlotId slotId);
					int slotSetParam(const UserId userId, const SaveDataSlotId slotId, const SaveDataSlotParam *param);
					int slotGetParam(const UserId userId, const SaveDataSlotId slotId, SaveDataSlotParam *outParam);
					int slotSearch(const UserId userId, const SaveDataSearchCond *cond, SaveDataSearchResult *outResult);
					int dataSave(const UserId userId, const SaveDataSlotId slotId, const DataSaveItem *data, const uint32_t dataNum, size_t *outRequiredSizeKiB);
					int dataLoadMount(const UserId userId, const SaveDataSlotId slotId, SaveDataMountPoint *outMountPoint);
					int dataLoadUmount(const SaveDataMountPoint *mountPoint);
					int dataRemove(const UserId userId, const SaveDataSlotId slotId, const DataRemoveItem *data, const uint32_t dataNum);

					int getRequiredBlocksForSaveDataMount(SceSaveDataBlocks *outRequiredBlocks);

					virtual std::string getResourceType(void) const
					{
						return "System::SaveData";
					}

				private:
					void setupSceSaveDataMount(const UserId userId,
											   const SceSaveDataDirName *dirName,
											   const SceSaveDataMountMode mode,
											   const size_t sizeKiB,
											   SceSaveDataMount2 *mount);

					bool m_hasRequiredBlocks;
					SceSaveDataBlocks m_requiredBlocks;

					int saveDataMount(const SceSaveDataMount2 *mount, SceSaveDataMountResult *mountResult);
				};
			}
		}
	}
}

namespace sss = sce::SampleUtil::System;
namespace sssi = sce::SampleUtil::System::Impl;

static void setDirName(const sss::SaveDataSlotId slotId, char *path)
{
	if (path != NULL) {
		snprintf(path, _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH,
				 "%s%03d", _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX, slotId);
	}
}

sssi::SaveDataImpl::SaveDataImpl(void)
{
	m_hasRequiredBlocks = false;
	m_requiredBlocks = 0;
}

int sssi::SaveDataImpl::initialize(const SaveDataOption *option)
{
	int ret = sceSaveDataInitialize3(NULL);
	if (ret < SCE_OK) {
		return ret;
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::finalize(void)
{
	int ret = sceSaveDataTerminate();
	return ret;
}

static void setDirName(const sss::SaveDataSlotId slotId, SceSaveDataDirName *dirName)
{
	if (dirName != NULL) {
		setDirName(slotId, dirName->data);
	}
}

static int getSlotId(const sce::SampleUtil::System::UserId userId, const SceSaveDataDirName *dirName, sss::SaveDataSlotId *outSlotId)
{
	if (dirName == NULL || outSlotId == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	size_t prefixSize = sizeof(_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX) -1;

	if (strncmp(dirName->data, _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX, prefixSize) != 0) {
		return -1;
	}

	char num[4];
	memset(num, 0, 4);
	strncpy(num, &dirName->data[prefixSize], 3);
	*outSlotId = atoi(num);

	return SCE_OK;
}

int sssi::SaveDataImpl::slotCreate(const sce::SampleUtil::System::UserId userId,
								   const SaveDataSlotId slotId,
								   const SaveDataSlotParam *param,
								   const size_t sizeKiB)
{
	if (slotId >= SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if (sizeKiB < _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_CREATE_MINIMUM_SIZE) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	SceSaveDataDirName dirName;
	setDirName(slotId, &dirName);

	SceSaveDataMount2 mount;
	setupSceSaveDataMount(userId, &dirName, (SCE_SAVE_DATA_MOUNT_MODE_CREATE | SCE_SAVE_DATA_MOUNT_MODE_RDWR), sizeKiB, &mount);

	SceSaveDataMountResult mountResult;
	memset(&mountResult, 0x00, sizeof(mountResult));
	int ret = saveDataMount(&mount, &mountResult);
	if (ret < 0) {
		return ret;
	}

	SceSaveDataMountPoint *mountPoint = &mountResult.mountPoint;

	ret = sceSaveDataUmount(mountPoint);
	if (ret < 0) {
		return ret;
	}

	if (param != NULL) {
		ret = slotSetParam(userId, slotId, param);
		if (ret < 0) {
			int ret2 = slotDelete(userId, slotId);
			(void)ret2;
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret2, SCE_OK);
			return ret;
		}
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::slotDelete(const sce::SampleUtil::System::UserId userId, const SaveDataSlotId slotId)
{
	SceSaveDataDirName dirName;
	setDirName(slotId, &dirName);

	SceSaveDataDelete del;

	memset(&del, 0x00, sizeof(SceSaveDataDelete));
	del.userId = userId;
	del.titleId = NULL;
	del.dirName = &dirName;
	int ret = sceSaveDataDelete(&del);

	return ret;
}

int sssi::SaveDataImpl::slotSetParam(const sce::SampleUtil::System::UserId userId, const SaveDataSlotId slotId, const SaveDataSlotParam *param)
{
	if (param == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceSaveDataDirName dirName;
	setDirName(slotId, &dirName);

	SceSaveDataMount2 mount;
	setupSceSaveDataMount(userId, &dirName, (SCE_SAVE_DATA_MOUNT_MODE_RDWR), 0, &mount);

	SceSaveDataMountResult mountResult;
	memset(&mountResult, 0x00, sizeof(mountResult));
	int ret = saveDataMount(&mount, &mountResult);
	if (ret < 0) {
		return ret;
	}

	SceSaveDataMountPoint *mountPoint = &mountResult.mountPoint;


	SceSaveDataParam nativeParam;
	memset(&nativeParam, 0x00, sizeof(SceSaveDataParam));

	strncpy(nativeParam.title, param->title, SCE_SAVE_DATA_TITLE_MAXSIZE-1);
	nativeParam.title[SCE_SAVE_DATA_TITLE_MAXSIZE-1] = '\0';

	strncpy(nativeParam.subTitle, param->subTitle, SCE_SAVE_DATA_SUBTITLE_MAXSIZE-1);
	nativeParam.subTitle[SCE_SAVE_DATA_SUBTITLE_MAXSIZE-1] = '\0';

	strncpy(nativeParam.detail, param->detail, SCE_SAVE_DATA_DETAIL_MAXSIZE-1);
	nativeParam.detail[SCE_SAVE_DATA_DETAIL_MAXSIZE-1] = '\0';

	nativeParam.userParam = param->userParam;

	ret = sceSaveDataSetParam(mountPoint,
							  SCE_SAVE_DATA_PARAM_TYPE_ALL,
							  &nativeParam, sizeof(SceSaveDataParam));
	if (ret < 0) {
		int ret2 = sceSaveDataUmount(mountPoint);
		(void)ret2;
		SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
		return ret;
	}

	if (param->icon.buf != NULL) {
		SceSaveDataIcon icon;
		memset(&icon, 0x00, sizeof(SceSaveDataIcon));
		icon.buf = param->icon.buf;
		icon.bufSize = param->icon.bufSize;
		icon.dataSize = param->icon.dataSize;
		ret = sceSaveDataSaveIcon(mountPoint, &icon);
		if (ret < 0) {
			int ret2 = sceSaveDataUmount(mountPoint);
			(void)ret2;
			SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
			return ret;
		}
	}

	ret = sceSaveDataUmount(mountPoint);
	if(ret < 0){
		return ret;
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::slotGetParam(const sce::SampleUtil::System::UserId userId, const SaveDataSlotId slotId, SaveDataSlotParam *outParam)
{
	if (outParam == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceSaveDataDirName dirName;
	setDirName(slotId, &dirName);

	SceSaveDataMount2 mount;
	setupSceSaveDataMount(userId, &dirName, (SCE_SAVE_DATA_MOUNT_MODE_RDONLY), 0, &mount);

	SceSaveDataMountResult mountResult;
	memset(&mountResult, 0x00, sizeof(mountResult));
	int ret = saveDataMount(&mount, &mountResult);
	if (ret < 0) {
		return ret;
	}

	SceSaveDataMountPoint *mountPoint = &mountResult.mountPoint;

	SceSaveDataParam nativeParam;
	memset(&nativeParam, 0x00, sizeof(SceSaveDataParam));

	size_t gotSize = 0;
	ret = sceSaveDataGetParam(mountPoint,
							  SCE_SAVE_DATA_PARAM_TYPE_ALL,
							  &nativeParam, sizeof(SceSaveDataParam), &gotSize);
	if (ret < 0) {
		int ret2 = sceSaveDataUmount(mountPoint);
		(void)ret2;
		SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
		return ret;
	}

	outParam->status = kSaveDataSlotStatusAvailable;

	strncpy(outParam->title, nativeParam.title, SCE_SAVE_DATA_TITLE_MAXSIZE-1);
	nativeParam.title[SCE_SAVE_DATA_TITLE_MAXSIZE-1] = '\0';

	strncpy(outParam->subTitle, nativeParam.subTitle, SCE_SAVE_DATA_SUBTITLE_MAXSIZE-1);
	nativeParam.subTitle[SCE_SAVE_DATA_SUBTITLE_MAXSIZE-1] = '\0';

	strncpy(outParam->detail, nativeParam.detail, SCE_SAVE_DATA_DETAIL_MAXSIZE-1);
	nativeParam.detail[SCE_SAVE_DATA_DETAIL_MAXSIZE-1] = '\0';

	outParam->userParam = nativeParam.userParam;

	ret = sceRtcSetTime_t(&outParam->mtime, nativeParam.mtime);
	if (ret < 0) {
		int ret2 = sceSaveDataUmount(mountPoint);
		(void)ret2;
		SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
		return ret;
	}

	if (outParam->icon.buf != NULL) {
		SceSaveDataIcon icon;
		memset(&icon, 0x00, sizeof(SceSaveDataIcon));
		icon.buf = outParam->icon.buf;
		icon.bufSize = outParam->icon.bufSize;
		ret = sceSaveDataLoadIcon(mountPoint, &icon);
		if (ret < 0) {
			int ret2 = sceSaveDataUmount(mountPoint);
			(void)ret2;
			SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
			return ret;
		}
		outParam->icon.dataSize = icon.dataSize;
	} else {
		outParam->icon.dataSize = 0;
	}

	ret = sceSaveDataUmount(mountPoint);
	if (ret < 0) {
		return ret;
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::slotSearch(const sce::SampleUtil::System::UserId userId, const SaveDataSearchCond *cond, SaveDataSearchResult *outResult)
{
	if (cond == NULL || outResult == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceSaveDataDirNameSearchCond nativeCond;
	memset(&nativeCond, 0x00, sizeof(SceSaveDataDirNameSearchCond));
	nativeCond.userId = userId;
	nativeCond.key = cond->key;
	nativeCond.order = cond->order;
	//E If NULL is set, titleId of the launched application is set as the condition.
	//J NULL を設定しておくと起動中のアプリの品番が対象となる
	nativeCond.titleId = NULL;

	SceSaveDataDirNameSearchResult nativeResult;
	memset(&nativeResult, 0x00, sizeof(SceSaveDataDirNameSearchResult));
	nativeResult.dirNames = new SceSaveDataDirName[SCE_SAVE_DATA_DIRNAME_MAX_COUNT];
	if (!nativeResult.dirNames) {
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}
	nativeResult.dirNamesNum = SCE_SAVE_DATA_DIRNAME_MAX_COUNT;

	int ret = sceSaveDataDirNameSearch(&nativeCond, &nativeResult);
	if (ret < 0) {
		delete [] nativeResult.dirNames;
		return ret;
	}

	SaveDataSlotId slotId;
	uint32_t count = 0;

	outResult->hitNum = 0;

	for (uint32_t i=0; i<nativeResult.hitNum; i++) {
		ret = getSlotId(userId, &nativeResult.dirNames[i], &slotId);
		if (ret == SCE_OK) {
			if (slotId >= cond->from && slotId < cond->from + cond->range) {
				outResult->hitNum++;
				if (count < outResult->slotListNum) {
					outResult->slotList[count] = slotId;
					count++;
				}
			}
		}
	}
	delete [] nativeResult.dirNames;
	return SCE_OK;
}

int sssi::SaveDataImpl::dataSave(const sce::SampleUtil::System::UserId userId,
								 const SaveDataSlotId slotId,
								 const DataSaveItem *data,
								 const uint32_t dataNum,
								 size_t *outRequiredSizeKiB)
{
	if (data == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceSaveDataDirName dirName;
	setDirName(slotId, &dirName);

	SceSaveDataMount2 mount;
	setupSceSaveDataMount(userId, &dirName, (SCE_SAVE_DATA_MOUNT_MODE_RDWR), 0, &mount);

	SceSaveDataMountResult mountResult;
	memset(&mountResult, 0x00, sizeof(mountResult));
	int ret = saveDataMount(&mount, &mountResult);
	if (ret < 0) {
		return ret;
	}

	SceSaveDataMountPoint *mountPoint = &mountResult.mountPoint;

	char path[_SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH + 1];
	int fd = -1;

	for (uint64_t i=0;i<dataNum;i++) {
		snprintf(path, sizeof(path), "%s/%s", mountPoint->data, data[i].dataPath);

		switch (data[i].mode) {
		case SaveDataSaveMode::kSaveDataSaveModeFile:
			fd = sceKernelOpen(path, SCE_KERNEL_O_WRONLY|SCE_KERNEL_O_TRUNC|SCE_KERNEL_O_CREAT, SCE_KERNEL_S_IRWU);
			if (fd < SCE_OK) {
				int ret2 = sceSaveDataUmount(mountPoint);
				(void)ret2;
				SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
				return fd;
			}
			ret = static_cast<int32_t>(sceKernelWrite(fd, data[i].buf, data[i].bufSize));
			sceKernelClose(fd);

			if (ret < SCE_OK) {
				int ret2 = sceSaveDataUmount(mountPoint);
				(void)ret2;
				SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
				return ret;
			}
			break;
		case SaveDataSaveMode::kSaveDataSaveModeFileTruncate:
			fd = sceKernelOpen(path, SCE_KERNEL_O_WRONLY|SCE_KERNEL_O_TRUNC|SCE_KERNEL_O_CREAT, SCE_KERNEL_S_IRWU);
			if (fd < SCE_OK) {
				int ret2 = sceSaveDataUmount(mountPoint);
				(void)ret2;
				SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
				return fd;
			}

			ret = sceKernelLseek(fd, data[i].offset, SCE_KERNEL_SEEK_SET);
			if (ret < SCE_OK) {
				sceKernelClose(fd);
				int ret2 = sceSaveDataUmount(mountPoint);
				(void)ret2;
				SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
				return SCE_OK;
			}

			ret = static_cast<int32_t>(sceKernelWrite(fd, data[i].buf, data[i].bufSize));
			sceKernelClose(fd);

			if (ret < SCE_OK) {
				int ret2 = sceSaveDataUmount(mountPoint);
				(void)ret2;
				SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
				return ret;
			}
			break;
		case SaveDataSaveMode::kSaveDataSaveModeDirectory:
			ret = sceKernelMkdir(path, SCE_KERNEL_S_IRUSR | SCE_KERNEL_S_IXUSR);
			if (ret != SCE_KERNEL_ERROR_EEXIST && ret < SCE_OK) {
				int ret2 = sceSaveDataUmount(mountPoint);
				(void)ret2;
				SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
				return ret;
			}
			break;
		default:
			break;
		}
	}

	ret = sceSaveDataUmount(mountPoint);
	return ret;
}

int sssi::SaveDataImpl::dataLoadMount(const sce::SampleUtil::System::UserId userId, const SaveDataSlotId slotId, SaveDataMountPoint *outMountPoint)
{
	if (outMountPoint == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceSaveDataDirName dirName;
	setDirName(slotId, &dirName);

	SceSaveDataMount2 mount;
	setupSceSaveDataMount(userId, &dirName, (SCE_SAVE_DATA_MOUNT_MODE_RDONLY), 0, &mount);

	SceSaveDataMountResult mountResult;
	memset(&mountResult, 0x00, sizeof(mountResult));
	int ret = saveDataMount(&mount, &mountResult);
	if (ret < 0) {
		return ret;
	}

	memset(outMountPoint, 0x00, sizeof(SaveDataMountPoint));
	memcpy(outMountPoint->data, mountResult.mountPoint.data, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	return SCE_OK;
}

int sssi::SaveDataImpl::dataLoadUmount(const SaveDataMountPoint *mountPoint)
{
	if (mountPoint == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceSaveDataMountPoint _mountPoint;
	memset(&_mountPoint, 0x00, sizeof(SceSaveDataMountPoint));
	memcpy(_mountPoint.data, mountPoint->data, SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE);

	int ret = sceSaveDataUmount(&_mountPoint);
	return ret;
}

int sssi::SaveDataImpl::dataRemove(const sce::SampleUtil::System::UserId userId,
								   const SaveDataSlotId slotId,
								   const DataRemoveItem *data,
								   const uint32_t dataNum)
{
	if (data == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceSaveDataDirName dirName;
	setDirName(slotId, &dirName);

	SceSaveDataMount2 mount;
	setupSceSaveDataMount(userId, &dirName, (SCE_SAVE_DATA_MOUNT_MODE_RDWR), 0, &mount);

	SceSaveDataMountResult mountResult;
	memset(&mountResult, 0x00, sizeof(mountResult));
	int ret = saveDataMount(&mount, &mountResult);
	if (ret < 0) {
		return ret;
	}

	SceSaveDataMountPoint *mountPoint = &mountResult.mountPoint;

	char path[_SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH + 1];

	for (uint64_t i=0;i<dataNum;i++) {
		snprintf(path, sizeof(path), "%s/%s", mountPoint->data, data[i].dataPath);

		ret = sceKernelUnlink(path);
		if (ret == SCE_KERNEL_ERROR_EISDIR || ret == SCE_KERNEL_ERROR_EPERM) {
			ret = sceKernelRmdir(path);
			if (ret < SCE_OK) {
				int ret2 = sceSaveDataUmount(mountPoint);
				(void)ret2;
				SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
				return ret;
			}
		} else if(ret < SCE_OK) {
			int ret2 = sceSaveDataUmount(mountPoint);
			(void)ret2;
			SCE_SAMPLE_UTIL_ASSERT(ret2 == SCE_OK || ret2 == SCE_SAVE_DATA_ERROR_NOT_MOUNTED);
			return ret;
		}
	}

	ret = sceSaveDataUmount(mountPoint);
	return ret;
}

int sssi::SaveDataImpl::getRequiredBlocksForSaveDataMount(SceSaveDataBlocks *outRequiredBlocks)
{
	if (outRequiredBlocks == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (m_hasRequiredBlocks) {
		*outRequiredBlocks = m_requiredBlocks;
		return SCE_OK;
	}

	return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
}

void sssi::SaveDataImpl::setupSceSaveDataMount(const sce::SampleUtil::System::UserId userId,
											   const SceSaveDataDirName *dirName,
											   const SceSaveDataMountMode mode,
											   const size_t sizeKiB,
											   SceSaveDataMount2 *mount)
{
	if (mount != NULL) {
		memset(mount, 0x00, sizeof(SceSaveDataMount2));
		mount->userId = userId;
		mount->dirName = dirName;
		mount->mountMode = mode;

		uint32_t dataBlockSizeKiB = SCE_SAVE_DATA_BLOCK_SIZE/1024;
		size_t alienedDataSizeKiB = Internal::Util::ceil(sizeKiB, (SCE_SAVE_DATA_BLOCK_SIZE/1024));
		mount->blocks = alienedDataSizeKiB/dataBlockSizeKiB;
	}
}

int sssi::SaveDataImpl::saveDataMount(const SceSaveDataMount2 *mount, SceSaveDataMountResult *mountResult)
{
	SCE_SAMPLE_UTIL_ASSERT(mount != NULL);
	SCE_SAMPLE_UTIL_ASSERT(mountResult != NULL);

	int ret = sceSaveDataMount2(mount, mountResult);

	if (ret == SCE_SAVE_DATA_ERROR_NO_SPACE_FS) {
		m_hasRequiredBlocks = true;
		m_requiredBlocks = mountResult->requiredBlocks;
	} else {
		m_hasRequiredBlocks = false;
		m_requiredBlocks = 0;
	}
	return ret;
}

int sss::createSaveData(sss::SaveData **outSaveData, const sss::SaveDataOption *option)
{
	if (outSaveData == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	sssi::SaveDataImpl *saveDataImpl = new sssi::SaveDataImpl();
	int ret = saveDataImpl->initialize(option);
	if (ret != SCE_OK) {
		delete saveDataImpl;
		return ret;
	}

	*outSaveData = (sss::SaveData *)saveDataImpl;
	return SCE_OK;
}

#endif //defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
