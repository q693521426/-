#include "SoftRender.h"
#include <atlconv.h>
#include <sstream>
#include <directxcolors.h>

SoftRender::SoftRender():timer(), paused(false), 
						fullScreen(false),hWndCaption(L"SoftRender"),
						hInst(nullptr),hWnd(nullptr), 
						screenWidth(0), screenHeight(0),
						mCamera()
{
	mCamera.SetPosition(0.0f, 0.0f, -15.0f);
	mCamera.UpdateViewMatrix();
}

SoftRender::SoftRender(const SoftRender& s)
{
	timer = s.timer;
}

SoftRender::~SoftRender()
{
}

bool SoftRender::Initialize(HINSTANCE hInstance, int nCmdShow,UINT screenWidth = 800, UINT screenHeight = 600)
{
	HRESULT hr;

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	hr = InitWindow(hInstance, nCmdShow,screenWidth , screenHeight);
	if (FAILED(hr))
	{
		return false;
	}

	CreateTextureFromFile();

	return true;
}

void SoftRender::CreateTextureFromFile()
{
	FLOAT ColorRBG[3] = { 1.0f,1.0f,1.0f };
	Tex.Resize(screenWidth,screenHeight, ColorRBG);

}

int SoftRender::Run()
{
	MSG msg = { 0 };

	timer.Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			timer.Tick();

			if (!paused)
			{
				CalculateFrameStats();
				Update(timer);
	//			Draw(timer);
			}
			else
			{
				Sleep(100);
			}
		}
	}
	
	return (int)msg.wParam;
}

void SoftRender::Shutdown()
{
	BackBuffer.Release();
	zBuffer.Release();
}

HRESULT SoftRender::InitWindow(HINSTANCE hInstance, int nCmdShow, UINT screenWidth, UINT screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDI_WINLOGO);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = L"SoftRender";
	wc.hIconSm = LoadIcon(wc.hInstance, IDI_WINLOGO);
	if (!RegisterClassEx(&wc))
		return E_FAIL;

	// Create window
	hInst = hInstance;
	
	std::wostringstream outs;
	outs.precision(6);
	outs << hWndCaption << L"    "
		<< L"FPS: " << L"    " << L"    "
		<< L"Frame Time: " << L"    " << L" (ms)";
	if (fullScreen)
	{
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		posX = posY = 0;
	}
	else
	{
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}
	hWnd = CreateWindow(hWndCaption.c_str(), outs.str().c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		posX, posY, screenWidth, screenHeight, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
		return E_FAIL;

	ShowWindow(hWnd, nCmdShow);

	return S_OK;
}

LRESULT CALLBACK SoftRender::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (umsg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wparam) == WA_INACTIVE)
		{
			paused = true;
			timer.Stop();
		}
		else
		{
			paused = false;
			timer.Start();
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Draw(hdc, timer);
		EndPaint(hWnd, &ps);
		break;
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void SoftRender::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		mCamera.Walk(10.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCamera.Walk(-10.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCamera.Strafe(-10.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCamera.Strafe(10.0f*dt);

	mCamera.UpdateViewMatrix();
}

void SoftRender::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);
}

void SoftRender::Draw(HDC& hdc,const GameTimer& gt)
{
	using namespace DirectX;

	ClearRenderTargetView(hdc, DirectX::Colors::MidnightBlue);
	zBuffer.Resize(screenWidth, screenHeight, 1.0f);

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};
	
	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};
	//sizeof(indices)/sizeof(WORD)/3
	for (int i = 0; i <2; i++)
	{
		DrawTriangle3D(vertices, indices+i*3, hdc);
	}

	for (int i = 0; i < screenWidth; ++i)
		for (int j = 0; j<screenHeight; ++j)
			SetPixel(hdc, i, j, RGB((BackBuffer(i, j)[0] * 255), BackBuffer(i, j)[1] * 255, BackBuffer(i, j)[2] * 255));
		
}

void SoftRender::DrawTriangle3D(SimpleVertex* vertices, WORD indices[3], HDC& hdc)
{

	XMFLOAT3 p0 = transProSpace(vertices[indices[0]].Pos);
	XMFLOAT3 p1 = transProSpace(vertices[indices[1]].Pos);
	XMFLOAT3 p2 = transProSpace(vertices[indices[2]].Pos);


	Triangle t(p0, p1, p2, screenWidth, screenHeight);
	if (t.IsBackCulling(-mCamera.GetLook()))
	{
		return;
	}

	XMFLOAT2 p0_2f = t.GetPoint2DPos2f(0);
	XMFLOAT2 p1_2f = t.GetPoint2DPos2f(1);
	XMFLOAT2 p2_2f = t.GetPoint2DPos2f(2);

	int left = MathHelper::Min(p0_2f.x, p1_2f.x, p2_2f.x);
	int right = MathHelper::Max(p0_2f.x, p1_2f.x, p2_2f.x);
	int top = MathHelper::Max(p0_2f.y, p1_2f.y, p2_2f.y);
	int bottom = MathHelper::Min(p0_2f.y, p1_2f.y, p2_2f.y);
	int scanWidth = right - left;
	int scanHeight = top - bottom;

	for (int i = 0; i < scanWidth; ++i)
	{
		for (int j = 0; j < scanHeight; ++j)
		{
			SimpleVertex2D v;
			v.Pos = XMFLOAT2(left + i, bottom + j);
			XMFLOAT3 p_bary = transBaryCentric(v.Pos, p0_2f, p1_2f, p2_2f);
			if (IsInTriangle(p_bary) && MathHelper::IsClamp(v.Pos.x, screenWidth / -2.0f, screenWidth / 2.0f) &&
				MathHelper::IsClamp(v.Pos.y, screenHeight / -2.0f, screenHeight / 2.0f))
			{
				XMFLOAT3 p_correct_bary = transPerspectiveCorrect(p_bary, p0.z, p1.z, p2.z);

				v.Tex = XMFLOAT2Add3(XMFLOAT2Mul(vertices[indices[0]].Tex, p_correct_bary.x),
					XMFLOAT2Mul(vertices[indices[1]].Tex, p_correct_bary.y),
					XMFLOAT2Mul(vertices[indices[2]].Tex, p_correct_bary.z));

				BackBuffer(screenWidth / 2.0f + v.Pos.x, screenHeight / 2.0f + v.Pos.y) = Tex(v.Tex.x*Tex.GetWidth(), v.Tex.y*Tex.GetHeight());

			}
		}
	}
}

