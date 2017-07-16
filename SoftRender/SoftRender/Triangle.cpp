#include "Triangle.h"

Triangle::Triangle(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2,FLOAT scale_x, FLOAT scale_y, WORD indices[3])
{
	Initialize(p0, p1, p2);

	this->indices = indices;
	for (int i = 0; i < 3; ++i)
		Point2D[i] = XMFLOAT3(Point3D[i].x*scale_x, Point3D[i].y*scale_y, Point3D[i].z);
}

Triangle::Triangle(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2)
{
	Initialize(p0, p1, p2);
}

Triangle::~Triangle()
{
	delete order;
}

void Triangle::Initialize(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2)
{
	this->indices = nullptr;
	this->order = new INT[3]{ 0,1,2 };
	Point3D.resize(3);
	Point2D.resize(3);
	Point3D[0] = p0;
	Point3D[1] = p1;
	Point3D[2] = p2;

	XMVECTOR v0 = GetPoint3DPos(0);
	XMVECTOR v1 = GetPoint3DPos(1);
	XMVECTOR v2 = GetPoint3DPos(2);

	XMVECTOR e0 = XMVectorSubtract(v1, v0);
	XMVECTOR e1 = XMVectorSubtract(v2, v0);

	XMStoreFloat3(&normal, XMVector3Normalize(XMVector3Cross(e0, e1)));
}

XMVECTOR Triangle::GetPoint3DPos(int index)const
{
	return XMLoadFloat3(&Point3D[order[index]]);
}

XMFLOAT3 Triangle::GetPoint3DPos3f(int index)const
{
	return Point3D[order[index]];
}

XMVECTOR Triangle::GetPoint2DPos(int index)const
{
	return XMLoadFloat3(&Point2D[order[index]]);
}

XMFLOAT3 Triangle::GetPoint2DPos3f(int index)const
{
	return Point2D[order[index]];
}

XMVECTOR Triangle::GetNormal()const
{
	return XMLoadFloat3(&normal);
}

XMFLOAT3 Triangle::GetNormal3f()const
{
	return normal;
}

void Triangle::SetNormal3f(const XMFLOAT3& normal)
{
	this->normal = normal;
}

bool Triangle::IsBackCulling(const XMVECTOR& LookAt)
{
	//Backface-culling
	XMFLOAT3 dot;
	XMStoreFloat3(&dot, XMVector3Dot(LookAt, GetNormal()));
	if (dot.x > 0)
	{
		return true;
	}
	return false;
}

void Triangle::sort_2D_x()
{
	if (GetPoint3DPos3f(0).x > GetPoint3DPos3f(1).x)
	{
		std::swap(order[0], order[1]);
	}

	if (GetPoint3DPos3f(0).x > GetPoint3DPos3f(2).x)
	{
		std::swap(order[0], order[2]);
	}

	if (GetPoint3DPos3f(1).x > GetPoint3DPos3f(2).x)
	{
		std::swap(order[1], order[2]);
	}
}

WORD Triangle::GetIndex(int index)const
{
	return indices[order[index]];
}

INT* Triangle::GetOrder()const
{
	return order;
}