#include "Camera3D.h"
#include "Input.h"
bool Camera3D::Init()
{
    return true;
}

bool Camera3D::Frame()
{
	if (GameInput.GetKey('W') == KEY_HOLD)
	{
		Vector3 v = LookVec * 10.0f * gSecondPerFrame;
		Position = Position + v;
	}
	if (GameInput.GetKey('S') == KEY_HOLD)
	{
		Vector3 v = LookVec * -10.0f * gSecondPerFrame;
		Position = Position + v;
	}
	if (GameInput.GetKey('A') == KEY_HOLD)
	{
		Vector3 v = RightVec * -10.0f * gSecondPerFrame;
		Position = Position + v;
	}
	if (GameInput.GetKey('D') == KEY_HOLD)
	{
		Vector3 v = RightVec * 10.0f * gSecondPerFrame;
		Position = Position + v;
	}
	if (GameInput.GetKey('Q') == KEY_HOLD)
	{
		Vector3 v = UpVec * 10.0f * gSecondPerFrame;
		Position = Position + v;
	}
	if (GameInput.GetKey('E') == KEY_HOLD)
	{
		Vector3 v = UpVec * -10.0f * gSecondPerFrame;
		Position = Position + v;
	}

	Vector3 vUp = { 0,1,0 };
	//TMatrix mCamera = TMath::RotationY(g_fGameTimer);
	//vPos = (vPos + vPosMovement) * mCamera;
	D3DXMatrixLookAtLH(&ViewMat, &Position, &Target, &UpVec);

	Update();
    return true;
}

bool Camera3D::Render()
{
    return true;
}

bool Camera3D::Release()
{
    return true;
}

void Camera3D::Update()
{
	RightVec.x = ViewMat._11;
	RightVec.y = ViewMat._21;
	RightVec.z = ViewMat._31;

	UpVec.x = ViewMat._12;
	UpVec.y = ViewMat._22;
	UpVec.z = ViewMat._32;

	LookVec.x = ViewMat._13;
	LookVec.y = ViewMat._23;
	LookVec.z = ViewMat._33;

	D3DXVec3Normalize(&RightVec, &RightVec);
	D3DXVec3Normalize(&UpVec, &UpVec);
	D3DXVec3Normalize(&LookVec, &LookVec);
}

void Camera3D::CreateViewMatrix(Vector3 pos, Vector3 target, Vector3 up)
{
    Position = pos;
    Target = target;
    UpVec = up;
    D3DXMatrixLookAtLH(&ViewMat, &Position, &Target, &UpVec);
    Update();
}

void Camera3D::CreateProjMatrix(float near_val, float far_val, float fov, float aspect)
{
    Near = near_val;
    Far = far_val;
    Fov = fov;
    Aspect = aspect;
    D3DXMatrixPerspectiveFovLH(&ProjMat, Fov, Aspect, Near, Far);
}

void Camera3D::UpdateProjMatrix(float aspect)
{
    Aspect = aspect;
    D3DXMatrixPerspectiveFovLH(&ProjMat, Fov, Aspect, Near, Far);
}


