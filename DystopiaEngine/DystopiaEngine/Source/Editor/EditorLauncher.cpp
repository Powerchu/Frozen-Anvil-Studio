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
#include "System/Graphics/GraphicsSystem.h"

#include <Windows.h>
#include <windef.h>
#include <WinUser.h>

#define LAUNCHER_HEIGHT 590
#define LAUNCHER_WIDTH 1010
#define SUBFOLDER_COUNT 11

static constexpr unsigned long LauncherStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
static constexpr unsigned long LauncherStyleEx = WS_EX_APPWINDOW;
static const char* g_SubFolders[SUBFOLDER_COUNT]=
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
static constexpr float g_inactiveAlpha = 0.3f;

Editor::EditorLauncher::EditorLauncher(void)
	: mProjFolderSelected{}, mProjFileSelected{}, mbClosing{ false }, mbProjectView{ true }, mOriginStyle {}, mOriginStyleEx{}, mOriginSizeX{}, mOriginSizeY{},
	mCurrentlySelected{ -1 }, mNameBuffer{}, mLocBuffer{}, mArrProjFolders{}
{}

Editor::EditorLauncher::~EditorLauncher(void)
{
}

void Editor::EditorLauncher::Init(void)
{
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
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFontSize(1);

	static constexpr ImGuiWindowFlags flag = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
											 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
											 ImGuiWindowFlags_NoSavedSettings;

	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{ 0,0,0,0 });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{ 0.3f,0.3f,0.3f, 1.f });
	ImGui::SetNextWindowPos(ImVec2{ 0, 0 });
	ImGui::SetNextWindowSize(ImVec2{ LAUNCHER_WIDTH, LAUNCHER_HEIGHT });
	ImGui::Begin("Projects", nullptr, flag);
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.3f,0.3f,0.3f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.6f,0.6f,0.6f,1 });
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1, 1, 1, 1 });
		float width = LAUNCHER_WIDTH - (4 * ImGui::GetStyle().WindowPadding.x);
		float ht = LAUNCHER_HEIGHT - (7 * ImGui::GetStyle().WindowPadding.y);
		float height = ht / 6;
		TopBar(width, height);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.f);
		MainBody(width, ht - height);
		ImGui::PopStyleColor(3);
	}
	ImGui::End();
	ImGui::PopStyleColor(2);
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFontSize();
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
	for (size_t i = 0; i < mArrProjFolders.size(); ++i)
	{
		if (!fs->CheckPathExist(mArrProjFolders[i]))
		{
			mArrProjFolders.FastRemove(i);
			--i;
		}
	}
	_out.InsertStartBlock("EditorLauncher");
	_out << mArrProjFolders.size();
	for (auto& p : mArrProjFolders)
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
		mArrProjFolders.push_back(temp);
	}
	_in.ConsumeEndBlock();
}

bool Editor::EditorLauncher::IsClosing(void) const
{
	return mbClosing;
}

HashString Editor::EditorLauncher::GetProjFolder(void) const
{
	return mProjFolderSelected;
}

HashString Editor::EditorLauncher::GetProjFile(void) const
{
	return mProjFileSelected;
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
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + subHeight * 0.45f);

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
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.646f, 0.129f, 0.225f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.863f, 0.088f, 0.294f, 1.f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.646f, 0.129f, 0.225f, 1.f });

	float w = _w - (2 * ImGui::GetStyle().WindowPadding.x);
	float h = _h - (2 * ImGui::GetStyle().WindowPadding.y);

	ImGui::BeginChild("Main Body", ImVec2{ _w, _h }, true);
	if (mbProjectView)
	{
		auto fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
		for (size_t i = 0; i < mArrProjFolders.size(); ++i)
		{
			if (!(fs->CheckPathExist(mArrProjFolders[i])))
			{
				mArrProjFolders.FastRemove(i);
				i--;
			}
		}

		float sectionW = w * 0.75f;
		float sectionH = h * 0.92f;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::BeginChild("ListOfRecentProjects", ImVec2{ sectionW , sectionH }, true );
		for (size_t i = 0; i < mArrProjFolders.size(); ++i)
		{
			ImGui::PushID(static_cast<int>(i));
			if (ProjectDetails(mArrProjFolders[i], sectionW, sectionH, mCurrentlySelected == static_cast<int>(i)))
				mCurrentlySelected = static_cast<int>(i);
			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::SameLine();
		float leftSectionW = w * 0.23f - ImGui::GetStyle().WindowPadding.x;
		ImGui::BeginChild("Launch", ImVec2{ leftSectionW, sectionH }, true);
		BrowseProject(leftSectionW, sectionH);
		OpenProject(leftSectionW, sectionH);
		ImGui::EndChild();
	}
	else
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ (w / 2.1f) - (w / 3), h / 5.2f });
		ImGui::BeginChild("CreatingProject", ImVec2{ w, h }, true);
		CreateFields(w, h);
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	ImGui::EndChild();

	ImGui::PopStyleColor(3);
}

