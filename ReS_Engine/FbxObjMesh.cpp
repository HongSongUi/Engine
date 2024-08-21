#include "FbxObjMesh.h"
#include "ShaderManager.h"
#include "TextureManager.h"
HRESULT FbxObjMesh::CreateVertexBuffer()
{
	HRESULT hr;
	hr = S_OK;
	if (SubVertexList.size() > 0) 
	{
		SubVertexBuffer.resize(SubVertexList.size());
		for (int index = 0; index < SubVertexList.size(); index++)
		{
			if (SubVertexList[index].size() > 0) 
			{
				SubVertexBuffer[index] = ObjectMesh::CreateSubBuffer(&SubVertexList[index].at(0), SubVertexList[index].size(), sizeof(Vertex));
			}
		}
	}
	else 
	{
		ObjectMesh::CreateVertexBuffer();
	}
	if (SubIWVertexList.size() > 0) 
	{
		SubIWVertexBuffer.resize(SubIWVertexList.size());
		for (int index = 0; index < SubIWVertexList.size(); index++) 
		{
			if (SubIWVertexList[index].size() > 0) 
			{
				SubIWVertexBuffer[index] = ObjectMesh::CreateSubBuffer(&SubIWVertexList[index].at(0), SubIWVertexList[index].size(), sizeof(IW_VERTEX));
			}
		}
	}
	else 
	{
		IWVertexBuffer = ObjectMesh::CreateSubBuffer(&IWVertexList.at(0), IWVertexList.size(), sizeof(IW_VERTEX));
	}
	return hr;
}

HRESULT FbxObjMesh::CreateConstantBuffer()
{
	ObjectMesh::CreateConstantBuffer();
	HRESULT hr;
	for (int bone = 0; bone < 255; bone++) 
	{
		D3DXMatrixIdentity(&BoneData.BoneMat[bone]);
	}
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(VS_CONSTANT_BONE_BUFFER) * 1;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &BoneData;
	hr = D3D11Device->CreateBuffer(&bd, &sd, &SkinBoneCB);

	return hr;
}

HRESULT FbxObjMesh::CreateInputLayout()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0,40,D3D11_INPUT_PER_VERTEX_DATA, 0},


		{"INDEX",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"WEIGHT",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,16,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT size = sizeof(ied) / sizeof(ied[0]);
	hr = D3D11Device->CreateInputLayout(ied, size, ShaderData->VsCode->GetBufferPointer(), ShaderData->VsCode->GetBufferSize(), &InputLayout);
	return hr;
}

bool FbxObjMesh::PostRender()
{
	UINT strides[2] = { sizeof(Vertex) ,sizeof(IW_VERTEX) };
	UINT offsets[2] = { 0, };
	if (IsSkinning)
	{
		D3D11Context->VSSetConstantBuffers(1, 1, &SkinBoneCB);
	}
	if (IndexBuffer == nullptr)
	{
		if (SubVertexList.size() > 0)
		{
			for (int Sub = 0; Sub < SubVertexBuffer.size(); Sub++) 
			{
				if (SubVertexList[Sub].size() <= 0) 
				{
					continue;
				}
				ID3D11Buffer* buffer[2] = { SubVertexBuffer[Sub], SubIWVertexBuffer[Sub] };
				D3D11Context->IASetVertexBuffers(0, 2, buffer, strides, offsets);
				if (SubTextureList[Sub] != nullptr) 
				{
					D3D11Context->PSSetShaderResources(0, 1, &SubTextureList[Sub]->TextureSRV);
				}
				D3D11Context->Draw(SubVertexList[Sub].size(), 0);
			}
		}
		else 
		{
			ID3D11Buffer* buffer[2] = { VertexBuffer, IWVertexBuffer };
			D3D11Context->IASetVertexBuffers(0, 2, buffer, strides, offsets);

			D3D11Context->Draw(VertexList.size(), 0);
		}
	}
	else
	{
		D3D11Context->DrawIndexed(IndexList.size(), 0, 0);
	}
    return true;
}

bool FbxObjMesh::Release()
{
	for (int sub = 0; sub < SubVertexBuffer.size(); sub++)
	{
		if (SubVertexBuffer[sub] != nullptr) 
		{
			SubVertexBuffer[sub]->Release();
			SubVertexBuffer[sub] = nullptr;
		}
	}
	SubVertexBuffer.clear();
	for (int sub = 0; sub < SubIWVertexBuffer.size(); sub++) 
	{
		if (SubIWVertexBuffer[sub] != nullptr)
		{
			SubIWVertexBuffer[sub]->Release();
			SubIWVertexBuffer[sub] = nullptr;
		}
	}
	SubIWVertexBuffer.clear();
	if (SkinBoneCB) SkinBoneCB->Release();
	if (IWVertexBuffer) IWVertexBuffer->Release();

	SkinBoneCB = nullptr;
	IWVertexBuffer = nullptr;

	ObjectMesh::Release();

    return true;
}

void FbxObjMesh::SetSubVertexList(std::vector<std::vector<Vertex>>& subVertex)
{
	SubVertexList = subVertex;
}

void FbxObjMesh::SetSubTextureList(std::vector<Texture*>& subTexture)
{
	SubTextureList = subTexture;
}

void FbxObjMesh::SetSubIWVertexList(std::vector<SubIWVertex>& subIWVertex)
{
	SubIWVertexList = subIWVertex;
}

void FbxObjMesh::SetIWVertexList(std::vector<IW_VERTEX>& IwVertex)
{
	IWVertexList = IwVertex;
}

void FbxObjMesh::SetSkinning(bool skinnig)
{
	IsSkinning = skinnig;
}
