#pragma once
#include "MathLib.h"

#define CalcEpsilon 0.0001f

enum HCollisionType
{
	RECT_OUT = 0,
	RECT_IN,
	RECT_OVERLAP,
};

struct Rect
{
public:
	Vector2 Center;
	Vector2 Min;
	Vector2 Max;
	Vector2 Size;
public:
	Rect();
	Rect(Vector2 cen, Vector2 size);
	Rect(float left, float top, float right, float bottom);
public:
	void SetRect(Vector2 cen, Vector2 size);
	void SetRect(float left, float top, float right, float bottom);
public:
	bool operator== (Rect& r);
};

struct Sphere
{
public:
	Vector3 Center;
	float Radius;
public:
	Sphere();
	Sphere(Vector3 cen, float rad);
};

struct Box
{
public:
	Vector3 Center;
	Vector3 Pos[8];

	Vector3 Max;
	Vector3 Min;

	Vector3 Axis[3];
	float Extent[3];
	class Object3D* Parent;
public:
	Box();
	Box(Vector3 max, Vector3 min);
	void SetBox(Vector3 max, Vector3 min);
	void SetParent(class Object3D* parent);
};

struct MouseRay
{
	float Extent;
	Vector3 Origin;
	Vector3 Direction;
	MouseRay()
	{
		Extent = -1;
	}
};

class Collision
{
public:
	static bool RectToRect(Rect& a, Rect& b);
public:
	static HCollisionType   BoxToBox(Box& a, Box& b);
	static bool             BoxToInBox(Box& a, Box& b);
	static bool             BoxToPosition(Box& a, Vector3& p);
	static bool             SphereToSphere(Sphere& a, Sphere& b);
};

