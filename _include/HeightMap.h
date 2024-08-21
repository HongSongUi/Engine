#pragma once
#include "GameMap.h"
#include "QuadTree.h"
struct VertexInfo
{
	std::vector<UINT> FaceIndexArray;
	Vector3 NormalVec;
};
struct FaceInfo
{
	UINT VertexArray[3];
	Vector3 NormalVec;
};
class HeightMap : public GameMap
{
public:
	QuadTree SpaceDivisionTree;
	DWORD RowIndex;
	DWORD ColIndex;
	Frustum GameCamera;
	std::vector<VertexInfo> VertexInfoList;
	std::vector<FaceInfo> FaceInfoList;
	std::vector<float> HeightList;

	class Texture* TextureArray[5];
	BYTE* AlphaData;
	ID3D11Texture2D* MaskAlphaTex;
	ID3D11ShaderResourceView* MaskAlphaTexSRV;

public:
	void SetMapSize(UINT width, UINT height, float cellDistance);
	void SetMapSize(UINT width, UINT height);
	void SetCellDist(float cellDist);
	void SetVertexList()override;
	void SetIndexList()override;
	void CheckVisible(Frustum fr);
	void GenVertexNorm();
	void ComputeVertexNorm(UINT vertex);
	bool Render()override;
	bool PreRender() override;
	bool CreateVertex()override;
	bool LoadHeightMap(std::wstring HeightMapName);
	bool Frame()override;
	bool Release()override;
	UINT SelectVertexList(Box& box, std::vector<Node*>& selectedNodeList);

	float GetMapHeight(int row, int col);
	float GetHeight(float posX, float posZ);
	float Lerp(float start, float end, float tangent);

	Vector3 ComputeFaceNorm(UINT i0, UINT i1, UINT i2);

	void MapUpdate(std::vector<Node*>& nodeList);
	void UpdateMapObjPos(std::vector<Node*>& nodeList);
	HRESULT CraeteAlphaTexture(DWORD width, DWORD height);
	void Splatting(Vector3 interSection, UINT splattiongTexIndex, float splattingRadius = 5.0f);

	void UpdateMapDataBuffer();
	void SetSubTexture(int index);

	void MapReset();
};

