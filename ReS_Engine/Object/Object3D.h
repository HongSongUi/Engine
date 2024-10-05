#pragma once
#include "BaseObject.h"
class Object3D : public BaseObject
{
public:
	Matrix WorldMat;
	Matrix ViewMat;
	Matrix ProjMat;

	Vector4 LightDir;
	Vector4 LightPos;
	Vector4 EyeDir;
	Vector4 EyePos;
public:
	Vector3 Position = { 0.0f,0.0f,0.0f };
	Vector3 Scale = { 1.0f , 1.0f , 1.0f };
	Vector3 Rotation = { 0.01f , 0.01f , 0.01f };
	Quaternion Quater;

	Vector3 Axis[3];

	Vector3 Max;
	Vector3 Min;

	Vector3 PreAxis[3];

	Box ObjectBox;
public:
	Object3D() { SetListSize(); };
	virtual ~Object3D() {};
public:
	void SetMatrix(Matrix* world, Matrix* view, Matrix* proj);
	virtual void SetListSize() override;
	virtual bool Render()override;
	virtual bool Release()override;
	virtual void UpdatePosition(Vector3& pos);
	virtual void UpdateScale(Vector3& scale);
};

