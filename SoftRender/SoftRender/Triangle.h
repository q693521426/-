#pragma once
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <DirectXMath.h>
#include <vector>
#include <windows.h>
using namespace DirectX;


class Triangle
{
public:
	Triangle(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2);
	Triangle(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2, FLOAT scale_x, FLOAT scale_y);
	~Triangle();

	XMVECTOR GetPoint3DPos(int index)const;
	XMFLOAT3 GetPoint3DPos3f(int index)const;

	XMVECTOR GetPoint2DPos(int index)const;
	XMFLOAT2 GetPoint2DPos2f(int index)const;
	
	XMVECTOR GetNormal()const;
	XMFLOAT3 GetNormal3f()const;
	void SetNormal3f(const XMFLOAT3& normal);

	bool IsBackCulling(const XMVECTOR& LookAt);
private:
	std::vector<XMFLOAT3> Point3D;
	std::vector<XMFLOAT2> Point2D;
	XMFLOAT3 normal;
};

#endif
