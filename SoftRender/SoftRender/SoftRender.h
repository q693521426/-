#pragma once
#ifndef _SOFTREND_H_
#define _SOFTREND_H_

#define WIN32_LEAN_AND_MEAN

#include "GameTimer.h"
#include "Camera.h"
#include "Buffer.h"
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
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Project;


	HRESULT InitWindow(HINSTANCE, int ,UINT, UINT);

	void CalculateFrameStats();
	void OnKeyboardInput(const GameTimer& gt);
	void Update(const GameTimer& gt);
	void Draw(HDC& hdc,const GameTimer& gt);
	void ClearRenderTargetView(HDC& hdc,const FLOAT* ColorRBGA);
	XMFLOAT3 transProSpace(XMFLOAT3 p);
	void DrawRect3D(SimpleVertex vertices[4]);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SoftRender* ApplicationHandle = nullptr;


#endif