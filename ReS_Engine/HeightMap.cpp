#include "HeightMap.h"
#include "Manager/TextureManager.h"
#include "Object/Object3D.h"
void HeightMap::SetMapSize(UINT width, UINT height, float cellDistance)
{
    MapWidth = width + 1;
    MapHeight = height + 1;
    RowIndex = height;
    ColIndex = width;
    FaceCount = width * height * 2.0f;
    CellDistance = cellDistance;
}

void HeightMap::SetMapSize(UINT width, UINT height)
{
    MapWidth = width + 1;
    MapHeight = height + 1;
    RowIndex = height;
    ColIndex = width;
    FaceCount = width * height * 2.0f;
    CellDistance = 1;
}

void HeightMap::SetCellDist(float cellDist)
{
    CellDistance = cellDist;
}

void HeightMap::SetVertexList()
{
    VertexList.resize(MapWidth * MapHeight);
    int HalfWidth = MapWidth / 2;
    int HalfHeight = MapHeight / 2;

    for (int iRow = 0; iRow < MapHeight; iRow++)
    {
        for (int iCol = 0; iCol < MapWidth; iCol++)
        {
            VertexList[iRow * MapWidth + iCol].Position = { (float)(iCol - HalfWidth) * CellDistance, 0.0f, (float)(HalfHeight - iRow) * CellDistance };

            VertexList[iRow * MapWidth + iCol].Color =
            { 0,0,0,0 };
            VertexList[iRow * MapWidth + iCol].Texture =
            { ((float)iCol / (float)(MapWidth - 1)),
              ((float)iRow / (float)(MapHeight - 1)) };
        }
    }
}

void HeightMap::SetIndexList()
{
    GameMap::SetIndexList();
    GenVertexNorm();
}

void HeightMap::CheckVisible(Frustum fr)
{
    GameCamera = fr;
    SpaceDivisionTree.DrawNode.clear();
    SpaceDivisionTree.SetFrustum(GameCamera);
    SpaceDivisionTree.SetMatrix(&ViewMat, &ProjMat);

    Render();
}

void HeightMap::GenVertexNorm()
{
    FaceInfoList.resize(FaceCount);
    UINT face = 0;
    for (UINT i = 0; i < IndexList.size(); i += 3)
    {
        UINT i0 = IndexList[i + 0];
        UINT i1 = IndexList[i + 1];
        UINT i2 = IndexList[i + 2];
        FaceInfoList[face].VertexArray[0] = i0;
        FaceInfoList[face].VertexArray[1] = i1;
        FaceInfoList[face].VertexArray[2] = i2;

        FaceInfoList[face++].NormalVec = ComputeFaceNorm(i0, i1, i2);
    }

    VertexInfoList.resize(MapWidth * MapHeight);
    for (UINT face = 0; face < FaceCount; face++)
    {
        for (UINT i = 0; i < 3; i++)
        {
            UINT index = IndexList[face * 3 + i];
            VertexInfoList[index].FaceIndexArray.push_back(face);
        }
    }
    for (UINT vertex = 0; vertex < VertexInfoList.size(); vertex++)
    {
        ComputeVertexNorm(vertex);
    }
}

void HeightMap::ComputeVertexNorm(UINT vertex)
{
    for (UINT i = 0; i < VertexInfoList[vertex].FaceIndexArray.size(); i++)
    {
        UINT faceIndex = VertexInfoList[vertex].FaceIndexArray[i];
        UINT i0 = FaceInfoList[faceIndex].VertexArray[0];
        UINT i1 = FaceInfoList[faceIndex].VertexArray[1];
        UINT i2 = FaceInfoList[faceIndex].VertexArray[2];
        FaceInfoList[faceIndex].NormalVec = ComputeFaceNorm(i0, i1, i2);

        VertexInfoList[vertex].NormalVec += FaceInfoList[faceIndex].NormalVec;
    }
    D3DXVec3Normalize(&VertexInfoList[vertex].NormalVec, &VertexInfoList[vertex].NormalVec);

    VertexList[vertex].Normal = VertexInfoList[vertex].NormalVec;
}

Vector3 HeightMap::ComputeFaceNorm(UINT i0, UINT i1, UINT i2)
{
    Vector3 nor;
    Vector3 edge0 = VertexList[i1].Position - VertexList[i0].Position;
    Vector3 edge1 = VertexList[i2].Position - VertexList[i0].Position;
    D3DXVec3Cross(&nor, &edge0, &edge1);
    D3DXVec3Normalize(&nor, &nor);

    return nor;
}

bool HeightMap::Render()
{
    PreRender();
    SpaceDivisionTree.Render();
    return true;
}

