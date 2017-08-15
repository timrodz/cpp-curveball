/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include "ngs_internal.h"

#include <ngs.h>
#include <ngs/error.h>
#include <libdbg.h>

namespace ssa = sce::SampleUtil::Audio;
namespace ssai = sce::SampleUtil::Audio::Internal;


// ---------------------------------------------------------------------------------
// Buss class
// ---------------------------------------------------------------------------------

int ssai::BussImpl::initialize(BussRack *bussRack, SceNgsHVoice	 voiceHandler)
{
	m_bussRack = bussRack;
	m_voiceHandler = voiceHandler;
	m_state = kVoiceStateAvailable;
	return SCE_OK;
}

int ssai::BussImpl::connectOutput(unsigned int nSourceOutputIndex, 
							 SceNgsHVoice hVoiceDestination, 
							 unsigned int nTargetInputIndex)
{
	return connectRacks(this->m_voiceHandler, 
		nSourceOutputIndex, 
		hVoiceDestination, nTargetInputIndex, 
		&this->patchHandler[nSourceOutputIndex]);
}


int ssai::BussImpl::close(void)
{
	if(m_bussRack == NULL){
		return SCE_OK;
	}

	if(m_state == kVoiceStateInvalid){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM	;
	}

	int ret = sceNgsVoiceKill(m_voiceHandler);
	if(ret < SCE_OK){
		return ret;
	}

	m_state = kVoiceStateInvalid;

	m_bussRack->m_freeBussHandles.push_back(m_voiceHandler);
	m_bussRack = NULL;
	return SCE_OK;
}


int ssai::BussImpl::getData(void *buffer, uint32_t size)
{
	return sceNgsVoiceGetStateData(m_voiceHandler, SCE_NGS_MASTER_BUSS_OUTPUT_MODULE,
		buffer, size);
}

int ssai::BussImpl::play(void)
{
	int ret = sceNgsVoicePlay(m_voiceHandler);
	if(ret >= SCE_OK){
		m_state = kVoiceStatePlaying;
	}
	return ret;
}

