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
#include "System/Window/WindowManager.h"	// File Header
#include "System/Window/Window.h"
#include "System/SystemMessage.h"
#include "System/Driver/Driver.h"
#include "System/Input/MouseData.h"
#include "System/Logger/LoggerSystem.h"

#include "IO/TextSerialiser.h"
#include "Editor/Editor.h"

#include "Globals.h"
#include "Utility/DebugAssert.h"

#define WIN32_LEAN_AND_MEAN					// Exclude rarely used stuff from Windows headers
#define NOMINMAX							// Disable window's min & max macros
#include <cstdio>							// FILE, freopen_s
#include <windows.h>						// Windows Header
#include <shellapi.h>
#include "../../../resource.h"

#undef  WIN32_LEAN_AND_MEAN					// Stop defines from spilling into code
#undef  NOMINMAX

#define ENGINE_NAME		L"Dystopia Engine"


namespace
{
	constexpr long	DEFAULT_WINDOWSTYLE		= WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	constexpr long	DEFAULT_WINDOWSTYLE_EX	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;


	Dystopia::MouseData* pMouse = nullptr;

	LRESULT WINAPI MessageProcessor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static auto oldsz = std::make_pair(0, 0);
		switch (message)
		{
		case WM_SETFOCUS:
			Dystopia::EngineCore::GetInstance()->BroadcastMessage(Dystopia::eSysMessage::FOCUS_GAIN);
			break;

		case WM_KILLFOCUS:
			Dystopia::EngineCore::GetInstance()->BroadcastMessage(Dystopia::eSysMessage::FOCUS_LOST);
			break;

		case WM_SIZE:
			if(SIZE_MAXIMIZED == wParam)
			{
				RECT scr;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &scr, 0);
				oldsz.first = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().GetWidth();
				oldsz.second = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().GetHeight();

				Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().
					SetSize(scr.right - scr.left, scr.bottom - scr.top);

				Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->ToggleFullscreen(true);
			}
			else if (SIZE_RESTORED == wParam)
			{
				if (oldsz.first)
				{
					Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().SetSize(oldsz.first, oldsz.second);
					oldsz.first = 0; oldsz.second = 0;
					Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->ToggleFullscreen(false);
				}
			}
			break;

		case WM_CLOSE:
			Dystopia::EngineCore::GetInstance()->BroadcastMessage(Dystopia::eSysMessage::QUIT);
			return 0;

		case WM_MOUSEWHEEL:
			if (pMouse)
				pMouse->mnWheel = GET_WHEEL_DELTA_WPARAM(wParam);
			break;

