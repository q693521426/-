#include "SoftRender.h"
#include <atlconv.h>
#include <sstream>
#include <directxcolors.h>
#include <cstdio>
#include <omp.h>

#pragma warning(disable:4996)

SimpleVertex vertices[]
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

WORD indices[]
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

int vertexNum = 24;
int triangleNum = 12;
int shininess = 60;

INT backColor = (INT)(Colors::MidnightBlue[0] * 255) << 16
| (INT)(Colors::MidnightBlue[1] * 255) << 8
| (INT)(Colors::MidnightBlue[2] * 255);

int SoftRender::screenWidth=800;
int SoftRender::screenHeight=600;
Buffer<INT> SoftRender::BackBuffer;
Buffer<FLOAT> SoftRender::zBuffer;
Buffer<INT> SoftRender::Tex;
SimpleVertex* SoftRender::vs_out= new SimpleVertex[vertexNum];
HANDLE* SoftRender::hThread=new HANDLE[triangleNum];
LightInfo SoftRender::Light;
XMFLOAT3 SoftRender::ViewPos = XMFLOAT3(0.0f, 3.0f, 6.0f);



SoftRender::SoftRender() :timer(), paused(false),
fullScreen(false), hWndCaption(L"SoftRender"),
hInst(nullptr), hWnd(nullptr),
mCamera()
{
	XMVECTOR Eye = XMVectorSet(ViewPos.x, ViewPos.y, ViewPos.z, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mCamera.LookAt(Eye, At, Up);
	mCamera.UpdateViewMatrix();

	Light.Pos = XMFLOAT3(0.0f, 2.0f, -2.0f);
	Light.Color = XMFLOAT3(1.0f,1.0f,1.0f);
	Light.View = MathHelper::LookAt(Light.Pos, XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 1.0f, 0.f));
	Light.Project = XMMatrixPerspectiveFovLH(MathHelper::Pi * 2 / 3, screenWidth / (FLOAT)screenHeight, 1.0f, 100.0f);
	Light.Ambient = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Light.Diffuse = XMFLOAT3(1.0f, 1.0f, 1.0f);
	Light.Specular = XMFLOAT3(1.0f, 1.0f, 1.0f);
	Light.Constant = 1.0f;
	Light.Linear = 0.009;
	Light.Quadratic = 0.0032;
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
		timer.Tick();
		CalculateFrameStats();
		Update(timer);
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
//	BackBuffer.Release();
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

	zBuffer.Initialize(screenWidth, screenHeight,1.0f);
	BackBuffer.Initialize(screenWidth, screenHeight);
	now_bitmap = CreateDIBSection(backbuffDC, &bmp_info, DIB_RGB_COLORS, (void**)(BackBuffer.GetppBuffer()), NULL, 0);

	for (int i = 0; i <triangleNum ; ++i)
	{
		//fragTex
		vs_out[indices[i * 3]].Tex = vertices[indices[i * 3]].Tex;
		vs_out[indices[i * 3 + 1]].Tex = vertices[indices[i * 3 + 1]].Tex;
		vs_out[indices[i * 3 + 2]].Tex = vertices[indices[i * 3 + 2]].Tex;
	}
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
	zBuffer.Clear(1.0f);
	BackBuffer.Clear(backColor);
	
	World = XMMatrixRotationY(gt.TotalTime());
	
	//for (int i = 8; i < 9; i++)
	for (int i = 0; i < triangleNum; ++i)
	{
		//normal
		Triangle t_3d(vertices[indices[i*3]].Pos, vertices[indices[i * 3+1]].Pos, vertices[indices[i * 3+2]].Pos);

		for (int j = 0; j < 3; ++j)
		{
			vertices[indices[j]].Normal.x += t_3d.GetNormal3f().x;
			vertices[indices[j]].Normal.y += t_3d.GetNormal3f().y;
			vertices[indices[j]].Normal.z += t_3d.GetNormal3f().z;
		}

		//fragPos
		vs_out[indices[i * 3]].Pos = transWorldSpace(vertices[indices[i * 3]].Pos);
		vs_out[indices[i * 3 + 1]].Pos = transWorldSpace(vertices[indices[i * 3 + 1]].Pos);
		vs_out[indices[i * 3 + 2]].Pos = transWorldSpace(vertices[indices[i * 3 + 2]].Pos);

		//DrawTriangle3D(vertices, indices + i * 3, hdc);
	}
	for (int i = 0; i < vertexNum; ++i)
	{
		//frag_normal
		XMStoreFloat3(&vertices[i].Normal, 
			XMVector3Normalize(XMLoadFloat3(&vertices[i].Normal)));
		XMStoreFloat3(&vs_out[i].Normal, XMVector4Transform(
			XMVectorSet(vertices[i].Normal.x, vertices[i].Normal.y, vertices[i].Normal.z,0),
			MathHelper::InverseTranspose(World)));
	}
	
	for (int i = 0; i < triangleNum; ++i)
	{
		XMFLOAT3 p0 = transProSpace(vs_out[indices[i*3]].Pos);
		XMFLOAT3 p1 = transProSpace(vs_out[indices[i * 3 + 1]].Pos);
		XMFLOAT3 p2 = transProSpace(vs_out[indices[i * 3 + 2]].Pos);

		Triangle* t=new Triangle(p0, p1, p2, screenWidth / 2, screenHeight / 2, indices+i*3);
		Triangle t_world(vs_out[indices[i * 3]].Pos, vs_out[indices[i * 3+1]].Pos, vs_out[indices[i * 3+2]].Pos);
		if (t_world.IsBackCulling(mCamera.GetLook()))
		{
			hThread[i] = nullptr;
			continue;
		}

		hThread[i]=CreateThread(NULL, 0, DrawTriangle3D, (LPVOID)t, 0, NULL);
		//DrawTriangle3D(vs_out, indices + i * 3);
	}
	//DWORD* exitCode = new DWORD[triangleNum];
	//while (1)
	//{
	//	int i;
	//	for(i=0;i<triangleNum;++i)
	//		GetExitCodeThread(hThread[i],exitCode+i);
	//	for (i = 0; i < triangleNum; ++i)
	//	{
	//		if (exitCode[i] != STILL_ACTIVE)
	//		{
	//			break;
	//		}
	//	}
	//	if (i == triangleNum)
	//		break;
	//}
	/*int tempNumThreads = triangleNum;
	int tempMax = 0;
	while (tempNumThreads >= MAXIMUM_WAIT_OBJECTS)
	{
		tempNumThreads -= MAXIMUM_WAIT_OBJECTS;
		WaitForMultipleObjects(MAXIMUM_WAIT_OBJECTS, &hThread[tempMax], TRUE, INFINITE);
		tempMax += MAXIMUM_WAIT_OBJECTS;
	}*/
	/*DWORD dwRet = 0;
	int nIndex = 0;
	while (1)
	{
		dwRet = WaitForMultipleObjects(triangleNum, hThread, FALSE, INFINITE);
		switch (dwRet)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
			return ;
		default:
		{
			nIndex = dwRet - WAIT_OBJECT_0;
			nIndex++;
			while (nIndex < triangleNum)
			{
				dwRet = WaitForMultipleObjects(triangleNum - nIndex, &hThread[nIndex], false, INFINITE);
				switch (dwRet)
				{
				case WAIT_TIMEOUT:
					nIndex = triangleNum; 
					break;
				case WAIT_FAILED:
					return;
				default:
				{
					nIndex = nIndex + dwRet - WAIT_OBJECT_0;
					nIndex++;
				}
				break;
				}
			}
		}
		break;
		}
	}*/
	for (int i = 0; i < triangleNum; ++i)
	{
		if(hThread)
			WaitForSingleObject(hThread[i], INFINITE);
	}

	for (int i = 0; i < triangleNum; ++i)
		if (hThread)
			CloseHandle(hThread[i]);

}