bool HeightMap::PreRender()
{
    GameMap::PreRender();
    D3D11Context->PSSetShaderResources(1, 1, &MaskAlphaTexSRV);

    for (int i = 1; i < 5; i++)
    {
        if (TextureArray[i] != nullptr)
        {
            D3D11Context->PSSetShaderResources(i + 1, 1, &TextureArray[i]->TextureSRV);
        }
    }
    return true;
}

bool HeightMap::CreateVertex()
{
    BaseObject::CreateVertex();
    SpaceDivisionTree.SetData(D3D11Device, D3D11Context, VertexList);

    SpaceDivisionTree.CreateTree(MapWidth, MapHeight, RowIndex, ColIndex);
    UpdateMapDataBuffer();
    return false;
}

bool HeightMap::LoadHeightMap(std::wstring HeightMapName)
{
    HRESULT hr;
    ID3D11Resource* Texture;
    hr = DirectX::CreateWICTextureFromFileEx(D3D11Device, D3D11Context, HeightMapName.c_str(), 0, D3D11_USAGE_STAGING, 0,
        D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, 0, WIC_LOADER_DEFAULT, &Texture, nullptr);
    if (hr == S_FALSE)
    {
        return false;
    }
    ID3D11Texture2D* Texture2D = NULL;
    if (FAILED(Texture->QueryInterface(__uuidof(ID3D11Texture2D), (LPVOID*)&Texture2D))) 
    {
        return false;
    }
    D3D11_TEXTURE2D_DESC td;
    Texture2D->GetDesc(&td);
    HeightList.resize(td.Height * td.Width);

    D3D11_MAPPED_SUBRESOURCE MappedFaceDest;
    if (SUCCEEDED(D3D11Context->Map(Texture2D, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_READ, 0, &MappedFaceDest))) 
    {
        UCHAR* Texels = (UCHAR*)MappedFaceDest.pData;
        for (UINT row = 0; row < td.Height; row++) 
        {
            UINT rowStart = row * MappedFaceDest.RowPitch;
            for (UINT col = 0; col < td.Width; col++) 
            {
                UINT colStart = col * 4;
                UINT Red = Texels[rowStart + colStart + 0];
                HeightList[row * td.Width + col] = (float)Red / 255.0f;
            }
        }
        D3D11Context->Unmap(Texture2D, D3D11CalcSubresource(0, 0, 1));
    }
    else {
        return false;
    }
    MapWidth = td.Width;
    MapHeight = td.Height;
    RowIndex = td.Height - 1;
    ColIndex = td.Width - 1;
    FaceCount = MapWidth * MapWidth * 2.0f;
    CellDistance = 1.0f;
    Texture2D->Release();
    Texture->Release();
    return true;
}

bool HeightMap::Frame()
{
    return true;
}

bool HeightMap::Release()
{
    delete[] AlphaData;

    if (MaskAlphaTex)
    {
        MaskAlphaTex->Release();
    }
    if (MaskAlphaTexSRV)
    {
        MaskAlphaTexSRV->Release();
    }

    GameMap::Release();
    SpaceDivisionTree.Release();

    return true;
}

UINT HeightMap::SelectVertexList(Box& box, std::vector<Node*>& selectedNodeList)
{
    return SpaceDivisionTree.SelectVertexList(box, selectedNodeList);
}

float HeightMap::GetMapHeight(int row, int col)
{
    return VertexList[row * MapWidth + col].Position.y;
}

float HeightMap::GetHeight(float posX, float posZ)
{
    float cellX = (float)(ColIndex * CellDistance / 2.0f + posX);
    float cellZ = (float)(RowIndex * CellDistance / 2.0f - posZ);

    cellX /= (float)CellDistance;
    cellZ /= (float)CellDistance;

    float vertexCol = ::floorf(cellX);
    float vertexRow = ::floorf(cellZ);

    if (vertexCol < 0.0f)
    {
        vertexCol = 0.0f;
    }
    if (vertexRow < 0.0f)
    {
        vertexRow = 0.0f;
    }
    if ((float)(ColIndex - 2) < vertexCol)
    {
        vertexCol = (float)(ColIndex - 2);
    }
    if ((float)(RowIndex - 2) < vertexRow)
    {
        vertexRow = (float)(RowIndex - 2);
    }

    float A = GetMapHeight((int)vertexRow, (int)vertexCol);
    float B = GetMapHeight((int)vertexRow, (int)vertexCol + 1);
    float C = GetMapHeight((int)vertexRow + 1, (int)vertexCol);
    float D = GetMapHeight((int)vertexRow + 1, (int)vertexCol + 1);

    float deltaX = cellX - vertexCol;
    float deltaZ = cellZ - vertexRow;

    float height = 0.0f;

    if (deltaZ < (1.0f - deltaX))
    {
        float uy = B - A;
        float vy = C - A;

        height = D + Lerp(0.0f, uy, deltaX) + Lerp(0.0f, vy, deltaZ);
    }
    else
    {
        float uy = C - D;
        float vy = B - D;

        height = D + Lerp(0.0f, uy, 1.0f - deltaX) + Lerp(0.0, vy, 1.0f - deltaZ);
    }
    return height;
}