		default:
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

Dystopia::WindowManager::WindowManager(void) : 
	mbFullscreen{ Gbl::FULLSCREEN }, mHInstance { GetModuleHandle(nullptr) },
	mWindows{ 1 }
{

}

Dystopia::WindowManager::~WindowManager(void)
{
	UnregisterClass(L"MainWindow", mHInstance);
}

void Dystopia::WindowManager::PreInit(void)
{
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

	HWND window = CreateWindowEx(
		WS_EX_APPWINDOW,
		L"MainWindow",
		L"Dystopia 2018.01.1a",
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		100, 100,
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

	long left = (GetSystemMetrics(SM_CXSCREEN) - mWidth) >> 1,
		top = (GetSystemMetrics(SM_CYSCREEN) - mHeight) >> 1;

	// center the window
	SetWindowPos(window, NULL, left, top, 0, 0, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOSIZE | SWP_NOACTIVATE);

#endif

	mWindows.EmplaceBack(window);
//	mWindows[0].ShowCursor(EDITOR);
}

bool Dystopia::WindowManager::Init(void)
{
#if EDITOR

	LoggerSystem::ConsoleLog(eLog::SYSINFO, "Window System: Screen Resolution %dx%d, Main window size %dx%d\n",
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), mWidth, mHeight);

#endif

	return true;
}

void Dystopia::WindowManager::PostInit(void)
{
	this->DestroySplash();

	mWidth  = GetSystemMetrics(SM_CXSCREEN);
	mHeight = GetSystemMetrics(SM_CYSCREEN);

#if EDITOR

	mWindows[0].SetAcceptFiles(true);

#endif
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
			case WM_DROPFILES:
			#if EDITOR
				HandleFileInput(msg.wParam);
			#endif
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
	//PostQuitMessage(0);
}

void Dystopia::WindowManager::LoadDefaults(void)
{
	mbFullscreen	= Gbl::FULLSCREEN;
	mWindowStyle	= DEFAULT_WINDOWSTYLE;
	mWindowStyleEx	= DEFAULT_WINDOWSTYLE_EX;
	mWidth			= Gbl::SCREEN_WIDTH;
	mHeight			= Gbl::SCREEN_HEIGHT;
}

void Dystopia::WindowManager::LoadSettings(DysSerialiser_t& _out)
{
	_out >> mbFullscreen;
	_out >> mWindowStyle;
	_out >> mWindowStyleEx;
	_out >> mWidth;
	_out >> mHeight;
}

void Dystopia::WindowManager::SaveSettings(DysSerialiser_t& _in)
{
	_in << mbFullscreen;
	_in << mWindowStyle;
	_in << mWindowStyleEx;
	_in << DEFAULT_WIDTH;
	_in << DEFAULT_HEIGHT;
}

void Dystopia::WindowManager::ToggleFullscreen(bool _bFullscreen)
{
	mbFullscreen = _bFullscreen;

	if (mbFullscreen)
	{
		/*
		DEVMODE mode{};
		mode.dmSize = sizeof(DEVMODE);
		mode.dmPelsWidth  = mWidth;
		mode.dmPelsHeight = mHeight;
		mode.dmBitsPerPel = 32;
		mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		ChangeDisplaySettings(&mode, 0);
		*/
	}
	else
	{
		//ChangeDisplaySettings(nullptr, 0);
	}
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

bool Dystopia::WindowManager::IsFullscreen() const
{
	return mbFullscreen;
}

Dystopia::Window& Dystopia::WindowManager::GetMainWindow(void) const
{
	return const_cast<Window&>(mWindows[0]);
}

void Dystopia::WindowManager::DestroySplash(void)
{
	mWindows[0].Hide();

	mWindows[0].SetStyle(mWindowStyle, mWindowStyleEx);
	mWindows[0].SetSize(mWidth, mHeight);
	mWindows[0].CenterWindow();

	if (mbFullscreen)
		mWindows[0].ShowMax();
	else
		mWindows[0].Show();
}

void Dystopia::WindowManager::HandleFileInput(uint64_t _wParam)
{
	HDROP handle = reinterpret_cast<HDROP>(_wParam);
	int files = DragQueryFile(handle, 0xFFFFFFFF, 0, 0);

	// For now only handle single file drop
	if (1 == files)
	{
		AutoArray<wchar_t> buf;
		std::wstring path, name;

		buf.reserve(DragQueryFile(handle, 0, 0, 0) + 1);
		DragQueryFile(handle, 0, buf.begin(), static_cast<unsigned>(buf.Cap()));
		
		path = buf.begin();
		if (path == L"")
			__debugbreak();

		size_t pos = path.find_last_of(L'\\');
		if (path.npos == pos)
			pos = path.find_last_of(L'/');

		name = path.substr(pos + 1);

		pos = name.find_last_of(L'.');
		if ((name.npos != pos) && (L"dscene" == name.substr(pos + 1)))
		{
			Editor::GetInstance()->OpenScene(path, name);
		}
		else
		{
			DEBUG_PRINT(eLog::MESSAGE, "Window: Ignoring Unknown file %ls!", name.data());
		}
	}
	else
	{
		DEBUG_PRINT(eLog::MESSAGE, "Window: %d files dropped, ignoring!", files);
	}

	DragFinish(handle);
}


