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
#include "Editor/EditorGLState.h"

#include "GL/glew.h"

#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"

#include "System/Driver/Driver.h"
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"
#include "System/Input/InputSystem.h"

#include <Windows.h>
#include <windef.h>
#include <WinUser.h>

#define LAUNCHER_HEIGHT 600
#define LAUNCHER_WIDTH 1240

static constexpr unsigned long LauncherStyle = WS_CAPTION | WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX;
static constexpr unsigned long LauncherStyleEx = WS_EX_APPWINDOW;

Editor::EditorLauncher::EditorLauncher(void)
	: mProjectSelected{}, mbLaunch{ false }, mOriginStyle{}, mOriginStyleEx{}, mOriginSizeX{}, mOriginSizeY{}
{}

Editor::EditorLauncher::~EditorLauncher(void)
{
}

void Editor::EditorLauncher::Init(void)
{
	auto wmgr = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>();
	mOriginStyle = wmgr->GetStyle();
	mOriginStyleEx = wmgr->GetStyleEx();
	mOriginSizeX = wmgr->GetScreenWidth();
	mOriginSizeY = wmgr->GetScreenHeight();

	wmgr->GetMainWindow().SetStyle(LauncherStyle, LauncherStyleEx);
	wmgr->GetMainWindow().SetSizeNoAdjust(LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
	wmgr->GetMainWindow().CenterWindow();

	EditorMain::GetInstance()->GetSystem<EditorUI>()->SetLauncherMode(true);
}

void Editor::EditorLauncher::StartFrame(void)
{
	Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->Update(0.016f);
	EditorMain::GetInstance()->GetSystem<EInput>()->Update(0.016f);
	EditorMain::GetInstance()->GetSystem<EditorUI>()->StartFrame();
}

void Editor::EditorLauncher::Update(float)
{
	EditorMain::GetInstance()->GetSystem<EditorUI>()->Update(0.016f);
	ImGui::Button("HELLO", ImVec2{ 400, 200 });
}

void Editor::EditorLauncher::EndFrame(void)
{
	EditorMain::GetInstance()->GetSystem<EditorUI>()->EndFrame();
}

void Editor::EditorLauncher::Shutdown(void)
{
	auto wmgr = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>();
	wmgr->GetMainWindow().SetStyle(mOriginStyle, mOriginStyleEx);
	wmgr->GetMainWindow().SetSize(mOriginSizeX, mOriginSizeY);
	wmgr->GetMainWindow().CenterWindow();

	EditorMain::GetInstance()->GetSystem<EditorUI>()->SetLauncherMode(false);
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






















