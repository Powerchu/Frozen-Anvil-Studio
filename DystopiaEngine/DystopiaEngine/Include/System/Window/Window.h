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

typedef struct HINSTANCE__* HINSTANCE;
typedef struct HWND__* HWND;

namespace Dystopia
{

	struct Window
	{

	private:

		HWND mWindow;
	};
}



#endif		// INCLUDE GUARD

