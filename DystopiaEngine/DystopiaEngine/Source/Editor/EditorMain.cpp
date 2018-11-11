/* HEADER *********************************************************************************/
/*!
\file	EditorMain.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorMain.h"
#include "Editor/EInput.h"
#include "Editor/EditorStates.h"
#include "Editor/EditorUI.h"
#include "Editor/EditorClipboard.h"
#include "Editor/EditorResource.h"
#include "Editor/EditorCommands.h"

#include "Editor/EGUI.h"

#include "Editor/EditorLauncher.h"

#include "Editor/Inspector.h"

#include "DataStructure/HashString.h"

#include "Allocator/DefaultAlloc.h"

#include "System/Driver/Driver.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Window/WindowManager.h"
#include "System/Profiler/Profiler.h"
#include "System/File/FileSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/Time/ScopedTimer.h"
#include "System/Profiler/Profiler.h"
#include "System/Profiler/ProfilerAction.h"

#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"

static const char * g_ProjectLauncherSave = "KnownProjects";
namespace
{
	template <typename Ty, typename ... T>
	AutoArray<Ty> MakeAutoArray(Ut::TypeList<T...>)
	{
		return AutoArray<Ty>{ static_cast<Ty>(Dystopia::DefaultAllocator<T>::ConstructAlloc())...};
	}
}

Editor::EditorMain* Editor::EditorMain::GetInstance(void)
{
	static EditorMain instance{};
	return &instance;
}

Editor::EditorMain::EditorMain(void)
	: mArrSystems{ MakeAutoArray<EditorSystem*>(Ut::MakeTypeList_t<Ut::TypeList, ESystems>{}) },
	  mArrPanels { MakeAutoArray<EditorPanel*>(Ut::MakeTypeList_t<Ut::TypeList, EPanels>{}) },
	mCurState{}, mNextState{}, mDelta{}, mProjFolder{}, mTimer{}
{
}

Editor::EditorMain::~EditorMain(void)
{
}

void Editor::EditorMain::Init(void)
{
	Dystopia::EngineCore::GetInstance()->PreInit();

	for (auto& s : mArrSystems)
		s->Load();

	for (auto& p : mArrPanels)
		p->Load();

	ProjectLauncher();
	UpdatePaths();

	Dystopia::EngineCore::GetInstance()->LoadSettings();
	Dystopia::EngineCore::GetInstance()->Init();
	Dystopia::EngineCore::GetInstance()->PostInit();

	for (auto& s : mArrSystems)
		if (!s->Init())
			__debugbreak();
		
	for (auto& p : mArrPanels)
		if (!p->Init())
			__debugbreak();

	LoadProjSettings();
	ChangeState(eState::MAIN);
}

void Editor::EditorMain::StartFrame(void)
{
	mDelta = mTimer.Elapsed();
	mTimer.Lap();

	for (auto& s : mArrSystems)
		s->StartFrame();
}

void Editor::EditorMain::Update(void)
{
	for (auto& s : mArrSystems)
		s->Update(mDelta);

	if (mCurState == eState::PLAY)
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);

	for (unsigned int i = 0; i < mArrPanels.size(); ++i)
	{
		EGUI::PushID(i);
		EditorPanel *p = mArrPanels[i];
		p->SetSize(EGUI::Docking::GetTabSize(p->GetLabel().c_str()) - Math::Vec2{ 8, 4 });
		p->SetPosition(EGUI::Docking::GetTabPosition(p->GetLabel().c_str()));
		//p->SetSceneContext(&(mpSceneSystem->GetCurrentScene()));
		{
			Dystopia::ScopedTimer<Dystopia::ProfilerAction> scopeT{ p->GetLabel().c_str(), "Update" };
			p->Update(mDelta);
		}
		{
			Dystopia::ScopedTimer<Dystopia::ProfilerAction> scopeT{ p->GetLabel().c_str(), "Editor UI" };
			//ImGuiWindowFlags f = p->GetVertClip() ? ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar : ImGuiWindowFlags_None;
			if (EGUI::StartTab(p->GetLabel().c_str(), &p->GetOpenedBool()/*, f*/))
			{
				EGUI::Indent(4);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
				p->EditorUI();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
				EGUI::UnIndent(4);
			}
		}
		EGUI::EndTab();
		EGUI::PopID();
	}

	if (mCurState == eState::PLAY)
		ImGui::PopStyleVar();
}

