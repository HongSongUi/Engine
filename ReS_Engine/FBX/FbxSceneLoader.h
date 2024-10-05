#pragma once
#include <Windows.h>
#include <vector>
#include <map>
#include "FbxEntity.h"
#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")
struct AnimationScene
{
	UINT StartFrame;
	UINT EndFrame;
	float TickPerFrame;
	float FrameSpeed;
};

class FbxSceneLoader
{
public:
	AnimationScene AnimScene;
	float AnimFrame = 0.0f;
	float AnimInverse = 1.0f;
	float AnimSpeed = 1.0f;

	FbxManager* Manager;
	FbxImporter* Importer;
	FbxScene* Scene;
	FbxNode* RootNode;

	std::vector<FbxMesh*> MeshList;
	std::vector<FbxEntity*> DrawObjList;
	std::vector<FbxEntity*> ObjList;
	std::map<FbxMesh*, FbxEntity*> MeshMap;
	std::map<FbxNode*, FbxEntity*> ObjMap;
	std::map<FbxNode*, UINT> ObjectIDMap;
	std::map<FbxEntity*, std::wstring> ObjectNameMap;
	std::wstring TextureFileName;
	std::wstring DefaultTextureName;

	VS_CONSTANT_BONE_BUFFER BoneData;
	ID3D11Buffer* BoneCB;
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
public:
	HRESULT CreateConstantBuffer(ID3D11Device* device);
	bool Load(std::wstring FileName);
	bool Load(ID3D11Device* device, std::wstring FileName);
	void PreProcess(FbxNode* Node);
	void ParseMesh(FbxMesh* Mesh, FbxEntity* obj);
	FbxColor ReadColor(FbxMesh* Mesh, FbxLayerElementVertexColor* VertexColor, int posIndex, int colorIndex);
	FbxVector2 ReadTextureCoord(FbxMesh* Mesh, FbxLayerElementUV* UVSet, int vertexIndex, int uvIndex);
	FbxVector4 ReadNormal(FbxMesh* Mesh, FbxLayerElementNormal* VertexNor, int posIndex, int colorIndex);
	void LoadTexture(FbxMesh* Mesh, int& count, FbxEntity* Obj);
	Matrix ConvertMatrix(FbxAMatrix& fbxMatrix);
	Matrix DxConvertMatrix(FbxAMatrix& fbxMatrix);
	int GetSubMaterialIndex(int Poly, FbxLayerElementMaterial* MaterialList);
	void FindMashObjcet();
	void LoadAnimation();
	bool ParseMeshSkinning(FbxMesh* mesh, FbxEntity* Obj);
	bool UpdateFrame(ID3D11DeviceContext* context);
	void UpdateSkeleton(ID3D11DeviceContext* context, float time, VS_CONSTANT_BONE_BUFFER& data);
	void UpdateSkinning(ID3D11DeviceContext* context, VS_CONSTANT_BONE_BUFFER& InputData, std::vector<VS_CONSTANT_BONE_BUFFER>& OutputData);
};

