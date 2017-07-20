#include "SoftRender.h"
#include <atlconv.h>
#include <sstream>
#include <directxcolors.h>
#include <cstdio>
#include <omp.h>

#pragma warning(disable:4996)

SimpleVertex vertices[]
{
	{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

	{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

	{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

	{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

	{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

	{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
	{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
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
int shininess = 20;

INT backColor = (INT)(1 * 255) << 16
| (INT)(0 * 255) << 8
| (INT)(0 * 255);

//int SoftRender::screenWidth=800;
//int SoftRender::screenHeight=600;
//Buffer<INT> SoftRender::BackBuffer;
//Buffer<FLOAT> SoftRender::zBuffer;
//Buffer<INT> SoftRender::zBuffer_fake;
//Buffer<XMFLOAT3> SoftRender::Tex;
//SimpleVertex* SoftRender::vs_out= new SimpleVertex[vertexNum];
//Triangle* SoftRender::t_world = new Triangle[triangleNum];
//HANDLE* SoftRender::hThread=new HANDLE[triangleNum];
//LightInfo SoftRender::Light;
//XMFLOAT3 SoftRender::ViewPos = XMFLOAT3(0.0f,3.0f, -4.0f);
//Camera SoftRender::mCamera;


SoftRender::SoftRender() :timer(), paused(false),
fullScreen(false), hWndCaption(L"SoftRender"),
hInst(nullptr), hWnd(nullptr),
rotate_flag_x(false), rotate_flag_y(false)
{
	screenWidth = 800;
	screenHeight = 600;
	vs_out = new SimpleVertex[vertexNum];
	t_world = new Triangle[triangleNum];
	//hThread = new HANDLE[triangleNum];
	ViewPos = XMFLOAT3(0.0f, 3.0f, -3.0f);


	XMVECTOR Eye = XMVectorSet(ViewPos.x, ViewPos.y, ViewPos.z, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, -0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mCamera.LookAt(Eye, At, Up);
	mCamera.UpdateViewMatrix();

	Light.Pos = XMFLOAT3(0.0f, 1.0f, -1.5f);
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

inline void SoftRender::CreateTextureFromFile()
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
			Tex(i, j) = XMFLOAT3(R/255.0f, G /255.0f,  B/255.0f);
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
}

void SoftRender::Shutdown()
{
//	BackBuffer.Release();
	zBuffer.Release();
	zBuffer_fake.Release();
}

inline HRESULT SoftRender::InitWindow(HINSTANCE hInstance, int nCmdShow, UINT screenWidth, UINT screenHeight)
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
	zBuffer_fake.Initialize(screenWidth, screenHeight, 1);
	BackBuffer.Initialize(screenWidth, screenHeight);
	now_bitmap = CreateDIBSection(backbuffDC, &bmp_info, DIB_RGB_COLORS, (void**)(BackBuffer.GetppBuffer()), NULL, 0);
	BackBuffer.Clear(backColor);

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

inline LRESULT CALLBACK SoftRender::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
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

inline void SoftRender::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
	{
		mCamera.Walk(1.0f*dt);
		ViewPos = mCamera.GetPosition3f();
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		mCamera.Walk(-1.0f*dt);
		ViewPos = mCamera.GetPosition3f();
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		mCamera.Strafe(-1.0f*dt);
		ViewPos = mCamera.GetPosition3f();
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		mCamera.Strafe(1.0f*dt);
		ViewPos = mCamera.GetPosition3f();
	}
	
	static bool old_flag_y, cur_flag_y;
	old_flag_y = cur_flag_y;
	cur_flag_y=GetAsyncKeyState('R') & 0x8000;
	if (!old_flag_y && cur_flag_y)
	{
		rotate_flag_y = rotate_flag_y ? false : true;
	}

	static bool old_flag_x, cur_flag_x;
	old_flag_x = cur_flag_x;
	cur_flag_x = GetAsyncKeyState('Q') & 0x8000;
	if (!old_flag_x &&cur_flag_x)
	{
		rotate_flag_x = rotate_flag_x ? false : true;
	}

	mCamera.UpdateViewMatrix();
	View = mCamera.GetView();
	Project = mCamera.GetProj();
}

inline void SoftRender::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);
}

void SoftRender::Draw(const GameTimer& gt)
{
//	zBuffer.Clear(1.0f);
//	zBuffer_fake.Clear(1);
	BackBuffer.Clear(backColor);
	
	UpdateWorldMatrix(gt);
	CalculateNormal();

//#pragma omp parallel for
	//for (int i = 8; i < 9; i++)
	for (int i = 0; i < triangleNum; ++i)
	{
		
			XMFLOAT4 p0 = transViewSpace(vs_out[indices[i * 3]].Pos);
			XMFLOAT4 p1 = transViewSpace(vs_out[indices[i * 3 + 1]].Pos);
			XMFLOAT4 p2 = transViewSpace(vs_out[indices[i * 3 + 2]].Pos);

			/*if (p0.z > 0 && p1.z > 0 && p2.z > 0)
			{
				continue;
			}*/
			if (p0.z < 0 && p1.z < 0 && p2.z < 0)
			{
				continue;
			}
			Triangle t_view(p0, p1, p2);
			XMFLOAT4 p;
			
			XMFLOAT4 p0_pro = transProSpace(p0);
			XMFLOAT4 p1_pro = transProSpace(p1);
			XMFLOAT4 p2_pro = transProSpace(p2);

			if (IsInView(p0_pro))
				p = p0;
			else if(IsInView(p1_pro))
				p = p1;
			else 
				p = p2;

			if (t_view.IsBackCulling(XMVectorSet(p.x, p.y, p.z, 0.0f)))
			{
				//			hThread[i] = nullptr;
				continue;
			}


			Triangle* t = new Triangle(p0_pro, p1_pro, p2_pro, screenWidth / 2, screenHeight / 2, indices + i * 3);
		
			DrawTriangle3D(t);
			//		hThread[i]=CreateThread(NULL, 0, DrawTriangle3D, (LPVOID)t, 0, NULL);
		
	}

	SelectObject(backbuffDC, now_bitmap);
	BitBlt(hdc, 0, 0, screenWidth, screenHeight, backbuffDC, 0, 0, SRCCOPY);
	BitBlt(hdc, 0, 0, screenWidth, screenHeight, backbuffDC, 0, 0, SRCCOPY);
	
//for (int i = 0; i < triangleNum; ++i)
	//{
	//	if (hThread)
	//		WaitForSingleObject(hThread[i], INFINITE);
	//}

	//for (int i = 0; i < triangleNum; ++i)
	//	if (hThread)
	//		CloseHandle(hThread[i]);
}

inline void SoftRender::UpdateWorldMatrix(const GameTimer& gt)
{
	static float cur_rotate_x = 0, old_rotate_x = 0;
	static float cur_rotate_y = 0, old_rotate_y = 0;
	if (rotate_flag_x)
	{
		old_rotate_x = cur_rotate_x;
		cur_rotate_x = cur_rotate_x + gt.DeltaTime();
	}
	else
	{
		cur_rotate_x = old_rotate_x;
	}
	if (rotate_flag_y)
	{
		old_rotate_y = cur_rotate_y;
		cur_rotate_y = cur_rotate_y + gt.DeltaTime();
	}
	else
	{
		cur_rotate_y = old_rotate_y;
	}
	World = XMMatrixRotationX(cur_rotate_x)*XMMatrixRotationY(cur_rotate_y);
}

inline void SoftRender::CalculateNormal()
{
	for (int i = 0; i < triangleNum; ++i)
	{
		//fragPos
		for (int j = 0; j < 3; ++j)
		{
			vs_out[indices[i * 3 + j]].Pos = transWorldSpace(vertices[indices[i * 3 + j]].Pos);
		}

		//normal
		t_world[i].Initialize(vs_out[indices[i * 3]].Pos, vs_out[indices[i * 3 + 1]].Pos, vs_out[indices[i * 3 + 2]].Pos);
		
		for (int j = 0; j < 3; ++j)
		{
			vertices[indices[j]].Normal.x += t_world[i].GetNormal3f().x;
			vertices[indices[j]].Normal.y += t_world[i].GetNormal3f().y;
			vertices[indices[j]].Normal.z += t_world[i].GetNormal3f().z;
		}
	}
	for (int i = 0; i < vertexNum; ++i)
	{
		//frag_normal
		XMStoreFloat3(&vertices[i].Normal,
			XMVector3Normalize(XMLoadFloat3(&vertices[i].Normal)));
		/*XMStoreFloat3(&vs_out[i].Normal, XMVector4Transform(
			XMVectorSet(vertices[i].Normal.x, vertices[i].Normal.y, vertices[i].Normal.z, 0),
			MathHelper::InverseTranspose(World)));*/
	}

}

inline DWORD WINAPI SoftRender::DrawTriangle3D(LPVOID lpParameter)
//void SoftRender::DrawTriangle3D(SimpleVertex* vs_out, const WORD indices[3])
{
	Triangle* t = (Triangle*)(lpParameter);
	Triangle t_world(vs_out[t->GetIndex(0)].Pos, vs_out[t->GetIndex(1)].Pos, vs_out[t->GetIndex(2)].Pos);
	//position

	//back-culling
	t->sort_2D_x();

	XMFLOAT4 p0 = t->GetPoint2DPos4f(0);
	XMFLOAT4 p1 = t->GetPoint2DPos4f(1);
	XMFLOAT4 p2 = t->GetPoint2DPos4f(2);

	int i;
	int left = max(floorf(p0.x), -screenWidth/2);
	int right = min(ceil(p2.x), screenWidth / 2);
	int top = min(ceil(MathHelper::Max(p0.y, p1.y, p2.y)), screenHeight / 2.0f);
	int bottom = max(floorf(MathHelper::Min(p0.y, p1.y, p2.y)), screenHeight / -2.0f);
	int scanWidth = right - left;
	int scanHeight = top - bottom;

	//float k01 = slope(p0, p1);
	//float k02 = slope(p0, p2);
	//float k12 = slope(p1, p2);
	//bool flag;

	float y_top = p0.y;
	float y_bottom = p0.y;
	
	XMFLOAT3 p_bary_start, p_bary_start_x1,p_bary_start_y1, p_bary_start_x1_half, p_bary_start_y1_half;

	for (int i = 0; i < 3; ++i)
	{
		if(i==0)
			p_bary_start = transBaryCentric(XMFLOAT4(left, bottom, 0.0f, 0.0f), p0, p1, p2);
		if(i==1)
			p_bary_start_x1 = transBaryCentric(XMFLOAT4(left + 1, bottom, 0.0f, 0.0f), p0, p1, p2);
		if(i==2)
			p_bary_start_y1 = transBaryCentric(XMFLOAT4(left, bottom + 1, 0.0f, 0.0f), p0, p1, p2);
	}


	XMFLOAT3 p_bary_x = XMFLOAT3(p_bary_start_x1.x - p_bary_start.x, 
									p_bary_start_x1.y - p_bary_start.y, 
									p_bary_start_x1.z - p_bary_start.z);
	XMFLOAT3 p_bary_y = XMFLOAT3(p_bary_start_y1.x - p_bary_start.x, 
									p_bary_start_y1.y - p_bary_start.y, 
									p_bary_start_y1.z - p_bary_start.z);

	XMFLOAT3 p_bary_x_half = XMFLOAT3(p_bary_x.x*0.5,
										p_bary_x.y*0.5,
										p_bary_x.z*0.5);
	XMFLOAT3 p_bary_y_half = XMFLOAT3(p_bary_y.x*0.5,
										p_bary_y.y*0.5,
										p_bary_y.z*0.5);
	//for (int i = left; i <= right; ++i)
	//{
	//	if (i <= ceil(p1.x))
	//	{
	//		if (flag)
	//		{
	//			y_top += k01;
	//			y_bottom += k02;
	//		}
	//		else
	//		{
	//			y_top += k02;
	//			y_bottom += k01;
	//		}
	//		for (int i = ceil(y_bottom); i < floor(y_top); ++i)
	//		{

	//		}
	//	}
	//	else
	//	{

	//	}
	//}

	//for (int i = right; i >= ceil(p1.x); --i)
	//{

	//}

#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i <= scanWidth; ++i)
	{
 //		#pragma omp parallel for
		bool flag = false;
		for (int j = 0; j <= scanHeight; ++j)
		{
			XMFLOAT3 p_bary, p_bary_left, p_bary_bottom;
			p_bary.x = p_bary_start.x + p_bary_x.x * i + p_bary_y.x * j;
			p_bary.y = p_bary_start.y + p_bary_x.y * i + p_bary_y.y * j;
			p_bary.z = p_bary_start.z + p_bary_x.z * i + p_bary_y.z * j;

			p_bary_bottom.x = p_bary.x - p_bary_y_half.x;
			p_bary_bottom.y = p_bary.y - p_bary_y_half.y;
			p_bary_bottom.z = p_bary.z - p_bary_y_half.z;

			p_bary_left.x = p_bary.x - p_bary_x_half.x;
			p_bary_left.y = p_bary.y - p_bary_x_half.y;
			p_bary_left.z = p_bary.z - p_bary_x_half.z;

			SimpleVertex v;
			v.Pos.x = left + i;
			v.Pos.y = bottom + j;

			int ScreenPosX = screenWidth / 2.0f + v.Pos.x;
			int ScreenPosY = screenHeight / 2.0f + v.Pos.y;

			if (!MathHelper::IsClamp(ScreenPosX, 0, screenWidth) 
				|| !MathHelper::IsClamp(ScreenPosY, 0, screenHeight))
				continue;

			bool cur_flag = IsInTriangle(p_bary_left)||IsInTriangle(p_bary_bottom)|| IsInTriangle(p_bary);
			if (flag && !cur_flag)
			{
				break;
			}
			else if (cur_flag)
			{
				flag = cur_flag;
				//perspective-correct
				XMFLOAT4 p_correct_bary = transPerspectiveCorrect(p_bary, p0.w, p1.w, p2.w);
				if (p_correct_bary.w < mCamera.GetNearZ()|| p_correct_bary.w > mCamera.GetFarZ())
				{
					continue;
				}
				//	v.Pos.z = NormalizeProjectZ(p_correct_bary.w);

				//	if (updateZBuffer(ScreenPosX, ScreenPosY, v.Pos.z))
				//	{

			//	if (zBuffer_fake(ScreenPosX, ScreenPosY))
			//	{
			//		zBuffer_fake(ScreenPosX, ScreenPosY) = 0;


					//TexCoords
					v.Tex.x = vs_out[t->GetIndexByOrder(0)].Tex.x * p_correct_bary.x
						+ vs_out[t->GetIndexByOrder(1)].Tex.x * p_correct_bary.y
						+ vs_out[t->GetIndexByOrder(2)].Tex.x * p_correct_bary.z;

					v.Tex.y = vs_out[t->GetIndexByOrder(0)].Tex.y * p_correct_bary.x
						+ vs_out[t->GetIndexByOrder(1)].Tex.y * p_correct_bary.y
						+ vs_out[t->GetIndexByOrder(2)].Tex.y * p_correct_bary.z;

					XMFLOAT3 TexColor = getBilinearFilteredPixelColor(Tex, v.Tex.x, v.Tex.y);
					//INT tex_x = v.Tex.x * (Tex.GetWidth() - 1);
					//INT tex_y = v.Tex.y* (Tex.GetHeight() - 1);
					//tex_x = MathHelper::Clamp(tex_x, 0, (int)Tex.GetWidth() - 1);
					//tex_y = MathHelper::Clamp(tex_y, 0, (int)Tex.GetHeight() - 1);
					//XMFLOAT3 TexColor = Tex(tex_x,tex_y);
					FLOAT R = TexColor.x;
					FLOAT G = TexColor.y;
					FLOAT B = TexColor.z;

					//Normal
					SimpleVertex v_world;
					v_world.Pos.x = vs_out[t->GetIndexByOrder(0)].Pos.x * p_correct_bary.x
						+ vs_out[t->GetIndexByOrder(1)].Pos.x * p_correct_bary.y
						+ vs_out[t->GetIndexByOrder(2)].Pos.x * p_correct_bary.z;

					v_world.Pos.y = vs_out[t->GetIndexByOrder(0)].Pos.y * p_correct_bary.x
						+ vs_out[t->GetIndexByOrder(1)].Pos.y * p_correct_bary.y
						+ vs_out[t->GetIndexByOrder(2)].Pos.y * p_correct_bary.z;

					v_world.Pos.z = vs_out[t->GetIndexByOrder(0)].Pos.z * p_correct_bary.x
						+ vs_out[t->GetIndexByOrder(1)].Pos.z * p_correct_bary.y
						+ vs_out[t->GetIndexByOrder(2)].Pos.z * p_correct_bary.z;

					if (p_correct_bary.x == 1.0f)
					{
						v_world.Normal = vs_out[t->GetIndexByOrder(0)].Normal;
					}
					else if (p_correct_bary.y == 1.0f)
					{
						v_world.Normal = vs_out[t->GetIndexByOrder(1)].Normal;
					}
					else if (p_correct_bary.z == 1.0f)
					{
						v_world.Normal = vs_out[t->GetIndexByOrder(2)].Normal;
					}
					else
					{
						v_world.Normal = t_world.GetNormal3f();
					}

					//Light
					XMVECTOR world_pos = XMLoadFloat4(&v_world.Pos);
					XMVECTOR light_pos = XMLoadFloat3(&Light.Pos);

					XMVECTOR lightDir = XMVector3Normalize(XMVectorSubtract(light_pos, world_pos));
					XMVECTOR viewDir = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&ViewPos), world_pos));
					XMVECTOR halfwayDir = XMVector3Normalize(lightDir + viewDir);

					//attenuation
					XMFLOAT3 distance;
					XMStoreFloat3(&distance, XMVector3Length(XMVectorSubtract(light_pos, world_pos)));
					float attenuation = 1.0f / (Light.Constant + Light.Linear * distance.x + Light.Quadratic * (distance.x * distance.x));

					//ambient

					//diffuse
					float diff = max(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v_world.Normal), lightDir)), 0.0);

					//specular
					float dot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v_world.Normal), halfwayDir));
					float spec =SoftRender::pow(dot, shininess);


					R *= (Light.Ambient.x + diff*Light.Diffuse.x + spec*Light.Specular.x)*Light.Color.x*attenuation*255.0f;
					G *= (Light.Ambient.y + diff*Light.Diffuse.y + spec*Light.Specular.y)*Light.Color.y*attenuation*255.0f;
					B *= (Light.Ambient.z + diff*Light.Diffuse.z + spec*Light.Specular.z)*Light.Color.z*attenuation*255.0f;

					/*R *= 255.0f;
					G *= 255.0f;
					B *= 255.0f;*/

					R = min(R, 255.0f);
					G = min(G, 255.0f);
					B = min(B, 255.0f);


					//bilinear-filter
			//		BackBuffer(ScreenPosX, ScreenPosY) = R << 16 | G << 8 | B;

					BackBuffer.SetBuffer(ScreenPosX, ScreenPosY, (int)R << 16 | (int)G << 8 | (int)B);
		//		}
			}
		}
	}

	//for (int i = 0; (i <= ceilf(p1_3f.x) - left)&& k01!= INFINITY && k02 != INFINITY; ++i)
	//{
	//	for (int j = floorf(y_bottom); j <= ceilf(y_top); ++j)
	//	{
	//		SimpleVertex v;
	//		v.Pos = XMFLOAT3(left + i, j, 0);
	//		DrawVextex(v, *t,t_world);
	//	}
	//	if (k01 > k02)
	//	{
	//		y_top = y_top + k01;
	//		y_bottom = y_bottom + k02;
	//	}
	//	else
	//	{
	//		y_top = y_top + k02;
	//		y_bottom = y_bottom + k01;
	//	}
	//}

	//y_top = p2_3f.y;
	//y_bottom = p2_3f.y;

	//for (int i = 0; i <= (right - floor(p1_3f.x)) && k02 != INFINITY && k12 != INFINITY; ++i)
	//{
	//	for (int j = floor(y_bottom); j <= ceil(y_top); ++j)
	//	{
	//		SimpleVertex v;
	//		v.Pos = XMFLOAT3(right - i, j, 0);
	//		DrawVextex(v, *t,t_world);
	//	}
	//	if (k12 < k02)
	//	{
	//		y_top = y_top - k12;
	//		y_bottom = y_bottom - k02;
	//	}
	//	else
	//	{
	//		y_top = y_top - k02;
	//		y_bottom = y_bottom - k12;
	//	}
	//}

