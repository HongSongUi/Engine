#include "BaseObject.h"
#include "ShaderManager.h"
#include "TextureManager.h"

bool BaseObject::SetData(ID3D11Device* device, ID3D11DeviceContext* context, RECT clientRt)
{
    D3D11Device = device;
    D3D11Context = context;
    Mesh.SetDevice(device, context);
    ClientRect = clientRt;
    return true;
}
bool BaseObject::Load(std::wstring ShaderFileName, std::wstring TextureFileName)
{
    if (FAILED(ShaderCompile(ShaderFileName)))
    {
        return false;
    }
    if (FAILED(LoadTexture(TextureFileName)))
    {
        return false;
    }
    return true;
}
bool BaseObject::CreateVertex()
{
    SetVertexList();
    InitVertexList = VertexList;
    SetIndexList();
    Mesh.SetVertexList(VertexList);
    Mesh.SetIndexList(IndexList);
    Mesh.SetShader(ShaderFile);
    Mesh.SetTexture(TextureFile);
    if (Mesh.Create() == false)
    {
        return false;
    }
    return true;
}

void BaseObject::UpdateVertexList()
{
    //SetVertexList();
    Mesh.SetVertexList(VertexList);
    Mesh.UpdateVertexBuffer();
}

void BaseObject::SetMatrix(Matrix* world, Matrix* view, Matrix* proj)
{
    if (world != nullptr)
    {
        WorldMat = *world;
    }
    if (view != nullptr)
    {
        ViewMat = *view;
    }
    if (proj != nullptr)
    {
        ProjMat = *proj;
    }
    Mesh.SetMatrix(world, view, proj);
}

void BaseObject::SetLight(Vector4* lightDir, Vector4* lightPos, Vector4* eyePos, Vector4* eyeDir)
{
    if (lightDir != nullptr)
    {
        LightDir = *lightDir;
    }
    if (lightPos != nullptr)
    {
        LightPos = *lightPos;
    }
    if (eyePos != nullptr)
    {
        EyePos = *eyePos;
    }
    if (eyeDir != nullptr)
    {
        EyeDir = *eyeDir;
    }
    Mesh.SetLightData(lightDir, lightPos, eyePos, eyeDir);
}

void BaseObject::SetVertexList()
{
    VertexList[0].Position = { -1.0f,1.0f,0.0f };
    VertexList[0].Color = { 1.0f,1.0f,1.0f,1.0f };
    VertexList[1].Position = { 1.0f,1.0f,0.0f };
    VertexList[1].Color = { 0.0f,1.0f,1.0f,1.0f };
    VertexList[2].Position = { -1.0f,-1.0f,0.0f };
    VertexList[2].Color = { 1.0f,1.0f,0.0f,1.0f };
    VertexList[3].Position = { 1.0f,-1.0f,0.0f };
    VertexList[3].Color = { 1.0f,1.0f,1.0f,1.0f };
    VertexList[0].Texture = { 0.0f, 0.0f };
    VertexList[1].Texture = { 1.0f, 0.0f };
    VertexList[2].Texture = { 0.0f, 1.0f };
    VertexList[3].Texture = { 1.0f, 1.0f };
}

void BaseObject::SetIndexList()
{
    IndexList[0] = 0;
    IndexList[1] = 1;
    IndexList[2] = 2;
    IndexList[3] = 2;
    IndexList[4] = 1;
    IndexList[5] = 3;
}


void BaseObject::SetListSize()
{
    VertexList.resize(4);
    IndexList.resize(6);
}

void BaseObject::SetSRV(ID3D11ShaderResourceView* srv)
{
    Mesh.TextureData->TextureSRV = srv;
}


void BaseObject::SetMask(std::wstring MaskFile)
{
    MaskTexture = TextureMgr.Load(MaskFile);
}

bool BaseObject::Init()
{
    return true;
}

bool BaseObject::Frame()
{
    return true;
}

bool BaseObject::Render()
{
    Mesh.Render();
    return true;
}

bool BaseObject::Release()
{
    Mesh.Release();
    return true;
}

bool BaseObject::PreRender()
{
    Mesh.PreRender();
    return true;
}

bool BaseObject::PostRender()
{
    Mesh.PostRender();
    return true;
}

void BaseObject::SetClientSize(RECT clientRt)
{
    ClientRect = clientRt;
}

HRESULT BaseObject::ShaderCompile(std::wstring ShaderFileName)
{
    HRESULT hr;
    ShaderFile = ShaderMgr.Load(ShaderFileName);
    if (ShaderFile != nullptr) 
    {
        hr = S_OK;
    }
    else
    {
        hr = S_FALSE;
    }
    return hr;
}

HRESULT BaseObject::LoadTexture(std::wstring TextureFileName)
{
    HRESULT hr;
    TextureFile = TextureMgr.Load(TextureFileName);
    if (TextureFile != nullptr) 
    {
        Mesh.TextureSRV = TextureFile->TextureSRV;
        hr = S_OK;
    }
    else 
    {
        hr = S_FALSE;
    }
    return hr;
}
