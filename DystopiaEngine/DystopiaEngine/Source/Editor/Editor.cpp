/* HEADER *********************************************************************************/
/*!
\file	Editor.cpp
\author Digipen (100%)
\par    email: digipen\@digipen.edu
\brief
	INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR

#include "System\Window\WindowManager.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Input\InputSystem.h"
#include "EGUI.h"

// Entry point for editor
int WinMain(HINSTANCE hInstance, HINSTANCE, char *, int)
{
	hInstance;

	Dystopia::WindowManager *win = new Dystopia::WindowManager{};
	Dystopia::GraphicsSystem *gfx = new Dystopia::GraphicsSystem{};
	Dystopia::InputManager *input = new Dystopia::InputManager{};

	win->LoadDefaults();
	win->Init();
	gfx->InitOpenGL(win->GetWindow());

	if (Dystopia::EGUI::Init(win, gfx, input))
	{
		Dystopia::EGUI::StartFrame();
		Dystopia::EGUI::Render();
		Dystopia::EGUI::Shutdown();
	}

	gfx->Shutdown();
	win->Shutdown();

	delete win;
	delete gfx;

	return 0;
}



#endif		// EDITOR ONLY