void Editor::EditorMain::EndFrame(void)
{
	for (auto& s : mArrSystems)
		s->EndFrame();

	if (mCurState != mNextState)
	{
		mCurState = mNextState;
		for (auto& s : mArrSystems)
			s->Message(eEMessage::STATE_CHANGED);
		for (auto& p : mArrPanels)
			p->Message(eEMessage::STATE_CHANGED);
	}

	auto sceneSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
	bool sceneChanged = (&sceneSys->GetCurrentScene() != &sceneSys->GetNextScene());

	Dystopia::EngineCore::GetInstance()->PostUpdate();

	if (sceneChanged)
	{
		for (auto& s : mArrSystems)
			s->Message(eEMessage::SCENE_CHANGED);
		for (auto& p : mArrPanels)
			p->Message(eEMessage::SCENE_CHANGED);
	}
}

void Editor::EditorMain::Shutdown(void)
{
	HashString fp = mProjFolder;
	fp += "\\";
	fp += mProjFile;
	auto projSerial = Dystopia::TextSerialiser::OpenFile(fp.c_str(), Dystopia::TextSerialiser::MODE_WRITE);

	for (auto& s : mArrSystems)
		s->SaveSettings(projSerial);

	for (auto& p : mArrPanels)
		p->SaveSettings(projSerial);

	for (auto& s : mArrSystems)
		s->Shutdown();

	for (auto& p : mArrPanels)
		p->Shutdown();

	for (auto& s : mArrSystems)
		Dystopia::DefaultAllocator<EditorSystem>::DestructFree(s);

	for (auto& p : mArrPanels)
		Dystopia::DefaultAllocator<EditorPanel>::DestructFree(p);

	mArrSystems.clear();
	mArrPanels.clear();

	Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::LoggerSystem>()->RedirectOutput(nullptr);
	Dystopia::EngineCore::GetInstance()->Shutdown();
}

void Editor::EditorMain::ChangeState(eState e)
{
	mNextState = e;
}

void Editor::EditorMain::Broadcast(eEMessage _msg) const
{
	for (auto& s : mArrSystems)
		s->Message(_msg);
	for (auto& p : mArrPanels)
		p->Message(_msg);
}

float Editor::EditorMain::GetDeltaTime(void) const
{
	return mDelta;
}

Editor::eState Editor::EditorMain::GetCurState(void) const
{
	return mCurState;
}

bool Editor::EditorMain::IsClosing(void)
{
	return mCurState == eState::EXIT;
}

HashString Editor::EditorMain::GetCurProjFolder(void) const
{
	return mProjFolder;
}

HashString Editor::EditorMain::GetCurProjFile(void) const
{
	return mProjFile;
}

void Editor::EditorMain::ProjectLauncher(void)
{
	auto fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();

	HashString fileName = g_ProjectLauncherSave;
	fileName += ".";
	fileName += Gbl::SETTINGS_EXT;

	HashString fullPath = (fs->GetFullPath("Dystopia", Dystopia::eFileDir::eAppData)).c_str();
	fullPath += "\\";
	fullPath += fileName;

	FILE *pFile;
	fopen_s(&pFile, fullPath.c_str(), "a");
	fclose(pFile);

	EditorLauncher launcher;
	launcher.Init();
	auto serialR = Dystopia::TextSerialiser::OpenFile(fullPath.c_str(), Dystopia::TextSerialiser::MODE_READ);
	launcher.LoadSettings(serialR);
	while (!launcher.IsClosing())
	{
		launcher.StartFrame();
		launcher.Update(0.016f);
		launcher.EndFrame();
	}
	auto serialW = Dystopia::TextSerialiser::OpenFile(fullPath.c_str(), Dystopia::TextSerialiser::MODE_WRITE);
	launcher.SaveSettings(serialW);
	launcher.Shutdown();

	mProjFolder = launcher.GetProjFolder();
	mProjFile = launcher.GetProjFile();
}

void Editor::EditorMain::UpdatePaths(void)
{
	auto fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
	fs->ChangeDirPath(Dystopia::eFileDir::eResource, mProjFolder);
}

void Editor::EditorMain::LoadProjSettings(void)
{
	HashString fp = mProjFolder;
	fp += "\\";
	fp += mProjFile;
	auto projSerial = Dystopia::TextSerialiser::OpenFile(fp.c_str(), Dystopia::TextSerialiser::MODE_READ);
	for (auto& s : mArrSystems)
		s->LoadSettings(projSerial);
	for (auto& p : mArrPanels)
		p->LoadSettings(projSerial);
}

AutoArray<Editor::EditorSystem*>& Editor::EditorMain::GetAllSystems(void)
{
	return mArrSystems;
}

AutoArray<Editor::EditorPanel*>& Editor::EditorMain::GetAllPanels(void)
{
	return mArrPanels;
}

#endif





