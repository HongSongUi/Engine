#pragma once
#include <string>
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment(lib,"fmod_vc.lib")

class GameSound
{
public:
	FMOD::System* FSystem = nullptr;
	FMOD::Sound* FSound = nullptr;
	FMOD::Channel* FChannel = nullptr;
	float Volume;
	std::wstring Buffer;
public:
	void Pause();
	void VolumeAdj(bool adjust);
	bool Play(bool loop = false);
	bool PlayEffect(float volume, bool loop = false);
	void Stop();
	void SetLoop(bool loop = false);
	bool IsPlay();
	void SetVolume(float volume);
public:
	FMOD_RESULT Load(FMOD::System* system, std::wstring filename);
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
};

