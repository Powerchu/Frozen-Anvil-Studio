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
#include "Editor/EditorProc.h"

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

#define LAUNCHER_HEIGHT 590
#define LAUNCHER_WIDTH 1010

static constexpr unsigned long LauncherStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
static constexpr unsigned long LauncherStyleEx = WS_EX_APPWINDOW;

Editor::EditorLauncher::EditorLauncher(void)
	: mProjectSelected{}, mbClosing{ false }, mbProjectView{ true }, mOriginStyle {}, mOriginStyleEx{}, mOriginSizeX{}, mOriginSizeY{}, mArrProjectFolders{},
	mCurrentlySelected{ 0 }
{}

Editor::EditorLauncher::~EditorLauncher(void)
{
}

void Editor::EditorLauncher::Init(void)
{
	FindAllProjects();
	SetWindowOptions();
	SetWindowStyles();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->SetLauncherMode(true);
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
		float width = LAUNCHER_WIDTH - (4 * ImGui::GetStyle().WindowPadding.y);
		float ht = LAUNCHER_HEIGHT - (7 * ImGui::GetStyle().WindowPadding.x);
		float height = ht / 5;
		TopBar(width, height);
		MainBody(width, ht - height);
		//ImGui::BeginChild("Segment1", ImVec2{ width1, height1 }, true);
		//for (auto& p : mArrProjectFolders)
		//	ProjectDetails(p);
		//ImGui::EndChild();
		//
		//ImGui::SameLine(0);
		//
		//ImGui::BeginChild("Segment2", ImVec2{ width2 , height1 });
		//	ImGui::BeginChild("Segment3", ImVec2{ width2 , height2 });
		//		if (ImGui::Button("New Project", ImVec2{ btnX, btnY }))
		//		{
		//		}
		//		if (ImGui::Button("Add Existing", ImVec2{ btnX, btnY }))
		//		{
		//		}
		//	ImGui::EndChild();
		//	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFont(1);
		//	ImGui::BeginChild("Segment4", ImVec2{ width2, btnY });
		//		if (ImGui::ButtonEx("Launch Editor", ImVec2{ btnX, btnY }, (mCurrentlySelected != - 1) ? ImGuiButtonFlags_Disabled : 0))
		//		{
		//			mbClosing = true;
		//		}
		//	ImGui::EndChild();
		//	EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFont();
		//ImGui::EndChild();

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
	RemoveWindowStyles();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->SetLauncherMode(false);
}

bool Editor::EditorLauncher::IsClosing(void) const
{
	return mbClosing;
}

HashString Editor::EditorLauncher::GetProjectPath(void) const
{
	return mProjectSelected;
}

void Editor::EditorLauncher::TopBar(float _w, float _h)
{
	float spacing = _w / 40;
	float subHeight = (_h * 0.85f);
	float offsetY = 5.f;
	float w1 = 130;
	float w2 = 160.f;

	ImGui::BeginChild("Top Bar", ImVec2{ _w, _h }, true);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0,0 });
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + subHeight * 0.3f);


	ImGui::BeginChild("SubButton1", ImVec2{ w1 + 2.f , subHeight * 0.7f }, true);
	if (InvisibleBtn("Projects", w1, subHeight * 0.6f, mbProjectView))
	{
		mbProjectView = true;
	}
	ImGui::EndChild();

	ImGui::SameLine(0, spacing);

	ImGui::BeginChild("SubButton2", ImVec2{ w2 + 2.f , subHeight * 0.7f }, true);
	if (InvisibleBtn("Get Started", w2, subHeight * 0.6f, !mbProjectView))
	{
		mbProjectView = false;
	}
	ImGui::EndChild();


	ImGui::PopStyleVar(2);
	ImGui::EndChild();
}

void Editor::EditorLauncher::MainBody(float _w, float _h)
{
	ImGui::BeginChild("Main Body", ImVec2{ _w, _h }, true);
	if (mbProjectView)
	{

	}
	else
	{
	}
	ImGui::EndChild();
}

bool Editor::EditorLauncher::InvisibleBtn(const char* _btn, float _x, float _y, bool _highlight)
{
	ImVec2 btnSize{ _x, _y };
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFont(1);
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(_btn);
	const ImVec2 label_size = ImGui::CalcTextSize(_btn, NULL, true);
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(btnSize, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
	const ImRect bb(pos, ImVec2{ pos.x + size.x, pos.y + size.y });
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	if (pressed)
		ImGui::MarkItemValueChanged(id);
	const ImU32 col = ImGui::GetColorU32(_highlight ? ImGuiCol_Text : hovered ? ImGuiCol_TextSelectedBg : ImGuiCol_TextDisabled);
	ImGui::PushStyleColor(ImGuiCol_Text, col); 
	ImGui::RenderTextClipped(ImVec2{ bb.Min.x + style.FramePadding.x, bb.Min.y + style.FramePadding.y }, 
							 ImVec2{ bb.Max.x - style.FramePadding.x, bb.Max.y - style.FramePadding.y },
							_btn, NULL, &label_size, style.ButtonTextAlign, &bb);
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFont();
	auto p = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddLine(p, ImVec2{ p.x + _x, p.y }, ImGui::GetColorU32(ImGuiCol_Text), 2.f);
	return pressed;
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

void Editor::EditorLauncher::SetWindowStyles(void)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 1.f, 1.f, 1.f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0, 0, 0, 1.f });
}

void Editor::EditorLauncher::RemoveWindowStyles(void)
{
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(2);
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

void Editor::EditorLauncher::ProjectDetails(const HashString& _path)
{

}
#endif






