DWORD WINAPI SoftRender::DrawTriangle3D(LPVOID lpParameter)
//void SoftRender::DrawTriangle3D(SimpleVertex* vs_out, const WORD indices[3])
{
	Triangle* t = (Triangle*)(lpParameter);
	//position
	
	//back-culling
	t->sort_2D_x();

	XMFLOAT3 p0_3f = t->GetPoint2DPos3f(0);
	XMFLOAT3 p1_3f = t->GetPoint2DPos3f(1);
	XMFLOAT3 p2_3f = t->GetPoint2DPos3f(2);

	int left = floorf(p0_3f.x);
	int right = ceil(p2_3f.x);
	int top = floorf(MathHelper::Max(p0_3f.y, p1_3f.y, p2_3f.y));
	int bottom = ceilf(MathHelper::Min(p0_3f.y, p1_3f.y, p2_3f.y));
	int scanWidth = right - left;
	int scanHeight = top - bottom;

	float k01 = slope(p0_3f, p1_3f);
	float k02 = slope(p0_3f, p2_3f);
	float k12 = slope(p1_3f, p2_3f);

	float y_top = p0_3f.y;
	float y_bottom = p0_3f.y;

	for (int i = 0; i <= ceilf(p1_3f.x)-left; ++i)
	{
		for (int j = floorf(y_bottom); j <= ceilf(y_top); ++j)
		{
			SimpleVertex v;
			v.Pos = XMFLOAT3(left + i, j, 0);
			DrawVextex(v, *t);
		}
		if (k01 > k02)
		{
			y_top = y_top + k01;
			y_bottom = y_bottom + k02;
		}
		else
		{
			y_top = y_top + k02;
			y_bottom = y_bottom + k01;
		}
	}

	y_top = p2_3f.y;
	y_bottom = p2_3f.y;

	for (int i = 0; i <= right - floorf(p1_3f.x); ++i)
	{
		if (y_top < y_bottom)
			std::swap(y_top, y_bottom);

		for (int j = floorf(y_bottom); j <= ceilf(y_top); ++j)
		{
			SimpleVertex v;
			v.Pos = XMFLOAT3(right - i, j, 0);
			DrawVextex(v,*t);
		}
		if (k12 < k02)
		{
			y_top = y_top - k12;
			y_bottom = y_bottom - k02;
		}
		else
		{
			y_top = y_top - k02;
			y_bottom = y_bottom - k12;
		}
	}

	//for (int i = 0; i < scanWidth; ++i)
	//{
	//	for (int j = 0; j < scanHeight; ++j)
	//	{
	//		SimpleVertex v;
	//		v.Pos = XMFLOAT3(left + i, bottom + j,0.0f);
	//		
	//		DrawVextex(v, p0_3f, p1_3f, p2_3f, indices, t.GetIndex());
	//	}
	//}
	delete t;
	return 0;
}

