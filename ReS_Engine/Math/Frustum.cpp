#include "Frustum.h"

void Frustum::CreateFrustum(Matrix* view, Matrix* proj)
{
	ViewMat = *view;
	ProjMat = *proj;
	Matrix X_View = *view;
	Matrix X_Proj = *proj;
	Matrix InViewProjMat = X_View * X_Proj;
	D3DXMatrixInverse(&InViewProjMat, NULL, &InViewProjMat);
	// _Frustum = v * world * (view * proj);
	// _Frustum = v * world * view*invView * proj * invproj;
	// _Frustum /= v.w     x,y= -1~+1,   z= 0~1;
	// 

	FrustArray[0] = Vector3(-1.0f, 1.0f, 0.0f);
	FrustArray[1] = Vector3(1.0f, 1.0f, 0.0f);
	FrustArray[2] = Vector3(-1.0f, -1.0f, 0.0f);
	FrustArray[3] = Vector3(1.0f, -1.0f, 0.0f);

	FrustArray[4] = Vector3(-1.0f, 1.0f, 1.0f);
	FrustArray[5] = Vector3(1.0f, 1.0f, 1.0f);
	FrustArray[6] = Vector3(-1.0f, -1.0f, 1.0f);
	FrustArray[7] = Vector3(1.0f, -1.0f, 1.0f);
	for (int ver = 0; ver < 8; ver++) 
	{
		D3DXVec3TransformCoord(&FrustArray[ver], &FrustArray[ver], &InViewProjMat);
	}

	PlaneArray[0].Create(FrustArray[0], FrustArray[2], FrustArray[1]);//near
	PlaneArray[1].Create(FrustArray[4], FrustArray[5], FrustArray[7]);//far
	PlaneArray[2].Create(FrustArray[4], FrustArray[6], FrustArray[2]);//left
	PlaneArray[3].Create(FrustArray[1], FrustArray[3], FrustArray[7]);//right
	PlaneArray[4].Create(FrustArray[0], FrustArray[1], FrustArray[5]);//top
	PlaneArray[5].Create(FrustArray[6], FrustArray[7], FrustArray[2]);//bottom
}

bool Frustum::ClassifyPoint(Vector3 v)
{
	for (int iPlane = 0; iPlane < 6; iPlane++)
	{
		float fDistance =
			PlaneArray[iPlane].a * v.x +
			PlaneArray[iPlane].b * v.y +
			PlaneArray[iPlane].c * v.z +
			PlaneArray[iPlane].d;
		if (fDistance < 0) return false;
	}
	return true;
}

H_POSITION Frustum::ClassifyBox(Box obb)
{
	float BoxToPlaneDis = 0.0f;
	float Distance = 0.0f;
	float DisX = 0.0f;
	float DisY = 0.0f;
	float DisZ = 0.0f;
	Vector3 Dir;

	H_POSITION Check = _FRONT;
	for (int p = 0; p < 6; p++) 
	{
		Dir = obb.Axis[0] * obb.Extent[0];
		DisX = fabs(D3DXVec3Dot(&PlaneArray[p].PlaneVec, &Dir));
		Dir = obb.Axis[1] * obb.Extent[1];
		DisY = fabs(D3DXVec3Dot(&PlaneArray[p].PlaneVec, &Dir));
		Dir = obb.Axis[2] * obb.Extent[2];
		DisZ = fabs(D3DXVec3Dot(&PlaneArray[p].PlaneVec, &Dir));

		Distance = DisX + DisY + DisZ;

		BoxToPlaneDis = D3DXVec3Dot(&PlaneArray[p].PlaneVec, &obb.Center) + PlaneArray[p].d;

		if (BoxToPlaneDis > 0) 
		{
			if (BoxToPlaneDis < Distance) 
			{
				Check = _SPANNING;
				break;
			}
		}
		else if (BoxToPlaneDis < 0) 
		{
			Check = _BACK;
			if (BoxToPlaneDis > -Distance) 
			{
				Check = _SPANNING;
			}
			break;
		}

	}
	return Check;
}
