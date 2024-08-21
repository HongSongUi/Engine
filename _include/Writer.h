#pragma once
#include <string>
#include <d2d1helper.h>
#include <dwrite.h>
#pragma comment( lib, "d2d1.lib")
#pragma comment( lib, "dwrite.lib")
class Writer
{
private:
	RECT ClientRect;
public:
	ID2D1Factory* D2DFactory;
	IDWriteFactory* WriteFactory;
	ID2D1RenderTarget* D2DRenderTarget;
	IDWriteTextFormat* TextFormat;
	ID2D1SolidColorBrush* TextColor;
	std::wstring DefaultText;
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
public:
	HRESULT Set(IDXGISurface1* Surface);
	bool Draw(float x, float y, std::wstring Text, D2D1_COLOR_F color = { 0,0,0,1 });
	void SetClientRect(RECT rect);
	HRESULT DeleteDxResource();
	HRESULT CreateDxResource();
};

