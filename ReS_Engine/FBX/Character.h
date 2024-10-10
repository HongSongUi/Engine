#pragma once
#include "../Object/Object3D.h"
#include "FbxSceneLoader.h"



struct ActionTable 
{
	//struct with animation information
	UINT StartFrame;
	UINT EndFrame;
	float TickPerFrame;
	float FrameSpeed;
	bool Loop;
	ActionTable() 
	{
		Loop = false;
	}
};

class Character : public Object3D
{
public:
	UINT FbxListID; //Manage fbx files Use id
	FbxSceneLoader* FbxFile = nullptr;
	std::basic_string<wchar_t> ObjName;

	Vector3 UpdateScaleVec = { 1.0f ,1.0f ,1.0f }; //When converting the scale of fbx
	Vector3 UpdateRotationVal = { 0.0f ,0.0f,0.0f }; // When converting the Rotation of fbx

	Matrix UpdateScaleMat; //When converting the scale of fbx
	Matrix UpdateRotationMat; // When converting the Rotation of fbx

	Matrix TransposMat; 
	Matrix ScaleMat;

	Matrix RotationMat;
	Quaternion Quater;
public:

	Vector3 MinPos = { 0.0f,0.0f,0.0f };
	Vector3 MaxPos = { 0.0f,0.0f,0.0f };
public:
	ActionTable ActionCurrent;
	AnimationScene AnimScene;
	float AniFrame = 0;
	UINT StartFrame;
	UINT EndFrame;
	float AniInverse = 1.0f;
	float AniSpeed = 1.0f;

	FbxSceneLoader* AnionFbxFile = nullptr;
	std::map<std::wstring, FbxSceneLoader*> ActionFileList; 
	std::map<std::wstring, ActionTable> ActionList;

	VS_CONSTANT_BONE_BUFFER BoneData;
	std::vector<VS_CONSTANT_BONE_BUFFER> DrawGeom;

	ID3D11Buffer* AniBoneBuffer;
	std::vector<ID3D11Buffer*> SkinBoneBuffer;
public:
	void SetDevice(ID3D11Device* device, ID3D11DeviceContext* context);
	HRESULT CreateConstantBuffer();
	bool UpdateFrame();
	void SetMatrix(Matrix* world, Matrix* view, Matrix* proj);
	bool Render();
	bool Release();
	void SetCharacter(FbxSceneLoader* fbxFile, Vector3 pos);
	void SetTransform(Vector3* pos, Vector3* scale, Vector3* rot);
	void SetObjBox();
	void UpdatePosition(Vector3& pos) override;
	void UpdateScale(Vector3& scale) override;
	void UpdateRotation(Vector3& rot);
};

