#pragma once
#include <string>
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#ifdef _DEBUG
#pragma comment(lib, "DirectXTK_D.lib")
#else
#pragma comment(lib, "DirectXTK_R.lib")
#endif
class Texture
{
public:
	ID3D11Texture2D*			Texture2D = nullptr;
	ID3D11ShaderResourceView*	TextureSRV = nullptr;
	D3D11_TEXTURE2D_DESC		Desc;
public:
	HRESULT Load(ID3D11Device* Device, ID3D11DeviceContext* Context, std::wstring FileName);
	void	Apply(ID3D11DeviceContext* Context,  UINT Slot = 0);
	bool	Release();
};

