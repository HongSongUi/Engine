#include "FbxSceneLoader.h"
#include "FbxObjMesh.h"
#include "../Utility/ReSUtility.h"
#include "../Manager/TextureManager.h"

bool FbxSceneLoader::Init()
{
	// Create Fbx Scene, Manager, Importer
	Manager = FbxManager::Create();
	Importer = FbxImporter::Create(Manager, "");
	Scene = FbxScene::Create(Manager, "");

	return true;
}

bool FbxSceneLoader::Frame()
{
	return true;
}

bool FbxSceneLoader::Render()
{
	return true;
}

bool FbxSceneLoader::Release()
{
	// Resource Release
	if (BoneCB)
	{
		BoneCB->Release();
		BoneCB = nullptr;
	}

	for (auto obj : ObjList)
	{
		obj->Release();
		delete obj;
	}
	ObjList.clear();

	if (Scene) {
		Scene->Destroy();
	}
	if (Importer) {
		Importer->Destroy();
	}
	if (Manager) {
		Manager->Destroy();
	}
	Scene = nullptr;
	Importer = nullptr;
	Manager = nullptr;

	return true;
}

HRESULT FbxSceneLoader::CreateConstantBuffer(ID3D11Device* device)
{
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
	hr = device->CreateBuffer(&bd, &sd, &BoneCB);
	return hr;
}

bool FbxSceneLoader::Load(std::wstring FileName)
{
	// Load the fbx use File name
	std::string name = WstrToStr(FileName);
	bool ret = Importer->Initialize(name.c_str());
	if (ret == false) 
	{
		Scene->Destroy();
		Importer->Destroy();
		Manager->Destroy();
		Scene = nullptr;
		Importer = nullptr;
		Manager = nullptr;
		return false;
	}
	ret = Importer->Import(Scene);
	if (ret == false) 
	{
		Scene->Destroy();
		Importer->Destroy();
		Manager->Destroy();
		Scene = nullptr;
		Importer = nullptr;
		Manager = nullptr;
		return false;
	}
	FbxAxisSystem SceneAxisSystem = Scene->GetGlobalSettings().GetAxisSystem();
	FbxSystemUnit::m.ConvertScene(Scene);// Convert the scene's unit system to a standard unit
	FbxAxisSystem::MayaZUp.ConvertScene(Scene);// Convert the scene's coordinate system to match Maya's Z-up convention. 

	RootNode = Scene->GetRootNode();
	PreProcess(RootNode);
	LoadAnimation();

	for (auto Obj : ObjList) {
		if (Obj->ParentNode != nullptr) 
		{
			auto data = ObjMap.find(Obj->ParentNode);
			Obj->SetParent(data->second);
		}
	}
	FindMashObjcet();

	for (auto mesh : MeshList) 
	{
		FbxEntity* obj = MeshMap.find(mesh)->second;
		ParseMesh(mesh, obj);
	}
	return true;
}

bool FbxSceneLoader::Load(ID3D11Device* device, std::wstring FileName)
{
	Init();
	if (!Load(FileName))
	{
		return false;
	}
	if (FAILED(CreateConstantBuffer(device)))
	{
		return false;
	}
	return true;
}

void FbxSceneLoader::PreProcess(FbxNode* Node)
{
	if (Node && (Node->GetCamera() || Node->GetLight())) // Check if the node is valid and if it is a camera or light node
	{
		return;
	}
	FbxEntity* obj = new FbxEntity;

	std::string name = Node->GetName();

	obj->Name = StrToWstr(name);
	obj->Node = Node;
	obj->ParentNode = Node->GetParent();
	obj->ObjectBone = ObjList.size();
	ObjList.push_back(obj);
	ObjMap.insert(std::make_pair(Node, obj));
	ObjectIDMap.insert(std::make_pair(Node, obj->ObjectBone));
	ObjectNameMap.insert(std::make_pair(obj, obj->Name));

	int Child = Node->GetChildCount();
	for (int i = 0; i < Child; i++) 
	{
		FbxNode* ChildNode = Node->GetChild(i);

		FbxNodeAttribute::EType type = ChildNode->GetNodeAttribute()->GetAttributeType();

		// Check if the child node is of type Mesh, Skeleton, or Null
		if ((type == FbxNodeAttribute::eMesh ||
			type == FbxNodeAttribute::eSkeleton ||
			type == FbxNodeAttribute::eNull))
		{
			PreProcess(ChildNode); // Recursively process the child node
		}
	}
}

