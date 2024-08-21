#pragma once
#include "ReSUtility.h"
#include <Windows.h>
enum KeyState 
{
	KEY_FREE = 0,
	KEY_UP,
	KEY_PUSH,
	KEY_HOLD,
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
#define GameInput Input::GetInstance()