void SoftRender::DrawVextex(SimpleVertex& v, const Triangle& t)
{
	if (!MathHelper::IsClamp((int)(ceilf(screenWidth / 2.0f) + v.Pos.x), 0, screenWidth)
		|| !MathHelper::IsClamp((int)(ceilf(screenHeight / 2.0f) + v.Pos.y), 0, screenHeight))
		return;
	//bary-centric
	XMFLOAT3 p0 = t.GetPoint2DPos3f(0);
	XMFLOAT3 p1 = t.GetPoint2DPos3f(1);
	XMFLOAT3 p2 = t.GetPoint2DPos3f(2);

	XMFLOAT3 p_bary = transBaryCentric(v.Pos, p0, p1, p2);
	if (IsInTriangle(p_bary))
	{
		//perspective-correct
		XMFLOAT4 p_correct_bary = transPerspectiveCorrect(p_bary, p0.z, p1.z, p2.z);
		v.Pos.z = p_correct_bary.w;
		if (updateZBuffer(XMFLOAT3(ceilf(screenWidth / 2.0f) + v.Pos.x, ceilf(screenHeight / 2.0f) + v.Pos.y, v.Pos.z)))
		{
			//TexCoords
			v.Tex.x = vs_out[t.GetIndex(0)].Tex.x * p_correct_bary.x
				+ vs_out[t.GetIndex(1)].Tex.x * p_correct_bary.y
				+ vs_out[t.GetIndex(2)].Tex.x * p_correct_bary.z;

			v.Tex.y = vs_out[t.GetIndex(0)].Tex.y * p_correct_bary.x
				+ vs_out[t.GetIndex(1)].Tex.y * p_correct_bary.y
				+ vs_out[t.GetIndex(2)].Tex.y * p_correct_bary.z;

			INT TexColor = getBilinearFilteredPixelColor(Tex, v.Tex.x, v.Tex.y);
			INT R = (TexColor >> 16) & 0xFF;
			INT G = (TexColor >> 8) & 0xFF;
			INT B = (TexColor) & 0xFF;

			//Normal
			SimpleVertex v_world;
			v_world.Pos.x = vs_out[t.GetIndex(0)].Pos.x * p_correct_bary.x
				+ vs_out[t.GetIndex(1)].Pos.x * p_correct_bary.y
				+ vs_out[t.GetIndex(2)].Pos.x * p_correct_bary.z;

			v_world.Pos.y = vs_out[t.GetIndex(0)].Pos.y * p_correct_bary.x
				+ vs_out[t.GetIndex(1)].Pos.y * p_correct_bary.y
				+ vs_out[t.GetIndex(2)].Pos.y * p_correct_bary.z;

			v_world.Pos.z = vs_out[t.GetIndex(0)].Pos.z * p_correct_bary.x
				+ vs_out[t.GetIndex(1)].Pos.z * p_correct_bary.y
				+ vs_out[t.GetIndex(2)].Pos.z * p_correct_bary.z;
			
			if (p_correct_bary.x == 1.0f)
			{
				v_world.Normal = vs_out[t.GetIndex(0)].Normal;
			}
			else if (p_correct_bary.y == 1.0f)
			{
				v_world.Normal = vs_out[t.GetIndex(1)].Normal;
			}
			else if (p_correct_bary.z == 1.0f)
			{
				v_world.Normal = vs_out[t.GetIndex(2)].Normal;
			}
			else
			{
				v_world.Normal = t.GetNormal3f();
			}

			//Light
			XMVECTOR lightDir = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&Light.Pos), XMLoadFloat3(&v_world.Pos)));
			XMVECTOR viewDir = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&ViewPos), XMLoadFloat3(&v_world.Pos)));
			XMVECTOR halfwayDir = XMVector3Normalize(lightDir + viewDir);

			//attenuation
			XMFLOAT3 distance;
			XMStoreFloat3(&distance,XMVector3Length(XMVectorSubtract(XMLoadFloat3(&Light.Pos), XMLoadFloat3(&v_world.Pos))));
			float attenuation = 1.0f / (Light.Constant + Light.Linear * distance.x + Light.Quadratic * (distance.x * distance.x));

			//ambient
			
			//diffuse
			float diff = max(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v_world.Normal), lightDir)), 0.0);
			
			//specular
			float spec = pow(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v_world.Normal), halfwayDir)), shininess);
			
			R *= (Light.Ambient.x + diff*Light.Diffuse.x + spec*Light.Specular.x)*Light.Color.x*attenuation;
			G *= (Light.Ambient.y + diff*Light.Diffuse.y + spec*Light.Specular.y)*Light.Color.y*attenuation;
			B *= (Light.Ambient.z + diff*Light.Diffuse.z + spec*Light.Specular.z)*Light.Color.z*attenuation;

			//bilinear-filter
			BackBuffer(ceilf(screenWidth / 2.0f) + v.Pos.x, ceilf(screenHeight / 2.0f) + v.Pos.y) = R << 16 | G << 8 | B;
		}
	}
}

