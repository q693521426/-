//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>
#include <cstdlib>

class MathHelper
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

    static int Rand(int a, int b)
    {
        return a + rand() % ((b - a) + 1);
    }

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Min(const T& a, const T& b, const T& c)
	{
		return Min(Min(a, b), c);
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	
	template<typename T>
	static T Max(const T& a, const T& b, const T& c)
	{
		return Max(Max(a, b), c);
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b-a)*t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x); 
	}

	template<typename T>
	static bool IsClamp(const T& x, const T& low, const T& high)
	{
		if (x >= low && x <= high)
			return true;
		else
			return false;
	}
	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius*sinf(phi)*cosf(theta),
			radius*cosf(phi),
			radius*sinf(phi)*sinf(theta),
			1.0f);
	}

    static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
        DirectX::XMMATRIX A = M;
        A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
        return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

    static DirectX::XMFLOAT4X4 Identity4x4()
    {
        static DirectX::XMFLOAT4X4 I(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        return I;
    }

	static DirectX::XMMATRIX LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
	{
		using namespace DirectX;
		XMFLOAT4X4 View;
		XMFLOAT3 Right, Up, Look;

		XMVECTOR P = XMLoadFloat3(&pos);
		XMVECTOR T = XMLoadFloat3(&target);
		XMVECTOR U = XMLoadFloat3(&up);

		XMVECTOR L = XMVector3Normalize(XMVectorSubtract(T, P));
		XMVECTOR R = XMVector3Normalize(XMVector3Cross(U, L));
		U = XMVector3Cross(L, R);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&Right, R);
		XMStoreFloat3(&Up, U);
		XMStoreFloat3(&Look, L);

		View(0, 0) = Right.x;
		View(1, 0) = Right.y;
		View(2, 0) = Right.z;
		View(3, 0) = x;

		View(0, 1) = Up.x;
		View(1, 1) = Up.y;
		View(2, 1) = Up.z;
		View(3, 1) = y;

		View(0, 2) = Look.x;
		View(1, 2) = Look.y;
		View(2, 2) = Look.z;
		View(3, 2) = z;

		View(0, 3) = 0.0f;
		View(1, 3) = 0.0f;
		View(2, 3) = 0.0f;
		View(3, 3) = 1.0f;

		return XMLoadFloat4x4(&View);
	}

    static DirectX::XMVECTOR RandUnitVec3();
    static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

	static const float Infinity;
	static const float Pi;


};

