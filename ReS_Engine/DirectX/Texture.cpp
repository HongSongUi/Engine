#include "Texture.h"

HRESULT Texture::Load(ID3D11Device* Device, ID3D11DeviceContext* Context, std::wstring FileName)
{
	//«Õ«¡«¤«ëÙ£ªÇtexture load
	HRESULT hr;
	hr = DirectX::CreateWICTextureFromFile(Device, Context, FileName.c_str(), (ID3D11Resource**)&Texture2D, &TextureSRV);
	if (FAILED(hr)) 
	{
		hr = DirectX::CreateDDSTextureFromFile(Device, Context, FileName.c_str(), (ID3D11Resource**)&Texture2D, &TextureSRV);
	}
	if (Texture2D)
	{
		Texture2D->GetDesc(&Desc);
	}
	return hr;
}

void Texture::Apply(ID3D11DeviceContext* Context, UINT Slot)
{
	Context->PSSetShaderResources(Slot, 1, &TextureSRV);
}

bool Texture::Release()
{
	if (Texture2D) Texture2D->Release();
	if (TextureSRV) TextureSRV->Release();

	return true;
}
