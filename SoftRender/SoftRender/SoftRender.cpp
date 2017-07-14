#include "SoftRender.h"
#include <atlconv.h>
#include <sstream>
#include <directxcolors.h>
#include <cstdio>
#pragma warning(disable:4996)

SoftRender::SoftRender() :timer(), paused(false),
fullScreen(false), hWndCaption(L"SoftRender"),
hInst(nullptr), hWnd(nullptr),
screenWidth(0), screenHeight(0),
mCamera()
{
	XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mCamera.LookAt(Eye, At, Up);
	mCamera.UpdateViewMatrix();
}

SoftRender::SoftRender(const SoftRender& s)
{
	timer = s.timer;
}

SoftRender::~SoftRender()
{
}

bool SoftRender::Initialize(HINSTANCE hInstance, int nCmdShow, UINT screenWidth = 800, UINT screenHeight = 600)
{
	HRESULT hr;

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;


	hr = InitWindow(hInstance, nCmdShow, screenWidth, screenHeight);
	if (FAILED(hr))
	{
		return false;
	}


	return true;
}

void SoftRender::CreateTextureFromFile()
{
	int i, j;

	FILE* f = fopen("images.bmp", "rb");
	if (NULL == f)
	{
		exit(1);
	}
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

											   // extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);

	for (i = 0; i < size; i += 3)
	{
		unsigned char tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}

	Tex.Resize(width, height);
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			int R = data[(j*width + i) * 3];
			int G = data[(j*width + i) * 3 + 1];
			int B = data[(j*width + i) * 3 + 2];
			Tex(i, j) = R << 16 | G << 8 | B;
		}
	}

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
		else
		{
		/*	timer.Tick();
			CalculateFrameStats();
			Update(timer);*/
		}

		Render();
	}

	return (int)msg.wParam;
}

void SoftRender::Render()
{
	Draw(timer);
	SelectObject(backbuffDC, now_bitmap);
	BitBlt(hdc, 0, 0, screenWidth, screenHeight, backbuffDC, 0, 0, SRCCOPY);
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
	CreateTextureFromFile();

	hdc = GetDC(hWnd);
	backbuffDC = CreateCompatibleDC(hdc);
	backbuffer = CreateCompatibleBitmap(hdc, screenWidth, screenHeight);

	//	int savedDC = SaveDC(backbuffDC);
	SelectObject(backbuffDC, backbuffer);
	//		HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));

	BITMAPINFO bmp_info;
	bmp_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);//结构体的字节数
	bmp_info.bmiHeader.biWidth = screenWidth;//以像素为单位的位图宽
	bmp_info.bmiHeader.biHeight = -(int)screenHeight;//以像素为单位的位图高,若为负，表示以左上角为原点，否则以左下角为原点
	bmp_info.bmiHeader.biPlanes = 1;//目标设备的平面数，必须设置为1
	bmp_info.bmiHeader.biBitCount = 32; //位图中每个像素的位数
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biSizeImage = 0;
	bmp_info.bmiHeader.biXPelsPerMeter = 0;
	bmp_info.bmiHeader.biYPelsPerMeter = 0;
	bmp_info.bmiHeader.biClrUsed = 0;
	bmp_info.bmiHeader.biClrImportant = 0;

	//// 设置文字背景色
	//SetBkColor(backbuffDC, RGB(0, 0, 0));
	//// 设置文字颜色
	//SetTextColor(backbuffDC, RGB(255, 255, 255));
	//TextOut(backbuffDC, 20, 470, L"X:", 3 * sizeof(wchar_t));
	//TextOut(backbuffDC, 20, 490, L"Y:", 3 * sizeof(wchar_t));
	//TextOut(backbuffDC, 20, 510, L"Z:", 3 * sizeof(wchar_t));
	////		DeleteObject(hBrush);

	INT color = (INT)(Colors::MidnightBlue[0] * 255) << 16
		| (INT)(Colors::MidnightBlue[1] * 255) << 8
		| (INT)(Colors::MidnightBlue[2] * 255);
	zBuffer.Initialize(screenWidth, screenHeight, 1.0f);
	BackBuffer.Initialize(screenWidth, screenHeight, color);
	now_bitmap = CreateDIBSection(backbuffDC, &bmp_info, DIB_RGB_COLORS, (void**)(BackBuffer.GetppBuffer()), NULL, 0);
	BackBuffer.Clear(color);
	
	//	RestoreDC(backbuffDC, savedDC);
	UpdateWindow(hWnd);

	return S_OK;
}

LRESULT CALLBACK SoftRender::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
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
		return 0;
	case WM_PAINT:
	{
	//	Render();
		return 0;
	}
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