//	int loop = ceilf(p1_3f.x) - left;
//
//		for (i = 0; i <= loop; ++i)
//		{
//			top = ceil(y_top);
//			bottom = floor(y_bottom);
//	#pragma omp parallel for
//			for (int j= bottom; j <= top; ++j)
//			{
//				SimpleVertex v;
//				v.Pos = XMFLOAT3(left + i, j, 0);
//				DrawVextex(v, *t, t_world);
//			}
//			
//			if (k01 > k02)
//			{
//				y_top = y_top + k01;
//				y_bottom = y_bottom + k02;
//			}
//			else
//			{
//				y_top = y_top + k02;
//				y_bottom = y_bottom + k01;
//			}
//		}
//	
//
//	y_top = p2_3f.y;
//	y_bottom = p2_3f.y;
//
//	loop = right - floorf(p1_3f.x);
//	
//	for (i = 0; i <= loop; ++i)
//	{
//		bottom = floor(y_bottom);
//		top = ceil(y_top);
//#pragma omp parallel for
//		for (int j = bottom; j <= top; ++j)
//		{
//			SimpleVertex v;
//			v.Pos = XMFLOAT3(right - i, j, 0);
//			DrawVextex(v, *t, t_world);
//		}
//		if (k12 < k02)
//		{
//			y_top = y_top - k12;
//			y_bottom = y_bottom - k02;
//		}
//		else
//		{
//			y_top = y_top - k02;
//			y_bottom = y_bottom - k12;
//		}
//	}

	delete t;
	return 0;
}
//
//void SoftRender::DrawVextex(SimpleVertex& v, const Triangle& t,const Triangle& t_world)
//{
//	if (!MathHelper::IsClamp((int)(screenWidth / 2.0f + v.Pos.x), 0, screenWidth)
//		|| !MathHelper::IsClamp((int)(screenHeight / 2.0f + v.Pos.y), 0, screenHeight))
//		return;
//	//bary-centric
//	XMFLOAT4 p0 = t.GetPoint2DPos4f(0);
//	XMFLOAT4 p1 = t.GetPoint2DPos4f(1);
//	XMFLOAT4 p2 = t.GetPoint2DPos4f(2);
//	
//	XMFLOAT3 p_bary = transBaryCentric(XMFLOAT4(v.Pos.x, v.Pos.y,0.0f,0.0f), p0, p1, p2);
//	if (IsInTriangle(p_bary))
//	{
//		//perspective-correct
//		XMFLOAT4 p_correct_bary = transPerspectiveCorrect(p_bary, p0.w, p1.w, p2.w);
//		v.Pos.z = NormalizeProjectZ(p_correct_bary.w);
//		
//		if (updateZBuffer(screenWidth / 2.0f + v.Pos.x, screenHeight / 2.0f + v.Pos.y, v.Pos.z))
//		{
//			//TexCoords
//			v.Tex.x = vs_out[t.GetIndexByOrder(0)].Tex.x * p_correct_bary.x
//				+ vs_out[t.GetIndexByOrder(1)].Tex.x * p_correct_bary.y
//				+ vs_out[t.GetIndexByOrder(2)].Tex.x * p_correct_bary.z;
//
//			v.Tex.y = vs_out[t.GetIndexByOrder(0)].Tex.y * p_correct_bary.x
//				+ vs_out[t.GetIndexByOrder(1)].Tex.y * p_correct_bary.y
//				+ vs_out[t.GetIndexByOrder(2)].Tex.y * p_correct_bary.z;
//			//v.Tex.x = vs_out[t.GetIndex(0)].Tex.x * p_bary.x
//			//	+ vs_out[t.GetIndex(1)].Tex.x * p_bary.y
//			//	+ vs_out[t.GetIndex(2)].Tex.x * p_bary.z;
//
//			//v.Tex.y = vs_out[t.GetIndex(0)].Tex.y * p_bary.x
//			//	+ vs_out[t.GetIndex(1)].Tex.y * p_bary.y
//			//	+ vs_out[t.GetIndex(2)].Tex.y * p_bary.z;
//
//			INT TexColor = getBilinearFilteredPixelColor(Tex, v.Tex.x, v.Tex.y);
//			INT R = (TexColor >> 16) & 0xFF;
//			INT G = (TexColor >> 8) & 0xFF;
//			INT B = (TexColor) & 0xFF;
//
//			//Normal
//			SimpleVertex v_world;
//			v_world.Pos.x = vs_out[t.GetIndexByOrder(0)].Pos.x * p_correct_bary.x
//				+ vs_out[t.GetIndexByOrder(1)].Pos.x * p_correct_bary.y
//				+ vs_out[t.GetIndexByOrder(2)].Pos.x * p_correct_bary.z;
//
//			v_world.Pos.y = vs_out[t.GetIndexByOrder(0)].Pos.y * p_correct_bary.x
//				+ vs_out[t.GetIndexByOrder(1)].Pos.y * p_correct_bary.y
//				+ vs_out[t.GetIndexByOrder(2)].Pos.y * p_correct_bary.z;
//
//			v_world.Pos.z = vs_out[t.GetIndexByOrder(0)].Pos.z * p_correct_bary.x
//				+ vs_out[t.GetIndexByOrder(1)].Pos.z * p_correct_bary.y
//				+ vs_out[t.GetIndexByOrder(2)].Pos.z * p_correct_bary.z;
//			
//			if (p_correct_bary.x == 1.0f)
//			{
//				v_world.Normal = vs_out[t.GetIndexByOrder(0)].Normal;
//			}
//			else if (p_correct_bary.y == 1.0f)
//			{
//				v_world.Normal = vs_out[t.GetIndexByOrder(1)].Normal;
//			}
//			else if (p_correct_bary.z == 1.0f)
//			{
//				v_world.Normal = vs_out[t.GetIndexByOrder(2)].Normal;
//			}
//			else
//			{
//				v_world.Normal = t_world.GetNormal3f();
//			}
//
//			//Light
//			XMVECTOR lightDir = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&Light.Pos), XMLoadFloat3(&v_world.Pos)));
//			XMVECTOR viewDir = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&ViewPos), XMLoadFloat3(&v_world.Pos)));
//			XMVECTOR halfwayDir = XMVector3Normalize(lightDir + viewDir);
//
//			//attenuation
//			XMFLOAT3 distance;
//			XMStoreFloat3(&distance,XMVector3Length(XMVectorSubtract(XMLoadFloat3(&Light.Pos), XMLoadFloat3(&v_world.Pos))));
//			float attenuation = 1.0f / (Light.Constant + Light.Linear * distance.x + Light.Quadratic * (distance.x * distance.x));
//
//			//ambient
//			
//			//diffuse
//			float diff = max(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v_world.Normal), lightDir)), 0.0);
//			
//			//specular
//			float spec = pow(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v_world.Normal), halfwayDir)), shininess);
//			
//			R *= (Light.Ambient.x + diff*Light.Diffuse.x + spec*Light.Specular.x)*Light.Color.x*attenuation;
//			G *= (Light.Ambient.y + diff*Light.Diffuse.y + spec*Light.Specular.y)*Light.Color.y*attenuation;
//			B *= (Light.Ambient.z + diff*Light.Diffuse.z + spec*Light.Specular.z)*Light.Color.z*attenuation;
//
//			R = min(R, 255);
//			G = min(G, 255);
//			B = min(B, 255);
//			//bilinear-filter
//			BackBuffer(screenWidth / 2.0f + v.Pos.x, screenHeight / 2.0f + v.Pos.y) = R << 16 | G << 8 | B;
//		}
//	}
//}
inline FLOAT SoftRender::pow(const FLOAT a, const INT b)
{
	if (b == 1)
	{
		return a;
	}
	if (b == 0)
	{
		return 1;
	}
	FLOAT x = a*a;
	if (b % 2)
	{
		return a*pow(x, b / 2);
	}
	else
	{
		return pow(x, b / 2);
	}
}