void FbxSceneLoader::ParseMesh(FbxMesh* Mesh, FbxEntity* obj)
{
	obj->Skinned = ParseMeshSkinning(Mesh, obj);
	FbxNode* Node = Mesh->GetNode();
	FbxAMatrix geom;
	FbxVector4 trans = Node->GetGeometricTranslation(FbxNode::eSourcePivot);

	FbxVector4 rot = Node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 scale = Node->GetGeometricScaling(FbxNode::eSourcePivot);
	geom.SetT(trans);
	geom.SetR(rot);
	geom.SetS(scale);
	FbxAMatrix normalLocalMatrix = geom;
	//normal 행렬은 역행렬의 전치
	normalLocalMatrix = normalLocalMatrix.Inverse();
	normalLocalMatrix = normalLocalMatrix.Transpose();

	FbxLayerElementUV* VertexUVSet = nullptr;
	FbxLayerElementVertexColor* VertexColor = nullptr;
	FbxLayerElementNormal* VertexNormalSet = nullptr;
	FbxLayerElementMaterial* Material = nullptr;
	int LayerCount = Mesh->GetLayerCount();
	FbxLayer* Layer = Mesh->GetLayer(0);

	if (Layer->GetUVs() != nullptr) 
	{
		VertexUVSet = Layer->GetUVs();
	}
	if (Layer->GetNormals() != nullptr) 
	{
		VertexNormalSet = Layer->GetNormals();
	}
	if (Layer->GetVertexColors() != nullptr) 
	{
		VertexColor = Layer->GetVertexColors();
	}
	if (Layer->GetMaterials() != nullptr)
	{
		Material = Layer->GetMaterials();
	}
	int MtrlCount = 0;

	LoadTexture(Mesh, MtrlCount, obj);

	int PolyCount = Mesh->GetPolygonCount();

	int FaceCount = 0;
	int BasePolyIndex = 0;
	int SubMtrl = 0;
	FbxVector4* VertexPos = Mesh->GetControlPoints();

	for (int poly = 0; poly < PolyCount; poly++) 
	{
		int PolySize = Mesh->GetPolygonSize(poly);	
		FaceCount = PolySize - 2;					
	
		if (Material) {
			SubMtrl = GetSubMaterialIndex(poly, Material);
		}
		for (int face = 0; face < FaceCount; face++) 
		{

			int VertexColorArray[3] = { 0, face + 2, face + 1 };

			int CornerIndex[3];
			CornerIndex[0] = Mesh->GetPolygonVertex(poly, 0);
			CornerIndex[1] = Mesh->GetPolygonVertex(poly, face + 2);
			CornerIndex[2] = Mesh->GetPolygonVertex(poly, face + 1);

			int iUVIndex[3];
			iUVIndex[0] = Mesh->GetTextureUVIndex(poly, 0);
			iUVIndex[1] = Mesh->GetTextureUVIndex(poly, face + 2);
			iUVIndex[2] = Mesh->GetTextureUVIndex(poly, face + 1);
			for (int index = 0; index < 3; index++) 
			{
				int vertexID = CornerIndex[index];

				FbxVector4 ver = VertexPos[vertexID];

				Vertex tmp;
				IW_VERTEX IwVertex;
				FbxVector4 vTmp = geom.MultT(ver);
				tmp.Position.x = vTmp.mData[0];
				tmp.Position.y = vTmp.mData[2];
				tmp.Position.z = vTmp.mData[1];
				tmp.Color = { 1,1,1,1 };
				if (VertexColor) 
				{
					FbxColor F_Color = ReadColor(Mesh, VertexColor, CornerIndex[index], BasePolyIndex + VertexColorArray[index]);
					tmp.Color.x = F_Color.mRed;
					tmp.Color.y = F_Color.mGreen;
					tmp.Color.z = F_Color.mBlue;
					tmp.Color.w = 1.0f;
				}
				if (VertexUVSet)
				{
					FbxVector2 t = ReadTextureCoord(Mesh, VertexUVSet, CornerIndex[index], iUVIndex[index]);
					tmp.Texture.x = t.mData[0];
					tmp.Texture.y = 1.0f - t.mData[1];
				}
				if (VertexNormalSet)
				{
					FbxVector4 n = ReadNormal(Mesh, VertexNormalSet, CornerIndex[index], BasePolyIndex + VertexColorArray[index]);
					n = normalLocalMatrix.MultT(n);
					tmp.Normal.x = n.mData[0];
					tmp.Normal.y = n.mData[1];
					tmp.Normal.z = n.mData[2];
				}
				if (obj->Skinned == false) {
					IwVertex.Index.x = ObjectIDMap.find(Node)->second;
					IwVertex.Index.y = 0;
					IwVertex.Index.z = 0;
					IwVertex.Index.w = 0;
					IwVertex.weight.x = 1.0f;
					IwVertex.weight.y = 0.0f;
					IwVertex.weight.z = 0.0f;
					IwVertex.weight.w = 0.0f;
				}
				else {
					BoneWeight* weight = &obj->WeightList[vertexID];
					IwVertex.Index.x = weight->Index[0];
					IwVertex.Index.y = weight->Index[1];
					IwVertex.Index.z = weight->Index[2];
					IwVertex.Index.w = weight->Index[3];
					IwVertex.weight.x = weight->Weight[0];
					IwVertex.weight.y = weight->Weight[1];
					IwVertex.weight.z = weight->Weight[2];
					IwVertex.weight.w = weight->Weight[3];
				}
				if (MtrlCount <= 1) {
					obj->VertexList.push_back(tmp);
					obj->IWVertexList.push_back(IwVertex);
				}
				else {
					obj->SubVertexList[SubMtrl].push_back(tmp);
					obj->SubIWVertexList[SubMtrl].push_back(IwVertex);
				}
			}
		}
		BasePolyIndex += PolySize;
	}
	DrawObjList.push_back(obj);
}

