#include "pch.h"

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 614; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }

#include "../Vortex/Vortex.h"
#include "../Vortex/SimpleRenderer.h"

std::shared_ptr<SimpleRenderer> g_renderer;

//CompositionHost* compHost = CompositionHost::GetInstance();


static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Camera* camera = reinterpret_cast<Camera*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

	UINT key = static_cast<UINT>(wParam);
	switch (message)
	{
	case WM_CREATE:
	{
		//LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		//::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		//compHost->SetSwapChain(g_renderer->GetSwapChain());
		//compHost->Initialize(hWnd);
	}
	return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W':
			g_renderer->MoveForward(1.0f);
			break;
		case 'S':
			g_renderer->MoveForward(-1.0f);
			break;
		case 'D':
			g_renderer->MoveRight(1.0f);
			break;
		case 'A':
			g_renderer->MoveRight(-1.0f);
			break;
		case 'E':
			g_renderer->MoveUp(1.0f);
			break;
		case 'Q':
			g_renderer->MoveUp(-1.0f);
			break;
		case VK_UP:
			g_renderer->RotateX(-1.0f);
			break;
		case VK_DOWN:
			g_renderer->RotateX(1.0f);
			break;
		case VK_RIGHT:
			g_renderer->RotateY(1.0f);
			break;
		case VK_LEFT:
			g_renderer->RotateY(-1.0f);
			break;
		}
		return 0;
	case WM_KEYUP:
		return 0;

	case WM_PAINT:
		//camera->Update();
		if (g_renderer)
		{
			g_renderer->Update();
			g_renderer->Render();
		}
		return 0;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
	winrt::init_apartment(winrt::apartment_type::single_threaded);
	int result = ::MessageBox(::GetDesktopWindow(), L"Enter Vortex?", L"Sample", MB_YESNO | MB_ICONQUESTION);
	if (result == IDNO)
	{
		return 0;
	}
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WindowClass";
	RegisterClassEx(&windowClass);


	// Create the window and store a handle to it.
	winrt::handle hwnd{ ::CreateWindowEx(
		WS_EX_CLIENTEDGE,
		windowClass.lpszClassName,
		L"Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		hInstance,
		nullptr)
	};

	RECT rect;
	winrt::check_hresult(::GetWindowRect(static_cast<HWND>(hwnd.get()), &rect));

	g_renderer = std::make_shared<SimpleRenderer>(static_cast<HWND>(hwnd.get()), rect.right - rect.left, rect.bottom - rect.top);



	winrt::check_bool(bool(hwnd));

	::ShowWindow(static_cast<HWND>(hwnd.detach()), nCmdShow);
	// Main sample loop.
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		// Process any messages in the queue.
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return 0;
}
