#include "GameWindow.h"
#include <d3d11.h>
#include <crtdbg.h>
GameWindow* MyWindow = nullptr;
LRESULT CALLBACK  WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    _ASSERT(MyWindow);
    return MyWindow->MsgProc(hWnd, message, wParam, lParam);
}
bool GameWindow::SetWindow(HINSTANCE hInstance, const WCHAR* Title, UINT width, UINT height)
{
    HInst = hInstance;
    MyRegisterClass();
    if (!InitInstance(Title, width, height))
    {
        return false;
    }
    return true;
}

ATOM GameWindow::MyRegisterClass()
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;

    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = HInst;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(200, 200, 200));
    wcex.lpszClassName = L"Window";
    wcex.hIconSm = 0; 
    wcex.hIcon = 0;
    wcex.lpszMenuName = 0;
    return RegisterClassExW(&wcex);
}

void GameWindow::CenterWindow()
{

    UINT iScreenWidht = GetSystemMetrics(SM_CXFULLSCREEN);
    UINT iScreenHieght = GetSystemMetrics(SM_CYFULLSCREEN);
    UINT cx, cy;
    cx = (iScreenWidht - (WindowRect.right - WindowRect.left)) * 0.5f;
    cy = (iScreenHieght - (WindowRect.bottom - WindowRect.top)) * 0.5f;

    MoveWindow(Hwnd, cx, cy, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, true);
}

BOOL GameWindow::InitInstance(const WCHAR* Title, UINT width, UINT height)
{
    Style = WS_OVERLAPPEDWINDOW;
    RECT rc = { 0,0,width , height };
    AdjustWindowRect(&rc, Style, FALSE);
    Hwnd = CreateWindowW(L"Window", Title, Style,  0, 0,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr, nullptr,
        HInst, nullptr);


    if (!Hwnd)
    {
        return FALSE;
    }
    CalcWindowRect();
    CenterWindow();
    ShowWindow(Hwnd, SW_SHOW);
    return TRUE;
}

void GameWindow::SetWinhWnd(HWND hWnd)
{
    Hwnd = hWnd;
    CalcWindowRect();
}

RECT GameWindow::GetClientRtSize()
{
    GetClientRect(Hwnd, &ClientRect);
    return ClientRect;
}

void GameWindow::CalcWindowRect()
{
    GetWindowRect(Hwnd, &WindowRect);
    GetClientRect(Hwnd, &ClientRect);
    ClientWidth = ClientRect.right - ClientRect.left;
    ClientHeight = ClientRect.bottom - ClientRect.top;
}

bool GameWindow::Run()
{  
    MSG msg = { 0, };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else 
        {
            return true;
        }
    }

    return false;
}

LRESULT GameWindow::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        if (SIZE_MINIMIZED != wParam) {
            TotalWidth = LOWORD(lParam);
            TotalHeight = HIWORD(lParam);
            RECT tmp = ClientRect;
            CalcWindowRect();
            if (tmp == ClientRect) {
                break;
            }
            ResizeWindow = true;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

GameWindow::GameWindow()
{
    ResizeWindow = false;
    MyWindow = this;
}
