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
	: mProjectSelected{}, mbClosing{ false }, mOriginStyle{}, mOriginStyleEx{}, mOriginSizeX{}, mOriginSizeY{}, mArrProjectFolders{},
	mCurrentlySelected{ 0 }
{}

Editor::EditorLauncher::~EditorLauncher(void)
{
}

void Editor::EditorLauncher::Init(void)
{
	FindAllProjects();
	SetWindowOptions();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->SetLauncherMode(true);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 1.f, 1.f, 1.f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0, 0, 0, 1.f });
}

void Editor::EditorLauncher::StartFrame(void)
{
	Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->Update(0.016f);
	EditorMain::GetInstance()->GetSystem<EInput>()->StartFrame();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->StartFrame();
}

void Editor::EditorLauncher::Update(float)
{
	EditorMain::GetInstance()->GetSystem<EditorUI>()->Update(0.016f);
	static constexpr ImGuiWindowFlags flag = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
											 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
											 ImGuiWindowFlags_NoSavedSettings;
	float height1 = LAUNCHER_HEIGHT - (7 * ImGui::GetStyle().WindowPadding.y);
	float width2 = (LAUNCHER_WIDTH / 3) - (7 * ImGui::GetStyle().WindowPadding.x);
	float width1 = (LAUNCHER_WIDTH - width2) - (7 * ImGui::GetStyle().WindowPadding.x);
	float btnX = width2;
	float btnY = 50;
	float height2 = height1 - btnY - (ImGui::GetStyle().WindowPadding.y);
	ImGui::SetNextWindowPos(ImVec2{ 0,0 });
	ImGui::SetNextWindowSize(ImVec2{ LAUNCHER_WIDTH, LAUNCHER_HEIGHT });
	ImGui::Begin("Projects", nullptr, flag);
	{
		ImGui::BeginChild("Segment1", ImVec2{ width1, height1 }, true);
		for (auto& p : mArrProjectFolders)
		{
			ImGui::Selectable(p.c_str());
		}
		ImGui::EndChild();

		ImGui::SameLine(0);

		ImGui::BeginChild("Segment2", ImVec2{ width2 , height1 });
			ImGui::BeginChild("Segment3", ImVec2{ width2 , height2 });
				if (ImGui::Button("New Project", ImVec2{ btnX, btnY }))
				{
				}
				if (ImGui::Button("Button2", ImVec2{ btnX, btnY }))
				{
				}
			ImGui::EndChild();
			ImGui::BeginChild("Segment4", ImVec2{ width2, btnY });
			if (ImGui::Button("Launch", ImVec2{ btnX, btnY }))
				{
				}
			ImGui::EndChild();
		ImGui::EndChild();

	}
	ImGui::End();
}

void Editor::EditorLauncher::EndFrame(void)
{
	EditorMain::GetInstance()->GetSystem<EInput>()->EndFrame();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->EndFrame();
}

void Editor::EditorLauncher::Shutdown(void)
{
	RemoveWindowOptions();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->SetLauncherMode(false);
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);
}

bool Editor::EditorLauncher::IsClosing(void) const
{
	return mbClosing;
}

HashString Editor::EditorLauncher::GetProjectPath(void) const
{
	return mProjectSelected;
}

void Editor::EditorLauncher::SetWindowOptions(void)
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
}

void Editor::EditorLauncher::RemoveWindowOptions(void)
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
}

void Editor::EditorLauncher::FindAllProjects(void)
{
	WIN32_FIND_DATAA data;
	static const HashString projFolder = "Projects/*";
	HANDLE hfind = FindFirstFileA(projFolder.c_str(), &data);
	if (hfind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, "..") && data.dwFileAttributes != FILE_ATTRIBUTE_HIDDEN)
			{
				if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				{
					HashString path{ "/Projects/" };
					path += data.cFileName;
					mArrProjectFolders.push_back(path);
				}
			}
		} while (FindNextFileA(hfind, &data));
		FindClose(hfind);
	}
}

void Editor::EditorLauncher::ProjectDetails(void)
{

}
#endif






















