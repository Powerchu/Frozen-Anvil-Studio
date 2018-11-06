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

static constexpr unsigned long LauncherStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
static constexpr unsigned long LauncherStyleEx = WS_EX_APPWINDOW;

Editor::EditorLauncher::EditorLauncher(void)
	: mProjectSelected{}, mbLaunch{ false }, mOriginStyle{}, mOriginStyleEx{}, mOriginSizeX{}, mOriginSizeY{}
{}

Editor::EditorLauncher::~EditorLauncher(void)
{
}

void Editor::EditorLauncher::Init(void)
{
	const auto& wmgr = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>();
	auto& win = wmgr->GetMainWindow();
	mOriginStyle = wmgr->GetStyle();
	mOriginStyleEx = wmgr->GetStyleEx();
	mOriginSizeX = wmgr->GetScreenWidth();
	mOriginSizeY = wmgr->GetScreenHeight();
	win.Hide();
	win.SetStyle(LauncherStyle, LauncherStyleEx);
	win.SetSize(LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
	win.CenterWindow();
	win.SetSizeNoAdjust(LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
	win.CenterWindow();
	win.Show();
	wmgr->mWidth = GetSystemMetrics(SM_CXSCREEN);
	wmgr->mHeight = GetSystemMetrics(SM_CYSCREEN);

	EditorMain::GetInstance()->GetSystem<EditorUI>()->SetLauncherMode(true);
}

void Editor::EditorLauncher::StartFrame(void)
{
	Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->Update(0.016f);
	EditorMain::GetInstance()->GetSystem<EInput>()->StartFrame();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->StartFrame();
	ImGui::SetNextWindowPos(ImVec2{ 0,0 });
}

void Editor::EditorLauncher::Update(float)
{
	EditorMain::GetInstance()->GetSystem<EditorUI>()->Update(0.016f);

	ImGui::Begin("Gello");
	ImGui::Button("Button", ImVec2{1100, 500});
	ImGui::End();
}

void Editor::EditorLauncher::EndFrame(void)
{
	EditorMain::GetInstance()->GetSystem<EInput>()->EndFrame();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->EndFrame();
}

void Editor::EditorLauncher::Shutdown(void)
{
	const auto& wmgr = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>();
	auto& win = wmgr->GetMainWindow();
	win.Hide();
	win.SetStyle(mOriginStyle, mOriginStyleEx);
	win.SetSize(mOriginSizeX, mOriginSizeY);
	win.CenterWindow();
	win.Show();
	wmgr->mWidth = GetSystemMetrics(SM_CXSCREEN);
	wmgr->mHeight = GetSystemMetrics(SM_CYSCREEN);
	
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






















