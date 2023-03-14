#include <Windows.h>
#include "Graphics.h"
#include "SwapChain.h"
#include "CommandQueueManager.h"
#include "Fence.h"
#include "FenceCounter.h"

//
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE:
		{
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		}
		return 0;

	case WM_PAINT:
	{
		
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
//
//int WinMain(HINSTANCE hInstance, HINSTANCE hIgnore, PSTR lpCmdLine, int cmdShow)
//{
//
//
//	WNDCLASSEX wc = {};
//	wc.lpszClassName = "MainWindow";
//	wc.hInstance = hInstance;
//	wc.lpfnWndProc = WinProc;
//	wc.cbSize = sizeof(wc);
//	wc.style = CS_HREDRAW | CS_VREDRAW;
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	RegisterClassEx(&wc);
//
//	HWND hwnd = CreateWindowEx(
//		0,
//		wc.lpszClassName,
//		"Teste",
//		WS_OVERLAPPEDWINDOW,
//		0,
//		0,
//		dxo.width,
//		dxo.height,
//		nullptr,
//		nullptr,
//		hInstance,
//		&dxo
//	);
//
//
//	ShowWindow(hwnd, cmdShow);
//
//	MSG msg = {};
//
//	while (msg.message != WM_QUIT)
//	{
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//	dxo.OnDestroy();
//
//	return static_cast<char>(msg.wParam);
//
//}

int WinMain(HINSTANCE hInstance, HINSTANCE hIgnore, PSTR lpCmdLine, int cmdShow)
{
	Graphics gfx = Graphics();

	DXGI_ADAPTER_DESC3 adDesc;
	gfx.GetAdapterDesc3(&adDesc);

	DXGI_QUERY_VIDEO_MEMORY_INFO memInfo;
	gfx.ReserveVideoMemory(10000u);
	gfx.GetVideoMemory(&memInfo);

	//Get current monitor
	POINT mousePoint;
	GetCursorPos(&mousePoint);
	HMONITOR hMonitor = MonitorFromPoint(mousePoint, MONITOR_DEFAULTTOPRIMARY);

	//Get output
	gfx.FindMonitor(hMonitor);
	DXGI_OUTPUT_DESC outDesc;
	gfx.GetOutputDesc(&outDesc);

	D3D12_FEATURE_DATA_D3D12_OPTIONS6 op;
	gfx.GetFeatureSupportD3D12(D3D12_FEATURE_D3D12_OPTIONS6, op);

	
	CommandQueueManager::GetInstance().createInternalObjects(gfx.GetDevice());

	CommandQueue& queue = CommandQueueManager::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Create fende and objects
	Fence fence(gfx.GetDevice());
	FenceCounter counter(fence);
	FenceCounter::Frontend frontEnd = counter.newFrontEnd();


	WNDCLASSEX wc = {};
	wc.lpszClassName = "MainWindow";
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WinProc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		"Teste",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		1920,
		1080,
		nullptr,
		nullptr,
		hInstance,
		NULL
	);

	SwapChain sc(gfx.GetDevice(), 
		CommandQueueManager::GetInstance().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT),
		gfx.GetFactory(), 
		hwnd, 
		DXGI_FORMAT_R8G8B8A8_UNORM);

	gfx.teste((IDXGISwapChain1*)sc);

	ShowWindow(hwnd, cmdShow);

	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		sc.Present(false);

		queue->Signal(frontEnd, frontEnd.next());
		frontEnd.GetCurrentWaitObject().Wait();

	}

	// flush GPU
	for (int i = 0; i < sc.NumberOfFramesInFlight() - 1; i++)
	{
		queue->Signal(frontEnd, frontEnd.next());
		frontEnd.GetCurrentWaitObject().Wait();
	}
}

