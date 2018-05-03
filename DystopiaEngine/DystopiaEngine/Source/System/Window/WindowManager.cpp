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

#define WIN32_LEAN_AND_MEAN					// Exclude rarely used stuff from Windows headers
#define NOMINMAX							// Disable window's min & max macros
#include <windows.h>						// Windows Header
#include <cstdio>							// FILE, freopen_s

#undef  WIN32_LEAN_AND_MEAN					// Stop defines from spilling into code
#undef  NOMINMAX

#define _COMMANDPROMPT	1
#define ENGINE_NAME		L"Dystopia Engine"

constexpr auto	DEFAULT_WINDOWSTYLE		= WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
constexpr auto	DEFAULT_WINDOWSTYLE_EX	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

constexpr bool	DEFAULT_FULLSCREEN		= false;
constexpr int	DEFAULT_WIDTH			= 1600;
constexpr int	DEFAULT_HEIGHT			= 900;
constexpr int	LOGO_WIDTH				= 600;
constexpr int	LOGO_HEIGHT				= 400;

namespace Dystopia
{
	LRESULT WINAPI MessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (message)
		{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);

			EndPaint(hWnd, &ps);
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	WindowManager::WindowManager(void)
		: mbFullscreen{ DEFAULT_FULLSCREEN }, mHInstance { GetModuleHandle(nullptr) }
	{

	}

	WindowManager::~WindowManager(void)
	{

	}

	bool WindowManager::Init(void)
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
	#endif

	#if EDITOR

		WNDCLASSEX editorSplash
		{
			sizeof(WNDCLASSEX),
			CS_CLASSDC,
			MessageProcessor,
			0, 0,
			GetModuleHandle(NULL),
			NULL,
			LoadCursor(NULL, IDC_ARROW),
			NULL, // Background
			NULL,
			L"SplashWindow",
			NULL // Icon
		};

		if (!RegisterClassEx(&editorSplash))
		{
			throw;
		}

	#endif // EDITOR ONLY
		
		WNDCLASSEX mainWindow
		{
			sizeof(WNDCLASSEX),
			CS_CLASSDC,
			MessageProcessor,
			0, 0,
			GetModuleHandle(NULL),
			NULL,
			LoadCursor(NULL, IDC_ARROW),
			NULL, // Background
			NULL,
			L"MainWindow",
			NULL // Icon
		};

		if (!RegisterClassEx(&mainWindow))
		{
			throw;
		}
		
	#if EDITOR

		RECT WindowRect{ 0, 0, LOGO_WIDTH, LOGO_HEIGHT };
		AdjustWindowRect(&WindowRect, mWindowStyle, FALSE);

		mWindow = CreateWindowEx(
			WS_EX_APPWINDOW,
			L"SplashWindow",
			NULL,
			WS_POPUP | WS_DLGFRAME,
			CW_USEDEFAULT, CW_USEDEFAULT,
			WindowRect.right - WindowRect.left,
			WindowRect.bottom - WindowRect.top,
			NULL, NULL, mHInstance, NULL
		);

		//SetWindowLong(mWindow, GWL_STYLE,
		//	GetWindowLong(mWindow, GWL_STYLE)
		//);

		long left = (GetSystemMetrics(SM_CXSCREEN) - LOGO_WIDTH) >> 1,
			top = (GetSystemMetrics(SM_CYSCREEN) - LOGO_HEIGHT) >> 1;
		// center the window
		SetWindowPos(mWindow, NULL, left, top, 0, 0, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOACTIVATE);

	#else

		RECT WindowRect{ 0, 0, mWidth, mHeight };
		AdjustWindowRect(&WindowRect, mWindowStyle, FALSE);

		mWindow = CreateWindowEx(
			mWindowStyleEx,
			L"MainWindow",
			mTitle.c_str(),
			mWindowStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			WindowRect.right - WindowRect.left,
			WindowRect.bottom - WindowRect.top,
			NULL, NULL, mHInstance, NULL
		);

		long left = (GetSystemMetrics(SM_CXSCREEN) - mWidth) >> 1,
			top = (GetSystemMetrics(SM_CYSCREEN) - mHeight) >> 1;
		// center the window
		SetWindowPos(mWindow, NULL, left, top, 0, 0, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOACTIVATE);

	#endif

		::ShowCursor(EDITOR);
		ShowWindow(mWindow, SW_SHOW);
		UpdateWindow(mWindow);

		return true;
	}

	void WindowManager::Update(float)
	{
		//if (GetActiveWindow() == mWindow)
		//{
		//	if (mbFullscreen)
		//	{
		//		DEVMODE mode{};
		//		mode.dmSize = sizeof(DEVMODE);
		//		mode.dmPelsWidth = mWidth;
		//		mode.dmPelsHeight = mHeight;
		//		mode.dmBitsPerPel = 32;
		//		mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		//		ChangeDisplaySettings(&mode, 0);
		//	}
		//	else
		//	{
		//		ChangeDisplaySettings(NULL, 0);
		//	}
		//}

		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowManager::Shutdown(void)
	{
	#if _COMMANDPROMPT
		FreeConsole();
	#endif

		PostQuitMessage(0);
	}

	void WindowManager::LoadDefaults(void)
	{
		mbFullscreen	= DEFAULT_FULLSCREEN;
		mWindowStyle	= DEFAULT_WINDOWSTYLE;
		mWindowStyleEx	= DEFAULT_WINDOWSTYLE_EX;
		mWidth			= DEFAULT_WIDTH;
		mHeight			= DEFAULT_HEIGHT;
	}

	void WindowManager::LoadSettings(TextSerialiser&)
	{

	}

	void WindowManager::ToggleFullscreen(bool _bFullscreen)
	{
		mbFullscreen = _bFullscreen;
	}

	void WindowManager::ShowCursor(bool _bShow) const
	{
		::ShowCursor(_bShow);
	}
}


