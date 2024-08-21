#pragma once
#include "ObjectMesh.h"

class Texture;

class FbxObjMesh : public ObjectMesh
{
public:
	using  SubIWVertex = std::vector<IW_VERTEX>;
	using  SubVertex = std::vector<Vertex>;
	std::vector<ID3D11Buffer*>	SubVertexBuffer;
	std::vector<ID3D11Buffer*>	SubIWVertexBuffer;
	std::vector<Texture*>		SubTextureList;
	std::vector<IW_VERTEX>		IWVertexList;

	std::vector<SubIWVertex>	SubIWVertexList;
	std::vector<SubVertex>		SubVertexList;

	VS_CONSTANT_BONE_BUFFER		BoneData;
	ID3D11Buffer*				SkinBoneCB;
	ID3D11Buffer*				IWVertexBuffer;
	bool IsSkinning;

public:
	HRESULT CreateVertexBuffer()override;
	HRESULT CreateConstantBuffer()override;
	HRESULT CreateInputLayout()override;
	bool PostRender()override;
	bool Release()override;
	void SetSubVertexList(std::vector<std::vector<Vertex>>& subVertex);
	void SetSubTextureList(std::vector<Texture*>& subTexture);
	void SetSubIWVertexList(std::vector<SubIWVertex>& subIWVertex);
	void SetIWVertexList(std::vector<IW_VERTEX>& IwVertex);
	void SetSkinning(bool skinnig);
};

