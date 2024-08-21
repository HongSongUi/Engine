#include "QuadTree.h"
#include "Object3D.h"

bool QuadTree::Render()
{
	for (auto node : DrawNode)
	{
		D3D11Context->IASetIndexBuffer(node->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		D3D11Context->DrawIndexed(node->FaceCount * 3, 0, 0);
	}
	RenderObj(RootNode);

	return true;
}

void QuadTree::SetData(ID3D11Device* device, ID3D11DeviceContext* context, std::vector<Vertex>& vt)
{
	D3D11Device = device;
	D3D11Context = context;
	VertexList = vt;
}

void QuadTree::CreateTree(DWORD width, DWORD height, DWORD RowIndex, DWORD ColIndex)
{
	DWORD LB = RowIndex * height;
	
	MapWidth = width;
	MapHeight = height;
	RootNode = CreateNode(nullptr, 0, RowIndex, LB, LB + RowIndex);
	BuildTree(RootNode, ChildCount);
}

void QuadTree::BuildTree(Node* Node, int ChildCount)
{
	if (Node->Depth >= MaxDepth) 
	{
		Node->LeafNode = true;
		return;
	}
	if (Node->RightTop - Node->LeftTop == 1) 
	{
		Node->LeafNode = true;
		return;
	}
	DWORD lt, rt, lb, rb;
	DWORD TopCenter, BottomCenter, LeftCenter, RightCenter, Center;

	lt = Node->LeftTop;
	rt = Node->RightTop;
	lb = Node->LeftBottom;
	rb = Node->RightBottom;
	TopCenter = (lt + rt) / 2;
	BottomCenter = (rb + lb) / 2;
	LeftCenter = (lt + lb) / 2;
	RightCenter = (rt + rb) / 2;
	Center = (lt + rt + lb + rb) / 4;
	Node->ChildList[0] = CreateNode(Node, lt, TopCenter, LeftCenter, Center);
	Node->ChildList[1] = CreateNode(Node, TopCenter, rt, Center, RightCenter);
	Node->ChildList[2] = CreateNode(Node, LeftCenter, Center, lb, BottomCenter);
	Node->ChildList[3] = CreateNode(Node, Center, RightCenter, BottomCenter, rb);
	for (int i = 0; i < Node->ChildList.size(); i++)
	{
		BuildTree(Node->ChildList[i], ChildCount);
	}
}

Node* QuadTree::CreateNode(Node* parent, DWORD lt, DWORD rt, DWORD lb, DWORD rb)
{
	Node* NewNode = new Node;
	NewNode->SetData(D3D11Device, VertexList, 4, MapWidth, MapHeight);
	NewNode->SetNode(lt, rt, lb, rb);
	if (parent != nullptr) 
	{
		NewNode->ParentNode = parent;
		NewNode->Depth = parent->Depth + 1;
	}
	NodeList.push_back(NewNode);
	return NewNode;
}

void QuadTree::VisibleNode(Node* node)
{
	if (node == nullptr) {
		return;
	}
	H_POSITION Rend = CameraFrustum.ClassifyBox(node->NodeBox);
	if (Rend == _FRONT)
	{
		DrawNode.push_back(node);
		return;
	}
	if (Rend == _SPANNING)
	{
		if (node->LeafNode == true)
		{
			DrawNode.push_back(node);
		}
		else
		{
			for (int iNode = 0; iNode < 4; iNode++)
			{
				VisibleNode(node->ChildList[iNode]);
			}
		}
	}
}

void QuadTree::SetFrustum(Frustum fr)
{
	CameraFrustum = fr;
	VisibleNode(RootNode);
}

bool QuadTree::Release()
{
	delete RootNode;
	RootNode = nullptr;
	return true;
}

bool QuadTree::AddObject(Object3D* object)
{
	Node* findNode = FindNode(RootNode, object->ObjectBox);
	if (findNode != nullptr)
	{
		findNode->ObjectList.push_back(object);
		return true;
	}
	return false;
}

Node* QuadTree::FindNode(Node* node, Box obj_box)
{
	for (int i = 0; i < 4; i++) {
		if (node->ChildList[i] != nullptr)
		{
			if (node->ChildList[i]->NodeBox.Min.x <= obj_box.Min.x &&
				node->ChildList[i]->NodeBox.Min.z <= obj_box.Min.z)
			{
				if (node->ChildList[i]->NodeBox.Max.x >= obj_box.Max.x &&
					node->ChildList[i]->NodeBox.Max.z >= obj_box.Max.z)
				{
					node = FindNode(node->ChildList[i], obj_box);
					break;
				}
			}
		}
	}
	return node;
}

void QuadTree::SetMatrix(Matrix* view, Matrix* proj)
{
	if (view != nullptr)
	{
		ViewMat = *view;
	}
	if (proj != nullptr)
	{
		ProjMat = *proj;
	}

}

void QuadTree::RenderObj(Node* node)
{
	if (node == nullptr)
	{
		return;
	}
	for (auto obj : node->ObjectList)
	{
		H_POSITION Rend = CameraFrustum.ClassifyBox(obj->ObjectBox);
		if (Rend == _FRONT || Rend == _SPANNING)
		{
			obj->SetMatrix(nullptr, &ViewMat, &ProjMat);
			obj->Render();
		}
	}
	for (int child = 0; child < 4; child++)
	{
		RenderObj(node->ChildList[child]);
	}
}

UINT QuadTree::SelectVertexList(Box& obj_box, std::vector<Node*>& selectedNodeList)
{
	for (auto node : DrawNode)
	{
		if (node != nullptr)
		{
			HCollisionType ret = Collision::BoxToBox(node->NodeBox, obj_box);
			if (ret > 0)
			{
				selectedNodeList.push_back(node);
			}
		}
	}
	return selectedNodeList.size();
}

void QuadTree::UpdateTree(std::vector<Vertex>& vertexList)
{
	VertexList = vertexList;
	for (auto node : NodeList)
	{
		node->UpdateNode(vertexList);
	}
}

void QuadTree::Reset()
{
	DynamicObjReset(RootNode);
}

void QuadTree::DynamicObjReset(Node* Node)
{
	if (Node == nullptr) 
	{
		return;
	}
	Node->ObjectList.clear();
	for (int i = 0; i < Node->ChildList.size(); i++) 
	{
		DynamicObjReset(Node->ChildList[i]);
	}
}

QuadTree::~QuadTree()
{
	delete RootNode;
	RootNode = nullptr;
}
