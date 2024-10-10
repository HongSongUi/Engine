#include "Picking.h"

void Picking::SetWindowData(HWND hwnd, RECT clientRt)
{
	//init Hwnd, Window Client Size
    Hwnd = hwnd;
    ClientRt = clientRt;
}

void Picking::SetMatrix(Matrix* pWorld, Matrix* pView, Matrix* pProj)
{
	if (pWorld != NULL)
	{
		MatWorld = *pWorld;
		Center.x = pWorld->_41;
		Center.y = pWorld->_42;
		Center.z = pWorld->_43;
	}
	if (pView != NULL)
	{
		MatView = *pView;
	}
	if (pProj != NULL)
	{
		MatProj = *pProj;
	}
	Update();
}

void Picking::Update()
{
	POINT ptCursor;
	GetCursorPos(&ptCursor);// Get the current position of the cursor in screen coordinates
	ScreenToClient(Hwnd, &ptCursor);// Convert the screen coordinates to client coordinates

	Vector3 v;
	// Calculate the normalized device coordinates (NDC) from the cursor position
	v.x = (((2.0f * ptCursor.x) / ClientRt.right) - 1) / MatProj._11;
	v.y = -(((2.0f * ptCursor.y) / ClientRt.bottom) - 1) / MatProj._22;
	v.z = 1.0f;


	Matrix mWorldView = MatWorld * MatView;// Combine the world and view matrices to get the world-view matrix
	Matrix m;
	D3DXMatrixInverse(&m, NULL, &mWorldView); // Calculate the inverse of the world-view matrix

	Ray.Origin = Vector3(0.0f, 0.0f, 0.0f); // Set the ray's origin at the camera's position (0, 0, 0)
	Ray.Direction = Vector3(v.x, v.y, v.z);
	D3DXVec3TransformCoord(&Ray.Origin, &Ray.Origin, &m);// Transform the ray's origin from view space to world space
	D3DXVec3TransformNormal(&Ray.Direction, &Ray.Direction, &m);// Transform the ray's direction from view space to world space
	D3DXVec3Normalize(&Ray.Direction, &Ray.Direction);// Normalize the ray's direction vector

}

bool Picking::IntersectRayToSphere(Sphere* pSphere, MouseRay* pRay)
{
	if (pRay == NULL)
	{
		pRay = &Ray;
	}
	Vector3 dir = pRay->Origin - pSphere->Center;// Calculate the vector from the sphere's center to the ray's origin

	float proj = D3DXVec3Dot(&pRay->Direction, &dir);// Project the ray's direction onto the 'dir' vector

	if (pRay->Extent > -1.0f && pRay->Extent < fabs(proj))
	{
		return false;
	}

	float b = 2.0f * proj;
	float c = D3DXVec3Dot(&dir, &dir) - (pSphere->Radius * pSphere->Radius);

	float discriminat = (b * b) - (4.0f * c);

	if (discriminat < 0.0f)
	{
		return false;
	}
	discriminat = sqrtf(discriminat);
	float t0 = (-b + discriminat) / 2.0f;
	float t1 = (-b - discriminat) / 2.0f;

	if (t0 >= 0.0f)
	{
		InterSection = pRay->Origin + pRay->Direction * t0;
		return true;
	}
	if (t1 >= 0.0f)
	{
		InterSection = pRay->Origin + pRay->Direction * t1;
		return true;
	}
	return false;
}

bool Picking::ChkOBBToRay(Box* pBox, MouseRay* pRay)
{
	if (pRay == nullptr)
	{
		pRay = &Ray;
	}
	float _min = -999999.0f;
	float _max = 999999.0f;
	float f[3], fa[3], s[3], sa[3];

	Vector3 R = pRay->Origin - pBox->Center;

	for (int v = 0; v < 3; v++)
	{
		f[v] = D3DXVec3Dot(&pBox->Axis[v], &pRay->Direction);
		s[v] = D3DXVec3Dot(&pBox->Axis[v], &R);
		fa[v] = fabs(f[v]);
		sa[v] = fabs(s[v]);

		if (sa[v] > pBox->Extent[v] && s[v] * f[v] >= 0.0f)
		{
			return false;
		}
		float t1 = (-s[v] - pBox->Extent[v]) / f[v];
		float t2 = (-s[v] + pBox->Extent[v]) / f[v];

		if (t1 > t2)
		{
			std::swap(t1, t2);
		}
		_min = max(_min, t1);
		_max = min(_max, t2);
		if (_min > _max)
		{
			return false;
		}
	}
	float cross[3], Rhs;
	Vector3 DxR;
	D3DXVec3Cross(&DxR, &pRay->Direction, &R);

	cross[0] = fabs(D3DXVec3Dot(&DxR, &pBox->Axis[0]));
	float axis2 = pBox->Extent[1] * fa[2];
	float axis1 = pBox->Extent[2] * fa[1];
	Rhs = axis2 + axis1;
	if (cross[0] > Rhs)
	{
		CrossProduct = DxR;
		return false;
	}
	cross[1] = fabs(D3DXVec3Dot(&DxR, &pBox->Axis[1]));

	Rhs = pBox->Extent[0] * fa[2] + pBox->Extent[2] * fa[0];

	if (cross[1] > Rhs)
	{
		CrossProduct = DxR;
		return false;
	}
	cross[2] = fabs(D3DXVec3Dot(&DxR, &pBox->Axis[2]));
	Rhs = pBox->Extent[0] * fa[1] + pBox->Extent[1] * fa[0];
	if (cross[2] > Rhs)
	{
		CrossProduct = DxR;
		return false;
	}
	InterSection = pRay->Origin + pRay->Direction * _min;
	return true;
}

