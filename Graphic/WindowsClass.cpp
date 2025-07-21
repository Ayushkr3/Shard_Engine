#include"WindowsClass.h"
#include <shellscalingapi.h>
#ifdef ImGUI_ENABLED
#include "Metrices.h"
#include <string>
#include "Global.h"
#pragma comment(lib, "Shcore.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
window::window():
	hint(GetModuleHandle(nullptr))
{
	std::string className = "Engine";
#ifdef ImGUI_ENABLED

#endif
	wndClass.hIcon = nullptr;
	wndClass.hIconSm = nullptr;
	wndClass.lpszMenuName = nullptr;
	wndClass.hCursor = nullptr;
	wndClass.hbrBackground = nullptr;
	wndClass.lpszMenuName = nullptr;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.cbSize = sizeof(wndClass);
	wndClass.hInstance = hint;
	wndClass.lpfnWndProc = HandleMsgSetup;
	wndClass.lpszClassName = className.c_str();
	wndClass.style = CS_OWNDC;
	RegisterClassEx(&wndClass);

	winRef = new WindowRef;
	scaledWidth = MulDiv(1620, 96, 144);
	scaledHeight = MulDiv(940, 96, 144);
	winRef->Additional_Param.heightY = &scaledHeight;
	winRef->Additional_Param.widthX = &scaledWidth;

#ifdef ImGUI_ENABLED
	
	hwnd = CreateWindowEx(
		0,                          // Optional window styles.
		className.c_str(),                     // Window class
		"Engine",				  // Window text
		WS_OVERLAPPEDWINDOW,     // Window style

							   // Size and position
		0, 0, scaledWidth, scaledHeight,

		NULL,				// Parent window    
		NULL,			   // Menu
		hint,			  // Instance handle
		(LPVOID)(winRef)				  // Additional application data
	);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(winRef));
#else 
	hwnd = CreateWindowEx(
		0,                          // Optional window styles.
		className.c_str(),                     // Window class
		"Start",				  // Window text
		WS_OVERLAPPEDWINDOW,     // Window style

							   // Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

		NULL,				// Parent window    
		NULL,			   // Menu
		hint,			  // Instance handle
		NULL			 // Additional application data
	);
#endif // ImGUI_ENABLED
	InputManager::Init();
	DebugConsole::InitDebugConsole();
	ObjectPropertiesFactory::Init();
	UIwindow = std::make_unique<UIElements>(className, hwnd, hint, 0, 0, MulDiv(1600,96,144), MulDiv(900,96,144), 1);
	ObjectPropertiesFactory::InitModule();
	RenderTargetWindows = std::make_unique<UIWindows>(className, hwnd, hint, MulDiv(300, 96, 144), MulDiv(70, 96, 144), MulDiv(1024, 96, 144), MulDiv(576, 96, 144),2);
	pGfx = std::make_unique<Graphic>(RenderTargetWindows.get()->cHwnd);
	//D3DFactory::Init(pGfx->pDevice,pGfx->pContext);
	/*Instantly set with engine device and context if not 
	 UI device made resource will not be binded with engine context*/
	IPC::SetD3D(pGfx->pDevice, pGfx->pContext);
	Scene = std::make_unique<SceneManager>(0, MulDiv(70, 96, 144), MulDiv(300, 96, 144), MulDiv(830, 96, 144));
	SceneManager::currentScene = pGfx->GetCurrentScene();
	Properties = std::make_unique<PropertiesWindow>(MulDiv(1324, 96, 144), MulDiv(70, 96, 144), MulDiv(280, 96, 144), MulDiv(830, 96, 144));
	file = std::make_unique<Files>(MulDiv(300, 96, 144), MulDiv(646, 96, 144), MulDiv(1024, 96, 144), MulDiv(254, 96, 144),pGfx->pDevice,pGfx->pContext);
	Control = std::make_unique<ControlMenu>(0, 0, MulDiv(1600, 96, 144), MulDiv(70, 96, 144),UIwindow,RenderTargetWindows,pGfx);
	UIwindow->UpdateUI();
	Scene->SetSizenWidth();
	Properties->SetSizenWidth();
	file->SetSizenWidth();
	Control->SetSizenWidth();
	UIwindow->Swap();
	SetWindowPos(UIwindow->cHwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(RenderTargetWindows->cHwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	winRef->Additional_Param.ptrFiles = &file;
	winRef->Additional_Param.ptrProp = &Properties;
	winRef->Additional_Param.ptrElem = &UIwindow;
	Globals::inPlayMode = new bool;
	
	*Globals::inPlayMode = false;
	SharedVarInit(Globals::dT,Globals::inPlayMode);
	ShowWindow(hwnd, SW_SHOWDEFAULT);
}
std::optional<int> window::ProcessMessage() {
	MSG msg;
	
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return (int)msg.wParam;
		}

		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

LRESULT CALLBACK window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		window* const pWnd = (window*)pCreate->lpCreateParams;
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::HandleMsgThunk));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window instance
	
	window* pWnd = reinterpret_cast<window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}