void SoftRender::Draw(const GameTimer& gt)
{
	using namespace DirectX;
	INT color=(INT)(Colors::MidnightBlue[0]*255) <<16 
				| (INT)(Colors::MidnightBlue[1] * 255) <<8
				| (INT)(Colors::MidnightBlue[2] * 255) ;
	ClearRenderTargetView( color);
	zBuffer.Clear(1.0f);

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
	SimpleVertex *vs_out = new SimpleVertex[sizeof(vertices) / sizeof(SimpleVertex)]();

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
	XMFLOAT3 LightPos = XMFLOAT3(0.0f,3.0f,0.0f);
	XMMATRIX LightView = MathHelper::LookAt(LightPos, XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 1.0f, 0.f));
	XMMATRIX LightProject = XMMatrixPerspectiveFovLH(MathHelper::Pi*2/3, screenWidth/(FLOAT)screenHeight, 1.0f, 100.0f);

	static float t = 0.0f;
	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	t = (timeCur - timeStart) / 1000.0f;
	World = XMMatrixRotationY(t);

	//for (int i = 8; i < 9; i++)
	for (int i = 0; i < sizeof(indices) / sizeof(*indices) / 3; ++i)
	{
		//normal
		Triangle t_3d(vertices[indices[i*3]].Pos, vertices[indices[i * 3+1]].Pos, vertices[indices[i * 3+2]].Pos);

		for (int j = 0; j<3; ++j)
			XMStoreFloat3(&vertices[indices[j]].Normal,
			(XMLoadFloat3((&vertices[indices[j]].Normal)) + t_3d.GetNormal()));
		
		//fragPos
		vs_out[indices[i * 3]].Pos = transWorldSpace(vertices[indices[i * 3]].Pos);
		vs_out[indices[i * 3 + 1]].Pos = transWorldSpace(vertices[indices[i * 3 + 1]].Pos);
		vs_out[indices[i * 3 + 2]].Pos = transWorldSpace(vertices[indices[i * 3 + 2]].Pos);

		//fragTex
		vs_out[indices[i * 3]].Tex = vertices[indices[i * 3]].Tex;
		vs_out[indices[i * 3 + 1]].Tex = vertices[indices[i * 3+1]].Tex;
		vs_out[indices[i * 3 + 2]].Tex = vertices[indices[i * 3+2]].Tex;
		//DrawTriangle3D(vertices, indices + i * 3, hdc);
	}
	for (int i = 0; i < sizeof(vertices) / sizeof(*vertices); ++i)
	{
		//frag_normal
		XMStoreFloat3(&vertices[i].Normal, 
			XMVector3Normalize(XMLoadFloat3(&vertices[i].Normal)));
		XMStoreFloat3(&vs_out[i].Normal, XMVector4Transform(
			XMVectorSet(vertices[i].Normal.x, vertices[i].Normal.y, vertices[i].Normal.z,0),
			MathHelper::InverseTranspose(World)));
	}

	for (int i = 0; i < sizeof(indices) / sizeof(*indices) / 3; ++i)
	{
		DrawTriangle3D(vs_out, indices + i * 3, hdc);
	}
	delete[] vs_out;
}

void SoftRender::DrawTriangle3D(SimpleVertex* vs_out, WORD indices[3], HDC& hdc)
{
	//position
	XMFLOAT3 p0 = transProSpace(vs_out[indices[0]].Pos);
	XMFLOAT3 p1 = transProSpace(vs_out[indices[1]].Pos);
	XMFLOAT3 p2 = transProSpace(vs_out[indices[2]].Pos);

	Triangle t(p0, p1, p2, screenWidth / 2, screenHeight / 2);
	
	//back-culling
	if (t.IsBackCulling(mCamera.GetLook()))
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
			
			//bary-centric
			XMFLOAT3 p_bary = transBaryCentric(v.Pos, p0_2f, p1_2f, p2_2f);
			if (IsInTriangle(p_bary) &&
				MathHelper::IsClamp(v.Pos.x, screenWidth / -2.0f, screenWidth / 2.0f) &&
				MathHelper::IsClamp(v.Pos.y, screenHeight / -2.0f, screenHeight / 2.0f))
			{
				//perspective-correct
				XMFLOAT4 p_correct_bary = transPerspectiveCorrect(p_bary, p0.z, p1.z, p2.z);
				if (updateZBuffer(XMFLOAT3(screenWidth / 2.0f + v.Pos.x, screenHeight / 2.0f + v.Pos.y, p_correct_bary.w)))
				{

					XMVECTOR v_tex=  XMLoadFloat2(&vs_out[indices[0]].Tex) * XMVectorReplicate(p_correct_bary.x)
									+ XMLoadFloat2(&vs_out[indices[1]].Tex) * XMVectorReplicate(p_correct_bary.y)
									+ XMLoadFloat2(&vs_out[indices[2]].Tex) * XMVectorReplicate(p_correct_bary.z);
					XMStoreFloat2(&v.Tex, v_tex);

					//bilinear-filter
					BackBuffer(screenWidth / 2.0f + v.Pos.x, screenHeight / 2.0f + v.Pos.y) =
						getBilinearFilteredPixelColor(Tex, v.Tex.x, v.Tex.y);
				}
			}
		}
	}
}

