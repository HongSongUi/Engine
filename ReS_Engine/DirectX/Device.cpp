#include "Device.h"

bool Device::Init()
{
	if (FAILED(CreateDevice())) {
		return false;
	}
	if (FAILED(CreateFactory())) {
		return false;
	}
	if (FAILED(CreateSwapChain())) {
		return false;
	}
	if (FAILED(CreateRenderTargetView())) {
		return false;
	}
	if (FAILED(CreateDepthStencilView())) {
		return false;
	}
	CreateViewPort();
	return true;
}
bool Device::Render()
{
	PreRender();
	PostRender();
	return true;
}

bool Device::PreRender()
{
	D3D11Context->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
	float color[4] = { 0.25f,0.25f,0.25f,1.0f };
	D3D11Context->ClearRenderTargetView(RenderTargetView, color);
	D3D11Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	D3D11Context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

bool Device::PostRender()
{
	SwapChain->Present(0, 0);
	return true;
}
bool Device::Frame()
{
	return true;
}

bool Device::Release()
{
	if (D3D11Device)  D3D11Device->Release();
	if (D3D11Context) D3D11Context->Release();
	if (GIFactory) GIFactory->Release();
	if (SwapChain) SwapChain->Release();
	if (RenderTargetView) RenderTargetView->Release();
	if (DepthStencilView) DepthStencilView->Release();
	D3D11Device = nullptr;
	D3D11Context = nullptr;
	GIFactory = nullptr;
	SwapChain = nullptr;
	RenderTargetView = nullptr;
	DepthStencilView = nullptr;
	return true;
}

HRESULT Device::CreateDevice()
{
	HRESULT hr;

	UINT CreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG

	D3D_FEATURE_LEVEL FeatureLevel;
	D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, CreateDeviceFlags, FeatureLevels, 1, D3D11_SDK_VERSION,
		&D3D11Device, &FeatureLevel, &D3D11Context);

	return hr;
}

HRESULT Device::CreateFactory()
{
	HRESULT hr;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&GIFactory);
	return hr;
}

HRESULT Device::CreateSwapChain()
{
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = ClientRect.right;
	sd.BufferDesc.Height = ClientRect.bottom;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = Hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	hr = GIFactory->CreateSwapChain(D3D11Device, &sd, &SwapChain);
	return hr;
}

HRESULT Device::CreateRenderTargetView()
{
	HRESULT hr;
	ID3D11Texture2D* pBackBuffer = nullptr;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	hr = D3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &RenderTargetView);
	pBackBuffer->Release();
	return hr;
}

HRESULT Device::ResizeWindow(UINT width, UINT height)
{
	HRESULT hr;
	if (D3D11Device == nullptr) {
		return S_OK;
	}
	ClientRect.right = width;
	ClientRect.bottom = height;
	D3D11Context->OMSetRenderTargets(0, nullptr, NULL);
	RenderTargetView->Release();
	RenderTargetView = nullptr;
	DepthStencilView->Release();
	DepthStencilView = nullptr;
	DXGI_SWAP_CHAIN_DESC CurrentSD;// , AfterSD;
	SwapChain->GetDesc(&CurrentSD);
	hr = SwapChain->ResizeBuffers(CurrentSD.BufferCount, width, height, CurrentSD.BufferDesc.Format, 0);
	if (FAILED(CreateRenderTargetView())) 
	{
		return S_FALSE;
	}
	if (FAILED(CreateDepthStencilView())) 
	{
		return S_FALSE;
	}
	CreateViewPort();
	return S_OK;
}

HRESULT Device::CreateDepthStencilView()
{
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC scd;
	SwapChain->GetDesc(&scd);

	ID3D11Texture2D* DSTexture;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));

	td.Width = scd.BufferDesc.Width;
	td.Height = scd.BufferDesc.Height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R24G8_TYPELESS;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = D3D11Device->CreateTexture2D(&td, NULL, &DSTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dtvd;
	ZeroMemory(&dtvd, sizeof(dtvd));
	dtvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dtvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = D3D11Device->CreateDepthStencilView(DSTexture, &dtvd, &DepthStencilView);
	DSTexture->Release();

	return hr;
}

void Device::CreateViewPort()
{
	ViewPort.TopLeftX = 0.0f;
	ViewPort.TopLeftY = 0.0f;
	ViewPort.Width = ClientRect.right;
	ViewPort.Height = ClientRect.bottom;
	ViewPort.MinDepth = 0.0f;
	ViewPort.MaxDepth = 1.0f;
	D3D11Context->RSSetViewports(1, &ViewPort);
}

void Device::SetWindowData(RECT ClientRt, HWND Hwnd)
{
	ClientRect = ClientRt;
	this->Hwnd = Hwnd;
}
ID3D11Device* Device::GetDevice()
{
	return D3D11Device;
}

ID3D11DeviceContext* Device::GetContext()
{
	return D3D11Context;
}
