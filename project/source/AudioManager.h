#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>
#include <libsysmodule.h>
#include <scream\sce_scream.h>

class AudioManager
{
public:

	//static AudioManager& Instance()
	//{
	//	static AudioManager instance;
	//	return instance;
	//}
	AudioManager()
	{
		Initialize();
	}

	~AudioManager() {}

	void Initialize();
	void PlaySound(const char*);

private:

	AudioManager(AudioManager const&) = delete;
	void operator=(AudioManager const&) = delete;

	static void* audio_malloc(int32_t bytes, int32_t use);
	static void audio_free(void*);

private:
	SceScreamSFXBlock2 *m_screamBank;
	SceScreamSoundParams m_soundParams;

};