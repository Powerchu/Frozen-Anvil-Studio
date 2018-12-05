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

#include "System/Base/Systems.h"		// Base Class
#include "DataStructure/AutoArray.h"	// AutoArray

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
		void LoadSettings(DysSerialiser_t&) override;
		void SaveSettings(DysSerialiser_t&) override;

		void ShowCursor(bool _bShow) const;

		inline int GetScreenWidth(void) const noexcept;
		inline int GetScreenHeight(void) const noexcept;

		Window& GetMainWindow(void) const;

		void RegisterMouseData(MouseData*);


		unsigned long GetStyle(void) const;
		unsigned long GetStyleEx(void) const;

		void DestroySplash(void);
		bool HasWindows(void) const;
		_DLL_EXPORT void ToggleFullscreen(bool _bFullscreen);

		bool IsFullscreen() const;

	private:

		std::wstring mTitle;
		AutoArray<Window> mWindows;

		HINSTANCE mHInstance;

		int mWidth, mHeight;

		unsigned long mWindowStyle;
		unsigned long mWindowStyleEx;
		bool mbFullscreen;

		void HandleFileInput(uint64_t);
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

