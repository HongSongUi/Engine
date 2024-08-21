#pragma once
#include "DefaultCamera.h"
#include "Frustum.h"
class Camera3D : public DefaultCamera
{

public:

	Vector3 Position;
	Vector3 Target = { 0.f,0.f,0.f };
	Vector3 UpVec = { 0.f,1.f,0.f };
	Vector3 RightVec;
	Vector3 LookVec;

	Matrix ViewMat;
	Matrix ProjMat;
	Frustum CameraFrustum;
	float Near;
	float Far;
	float Fov;
	float Aspect;

	float Yaw = 0.f;
	float Pitch = 0.f;
	float Roll = 0.f;


public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;

	virtual void Update();
	virtual void CreateViewMatrix(Vector3 pos, Vector3 target, Vector3 up);
	virtual void CreateProjMatrix(float near_val, float far_val, float fov, float aspect);
	virtual void UpdateProjMatrix(float aspect);
};

