/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/audio.h>
#include "ngs_internal.h"

#include <audioout.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel.h>
#include <libsysmodule.h>
#include <libdbg.h>

#include <ngs.h>
#include <ngs/error.h>

#pragma comment(lib, "SceNgs_stub")
#pragma comment(lib, "SceAudio_stub")

namespace ssa = sce::SampleUtil::Audio;
namespace ssai = sce::SampleUtil::Audio::Internal;

#define		SOUND_UTIL_MAX_MONO_VOICE_NUM		64
#define		SOUND_UTIL_MAX_STEREO_VOICE_NUM		16
#define		SOUND_UTIL_MAX_REVERB_NUM			4
#define		SOUND_UTIL_MAX_MIXER_NUM			8
#define		SOUND_UTIL_MAX_AT9_VOICE_NUM		32




int ssai::AudioContextImpl::initNGS(const SceNgsSystemInitParams* initParams)
{
	int returnCode = SCE_OK;

	returnCode = sceSysmoduleLoadModule( SCE_SYSMODULE_NGS );
	if( returnCode != SCE_OK ){
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}

	size_t	size;
	returnCode = sceNgsSystemGetRequiredMemorySize( initParams, &size );
	if( returnCode != SCE_NGS_OK ){
		return returnCode;
	}
	m_sysMem = ::memalign(256, size);
	if( m_sysMem == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}
	memset(m_sysMem, 0, size);

	returnCode = sceNgsSystemInit( m_sysMem, size, initParams, &m_sysHandle );
	if( returnCode != SCE_NGS_OK ){
		return returnCode;
	}

#ifndef	_DEBUG
	sceNgsSystemSetFlags(m_sysHandle, SCE_NGS_SYSTEM_FLAG_NO_MODULE_PARAM_CHECKING);
#endif

	return returnCode;
}

int ssai::AudioContextImpl::exitNgs(void)
{
	int returnCode;

	returnCode = sceNgsSystemRelease( m_sysHandle );
	if( returnCode != SCE_NGS_OK ){
		SCE_DBG_LOG_ERROR( "soundUtilExit: sceNgsSystemRelease() failed: 0x%08X\n", returnCode );
	}
	m_sysHandle = 0;
	::free((void*)m_sysMem);

	returnCode = sceSysmoduleUnloadModule( SCE_SYSMODULE_NGS );
	if( returnCode != SCE_OK ){
		return returnCode;
	}

	return SCE_OK;
}




int ssai::AudioContextImpl::initialize(const ssa::AudioContextOption *pOption)
{
	int ret;
	int returnCode;

	if(pOption != NULL){
		m_option = *pOption;
	}
	m_option.stereoVoiceNum = (m_option.stereoVoiceNum < 1) ? 1 : m_option.stereoVoiceNum;
	m_option.monoVoiceNum   = (m_option.monoVoiceNum   < 1) ? 1 : m_option.monoVoiceNum;
	m_option.at9VoiceNum    = (m_option.at9VoiceNum    < 1) ? 1 : m_option.at9VoiceNum;

	m_option.reverbBussNum  = (m_option.reverbBussNum  < 1) ? 1 : m_option.reverbBussNum;
	m_option.mixerBussNum   = (m_option.reverbBussNum  < 1) ? 1 : m_option.reverbBussNum;

	lock();
	{
		uint32_t monoVoiceMaxNum	   = (m_option.monoVoiceNum   != 0) ? m_option.monoVoiceNum   : 1;
		uint32_t stereoVoiceMaxNum	   = (m_option.stereoVoiceNum != 0) ? m_option.stereoVoiceNum : 1;
		uint32_t reverbBussMaxNum	   = (m_option.reverbBussNum  != 0) ? m_option.reverbBussNum  : 1;
		uint32_t mixerBussMaxNum	   = (m_option.mixerBussNum   != 0) ? m_option.mixerBussNum   : 1;
		uint32_t at9VoiceMaxNum		   = (m_option.at9VoiceNum    != 0) ? m_option.at9VoiceNum    : 1;
		uint32_t masterBussMaxNum	   = 2;

		uint32_t maxModules = 
			( SCE_NGS_VOICE_T1_NUM_MODULES * (monoVoiceMaxNum + stereoVoiceMaxNum) 
			+ SCE_NGS_VOICE_AT9_NUM_MODULES * at9VoiceMaxNum 
			+ VoiceManager::REVERB_BUSS_MODULE_NUM * reverbBussMaxNum 
			+ VoiceManager::MIXER_BUSS_MODULE_NUM * mixerBussMaxNum );

		uint32_t maxVoices = monoVoiceMaxNum + stereoVoiceMaxNum + at9VoiceMaxNum 
			+ reverbBussMaxNum + mixerBussMaxNum + masterBussMaxNum;

		//E Initialize NGS
		//J NGS の初期化
		m_ngsInitParams.nMaxModules  = maxModules;
		m_ngsInitParams.nMaxRacks    = 7;
		m_ngsInitParams.nMaxVoices   = maxVoices;
		m_ngsInitParams.nGranularity = 512;
		m_ngsInitParams.nSampleRate  = 48000;
		
		ret = initNGS(&m_ngsInitParams);
	}
	if(ret != SCE_OK){
		returnCode = ret;
		goto failed;
	}
	

	ret = m_voiceManager.m_masterBuss.initialize(
		m_sysHandle,
		m_option.stereoVoiceNum + m_option.monoVoiceNum + m_option.at9VoiceNum,
		m_option.mixerBussNum + m_option.reverbBussNum, 
		m_option.stereoVoiceNum + m_option.monoVoiceNum + m_option.at9VoiceNum + m_option.mixerBussNum + m_option.reverbBussNum);
	if(ret != SCE_OK){
		returnCode = ret;
		goto failed;
	}

	ret = m_voiceManager.init(m_sysHandle, &m_option);
	if(ret < 0){
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}

	ret = m_voiceManager.m_masterBuss.start();
	if(ret != SCE_OK){
		returnCode = ret;
		goto failed;
	}


	AudioOutThreadInfo userThreadInfo;
	userThreadInfo.priority     = SCE_KERNEL_HIGHEST_PRIORITY_USER;
	userThreadInfo.stackSize    = 128*1024;
	userThreadInfo.cpuAffinityMask = SCE_KERNEL_CPU_MASK_USER_ALL;

	//E Initialize AudioOut(Main)
	//J AudioOut の初期化(Main)
	{
		uint32_t grain=512;
		ret = m_audioOutMain.start(updateCallabck, this,  grain, userThreadInfo);
		if(ret != SCE_OK){
			returnCode = ret;
			goto failed;
		}
	}
		
	//J 初めはBGM ポートは enable にしておく
	ret = m_voiceManager.m_masterBuss.enableBgmPort(true);

	unlock();

	m_isInitialized = true;
	return SCE_OK;

failed:

	unlock();;

	m_voiceManager.finalize();
	exitNgs();

	return returnCode;

}





