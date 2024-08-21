#include "RenderToTexture.h"

bool RenderToTexture::Create(ID3D11Device* Device, FLOAT width, FLOAT height)
{
    HRESULT hr;
    ViewPort.Width = width;
    ViewPort.Height = height;
    ViewPort.TopLeftX = 0;
    ViewPort.TopLeftY = 0;
    ViewPort.MinDepth = 0.0f;
    ViewPort.MaxDepth = 1.0f;


    TextureDesc.Width = (UINT)width;
    TextureDesc.Height = (UINT)height;
    TextureDesc.MipLevels = 1;
    TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;
    TextureDesc.ArraySize = 1;

    if (FAILED(hr = Device->CreateTexture2D(&TextureDesc, NULL, &Texture2D))) {
        return false;
    }
    if (FAILED(hr = Device->CreateShaderResourceView(Texture2D, NULL, &ShaderResourceView))) {
        return false;
    }
    if (FAILED(hr = Device->CreateRenderTargetView(Texture2D, NULL, &RenderTargetView))) {
        return false;
    }
    ID3D11Texture2D* DSTexture = nullptr;
    D3D11_TEXTURE2D_DESC DescDepth;
    DescDepth.Width = width;
    DescDepth.Height = height;
    DescDepth.MipLevels = 1;
    DescDepth.ArraySize = 1;
    DescDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
    DescDepth.SampleDesc.Count = 1;
    DescDepth.SampleDesc.Quality = 0;
    DescDepth.Usage = D3D11_USAGE_DEFAULT;
    DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    DescDepth.CPUAccessFlags = 0;
    DescDepth.MiscFlags = 0;
    if (FAILED(hr = Device->CreateTexture2D(&DescDepth, NULL, &DSTexture)))
    {
        return false;
    }
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if (FAILED(hr = Device->CreateDepthStencilView(DSTexture, &dsvDesc, &DepthStencilView)))
    {
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    if (FAILED(hr = Device->CreateShaderResourceView(DSTexture, &srvDesc, &DepthSRV)))
    {
        return false;
    }

    DSTexture->Release();
    DSTexture = nullptr;
    return true;
}

bool RenderToTexture::Begin(ID3D11DeviceContext* Context)
{
    ID3D11RenderTargetView* NULLRTV = NULL;
    Context->OMSetRenderTargets(1, &NULLRTV, NULL);
    Context->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);
    const FLOAT color[] = { 0.5f,0.5f,0.5f,1 };
    Context->ClearRenderTargetView(RenderTargetView, color);
    Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);
    Context->RSSetViewports(1, &ViewPort);
    return true;
}

void RenderToTexture::End(ID3D11DeviceContext* Context)
{
    Context->OMSetRenderTargets(1, &OldRenderTarget, OldDepthStencil);
    Context->RSSetViewports(1, OldViewPort);
}

bool RenderToTexture::Release()
{
    if (RenderTargetView) {
        RenderTargetView->Release();
    }
    if (DepthStencilView) {
        DepthStencilView->Release();
    }
    if (ShaderResourceView) {
        ShaderResourceView->Release();
    }
    if (DepthSRV) {
        DepthSRV->Release();
    }
    if (Texture2D) {
        Texture2D->Release();
    }
    RenderTargetView = nullptr;
    DepthStencilView = nullptr;
    ShaderResourceView = nullptr;
    DepthSRV = nullptr;
    Texture2D = nullptr;
    return true;
}
