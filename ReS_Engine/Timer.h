#pragma once
#include <Windows.h>
#include "ReSUtility.h"
#pragma comment(lib, "winmm.lib") 
class Timer : public Singleton<Timer>
{
public:
	float InGameTimer = 0.0f;
	float ElapseTimer = 10.0f;
	UINT FPS = 0;
	std::wstring TimerString;
private:
	DWORD BeforeTime;
	UINT FPSCounter = 0;
	float FPSTimer = 0.0f;
public:
	bool Init();
	bool Render();
	bool Frame();
	bool Release();
};

#define GameTimer Timer::GetInstance()