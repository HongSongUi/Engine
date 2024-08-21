#include "Collision.h"
#include "Object3D.h"
Rect::Rect()
{

}
Rect::Rect(Vector2 cen, Vector2 size)
{
	SetRect(cen, size);
}

Rect::Rect(float left, float top, float right, float bottom)
{
	SetRect(left, top, right, bottom);
}

void Rect::SetRect(Vector2 cen, Vector2 size)
{
	Size = size;
	Vector2 tmp = Size;
	tmp /= 2.f;
	Center = cen;

	Min = Center - tmp;
	Max = Center + tmp;
}

void Rect::SetRect(float left, float top, float right, float bottom)
{
	float cenX = (right + left) / 2.0f;
	float cenY = (bottom + top) / 2.0f;
	float sizeX = right - left;
	float sizeY = bottom - top;
	SetRect({ cenX,cenY }, { sizeX,sizeY });
}

bool Rect::operator==(Rect& r)
{
	if (fabs(Min.x - r.Min.x) < CalcEpsilon)
	{
		if (fabs(Min.y - r.Min.y) < CalcEpsilon)
		{
			if (fabs(Max.x - r.Max.x) < CalcEpsilon)
			{
				if (fabs(Max.y - r.Max.y) < CalcEpsilon)
				{
					return true;
				}
			}
		}
	}
	return false;
}

Sphere::Sphere()
{
}

Sphere::Sphere(Vector3 cen, float rad)
{
	Center = cen;
	Radius = rad;
}

Box::Box()
{
}

Box::Box(Vector3 max, Vector3 min)
{
	SetBox(max, min);
}

void Box::SetBox(Vector3 max, Vector3 min)
{
	Max = max;
	Min = min;
	Center = (Max + Min) * 0.5f;
	Axis[0] = { 1,0,0 };
	Axis[1] = { 0,1,0 };
	Axis[2] = { 0,0,1 };
	Extent[0] = Max.x - Center.x;
	Extent[1] = Max.y - Center.y;
	Extent[2] = Max.z - Center.z;
}

void Box::SetParent(Object3D* parent)
{
	Parent = parent;
}


bool Collision::RectToRect(Rect& a, Rect& b)
{
	float MinX;
	float MinY;
	float MaxX;
	float MaxY;

	MinX = a.Min.x > b.Min.x ? b.Min.x : a.Min.x;
	MinY = a.Min.y > b.Min.y ? b.Min.y : a.Min.y;
	MaxX = a.Max.x > b.Max.x ? a.Max.x : b.Max.x;
	MaxY = a.Max.y > b.Max.y ? a.Max.y : b.Max.y;

	if ((a.Size.x + b.Size.x) >= (MaxX - MinX)) 
	{
		if ((a.Size.y + b.Size.y) >= (MaxY - MinY)) 
		{
			Rect tmp;
			float x1, y1, x2, y2;
			x1 = a.Min.x > b.Min.x ? a.Min.x : b.Min.x;
			y1 = a.Min.y > b.Min.y ? a.Min.y : b.Min.y;
			x2 = a.Max.x > b.Max.x ? b.Max.x : a.Max.x;
			y2 = a.Max.y > b.Max.y ? b.Max.y : a.Max.y;
			tmp.SetRect(x1, y1, x2, y2);
			if (tmp == a || tmp == b)
			{
				return true;
			}
			return true;
		} 
	}
	return false;
}

HCollisionType Collision::BoxToBox(Box& a, Box& b)
{
	float fMinX;    float fMaxX;
	float fMinY;    float fMaxY;
	float fMinZ;    float fMaxZ;
	fMinX = a.Min.x < b.Min.x ? a.Min.x : b.Min.x;
	fMinY = a.Min.y < b.Min.y ? a.Min.y : b.Min.y;
	fMaxX = a.Max.x > b.Max.x ? a.Max.x : b.Max.x;
	fMaxY = a.Max.y > b.Max.y ? a.Max.y : b.Max.y;

	fMinZ = a.Min.z < b.Min.z ? a.Min.z : b.Min.z;
	fMaxZ = a.Max.z > b.Max.z ? a.Max.z : b.Max.z;

	Vector3 vSize;
	vSize.x = (a.Max.x - a.Min.x) + (b.Max.x - b.Min.x);
	vSize.y = (a.Max.y - a.Min.y) + (b.Max.y - b.Min.y);
	vSize.z = (a.Max.z - a.Min.z) + (b.Max.z - b.Min.z);

	//  가로 판정
	if (vSize.x >= (fMaxX - fMinX))
	{        //  세로 판정
		if (vSize.y >= (fMaxY - fMinY))
		{
			if (vSize.z >= (fMaxZ - fMinZ))
			{
				// 교차한다. 교집합
				Vector3 vMin, vMax;
				Box Intersect;
				vMin.x = a.Min.x > b.Min.x ? a.Min.x : b.Min.x;
				vMin.y = a.Min.y > b.Min.y ? a.Min.y : b.Min.y;
				vMin.z = a.Min.z > b.Min.z ? a.Min.z : b.Min.z;

				vMax.x = a.Max.x < b.Max.x ? a.Max.x : b.Max.x;
				vMax.y = a.Max.y < b.Max.y ? a.Max.y : b.Max.y;
				vMax.z = a.Max.z < b.Max.z ? a.Max.z : b.Max.z;

				if (BoxToInBox(a, b))
				{
					return HCollisionType::RECT_IN;
				}
				return HCollisionType::RECT_OVERLAP;
			}
		}
	}
	return HCollisionType::RECT_OUT;
}

bool Collision::BoxToInBox(Box& a, Box& b)
{
	if (a.Min.x <= b.Min.x &&
		a.Min.y <= b.Min.y &&
		a.Min.z <= b.Min.z)
	{
		if (a.Max.x >= b.Max.x &&
			a.Max.y >= b.Max.y &&
			a.Max.z >= b.Max.z)
		{
			return true;
		}
	}
	return false;
}

bool Collision::BoxToPosition(Box& a, Vector3& p)
{
	if (a.Min.x <= p.x &&
		a.Min.y <= p.y &&
		a.Min.z <= p.z)
	{
		if (a.Max.x >= p.x &&
			a.Max.y >= p.y &&
			a.Max.z >= p.z)
		{
			return true;
		}
	}
	return false;
}

bool Collision::SphereToSphere(Sphere& a, Sphere& b)
{
	float SumRadius = a.Radius + b.Radius;
	Vector3 Dir = a.Center - b.Center;
	float Distance = Dir.Length();
	if (Distance <= SumRadius)
	{
		return true;
	}
	return false;
}
