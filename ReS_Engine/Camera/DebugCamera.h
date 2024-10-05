#pragma once
#include "Camera3D.h"
class DebugCamera : public Camera3D
{
public:
	virtual void CreateViewMatrix(Vector3 pos, Vector3 target, Vector3 up)override;
	virtual void CreateProjMatrix(float n, float f, float FoVy, float aspect)override;
	virtual void UpdateProjMatrix(float aspect)override;
	virtual bool Frame() override;

};

