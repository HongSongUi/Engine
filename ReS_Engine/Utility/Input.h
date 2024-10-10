#pragma once
#include "ReSUtility.h"
#include <Windows.h>
enum KeyState 
{
	KEY_FREE = 0,// Key is not pressed
	KEY_UP, // Key was just released
	KEY_PUSH, // Key was just pressed down
	KEY_HOLD,// Key is being held down
};
class Input : public Singleton<Input>
{
private:
	DWORD KeyState[256];
	HWND  InputHwnd;
public:
	POINT MousePos;
	POINT PreMousePos;
	POINT OffSet;
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
public:
	DWORD GetKey(DWORD Key);
	void SetWinHwnd(HWND hwnd);

};
#define GameInput Input::GetInstance() // using Singleton

