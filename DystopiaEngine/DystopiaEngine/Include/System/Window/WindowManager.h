/* HEADER *********************************************************************************/
/*!
\file	WindowManager.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Creates and manages Win32 windows.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _WINDOW_MANAGER_H_
#define _WINDOW_MANAGER_H_

#include "System\Base\Systems.h"		// Base Class
#include "DataStructure\AutoArray.h"	// AutoArray

#include <string>

typedef struct HINSTANCE__* HINSTANCE;
typedef struct HWND__* HWND;

namespace Dystopia
{
	class Window;
	struct MouseData;

	class WindowManager : public Systems
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		WindowManager(void);
		~WindowManager(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void PreInit(void);
		bool Init(void);
		void PostInit(void);

		void Update(float _dt);
		void Shutdown(void);
		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);

		void GetSplashDimensions(int& w, int& h);
		void DestroySplash(void);

		void ToggleFullscreen(bool _bFullscreen);
		void ShowCursor(bool _bShow) const;

		inline int GetScreenWidth(void) const noexcept;
		inline int GetScreenHeight(void) const noexcept;

		Window& GetMainWindow(void) const;

		void RegisterMouseData(MouseData*);

	private:

		std::wstring mTitle;
		AutoArray<Window> mWindows;

		HINSTANCE mHInstance;

		int mWidth, mHeight;

		unsigned long mWindowStyle;
		unsigned long mWindowStyleEx;
		bool mbFullscreen;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline int Dystopia::WindowManager::GetScreenWidth(void) const noexcept
{
	return mWidth;
}

inline int Dystopia::WindowManager::GetScreenHeight(void) const noexcept
{
	return mHeight;
}



#endif		// INCLUDE GUARD