float HeightMap::Lerp(float start, float end, float tangent)
{
    return start - (start * tangent) + (end * tangent);
}


void HeightMap::MapUpdate(std::vector<Node*>& nodeList)
{
    BaseObject::UpdateVertexList();
    SpaceDivisionTree.UpdateTree(VertexList);
    UpdateMapObjPos(nodeList);
}

void HeightMap::UpdateMapObjPos(std::vector<Node*>& nodeList)
{
    for (auto node : SpaceDivisionTree.NodeList)
    {
        for (auto obj : node->ObjectList)
        {
            Vector3 pos;
            pos.x = obj->Position.x;
            pos.z = obj->Position.z;
            pos.y = GetHeight(pos.x, pos.z);
            obj->UpdatePosition(pos);
        }
    }
}

HRESULT HeightMap::CraeteAlphaTexture(DWORD width, DWORD height)
{
    HRESULT hr;
    D3D11_TEXTURE2D_DESC td;
    ZeroMemory(&td, sizeof(td));
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    AlphaData = new BYTE[width * height * 4];

    for (UINT y = 0; y < height; y++)
    {
        for (UINT x = 0; x < width; x++)
        {
            BYTE* pixel = &AlphaData[width * y * 4 + x * 4];
            pixel[0] = 0; //r
            pixel[1] = 0; //g
            pixel[2] = 0; //b
            pixel[3] = 0; //a
        }
    }
    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = AlphaData;
    initData.SysMemPitch = sizeof(BYTE) * 4 * width;
    initData.SysMemSlicePitch = 0;
    if (FAILED(hr = D3D11Device->CreateTexture2D(&td, &initData, &MaskAlphaTex)))
    {
        return hr;
    }
    if (FAILED(hr = D3D11Device->CreateShaderResourceView(MaskAlphaTex, NULL, &MaskAlphaTexSRV)))
    {
        return hr;
    }
    return hr;
}

void HeightMap::Splatting(Vector3 interSection, UINT splattiongTexIndex, float splattingRadius)
{
    UINT splattingIndex = splattiongTexIndex;
    UINT const DataSize = sizeof(BYTE) * 4;
    UINT const RowPitch = DataSize * 1024;
    UINT const DepthPitch = 0;

    Vector2 texIndex;
    Vector2 uv;
    Vector2 maxSize = { (float)RowIndex / 2,(float)RowIndex / 2 };
    UINT texSize = 1024;
    Vector3 texPos;
    Vector3 pickPos = interSection;


    //splattingIndex = rand() % 4;

    for (UINT y = 0; y < texSize; y++)
    {
        texIndex.y = y;
        for (UINT x = 0; x < texSize; x++)
        {
            texIndex.x = x;
            uv = Vector2((texIndex.x / (float)texSize) * 2.0f - 1.0f, -(texIndex.y / (float)texSize * 2.0f - 1.0f));

            texPos = Vector3(uv.x * maxSize.x, 0.0f, uv.y * maxSize.y);
            BYTE* pixel = &AlphaData[texSize * y * 4 + x * 4];
            Vector3 temp = pickPos - texPos;
            float radius = D3DXVec3Length(&temp);

            if (radius < splattingRadius)
            {
                float dot = 1.0f - (radius / splattingRadius);

                if (splattingIndex == 0 && (dot * 255) > pixel[0])
                {
                    pixel[0] = dot * 255;
                }
                if (splattingIndex == 1 && (dot * 255) > pixel[1])
                {
                    pixel[1] = dot * 255;
                }
                if (splattingIndex == 2 && (dot * 255) > pixel[2])
                {
                    pixel[2] = dot * 255;
                }
                if (splattingIndex == 3 && (dot * 255) > pixel[3])
                {
                    pixel[3] = dot * 255;
                }
            }
        }
    }
    D3D11Context->UpdateSubresource(MaskAlphaTex, 0, nullptr, AlphaData, RowPitch, DepthPitch);
}

void HeightMap::UpdateMapDataBuffer()
{
    Mesh.ConstBufferData.mapData = Vector4(MapWidth, MapHeight, 0, 0);
    D3D11Context->UpdateSubresource(Mesh.ConstantBuffer, 0, nullptr, &Mesh.ConstBufferData, 0, 0);
}

void HeightMap::SetSubTexture(int index)
{
}

void HeightMap::MapReset()
{
    SpaceDivisionTree.Reset();
}
