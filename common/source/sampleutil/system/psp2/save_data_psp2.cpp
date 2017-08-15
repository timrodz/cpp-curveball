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

#if defined(_SCE_TARGET_OS_PSP2) && _SCE_TARGET_OS_PSP2

#include <apputil.h>

#pragma comment(lib,"libSceAppUtil_stub_weak.a")

#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PREFIX			"slot"
#define _SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH		8
#define _SCE_SAMPLE_UTIL_SAVE_DATA_FILEPATH_LENGTH				255
#define _SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_MY_TITLE			"savedata0:"
#define _SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_OTHER_TITLE		"savedata1:"

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
					int atomicSlotCreateAndDataSave(const SaveDataSlotId slotId, const DataSaveItem *data, const uint32_t dataNum, const SaveDataSlotParam *slotParam, const size_t slotSizeKiB);
					int atomicDataSaveAndSlotSetParam(const SaveDataSlotId slotId, const DataSaveItem *data, const uint32_t dataNum, const SaveDataSlotParam *slotParam);
					int dataLoadMount(const UserId userId, const SaveDataSlotId slotId, SaveDataMountPoint *outMountPoint);
					int dataLoadUmount(const SaveDataMountPoint *mountPoint);
					int dataRemove(const UserId userId, const SaveDataSlotId slotId, const DataRemoveItem *data, const uint32_t dataNum);

					virtual std::string getResourceType(void) const
					{
						return "System::SaveData";
					}
				private:
					bool m_isMount;
					SceAppUtilMountPoint m_mountPoint;
					void setupSceAppUtilSaveDataSlotParam(const SaveDataSlotParam *slotParam, const size_t sizeKiB, SceAppUtilSaveDataSlotParam *sceSlotParam);
					int getSizeKiB(const SaveDataSlotId slotId, size_t *outSizeKiB);
					int dataSaveCommon(const SaveDataSlotId slotId, const DataSaveItem *data, const uint32_t dataNum, const SaveDataSlotParam *slotParam, const size_t slotSizeKiB, size_t *outRequiredSizeKiB);
					int existsSlot(const SaveDataSlotId slotId, bool *outExistsSlot);

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
	m_isMount = false;
	memset(&m_mountPoint, 0, sizeof(SceAppUtilMountPoint));
}

int sssi::SaveDataImpl::initialize(const sss::SaveDataOption *option)
{
	SceAppUtilInitParam initParam;
	SceAppUtilBootParam bootParam;
	memset(&initParam, 0, sizeof(SceAppUtilInitParam));
	memset(&bootParam, 0, sizeof(SceAppUtilBootParam));

	int ret = sceAppUtilInit( &initParam, &bootParam );
	if (ret != SCE_OK && ret != SCE_APPUTIL_ERROR_BUSY) {
		return ret;
	}

	if (option != NULL) {
		if (strlen(option->titleId) > 0 && strlen(option->passCode) > 0) {
			SceAppUtilTitleId titleId;
			memset(&titleId, 0, sizeof(titleId));
			memcpy(&titleId.data, option->titleId, sizeof(titleId.data));

			SceAppUtilPassCode passCode;
			memset(&passCode, 0, sizeof(passCode));
			memcpy(&passCode.data, option->passCode, sizeof(passCode.data));

			ret = sceAppUtilSaveDataMount(&titleId, &passCode);
			if (ret == SCE_OK) {
				m_isMount = true;
				strncpy((char*)&m_mountPoint.data, _SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_OTHER_TITLE, SCE_APPUTIL_MOUNTPOINT_DATA_MAXSIZE);
			}
			return ret;
		}
	}

	strncpy((char*)&m_mountPoint.data, _SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_MY_TITLE, SCE_APPUTIL_MOUNTPOINT_DATA_MAXSIZE);

	return SCE_OK;
}

int sssi::SaveDataImpl::finalize(void)
{
	int ret = SCE_OK;
	if (m_isMount) {
		ret = sceAppUtilSaveDataUmount();
	}
	return ret;
}

