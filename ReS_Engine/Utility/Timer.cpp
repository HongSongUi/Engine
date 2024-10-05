#include "Timer.h"

float gGameTimer = 0.0f;
float gSecondPerFrame = 0.0f;

bool Timer::Init()
{
    InGameTimer = 0.0f;
    ElapseTimer = 0.0f;
    BeforeTime = timeGetTime();
    return true;
}

bool Timer::Render()
{
    TimerString = std::to_wstring(InGameTimer);
    TimerString += L"   ";
    TimerString += std::to_wstring(FPS);
    TimerString += L"\n";
    return true;
}

bool Timer::Frame()
{
	DWORD CurrentTime = timeGetTime();
	DWORD TimeBlank = CurrentTime - BeforeTime;
	ElapseTimer = TimeBlank / 1000.0f;
	InGameTimer += ElapseTimer;
	{
		FPSCounter++;
		FPSTimer += ElapseTimer;
		if (FPSTimer >= 1.0f) 
		{
			FPS = FPSCounter;
			FPSCounter = 0;
			FPSTimer = FPSTimer - 1.0f;
		}
	}
	gGameTimer = InGameTimer;
	gSecondPerFrame = ElapseTimer;
	BeforeTime = CurrentTime;
    return true;
}

bool Timer::Release()
{
    return true;
}
