#include "FbxEntity.h"
#include "../Manager/TextureManager.h"
#include "FbxObjMesh.h"
#include "FbxSceneLoader.h"
HRESULT FbxEntity::LoadTexture(std::wstring file_name)
{
	std::wstring path = L"../../fbx/";
	TextureFileName = path + file_name;
	HRESULT hr = S_OK;
	if (TextureFileList.size() > 1) 
	{
		SubTextureList.resize(TextureFileList.size());
		for (int Tex = 0; Tex < TextureFileList.size(); Tex++) 
		{
			TextureFileName = path + TextureFileList[Tex];
			SubTextureList[Tex] = TextureMgr.Load(file_name);
			if (SubTextureList[Tex] == nullptr)
			{
				hr = S_FALSE;
			}
		}
	}
	else 
	{
		TextureFile = TextureMgr.Load(file_name);
		if (TextureFile == nullptr)
		{

			hr = S_FALSE;
		}
		else {
			ObjMesh.TextureSRV = TextureFile->TextureSRV;
		}
	}
	return hr;
}

void FbxEntity::SetListSize()
{
	VertexList.resize(0);
	IndexList.resize(0);
	return;
}

void FbxEntity::SetVertexList()
{
	return;
}

void FbxEntity::SetIndexList()
{
	return;
}

void FbxEntity::SetMatrix(Matrix* world, Matrix* view, Matrix* proj)
{
	ObjMesh.SetMatrix(world, view, proj);
}

void FbxEntity::SetParent(FbxEntity* parent)
{
	parent->ObjectChildList.push_back(this);
	ParentObj = parent;
}

bool FbxEntity::CreateVertex()
{
	if (VertexList.size() == 0) 
	{
		SetVertexList();
	}
	InitVertexList = VertexList;

	SetIndexList();
	
	ObjMesh.SetSkinning(Skinned);
	ObjMesh.SetVertexList(VertexList);
	ObjMesh.SetIndexList(IndexList);
	ObjMesh.SetShader(ShaderFile);
	ObjMesh.SetTexture(TextureFile);
	ObjMesh.SetIWVertexList(IWVertexList);

	if (SubVertexList.size() > 0) 
	{
		ObjMesh.SetSubVertexList(SubVertexList);
	}
	if (SubTextureList.size() > 0) 
	{
		ObjMesh.SetSubTextureList(SubTextureList);
	}
	if (SubIWVertexList.size() > 0) 
	{
		ObjMesh.SetSubIWVertexList(SubIWVertexList);
	}
	if (ObjMesh.Create() == false) 
	{
		return false;
	}
	return true;
}

bool FbxEntity::Load(std::wstring ShaderFile)
{
	if (FAILED(ShaderCompile(ShaderFile))) 
	{
		return false;
	}
	if (FAILED(LoadTexture(TextureFileName))) 
	{
		return false;
	}
	return true;
}

bool FbxEntity::SetData(ID3D11Device* device, ID3D11DeviceContext* context, RECT clientRT)
{
	D3D11Device = device;
	D3D11Context = context;
	ClientRect = clientRT;
	ObjMesh.SetDevice(device, context);
	return true;
}

Matrix FbxEntity::Interplate(FbxSceneLoader* pAnimImp, float frame)
{
	Matrix Ani;
	AnimTrack A, B;
	AnimationScene Scene = pAnimImp-> AnimScene;
	int _start = max(Scene.StartFrame, frame);
	int _end = min(Scene.EndFrame, frame + 1);

	A = AniTrackList[_start];
	B = AniTrackList[_end];
	float s = frame - (float)_start;
	Vector3 pos;
	D3DXVec3Lerp(&pos, &A.Trans, &B.Trans, s);
	Vector3 scale;
	D3DXVec3Lerp(&scale, &A.Scale, &B.Scale, s);
	Quaternion rot;
	D3DXQuaternionSlerp(&rot, &A.Rotate, &B.Rotate, s);
	Matrix ScaleMat;
	D3DXMatrixScaling(&ScaleMat, scale.x, scale.y, scale.z);
	Matrix RotationMat;
	D3DXMatrixRotationQuaternion(&RotationMat, &rot);

	Ani = ScaleMat * RotationMat;
	Ani._41 = pos.x;
	Ani._42 = pos.y;
	Ani._43 = pos.z;
	return Ani;
}


bool FbxEntity::Init()
{
	return true;
}

bool FbxEntity::Frame()
{
	return true;
}

bool FbxEntity::Release()
{
	for (auto data : SubTextureList)
	{
		data->Release();
	}

	ObjMesh.Release();
	
	
	Object3D::Release();
	return true;
}

bool FbxEntity::Render()
{
	ObjMesh.Render();
	return true;
}

bool FbxEntity::PreRender()
{
	ObjMesh.PreRender();
	return true;
}

bool FbxEntity::PostRender()
{
	ObjMesh.PostRender();
	return true;
}
