#include "Writer.h"

bool Writer::Init()
{
    HRESULT hr;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DFactory);
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&WriteFactory);
    hr = WriteFactory->CreateTextFormat(L"°íµñ", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20,
        L"ko-kr", &TextFormat);
    DefaultText = L"Hello";
    return true;
}

bool Writer::Frame()
{
    return true;
}

bool Writer::Render()
{
    Draw(0, 0, DefaultText, { 1,0,0,1 });
    return true;
}

bool Writer::Release()
{
    DeleteDxResource();

    if (WriteFactory)WriteFactory->Release();
    if (D2DFactory)D2DFactory->Release();
    return true;
}

HRESULT Writer::Set(IDXGISurface1* Surface)
{
    HRESULT hr;
    D2D1_RENDER_TARGET_PROPERTIES props;
    ZeroMemory(&props, sizeof(props));
    props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
    props.dpiX = 96;
    props.dpiY = 96;
    props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

    hr = D2DFactory->CreateDxgiSurfaceRenderTarget(Surface, &props, &D2DRenderTarget);
    CreateDxResource();
    hr = D2DRenderTarget->CreateSolidColorBrush({ 0,0,0,1 }, &TextColor);
    return hr;
}

bool Writer::Draw(float x, float y, std::wstring Text, D2D1_COLOR_F color)
{
    D2DRenderTarget->BeginDraw();

    D2D_RECT_F rt = { x,y,ClientRect.right,ClientRect.bottom };
    TextColor->SetColor(color);
    TextColor->SetOpacity(1.0f);
    D2DRenderTarget->DrawText(Text.c_str(), Text.size(), TextFormat, rt, TextColor);
    D2DRenderTarget->EndDraw();

    return true;
}

void Writer::SetClientRect(RECT rect)
{
    ClientRect = rect;
}

HRESULT Writer::DeleteDxResource()
{
    if (TextColor) TextColor->Release();
    if (D2DRenderTarget) D2DRenderTarget->Release();
    TextColor = nullptr;
    D2DRenderTarget = nullptr;
    return S_OK;
}

HRESULT Writer::CreateDxResource()
{
    return S_OK;
}
