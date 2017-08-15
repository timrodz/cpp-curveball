#include "AudioManager.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include <libsysmodule.h>
#include <scream\sce_scream.h>

void AudioManager::Initialize()
{
	//sceSysmoduleLoadModule(SCE_SYSMODULE_NGS2);

	//SceScreamPlatformInitEx2 screamInit;

	//memset(&screamInit, 0, sizeof(SceScreamPlatformInitEx2));
	//screamInit.size = sizeof(SceScreamPlatformInitEx2);

	//// Set default scream arguments
	//sceScreamFillDefaultScreamPlatformInitArgsEx2(&screamInit);

	//// Specify alloc and dealloc static functions
	//screamInit.memAlloc = this->audio_malloc;
	//screamInit.memFree = this->audio_free;

	//// Start soundsystem
	//sceScreamStartSoundSystemEx2(&screamInit);

	//printf("===================================== SYSTEM STATE\n");
	//if (sceScreamGetSystemRunning())
	//{
	//	printf("Sound system is running!\n");
	//}
	//else
	//{
	//	printf("Sound system not loaded correctly\n");
	//}

	//printf("===================================== LOADING DATA BANK\n");
	////m_screamBank = "app0/bgMusic.bnk", 0;
	//m_screamBank = sceScreamBankLoadEx("app0/dankBank.bnk", 0);

	//if (m_screamBank == nullptr)
	//{
	//	printf("===================================== SOUND BANK STATE\n");
	//	printf("Scream bank not loaded correctly\n");
	//	printf("===================================== GET LAST LOAD ERROR\n");
	//	sceScreamGetLastLoadError();
	//	printf("===================================== END LAST LOAD ERROR\n");
	//}

	//int soundCount = sceScreamBankGetNumSoundsInBank(m_screamBank);
	//printf("===================================== SOUND LIST\n");
	//printf("Sound list count: %i\n", soundCount);

	//memset(&m_soundParams, 0, sizeof(SceScreamSoundParams));
	//m_soundParams.size = sizeof(SceScreamSoundParams);

	//m_soundParams.synthParams.flags = SCE_SCREAM_SND_SYNTH_FLAG_USE_AUDIO_3D;

	//m_soundParams.synthParams.mask = SCE_SCREAM_SND_MASK_SYNTH_FLAGS;

	//m_soundParams.gain = 1.0f;
	//m_soundParams.azimuth = 0.0f;
	//m_soundParams.distance = 1.0f;

	//m_soundParams.synthParams.mask = SCE_SCREAM_SND_MASK_SYNTH_PARAMS | SCE_SCREAM_SND_MASK_GAIN | SCE_SCREAM_SND_MASK_PAN_AZIMUTH | SCE_SCREAM_SND_MASK_DISTANCE;

}

void AudioManager::PlaySound(const char* sound)
{
	sceScreamPlaySoundByNameEx(m_screamBank, "bugs_big_one.wav", &m_soundParams);
}

void* AudioManager::audio_malloc(int32_t bytes, int32_t use)
{
	void* p;

	p = malloc(bytes);

	if (p) return p;

	return NULL;
}

void AudioManager::audio_free(void* memory)
{
	free(memory);
}