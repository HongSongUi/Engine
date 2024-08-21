#pragma once
#include <memory>
#include <d3dcompiler.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
class Device
{
private:
	RECT ClientRect;
	HWND Hwnd;
public:
	ID3D11Device*	D3D11Device = nullptr;
	ID3D11DeviceContext*	D3D11Context = nullptr;
	IDXGIFactory*			GIFactory = nullptr;
	IDXGISwapChain*			SwapChain = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;
	ID3D11DepthStencilView* DepthStencilView = nullptr;
	D3D11_VIEWPORT			ViewPort;
public:
	bool Init();
	bool Frame();
	bool Render();
	bool PreRender();
	bool PostRender();
	bool Release();
public:
	HRESULT CreateDevice();
	HRESULT CreateFactory();
	HRESULT CreateSwapChain();
	HRESULT CreateRenderTargetView();
	HRESULT ResizeWindow(UINT width, UINT height);
	HRESULT CreateDepthStencilView();
	void	CreateViewPort();
	void	SetWindowData(RECT ClientRt, HWND Hwnd);
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
};

