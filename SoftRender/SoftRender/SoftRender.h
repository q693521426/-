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
	XMFLOAT4 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Normal;
};

struct VS_OUT
{
	SimpleVertex* vs_out;
	FLOAT w;
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
	inline LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	int Run();
	void Shutdown();

public:
	 int screenWidth;
	 int screenHeight;
	 Buffer<INT> BackBuffer;
	 Buffer<FLOAT> zBuffer;
	 Buffer<INT> zBuffer_fake;
	 Buffer<XMFLOAT3> Tex;
	 SimpleVertex *vs_out;
	 HANDLE* hThread;
	 LightInfo Light;
	 XMFLOAT3 ViewPos;
	 Camera mCamera;
	 Triangle* t_world;
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
	bool rotate_flag_x;
	bool rotate_flag_y;

	HRESULT InitWindow(HINSTANCE, int ,UINT, UINT);

	inline void CalculateFrameStats();
	inline void CalculateNormal();
	inline void UpdateWorldMatrix(const GameTimer& gt);
	inline void OnKeyboardInput(const GameTimer& gt);
	inline void Update(const GameTimer& gt);
	void Draw(const GameTimer& gt);
	void Render();
	//void ClearRenderTargetView(INT ColorRBGA);
	inline XMFLOAT4 transProSpace(const XMFLOAT4& p);
	inline XMFLOAT4 transViewSpace(const XMFLOAT4& p);
	inline XMFLOAT4 transWorldSpace(const XMFLOAT4& p);
	inline  DWORD WINAPI DrawTriangle3D(LPVOID lpParameter);
	//void (XMFLOAT3& a, XMFLOAT3& b, XMFLOAT3& c);
	inline  FLOAT slope(const XMFLOAT4& a, const XMFLOAT4& b);
	inline  bool updateZBuffer(const INT x, const INT y, const INT z);
	inline  void CreateTextureFromFile();
	inline  XMFLOAT3 transBaryCentric(const XMFLOAT4& p, const XMFLOAT4& p0, const XMFLOAT4& p1, const XMFLOAT4& p2);
	inline  XMFLOAT4 transPerspectiveCorrect(const XMFLOAT3& p_bary, const FLOAT z0, const FLOAT z1, const FLOAT z2);
	inline  bool IsInTriangle(XMFLOAT3 p_bary);
	inline  bool IsInTriangle(XMFLOAT4 p_bary);
	inline  bool IsInView(const XMFLOAT4& p);
	inline  XMFLOAT3 getBilinearFilteredPixelColor(Buffer<XMFLOAT3>& tex, double u, double v);
	inline  FLOAT NormalizeProjectZ(FLOAT z);
	inline FLOAT pow(const FLOAT a, const INT b);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SoftRender* ApplicationHandle = nullptr;


#endif