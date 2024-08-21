#pragma once
#include <d3dcompiler.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
class RenderToTexture
{
public:
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11ShaderResourceView* ShaderResourceView;
	ID3D11ShaderResourceView* DepthSRV;
	ID3D11Texture2D* Texture2D;
	D3D11_DEPTH_STENCIL_VIEW_DESC	DepthStencilDesc;
	D3D11_TEXTURE2D_DESC	TextureDesc;
	D3D11_VIEWPORT ViewPort;

	ID3D11RenderTargetView* OldRenderTarget;
	ID3D11DepthStencilView* OldDepthStencil;
	D3D11_VIEWPORT OldViewPort[D3D11_VIEWPORT_AND_SCISSORRECT_MAX_INDEX];
public:
	bool Create(ID3D11Device* Device, FLOAT width = 1000.0f, FLOAT height = 700.0f);
	bool Begin(ID3D11DeviceContext* Context);
	void End(ID3D11DeviceContext* Context);
	bool Release();
};

