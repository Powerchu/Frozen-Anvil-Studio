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

#include "Editor/EGUI.h"

#include "GL/glew.h"

#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"

#include "System/Driver/Driver.h"
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"
#include "System/Input/InputSystem.h"
#include "System/File/FileSystem.h"

#include <Windows.h>
#include <windef.h>
#include <WinUser.h>

#define LAUNCHER_HEIGHT 590
#define LAUNCHER_WIDTH 1010
#define SUBFOLDER_COUNT 11

static constexpr unsigned long LauncherStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
static constexpr unsigned long LauncherStyleEx = WS_EX_APPWINDOW;
static const char* g_SubFolders[11]=
{
	"Asset",
	"Audio",
	"BehaviourFormat",
	"Behaviours",
	"Editor",
	"Font",
	"Meshes",
	"Prefab",
	"Scene",
	"Shader",
	"Temp"
};

Editor::EditorLauncher::EditorLauncher(void)
	: mProjectSelected{}, mbClosing{ false }, mbProjectView{ true }, mOriginStyle {}, mOriginStyleEx{}, mOriginSizeX{}, mOriginSizeY{}, mArrProjectFolders{},
	mCurrentlySelected{ 0 }, mNameBuffer{}, mLocBuffer{}, mArrKnownProjects{}
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
	
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.3f,0.3f,0.3f, 1.f });
	ImGui::SetNextWindowPos(ImVec2{ 0,0 });
	ImGui::SetNextWindowSize(ImVec2{ LAUNCHER_WIDTH, LAUNCHER_HEIGHT });
	ImGui::Begin("Projects", nullptr, flag);
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.3f,0.3f,0.3f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.6f,0.6f,0.6f,1 });
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1, 1, 1, 1 });
		float width = LAUNCHER_WIDTH - (4 * ImGui::GetStyle().WindowPadding.y);
		float ht = LAUNCHER_HEIGHT - (7 * ImGui::GetStyle().WindowPadding.x);
		float height = ht / 6;
		TopBar(width, height);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.f);
		MainBody(width, ht - height);
		ImGui::PopStyleColor(3);
	}
	ImGui::End();
	ImGui::PopStyleColor();
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

void Editor::EditorLauncher::SaveSettings(Dystopia::TextSerialiser& _out)
{
	const auto& fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
	for (size_t i = 0; i < mArrKnownProjects.size(); ++i)
	{
		if (!fs->CheckPathExist(mArrKnownProjects[i]))
		{
			mArrKnownProjects.FastRemove(i);
			--i;
		}
	}
	_out.InsertStartBlock("EditorLauncher");
	_out << mArrKnownProjects.size();
	for (auto& p : mArrKnownProjects)
		_out << p.c_str();
	_out.InsertEndBlock("EditorLauncher");
}

void Editor::EditorLauncher::LoadSettings(Dystopia::TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	unsigned s = 0;
	_in >> s;
	for (unsigned i = 0; i < s; ++i)
	{
		HashString temp;
		_in >> temp;
		mArrKnownProjects.push_back(temp);
	}
	_in.ConsumeEndBlock();
}

bool Editor::EditorLauncher::IsClosing(void) const
{
	return mbClosing;
}

HashString Editor::EditorLauncher::GetProjectPath(void) const
{
	return mProjectSelected;
}



/****************************************************************** Private Functions *********************************************************************/


void Editor::EditorLauncher::TopBar(float _w, float _h)
{
	float spacing = _w / 40;
	float subHeight = (_h * 0.85f);
	float w1 = 130;
	float w2 = 160.f;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0,0 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ ImGui::GetStyle().WindowPadding.x ,0 });
	ImGui::BeginChild("Top Bar", ImVec2{ _w, _h }, true);
	ImGui::PopStyleVar();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + subHeight * 0.5f);

	ImGui::BeginChild("SubButton1", ImVec2{ w1 + 2.f , subHeight * 0.7f }, true);
	if (InvisibleBtn("Projects", w1, subHeight * 0.6f, mbProjectView))
	{
		mbProjectView = true;
	}
	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	ImGui::EndChild();

	ImGui::SameLine(0, spacing);

	ImGui::BeginChild("SubButton2", ImVec2{ w2 + 2.f , subHeight * 0.7f }, true);
	if (InvisibleBtn("Get Started", w2, subHeight * 0.6f, !mbProjectView))
	{
		mbProjectView = false;
	}
	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	ImGui::EndChild();

	ImGui::EndChild();
	ImGui::PopStyleVar(2);
}

