#include "DxState.h"
ID3D11SamplerState* DxState::_DefaultSS = nullptr;
ID3D11BlendState* DxState::_DefaultBS = nullptr;

ID3D11RasterizerState* DxState::_DefaultRSWireFrame = nullptr;
ID3D11RasterizerState* DxState::_DefaultRSSolid = nullptr;

ID3D11DepthStencilState* DxState::_DefaultDepthStencil = nullptr;

bool DxState::SetState(ID3D11Device* d3dDevice)
{
	HRESULT hr;
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	hr = d3dDevice->CreateSamplerState(&sd, &_DefaultSS);

	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.DepthClipEnable = TRUE;
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	d3dDevice->CreateRasterizerState(&rd, &_DefaultRSWireFrame);

	rd.FillMode = D3D11_FILL_SOLID;
	d3dDevice->CreateRasterizerState(&rd, &_DefaultRSSolid);

	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	d3dDevice->CreateBlendState(&bd, &_DefaultBS);

	D3D11_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&dsd, sizeof(dsd));
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = d3dDevice->CreateDepthStencilState(&dsd, &_DefaultDepthStencil);
	return hr;
}

bool DxState::Release()
{
	if (_DefaultSS) _DefaultSS->Release();
	if (_DefaultRSSolid) _DefaultRSSolid->Release();
	if (_DefaultRSWireFrame) _DefaultRSWireFrame->Release();
	if (_DefaultBS) _DefaultBS->Release();
	if (_DefaultDepthStencil) _DefaultDepthStencil->Release();
	return true;
}