FbxColor FbxSceneLoader::ReadColor(FbxMesh* Mesh, FbxLayerElementVertexColor* VertexColor, int posIndex, int colorIndex)
{
	FbxColor color = { 1,1,1,1 };
	FbxLayerElement::EMappingMode mode = VertexColor->GetMappingMode();

	switch (mode)
	{
	case FbxLayerElementUV::eByControlPoint:
		switch (VertexColor->GetReferenceMode()) 
		{
		case FbxLayerElementUV::eDirect:
			color = VertexColor->GetDirectArray().GetAt(posIndex);
			break;

		case FbxLayerElementUV::eIndexToDirect:
			int index = VertexColor->GetIndexArray().GetAt(posIndex);
			color = VertexColor->GetDirectArray().GetAt(index);
			break;
		}
		break;
	case FbxLayerElementUV::eByPolygonVertex:
		switch (VertexColor->GetReferenceMode()) 
		{

		case FbxLayerElementUV::eDirect:
			color = VertexColor->GetDirectArray().GetAt(colorIndex);
			break;
		case FbxLayerElementUV::eIndexToDirect:
			int index = VertexColor->GetIndexArray().GetAt(colorIndex);
			color = VertexColor->GetDirectArray().GetAt(index);
			break;
		}
		break;
	}
	return color;
}

FbxVector2 FbxSceneLoader::ReadTextureCoord(FbxMesh* Mesh, FbxLayerElementUV* UVSet, int vertexIndex, int uvIndex)
{
	FbxVector2 fbx_vector;
	FbxLayerElement::EMappingMode mode = UVSet->GetMappingMode();
	switch (mode)
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		switch (UVSet->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		{
			fbx_vector = UVSet->GetDirectArray().GetAt(vertexIndex);
		}break;
		case FbxLayerElementUV::eIndexToDirect:
		{
			int index = UVSet->GetIndexArray().GetAt(vertexIndex);
			fbx_vector = UVSet->GetDirectArray().GetAt(index);
		}break;
		}break;
	} break;
	case FbxLayerElementUV::eByPolygonVertex:
	{
		switch (UVSet->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		case FbxLayerElementUV::eIndexToDirect:
		{
			fbx_vector = UVSet->GetDirectArray().GetAt(uvIndex);
		}break;
		}break;
	}break;
	}
	return fbx_vector;
}

