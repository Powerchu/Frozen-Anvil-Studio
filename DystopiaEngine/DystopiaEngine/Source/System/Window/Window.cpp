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
#include "System\Window\Window.h"		// File Header
#include "DataStructure\Queue.h"		// Queue
#include "System\Input\InputMap.h"		// eButton

#define WIN32_LEAN_AND_MEAN					// Exclude rarely used stuff from Windows headers
#define NOMINMAX							// Disable window's min & max macros
#include <windows.h>						// Windows Header

#undef  WIN32_LEAN_AND_MEAN					// Stop defines from spilling into code
#undef  NOMINMAX

constexpr unsigned DEFAULT_QUEUE_SIZE = 60;


Dystopia::Window::Window(HWND _handle) :
	mHandle{ _handle }, mDeviceContext{ GetDC(_handle) }, 
	mInputQueue{ DEFAULT_QUEUE_SIZE }, mnWidth{ 0 }, mnHeight{ 0 }
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

const Queue<eButton>& Dystopia::Window::GetInputQueue(void) const noexcept
{
	return mInputQueue;
}

void Dystopia::Window::SetTitle(const std::wstring& _strTitle)
{
	SetWindowText(mHandle, _strTitle.c_str());
}

void Dystopia::Window::SetStyle(long _nStyle, long _nStyleEx)
{
	mStyle = _nStyle;
	mStyleEx = _nStyleEx;
	SetWindowLong(mHandle, GWL_STYLE, mStyle);
	SetWindowLong(mHandle, GWL_EXSTYLE, mStyleEx);
}

void Dystopia::Window::SetSize(int _nWidth, int _nHeight)
{
	RECT WindowRect{ 0, 0, _nWidth, _nHeight };
	AdjustWindowRect(&WindowRect, mStyle, FALSE);

	mnWidth  = WindowRect.right - WindowRect.left;
	mnHeight = WindowRect.bottom - WindowRect.top;
}

void Dystopia::Window::CenterWindow(void) noexcept
{
	int left = (GetSystemMetrics(SM_CXSCREEN) - mnWidth) >> 1,
		top  = (GetSystemMetrics(SM_CYSCREEN) - mnHeight) >> 1;

	// center the window
	SetWindowPos(GetWindowHandle(), NULL,
		left, top, mnWidth, mnHeight,
		SWP_NOZORDER | SWP_NOACTIVATE
	);
}


void Dystopia::Window::ShowCursor(int _bShow) const
{
	::ShowCursor(_bShow);
}

void Dystopia::Window::Show(void) const noexcept
{
	ShowWindow(GetWindowHandle(), SW_SHOW);
}

void Dystopia::Window::Hide(void) const noexcept
{
	ShowWindow(GetWindowHandle(), SW_HIDE);
}


bool Dystopia::operator== (const Window& _lhs, const Window& _rhs)
{
	return _lhs.GetWindowHandle() == _rhs.GetWindowHandle();
}