LRESULT window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return true;
}
	InputManager::GetInputDevices()->kb->ForwardMessage(msg, lParam, wParam);
	switch (msg)
	{
	case WM_SIZE: {
		WindowRef* const winRef = reinterpret_cast<WindowRef*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (winRef == nullptr)
			return 0;
		UINT height = HIWORD(lParam);//= *winRef->Additional_Param.heightY;
		UINT width = LOWORD(lParam);//*winRef->Additional_Param.widthX;
		//winRef->Additional_Param.ptrElem->get()->SetSizenWidth(width, height);
		//winRef->Additional_Param.ptrFiles->get()->SetSizenWidth(width - *(winRef->Additional_Param.widthX), height - *(winRef->Additional_Param.heightY));
		//winRef->Additional_Param.ptrProp->get()->SetSizenWidth(width - *(winRef->Additional_Param.widthX), height - *(winRef->Additional_Param.heightY));

		*winRef->Additional_Param.heightY = height;
		*winRef->Additional_Param.widthX = width;
		return 0;
	}
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_INPUT:
		InputManager::Read(lParam);
#ifdef ImGUI_ENABLED
		/*if (io.WantCaptureMouse) {
			Mouse::deltaMouseX = 0;
			Mouse::deltaMouseY = 0;*/
			break;
	//}
#endif // ImGUI_ENABLED

		// x = mouse_cont.GetRawData(&raw, &lParam);
		// y = mouse_cont.GetRawData(&raw, &lParam)[1];
		
		return 0;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
#ifdef ImGUI_ENABLED
		/*if (!io.WantCaptureKeyboard){
			if (wParam == VK_UP) {
				Keyboard::isPressedUP = true;
				break;
			}
			if (wParam == VK_DOWN) {
				Keyboard::isPressedDown = true;
				break;
			}
			if (wParam == VK_LEFT) {
				Keyboard::isPressedLeft = true;
				break;
			}
			if (wParam == VK_RIGHT) {
				Keyboard::isPressedRight = true;
				break;
			}*/
			if (wParam == VK_SPACE) {
				PostQuitMessage(0);
			}/* 
			if (wParam == VK_SHIFT) {
				Keyboard::isPressedYPositive = true;
			}
			if (wParam == VK_CONTROL) {
				Keyboard::isPressedYNegative = true;
			}
			if (wParam == VK_RETURN) {
				if (!Keyboard::WantCamControl) {
					Keyboard::WantCamControl = true;
					break;
				}
				else {
					Keyboard::WantCamControl = false;
					break;
				}
			}
		}*/
#endif
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
#ifdef ImGUI_ENABLED
		/*if (!io.WantCaptureKeyboard) {
			if (wParam == VK_UP) {
				Keyboard::isPressedUP = false;
				break;
			}
			if (wParam == VK_DOWN) {
				Keyboard::isPressedDown = false;
				break;
			}
			if (wParam == VK_LEFT) {
				Keyboard::isPressedLeft = false;
				break;
			}
			if (wParam == VK_RIGHT) {
				Keyboard::isPressedRight = false;
				break;
			}
			if (wParam == VK_SHIFT) {
				Keyboard::isPressedYPositive = false;
			}
			if (wParam == VK_CONTROL) {
				Keyboard::isPressedYNegative = false;
			}
			if (wParam == VK_RETURN) {
				Keyboard::WantCamControl = false;
			}
		}*/
#endif
		return 0;
	case WM_CHAR:
#ifdef ImGUI_ENABLED
		/*if (!io.WantCaptureKeyboard) {
			break;
		}*/
#endif
		return 0;
	case WM_MOUSEMOVE:
			InputManager::Ms.MouseX = GET_X_LPARAM(lParam);
			InputManager::Ms.MouseY = GET_Y_LPARAM(lParam);
		
	return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

window::~window() {
#ifdef ImGUI_ENABLED
#endif // ImGUI_ENABLED
	DebugConsole::DestroyConsole();
	D3DFactory::DeInit();
	InputManager::DeInit();
	DestroyWindow(hwnd);
	UnregisterClass("OI",hint);
	ObjectPropertiesFactory::DeInit();
	//mouse_cont.DestroyRawInput();
}
