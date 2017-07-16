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
	Triangle(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2);
	Triangle(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, FLOAT scale_x, FLOAT scale_y,WORD indices[3]);
	~Triangle();

	void Initialize(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2);

	XMVECTOR GetPoint3DPos(int index)const;
	XMFLOAT3 GetPoint3DPos3f(int index)const;

	XMVECTOR GetPoint2DPos(int index)const;
	XMFLOAT3 GetPoint2DPos3f(int index)const;
	
	XMVECTOR GetNormal()const;
	XMFLOAT3 GetNormal3f()const;
	void SetNormal3f(const XMFLOAT3& normal);

	WORD GetIndex(int index)const;
	INT* GetOrder()const;

	bool IsBackCulling(const XMVECTOR& LookAt);
	void sort_2D_x();

private:
	std::vector<XMFLOAT3> Point3D;
	std::vector<XMFLOAT3> Point2D;
	XMFLOAT3 normal;
	WORD* indices;
	INT* order;
};

#endif