bool Editor::EditorLauncher::InvisibleBtn(const char* _btn, float _x, float _y, bool _highlight)
{
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFontSize(2);

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
	wmgr->DestroySplash();

	mOriginStyle = wmgr->GetStyle();
	mOriginStyleEx = wmgr->GetStyleEx();
	mOriginSizeX = win.GetWidth();
	mOriginSizeY = win.GetHeight();

	win.Hide();
	win.SetStyle(LauncherStyle, LauncherStyleEx);
	win.SetSize(LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
	win.CenterWindow();
	win.SetSizeNoAdjust(LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
	win.CenterWindow();
	win.Show();

	//wmgr->mWidth = GetSystemMetrics(SM_CXSCREEN);
	//wmgr->mHeight = GetSystemMetrics(SM_CYSCREEN);
}

void Editor::EditorLauncher::RemoveWindowOptions(void)
{
	const auto& gfx = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>();
	const auto& wmgr = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>();
	auto& win = wmgr->GetMainWindow();
	win.SetStyle(WS_POPUP, WS_EX_APPWINDOW);
	gfx->DrawSplash();
	//win.Hide();
	//win.SetStyle(mOriginStyle, mOriginStyleEx);
	//win.SetSize(mOriginSizeX, mOriginSizeY);
	//win.CenterWindow();
	//win.SetSizeNoAdjust(LAUNCHER_WIDTH, LAUNCHER_HEIGHT);
	//win.CenterWindow();
	//win.Show();
	//wmgr->mWidth = GetSystemMetrics(SM_CXSCREEN);
	//wmgr->mHeight = GetSystemMetrics(SM_CYSCREEN);
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

bool Editor::EditorLauncher::ProjectDetails(const HashString& _path, float _w, float, bool _highlight)
{
	bool ret = false;
	static constexpr float fixedH = 70;
	float width = _w - (2*ImGui::GetStyle().WindowPadding.x);
	size_t pos = _path.rfind('/');
	if (pos == HashString::nPos)
		pos = _path.rfind("\\");
	HashString path{ _path.cbegin(), _path.cbegin() + pos };
	HashString name{ _path.cbegin() + pos + 1, _path.cend() };
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 5.f,0 });
	ImGui::BeginChild(_path.c_str(), ImVec2{ width, fixedH }, true);
	{
		auto startingPos = ImGui::GetCursorPos();
		HashString displayBtn{ "##" };
		displayBtn += _path.c_str();
		if (SelectableProjects(displayBtn.c_str(), width, fixedH, _highlight))
		{
			ret = true;
		}
		ImGui::SetItemAllowOverlap();
		ImGui::SetCursorPosX(startingPos.x + 7.f);
		ImGui::SetCursorPosY(startingPos.y + 7.f);
		EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFontSize(2);
		ImGui::Text(name.c_str());
		EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFontSize();
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7.f);
		ImGui::Text("Path: ");
		ImGui::SameLine();
		ImGui::Text(path.c_str());
		ImGui::PopStyleColor();
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
	return ret;
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
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFontSize(2);
	{
		ImGui::PushItemWidth(itemWidth);
		ImGui::InputText("###Project Name", mNameBuffer, bufSize, flags);
		ImGui::PopItemWidth(); 
	}
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFontSize();

	ImGui::Text("Location");
	auto linePos = ImGui::GetCursorPos();
	{
		ImGui::PushItemWidth(itemWidth - folderX - offsetX - 13.f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 10,10 });
		ImGui::InputText("###Location", mLocBuffer, bufSize, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();
	}
	ImGui::SameLine();
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 20.f);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.6f,0.6f,0.6f,1 });
		ImGui::BeginChild("Emptiness", ImVec2{ 10.f, 40.f});
		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + itemWidth - folderX - offsetX + 3.f);
	ImGui::SetCursorPosY(linePos.y + 6.f); 
	if (EGUI::Display::IconFolder("BrowseDir", folderX, 18.f, true))
	{
		HashString folder;
		if (Browse(folder))
		{
			strcpy_s(mLocBuffer, folder.c_str());
		}
	}
	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

	bool active = (strlen(mNameBuffer) && strlen(mLocBuffer));
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PushFontSize(2);

	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (itemWidth / 2) - (btnX / 2));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());

		if (!active)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_inactiveAlpha);

		if (ImGui::ButtonEx("Create", ImVec2{ btnX, btnY }, active ? 0 : ImGuiButtonFlags_Disabled))
		{
			const auto& fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
			bool firstF = true;
			unsigned failCount = 1;
			HashString tempName = mNameBuffer;
			size_t pos = tempName.size() - 1;
			while (!(fs->CreateFolder(tempName, mLocBuffer)))
			{
				if (firstF)
				{
					tempName += failCount;
					firstF = false;
				}
				else
				{
					tempName.erase(pos);
					tempName += failCount;
				}
				failCount++;
			}

			mProjFolderSelected = mLocBuffer;
			mProjFolderSelected += "/";
			mProjFolderSelected += mNameBuffer;
			size_t p = mProjFolderSelected.find('\\');
			while (p != HashString::nPos)
			{
				mProjFolderSelected[p] = '/';
				p = mProjFolderSelected.find('\\');
			}

			for (unsigned i = 0; i < SUBFOLDER_COUNT; ++i)
				fs->CreateFolder(g_SubFolders[i], mProjFolderSelected);

			mArrProjFolders.push_back(mProjFolderSelected);

			mProjFileSelected = mNameBuffer;
			mProjFileSelected += ".dyst";

			HashString forceCreate = mProjFolderSelected;
			forceCreate += '/';
			forceCreate += mProjFileSelected;

			FILE *pFile;
			fopen_s(&pFile, forceCreate.c_str(), "w");
			fclose(pFile);

			mbClosing = true;
		}
		if (ImGui::IsItemHovered() && (strlen(mNameBuffer) && strlen(mLocBuffer)))
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		ImGui::PopStyleVar((!active) ? 2 : 1);
	}
	EditorMain::GetInstance()->GetSystem<EditorUI>()->PopFontSize();
}

