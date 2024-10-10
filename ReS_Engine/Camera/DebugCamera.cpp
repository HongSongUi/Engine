#include "DebugCamera.h"
#include "../Utility/Input.h"
void DebugCamera::CreateViewMatrix(Vector3 pos, Vector3 target, Vector3 up)
{
	// «Ó«å?ú¼ÖªªÎíÂà÷: «««á«éªÎêÈöÇ¡¢ñ¼ãÊïÃ¡¢ß¾Û°ú¾«Ù«¯«È«ëªòàâïÒ
    Position = pos; 
    Target = target; 
    UpVec = up; 
    
    Pitch = Target.y; // YõîªòĞññŞªËª·ª¿üŞ?ÊÇÓøªòàâïÒ («Ô«Ã«Á)

    D3DXMatrixLookAtLH(&ViewMat, &Position, &Target, &UpVec); //«Ó«å?ú¼ÖªªÎíÂà÷
    Update();// àâïÒªµªìª¿ö·ªÇ«««á«éªòÌÚãæ
}

void DebugCamera::CreateProjMatrix(float n, float f, float FoVy, float aspect)
{
	// ŞÒç¯ú¼ÖªªÎíÂà÷: ĞÎ«¯«ê«Ã«×Øü¡¢êÀ«¯«ê«Ã«×Øü¡¢?Øüİïáã(FoV¡¢«¢«¹«Ú«¯«Èİï)ªòàâïÒ
    Near = n;
    Far = f;
    Fov = FoVy;
    Aspect = aspect;
    D3DXMatrixPerspectiveFovLH(&ProjMat, Fov, Aspect, Near, Far); //ŞÒç¯ú¼ÖªªÎíÂà÷
}

void DebugCamera::UpdateProjMatrix(float aspect)
{
	// «¢«¹«Ú«¯«ÈİïªÎÌÚãæ
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

	// «««á«éªÎêÈöÇ«Ù«¯«È«ëªòÌÚãæ

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

	// «Ó«å?ú¼ÖªªÈŞÒç¯ú¼ÖªªòŞÅªÃªÆ«««á«éªÎ«Õ«é«¹«¿«àªòíÂà÷
	CameraFrustum.CreateFrustum(&ViewMat, &ProjMat);

    return true;
}

