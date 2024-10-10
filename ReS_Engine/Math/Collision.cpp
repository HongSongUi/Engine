#include "Collision.h"
#include "../Object/Object3D.h"
Rect::Rect()
{

}
Rect::Rect(Vector2 cen, Vector2 size)
{
	SetRect(cen, size); // Create Rect use center vector and size vector
}

Rect::Rect(float left, float top, float right, float bottom)
{
	SetRect(left, top, right, bottom);// Create Rect use left, top, right, bottom
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
	if (fabs(Min.x - r.Min.x) < CalcEpsilon) //Judging that it's the same if it's less than epsilon
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
	// Set Sphere value
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
	Axis[0] = { 1,0,0 }; //x
	Axis[1] = { 0,1,0 }; //y
	Axis[2] = { 0,0,1 }; //z
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

	// Calculate the min and max coordinates for the overlapping area
	MinX = a.Min.x > b.Min.x ? b.Min.x : a.Min.x;
	MinY = a.Min.y > b.Min.y ? b.Min.y : a.Min.y;
	MaxX = a.Max.x > b.Max.x ? a.Max.x : b.Max.x;
	MaxY = a.Max.y > b.Max.y ? a.Max.y : b.Max.y;

	// Check for X overlap
	if ((a.Size.x + b.Size.x) >= (MaxX - MinX)) 
	{
		// Check for Y overlap
		if ((a.Size.y + b.Size.y) >= (MaxY - MinY)) 
		{
			Rect tmp;// Temporary Rect for overlap 
			float x1, y1, x2, y2;
			// Determine corners of the overlapping area
			x1 = a.Min.x > b.Min.x ? a.Min.x : b.Min.x;
			y1 = a.Min.y > b.Min.y ? a.Min.y : b.Min.y;
			x2 = a.Max.x > b.Max.x ? b.Max.x : a.Max.x;
			y2 = a.Max.y > b.Max.y ? b.Max.y : a.Max.y;
			tmp.SetRect(x1, y1, x2, y2);// Set temporary rectangle
			if (tmp == a || tmp == b)// Check if the overlap is exactly one of the rectangles
			{
				return true; // Return true for exact overlap
			}
			return true;// Return true for any overlap
		} 
	}
	return false;// Return false if no overlap
}

HCollisionType Collision::BoxToBox(Box& a, Box& b)
{
	float fMinX;    float fMaxX;
	float fMinY;    float fMaxY;
	float fMinZ;    float fMaxZ;
	// Calculate the min and max coordinates for the overlap
	fMinX = a.Min.x < b.Min.x ? a.Min.x : b.Min.x;
	fMinY = a.Min.y < b.Min.y ? a.Min.y : b.Min.y;
	fMaxX = a.Max.x > b.Max.x ? a.Max.x : b.Max.x;
	fMaxY = a.Max.y > b.Max.y ? a.Max.y : b.Max.y;

	fMinZ = a.Min.z < b.Min.z ? a.Min.z : b.Min.z;
	fMaxZ = a.Max.z > b.Max.z ? a.Max.z : b.Max.z;

	Vector3 vSize;// Size of the combined boxes
	vSize.x = (a.Max.x - a.Min.x) + (b.Max.x - b.Min.x);
	vSize.y = (a.Max.y - a.Min.y) + (b.Max.y - b.Min.y);
	vSize.z = (a.Max.z - a.Min.z) + (b.Max.z - b.Min.z);

	//  Check for horizontal overlap
	if (vSize.x >= (fMaxX - fMinX))
	{
		// Check for vertical overlap
		if (vSize.y >= (fMaxY - fMinY))
		{
			// Check for depth overlap 
			if (vSize.z >= (fMaxZ - fMinZ))
			{
				// There is an intersection
				Vector3 vMin, vMax;
				Box Intersect; // Intersection box 
				// Calculate the minimum corner of the intersection
				vMin.x = a.Min.x > b.Min.x ? a.Min.x : b.Min.x;
				vMin.y = a.Min.y > b.Min.y ? a.Min.y : b.Min.y;
				vMin.z = a.Min.z > b.Min.z ? a.Min.z : b.Min.z;

				// Calculate the maximum corner of the intersection
				vMax.x = a.Max.x < b.Max.x ? a.Max.x : b.Max.x;
				vMax.y = a.Max.y < b.Max.y ? a.Max.y : b.Max.y;
				vMax.z = a.Max.z < b.Max.z ? a.Max.z : b.Max.z;

				// Check if one box is completely inside the other
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