bool Picking::IntersectBox(Box* pBox, MouseRay* pRay)
{
	if (pRay == nullptr)
	{
		pRay = &Ray;
	}
	Vector3 _min;
	_min.x = (pBox->Min.x - pRay->Origin.x) / pRay->Direction.x + 0.001f;//epsilon
	_min.y = (pBox->Min.y - pRay->Origin.y) / pRay->Direction.y + 0.001f;
	_min.z = (pBox->Min.z - pRay->Origin.z) / pRay->Direction.z + 0.001f;

	Vector3 _max;
	_max.x = (pBox->Max.x - pRay->Origin.x) / pRay->Direction.x + 0.001f;
	_max.y = (pBox->Max.y - pRay->Origin.y) / pRay->Direction.y + 0.001f;
	_max.z = (pBox->Max.z - pRay->Origin.z) / pRay->Direction.z + 0.001f;

	Vector3 realMin;
	realMin.x = min(_min.x, _max.x);
	realMin.y = min(_min.y, _max.y);
	realMin.z = min(_min.z, _max.z);

	Vector3 realMax;
	realMax.x = max(_min.x, _max.x);
	realMax.y = max(_min.y, _max.y);
	realMax.z = max(_min.z, _max.z);

	float minmax = min(min(realMax.x, realMax.y), realMax.z);
	float maxmin = max(max(realMin.x, realMin.y), realMin.z);

	if (minmax >= maxmin)
	{
		InterSection = pRay->Origin + pRay->Direction * maxmin;
		return true;
	}
	return false;
}

bool Picking::AABBtoRay(Box* a, MouseRay* pRay)
{
	if (pRay == nullptr)
	{
		pRay = &Ray;
	}
	float _min = 0;
	float _max = 999999.0f;
	for (int i = 0; i < 3; i++)
	{
		if (abs(pRay->Direction[i]) < 0.0001f)
		{
			if (pRay->Origin[i] < a->Min[i] || pRay->Origin[i] > a->Max[i])
			{
				return false;
			}
		}
		else
		{
			float denom = 1.0f / pRay->Direction[i];
			float t1 = (a->Min[i] - pRay->Origin[i]) * denom;
			float t2 = (a->Max[i] - pRay->Origin[i]) * denom;
			if (t1 > t2)
			{
				std::swap(t1, t2);
			}
			_min = max(_min, t1);
			_max = min(_max, t2);
			if (_min > _max)
			{
				return false;
			}
		}
	}
	InterSection = pRay->Origin + pRay->Direction * _min;
	return true;
}

bool Picking::OBBtoRay(Box* a, MouseRay* pRay)
{
	if (pRay == nullptr)
	{
		pRay = &Ray;
	}
	float _min = -999999.0f;
	float _max = 999999.0f;

	Vector3 diff = a->Center - pRay->Origin;

	for (int v = 0; v < 3; v++)
	{
		float f = D3DXVec3Dot(&a->Axis[v], &pRay->Direction);
		float s = D3DXVec3Dot(&a->Axis[v], &diff);

		if (abs(pRay->Direction[v]) < 0.0001f)
		{
			if (-s - a->Extent[v] > 0 || -s + a->Extent[v] > 0)
			{
				return false;
			}
		}
		else
		{
			float t1 = (s - a->Extent[v]) / f;
			float t2 = (s + a->Extent[v]) / f;
			if (t1 > t2)
			{
				std::swap(t1, t2);
			}
			_min = max(_min, t1);
			_max = min(_max, t2);
			if (_min > _max)
			{
				return false;
			}
		}
	}
	InterSection = pRay->Origin + pRay->Direction * _min;
	return true;
}

