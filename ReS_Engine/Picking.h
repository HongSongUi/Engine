#pragma once
#include "Collision.h"
class Picking
{
public:
	MouseRay Ray;
	Vector3 InterSection;
	Matrix MatWorld;
	Matrix MatView;
	Matrix MatProj;
	Vector3 CrossProduct;
	Vector3 Center;
	Vector3 SrcVex[3];
	float PickDistance;
	Matrix MatWorldPick;
	Vector3 qVec;
	Vector3 tVec;
	Vector3 pVec;
	HWND Hwnd;
	RECT ClientRt;
public:
	void SetWindowData(HWND hwnd, RECT clientRt);
	void SetMatrix(Matrix* pWorld, Matrix* pView, Matrix* pProj);
	void Update();
	bool IntersectRayToSphere(Sphere* pSphere, MouseRay* pRay = NULL);
	bool ChkOBBToRay(Box* pBox, MouseRay* pRay = NULL);
	bool IntersectBox(Box* pBox, MouseRay* pRay = NULL);
	bool AABBtoRay(Box* a, MouseRay* pRay = NULL);
	bool OBBtoRay(Box* a, MouseRay* pRay = NULL);
	Vector3 GetIntersection();
	bool    GetIntersection(Vector3 vStart, Vector3 vEnd, Vector3 vNormal, Vector3 v0, Vector3 v1, Vector3 v2, float* fPercentage = nullptr);
	bool    PointInPolygon(Vector3 vert, Vector3 faceNormal, Vector3 v0, Vector3 v1, Vector3 v2);
	bool	ChkPick(Vector3& v0, Vector3& v1, Vector3& v2);
	bool	IntersectTriangle(const Vector3& orig, const Vector3& dir, Vector3& v0, Vector3& v1, Vector3& v2, FLOAT* t, FLOAT* u, FLOAT* v);
public:
	Picking(void);
	virtual ~Picking(void);
};

