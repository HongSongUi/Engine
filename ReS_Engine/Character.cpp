#include "Character.h"
#include "FbxObjMesh.h"
void Character::SetDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	D3D11Device = device;
	D3D11Context = context;
}

HRESULT Character::CreateConstantBuffer()
{
	int size = FbxFile->DrawObjList.size();
	HRESULT hr;
	for (int index = 0; index < 255; index++) 
	{
		D3DXMatrixIdentity(&BoneData.BoneMat[index]);
	}
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(VS_CONSTANT_BONE_BUFFER) * 1;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &BoneData;
	hr = D3D11Device->CreateBuffer(&bd, &sd, &AniBoneBuffer);
	SkinBoneBuffer.resize(size);
	DrawGeom.resize(size);

	for (int mesh = 0; mesh < size; mesh++) 
	{
		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = &BoneData;
		hr = D3D11Device->CreateBuffer(&bd, &sd, &SkinBoneBuffer[mesh]);
	}
	return hr;
}

bool Character::UpdateFrame()
{
	std::vector<Matrix> matCurrentAnimList;
	AniFrame = AniFrame + gSecondPerFrame * AniSpeed * AnimScene.FrameSpeed;
	if (AniFrame > ActionCurrent.EndFrame ||
		AniFrame < ActionCurrent.StartFrame)
	{
		AniFrame = min(AniFrame, ActionCurrent.EndFrame);
		AniFrame = max(AniFrame, ActionCurrent.StartFrame);
	}
	if (ActionCurrent.Loop) {
		if (AniFrame >= ActionCurrent.EndFrame) {
			AniFrame = ActionCurrent.StartFrame;
		}
	}
	else {
		if (AniFrame >= ActionCurrent.EndFrame) {
			AniFrame = ActionCurrent.StartFrame;
		}
	}
	if (AnionFbxFile) {

		for (int Bone = 0; Bone < AnionFbxFile->ObjList.size(); Bone++) {
			Matrix AniMat = AnionFbxFile->ObjList[Bone]->Interplate(FbxFile, AniFrame);
			D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &AniMat);
			matCurrentAnimList.push_back(AniMat);
		}
		D3D11Context->UpdateSubresource(FbxFile->BoneCB, 0, nullptr, &BoneData, 0, 0);
		for (int Draw = 0; Draw < FbxFile->DrawObjList.size(); Draw++) 
		{

			if (FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.size()) 
			{

				for (int Bone = 0; Bone < FbxFile->ObjList.size(); Bone++) {
					std::wstring name = FbxFile->ObjList[Bone]->Name;
					auto iter = FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.find(name);
					if (iter != FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.end())
					{
						Matrix matBind = iter->second;
						Matrix matAni = matBind * matCurrentAnimList[Bone];
						D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &matAni);
					}
				}
				D3D11Context->UpdateSubresource(FbxFile->DrawObjList[Draw]->ObjMesh.SkinBoneCB, 0, nullptr, &BoneData, 0, 0);
			}
		}
	}
	//	_AnionFbxFile->UpdateSkeleton(_Context, _AniFrame, _BoneData);
	//	_FbxFile->UpdateSkinning(_Context, _BoneData, _DrawGeom);
	//}
	else {
		//	_FbxFile->UpdateSkeleton(_Context, _AniFrame, _BoneData);
		//	_FbxFile->UpdateSkinning(_Context, _BoneData, _DrawGeom);
		for (int Bone = 0; Bone < FbxFile->ObjList.size(); Bone++) {
			Matrix AniMat = FbxFile->ObjList[Bone]->Interplate(FbxFile, AniFrame);
			D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &AniMat);
			matCurrentAnimList.push_back(AniMat);
		}
		D3D11Context->UpdateSubresource(FbxFile->BoneCB, 0, nullptr, &BoneData, 0, 0);
		for (int Draw = 0; Draw < FbxFile->DrawObjList.size(); Draw++) {

			if (FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.size()) {

				for (int Bone = 0; Bone < FbxFile->ObjList.size(); Bone++) {
					std::wstring name = FbxFile->ObjList[Bone]->Name;
					auto iter = FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.find(name);
					if (iter != FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.end()) 
					{
						Matrix matBind = iter->second;
						Matrix matAni = matBind * matCurrentAnimList[Bone];
						D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &matAni);
					}
				}
				D3D11Context->UpdateSubresource(FbxFile->DrawObjList[Draw]->ObjMesh.SkinBoneCB, 0, nullptr, &BoneData, 0, 0);
			}
		}
	}
	/*for (int bone = 0; bone < _SkinBoneBuffer.size(); bone++) {

		_Context->UpdateSubresource(_SkinBoneBuffer[bone], 0, nullptr, &_DrawGeom[bone], 0, 0);
	}
	for (int bone = 0; bone < _FbxFile->_ObjList.size(); bone++) {
		D3DXMatrixTranspose(&_BoneData._BoneMat[bone], &_BoneData._BoneMat[bone]);
	}
	_Context->UpdateSubresource(_AniBoneBuffer, 0, nullptr, &_BoneData, 0, 0);*/

	return true;
}

void Character::SetMatrix(Matrix* world, Matrix* view, Matrix* proj)
{
	if (world)
	{
		WorldMat = *world;
	}
	if (view)
	{
		ViewMat = *view;
	}
	if (proj) {
		ProjMat = *proj;
	}
}