inline FLOAT SoftRender::slope(const XMFLOAT4& a, const XMFLOAT4& b)
{
	if (a.x - b.x == 0)
		return INFINITY;
	return (a.y - b.y) / (a.x - b.x);
}

inline FLOAT SoftRender::NormalizeProjectZ(FLOAT z)
{
	FLOAT NearZ = mCamera.GetNearZ();
	FLOAT FarZ = mCamera.GetFarZ();
	FLOAT A = FarZ / (FarZ - NearZ);
	FLOAT B = -A*NearZ;
	return A + B / z;
}

inline bool SoftRender::IsInView(const XMFLOAT4& p)
{
	return MathHelper::IsClampClosed(p.x, -1.0f, 1.0f)
		&& MathHelper::IsClampClosed(p.y, -1.0f, 1.0f)
		&& MathHelper::IsClampClosed(p.w, mCamera.GetNearZ(), mCamera.GetFarZ())
		&& MathHelper::IsClampClosed(p.z, 0.f, 1.0f);
}

inline XMFLOAT3 SoftRender::getBilinearFilteredPixelColor(Buffer<XMFLOAT3>& tex, double u, double v)
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
		result_x = tex(x, y).x;
		result_y = tex(x, y).y;
		result_z = tex(x, y).z;
	}
	else
	{
		float tex_x_y_R = tex(x, y).x;
		float tex_x_y_G = tex(x, y).y;
		float tex_x_y_B = tex(x, y).z;
		float tex_x1_y_R = tex(x+1, y).x;
		float tex_x1_y_G = tex(x+1, y).y;
		float tex_x1_y_B = tex(x+1, y).z;
		float tex_x_y1_R = tex(x, y+1).x;
		float tex_x_y1_G = tex(x, y + 1).y;
		float tex_x_y1_B = tex(x, y + 1).z;
		float tex_x1_y1_R = tex(x+1, y + 1).x;
		float tex_x1_y1_G = tex(x+1, y + 1).y;
		float tex_x1_y1_B = tex(x+1, y + 1).z;

		result_x = (tex_x_y_R * u_opposite + tex_x1_y_R * u_ratio) * v_opposite +
			(tex_x_y1_R * u_opposite + tex_x1_y1_R * u_ratio) * v_ratio;
		result_y = (tex_x_y_G * u_opposite + tex_x1_y_G * u_ratio) * v_opposite +
			(tex_x_y1_G * u_opposite + tex_x1_y1_G * u_ratio) * v_ratio;
		result_z = (tex_x_y_B * u_opposite + tex_x1_y_B * u_ratio) * v_opposite +
			(tex_x_y1_B * u_opposite + tex_x1_y1_B * u_ratio) * v_ratio;

	}
	
	return XMFLOAT3(result_x,result_y,result_z);
}

