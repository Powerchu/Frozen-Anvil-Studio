/* HEADER *********************************************************************************/
/*!
\file	WindowManager.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Creates and manages Win32 windows.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Window\WindowManager.h"	// File Header
#include "System\Window\Window.h"
#include "System\SystemMessage.h"
#include "System\Driver\Driver.h"
#include "System\Input\MouseData.h"

#define WIN32_LEAN_AND_MEAN					// Exclude rarely used stuff from Windows headers
#define NOMINMAX							// Disable window's min & max macros
#include <windows.h>						// Windows Header
#include <cstdio>							// FILE, freopen_s
#include "../../../resource.h"

#undef  WIN32_LEAN_AND_MEAN					// Stop defines from spilling into code
#undef  NOMINMAX

#define _COMMANDPROMPT	1
#define ENGINE_NAME		L"Dystopia Engine"

namespace
{
	constexpr long	DEFAULT_WINDOWSTYLE		= WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	constexpr long	DEFAULT_WINDOWSTYLE_EX	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	constexpr bool	DEFAULT_FULLSCREEN		= false;
	constexpr int	DEFAULT_WIDTH			= 1600;
	constexpr int	DEFAULT_HEIGHT			= 900;
	constexpr int	LOGO_WIDTH				= 980;
	constexpr int	LOGO_HEIGHT				= 460;

	Dystopia::MouseData* pMouse = nullptr;

	LRESULT WINAPI MessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SETFOCUS:
			/*
			if (mbFullscreen)
			{
				DEVMODE mode{};
				mode.dmSize = sizeof(DEVMODE);
				mode.dmPelsWidth = mWidth;
				mode.dmPelsHeight = mHeight;
				mode.dmBitsPerPel = 32;
				mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
				ChangeDisplaySettings(&mode, 0);
			} */
			break;
		case WM_KILLFOCUS:
			/*
			if (mbFullscreen)
			{
				ChangeDisplaySettings(NULL, 0);
			} */
			break;

		case WM_CLOSE:
			Dystopia::EngineCore::GetInstance()->BroadcastMessage(Dystopia::eSysMessage::QUIT);
			return 0;

		case WM_MOUSEWHEEL:
			if (pMouse)
				pMouse->mnWheel = GET_WHEEL_DELTA_WPARAM(wParam);

		default:
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

Dystopia::WindowManager::WindowManager(void) : 
	mbFullscreen{ DEFAULT_FULLSCREEN }, mHInstance { GetModuleHandle(nullptr) },
	mWindows{ 1 }
{

}

Dystopia::WindowManager::~WindowManager(void)
{
	UnregisterClass(L"MainWindow", mHInstance);
}

void Dystopia::WindowManager::PreInit(void)
{
#if _COMMANDPROMPT

	if (AllocConsole())
	{
		FILE* file;

		freopen_s(&file, "CONOUT$", "wt", stdout);
		freopen_s(&file, "CONOUT$", "wt", stderr);
//			freopen_s(&file, "CONOUT$", "wt", stdin);

		SetConsoleTitle(ENGINE_NAME);
	}

#endif	// Show Command Prompt

	WNDCLASSEX mainWindow
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		MessageProcessor,
		0, 0,
		GetModuleHandle(NULL),
		LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)),
		LoadCursor(NULL, IDC_ARROW),
		NULL, // Background
		NULL,
		L"MainWindow",
		LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1))
	};

	if (!RegisterClassEx(&mainWindow))
	{
		throw;
	}

#if EDITOR

	RECT WindowRect{ 0, 0, LOGO_WIDTH, LOGO_HEIGHT };
	AdjustWindowRect(&WindowRect, mWindowStyle, FALSE);

	HWND window = CreateWindowEx(
		WS_EX_APPWINDOW,
		L"MainWindow",
		L"Dystopia 2018.01.1a",
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL, NULL, mHInstance, NULL
	);

#else

	RECT WindowRect{ 0, 0, mWidth, mHeight };
	AdjustWindowRect(&WindowRect, mWindowStyle, FALSE);

	HWND window = CreateWindowEx(
		mWindowStyleEx,
		L"MainWindow",
		mTitle.c_str(),
		mWindowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL, NULL, mHInstance, NULL
	);

#endif

	long left = (GetSystemMetrics(SM_CXSCREEN) - LOGO_WIDTH) >> 1,
		top = (GetSystemMetrics(SM_CYSCREEN) - LOGO_HEIGHT) >> 1;

	// center the window
	SetWindowPos(window, NULL, left, top, 0, 0, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOACTIVATE);

	mWindows.EmplaceBack(window);
//	mWindows[0].ShowCursor(EDITOR);

//	ShowWindow(window, SW_SHOW);
//	UpdateWindow(window);
}

bool Dystopia::WindowManager::Init(void)
{
#if EDITOR

	std::fprintf(stdout, "Window System: Screen Resolution %dx%d, Main window size %dx%d\n", 
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), mWidth, mHeight);

#endif

	return true;
}

void Dystopia::WindowManager::PostInit(void)
{
	this->DestroySplash();

	mWidth  = GetSystemMetrics(SM_CXSCREEN);
	mHeight = GetSystemMetrics(SM_CYSCREEN);
}

void Dystopia::WindowManager::Update(float)
{
	MSG msg;
	for (Window& w : mWindows)
	{
		auto& inputQueue = const_cast<Queue<eButton>&>(w.GetInputQueue());
		inputQueue.clear();

		while (!inputQueue.IsFull() && PeekMessage(&msg, w.GetWindowHandle(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);

			switch (msg.message)
			{
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				inputQueue.Insert(static_cast<eButton>(msg.wParam));
				break;
			case WM_KEYUP:
			case WM_SYSKEYUP:
			default:
				break;
			}

			DispatchMessage(&msg);
		}
	}
}

void Dystopia::WindowManager::Shutdown(void)
{
#if _COMMANDPROMPT
	FreeConsole();
#endif

	//PostQuitMessage(0);
}

void Dystopia::WindowManager::LoadDefaults(void)
{
	mbFullscreen	= DEFAULT_FULLSCREEN;
	mWindowStyle	= DEFAULT_WINDOWSTYLE;
	mWindowStyleEx	= DEFAULT_WINDOWSTYLE_EX;
	mWidth			= DEFAULT_WIDTH;
	mHeight			= DEFAULT_HEIGHT;
}

void Dystopia::WindowManager::LoadSettings(TextSerialiser&)
{

}

void Dystopia::WindowManager::ToggleFullscreen(bool _bFullscreen)
{
	mbFullscreen = _bFullscreen;
}

void Dystopia::WindowManager::ShowCursor(bool _bShow) const
{
	for (Window& w : mWindows)
		w.ShowCursor(_bShow);
}

void Dystopia::WindowManager::RegisterMouseData(MouseData* _pMouse)
{
	pMouse = _pMouse;
}

Dystopia::Window& Dystopia::WindowManager::GetMainWindow(void) const
{
	return const_cast<Window&>(mWindows[0]);
}

void Dystopia::WindowManager::GetSplashDimensions(int & w, int & h)
{
	w = LOGO_WIDTH;
	h = LOGO_HEIGHT;
}

void Dystopia::WindowManager::DestroySplash(void)
{
	mWindows[0].Hide();

	mWindows[0].SetStyle(mWindowStyle, mWindowStyleEx);
	mWindows[0].SetSize(mWidth, mHeight);
	mWindows[0].CenterWindow();

	mWindows[0].Show();
}