Vector3 Picking::GetIntersection()
{
	return InterSection;
}

bool Picking::GetIntersection(Vector3 vStart, Vector3 vEnd, Vector3 vNormal, Vector3 v0, Vector3 v1, Vector3 v2, float* fPercentage)
{
	Vector3 vDirection = vEnd - vStart;
	float D = D3DXVec3Dot(&vNormal, &vDirection);
	Vector3 Temp = v0 - vStart;
	float a0 = D3DXVec3Dot(&vNormal, &Temp);
	float fT = a0 / D;
	if (fPercentage != nullptr)
	{
		*fPercentage = fT;
	}
	if (fT < 0.0f || fT > 1.0f)
	{
		return false;
	}
	InterSection = vStart + vDirection * fT;

	return true;
}

bool Picking::PointInPolygon(Vector3 vert, Vector3 faceNormal, Vector3 v0, Vector3 v1, Vector3 v2)
{
	Vector3 e0, e1, iInter, vNormal;

	e0 = v2 - v1;
	e1 = v0 - v1;
	iInter = vert - v1;
	D3DXVec3Cross(&vNormal, &e0, &iInter);
	D3DXVec3Normalize(&vNormal, &vNormal);
	float fDot = D3DXVec3Dot(&faceNormal, &vNormal);
	if (fDot < 0.0f) return false;

	D3DXVec3Cross(&vNormal, &iInter, &e1);
	D3DXVec3Normalize(&vNormal, &vNormal);
	fDot = D3DXVec3Dot(&faceNormal, &vNormal);

	if (fDot < 0.0f) return false;

	e0 = v0 - v2;
	e1 = v1 - v2;
	iInter = vert - v2;
	D3DXVec3Cross(&vNormal, &e0, &iInter);
	D3DXVec3Normalize(&vNormal, &vNormal);
	fDot = D3DXVec3Dot(&faceNormal, &vNormal);
	if (fDot < 0.0f) return false;

	D3DXVec3Cross(&vNormal, &iInter, &e1);
	D3DXVec3Normalize(&vNormal, &vNormal);
	fDot = D3DXVec3Dot(&faceNormal, &vNormal);
	if (fDot < 0.0f) return false;
	return true;
}

bool Picking::ChkPick(Vector3& v0, Vector3& v1, Vector3& v2)
{

	FLOAT fBary1, fBary2;
	FLOAT fDist;
	// Check if the pick ray passes through this point
	if (IntersectTriangle(Ray.Origin, Ray.Direction, v0, v1, v2,
		&fDist, &fBary1, &fBary2))
	{
		SrcVex[0] = v0;
		SrcVex[1] = v1;
		SrcVex[2] = v2;
		MatWorldPick = MatWorld;
		PickDistance = fDist;
		InterSection = Ray.Origin + Ray.Direction * fDist;
		return true;
	}
	return false;
}

bool Picking::IntersectTriangle(const Vector3& orig, const Vector3& dir, Vector3& v0, Vector3& v1, Vector3& v2, FLOAT* t, FLOAT* u, FLOAT* v)
{
	Vector3 edge1 = v1 - v0;
	Vector3 edge2 = v2 - v0;

	D3DXVec3Cross(&pVec, &dir, &edge2);

	FLOAT det = D3DXVec3Dot(&edge1, &pVec);

	if (det > 0)
	{
		tVec = orig - v0;
	}
	else
	{
		tVec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
	{
		return false;
	}
		

	*u = D3DXVec3Dot(&tVec, &pVec);
	if (*u < 0.0f || *u > det)
	{
		return false;
	}
		

	D3DXVec3Cross(&qVec, &tVec, &edge1);


	*v = D3DXVec3Dot(&dir, &qVec);
	if (*v < 0.0f || *u + *v > det)
	{
		return false;
	}

	*t = D3DXVec3Dot(&edge2, &qVec);
	FLOAT fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}

Picking::Picking(void)
{
	SrcVex[0] = Vector3(0.0f, 0.0f, 0.0f);
	SrcVex[1] = Vector3(0.0f, 0.0f, 0.0f);
	SrcVex[2] = Vector3(0.0f, 0.0f, 0.0f);
	PickDistance = 10000.0f;

	InterSection = Vector3(0, 0, 0);
	Center = Vector3(0, 0, 0);
	CrossProduct = Vector3(1, 1, 1);
	D3DXMatrixIdentity(&MatWorld);
	D3DXMatrixIdentity(&MatView);
	D3DXMatrixIdentity(&MatProj);
}

Picking::~Picking(void)
{
}
