#include "DebugCamera.h"
#include "../Utility/Input.h"
void DebugCamera::CreateViewMatrix(Vector3 pos, Vector3 target, Vector3 up)
{
	// Create View Matrix: Set Camera Location, Target, Upward Vector
    Position = pos; 
    Target = target; 
    UpVec = up; 
    
    Pitch = Target.y; //Set Y-axis rotation angle (pitch)

    D3DXMatrixLookAtLH(&ViewMat, &Position, &Target, &UpVec); //Creating a View Matrix 
    Update();// Update camera with set value
}

void DebugCamera::CreateProjMatrix(float n, float f, float FoVy, float aspect)
{
	// Create projection matrix: Set near clip, far clip, and screen ratio (FoV, aspect ratio)
    Near = n;
    Far = f;
    Fov = FoVy;
    Aspect = aspect;
    D3DXMatrixPerspectiveFovLH(&ProjMat, Fov, Aspect, Near, Far); //Creating a Projection Matrix
}

void DebugCamera::UpdateProjMatrix(float aspect)
{
    Aspect = aspect;
    D3DXMatrixPerspectiveFovLH(&ProjMat, Fov, Aspect, Near, Far);
}

bool DebugCamera::Frame()
{
	if (GameInput.GetKey(VK_RBUTTON) == KEY_HOLD) {
		Yaw += GameInput.OffSet.x * 0.002f;
		Pitch += GameInput.OffSet.y * 0.002f;
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
	}

	// Update camera position vector

	Vector3 p;
	p.x = Position.x;
	p.y = Position.y;
	p.z = Position.z;
	Matrix matWorld;
	Matrix matView;
	Matrix matRotation;
	Quaternion m_qRotation;

	D3DXQuaternionRotationYawPitchRoll(&m_qRotation, Yaw, Pitch, Roll);
	D3DXMatrixAffineTransformation(&matWorld, 1.0f, NULL, &m_qRotation, &p);
	D3DXMatrixInverse(&matView, NULL, &matWorld);
	ViewMat = matView;

	Update();

	// Create camera flustam using view matrix and projection matrix
	CameraFrustum.CreateFrustum(&ViewMat, &ProjMat);

    return true;
}

