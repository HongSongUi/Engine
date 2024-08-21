#include "Input.h"

bool Input::Init()
{
    ZeroMemory(KeyState, sizeof(DWORD) * 256);
    ::GetCursorPos(&MousePos);
    ::ScreenToClient(InputHwnd, &MousePos);
    PreMousePos = MousePos;
    return true;
}

bool Input::Frame()
{
	::GetCursorPos(&MousePos);
	::ScreenToClient(InputHwnd, &MousePos);
	OffSet.x = MousePos.x - PreMousePos.x;
	OffSet.y = MousePos.y - PreMousePos.y;

	for (int i = 0; i < 255; i++) 
	{
		SHORT sKey = ::GetAsyncKeyState(i);

		if (sKey & 0x8000)
		{
			if (KeyState[i] == KEY_FREE || KeyState[i] == KEY_UP)
			{
				KeyState[i] = KEY_PUSH;
			}
			else 
			{
				KeyState[i] = KEY_HOLD;
			}
		}
		else 
		{
			if (KeyState[i] == KEY_PUSH || KeyState[i] == KEY_HOLD)
			{
				KeyState[i] = KEY_UP;
			}
			else 
			{
				KeyState[i] = KEY_FREE;
			}
		}
	}
	PreMousePos = MousePos;
	return true;

}

bool Input::Render()
{
    return true;
}

bool Input::Release()
{
    return true;
}

DWORD Input::GetKey(DWORD Key)
{
    return KeyState[Key];
}

void Input::SetWinHwnd(HWND hwnd)
{
    InputHwnd = hwnd;
}