FLOAT SoftRender::slope(const XMFLOAT3& a, const XMFLOAT3& b)
{
	if (a.x - b.x == 0)
		return INFINITY;
	return (a.y - b.y) / (a.x - b.x);
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

	x = MathHelper::Clamp(x, 0, (int)tex.GetWidth()-1);
	y = MathHelper::Clamp(y, 0, (int)tex.GetHeight()-1);

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

XMFLOAT3 SoftRender::transBaryCentric(const XMFLOAT3& p, const XMFLOAT3& p0, const XMFLOAT3& p1, const XMFLOAT3& p2)
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
	XMVECTOR v;

	View = mCamera.GetView();
	Project = mCamera.GetProj();

	v = XMVector4Transform(XMVectorSet(p.x, p.y, p.z, 1.0f), View*Project);
	
	return XMFLOAT3(XMVectorGetX(v) / XMVectorGetW(v), XMVectorGetY(v) / XMVectorGetW(v), XMVectorGetZ(v) / XMVectorGetW(v));
}

XMFLOAT3 SoftRender::transWorldSpace(const XMFLOAT3& p)
{
	XMVECTOR v;

	v = XMVector4Transform(XMVectorSet(p.x, p.y, p.z, 1.0f), World);

	return XMFLOAT3(XMVectorGetX(v) / XMVectorGetW(v), XMVectorGetY(v) / XMVectorGetW(v), XMVectorGetZ(v) / XMVectorGetW(v));
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

