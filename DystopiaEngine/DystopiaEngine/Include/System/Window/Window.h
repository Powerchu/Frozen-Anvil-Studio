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

#include "DataStructure\Queue.h"
#include "System\Input\InputMap.h"

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

		void SetTitle(const std::wstring&);
		void SetStyle(long _nStyle, long _nStyleEx);
		void SetSize(int _nWidth, int _nHeight);

		void CenterWindow(void) noexcept;

		void ShowCursor(int _bShow) const;

		void Show(void) const noexcept;
		void Hide(void) const noexcept;

		Window& operator= (Window&&) = default;

	private:

		HWND mHandle;
		HDC mDeviceContext;

		int mnWidth, mnHeight;
		unsigned long mStyle, mStyleEx;

		Queue<eButton> mInputQueue;
	};

	bool operator== (const Window&, const Window&);
}



#endif		// INCLUDE GUARD