FbxVector4 FbxSceneLoader::ReadNormal(FbxMesh* Mesh, FbxLayerElementNormal* VertexNor, int posIndex, int colorIndex)
{
	FbxVector4 nor(1, 1, 1, 1);
	FbxLayerElement::EMappingMode mode = VertexNor->GetMappingMode();
	switch (mode) {
	case FbxLayerElementUV::eByControlPoint:
		switch (VertexNor->GetReferenceMode()) {
		case FbxLayerElementUV::eDirect:
			nor = VertexNor->GetDirectArray().GetAt(posIndex);
			break;
		case FbxLayerElementUV::eIndexToDirect:
			int index = VertexNor->GetIndexArray().GetAt(posIndex);
			nor = VertexNor->GetDirectArray().GetAt(index);
			break;
		}
		break;
	case FbxLayerElementUV::eByPolygonVertex:
		switch (VertexNor->GetReferenceMode()) {
		case FbxLayerElementUV::eDirect:
			nor = VertexNor->GetDirectArray().GetAt(colorIndex);
			break;
		case FbxLayerElementUV::eIndexToDirect:
			int index = VertexNor->GetIndexArray().GetAt(colorIndex);
			nor = VertexNor->GetDirectArray().GetAt(index);
			break;
		}
		break;
	}
	return nor;
}

void FbxSceneLoader::LoadTexture(FbxMesh* Mesh, int& count, FbxEntity* Obj)
{
	FbxNode* node = Mesh->GetNode();
	int MaterialCount = node->GetMaterialCount();
	count = MaterialCount;

	std::wstring FileName;
	std::vector<std::wstring> TextureFullPath;
	Obj->TextureFileList.resize(MaterialCount);
	TextureFullPath.resize(MaterialCount);

	for (int i = 0; i < MaterialCount; i++) {
		FbxSurfaceMaterial* Surface = node->GetMaterial(i);
		if (Surface) {
			auto prop = Surface->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (prop.IsValid()) {
				FbxFileTexture* tex = prop.GetSrcObject<FbxFileTexture>(0);

				if (tex != nullptr) {
					FileName = StrToWstr(tex->GetFileName());
					TextureFileName = FileName;
					TextureFullPath[i] = TextureFileName;
				}
			}
		}
	}
	if (MaterialCount == 1) 
	{
		Obj->TextureFileName = TextureMgr.GetSplitName(TextureFileName);
	}
	else {
		Obj->SubVertexList.resize(MaterialCount);
		Obj->SubIWVertexList.resize(MaterialCount);
		Obj->TextureFileList.resize(MaterialCount);
		for (int tex = 0; tex < MaterialCount; tex++) 
		{
			Obj->TextureFileList[tex] = TextureMgr.GetSplitName(TextureFullPath[tex]);
		}
	}
}

Matrix FbxSceneLoader::ConvertMatrix(FbxAMatrix& fbxMatrix)
{
	Matrix mat;
	float* Array = (float*)(&mat);
	double* fbxArray = (double*)(&fbxMatrix);
	for (int i = 0; i < 16; i++)
	{
		Array[i] = fbxArray[i];
	}
	return mat;
}

Matrix FbxSceneLoader::DxConvertMatrix(FbxAMatrix& fbxMatrix)
{
	Matrix tmp = ConvertMatrix(fbxMatrix);
	Matrix mat;

	mat._11 = tmp._11; mat._12 = tmp._13; mat._13 = tmp._12;
	mat._21 = tmp._31; mat._22 = tmp._33; mat._23 = tmp._32;
	mat._31 = tmp._21; mat._32 = tmp._23; mat._33 = tmp._22;
	mat._41 = tmp._41; mat._42 = tmp._43; mat._43 = tmp._42;
	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;
	return mat;
}

