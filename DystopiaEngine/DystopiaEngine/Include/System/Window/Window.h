/* HEADER *********************************************************************************/
/*!
\file	Window.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Encapsulates window manipulation

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "DataStructure/Queue.h"
#include "System/Input/InputMap.h"

#include <string>

typedef struct HWND__* HWND;
typedef struct HDC__* HDC;

namespace Dystopia
{
	class Window
	{
	public:

		explicit Window(HWND);
		Window(Window&&) = default;

		const Queue<eButton>& GetInputQueue(void) const noexcept;

		HWND GetWindowHandle(void) const noexcept;
		HDC GetDeviceContext(void) const noexcept;

		int GetWidth(void) const noexcept;
		int GetHeight(void) const noexcept;
		int GetFullWidth(void) const noexcept;
		int GetFullHeight(void) const noexcept;

		void SetTitle(const std::wstring&);
		std::wstring GetTitle(void) const noexcept;

		void SetStyle(long _nStyle, long _nStyleEx) noexcept;

		void SetSize(int _nWidth, int _nHeight, bool = true) noexcept;
		void SetSizeNoAdjust(int _nWidth, int _nHeight, bool = true) noexcept;
		void CenterWindow(void) noexcept;

		void ShowCursor(int _bShow) const noexcept;

		void Show(void) const noexcept;
		void Hide(void) const noexcept;

		void ToggleFullscreen(bool _bFullscreen) noexcept;

		void SetAcceptFiles(bool) const noexcept;

		Window& operator = (Window&&) = default;

	private:

		HWND mHandle;
		HDC mDeviceContext;

		int mnWidth, mnHeight;
		int mnFWidth, mnFHeight;
		long mStyle, mStyleEx;

		void PushSize(int w, int h) const noexcept;
		void PushSize(int x, int y, int w, int h) const noexcept;
		void PushStyle(long, long) const noexcept;

		Queue<eButton> mInputQueue;
	};

	bool operator == (const Window&, const Window&);
}



#endif		// INCLUDE GUARD

