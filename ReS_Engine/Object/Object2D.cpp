#include "Object2D.h"
#include "../Manager/TextureManager.h"
bool Object2D::Frame()
{

	return true;
}

void Object2D::SetVertexList()
{
    if (Inverse == true)
    {
        VertexList[0].Position = { NdcPos.x - NdcSize.x ,NdcPos.y + NdcSize.y,0.0f };
        VertexList[0].Texture = { UvRect.Min.x, UvRect.Min.y };

        VertexList[1].Position = { NdcPos.x + NdcSize.x , NdcPos.y + NdcSize.y,0.0f };
        VertexList[1].Texture = { UvRect.Max.x , UvRect.Min.y };

        VertexList[2].Position = { NdcPos.x - NdcSize.x,NdcPos.y - NdcSize.y ,0.0f };
        VertexList[2].Texture = { UvRect.Min.x, UvRect.Max.y };

        VertexList[3].Position = { NdcPos.x + NdcSize.x , NdcPos.y - NdcSize.y,0.0f };
        VertexList[3].Texture = { UvRect.Max.x , UvRect.Max.y };
    }
    else
    {
        VertexList[0].Position = { NdcPos.x - NdcSize.x ,NdcPos.y + NdcSize.y,0.0f };
        VertexList[0].Texture = { UvRect.Max.x, UvRect.Min.y };

        VertexList[1].Position = { NdcPos.x + NdcSize.x , NdcPos.y + NdcSize.y,0.0f };
        VertexList[1].Texture = { UvRect.Min.x , UvRect.Min.y };

        VertexList[2].Position = { NdcPos.x - NdcSize.x,NdcPos.y - NdcSize.y ,0.0f };
        VertexList[2].Texture = { UvRect.Max.x,UvRect.Max.y };

        VertexList[3].Position = { NdcPos.x + NdcSize.x , NdcPos.y - NdcSize.y,0.0f };
        VertexList[3].Texture = { UvRect.Min.x , UvRect.Max.y };
    }
}

void Object2D::UpdateVertexList()
{
    SetVertexList();
    Mesh.SetVertexList(VertexList);
    Mesh.UpdateVertexBuffer();
}

void Object2D::Hit(Object2D& HitObject , float Damage)
{
 
}


void Object2D::SetPosition(Vector2 pos)
{
    WorldPos = pos;
	Size.x = TextureRt.Size.x;
	Size.y = TextureRt.Size.y;
	ObjectRect.SetRect(WorldPos, Size);

}

void Object2D::ScreenToNdc()
{
    NdcPos.x = (WorldPos.x / ClientRect.right) * 2.0f - 1.0f;
	NdcPos.y = -((WorldPos.y / ClientRect.bottom) * 2.0f - 1.0f);
	NdcSize.x = (TextureRt.Size.x / ClientRect.right) * 2.0f;
	NdcSize.y = (TextureRt.Size.y / ClientRect.bottom) * 2.0f;
}

void Object2D::ScreenToView(Vector2 cam_pos, Vector2 cam_size)
{
    CamPos = WorldPos;
    CamPos.x = CamPos.x - cam_pos.x;
    CamPos.y = CamPos.y - cam_pos.y;
	NdcPos.x = (CamPos.x / cam_size.x) * 2.0f;
	NdcPos.y = -((CamPos.y / cam_size.y) * 2.0f);
	NdcSize.x = (TextureRt.Size.x / cam_size.x) *2.0f;
	NdcSize.y = (TextureRt.Size.y / cam_size.y) * 2.0f;
}

void Object2D::SetPhysics()
{
}

void Object2D::SetRect(Rect rt)
{
    TextureRt = rt;
	TextureSize.x = TextureFile->Desc.Width;
    TextureSize.y = TextureFile->Desc.Height;
	UvRect.Min.x = rt.Min.x / TextureSize.x;
	UvRect.Min.y = rt.Min.y / TextureSize.y;
	UvRect.Max.x = rt.Max.x / TextureSize.x;
	UvRect.Max.y = rt.Max.y / TextureSize.y;
}

void Object2D::ShowObject()
{
    ScreenToNdc();
    UpdateVertexList();
}

void Object2D::MaskRender()
{
    PreRender();
    D3D11Context->PSSetShaderResources(1, 1, &MaskTexture->TextureSRV);
    PostRender();
}

void Object2D::CameraSet(Vector2 CamPos, Vector2 CamSize)
{
    ScreenToView(CamPos, CamSize);
    UpdateVertexList();
}

void Object2D::GetDamage(float damage)
{
    return;
}

Rect Object2D::GetObjectRect()
{
    return ObjectRect;
}
