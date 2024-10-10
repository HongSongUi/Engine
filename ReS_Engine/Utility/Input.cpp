#include "Input.h"

bool Input::Init()
{
    ZeroMemory(KeyState, sizeof(DWORD) * 256); // memory init
    ::GetCursorPos(&MousePos);// Get the current position of the mouse cursor
    ::ScreenToClient(InputHwnd, &MousePos);// Convert the screen coordinates to client coordinates
    PreMousePos = MousePos;
    return true;
}

bool Input::Frame()
{
	::GetCursorPos(&MousePos);
	::ScreenToClient(InputHwnd, &MousePos);
	// Calculate the offset of the mouse movement
	OffSet.x = MousePos.x - PreMousePos.x;
	OffSet.y = MousePos.y - PreMousePos.y;

	for (int i = 0; i < 255; i++) // Check the state of each key from 0 to 254
	{
		SHORT sKey = ::GetAsyncKeyState(i);// Get the async key state of the key at index i

		if (sKey & 0x8000)// If the key is currently pressed down
		{
			if (KeyState[i] == KEY_FREE || KeyState[i] == KEY_UP)// If the key was previously free or just released, mark it as pushed
			{
				KeyState[i] = KEY_PUSH;
			}
			else 
			{
				KeyState[i] = KEY_HOLD;
			}
		}
		else // If the key is currently not pressed down
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
	PreMousePos = MousePos;// Update the previous mouse position for the next frame
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