int ssai::BussImpl::pause(void)
{
	if(m_state != kVoiceStatePlaying){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	return sceNgsVoicePause(m_voiceHandler);
}

int ssai::BussImpl::resume(void)
{
	if(m_state != kVoiceStatePause){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	return sceNgsVoiceResume(m_voiceHandler);
}

int ssai::BussImpl::setReverbMode(SceNgsHSynSystem sysHandle, int reverbMode)
{
	SceNgsBufferInfo   bufferInfo;

	int returnCode = SCE_OK;
	// Get reverb parameters
	returnCode = sceNgsVoiceLockParams( m_voiceHandler, SCE_NGS_REVERB_VOICE_REVERB_MODULE, SCE_NGS_REVERB_PARAMS_STRUCT_ID, &bufferInfo );
	if ( returnCode != SCE_OK ) {
		SCE_DBG_LOG_ERROR( "soundUtilBussCreateReverb: sceNgsVoiceLockParams() failed: 0x%08X\n", returnCode );
		return returnCode;
	}

	// Set reverb parameters
	memset( bufferInfo.data, 0, bufferInfo.size);

	returnCode = sceNgsModuleGetPreset( sysHandle, SCE_NGS_REVERB_ID, reverbMode, &bufferInfo);
	if(returnCode != SCE_OK){
		sceNgsVoiceUnlockParams( m_voiceHandler, SCE_NGS_REVERB_VOICE_REVERB_MODULE );
		return returnCode;
	}

	// Update reverb parameters
	returnCode = sceNgsVoiceUnlockParams( m_voiceHandler, SCE_NGS_REVERB_VOICE_REVERB_MODULE );
	if ( returnCode != SCE_OK ) {
		SCE_DBG_LOG_ERROR( "soundUtilBussCreateReverb: sceNgsVoiceUnlockParams  failed: 0x%08X\n", returnCode );
		return returnCode;
	}
	return SCE_OK;

}

int ssai::BussImpl::setOutput(uint32_t nSourceOutputIndex, Buss *_buss)
{
	BussImpl *buss = dynamic_cast<BussImpl *>(_buss);
	if(buss != NULL){
		return connectOutput(nSourceOutputIndex, buss->m_voiceHandler, 0);
	}else{
		return disConnectOutput(nSourceOutputIndex);
	}
}


int ssai::BussImpl::disConnectOutput(uint32_t nSourceOutputIndex)
{
	if(nSourceOutputIndex >= NUM_OUTPUT_INDEX){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	if(patchHandler[nSourceOutputIndex] == NULL){
		return SCE_OK;
	}
	int returnCode = sceNgsPatchRemoveRouting(this->patchHandler[nSourceOutputIndex]);
	if(returnCode < 0){
		return returnCode;
	}
	this->patchHandler[nSourceOutputIndex] = NULL;
	return SCE_OK;

}

// ---------------------------------------------------------------------------------
// BussRack class
// ---------------------------------------------------------------------------------


int ssai::BussRack::initialize(SceNgsHSynSystem		sysHandle,
	const SceNgsRackDescription &rackDesc)
{
	SceNgsBufferInfo      bufferInfo;
	int ret = sceNgsRackGetRequiredMemorySize( sysHandle, &rackDesc, &bufferInfo.size);
	if( ret != SCE_NGS_OK )	{
		SCE_DBG_LOG_ERROR( "createRacks: sceNgsRackGetRequiredMemorySize(mono-player) failed: 0x%08X\n", ret );
		return ret;
	} 

	m_rackMem = ::memalign(16, bufferInfo.size);
	if( m_rackMem == NULL) {
		SCE_DBG_LOG_ERROR( "createRacks: malloc(mono-player) failed for %d bytes\n", bufferInfo.size );
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}

	bufferInfo.data = m_rackMem;
	ret = sceNgsRackInit( sysHandle, &bufferInfo, &rackDesc, &m_rack );
	if( ret != SCE_NGS_OK )
	{
		SCE_DBG_LOG_ERROR( "createRacks: sceNgsRackInit(mono-player) failed: 0x%08X\n", ret );
		return ret;
	}


	for(uint32_t i=0; i<rackDesc.nVoices; i++){
		SceNgsHVoice voice;
		ret = sceNgsRackGetVoiceHandle( m_rack, i, &voice );
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

		m_freeBussHandles.push_back(voice);
	}

	return SCE_OK;
}

int ssai::BussRack::finalize(void)
{
	m_freeBussHandles.clear();

	int ret = sceNgsRackRelease(m_rack, NULL);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	::free(m_rackMem);
	return SCE_OK;
}


ssa::Buss *ssai::BussRack::createBuss(void)
{
	if(m_freeBussHandles.size() == 0){
		return NULL;
	}
	BussImpl* buss = new BussImpl;
	if(buss == NULL){
		return NULL;
	}

	std::vector<SceNgsHVoice>::iterator it = m_freeBussHandles.begin();
	SceNgsHVoice voiceHandler = *it;
	m_freeBussHandles.erase(it);

	int ret = buss->initialize(this, voiceHandler);
	if(ret != SCE_OK){
		delete buss;
		return NULL;
	}

	return buss;
}
int ssai::BussRack::releaseBuss(Buss *_buss)
{
	delete _buss;
	return SCE_OK;
}



// ---------------------------------------------------------------------------------
// MasterBuss class
// ---------------------------------------------------------------------------------

int ssai::MasterBuss::initialize(SceNgsHSynSystem sysHandle, uint32_t voiceTotal, uint32_t bussTotal, int idStart)
{
	int ret;

	//E Create the master buss
	//J マスターバスの作成
	SceNgsRackDescription rackDesc;
	rackDesc.nChannelsPerVoice      = 2;
	rackDesc.nVoices                = 2;
	rackDesc.pVoiceDefn             = sceNgsVoiceDefGetMasterBuss();
	rackDesc.nMaxPatchesPerInput    = voiceTotal + bussTotal; 

	rackDesc.nPatchesPerOutput      = 0;

	ret = m_masterRack.initialize(sysHandle, rackDesc);
	if( ret != SCE_NGS_OK )	{
		return ret;
	}

	/*--------------------------------------------------------------- */
	m_mainBuss = NULL;
	m_bgmBuss = NULL;

	ssa::Buss *b = m_masterRack.createBuss();
	m_mainBuss = dynamic_cast<BussImpl*>(b);
	SCE_SAMPLE_UTIL_ASSERT(m_mainBuss != NULL);
	
	m_bgmBuss = dynamic_cast<BussImpl*>(m_masterRack.createBuss());
	SCE_SAMPLE_UTIL_ASSERT(m_bgmBuss != NULL);

	return SCE_OK;


}

int ssai::MasterBuss::finalize(void)
{
	if(m_mainBuss != NULL){
		m_masterRack.releaseBuss(m_mainBuss);
		m_mainBuss = NULL;
	}
	if(m_bgmBuss != NULL){
		m_masterRack.releaseBuss(m_bgmBuss);
		m_bgmBuss = NULL;
	}

	m_masterRack.finalize();
	return SCE_OK;
}

int ssai::MasterBuss::enableBgmPort(bool enable)
{
	if(enable)	{
		if(m_bgmBuss->getState() == kVoiceStatePause){
			m_bgmBuss->resume();
		}else{
			m_bgmBuss->play();
		}
	}else{
		if(m_bgmBuss->getState() == kVoiceStatePlaying){
			m_bgmBuss->pause();
		}
	}
	return SCE_OK;
}


ssa::Buss *ssai::MasterBuss::getMainBuss(void)
{
	return m_mainBuss;
}

ssa::Buss *ssai::MasterBuss::getBgmBuss(void)
{
	return m_bgmBuss;
}

int ssai::MasterBuss::start(void)
{
	int ret;

	ret = m_mainBuss->play();
	if(ret != SCE_OK) {
		SCE_DBG_LOG_ERROR( "soundUtilInit: sceNgsVoicePlay failed: 0x%08X\n", ret );
		return ret;
	}

	ret = m_bgmBuss->play();
	if(ret != SCE_OK) {
		SCE_DBG_LOG_ERROR( "soundUtilInit: sceNgsVoicePlay failed: 0x%08X\n", ret );
		return ret;
	}

	return SCE_OK;
}


#endif /* _SCE_TARGET_OS_PSP2 */