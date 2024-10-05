#pragma once
#include <string>
#include <d3dcompiler.h>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

class Shader
{
public:
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3DBlob* VsCode;
	ID3DBlob* PsCode;
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
public:
	HRESULT Load(std::wstring FileName, ID3D11Device* Device);
};

