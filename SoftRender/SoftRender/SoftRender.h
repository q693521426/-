#pragma once
#ifndef _SOFTREND_H_
#define _SOFTREND_H_

#define WIN32_LEAN_AND_MEAN

#include "GameTimer.h"
#include "Camera.h"
#include "Buffer.h"
#include "Triangle.h"
#include <windows.h>
#include <string>


template class Buffer<FLOAT>;
template class Buffer<FLOAT*>;

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

struct SimpleVertex2D
{
	XMFLOAT2 Pos;
	XMFLOAT2 Tex;
};

class SoftRender
{
public:
	SoftRender();
	SoftRender(const SoftRender&);
	~SoftRender();

	bool Initialize(HINSTANCE, int ,UINT ,UINT);
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	int Run();
	void Shutdown();

private:
	GameTimer timer;
	bool paused;
	HINSTANCE hInst;
	HWND hWnd;
	UINT screenWidth;
	UINT screenHeight;
	bool fullScreen;
	std::wstring hWndCaption;
	Camera mCamera;
	Buffer<FLOAT*> BackBuffer;
	Buffer<FLOAT> zBuffer;
	Buffer<FLOAT*> Tex;
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Project;


	HRESULT InitWindow(HINSTANCE, int ,UINT, UINT);

	void CalculateFrameStats();
	void OnKeyboardInput(const GameTimer& gt);
	void Update(const GameTimer& gt);
	void Draw(HDC& hdc,const GameTimer& gt);
	void ClearRenderTargetView(HDC& hdc,const FLOAT* ColorRBGA);
	XMFLOAT3 transProSpace(const XMFLOAT3& p);
	void SoftRender::DrawTriangle3D(SimpleVertex* vertices, WORD indices[3], HDC& hdc);
	void updateZBuffer(const XMFLOAT3& v_3f, const XMFLOAT2& v_2f);
	void CreateTextureFromFile();
	XMFLOAT2 XMFLOAT2Mul(const XMFLOAT2& f, const FLOAT k);
	XMFLOAT2 XMFLOAT2Add3(const XMFLOAT2& a, const XMFLOAT2& b, const XMFLOAT2& c);
	XMFLOAT3 transBaryCentric(const XMFLOAT2& p, const XMFLOAT2& p0, const XMFLOAT2& p1, const XMFLOAT2& p2);
	XMFLOAT3 transPerspectiveCorrect(const XMFLOAT3& p_bary, const FLOAT z0, const FLOAT z1, const FLOAT z2);
	bool IsInTriangle(XMFLOAT3 p_bary);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SoftRender* ApplicationHandle = nullptr;


#endif