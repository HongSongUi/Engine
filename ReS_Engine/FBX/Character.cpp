#include "Character.h"
#include "FbxObjMesh.h"
void Character::SetDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	//device, context variable setting
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
	//Vector to store the current animation matrix list
	std::vector<Matrix> matCurrentAnimList;
	AniFrame = AniFrame + gSecondPerFrame * AniSpeed * AnimScene.FrameSpeed;// Update the animation frame.

	// Check if the animation frame is within the valid range.
	if (AniFrame > ActionCurrent.EndFrame ||
		AniFrame < ActionCurrent.StartFrame)
	{
		AniFrame = min(AniFrame, ActionCurrent.EndFrame);
		AniFrame = max(AniFrame, ActionCurrent.StartFrame);
	}
	if (ActionCurrent.Loop)
	{
		// Reset the animation frame to the start frame if it reaches the end frame.
		if (AniFrame >= ActionCurrent.EndFrame) 
		{
			AniFrame = ActionCurrent.StartFrame;
		}
	}
	else 
	{
		if (AniFrame >= ActionCurrent.EndFrame) 
		{
			AniFrame = ActionCurrent.StartFrame;
		}
	}
	// Check if the FBX file has animation.
	if (AnionFbxFile) 
	{
		for (int Bone = 0; Bone < AnionFbxFile->ObjList.size(); Bone++) // Interpolate the animation matrix for each bone.
		{
			Matrix AniMat = AnionFbxFile->ObjList[Bone]->Interplate(FbxFile, AniFrame);
			// Store the interpolated animation matrix in the bone data.
			D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &AniMat);
			matCurrentAnimList.push_back(AniMat); // Add to the current animation list
		}
		D3D11Context->UpdateSubresource(FbxFile->BoneCB, 0, nullptr, &BoneData, 0, 0); // Update the subresource for the bone data.

		for (int Draw = 0; Draw < FbxFile->DrawObjList.size(); Draw++) // Check for bind pose map in each draw object.
		{
			if (FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.size()) 
			{
				// Combine the bind pose and animation matrices for each bone.
				for (int Bone = 0; Bone < FbxFile->ObjList.size(); Bone++) 
				{
					std::wstring name = FbxFile->ObjList[Bone]->Name;
					auto iter = FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.find(name);
					if (iter != FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.end())
					{
						Matrix matBind = iter->second; // Retrieve the bind pose matrix.
						Matrix matAni = matBind * matCurrentAnimList[Bone]; // Combine with the animation matrix.
						D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &matAni); // Store the result in bone data.
					}
				}
				D3D11Context->UpdateSubresource(FbxFile->DrawObjList[Draw]->ObjMesh.SkinBoneCB, 0, nullptr, &BoneData, 0, 0);
			}
		}
	}
	else  // Perform default animation update if the FBX file has no animation.
	{
		for (int Bone = 0; Bone < FbxFile->ObjList.size(); Bone++) 
		{
			Matrix AniMat = FbxFile->ObjList[Bone]->Interplate(FbxFile, AniFrame);
			D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &AniMat);
			matCurrentAnimList.push_back(AniMat);// Add to the current animation list.
		}
		D3D11Context->UpdateSubresource(FbxFile->BoneCB, 0, nullptr, &BoneData, 0, 0);
		for (int Draw = 0; Draw < FbxFile->DrawObjList.size(); Draw++) // Check for bind pose map in each draw object.
		{
			if (FbxFile->DrawObjList[Draw]->MatrixBindPoseMapName.size()) 
			{

				for (int Bone = 0; Bone < FbxFile->ObjList.size(); Bone++) 
				{
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
	D3D11Context->VSSetConstantBuffers(1, 1, &AniBoneBuffer);// Set the animation bone buffer for the vertex shader.
	for (int mesh = 0; mesh < FbxFile->DrawObjList.size(); mesh++) 
	{
		if (FbxFile->DrawObjList[mesh]->Skinned) // Check if the current drawable object is skinned.
		{
			D3D11Context->VSSetConstantBuffers(1, 1, &SkinBoneBuffer[mesh]);
		}
		FbxFile->DrawObjList[mesh]->SetMatrix(&WorldMat, &ViewMat, &ProjMat);// Set the world, view, and projection matrices for the current drawable object.
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
	FbxFile = fbxFile;  // Assign the provided FBX file to the class member.

	Position = pos; // Set the character's position in the 3D space.

	Scale = { 0.05f,0.05f,0.05f }; // Set the character's scale.
	CreateConstantBuffer(); // Create the constant buffer used for rendering.
	SetTransform(&Position, &Scale, nullptr); // Set the transformation matrices for the character based on position and scale.

	SetObjBox();    // Initialize the object's bounding box based on its current parameters.

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

	// Create a matrix
	TransposMat = TransposMat.CreateTranslation(Position);
	ScaleMat = ScaleMat.CreateScale(Scale);
	RotationMat = RotationMat.CreateFromYawPitchRoll(Rotation.y, Rotation.x, Rotation.z);

	// Create an update scale matrix based on the update scale vector.
	UpdateScaleMat = UpdateScaleMat.CreateScale(UpdateScaleVec); 
	UpdateScaleMat = ScaleMat * UpdateScaleMat;

	// Create a quaternion based on the updated rotation values.
	Quater = Quater.CreateFromYawPitchRoll(UpdateRotationVal.y, UpdateRotationVal.x, UpdateRotationVal.z);
	UpdateRotationMat = UpdateRotationMat.CreateFromQuaternion(Quater);

	UpdateRotationMat = RotationMat * UpdateRotationMat; // Create an update rotation matrix from the quaternion.

	WorldMat = ScaleMat * RotationMat * TransposMat;
}

void Character::SetObjBox()
{
	// Loop through each vertex to find the min and max positions.

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


	// Transform the Min and Max positions using the World matrix.
	D3DXVec3TransformCoord(&MaxPos, &MaxPos, &WorldMat);
	D3DXVec3TransformCoord(&MinPos, &MinPos, &WorldMat);

	ObjectBox.SetBox(MaxPos, MinPos);// Set the bounding box using the transformed max and min positions.
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