bool Editor::EditorLauncher::Browse(HashString& _outFolder)
{
	Dystopia::EditorProc p;
	return p.BrowseFolder(_outFolder);
}

bool Editor::EditorLauncher::SelectableProjects(const char* _btn, float _x, float _y, bool _highlight)
{
	ImVec2 btnSize{ _x, _y };
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(_btn);
	const ImVec2 label_size = ImGui::CalcTextSize(_btn, NULL, true);
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(btnSize, label_size.x, label_size.y);
	const ImRect bb(pos, ImVec2{ pos.x + size.x, pos.y + size.y });
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);
	if (pressed)
		ImGui::MarkItemValueChanged(id);
	ImVec4 defCol = ImGui::GetStyleColorVec4(ImGuiCol_Button);
	defCol.w = _highlight ? defCol.w : 0.f;
	ImGui::PushStyleColor(ImGuiCol_Button, defCol);
	const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	ImGui::RenderFrame(bb.Min, bb.Max, col);
	ImGui::PopStyleColor();
	if (hovered)
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	return pressed;
}

void Editor::EditorLauncher::OpenProject(float _w, float)
{
	static constexpr float btnH = 30;
	bool active = (mCurrentlySelected != -1);
	if (!active)
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_inactiveAlpha);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
	if (ImGui::ButtonEx("Open", ImVec2{ _w - (2 * ImGui::GetStyle().WindowPadding.x), btnH }, active ? 0 : ImGuiButtonFlags_Disabled))
	{
		mProjFolderSelected = mArrProjFolders[mCurrentlySelected];

		size_t pos = mProjFolderSelected.find_last_of("\\");
		if (pos == HashString::nPos)
			pos = mProjFolderSelected.find_last_of("/");
		mProjFileSelected = HashString{ mProjFolderSelected.cbegin() + 1 + pos, mProjFolderSelected.cend() };
		mProjFileSelected += ".dyst";

		mbClosing = true;
	}
	if (ImGui::IsItemHovered() && active)
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	ImGui::PopStyleVar((!active) ? 2 : 1);
}

void Editor::EditorLauncher::BrowseProject(float _w, float _h)
{
	static constexpr float btnH = 30;
	float offset = (2*btnH) + (4 * ImGui::GetStyle().WindowPadding.y);

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + _h - offset);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
	if (ImGui::ButtonEx("Browse...", ImVec2{ _w - (2 * ImGui::GetStyle().WindowPadding.x), btnH }))
	{
		Dystopia::EditorProc p;
		HashString projectLoaded;
		if (p.Load(projectLoaded, eFileTypes::eSETTINGS))
		{
			bool exist = false;
			size_t pos = projectLoaded.find_last_of("\\");
			if (pos == HashString::nPos)
				pos = projectLoaded.find_last_of("/");

			HashString folder{ projectLoaded.cbegin(), projectLoaded.cbegin() + pos };
			HashString file{ projectLoaded.cbegin() + pos + 1, projectLoaded.cend() };

			for (auto& elem : mArrProjFolders)
			{
				if (elem == folder)
				{
					exist = true;
					break;
				}
			}
			if (!exist)
				mArrProjFolders.push_back(folder);

			mProjFolderSelected = folder;
			mProjFileSelected = file;
			mbClosing = true;
		}
	}
	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	ImGui::PopStyleVar();
}
#endif






