int FbxSceneLoader::GetSubMaterialIndex(int Poly, FbxLayerElementMaterial* MaterialList)
{
	int SubMtrl = 0;

	switch (MaterialList->GetMappingMode())
	{
	case FbxLayerElement::eByPolygon:
		switch (MaterialList->GetReferenceMode())
		{
		case FbxLayerElement::eIndex:
			SubMtrl = Poly;
			break;
		case FbxLayerElement::eIndexToDirect:
			SubMtrl = MaterialList->GetIndexArray().GetAt(Poly);
			break;
		}
		break;
	}
	return SubMtrl;
}

void FbxSceneLoader::FindMashObjcet()
{
	for (auto obj : ObjList) 
	{
		FbxMesh* mesh = obj->Node->GetMesh();
		if (mesh) 
		{
			MeshMap.insert(std::make_pair(mesh, obj));
			MeshList.push_back(mesh);
		}
	}
}

void FbxSceneLoader::LoadAnimation()
{
	FbxTime::SetGlobalTimeMode(FbxTime::eFrames30);
	FbxAnimStack* stack = Scene->GetSrcObject<FbxAnimStack>(0);

	if (stack == nullptr)
	{
		return;
	}
	FbxString TakeName = stack->GetName();
	FbxTakeInfo* TakeInfo = Scene->GetTakeInfo(TakeName);
	FbxTimeSpan LocalTimeSpan = TakeInfo->mLocalTimeSpan;
	FbxTime start = LocalTimeSpan.GetStart();
	FbxTime end = LocalTimeSpan.GetStop();
	FbxTime Duration = LocalTimeSpan.GetDuration();


	FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
	FbxLongLong s = start.GetFrameCount(TimeMode);
	FbxLongLong n = end.GetFrameCount(TimeMode);
	AnimScene.StartFrame = s;
	AnimScene.EndFrame = n;
	AnimScene.FrameSpeed = 30.0f;
	AnimScene.TickPerFrame = 160;
	FbxTime time;
	AnimTrack Track;

	for (FbxLongLong t = s; t <= n; t++) 
	{
		time.SetFrame(t, TimeMode);
		for (int obj = 0; obj < ObjList.size(); obj++) {
			FbxAMatrix GlobalMat = ObjList[obj]->Node->EvaluateGlobalTransform(time);
			Track.Frame = t;
			Track.AniMat = DxConvertMatrix(GlobalMat);
			D3DXMatrixDecompose(&Track.Scale, &Track.Rotate, &Track.Trans, &Track.AniMat); 
			ObjList[obj]->AniTrackList.push_back(Track);
		}
	}
}

bool FbxSceneLoader::ParseMeshSkinning(FbxMesh* mesh, FbxEntity* Obj)
{
	int DeformerCount = mesh->GetDeformerCount(FbxDeformer::eSkin);//스킨의 수를 추출
	//skinning 찾는 단계
	if (DeformerCount == 0) {//없으면 return
		return false;
	}

	//정점의 갯수 추출
	int VertexCount = mesh->GetControlPointsCount();
	Obj->WeightList.resize(VertexCount);//정점의 갯수 만큼 resize

	for (int DeformerIndex = 0; DeformerIndex < DeformerCount; DeformerIndex++) 
	{
		FbxDeformer* deformer = mesh->GetDeformer(DeformerIndex, FbxDeformer::eSkin);
		FbxSkin* Skin = (FbxSkin*)deformer;

		DWORD ClusterCount = Skin->GetClusterCount();


		for (int ClusterIndex = 0; ClusterIndex < ClusterCount; ClusterIndex++) 
		{
			FbxCluster* Cluster = Skin->GetCluster(ClusterIndex);
			FbxNode* node = Cluster->GetLink();

			FbxAMatrix matXBindPose; 
			FbxAMatrix matReferenceGlobalInitPosition;

			Cluster->GetTransformLinkMatrix(matXBindPose);	


			Cluster->GetTransformMatrix(matReferenceGlobalInitPosition);

			FbxAMatrix matBindPos = matReferenceGlobalInitPosition.Inverse() * matXBindPose; 
			

			Matrix matInvBindPos = DxConvertMatrix(matBindPos);

			matInvBindPos = matInvBindPos.Invert();

			int BoneIndex = ObjectIDMap.find(node)->second;
			std::wstring name = StrToWstr(node->GetName());

			Obj->MatrixBindPoseMap.insert(std::make_pair(BoneIndex, matInvBindPos));	
			Obj->MatrixBindPoseMapName.insert(std::make_pair(name, matInvBindPos));	

			int ClusterSize = Cluster->GetControlPointIndicesCount();					

			int* Indices = Cluster->GetControlPointIndices();							
			double* Weights = Cluster->GetControlPointWeights();						

			for (int i = 0; i < ClusterSize; i++)
			{	
				int VertexIndex = Indices[i];		
				float weigth = Weights[i];			
				Obj->WeightList[VertexIndex].Insert(BoneIndex, weigth);
			}
		}
	}
	return true;
}

