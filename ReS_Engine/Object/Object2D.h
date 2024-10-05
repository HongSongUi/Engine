#pragma once
#include "BaseObject.h"
struct SpriteRect
{
	float Left;
	float Top;
	float Right;
	float Bottom;
};
class Object2D : public BaseObject
{
private:
	Vector2 NdcPos;
	Vector2 NdcSize;
public:
	Rect TextureRt;
	Rect UvRect;
	Rect ShowRect;
	POINT TextureSize;
public:
	Rect ObjectRect;

	Vector2 Dir;
	Vector2 Size;
public:
	Vector2 WorldPos;
	Vector2 CamPos;
	Vector2 CamSize;
public:
	Object2D() {
		SetPhysics();
	}
	virtual ~Object2D() {};
public:
	virtual bool Frame()override;
	virtual void SetVertexList()override ;
	virtual void UpdateVertexList() override;
	virtual void Hit(Object2D& HitObject, float Damage);
	void SetPosition(Vector2 pos) override;
	void ScreenToNdc();
	void ScreenToView(Vector2 CamPos, Vector2 CamSize);
	virtual void SetPhysics() override ;
	void SetRect(Rect rt) ;
	virtual void ShowObject();
	virtual void MaskRender();
	virtual void CameraSet(Vector2 CamPos, Vector2 CamSize);
	virtual void GetDamage(float damage);
	virtual Rect GetObjectRect();
};