void sssi::SaveDataImpl::setupSceAppUtilSaveDataSlotParam(const SaveDataSlotParam *slotParam,
														  const size_t sizeKiB,
														  SceAppUtilSaveDataSlotParam *sceSlotParam)
{
	if (slotParam != NULL && sceSlotParam != NULL) {
		memset(sceSlotParam, 0, sizeof(SceAppUtilSaveDataSlotParam));
		strncpy((char*)&sceSlotParam->title, slotParam->title, SCE_APPUTIL_SAVEDATA_SLOT_TITLE_MAXSIZE);
		strncpy((char*)&sceSlotParam->subTitle, slotParam->subTitle, SCE_APPUTIL_SAVEDATA_SLOT_SUBTITLE_MAXSIZE);
		strncpy((char*)&sceSlotParam->detail, slotParam->detail, SCE_APPUTIL_SAVEDATA_SLOT_DETAIL_MAXSIZE);
		strncpy((char*)&sceSlotParam->iconPath, slotParam->iconPath, SCE_APPUTIL_SAVEDATA_SLOT_ICON_PATH_MAXSIZE);
		sceSlotParam->userParam = slotParam->userParam;
		sceSlotParam->sizeKiB = sizeKiB;
	}
}

int sssi::SaveDataImpl::slotCreate(const sce::SampleUtil::System::UserId userId,
								   const sss::SaveDataSlotId slotId,
								   const sss::SaveDataSlotParam *param,
								   const size_t sizeKiB)
{
	SceAppUtilSaveDataSlotParam slotParam;

	if (param != NULL) {
		setupSceAppUtilSaveDataSlotParam(param, sizeKiB, &slotParam);
	} else {
		memset(&slotParam, 0, sizeof(SceAppUtilSaveDataSlotParam));
		strncpy((char*)&slotParam.title, "", SCE_APPUTIL_SAVEDATA_SLOT_TITLE_MAXSIZE);
		strncpy((char*)&slotParam.subTitle, "", SCE_APPUTIL_SAVEDATA_SLOT_SUBTITLE_MAXSIZE);
		strncpy((char*)&slotParam.detail, "", SCE_APPUTIL_SAVEDATA_SLOT_DETAIL_MAXSIZE);
		strncpy((char*)&slotParam.iconPath, "", SCE_APPUTIL_SAVEDATA_SLOT_ICON_PATH_MAXSIZE);
		slotParam.userParam = 0;
		slotParam.sizeKiB = sizeKiB;
	}

	int ret = sceAppUtilSaveDataSlotCreate(slotId, &slotParam, &m_mountPoint);
	return ret;
}

int sssi::SaveDataImpl::slotDelete(const sce::SampleUtil::System::UserId userId, const sss::SaveDataSlotId slotId)
{
	int ret = sceAppUtilSaveDataSlotDelete(slotId, &m_mountPoint);
	return ret;
}

