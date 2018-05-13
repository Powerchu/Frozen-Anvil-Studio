/* HEADER *********************************************************************************/
/*!
\file	Editor.cpp
\author Digipen (100%)
\par    email: digipen\@digipen.edu
\brief
	INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR

#include "System\Window\WindowManager.h"
#include "System\Graphics\GraphicsSystem.h"

// Entry point for editor
int WinMain(HINSTANCE, HINSTANCE, char *, int)
{
	Dystopia::WindowManager *win = new Dystopia::WindowManager{};
	Dystopia::GraphicsSystem *gfx = new Dystopia::GraphicsSystem{};

	win->LoadDefaults();
	win->Init();

	gfx->InitOpenGL(win->GetMainWindow());

	gfx->Shutdown();
	win->Shutdown();

	delete win;
	delete gfx;

	return 0;
}


#endif		// EDITOR ONLY

