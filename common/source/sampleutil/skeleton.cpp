/* SIE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2016 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#include "sampleutil/skeleton.h"

#if _SCE_TARGET_OS_PSP2
#pragma comment(lib, "SceFios2_stub")
#endif
#if _SCE_TARGET_OS_WINDOWS
#pragma comment(lib, "libfios2")
#endif
#if _SCE_TARGET_OS_ORBIS
#pragma comment(lib, "SceVideoOut_stub_weak")
#include <video_out.h>
#endif

sce::SampleUtil::SampleSkeleton::SampleSkeleton(void) : m_functionFlags(0)
{

}

sce::SampleUtil::SampleSkeleton::~SampleSkeleton(void)
{

}

sce::SampleUtil::SampleSkeleton::SampleSkeletonOption::SampleSkeletonOption(void)
{
	graphicsOption = NULL;
	audioOption = NULL;
	fios2Option = NULL;
	padOption = NULL;
}

int sce::SampleUtil::SampleSkeleton::initializeUtil(
	uint32_t functionFlags,
	int32_t width,
	int32_t height,
	SampleSkeletonOption* option)
{
	printf("## Sample Application: start initializing ##\n");

	int ret = SCE_OK;

	if (width < 0 || height < 0) {
#if _SCE_TARGET_OS_PSP2
		width = 960;
		height = 544;
#endif
#if _SCE_TARGET_OS_WINDOWS
		width = 1280;
		height = 720;
#endif
#if _SCE_TARGET_OS_ORBIS
		width = 1920;
		height = 1080;
		int handle = sceVideoOutOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);
		SCE_SAMPLE_UTIL_ASSERT(handle > 0);
		if (handle > 0){
			SceVideoOutResolutionStatus status;
			if (SCE_OK == sceVideoOutGetResolutionStatus(handle, &status) && status.fullHeight > 1080)
			{
				width = 3840;
				height = 2160;
			}
			sceVideoOutClose(handle);
		}
#endif
	}

	m_functionFlags = functionFlags;

	Graphics::GraphicsContextOption* graphicsOption = NULL;
	Audio::AudioContextOption* audioOption = NULL;
	SceFiosParams*			   fios2Option = NULL;
	Input::PadContextOption*   padOption = NULL;

	(void)padOption;

	if (option != NULL) {
		graphicsOption = option->graphicsOption;
		audioOption = option->audioOption;
		fios2Option = option->fios2Option;
		padOption = option->padOption;
	}

	if (!(m_functionFlags & kFunctionFlagGraphics) && (m_functionFlags & kFunctionFlagSpriteRenderer)) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if (m_functionFlags & kFunctionFlagGraphics) {
		ret = Graphics::createGraphicsContext(&m_graphicsContext, width, height, graphicsOption);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
		if (m_functionFlags & kFunctionFlagSpriteRenderer) {
			ret = createSpriteRenderer(&m_spriteRenderer, m_graphicsContext);
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
			if (ret != SCE_OK) {
				return ret;
			}
		}
	}

	if (m_functionFlags & kFunctionFlagFios2) {
		ret = initializeFios2(fios2Option);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
	}

	if (m_functionFlags & kFunctionFlagAudio) {
		ret = Audio::createAudioContext(&m_audioContext, audioOption);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
	}

	if (m_functionFlags & kFunctionFlagUserIdManager) {
		ret = System::createUserIdManager(&m_userIdManager);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
	}

#if _SCE_TARGET_OS_ORBIS
	if (!(m_functionFlags & kFunctionFlagUserIdManager) && (m_functionFlags & kFunctionFlagPadOfInitialUser)) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if (m_functionFlags & kFunctionFlagPadOfInitialUser) {
		System::UserId initialUserId = 0;
		ret = m_userIdManager->getInitialUser(&initialUserId);
		if (ret != SCE_OK) {
			return ret;
		}
		ret = Input::createPadContext(&m_padContextOfInitialUser, initialUserId, 0, 0, padOption);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
	}
#endif

	return SCE_OK;
}



int sce::SampleUtil::SampleSkeleton::updateUtil(void)
{
	int ret = SCE_OK;

	if (m_functionFlags & kFunctionFlagUserIdManager) {
		ret = m_userIdManager->update();
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	}

#if _SCE_TARGET_OS_ORBIS
	if (m_functionFlags & kFunctionFlagPadOfInitialUser) {
		ret = m_padContextOfInitialUser->update();
		SCE_SAMPLE_UTIL_ASSERT(ret >= SCE_OK);
	}
#endif
#if _SCE_TARGET_OS_PSP2 && 0 //TODO
	if(m_functionFlags & kFunctionFlagMotion){
		// Process motion input
		m_motionContext->update();
	}

	if(m_functionFlags & kFunctionFlagTouch){
		// Process touch input
		m_touchContext->update();
	}
#endif	/* __psp2__ */
	(void)ret;
	return SCE_OK;
}

