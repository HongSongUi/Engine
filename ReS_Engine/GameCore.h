#pragma once
#include "GameWindow.h"
#include "Device.h"
#include "Timer.h"
#include "Input.h"
#include "Writer.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "SoundManager.h"
#include "DxState.h"
#include "BaseObject.h"
#include "RenderToTexture.h"
#include "DebugCamera.h"

class GameCore
{
protected:
	ID3D11Device*			D3D11Device = nullptr;
	ID3D11DeviceContext*	D3D11Context = nullptr;
	BaseObject			RenderTarget;
	GameWindow			Window;
	Device				GameDevice;
	HWND				Hwnd;
	Writer				GameWriter;
	RECT				ClientRect;
	RenderToTexture		RenderTexture;
public:
	DebugCamera		DebugCam;
	bool			IsGameRun;
	bool			UseDebugCam;
	bool			Is2DGame;
public:
	virtual bool Init();
	virtual bool Render();
	virtual bool Release();
	virtual bool Frame();
	virtual bool PreProcess();
	virtual bool PreFrame();
	virtual bool PostFrame();
	virtual bool PreRender();
	virtual bool PostRender();
	virtual bool PostProcess();
public:
	bool CoreInit();
	bool CoreFrame();
	bool CoreRender();
	bool CoreRelease();
	bool CorePreRender();
	bool CorePostRender();
public:
	bool					Run();
	bool					ToolRun();
	bool					SetWindow(HINSTANCE hInstance, const WCHAR* Title, UINT width, UINT height);
	void					ClearD3D11DeviceContext();
	void					UpdateCamString();
	void					SetHwnd(HWND hWnd);
	void					ReSizeWindow(UINT width, UINT height);
	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetContext();
	HWND					GetWindowHwnd();
	RECT					GetClientRect();
};