int ssai::AudioContextImpl::finalize(void)
{
	if(!m_isInitialized){
		return SCE_OK;
	}
	int returnCode = SCE_OK;

	returnCode = m_audioOutMain.stop();
	if(returnCode != SCE_OK){
		return returnCode;
	}

	m_voiceManager.finalize();

	exitNgs();

	return SCE_OK;
}


void ssai::AudioContextImpl::updateCallabck(void* mainBuffer, uint32_t mainBufferSize, 
											void* bgmBuffer, uint32_t bgmBufferSize, 
											void* userAttr)
{
	AudioContextImpl *context = (AudioContextImpl*)userAttr;
	context->process(mainBuffer, mainBufferSize, 
		             bgmBuffer, bgmBufferSize );
	if(context->m_option.contextUpdate != NULL){
		context->m_option.contextUpdate(mainBuffer, mainBufferSize, 
		                               bgmBuffer, bgmBufferSize, 
									   context->m_option.userArg);
	}
}



void ssai::AudioContextImpl::process(void* mainBuffer, uint32_t mainSize,
								 void* bgmBuffer, uint32_t bgmSize)
{
	int   returnCode     = SCE_OK;

	if( this == NULL || mainBuffer == NULL || bgmBuffer==NULL) {
		SCE_DBG_LOG_ERROR("soundUtilProcess: parameter failed \n");
		return;
	}


	returnCode = sceNgsSystemUpdate( m_sysHandle );
	if( returnCode != SCE_NGS_OK ){
		SCE_DBG_LOG_ERROR( "process: sceNgsSystemUpdate() failed: 0x%08X\n", returnCode );
	}

	Buss *mainBuss = m_voiceManager.getMainBuss();
	SCE_SAMPLE_UTIL_ASSERT(mainBuss != NULL);
	Buss *bgmBuss = m_voiceManager.getBgmBuss();
	SCE_SAMPLE_UTIL_ASSERT(bgmBuss != NULL);

	returnCode = dynamic_cast<BussImpl*>(mainBuss)->getData(mainBuffer, sizeof(short) * m_ngsInitParams.nGranularity * 2 );
	if( returnCode != SCE_NGS_OK ){
		SCE_DBG_LOG_ERROR( "process: sceNgsVoiceGetStateData() failed: 0x%08X\n", returnCode );
	}

	returnCode = dynamic_cast<BussImpl*>(bgmBuss)->getData(bgmBuffer, sizeof(short) * m_ngsInitParams.nGranularity * 2 );
	if( returnCode != SCE_NGS_OK ){
		SCE_DBG_LOG_ERROR( "process: sceNgsVoiceGetStateData() failed: 0x%08X\n", returnCode );
	}



	this->lock();

	returnCode = this->m_voiceManager.checkFinishedVoices();
	if( returnCode != SCE_OK )
	{
		SCE_DBG_LOG_ERROR( "process: checkFinishedMonoVoices() failed: 0x%08X\n", returnCode );
	}

	this->unlock();

	return;
}


