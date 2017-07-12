#include "Triangle.h"

Triangle::Triangle(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2,FLOAT scale_x, FLOAT scale_y)
{
	Point3D.resize(3);
	Point2D.resize(3);
	Point3D[0] = p0;
	Point3D[1] = p1;
	Point3D[2] = p2;
	for (int i = 0; i < 3; ++i)
		Point2D[i] = XMFLOAT2(Point3D[i].x*scale_x, Point3D[i].y*scale_y);
}

Triangle::~Triangle()
{
	
}

XMVECTOR Triangle::GetPoint3DPos(int index)const
{
	return XMLoadFloat3(&Point3D[index]);
}

XMFLOAT3 Triangle::GetPoint3DPos3f(int index)const
{
	return Point3D[index];
}

XMVECTOR Triangle::GetPoint2DPos(int index)const
{
	return XMLoadFloat2(&Point2D[index]);
}

XMFLOAT2 Triangle::GetPoint2DPos2f(int index)const
{
	return Point2D[index];
}

bool Triangle::IsBackCulling()
{
	XMVECTOR p0 = GetPoint3DPos(0);
	XMVECTOR p1 = GetPoint3DPos(1);
	XMVECTOR p2 = GetPoint3DPos(2);

	XMVECTOR v0 = XMVectorSubtract(p1, p0);
	XMVECTOR v1 = XMVectorSubtract(p2, p0);

	XMVECTOR n = XMVector3Normalize(XMVector3Cross(p0, p1));

	//Backface-culling
	XMFLOAT3 dot;
	XMStoreFloat3(&dot, XMVector3Dot(p0, n));
	if (dot.x < 0)
	{
		return true;
	}
	return false;
}
