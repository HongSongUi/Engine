#pragma once
#include <d3dcompiler.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
class DxState
{
public:
	static ID3D11SamplerState* _DefaultSS;
	static ID3D11BlendState* _DefaultBS;
	static ID3D11RasterizerState* _DefaultRSWireFrame;
	static ID3D11RasterizerState* _DefaultRSSolid;
	static ID3D11DepthStencilState* _DefaultDepthStencil;
public:
	static bool SetState(ID3D11Device* d3dDevice);
	static bool Release();
};

