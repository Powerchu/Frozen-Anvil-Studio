/* HEADER *********************************************************************************/
/*!
\file	EditorLauncher.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorLauncher.h"
#include "Editor/EditorMain.h"
#include "Editor/EInput.h"
#include "Editor/EditorUI.h"

#include "GL/glew.h"

#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"

#include "System/Driver/Driver.h"
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"

#include <Windows.h>
#include <windef.h>
#include <WinUser.h>

#define LAUNCHER_HEIGHT 600
#define LAUNCHER_WIDTH 1240

Editor::EditorLauncher::EditorLauncher(void)
	: mProjectSelected{}, mbLaunch{ false }
{}

Editor::EditorLauncher::~EditorLauncher(void)
{
}

void Editor::EditorLauncher::Init(void)
{
	auto wmgr = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>();
	wmgr->GetMainWindow().SetSizeNoAdjust(LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
	wmgr->GetMainWindow().CenterWindow();
}

void Editor::EditorLauncher::StartFrame(void)
{
	Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->Update(0.016f);
	EditorMain::GetInstance()->GetSystem<EInput>()->Update(0.016f);

	glViewport(0, 0, LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Editor::EditorLauncher::Update(float _f)
{
}

void Editor::EditorLauncher::EndFrame(void)
{
	auto& win = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow();
	win.Show();
	SwapBuffers(win.GetDeviceContext());
}

bool Editor::EditorLauncher::IsClosing(void) const
{
	return mbLaunch;
}

HashString Editor::EditorLauncher::GetProjectPath(void) const
{
	return mProjectSelected;
}



#endif






