inline XMFLOAT3 SoftRender::transBaryCentric(const XMFLOAT4& p, const XMFLOAT4& p0, const XMFLOAT4& p1, const XMFLOAT4& p2)
{
	XMFLOAT3 p_bary;
	FLOAT det = (p1.y - p2.y)*(p0.x - p2.x) + (p2.x - p1.x)*(p0.y - p2.y);
	FLOAT a = ((p1.y - p2.y)*(p.x - p2.x) + (p2.x - p1.x)*(p.y - p2.y)) / det;
	FLOAT b = ((p2.y - p0.y)*(p.x - p2.x) + (p0.x - p2.x)*(p.y - p2.y)) / det;

	return XMFLOAT3(a, b, 1.0f - a - b);
}

inline XMFLOAT4 SoftRender::transPerspectiveCorrect(const XMFLOAT3& p_bary, const FLOAT z0, const FLOAT z1, const FLOAT z2)
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

inline bool SoftRender::IsInTriangle(XMFLOAT3 p_bary)
{
	return p_bary.x>=0 &&
		p_bary.y >= 0 &&
		p_bary.z >= 0 ;
}

inline bool SoftRender::IsInTriangle(XMFLOAT4 p_bary)
{
	return MathHelper::IsClampClosed(p_bary.x, 0.f, 1.0f) &&
		MathHelper::IsClampClosed(p_bary.y, 0.f, 1.0f) &&
		MathHelper::IsClampClosed(p_bary.z, 0.f, 1.0f);
}

