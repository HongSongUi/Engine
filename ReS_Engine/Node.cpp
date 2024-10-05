#include "Node.h"
#include "Object/Object3D.h"
Node::Node()
{
    ChildCount = 0;
    Depth = 0;
    ParentNode = nullptr;
}

Node::~Node()
{
	Release();
}

bool Node::Init()
{
	return true;
}

bool Node::Render()
{
	return true;
}

bool Node::Frame()
{
	return true;
}

bool Node::Release()
{
	for (auto obj : ObjectList)
	{
		obj->Release();
		delete obj;
		obj = nullptr;
	}
	ObjectList.clear();

	for (int i = 0; i < ChildList.size(); i++)
	{
		delete ChildList[i];
		ChildList[i] = nullptr;
	}


	if (IndexBuffer) IndexBuffer->Release();
	IndexBuffer = nullptr;
	return true;
}
void Node::SetNode(DWORD lt, DWORD rt, DWORD lb, DWORD rb)
{
	LeftTop = lt;
	RightTop = rt;
	LeftBottom = lb;
	RightBottom = rb;
	CreateIndexBuffer();
}

void Node::SetData(ID3D11Device* device, std::vector<Vertex>& vt, int child, DWORD Width, DWORD Height)
{
	D3D11Device = device;
	VertexList = vt;
	ChildCount = child;
	ChildList.resize(ChildCount);
	MapWidth = Width;
	MapHeight = Height;
	RowIndex = MapHeight - 1;
	ColIndex = MapWidth - 1;
}

void Node::SetIndexNBox()
{
	DWORD RowCellCount = (LeftBottom - LeftTop) / MapWidth;
	DWORD ColCellCount = RightTop - LeftTop;
	DWORD CellsCount = RowCellCount * ColCellCount;
	IndexList.resize(CellsCount * 2 * 3);
	int iIndex = 0;


	NodeBox.Min.x = VertexList[LeftTop].Position.x;
	NodeBox.Min.y = 100000.0f;
	NodeBox.Min.z = VertexList[LeftBottom].Position.z;
	NodeBox.Max.x = VertexList[RightTop].Position.x;
	NodeBox.Max.y = -100000.0f;
	NodeBox.Max.z = VertexList[LeftTop].Position.z;

	for (int dwRow = 0; dwRow < RowCellCount; dwRow++)
	{
		for (int dwCol = 0; dwCol < ColCellCount; dwCol++)
		{
			IndexList[iIndex + 0] = LeftTop + dwCol + (dwRow * MapWidth);
			IndexList[iIndex + 1] = IndexList[iIndex + 0] + 1;
			IndexList[iIndex + 2] = LeftTop + dwCol + ((dwRow + 1) * MapHeight);

			IndexList[iIndex + 3] = IndexList[iIndex + 2];
			IndexList[iIndex + 4] = IndexList[iIndex + 1];
			IndexList[iIndex + 5] = IndexList[iIndex + 2] + 1;

			for (DWORD dwVertex = 0; dwVertex < 6; dwVertex++)
			{
				if (NodeBox.Min.y > VertexList[IndexList[iIndex + dwVertex]].Position.y)
				{
					NodeBox.Min.y = VertexList[IndexList[iIndex + dwVertex]].Position.y;
				}
				if (NodeBox.Max.y < VertexList[IndexList[iIndex + dwVertex]].Position.y)
				{
					NodeBox.Max.y = VertexList[IndexList[iIndex + dwVertex]].Position.y;
				}
			}
			iIndex += 6;
		}
	}
	FaceCount = IndexList.size() / 3;

	NodeBox.SetBox(NodeBox.Max, NodeBox.Min);
}

HRESULT Node::CreateIndexBuffer()
{
	HRESULT hr;
	SetIndexNBox();
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(DWORD) * IndexList.size();
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &IndexList.at(0);
	hr = D3D11Device->CreateBuffer(&bd, &sd, &IndexBuffer);
	return hr;
}



void Node::UpdateNode(std::vector<Vertex>& vertexList)
{
	VertexList = vertexList;
	SetIndexNBox();
}
