#include "Shader.h"

bool Shader::Init()
{
	return true;
}

bool Shader::Frame()
{
	return true;
}

bool Shader::Render()
{
	return true;
}

bool Shader::Release()
{
	//Resource Release
	if (VsCode) VsCode->Release();
	if (PsCode) PsCode->Release();
	if (VertexShader) VertexShader->Release();
	if (PixelShader) PixelShader->Release();
	return true;
}

HRESULT Shader::Load(std::wstring FileName, ID3D11Device* Device)
{
	//«Õ«¡«¤«ëÙ£ªÇshader load
	HRESULT hr;
	ID3DBlob* ErrorCode = nullptr;
	hr = D3DCompileFromFile(FileName.c_str(), NULL, NULL, "VS", "vs_5_0", 0, 0, &VsCode, &ErrorCode);
	if (FAILED(hr)) {
		if (ErrorCode) {
			OutputDebugStringA((char*)ErrorCode->GetBufferPointer());
			ErrorCode->Release();
		}
		return S_FALSE;
	}
	hr = Device->CreateVertexShader(VsCode->GetBufferPointer(), VsCode->GetBufferSize(), NULL, &VertexShader);

	hr = D3DCompileFromFile(FileName.c_str(), NULL, NULL, "PS", "ps_5_0", 0, 0, &PsCode, &ErrorCode);
	if (FAILED(hr)) {
		if (ErrorCode) {
			OutputDebugStringA((char*)ErrorCode->GetBufferPointer());
			ErrorCode->Release();
		}
		return S_FALSE;
	}
	hr = Device->CreatePixelShader(PsCode->GetBufferPointer(), PsCode->GetBufferSize(), NULL, &PixelShader);

	return hr;
}
