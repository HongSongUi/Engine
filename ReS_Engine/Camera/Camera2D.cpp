#include "Camera2D.h"

bool Camera2D::Init()
{
    return true;
}

bool Camera2D::Frame()
{
    return true;
}

bool Camera2D::Render()
{
    return true;
}

bool Camera2D::Release()
{
    return true;
}

void Camera2D::SetData(Vector2 Pos, Vector2 CamSize)
{
    //Initialize Camera Data 
    Position = Pos;
    CameraSize = CamSize;
    Vector2 temp = CameraSize;
    temp /= 2.0f;
    Min = Position - temp;
    Max = Position + temp;
}