void Editor::EditorLauncher::MainBody(float _w, float _h)
{
	ImGui::BeginChild("Main Body", ImVec2{ _w, _h }, true);
	if (mbProjectView)
	{
		ImGui::BeginChild("ListOfRecentProjects", ImVec2{ _w - (2 * ImGui::GetStyle().WindowPadding.x), _h - (2 * ImGui::GetStyle().WindowPadding.y) }, true );

		ImGui::EndChild();
	}
	else
	{
		float w = _w - (2 * ImGui::GetStyle().WindowPadding.x);
		float h = _h - (2 * ImGui::GetStyle().WindowPadding.y);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ (w / 2) - (w / 3), h / 5 });
		ImGui::BeginChild("CreatingProject", ImVec2{ w, h }, true);
		CreateFields(w, h);
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
}

bool Editor::EditorLauncher::InvisibleBtn(const char* _btn, float _x, float _y, bool _highlight)
{
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFontSize(1);

	ImVec2 btnSize{ _x, _y };
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) return false;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(_btn);
	const ImVec2 label_size = ImGui::CalcTextSize(_btn, NULL, true);
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(btnSize, label_size.x, label_size.y);
	const ImRect bb(pos, ImVec2{ pos.x + size.x, pos.y + size.y });
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id)) return false;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	if (pressed) ImGui::MarkItemValueChanged(id);
	const ImU32 col = ImGui::GetColorU32(_highlight ? ImGuiCol_Text : ImGuiCol_TextDisabled);

	ImGui::PushStyleColor(ImGuiCol_Text, col); 
	ImGui::RenderTextClipped(bb.Min, bb.Max,_btn, NULL, &label_size, style.ButtonTextAlign, &bb);
	if (_highlight)
	{
		auto p = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2{ p.x + _x, p.y }, ImGui::GetColorU32(ImGuiCol_Text), 2.f);
	}
	else if (hovered)
	{
		auto p = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2{ p.x + _x, p.y }, ImGui::GetColorU32(ImGuiCol_TextDisabled), 2.f);
	}
	ImGui::PopStyleColor();

	EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFontSize();
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

void Editor::EditorLauncher::CreateFields(float _x, float _y)
{
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
	float itemWidth = _x / 1.5f;
	float btnX = _x * 0.2f;
	float btnY = _y * 0.1f;
	float folderX = 30.f;
	static constexpr float offsetX = 6.f;

	ImGui::Text("Project Name");
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFontSize(1);
	ImGui::PushItemWidth(itemWidth);
	ImGui::InputText("###Project Name", mNameBuffer, bufSize, flags);
	ImGui::PopItemWidth();
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFontSize();

	ImGui::Text("Location");
	ImGui::PushItemWidth(itemWidth - folderX - offsetX - 13.f);
	auto linePos = ImGui::GetCursorPos();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 10,10 });
	ImGui::InputText("###Location", mLocBuffer, bufSize, ImGuiInputTextFlags_ReadOnly);
	ImGui::PopStyleVar();
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.f);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.6f,0.6f,0.6f,1 });
	ImGui::BeginChild("Emptiness", ImVec2{ 10.f, 40.f});
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + itemWidth - folderX - offsetX + 3.f);
	ImGui::SetCursorPosY(linePos.y + 6.f); 
	if (EGUI::Display::IconFolder("Browse", folderX, 18.f, true))
	{
		HashString folder;
		if (Browse(folder))
		{
			strcpy_s(mLocBuffer, folder.c_str());
		}
	}
	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFontSize(1);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.646f, 0.129f, 0.225f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.863f, 0.088f, 0.294f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.646f, 0.129f, 0.225f, 1.f });
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (itemWidth / 2) - (btnX / 2));
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());
	if (ImGui::ButtonEx("Create", ImVec2{ btnX, btnY }, (strlen(mNameBuffer) && strlen(mLocBuffer)) ? 0 : ImGuiButtonFlags_Disabled))
	{
		const auto& fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
		bool firstF = true;
		unsigned failCount = 1;
		HashString tempName = mNameBuffer;
		size_t pos = tempName.size() - 1;
		while (!(fs->CreateFolder(mNameBuffer, mLocBuffer)))
		{
			if (firstF)
			{
				tempName += failCount;
				pos = tempName.size() - 1;
				firstF = false;
			}
			else
			{
				tempName.erase(pos);
				tempName += failCount;
			}
			failCount++;
		}

		mProjectSelected = mLocBuffer;
		mProjectSelected += "/";
		mProjectSelected += mNameBuffer;
		size_t p = mProjectSelected.find('\\');
		while (p != HashString::nPos)
		{
			mProjectSelected[p] = '/';
			p = mProjectSelected.find('\\');
		}

		for (unsigned i = 0; i < SUBFOLDER_COUNT; ++i)
			fs->CreateFolder(g_SubFolders[i], mProjectSelected);

		mbClosing = true;
		mArrKnownProjects.push_back(mProjectSelected);
	}
	if (ImGui::IsItemHovered() && (strlen(mNameBuffer) && strlen(mLocBuffer)))
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFontSize();
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
}

bool Editor::EditorLauncher::Browse(HashString& _outFolder)
{
	Dystopia::EditorProc p;
	return p.BrowseFolder(_outFolder, Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().GetWindowHandle());
}
#endif






















