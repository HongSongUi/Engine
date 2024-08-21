#pragma once
#include <vector>
#include <d3dcompiler.h>
#include <d3d11.h>
#include "MathLib.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

struct Vertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector4 Color;
	Vector2 Texture;
	
	Vertex() {};
	Vertex(Vector3 pos, Vector4 col, Vector2 tex)
	{
		Position = pos;
		Color = col;
		Texture = tex;
	}
	Vertex(Vector3 pos, Vector3 nor, Vector4 col, Vector2 tex) {
		Position = pos;
		Normal = nor;
		Color = col;
		Texture = tex;
	}
};
struct Constant_Buffer 
{
	Matrix WorldMat;
	Matrix ViewMat;
	Matrix ProjMat;
	Matrix WorldMatInverse;

	Vector4 LightDir;
	Vector4 LightPos;
	Vector4 EyeDir;
	Vector4 EyePos;

	Vector4 mapData;
};
struct IW_VERTEX 
{
	Vector4 Index;
	Vector4 weight;
	IW_VERTEX() {}
	IW_VERTEX(Vector4 i, Vector4 w) {
		Index = i;
		weight = w;
	}
};

struct VS_CONSTANT_BONE_BUFFER 
{
	Matrix BoneMat[255];
};
class ObjectMesh
{
protected:
	std::vector<Vertex> VertexList;
	std::vector<unsigned int> IndexList;
	ID3D11Device* D3D11Device;
	ID3D11DeviceContext* D3D11Context;
public:
	ID3D11ShaderResourceView* TextureSRV = nullptr;
	Matrix WorldMat;
	Matrix ViewMat;
	Matrix ProjMat;

	Vector4 LightDir;
	Vector4 LightPos;
	Vector4 EyeDir;
	Vector4 EyePos;
public:
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	ID3D11Buffer* ConstantBuffer;
	Constant_Buffer ConstBufferData;
	ID3D11InputLayout* InputLayout;
	class Shader* ShaderData;
	class Texture* TextureData;
public:
	virtual HRESULT CreateVertexBuffer();
	virtual HRESULT CreateIndexBuffer();
	virtual HRESULT CreateInputLayout();
	virtual HRESULT CreateConstantBuffer();
public:
	void SetDevice(ID3D11Device* device, ID3D11DeviceContext* context);
	bool Create();
	void UpdateVertexBuffer();
	void SetShader(class Shader* shader);
	void SetTexture(class Texture* texture);
	void SetVertexList(std::vector<Vertex>& list);
	void SetIndexList(std::vector<unsigned int>& list);
	void CreateConstantData();
	void CreateLightData();
	void SetMatrix(Matrix* world, Matrix* view, Matrix* proj);
	void SetLightData(Vector4* lightDir, Vector4* lightPos, Vector4* eyeDir, Vector4* eyePos);
	void UpdateConstBuffer();
	void UpdateLightBuffer();
	ID3D11Buffer* CreateSubBuffer(void* DataAddress, UINT VertexCount, UINT VertexSize);
public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();
	virtual bool PreRender();
	virtual bool PostRender();
};