int ssai::AudioContextImpl::createVoice(Voice** outVoice, const VoiceData *_voiceData)
{
	if( outVoice == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(_voiceData == NULL){
		SCE_DBG_LOG_ERROR("soundUtilVoiceOpen : dataInfo is invalid\n");
		return -1;
	}

	const VoiceDataImpl *voiceData = dynamic_cast<const VoiceDataImpl *>(_voiceData);
	bool isAt9 = voiceData->getCodecType() == Data::CODEC_TYPE_AT9;
	int numChannels = voiceData->getNumChannels();

	lock();

	VoiceRack *rack = m_voiceManager.getVoiceRack(isAt9, numChannels);
	if(rack == NULL){
		unlock();
		return -1;
	}
	VoiceInternal *voice = rack->createVoice(voiceData);
	unlock();

	if(voice == NULL){
		printf(__FILE__"(%d) play \n", __LINE__);
		return -1;
	}
	lock();
	
	voice->setOutput(0, m_voiceManager.getMainBuss());

	unlock();

	*outVoice = voice;
	return SCE_OK;
}



ssa::Buss* ssai::AudioContextImpl::bussCreateReverb(int reverbMode)
{
	int returnID = -1;
	int returnCode = 0;
	

	lock();

	Buss *reverbBuss = m_voiceManager.m_reverbRack.createBuss();
	if(reverbBuss == NULL){
		SCE_DBG_LOG_ERROR("soundUtilBussCreateReverb: Reverb Voice is unavailable!!!\n");
		returnCode = SCE_SAMPLE_UTIL_ERROR_BUSY;
		goto End;
	}

	// Get reverb parameters
	returnCode = dynamic_cast<BussImpl*>(reverbBuss)->setReverbMode(m_sysHandle, reverbMode);
	if(returnCode != SCE_OK){
		goto Fail;
	}

	returnCode = reverbBuss->setOutput(0, this->m_voiceManager.getMainBuss());
	if ( returnCode != SCE_NGS_OK ) {
		SCE_DBG_LOG_ERROR( "soundUtilBussCreateReverb: connectRacks() failed: 0x%08X\n", returnCode );
		goto Fail;
	}

	//E Start the reverb
	//J リバーブのスタート
	returnCode = dynamic_cast<BussImpl*>(reverbBuss)->play();
	if ( returnCode != SCE_NGS_OK ) {
		SCE_DBG_LOG_ERROR( "soundUtilBussCreateReverb: sceNgsVoicePlay() failed: 0x%08X\n", returnCode );
		unlock();
		return NULL;
	}

	returnCode = returnID;
	goto End;

Fail:


End:
	unlock();;
	return reverbBuss;
}


/* ---------------------------------------------------------------- */




int ssai::VoiceManager::init(
	SceNgsHSynSystem		sysHandle, 
	const AudioContextOption *option
	)
{
	m_option = option;
	if( m_option->monoVoiceNum   > SOUND_UTIL_MAX_MONO_VOICE_NUM ||
		m_option->stereoVoiceNum > SOUND_UTIL_MAX_STEREO_VOICE_NUM ||
		m_option->at9VoiceNum    > SOUND_UTIL_MAX_AT9_VOICE_NUM ||
		m_option->reverbBussNum  > SOUND_UTIL_MAX_REVERB_NUM ||
		m_option->mixerBussNum   > SOUND_UTIL_MAX_MIXER_NUM 
		)
	{

		SCE_DBG_LOG_ERROR("soundUtilInit : initParam is invalid!!\n");
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	this->masterBussMaxNum		= 2;

	int ret;

	const struct SceNgsVoiceDefinition *pT1VoiceDef         = sceNgsVoiceDefGetTemplate1();
	{
		//E Create the rack for the monaural player
		//J モノラルプレイヤー用ラックの作成
		SceNgsRackDescription rackDesc;
		rackDesc.nChannelsPerVoice   = 1;
		rackDesc.nVoices             = m_option->monoVoiceNum;
		rackDesc.pVoiceDefn          = pT1VoiceDef;
		rackDesc.nMaxPatchesPerInput = 0;
		rackDesc.nPatchesPerOutput   = ssa::NUM_OUTPUT_INDEX;

		ret = m_monoRack.initialize(sysHandle, rackDesc, 0);
		if( ret != SCE_NGS_OK )	{
			return ret;
		}
	}
	{
		//E Create the rack for the stereo player
		//J ステレオプレイヤー用のラックの作成
		SceNgsRackDescription rackDesc;
		rackDesc.nChannelsPerVoice   = 2;
		rackDesc.nVoices             = m_option->stereoVoiceNum;
		rackDesc.pVoiceDefn          = pT1VoiceDef;
		rackDesc.nMaxPatchesPerInput = 0;
		rackDesc.nPatchesPerOutput   = ssa::NUM_OUTPUT_INDEX;

		ret = m_stereoRack.initialize(sysHandle, rackDesc, m_option->monoVoiceNum);
		if( ret != SCE_NGS_OK )	{
			return ret;
		}
	}

	{
		//J AT9プレーヤー用のラックの作成
		SceNgsRackDescription rackDesc;
		rackDesc.nChannelsPerVoice   = 2;
		rackDesc.nVoices             = m_option->at9VoiceNum;
		rackDesc.pVoiceDefn          = sceNgsVoiceDefGetAtrac9Voice();		
		rackDesc.nMaxPatchesPerInput = 0;
		rackDesc.nPatchesPerOutput   = ssa::NUM_OUTPUT_INDEX;
		rackDesc.pUserReleaseData    = NULL;

		ret = m_at9Rack.initialize(sysHandle, rackDesc, m_option->monoVoiceNum + m_option->stereoVoiceNum);
		if( ret != SCE_NGS_OK )	{
			return ret;
		}

	}

	{
		//E Create the reverb buss
		//J リバーブバスの作成
		SceNgsRackDescription rackDesc;
		rackDesc.nChannelsPerVoice   = 2;
		rackDesc.nVoices             = m_option->reverbBussNum;
		rackDesc.pVoiceDefn          = sceNgsVoiceDefGetReverbBuss();
		rackDesc.nMaxPatchesPerInput = m_option->stereoVoiceNum  + m_option->monoVoiceNum;
		rackDesc.nPatchesPerOutput   = 4;

		ret = m_reverbRack.initialize(sysHandle, rackDesc);
		if( ret != SCE_NGS_OK )	{
			return ret;
		}
	}


	{
		//E Create the mixer buss
		//J ミキサーバスの作成
		SceNgsRackDescription rackDesc;
		rackDesc.nChannelsPerVoice   = 2;
		rackDesc.nVoices             = m_option->mixerBussNum;
		rackDesc.pVoiceDefn          = sceNgsVoiceDefGetMixerBuss();
		rackDesc.nMaxPatchesPerInput = m_option->stereoVoiceNum  +  m_option->monoVoiceNum;
		rackDesc.nPatchesPerOutput   = 4;

		ret = m_mixerRack.initialize(sysHandle, rackDesc);
		if( ret != SCE_NGS_OK )	{
			return ret;
		}
	}

	return SCE_OK;
}


int ssai::VoiceManager::finalize(void)
{
	//E Free the memory for the management structure
	//J 管理構造体用メモリの解放
	m_reverbRack.finalize();
	m_mixerRack.finalize();
	m_at9Rack.finalize();
	m_stereoRack.finalize();
	m_monoRack.finalize();

	return SCE_OK;

}




int ssai::VoiceManager::checkFinishedVoices(void)
{
	int returnCode;
	returnCode = m_monoRack.checkFinishedVoice();
	if( returnCode != SCE_OK ){
		SCE_DBG_LOG_ERROR( "process: checkFinishedMonoVoice() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	returnCode = m_stereoRack.checkFinishedVoice();
	if( returnCode != SCE_OK ){
		SCE_DBG_LOG_ERROR( "process: checkFinishedStereoVoice() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	returnCode = m_at9Rack.checkFinishedVoice();
	if( returnCode != SCE_OK )	{
		SCE_DBG_LOG_ERROR( "process: checkFinishedAt9Voice() failed: 0x%08X\n", returnCode );
		return returnCode;
	}
	return SCE_OK;
}

ssai::VoiceRack *ssai::VoiceManager::getVoiceRack(bool isAt9, int channels)
{
	if(isAt9){
		return &m_at9Rack;
	}else{
		if(channels == 1){
			return &m_monoRack;
		}else if(channels == 2)	{
			return &m_stereoRack;
		}else{
			return NULL;
		}
	}
}

// --------------------------------------------------------------------------------------------------

int ssa::createAudioContext(AudioContext **outContext, const AudioContextOption *option)
{
	ssai::AudioContextImpl *impl = new ssai::AudioContextImpl();
	int ret  = impl->initialize(option);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outContext  = impl;
	return SCE_OK;
}

#endif /* _SCE_TARGET_OS_PSP2 */