XMFLOAT2 SoftRender::XMFLOAT2Mul(const XMFLOAT2& f, const FLOAT k)
{
	return XMFLOAT2(f.x*k, f.y*k);
}

XMFLOAT2 SoftRender::XMFLOAT2Add3(const XMFLOAT2& a, const XMFLOAT2& b, const XMFLOAT2& c)
{
	return XMFLOAT2(a.x + b.x + c.x, a.y + b.y + c.y);
}

XMFLOAT3 SoftRender::transBaryCentric(const XMFLOAT2& p,const XMFLOAT2& p0, const XMFLOAT2& p1, const XMFLOAT2& p2)
{
	XMFLOAT3 p_bary;
	FLOAT det = (p1.y - p2.y)*(p0.x - p2.x) + (p2.x - p1.x)*(p0.y - p2.y);
	FLOAT a = ((p1.y - p2.y)*(p.x - p2.x) + (p2.x - p1.x)*(p.y - p2.y)) / det;
	FLOAT b = ((p2.y - p0.y)*(p.x - p2.x) + (p0.x - p2.x)*(p.y - p2.y)) / det;
	
	return XMFLOAT3(a, b, 1.0f - a - b);
}

XMFLOAT3 SoftRender::transPerspectiveCorrect(const XMFLOAT3& p_bary,const FLOAT z0, const FLOAT z1, const FLOAT z2)
{
	FLOAT a = p_bary.x / z0;
	FLOAT b = p_bary.y / z1;
	FLOAT c = p_bary.z / z2;
	FLOAT d = a+b+c;

	FLOAT x = a / d;
	FLOAT y = b / d;
	FLOAT z = 1 - x - y;
	
	return XMFLOAT3(x, y, z);
}

bool SoftRender::IsInTriangle(XMFLOAT3 p_bary)
{
	return MathHelper::IsClamp(p_bary.x,0.f,1.0f) &&
			MathHelper::IsClamp(p_bary.y, 0.f, 1.0f) &&
			MathHelper::IsClamp(p_bary.z, 0.f, 1.0f);
}

XMFLOAT3 SoftRender::transProSpace(const XMFLOAT3& p)
{
	XMFLOAT3 v_3f;
	XMVECTOR v;

	World = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	View = XMMatrixLookAtLH(Eye, At, Up);
	Project = XMMatrixPerspectiveFovLH(XM_PIDIV4, screenWidth / (FLOAT)screenHeight, 0.01f, 100.0f);
	/*View = mCamera.GetView();
	Project = mCamera.GetProj();*/
	
	v = XMVector4Normalize(XMVector4Transform(XMVectorSet(p.x, p.y, p.z, 1.0f), World*View*Project));
	XMStoreFloat3(&v_3f, v);
	return v_3f;
}

void SoftRender::updateZBuffer(const XMFLOAT3& v_3f, const XMFLOAT2& v_2f)
{
	zBuffer(v_2f.x, v_2f.y) = MathHelper::Min(zBuffer(v_2f.x, v_2f.y), v_3f.z);
}


//XMFLOAT4 transFloat3T4(const XMFLOAT3& v3, FLOAT f)
//{
//	XMFLOAT4 v4;
//	v4.x = v3.x;
//	v4.y = v3.y;
//	v4.z = v3.z;
//	v4.w = f;
//}
//
//XMFLOAT3 transFloat4T3(const XMFLOAT4& v4)
//{
//	XMFLOAT3 v3;
//	v3.x = (v4.w == 0.f ? v4.x : v4.x / v4.w);
//	v3.y = (v4.w == 0.f ? v4.y : v4.y / v4.w);
//	v3.z = (v4.w == 0.f ? v4.z : v4.z / v4.w);
//}

void SoftRender::ClearRenderTargetView(HDC& hdc,const FLOAT* ColorRBGA)
{
	FLOAT* color=const_cast<FLOAT*>(ColorRBGA);
	BackBuffer.Resize(screenWidth, screenHeight,color);
}

void SoftRender::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = hWndCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(hWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}

