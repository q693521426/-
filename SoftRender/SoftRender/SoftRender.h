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
	XMFLOAT3 Normal;
};

struct VS_OUT
{
	SimpleVertex* vs_out;
	WORD* indices;
	VS_OUT(SimpleVertex* vs_out,WORD* indices):vs_out(vs_out),indices(indices){}
};

struct SimpleVertex2D
{
	XMFLOAT2 Pos;
	XMFLOAT2 Tex;
};

struct LightInfo
{
	XMFLOAT3 Pos;
	XMFLOAT3 Color;
	XMMATRIX View;
	XMMATRIX Project;
	XMFLOAT3 Ambient;
	XMFLOAT3 Diffuse;
	XMFLOAT3 Specular;
	FLOAT Constant;
	FLOAT Linear;
	FLOAT Quadratic;
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

public:
	static int screenWidth;
	static int screenHeight;
	static Buffer<INT> BackBuffer;
	static Buffer<FLOAT> zBuffer;
	static Buffer<INT> Tex;
	static SimpleVertex *vs_out;
	static HANDLE* hThread;
	static LightInfo Light;
	static XMFLOAT3 ViewPos;
	static Camera mCamera;
private:
	GameTimer timer;
	bool paused;
	HINSTANCE hInst;
	HWND hWnd;
	bool fullScreen;
	std::wstring hWndCaption;
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Project;
	PAINTSTRUCT ps;
	HDC hdc;
	HDC backbuffDC;
	HBITMAP backbuffer;
	HBITMAP now_bitmap;
	bool rotate_flag;

	HRESULT InitWindow(HINSTANCE, int ,UINT, UINT);

	void CalculateFrameStats();
	void CalculateNormal();
	void UpdateWorldMatrix(const GameTimer& gt);
	void OnKeyboardInput(const GameTimer& gt);
	void Update(const GameTimer& gt);
	void Draw(const GameTimer& gt);
	void Render();
	//void ClearRenderTargetView(INT ColorRBGA);
	XMFLOAT4 transProSpace(const XMFLOAT3& p);
	XMFLOAT3 transWorldSpace(const XMFLOAT3& p);
	static DWORD WINAPI DrawTriangle3D(LPVOID lpParameter);
	static void DrawVextex(SimpleVertex& v, const Triangle& t, const Triangle& t_world);
	//void sort_x(XMFLOAT3& a, XMFLOAT3& b, XMFLOAT3& c);
	static FLOAT slope(const XMFLOAT4& a, const XMFLOAT4& b);
	static bool updateZBuffer(const INT x, const INT y, const FLOAT z);
	static void CreateTextureFromFile();
	static XMFLOAT3 transBaryCentric(const XMFLOAT4& p, const XMFLOAT4& p0, const XMFLOAT4& p1, const XMFLOAT4& p2);
	static XMFLOAT4 transPerspectiveCorrect(const XMFLOAT3& p_bary, const FLOAT z0, const FLOAT z1, const FLOAT z2);
	static bool IsInTriangle(XMFLOAT3 p_bary);
	static INT getBilinearFilteredPixelColor(Buffer<INT>& tex, double u, double v);
	static FLOAT NormalizeProjectZ(FLOAT z);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SoftRender* ApplicationHandle = nullptr;


#endif