/* HEADER *********************************************************************************/
/*!
\file	Window.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Encapsulates window manipulation

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Window/Window.h"		 // File Header
#include "System/Window/WindowManager.h"
#include "System/Input/InputMap.h"		 // eButton
#include "System/Driver/Driver.h"

#include "DataStructure/Queue.h"		 // Queue


#define WIN32_LEAN_AND_MEAN					// Exclude rarely used stuff from Windows headers
#define NOMINMAX							// Disable window's min & max macros
#include <windows.h>						// Windows Header
#include <shellapi.h>

#undef  WIN32_LEAN_AND_MEAN					// Stop defines from spilling into code
#undef  NOMINMAX

constexpr unsigned DEFAULT_QUEUE_SIZE = 60;


Dystopia::Window::Window(HWND _handle) :
	mHandle{ _handle }, mDeviceContext{ GetDC(_handle) }, 
	mInputQueue{ DEFAULT_QUEUE_SIZE }, mnWidth{ 0 }, mnHeight{ 0 }
	, mbFullscreen(Gbl::FULLSCREEN)
{

}

HWND Dystopia::Window::GetWindowHandle(void) const noexcept
{
	return mHandle;
}

HDC Dystopia::Window::GetDeviceContext(void) const noexcept
{
	return mDeviceContext;
}


int Dystopia::Window::GetWidth(void) const noexcept
{
	return mnWidth;
}

int Dystopia::Window::GetHeight(void) const noexcept
{
	return mnHeight;
}

int Dystopia::Window::GetFullWidth(void) const noexcept
{
	return mnFWidth;
}

int Dystopia::Window::GetFullHeight(void) const noexcept
{
	return mnFHeight;
}


const Queue<eButton>& Dystopia::Window::GetInputQueue(void) const noexcept
{
	return mInputQueue;
}


void Dystopia::Window::SetTitle(const std::wstring& _strTitle)
{
	SetWindowText(mHandle, _strTitle.c_str());
}

std::wstring Dystopia::Window::GetTitle(void) const noexcept
{
	static constexpr unsigned max = 254;
	char buffer[max];
	::GetWindowTextA(mHandle, buffer, max);
	std::string s{ buffer };
	return std::wstring{ s.begin(), s.end() };
}


void Dystopia::Window::SetStyle(long _nStyle, long _nStyleEx) noexcept
{
	PushStyle(mStyle = _nStyle, mStyleEx = _nStyleEx);
}

void Dystopia::Window::PushStyle(long _nStyle, long _nStyleEx) const noexcept
{
	SetWindowLong(mHandle, GWL_STYLE, _nStyle);
	SetWindowLong(mHandle, GWL_EXSTYLE, _nStyleEx);
}


void Dystopia::Window::SetSize(int _nWidth, int _nHeight, bool _bPush) noexcept
{
	mnWidth  = _nWidth;
	mnHeight = _nHeight;

	RECT WindowRect{ 0, 0, _nWidth, _nHeight };
	::AdjustWindowRectEx(&WindowRect, mStyle, FALSE, mStyleEx);

	mnFWidth  = WindowRect.right  - WindowRect.left;
	mnFHeight = WindowRect.bottom - WindowRect.top;

	if (_bPush) PushSize(mnFWidth, mnFHeight);
}

void Dystopia::Window::SetSizeNoAdjust(int _nWidth, int _nHeight, bool _bPush) noexcept
{
	mnFWidth  = mnWidth  = _nWidth;
	mnFHeight = mnHeight = _nHeight;

	if (_bPush) PushSize(mnFWidth, mnFHeight);
}

void Dystopia::Window::CenterWindow(void) noexcept
{
	const int 
		left = (GetSystemMetrics(SM_CXSCREEN) - mnFWidth)  >> 1,
		top  = (GetSystemMetrics(SM_CYSCREEN) - mnFHeight) >> 1;

	// center the window
	PushSize(left, top, mnFWidth, mnFHeight);
}

void Dystopia::Window::PushToFront(void) noexcept
{
	::SetForegroundWindow(GetWindowHandle());

	::UpdateWindow(GetWindowHandle());
}

void Dystopia::Window::PushSize(int w, int h) const noexcept
{
	RECT wr;
	::GetWindowRect(GetWindowHandle(), &wr);
	PushSize(wr.left, wr.top, w, h);
}

void Dystopia::Window::PushSize(int x, int y, int w, int h) const noexcept
{
	::SetWindowPos(GetWindowHandle(), NULL,
		x, y, w, h,
		SWP_NOZORDER | SWP_NOACTIVATE
	);
}


void Dystopia::Window::ShowCursor(int _bShow) const noexcept
{
	::ShowCursor(_bShow);
}

void Dystopia::Window::Show(void) const noexcept
{
	::ShowWindow(GetWindowHandle(), SW_SHOW);
}

void Dystopia::Window::Hide(void) const noexcept
{
	::ShowWindow(GetWindowHandle(), SW_HIDE);
}


void Dystopia::Window::ToggleFullscreen(bool _bFullscreen) noexcept
{
	if (_bFullscreen)
	{
#       if !EDITOR
			PushStyle(
				mStyle & Ut::Constant<long, ~(WS_CAPTION | WS_SIZEBOX)>::value,
				mStyleEx & Ut::Constant<long, ~(WS_EX_WINDOWEDGE)>::value
			);
#       endif

		::ShowWindow(GetWindowHandle(), SW_MAXIMIZE);
	}
	else
	{
#       if !EDITOR
			PushStyle(mStyle, mStyleEx);
			SetSize(mnWidth, mnHeight, true);
#       endif

		Show();
	}
	mbFullscreen = _bFullscreen;
}

bool Dystopia::Window::IsFullscreen() const
{
	return mbFullscreen;
}


void Dystopia::Window::SetAcceptFiles(bool _bAccept) const noexcept
{
	::DragAcceptFiles(GetWindowHandle(), _bAccept ? TRUE : FALSE);
}


bool Dystopia::operator == (const Window& _lhs, const Window& _rhs)
{
	return _lhs.GetWindowHandle() == _rhs.GetWindowHandle();
}



