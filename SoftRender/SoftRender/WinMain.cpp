#include "SoftRender.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SoftRender* softRender = new SoftRender();
	bool hr;

	if (!softRender)
	{
		return 0;
	}

	hr = softRender->Initialize(hInstance, nCmdShow, 800, 600);
	if (!hr)
		return 0;

	softRender->Run();

	softRender->Shutdown();
	delete softRender;
	softRender = nullptr;
	
	return 0;
}