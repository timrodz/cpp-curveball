/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <scebase_common.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil/system.h>
#include "sampleutil_internal.h"

#if defined(_SCE_TARGET_OS_WINDOWS) && _SCE_TARGET_OS_WINDOWS

#include <algorithm>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define _SCE_SAMPLE_UTIL_SAVE_DATA_DIRNAME					_TEXT("savedata")
#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX		"slot"
#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_DIR			"sce_sys"
#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_FILE			"slot_info.dat"
#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH	17
#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_PATH_LENGTH	64
#define _SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH			255

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

					virtual std::string getResourceType(void) const
					{
						return "System::SaveData";
					}

				private:
				};

				struct SlotSearchResult{
					SaveDataSlotId slotId;
					SaveDataSlotParam param;
					FILETIME mtime;
					int dataSize;
				};
			}
		}
	}
}

namespace sss = sce::SampleUtil::System;
namespace sssi = sce::SampleUtil::System::Impl;

static void setDirName(const sss::SaveDataSlotId slotId, LPTSTR path)
{
	if (path != NULL) {
		SNTPRINTF(path, _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH,
				 _TEXT("%s/%s%03d"), _SCE_SAMPLE_UTIL_SAVE_DATA_DIRNAME, _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX, slotId);
	}
}

static void setSlotInfoDirPath(const sss::SaveDataSlotId slotId, LPTSTR path)
{
	if (path != NULL) {
		SNTPRINTF(path, _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_PATH_LENGTH,
			_TEXT("%s/%s%03d/%s"),
				 _SCE_SAMPLE_UTIL_SAVE_DATA_DIRNAME,
				 _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX,
				 slotId,
				 _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_DIR);
	}
}

static void setSlotInfoPath(const sss::SaveDataSlotId slotId, SCE_SAMPLE_LPTSTR path)
{
	if (path != NULL) {
		SNTPRINTF(path, _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_PATH_LENGTH,
				 _TEXT("%s/%s%03d/%s/%s"),
				 _SCE_SAMPLE_UTIL_SAVE_DATA_DIRNAME,
				 _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX,
				 slotId,
				 _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_DIR,
				 _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_FILE);
	}
}

static bool existsSlot(const sss::SaveDataSlotId slotId)
{
	SCE_SAMPLE_TCHAR path[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_PATH_LENGTH];
	setSlotInfoPath(slotId, path);

	return PathFileExists(path);
}

