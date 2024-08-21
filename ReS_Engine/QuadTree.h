#pragma once
#include "Node.h"
#include "Frustum.h"

class QuadTree
{
public:
	ID3D11Device* D3D11Device;
	ID3D11DeviceContext* D3D11Context;
	Node* RootNode;
	DWORD MapWidth;
	DWORD MapHeight;
	Frustum CameraFrustum;
	int ChildCount = 4;
	int MaxDepth = 4;
	std::vector<Node*> DrawNode;
	std::vector<Node*> NodeList;
	std::vector<Vertex> VertexList;
	std::vector<DWORD> IndexList;

	Matrix ViewMat;
	Matrix ProjMat;
public:
	bool					Render();
	void					SetData(ID3D11Device* device, ID3D11DeviceContext* context, std::vector<Vertex>& vt);
	void					CreateTree(DWORD width, DWORD height, DWORD RowIndex, DWORD ColIndex);
	void					BuildTree(Node* Node, int ChildCount);
	Node*					CreateNode(Node* parent, DWORD lt, DWORD rt, DWORD lb, DWORD rb);
	void					VisibleNode(Node* node);
	void					SetFrustum(Frustum fr);
	bool					Release();
	bool					AddObject(class Object3D* object);
	Node*					FindNode(Node* node, Box obj_box);
	void					SetMatrix(Matrix* view, Matrix* proj);
	void					RenderObj(Node* node);
	UINT					SelectVertexList(Box& obj_box, std::vector<Node*>& selectedNodeList);
	void					UpdateTree(std::vector<Vertex>& vertexList);
	void					Reset();
	void					DynamicObjReset(Node* Node);

public:
	~QuadTree();
};

