/* HEADER *********************************************************************************/
/*!
\file	Window.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Encapsulates window manipulation

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
	mInputQueue{ DEFAULT_QUEUE_SIZE }
{

}

HWND Dystopia::Window::GetWindowHandle(void) const
{
	return mHandle;
}

HDC Dystopia::Window::GetDeviceContext(void) const
{
	return mDeviceContext;
}

const Queue<eButton>& Dystopia::Window::GetInputQueue(void) const
{
	return mInputQueue;
}

void Dystopia::Window::SetTitle(const std::wstring& _strTitle)
{
	SetWindowText(mHandle, _strTitle.c_str());
}

void Dystopia::Window::SetStyle(long _nStyle, long _nStyleEx)
{
	SetWindowLong(mHandle, GWL_STYLE, _nStyle);
	SetWindowLong(mHandle, GWL_EXSTYLE, _nStyleEx);
}

void Dystopia::Window::ShowCursor(int _bShow) const
{
	::ShowCursor(_bShow);
}


bool Dystopia::operator== (const Window& _lhs, const Window& _rhs)
{
	return _lhs.GetWindowHandle() == _rhs.GetWindowHandle();
}