int sce::SampleUtil::SampleSkeleton::finalizeUtil(void)
{
	int ret;


	if(m_functionFlags & kFunctionFlagAudio){
		ret = sce::SampleUtil::destroy(m_audioContext);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
		m_audioContext = NULL;
	}

	if(m_functionFlags & kFunctionFlagFios2){
		ret = finalizeFios2();
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
	}

#if _SCE_TARGET_OS_ORBIS
	if (m_functionFlags & kFunctionFlagPadOfInitialUser) {
		ret = sce::SampleUtil::destroy(m_padContextOfInitialUser);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
		m_padContextOfInitialUser = NULL;
	}
#endif

	if(m_functionFlags & kFunctionFlagUserIdManager){
		ret = sce::SampleUtil::destroy(m_userIdManager);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
		m_userIdManager = NULL;
	}

	if(m_functionFlags & kFunctionFlagGraphics){
#if !defined(SCE_SAMPLE_UTIL_SUBMISSION_RELEASE_BUILD) && defined(__psp2__) && 0 // TODO
		if(m_functionFlags & kFunctionFlagDebugFont){
			returnCode = sceDbgFontExit();
			SCE_SAMPLE_UTIL_ASSERT(returnCode == SCE_OK);
		}
#endif /* SCE_SAMPLE_UTIL_SUBMISSION_RELEASE_BUILD */

		if(m_functionFlags & kFunctionFlagSpriteRenderer){
			ret = destroy(m_spriteRenderer);
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
			if (ret != SCE_OK) {
				return ret;
			}
			m_spriteRenderer = NULL;
		}

		ret = sce::SampleUtil::destroy(m_graphicsContext);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		if (ret != SCE_OK) {
			return ret;
		}
		m_graphicsContext = NULL;
	}

	printf("## Sample Application: finalized ##\n");

	return SCE_OK;
}


sce::SampleUtil::Graphics::GraphicsContext *sce::SampleUtil::SampleSkeleton::getGraphicsContext(void)
{
	return (m_functionFlags & kFunctionFlagGraphics) ? m_graphicsContext : NULL;
}

sce::SampleUtil::Graphics::SpriteRenderer *sce::SampleUtil::SampleSkeleton::getSpriteRenderer(void)
{
	return (m_functionFlags & kFunctionFlagSpriteRenderer) ? m_spriteRenderer : NULL;
}

sce::SampleUtil::Audio::AudioContext *sce::SampleUtil::SampleSkeleton::getAudioContext(void)
{
	return (m_functionFlags & kFunctionFlagAudio) ? m_audioContext : NULL;
}

sce::SampleUtil::System::UserIdManager *sce::SampleUtil::SampleSkeleton::getUserIdManager(void)
{
	return (m_functionFlags & kFunctionFlagUserIdManager) ? m_userIdManager : NULL;
}

sce::SampleUtil::Input::PadContext *sce::SampleUtil::SampleSkeleton::getPadContextOfInitialUser(void)
{
	return (m_functionFlags & kFunctionFlagPadOfInitialUser) ? m_padContextOfInitialUser : NULL;
}

static unsigned int roundUp(unsigned int size, unsigned int align)
{
	return ((size+(align-1u))&(~(align-1u)));
}

int sce::SampleUtil::SampleSkeleton::initializeFios2(SceFiosParams *params)
{
	int32_t ret = SCE_OK;

	if(params == NULL){
		SceFiosParams _params = SCE_FIOS_PARAMS_INITIALIZER;

		int32_t sizeOp    = SCE_FIOS_OP_STORAGE_SIZE(64, SCE_FIOS_PATH_MAX);	/* 64 ops: */
		int32_t sizeChunk = SCE_FIOS_CHUNK_STORAGE_SIZE(1024);					/* 1024 chunks, 64KiB: */
		int32_t sizeFh    = SCE_FIOS_FH_STORAGE_SIZE(16, SCE_FIOS_PATH_MAX);	/* 16 file handles: */
		int32_t sizeDh    = SCE_FIOS_DH_STORAGE_SIZE(4, SCE_FIOS_PATH_MAX);		/* 4 directory handles: */

		sizeOp    = roundUp(sizeOp, 8);
		sizeChunk = roundUp(sizeChunk, 8);
		sizeFh    = roundUp(sizeFh, 8);
		sizeDh    = roundUp(sizeDh, 8);

		int32_t sizeTotal = sizeOp + sizeChunk + sizeFh + sizeDh;

		m_pStorageForFios2 = (unsigned char*)malloc(sizeTotal);

		if(m_pStorageForFios2 == NULL){
			return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
		}

		_params.opStorage.pPtr      = m_pStorageForFios2;
		_params.opStorage.length    = sizeOp;
		_params.chunkStorage.pPtr   = m_pStorageForFios2 + sizeOp;
		_params.chunkStorage.length = sizeChunk;
		_params.fhStorage.pPtr      = m_pStorageForFios2 + sizeOp + sizeChunk;
		_params.fhStorage.length    = sizeFh;
		_params.dhStorage.pPtr      = m_pStorageForFios2 + sizeOp + sizeChunk + sizeFh;
		_params.dhStorage.length    = sizeDh;
		_params.pathMax             = SCE_FIOS_PATH_MAX;
		_params.pMemcpy             = memcpy;

		ret = sceFiosInitialize(&_params);
	}else{
		ret = sceFiosInitialize(params);
	}

	return ret;
}

int32_t sce::SampleUtil::SampleSkeleton::finalizeFios2(void)
{
	sceFiosTerminate();

	if(m_pStorageForFios2 != NULL){
		free(m_pStorageForFios2);
		m_pStorageForFios2 = NULL;
	}

	return SCE_OK;
}
