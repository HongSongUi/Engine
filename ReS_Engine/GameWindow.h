#pragma once

#include <Windows.h>

class GameWindow
{
public:
	HINSTANCE	HInst;
	HWND		Hwnd;
	DWORD       Style;
	RECT		WindowRect;
	RECT		ClientRect;
	UINT		ClientWidth;
	UINT		ClientHeight;
	UINT		TotalWidth;
	UINT		TotalHeight;
	bool 		ResizeWindow;
public:
	bool		SetWindow(HINSTANCE hInstance, const WCHAR* Title, UINT width, UINT height);
	ATOM		MyRegisterClass();
	void		CenterWindow();
	BOOL		InitInstance(const WCHAR* Title, UINT width, UINT height);
	void		SetWinhWnd(HWND hWnd);
	RECT		GetClientRtSize();
	void		CalcWindowRect();
public:
	virtual bool		Run();
	virtual LRESULT		MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	GameWindow();
};

