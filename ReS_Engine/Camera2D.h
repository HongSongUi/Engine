#pragma once
#include "DefaultCamera.h"
class Camera2D : public DefaultCamera
{
public:
	Vector2 Position;
	Vector2 CameraSize;
	Vector2 Min;
	Vector2 Max;
public:
	virtual bool Init() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;
public:
	void SetData(Vector2 Pos, Vector2 CamSize);
};