bool Character::Render()
{
	D3D11Context->VSSetConstantBuffers(1, 1, &AniBoneBuffer);
	for (int mesh = 0; mesh < FbxFile->DrawObjList.size(); mesh++) 
	{
		if (FbxFile->DrawObjList[mesh]->Skinned) 
		{
			D3D11Context->VSSetConstantBuffers(1, 1, &SkinBoneBuffer[mesh]);
		}
		FbxFile->DrawObjList[mesh]->SetMatrix(&WorldMat, &ViewMat, &ProjMat);
		FbxFile->DrawObjList[mesh]->Render();
	}
	return true;
}

bool Character::Release()
{
	if (AniBoneBuffer) AniBoneBuffer->Release();
	for (auto bone : SkinBoneBuffer) 
	{
		bone->Release();
		bone = nullptr;
	}
	return true;
}

void Character::SetCharacter(FbxSceneLoader* fbxFile, Vector3 pos)
{
	FbxFile = fbxFile;

	Position = pos;

	Scale = { 0.05f,0.05f,0.05f };
	CreateConstantBuffer();
	SetTransform(&Position, &Scale, nullptr);

	SetObjBox();
}

void Character::SetTransform(Vector3* pos, Vector3* scale, Vector3* rot)
{
	if (pos != nullptr)
	{
		Position = *pos;
	}
	if (scale != nullptr)
	{
		Scale = *scale;
	}
	if (rot != nullptr)
	{
		Rotation = *rot;
	}


	TransposMat = TransposMat.CreateTranslation(Position);
	ScaleMat = ScaleMat.CreateScale(Scale);
	RotationMat = RotationMat.CreateFromYawPitchRoll(Rotation.y, Rotation.x, Rotation.z);


	UpdateScaleMat = UpdateScaleMat.CreateScale(UpdateScaleVec);
	UpdateScaleMat = ScaleMat * UpdateScaleMat;

	Quater = Quater.CreateFromYawPitchRoll(UpdateRotationVal.y, UpdateRotationVal.x, UpdateRotationVal.z);
	UpdateRotationMat = UpdateRotationMat.CreateFromQuaternion(Quater);

	UpdateRotationMat = RotationMat * UpdateRotationMat;

	WorldMat = ScaleMat * RotationMat * TransposMat;
}

void Character::SetObjBox()
{
	for (int mesh = 0; mesh < FbxFile->DrawObjList.size(); mesh++)
	{
		std::vector<Vertex>& drawObjVertex = FbxFile->DrawObjList[mesh]->VertexList;

		int vertexSize = drawObjVertex.size();

		for (int vertex = 0; vertex < vertexSize; vertex++)
		{
			if (MinPos.x >= drawObjVertex[vertex].Position.x)
			{
				MinPos.x = drawObjVertex[vertex].Position.x;
			}
			if (MinPos.y >= drawObjVertex[vertex].Position.y)
			{
				MinPos.y = drawObjVertex[vertex].Position.y;
			}
			if (MinPos.z >= drawObjVertex[vertex].Position.z)
			{
				MinPos.z = drawObjVertex[vertex].Position.z;
			}

			if (MaxPos.x <= drawObjVertex[vertex].Position.x)
			{
				MaxPos.x = drawObjVertex[vertex].Position.x;
			}
			if (MaxPos.y <= drawObjVertex[vertex].Position.y)
			{
				MaxPos.y = drawObjVertex[vertex].Position.y;
			}
			if (MaxPos.z <= drawObjVertex[vertex].Position.x)
			{
				MaxPos.z = drawObjVertex[vertex].Position.z;
			}
		}
	}
	Vector3 size = { WorldMat._11, WorldMat._22, WorldMat._33 };



	D3DXVec3TransformCoord(&MaxPos, &MaxPos, &WorldMat);
	D3DXVec3TransformCoord(&MinPos, &MinPos, &WorldMat);

	ObjectBox.SetBox(MaxPos, MinPos);
	ObjectBox.SetParent(this);
}

void Character::UpdatePosition(Vector3& pos)
{
	Position = pos;

	TransposMat = TransposMat.CreateTranslation(Position);
	WorldMat._41 = Position.x;
	WorldMat._42 = Position.y;
	WorldMat._43 = Position.z;


	SetObjBox();
}

void Character::UpdateScale(Vector3& scale)
{
	UpdateScaleVec = scale;


	UpdateScaleMat = UpdateScaleMat.CreateScale(UpdateScaleVec);

	UpdateScaleMat = ScaleMat * UpdateScaleMat;
	WorldMat = UpdateScaleMat * UpdateRotationMat * TransposMat;

	SetObjBox();
}

void Character::UpdateRotation(Vector3& rot)
{
	UpdateRotationVal = rot;

	Quater = Quater.CreateFromYawPitchRoll(UpdateRotationVal.y, UpdateRotationVal.x, UpdateRotationVal.z);
	UpdateRotationMat = UpdateRotationMat.CreateFromQuaternion(Quater);

	UpdateRotationMat = RotationMat * UpdateRotationMat;

	WorldMat = (UpdateScaleMat * UpdateRotationMat * TransposMat);

	SetObjBox();
}