INT SoftRender::getBilinearFilteredPixelColor(Buffer<INT>& tex, double u, double v)
{
	u *= tex.GetWidth()-1;
	v *= tex.GetHeight()-1;
	int x = floor(u);
	int y = floor(v);
	double u_ratio = u - x;
	double v_ratio = v - y;
	double u_opposite = 1 - u_ratio;
	double v_opposite = 1 - v_ratio;
	
	double result_x, result_y, result_z;
	if (x+1 >= tex.GetWidth() || y+1 >= tex.GetHeight())
	{
		result_x = ((tex(x, y) >> 16) & 0xFF);
		result_y = ((tex(x, y) >> 8) & 0xFF);
		result_z = (tex(x, y) & 0xFF);
	}
	else
	{
		result_x = (((tex(x, y) >> 16) & 0xFF) * u_opposite + ((tex(x+1, y) >> 16) & 0xFF) * u_ratio) * v_opposite +
			(((tex(x, y+1) >> 16) & 0xFF) * u_opposite + ((tex(x+1, y+1) >> 16) & 0xFF) * u_ratio) * v_ratio;
		result_y = (((tex(x, y) >> 8) & 0xFF) * u_opposite + ((tex(x + 1, y) >> 8) & 0xFF) * u_ratio) * v_opposite +
			(((tex(x, y + 1) >> 8) & 0xFF) * u_opposite + ((tex(x + 1, y + 1) >> 8) & 0xFF) * u_ratio) * v_ratio;
		result_z = ((tex(x, y) & 0xFF) * u_opposite + (tex(x + 1, y) & 0xFF) * u_ratio) * v_opposite +
			((tex(x, y + 1) & 0xFF) * u_opposite + (tex(x + 1, y + 1) & 0xFF) * u_ratio) * v_ratio;

	}
	
	return (int)result_x<<16 | (int)result_y<<8 | (int)result_z;
}

XMFLOAT3 SoftRender::transBaryCentric(const XMFLOAT2& p, const XMFLOAT2& p0, const XMFLOAT2& p1, const XMFLOAT2& p2)
{
	XMFLOAT3 p_bary;
	FLOAT det = (p1.y - p2.y)*(p0.x - p2.x) + (p2.x - p1.x)*(p0.y - p2.y);
	FLOAT a = ((p1.y - p2.y)*(p.x - p2.x) + (p2.x - p1.x)*(p.y - p2.y)) / det;
	FLOAT b = ((p2.y - p0.y)*(p.x - p2.x) + (p0.x - p2.x)*(p.y - p2.y)) / det;
		
	return XMFLOAT3(a, b, 1.0f - a - b);
}

XMFLOAT4 SoftRender::transPerspectiveCorrect(const XMFLOAT3& p_bary, const FLOAT z0, const FLOAT z1, const FLOAT z2)
{
	FLOAT a = p_bary.x / z0;
	FLOAT b = p_bary.y / z1;
	FLOAT c = p_bary.z / z2;
	FLOAT d = a + b + c;

	FLOAT x = a / d;
	FLOAT y = b / d;
	FLOAT z = 1 - x - y;

	return XMFLOAT4(x, y, z, 1 / d);
}

bool SoftRender::IsInTriangle(XMFLOAT3 p_bary)
{
	return MathHelper::IsClamp(p_bary.x, 0.f, 1.0f) &&
		MathHelper::IsClamp(p_bary.y, 0.f, 1.0f) &&
		MathHelper::IsClamp(p_bary.z, 0.f, 1.0f);
}

XMFLOAT3 SoftRender::transProSpace(const XMFLOAT3& p)
{
	XMFLOAT4 v_4f;
	XMVECTOR v;

	View = mCamera.GetView();
	Project = mCamera.GetProj();

	v = XMVector4Transform(XMVectorSet(p.x, p.y, p.z, 1.0f), View*Project);
	XMStoreFloat4(&v_4f, v);

	return XMFLOAT3(v_4f.x / v_4f.w, v_4f.y / v_4f.w, v_4f.z / v_4f.w);
}

XMFLOAT3 SoftRender::transWorldSpace(const XMFLOAT3& p)
{
	XMFLOAT4 v_4f;
	XMVECTOR v;

	v = XMVector4Transform(XMVectorSet(p.x, p.y, p.z, 1.0f), World);
	XMStoreFloat4(&v_4f, v);

	return XMFLOAT3(v_4f.x/ v_4f.w, v_4f.y / v_4f.w, v_4f.z / v_4f.w);
}

bool SoftRender::updateZBuffer(const XMFLOAT3& v)
{
	if (v.z <= zBuffer(v.x, v.y))
	{
		zBuffer(v.x, v.y) = v.z;
		return true;
	}
	return false;
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

void SoftRender::ClearRenderTargetView(INT ColorRBGA)
{
	BackBuffer.Clear(ColorRBGA);
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

