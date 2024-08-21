#pragma once
#include "Device.h"
#include "Collision.h"
#include "ObjectMesh.h"
class Node
{
public:
	int						Depth;
	int						ChildCount;
	bool					LeafNode = false;
	ID3D11Device*			D3D11Device;
	ID3D11Buffer*			IndexBuffer;
	DWORD					LeftTop;
	DWORD					RightTop;
	DWORD					LeftBottom;
	DWORD					RightBottom;
	DWORD					RowIndex;
	DWORD					ColIndex;
	DWORD					MapWidth;
	DWORD					MapHeight;
	DWORD					FaceCount;
	Node*					ParentNode;

	Box						NodeBox;

	std::vector<Node*>		ChildList;
	std::vector<Vertex>		VertexList;
	std::vector<DWORD>		IndexList;
	std::vector<class Object3D*>	ObjectList;
public:
	Node();
	~Node();
public:
	void SetNode(DWORD lt, DWORD rt, DWORD lb, DWORD rb);
	void SetData(ID3D11Device* device, std::vector<Vertex>& vt, int child, DWORD MapWidth, DWORD MapHeight);
	void SetIndexNBox();
	HRESULT CreateIndexBuffer();
public:
	bool Init();
	bool Render();
	bool Frame();
	bool Release();
	void UpdateNode(std::vector<Vertex>& vertexList);
};