bool FbxSceneLoader::UpdateFrame(ID3D11DeviceContext* context)
{
	AnimFrame = AnimFrame + gSecondPerFrame * AnimSpeed * AnimScene.FrameSpeed * AnimInverse;
	if (AnimFrame > AnimScene.EndFrame ||
		AnimFrame < AnimScene.StartFrame)
	{
		AnimFrame = min(AnimFrame, AnimScene.EndFrame);
		AnimFrame = max(AnimFrame, AnimScene.StartFrame);
		AnimInverse *= -1.0f;
	}
	std::vector<Matrix> matCurrentAnimList;
	for (int Bone = 0; Bone < ObjList.size(); Bone++) 
	{
		Matrix AniMat = ObjList[Bone]->Interplate(this, AnimFrame);
		D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &AniMat);
		matCurrentAnimList.push_back(AniMat);
	}
	context->UpdateSubresource(BoneCB, 0, nullptr, &BoneData, 0, 0);

	for (int Draw = 0; Draw < DrawObjList.size(); Draw++) 
	{

		if (DrawObjList[Draw]->MatrixBindPoseMapName.size()) 
		{

			for (int Bone = 0; Bone < ObjList.size(); Bone++) 
			{
				std::wstring name = ObjList[Bone]->Name;
				auto iter = DrawObjList[Draw]->MatrixBindPoseMapName.find(name);
				if (iter != DrawObjList[Draw]->MatrixBindPoseMapName.end()) 
				{
					Matrix matBind = iter->second;
					Matrix matAni = matBind * matCurrentAnimList[Bone];
					D3DXMatrixTranspose(&BoneData.BoneMat[Bone], &matAni);
				}
			}
			context->UpdateSubresource(DrawObjList[Draw]->ObjMesh.SkinBoneCB, 0, nullptr, &BoneData, 0, 0);
		}
	}
	return true;
}

void FbxSceneLoader::UpdateSkeleton(ID3D11DeviceContext* context, float time, VS_CONSTANT_BONE_BUFFER& data)
{
	for (int bone = 0; bone < ObjList.size(); bone++) 
	{
		Matrix AniMat = ObjList[bone]->Interplate(this, time);
		data.BoneMat[bone] = AniMat;
		//D3DXMatrixTranspose(&data.BoneMat[bone], &AniMat);
	}
}

void FbxSceneLoader::UpdateSkinning(ID3D11DeviceContext* context, VS_CONSTANT_BONE_BUFFER& InputData, std::vector<VS_CONSTANT_BONE_BUFFER>& OutputData)
{
	for (int draw = 0; draw < DrawObjList.size(); draw++) 
	{
		if (DrawObjList[draw]->MatrixBindPoseMap.size()) 
		{
			for (int bone = 0; bone < ObjList.size(); bone++) 
			{
				auto iter = DrawObjList[draw]->MatrixBindPoseMap.find(bone);
				if (iter != DrawObjList[draw]->MatrixBindPoseMap.end())
				{
					Matrix BindMat = iter->second;
					Matrix Animat = BindMat * InputData.BoneMat[bone];
					D3DXMatrixTranspose(&OutputData[draw].BoneMat[bone], &Animat);
				}
			}
		}
	}
}
