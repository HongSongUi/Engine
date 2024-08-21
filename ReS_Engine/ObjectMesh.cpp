#include "ObjectMesh.h"
#include "TextureManager.h"
#include "ShaderManager.h"
bool ObjectMesh::Init()
{
    return true;
}

bool ObjectMesh::Frame()
{

    return true;
}

bool ObjectMesh::Release()
{
	if (VertexBuffer) VertexBuffer->Release();
	if (IndexBuffer) IndexBuffer->Release();
	if (ConstantBuffer) ConstantBuffer->Release();
	if (InputLayout) InputLayout->Release();
    return true;
}

bool ObjectMesh::PreRender()
{
	D3D11Context->PSSetShaderResources(0, 1, &TextureSRV);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D11Context->IASetInputLayout(InputLayout);
	D3D11Context->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	D3D11Context->VSSetShader(ShaderData->VertexShader, NULL, 0);
	D3D11Context->PSSetShader(ShaderData->PixelShader, NULL, 0);
	D3D11Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D11Context->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	D3D11Context->PSSetConstantBuffers(0, 1, &ConstantBuffer);
    return true;
}

bool ObjectMesh::Render()
{
	PreRender();
	PostRender();
	return true;
}

bool ObjectMesh::PostRender()
{
	if (IndexBuffer == nullptr)
	{
		D3D11Context->Draw(VertexList.size(), 0);
	}
	else 
	{
		D3D11Context->DrawIndexed(IndexList.size(), 0, 0);
	}
    return true;
}

HRESULT ObjectMesh::CreateVertexBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(Vertex) * VertexList.size();
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &VertexList.at(0);
	hr = D3D11Device->CreateBuffer(&bd, &sd, &VertexBuffer);
	return hr;

}

HRESULT ObjectMesh::CreateIndexBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(DWORD) * IndexList.size();
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &IndexList.at(0);
	hr = D3D11Device->CreateBuffer(&bd, &sd, &IndexBuffer);
	return hr;
}

HRESULT ObjectMesh::CreateInputLayout()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0,40,D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT size = sizeof(ied) / sizeof(ied[0]);
	hr = D3D11Device->CreateInputLayout(ied, size, ShaderData->VsCode->GetBufferPointer(), ShaderData->VsCode->GetBufferSize(), &InputLayout);
	return hr;
}

HRESULT ObjectMesh::CreateConstantBuffer()
{
	HRESULT hr;
	CreateConstantData();
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(Constant_Buffer) * 1;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &ConstBufferData;
	hr = D3D11Device->CreateBuffer(&bd, &sd, &ConstantBuffer);
	return hr;
}

void ObjectMesh::SetDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	D3D11Device = device;
	D3D11Context = context;
}

bool ObjectMesh::Create()
{
	if (FAILED(CreateVertexBuffer())) 
	{
		return false;
	}

	if (IndexList.size() > 0) 
	{
		if (FAILED(CreateIndexBuffer()))
		{
			return false;
		}
	}

	if (FAILED(CreateInputLayout())) 
	{
		return false;
	}

	if (FAILED(CreateConstantBuffer())) 
	{
		return false;
	}
    return true;
}

void ObjectMesh::UpdateVertexBuffer()
{
	D3D11Context->UpdateSubresource(VertexBuffer, 0, nullptr, &VertexList.at(0), 0, 0);
}

void ObjectMesh::SetShader(Shader* shader)
{
	ShaderData = shader;
}

void ObjectMesh::SetTexture(Texture* texture)
{
	TextureData = texture;
}

void ObjectMesh::SetVertexList(std::vector<Vertex>& list)
{
	VertexList = list;
}

void ObjectMesh::SetIndexList(std::vector<unsigned int>& list)
{
	IndexList = list;
}

void ObjectMesh::CreateConstantData()
{
	D3DXMatrixIdentity(&ConstBufferData.WorldMat);
	D3DXMatrixIdentity(&ConstBufferData.ViewMat);
	D3DXMatrixIdentity(&ConstBufferData.ProjMat);
	D3DXMatrixTranspose(&ConstBufferData.WorldMat, &ConstBufferData.WorldMat);
	D3DXMatrixTranspose(&ConstBufferData.ViewMat, &ConstBufferData.ViewMat);
	D3DXMatrixTranspose(&ConstBufferData.ProjMat, &ConstBufferData.ProjMat);
}

void ObjectMesh::CreateLightData()
{
	LightDir = { 1.0f,1.0f,1.0f,1.0f };
	LightPos = { 0.0f,0.0f,0.0f,0.0f };
	EyeDir = { 0.0f,0.0f,0.0f,0.0f };
	EyePos = { 0.0f,0.0f,0.0f,0.0f };
}

void ObjectMesh::SetMatrix(Matrix* world, Matrix* view, Matrix* proj)
{
	if (world != nullptr) {
		WorldMat = *world;
	}
	if (view != nullptr) {
		ViewMat = *view;
	}
	if (proj != nullptr) {
		ProjMat = *proj;
	}
	LightDir = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	UpdateConstBuffer();
}

void ObjectMesh::SetLightData(Vector4* lightDir, Vector4* lightPos, Vector4* eyeDir, Vector4* eyePos)
{
	if (lightDir != nullptr)
	{
		LightDir = *lightDir;
	}
	if (lightPos != nullptr)
	{
		LightPos = *lightPos;
	}
	if (eyeDir != nullptr)
	{
		EyeDir = *eyeDir;
	}
	if (eyePos != nullptr)
	{
		EyePos = *eyePos;
	}
	ConstBufferData.LightDir = LightDir;
	ConstBufferData.LightPos = LightPos;
	ConstBufferData.EyeDir = EyeDir;
	ConstBufferData.EyePos = EyePos;
	UpdateLightBuffer();
}

void ObjectMesh::UpdateConstBuffer()
{
	ConstBufferData.WorldMat = WorldMat.Transpose();
	ConstBufferData.ViewMat = ViewMat.Transpose();
	ConstBufferData.ProjMat = ProjMat.Transpose();
	ConstBufferData.WorldMatInverse = WorldMat.Invert();
	ConstBufferData.LightDir = LightDir;
	D3D11Context->UpdateSubresource(ConstantBuffer, 0, nullptr, &ConstBufferData, 0, 0);
}

void ObjectMesh::UpdateLightBuffer()
{
	ConstBufferData.LightDir = LightDir;
	ConstBufferData.LightPos = LightPos;
	ConstBufferData.EyeDir = EyeDir;
	ConstBufferData.EyePos = EyePos;
	D3D11Context->UpdateSubresource(ConstantBuffer, 0, nullptr, &ConstBufferData, 0, 0);
}

ID3D11Buffer* ObjectMesh::CreateSubBuffer(void* DataAddress, UINT VertexCount, UINT VertexSize)
{
	HRESULT hr;
	ID3D11Buffer* buffer = nullptr;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = VertexCount * VertexSize;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = DataAddress;
	hr = D3D11Device->CreateBuffer(&bd, &sd, &buffer);
	return buffer;
}

