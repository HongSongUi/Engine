#pragma once
#include "MathLib.h"
#include "Collision.h"
enum H_POSITION {
	_BACK = 0,
	_FRONT,
	_ONPLANE,
	_SPANNING,
};
struct FPlane 
{
	float a, b, c, d;
	Vector3 PlaneVec;
	void Create(Vector3 v0, Vector3 v1, Vector3 v2) 
	{
		Vector3 p1 = v1 - v0;
		Vector3 p2 = v2 - v0;
		Vector3 normal;
		D3DXVec3Cross(&normal, &p1, &p2);
		D3DXVec3Normalize(&normal, &normal);
		a = normal.x;
		b = normal.y;
		c = normal.z;
		d = -D3DXVec3Dot(&normal, &v0);
		PlaneVec.x = a;
		PlaneVec.y = b;
		PlaneVec.z = c;
	}
	void Create(Vector3 n, Vector3 v0) 
	{
		D3DXVec3Normalize(&n, &n);
		a = n.x;
		b = n.y;
		c = n.z;
		d = -D3DXVec3Dot(&n, &v0);
	}
};
class Frustum
{
	Matrix ViewMat;
	Matrix ProjMat;
public:
	Vector3 FrustArray[8];
	FPlane PlaneArray[6];
public:
	void CreateFrustum(Matrix* view, Matrix* proj);
	bool ClassifyPoint(Vector3 v);
	H_POSITION ClassifyBox(Box obb);
};