inline XMFLOAT4 SoftRender::transViewSpace(const XMFLOAT4& p)
{
	XMFLOAT4 v;

	View = mCamera.GetView();

	XMStoreFloat4(&v, XMVector4Transform(XMVectorSet(p.x, p.y, p.z,p.w), View));

	return v;
}

inline XMFLOAT4 SoftRender::transProSpace(const XMFLOAT4& p)
{
	XMVECTOR v;

	View = mCamera.GetView();
	Project = mCamera.GetProj();

	v = XMVector4Transform(XMVectorSet(p.x, p.y, p.z, p.w), Project);

	return XMFLOAT4(XMVectorGetX(v)/ XMVectorGetW(v), XMVectorGetY(v)/ XMVectorGetW(v), XMVectorGetZ(v)/ XMVectorGetW(v), XMVectorGetW(v));
}

inline XMFLOAT4 SoftRender::transWorldSpace(const XMFLOAT4& p)
{
	XMVECTOR v;

	v = XMVector4Transform(XMVectorSet(p.x, p.y, p.z, p.w), World);

	return XMFLOAT4(XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetW(v));
}

inline bool SoftRender::updateZBuffer(const INT x, const INT y, const INT z)
{
	if (z < zBuffer_fake(x, y))
	{
		zBuffer_fake(x, y) = z;
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

inline void SoftRender::CalculateFrameStats()
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


inline LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
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