static int _slotSetParam(const sss::UserId userId, const sss::SaveDataSlotId slotId, const sss::SaveDataSlotParam *param)
{
	if (param == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SCE_SAMPLE_TCHAR path[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_PATH_LENGTH];
	setSlotInfoDirPath(slotId, path);

	if (!PathIsDirectory(path)) {
		if (!CreateDirectory(path, NULL)) {
			return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
		}
	}

	setSlotInfoPath(slotId, path);
	FILE *fp = FOPEN(path, _TEXT("wb"));

	int ret = 0;

	if (fp != NULL) {
		ret = fwrite(param, sizeof(sss::SaveDataSlotParam), 1, fp);
		fclose(fp);
	} else {
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}

	if (ret == 1) {
		return SCE_OK;
	} else {
		return SCE_SAMPLE_UTIL_ERROR_FILE_SAVE;
	}
}

sssi::SaveDataImpl::SaveDataImpl(void)
{
}

int sssi::SaveDataImpl::initialize(const sss::SaveDataOption *option)
{
	if (!PathIsDirectory(_SCE_SAMPLE_UTIL_SAVE_DATA_DIRNAME)) {
		if (!CreateDirectory(_SCE_SAMPLE_UTIL_SAVE_DATA_DIRNAME, NULL)) {
			return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
		}
	}
	return SCE_OK;
}

int sssi::SaveDataImpl::finalize(void)
{
	return SCE_OK;
}

int sssi::SaveDataImpl::slotCreate(const sss::UserId userId, const sss::SaveDataSlotId slotId, const sss::SaveDataSlotParam *param, const size_t sizeKiB)
{
	if (slotId >= SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if (existsSlot(slotId)) {
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_EXISTS;
	}

	TCHAR path[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH];
	setDirName(slotId, path);

	if (!PathIsDirectory(path)) {
		if (!CreateDirectory(path, NULL)) {
			return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
		}
	}

	int ret = SCE_OK;

	if (param != NULL) {
		ret = _slotSetParam(userId, slotId, param);
	} else {
		sss::SaveDataSlotParam _param;
		memset(&_param, 0, sizeof(sss::SaveDataSlotParam));
		ret = _slotSetParam(userId, slotId, &_param);
	}

	return ret;
}

int sssi::SaveDataImpl::slotDelete(const sss::UserId userId, const sss::SaveDataSlotId slotId)
{
	if (!existsSlot(slotId)) {
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_NOT_FOUND;
	}

	SCE_SAMPLE_TCHAR path[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_PATH_LENGTH];
	setSlotInfoPath(slotId, path);

	int ret = _tremove(path);
	if (ret != 0) {
		return ret;
	}

	setSlotInfoDirPath(slotId, path);
	if (!RemoveDirectory(path)) {
		return SCE_SAMPLE_UTIL_ERROR_DIRECTORY_DELETE;
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::slotSetParam(const sss::UserId userId, const sss::SaveDataSlotId slotId, const sss::SaveDataSlotParam *param)
{
	if (!existsSlot(slotId)) {
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_NOT_FOUND;
	}

	return _slotSetParam(userId, slotId, param);
}

static int getFileTime(SCE_SAMPLE_LPCTSTR path, FILETIME *outFileTime)
{
	if (path == NULL || outFileTime == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	HANDLE file;
	file = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE) {
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}

	GetFileTime(file, NULL, NULL, outFileTime);

	CloseHandle(file);

	return SCE_OK;
}

int sssi::SaveDataImpl::slotGetParam(const sss::UserId userId, const sss::SaveDataSlotId slotId, sss::SaveDataSlotParam *outParam)
{
	if (outParam == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (!existsSlot(slotId)) {
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_NOT_FOUND;
	}

	TCHAR path[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_PATH_LENGTH];
	setSlotInfoPath(slotId, path);
	FILE *fp = _tfopen(path, _TEXT("rb"));

	int ret = 0;

	if (fp != NULL) {
		ret = fread(outParam, sizeof(sss::SaveDataSlotParam), 1, fp);
		fclose(fp);
	} else {
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}

	if (ret != 1) {
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}

	FILETIME fileTime;
	ret = getFileTime(path, &fileTime);
	if (ret < 0) {
		return ret;
	}

	FILETIME localFileTime;
	FileTimeToLocalFileTime(&fileTime, &localFileTime);

	SYSTEMTIME systemTime;
	FileTimeToSystemTime(&localFileTime, &systemTime);

	outParam->mtime.year = systemTime.wYear;
	outParam->mtime.month = systemTime.wMonth;
	outParam->mtime.day = systemTime.wDay;
	outParam->mtime.hour = systemTime.wHour;
	outParam->mtime.minute = systemTime.wMinute;
	outParam->mtime.second = systemTime.wSecond;
	outParam->mtime.microsecond = systemTime.wMilliseconds*1000;

	outParam->status = kSaveDataSlotStatusAvailable;

	return SCE_OK;
}

static bool ascentCompareSlotId(const sssi::SlotSearchResult result1, const sssi::SlotSearchResult result2)
{
	return (result1.slotId < result2.slotId);
}

static bool descentCompareSlotId(const sssi::SlotSearchResult result1, const sssi::SlotSearchResult result2)
{
	return (result1.slotId > result2.slotId);
}

static bool ascentCompareUserParam(const sssi::SlotSearchResult result1, const sssi::SlotSearchResult result2)
{
	return (result1.param.userParam < result2.param.userParam);
}

static bool descentCompareUserParam(const sssi::SlotSearchResult result1, const sssi::SlotSearchResult result2)
{
	return (result1.param.userParam > result2.param.userParam);
}

static bool ascentCompareDataSize(const sssi::SlotSearchResult result1, const sssi::SlotSearchResult result2)
{
	return (result1.dataSize < result2.dataSize);
}

static bool descentCompareDataSize(const sssi::SlotSearchResult result1, const sssi::SlotSearchResult result2)
{
	return (result1.dataSize > result2.dataSize);
}

static bool ascentCompareMtime(const sssi::SlotSearchResult result1, const sssi::SlotSearchResult result2)
{
	LONG ret = CompareFileTime(&(result1.mtime), &(result2.mtime));
	return (ret < 0);
}

static bool descentCompareMtime(const sssi::SlotSearchResult result1, const sssi::SlotSearchResult result2)
{
	LONG ret = CompareFileTime(&(result1.mtime), &(result2.mtime));
	return (ret > 0);
}

static int getDataSize(LPCTSTR path)
{
	if (path == NULL) {
		return 0;
	}

	TCHAR dirPath[_SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH+1];
	SNTPRINTF(dirPath, _SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH, _TEXT("%s/*"), path);

	HANDLE find;
	WIN32_FIND_DATA findData;
	find = FindFirstFile(dirPath, &findData);

	if (find == INVALID_HANDLE_VALUE) {
		return 0;
	}

	int totalSize = 0;
	do {
		if (_tcscmp(findData.cFileName, _TEXT(".")) != 0 && _tcscmp(findData.cFileName, _TEXT("..")) != 0) {
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				SNTPRINTF(dirPath, _SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH, _TEXT("%s/%s"), path, findData.cFileName);
				totalSize += getDataSize(dirPath);
			} else {
				totalSize += findData.nFileSizeLow;
			}
		}
	} while (FindNextFile(find, &findData));

	FindClose(find);

	return totalSize;
}

int sssi::SaveDataImpl::slotSearch(const sss::UserId userId, const sss::SaveDataSearchCond *cond, sss::SaveDataSearchResult *outResult)
{
	if (cond == NULL || outResult == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	int i = 0;
	int ret = 0;
	TCHAR path[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_INFO_PATH_LENGTH];
	FILE *fp = NULL;
	sss::SaveDataSlotParam param;
	sssi::SlotSearchResult result;
	memset(&result, 0, sizeof(sssi::SlotSearchResult));
	std::vector<sssi::SlotSearchResult> resultList;

	//search
	for (i=cond->from;i<cond->from+cond->range;i++) {
		if (existsSlot(i)) {
			setSlotInfoPath(i, path);
			fp = _tfopen(path, _TEXT("rb"));

			if (fp != NULL) {
				ret = fread(&param, sizeof(sss::SaveDataSlotParam), 1, fp);
				fclose(fp);

				if (ret == 1) {
					result.slotId = i;
					result.param = param;

					if (cond->key == kSaveDataSlotSortKeySize) {
						setDirName(i, path);
						result.dataSize = getDataSize(path);
					} else if (cond->key == kSaveDataSlotSortKeyMtime) {
						getFileTime(path, &(result.mtime));
					}

					resultList.push_back(result);
				}
			}
		}
	}

	switch (cond->key) {
	default:
	case kSaveDataSlotSortKeySlotId:
		if (cond->order == kSaveDataSlotSortOrderAscent) {
			std::sort(resultList.begin(), resultList.end(), ascentCompareSlotId);
		} else if (cond->order == kSaveDataSlotSortOrderDescent) {
			std::sort(resultList.begin(), resultList.end(), descentCompareSlotId);
		}
		break;
	case kSaveDataSlotSortKeyUserParam:
		if (cond->order == kSaveDataSlotSortOrderAscent) {
			std::sort(resultList.begin(), resultList.end(), ascentCompareUserParam);
		} else if (cond->order == kSaveDataSlotSortOrderDescent) {
			std::sort(resultList.begin(), resultList.end(), descentCompareUserParam);
		}
		break;
	case kSaveDataSlotSortKeySize:
		if (cond->order == kSaveDataSlotSortOrderAscent) {
			std::sort(resultList.begin(), resultList.end(), ascentCompareDataSize);
		} else if (cond->order == kSaveDataSlotSortOrderDescent) {
			std::sort(resultList.begin(), resultList.end(), descentCompareDataSize);
		}
		break;
	case kSaveDataSlotSortKeyMtime:
		if (cond->order == kSaveDataSlotSortOrderAscent) {
			std::sort(resultList.begin(), resultList.end(), ascentCompareMtime);
		} else if (cond->order == kSaveDataSlotSortOrderDescent) {
			std::sort(resultList.begin(), resultList.end(), descentCompareMtime);
		}
		break;
	}

	outResult->hitNum = resultList.size();

	for (i=0;i<outResult->slotListNum;i++) {
		if (i < outResult->hitNum) {
			outResult->slotList[i] = resultList[i].slotId;
		}
	}

	resultList.clear();

	return SCE_OK;
}

int sssi::SaveDataImpl::dataSave(const sss::UserId userId, const sss::SaveDataSlotId slotId, const sss::DataSaveItem *data, const uint32_t dataNum, size_t *outRequiredSizeKiB)
{
	if (data == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (!existsSlot(slotId)) {
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_NOT_FOUND;
	}

	TCHAR path[_SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH];
	TCHAR dirPath[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH];
	setDirName(slotId, dirPath);

	FILE *fp = NULL;
	int ret = 0;

	for (uint64_t i=0;i<dataNum;i++) {
		SNTPRINTF(path, _SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH, _TEXT("%s/%s"), dirPath, data[i].dataPath);

		switch (data[i].mode) {
		case kSaveDataSaveModeFile:
			fp = _tfopen(path, _TEXT("wb"));
			if (fp == NULL) {
				return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
			}
			ret = fwrite(data[i].buf, data[i].bufSize, 1, fp);
			fclose(fp);

			if (ret != 1) {
				return SCE_SAMPLE_UTIL_ERROR_FILE_SAVE;
			}
			break;
		case kSaveDataSaveModeFileTruncate:
			fp = _tfopen(path, _TEXT("wb"));
			if (fp == NULL) {
				return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
			}

			ret = fseek(fp, data[i].offset, 0);

			if (ret != 0) {
				fclose(fp);
				return SCE_SAMPLE_UTIL_ERROR_FILE_SAVE;
			}

			ret = fwrite(data[i].buf, data[i].bufSize, 1, fp);
			fclose(fp);

			if (ret != 1) {
				return SCE_SAMPLE_UTIL_ERROR_FILE_SAVE;
			}
			break;
		case kSaveDataSaveModeDirectory:
			if (!PathIsDirectory(path)) {
				if (!CreateDirectory(path, NULL)) {
					return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
				}
			}
			break;
		default:
			break;
		}
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::dataLoadMount(const sss::UserId userId, const sss::SaveDataSlotId slotId, sss::SaveDataMountPoint *outMountPoint)
{
	if (outMountPoint == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (!existsSlot(slotId)) {
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_NOT_FOUND;
	}

	TCHAR dirName[SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE];
	setDirName(slotId, outMountPoint->data);

	return SCE_OK;
}

int sssi::SaveDataImpl::dataLoadUmount(const sss::SaveDataMountPoint *mountPoint)
{
	if (mountPoint == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::dataRemove(const sss::UserId userId, const sss::SaveDataSlotId slotId, const sss::DataRemoveItem *data, const uint32_t dataNum)
{
	if (data == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (!existsSlot(slotId)) {
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_NOT_FOUND;
	}

	TCHAR path[_SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH];
	TCHAR dirPath[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH];
	setDirName(slotId, dirPath);

	for (uint64_t i=0;i<dataNum;i++) {
		SNTPRINTF(path, _SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH, _TEXT("%s/%s"), dirPath, data[i].dataPath);

		if (PathIsDirectory(path)) {
			if (!RemoveDirectory(path)) {
				return SCE_SAMPLE_UTIL_ERROR_DIRECTORY_DELETE;
			}
		} else {
			int ret = _tremove(path);
			if (ret != 0) {
				return ret;
			}
		}
	}

	return SCE_OK;
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

#endif //defined(_SCE_TARGET_OS_WINDOWS) && _SCE_TARGET_OS_WINDOWS
