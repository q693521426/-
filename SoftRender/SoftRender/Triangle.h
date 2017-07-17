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
	Triangle(const XMFLOAT4& p0, const XMFLOAT4& p1, const XMFLOAT4& p2);
	Triangle(const XMFLOAT4& p0, const XMFLOAT4& p1, const XMFLOAT4& p2, FLOAT scale_x, FLOAT scale_y,WORD indices[3]);
	Triangle(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2);
	Triangle(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2, FLOAT scale_x, FLOAT scale_y, WORD indices[3]);

	~Triangle();

	void Initialize(const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2);
	void Initialize(const XMFLOAT4& p0, const XMFLOAT4& p1, const XMFLOAT4& p2);

	XMVECTOR GetPoint3DPos(int index)const;
	XMFLOAT4 GetPoint3DPos4f(int index)const;

	XMVECTOR GetPoint2DPos(int index)const;
	XMFLOAT4 GetPoint2DPos4f(int index)const;
	
	XMVECTOR GetNormal()const;
	XMFLOAT3 GetNormal3f()const;
	void SetNormal3f(const XMFLOAT3& normal);
	void SetOrder(const INT order[3]);

	WORD GetIndex(int index)const;
	WORD GetIndexByOrder(int index)const;
	INT* GetOrder()const;

	bool IsBackCulling(const XMVECTOR& LookAt);
	bool IsBackCulling(const FLOAT x, const  FLOAT y, const  FLOAT z);
	void sort_2D_x();

private:
	std::vector<XMFLOAT4> Point3D;
	std::vector<XMFLOAT4> Point2D;
	XMFLOAT3 normal;
	WORD* indices;
	INT* order;
};

#endif