int sssi::SaveDataImpl::getSizeKiB(const sss::SaveDataSlotId slotId, size_t *outSizeKiB)
{
	SceAppUtilSaveDataSlotParam slotParam;
	memset(&slotParam, 0, sizeof(SceAppUtilSaveDataSlotParam));
	int ret = sceAppUtilSaveDataSlotGetParam(slotId, &slotParam, &m_mountPoint);
	if (ret < SCE_OK) {
		return ret;
	}

	if (outSizeKiB != NULL) {
		*outSizeKiB = slotParam.sizeKiB;
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::slotSetParam(const sce::SampleUtil::System::UserId userId,
									 const sss::SaveDataSlotId slotId,
									 const sss::SaveDataSlotParam *param)
{
	if (param == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	size_t sizeKiB = 0;
	int ret = getSizeKiB(slotId, &sizeKiB);
	if (ret < SCE_OK) {
		return ret;
	}

	SceAppUtilSaveDataSlotParam slotParam;
	setupSceAppUtilSaveDataSlotParam(param, sizeKiB, &slotParam);

	ret = sceAppUtilSaveDataSlotSetParam(slotId, &slotParam, &m_mountPoint);
	return ret;
}

int sssi::SaveDataImpl::slotGetParam(const sce::SampleUtil::System::UserId userId, const sss::SaveDataSlotId slotId, sss::SaveDataSlotParam *outParam)
{
	if (outParam == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceAppUtilSaveDataSlotParam slotParam;
	memset(&slotParam, 0, sizeof(SceAppUtilSaveDataSlotParam));
	int ret = sceAppUtilSaveDataSlotGetParam(slotId, &slotParam, &m_mountPoint);
	if (ret < SCE_OK) {
		return ret;
	}

	outParam->status = (SaveDataSlotStatus)slotParam.status;
	strncpy(outParam->title, (char*)&slotParam.title, SCE_APPUTIL_SAVEDATA_SLOT_TITLE_MAXSIZE);
	strncpy(outParam->subTitle, (char*)&slotParam.subTitle, SCE_APPUTIL_SAVEDATA_SLOT_SUBTITLE_MAXSIZE);
	strncpy(outParam->detail, (char*)&slotParam.detail, SCE_APPUTIL_SAVEDATA_SLOT_DETAIL_MAXSIZE);
	strncpy(outParam->iconPath, (char*)&slotParam.iconPath, SCE_APPUTIL_SAVEDATA_SLOT_ICON_PATH_MAXSIZE);
	outParam->userParam = slotParam.userParam;
	outParam->mtime = slotParam.modifiedTime;
	return SCE_OK;
}

int sssi::SaveDataImpl::slotSearch(const sce::SampleUtil::System::UserId userId, const sss::SaveDataSearchCond *cond, sss::SaveDataSearchResult *outResult)
{
	if (cond == NULL || outResult == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceAppUtilWorkBuffer buffer;
	memset( &buffer, 0, sizeof(SceAppUtilWorkBuffer) );
	
	int bufSize = 0;
	switch (cond->key) {
	default:
		bufSize = SCE_APPUTIL_WORKBUF_SEARCH_SLOT_DEFAULT_ELEMENT_SIZE * cond->range;
		break;
	case kSaveDataSlotSortKeySize:
		bufSize = SCE_APPUTIL_WORKBUF_SEARCH_SLOT_BY_SIZE_KIB_ELEMENT_SIZE * cond->range;
		break;
	case kSaveDataSlotSortKeyMtime:
		bufSize = SCE_APPUTIL_WORKBUF_SEARCH_SLOT_BY_MODIFIEDTIME_ELEMENT_SIZE * cond->range;
		break;
	}

	buffer.buf = malloc(bufSize);
	buffer.bufSize = bufSize;

	SceAppUtilSaveDataSlotSearchCond nativeCond;
	memset(&nativeCond, 0, sizeof(SceAppUtilSaveDataSlotSearchCond));

	nativeCond.type  = SCE_APPUTIL_SAVEDATA_SLOT_SEARCH_TYPE_EXIST_SLOT;
	nativeCond.from  = cond->from;
	nativeCond.range = cond->range;
	nativeCond.key   = cond->key;
	nativeCond.order = cond->order;

	SceAppUtilSlotSearchResult nativeResult;
	memset(&nativeResult, 0, sizeof(SceAppUtilSlotSearchResult));

	int ret = sceAppUtilSaveDataSlotSearch(&buffer, &nativeCond, &nativeResult, &m_mountPoint);
	if (ret < SCE_OK) {
		free(buffer.buf);
		return ret;
	}

	outResult->hitNum = nativeResult.hitNum;
	for (uint32_t i=0; i<nativeResult.hitNum; i++) {
		if (i < outResult->slotListNum) {
			outResult->slotList[i] = nativeResult.slotList[i].id;
		}
	}

	free(buffer.buf);
	return SCE_OK;
}

int sssi::SaveDataImpl::existsSlot(const SaveDataSlotId slotId, bool *outExistsSlot)
{
	SCE_SAMPLE_UTIL_ASSERT(outExistsSlot != NULL);
	
	int ret = SCE_OK;
	sss::SaveDataSearchCond cond;

	cond.from = slotId;
	cond.key = sss::kSaveDataSlotSortKeySlotId;
	cond.range = 1;
	cond.order = sss::kSaveDataSlotSortOrderAscent;

	sss::SaveDataSlotId slotList;

	sce::SampleUtil::System::SaveDataSearchResult result;
	result.slotList = &slotList;
	result.slotListNum = 1;

	ret = slotSearch(0, &cond, &result);
	if (ret != SCE_OK){
		return ret;
	}

	if(result.hitNum == 0){
		*outExistsSlot = false;
	}else{
		*outExistsSlot = true;
	}

	return SCE_OK;
}

int sssi::SaveDataImpl::dataSave(const sce::SampleUtil::System::UserId userId,
								 const sss::SaveDataSlotId slotId,
								 const sss::DataSaveItem *data,
								 const uint32_t dataNum,
								 size_t *outRequiredSizeKiB)
{
	int ret = SCE_OK;	
	ret = dataSaveCommon(slotId, data, dataNum, NULL, 0, outRequiredSizeKiB);
	return ret;
}

int sssi::SaveDataImpl::atomicSlotCreateAndDataSave(	const sss::SaveDataSlotId slotId,
												const sss::DataSaveItem *data,
												const uint32_t dataNum,
												const sss::SaveDataSlotParam *slotParam,
												const size_t slotSizeKiB)
{
	int ret = SCE_OK;

	if (slotParam == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	bool isSlotExisting = false;
	ret = existsSlot(slotId, &isSlotExisting);
	if(ret != SCE_OK){
		return ret;
	}

	if(isSlotExisting){
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_EXISTS;
	}

	std::size_t dummySize;
	ret = dataSaveCommon(slotId, data, dataNum, slotParam, slotSizeKiB, &dummySize);
	return ret;
}

int sssi::SaveDataImpl::atomicDataSaveAndSlotSetParam(	const sss::SaveDataSlotId slotId,
												const sss::DataSaveItem *data,
												const uint32_t dataNum,
												const sss::SaveDataSlotParam *slotParam)
{
	int ret = SCE_OK;

	if (slotParam == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	bool isSlotExisting = false;
	ret = existsSlot(slotId, &isSlotExisting);
	if(ret != SCE_OK){
		return ret;
	}

	if(!isSlotExisting){
		return SCE_SAMPLE_UTIL_ERROR_SAVE_DATA_SLOT_NOT_FOUND;
	}

	size_t slotSizeKiB = 0;
	ret = getSizeKiB(slotId, &slotSizeKiB);
	if (ret < SCE_OK) {
		return ret;
	}

	std::size_t dummySize;
	ret = dataSaveCommon(slotId, data, dataNum, slotParam, slotSizeKiB, &dummySize);
	return ret;
}

int sssi::SaveDataImpl::dataSaveCommon(	const sss::SaveDataSlotId slotId,
										const sss::DataSaveItem *data,
										const uint32_t dataNum,
										const sss::SaveDataSlotParam *slotParam,
										const size_t slotSizeKiB,
										size_t *outRequiredSizeKiB)
{
	if (dataNum > SCE_APPUTIL_SAVEDATA_DATA_MAXNUM) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if (data == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	SceAppUtilSaveDataDataSlot slot;
	memset(&slot, 0, sizeof(SceAppUtilSaveDataDataSlot));

	slot.id = slotId;

	SceAppUtilSaveDataSlotParam sceSlotParam;

	if (slotParam != NULL) {	
		setupSceAppUtilSaveDataSlotParam(slotParam, slotSizeKiB, &sceSlotParam);
		slot.slotParam = &sceSlotParam;
	} else {
		slot.slotParam = NULL;
	}	
	
	SceAppUtilSaveDataDataSaveItem saveItem[SCE_APPUTIL_SAVEDATA_DATA_MAXNUM];
	memset(saveItem, 0, sizeof(SceAppUtilSaveDataDataSaveItem)*SCE_APPUTIL_SAVEDATA_DATA_MAXNUM);

	char path[SCE_APPUTIL_SAVEDATA_DATA_MAXNUM][SCE_APPUTIL_SAVEDATA_DATA_PATH_MAXSIZE];
	memset(path, 0, sizeof(SceChar8)*SCE_APPUTIL_SAVEDATA_DATA_MAXNUM*SCE_APPUTIL_SAVEDATA_DATA_PATH_MAXSIZE);

	char dirName[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH];
	setDirName(slotId, dirName);

	for (int i=0;i<dataNum;i++) {
		snprintf(path[i], SCE_APPUTIL_SAVEDATA_DATA_PATH_MAXSIZE,
				 "%s/%s", dirName, data[i].dataPath);
		saveItem[i].dataPath = (const SceChar8 *)path[i];
		saveItem[i].buf = data[i].buf;
		saveItem[i].bufSize = data[i].bufSize;
		saveItem[i].offset = data[i].offset;
		saveItem[i].mode = data[i].mode;
	}

	int ret = SCE_OK;
	if( outRequiredSizeKiB == NULL ){
		size_t requiredSizeKiBDummy = 0;
		ret = sceAppUtilSaveDataDataSave(&slot, saveItem, dataNum, &m_mountPoint, &requiredSizeKiBDummy);
	} else {
		ret = sceAppUtilSaveDataDataSave(&slot, saveItem, dataNum, &m_mountPoint, outRequiredSizeKiB);
	}
	return ret;
}

int sssi::SaveDataImpl::dataLoadMount(const sce::SampleUtil::System::UserId userId, const sss::SaveDataSlotId slotId, sss::SaveDataMountPoint *outMountPoint)
{
	if (outMountPoint == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	char dirName[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH];
	setDirName(slotId, dirName);

	snprintf(outMountPoint->data, SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE, "%s/%s", (char *)&m_mountPoint.data, dirName);

	return SCE_OK;
}

int sssi::SaveDataImpl::dataLoadUmount(const sss::SaveDataMountPoint *mountPoint)
{
	if (mountPoint == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	return SCE_OK;
}

int sssi::SaveDataImpl::dataRemove(const sce::SampleUtil::System::UserId userId,
								   const sss::SaveDataSlotId slotId,
								   const sss::DataRemoveItem *data,
								   const uint32_t dataNum)
{
	if (data == NULL && dataNum > 0) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	if (dataNum > SCE_APPUTIL_SAVEDATA_DATA_MAXNUM) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	SceAppUtilSaveDataDataSlot dataSlot;
	memset(&dataSlot, 0, sizeof(SceAppUtilSaveDataDataSlot));
	dataSlot.id = slotId;

	SceAppUtilSaveDataDataRemoveItem removeItem[SCE_APPUTIL_SAVEDATA_DATA_MAXNUM];
	memset(removeItem, 0, sizeof(SceAppUtilSaveDataDataRemoveItem)*SCE_APPUTIL_SAVEDATA_DATA_MAXNUM);

	char path[SCE_APPUTIL_SAVEDATA_DATA_MAXNUM][SCE_APPUTIL_SAVEDATA_DATA_PATH_MAXSIZE];
	memset(path, 0, sizeof(SceChar8)*SCE_APPUTIL_SAVEDATA_DATA_MAXNUM*SCE_APPUTIL_SAVEDATA_DATA_PATH_MAXSIZE);

	char dirName[_SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_DIRNAME_PATH_LENGTH];
	setDirName(slotId, dirName);

	for (int i=0;i<dataNum;i++) {
		snprintf(path[i], SCE_APPUTIL_SAVEDATA_DATA_PATH_MAXSIZE,
				 "%s/%s", dirName, data[i].dataPath);
		removeItem[i].dataPath = (const SceChar8 *)path[i];
		removeItem[i].mode = SCE_APPUTIL_SAVEDATA_DATA_REMOVE_MODE_KEEP_SLOT;
	}

	int ret = sceAppUtilSaveDataDataRemove( &dataSlot, removeItem, dataNum, &m_mountPoint );
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

#endif //defined(_SCE_TARGET_OS_PSP2) && _SCE_TARGET_OS_PSP2
