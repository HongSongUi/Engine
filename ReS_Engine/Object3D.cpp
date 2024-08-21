#include "Object3D.h"

void Object3D::SetMatrix(Matrix* world, Matrix* view, Matrix* proj)
{
	if (world != nullptr)
	{
		WorldMat = *world;
	}
	if (view != nullptr)
	{
		ViewMat = *view;
	}
	if (proj != nullptr)
	{
		ProjMat = *proj;
	}
	Mesh.SetMatrix(world, view, proj);
	Position.x = Mesh.WorldMat._41;
	Position.y = Mesh.WorldMat._42;
	Position.z = Mesh.WorldMat._43;
	for (int i = 0; i < 3; i++) {
		D3DXVec3TransformCoord(&Axis[i], &PreAxis[i], &Mesh.WorldMat);
		D3DXVec3Normalize(&Axis[i], &Axis[i]);

	}
}

void Object3D::SetListSize()
{
	VertexList.resize(24);
	IndexList.resize(36);
	Axis[0] = { 1.0f,0.0f,0.0f };
	Axis[1] = { 0.0f,1.0f,0.0f };
	Axis[2] = { 0.0f,0.0f,1.0f };
	memcpy(PreAxis, Axis, sizeof(Axis[0]) * 3);
}

bool Object3D::Render()
{
	PreRender();
	PostRender();

    return true;
}


bool Object3D::Release()
{
	BaseObject::Release();
    return true;
}

void Object3D::UpdatePosition(Vector3& pos)
{
	Position = pos;
}

void Object3D::UpdateScale(Vector3& scale)
{
	Scale = scale;
}
