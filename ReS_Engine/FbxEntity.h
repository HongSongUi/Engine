#pragma once
#include <fbxsdk.h>
#include "MathLib.h"
#include "Object3D.h"
#include <map>
#include "FbxObjMesh.h"
struct BoneWeight
{
	std::vector<int> Index;
	std::vector<float> Weight;
	void Insert(int Bone, float weight) 
	{
		for (int i = 0; i < Index.size(); i++)
		{
			if (weight > Weight[i]) 
			{
				for (int j = Index.size() - 1; j > i; --j)
				{
					Index[j] = Index[j - 1];
					Weight[j] = Weight[j - 1];
				}
				Index[i] = Bone;
				Weight[i] = weight;
				break;
			}
		}
	}
	BoneWeight()
	{
		Index.resize(8);
		Weight.resize(8);
	}
};
struct AnimTrack
{
	UINT Frame;
	Matrix AniMat;//self * parent
	Matrix MyAniMat;//animat * parent(inv)
	Vector3 Trans;
	Quaternion Rotate;
	Vector3 Scale;
};

class FbxEntity : public Object3D
{
public:
	using SubVertex = std::vector<Vertex>;
	using SubIWVertex = std::vector<IW_VERTEX>;
	UINT ObjectBone;
	bool Skinned;
	std::vector<BoneWeight> WeightList;
	FbxObjMesh ObjMesh;
	FbxNode* Node = nullptr;
	FbxNode* ParentNode = nullptr;
	FbxEntity* ParentObj = nullptr;
	FbxAMatrix LocalMatrix;

	Matrix AnimMatrix;
	Matrix ControlMatrix;

	std::wstring TextureFileName;
	std::wstring Name;

	std::vector<FbxEntity*> ObjectChildList;
	std::vector<class Texture*> SubTextureList;
	std::vector<AnimTrack> AniTrackList;
	std::vector<IW_VERTEX> IWVertexList;
	std::vector<SubIWVertex> SubIWVertexList;
	std::vector<SubVertex> SubVertexList;
	std::vector<std::wstring> TextureFileList;

	std::map<UINT, Matrix> MatrixBindPoseMap;
	std::map<std::wstring, Matrix> MatrixBindPoseMapName;
public:
	HRESULT LoadTexture(std::wstring file_name);
	void SetListSize()override;
	void SetVertexList()override;
	void SetIndexList()override;
	void SetMatrix(Matrix* world, Matrix* view, Matrix* proj)override;
	void SetParent(FbxEntity* Parent);
	bool CreateVertex()override;
	bool Load(std::wstring ShaderFile);
	bool SetData(ID3D11Device* device, ID3D11DeviceContext* context, RECT clientRT)override;
	Matrix Interplate(class FbxSceneLoader* pAnimImp, float frame);
public:
	bool Init();
	bool Frame();
	bool Release()override;
	bool Render()override;
	bool PreRender()override;
	bool PostRender()override;
public:
	FbxEntity()
	{
		SetListSize();
	}
};

