/* HEADER *********************************************************************************/
/*!
\file	EditorMain.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
#include "Editor/EditorFactory.h"
#include "Editor/Payloads.h"

#include "Editor/EGUI.h"

#include "Editor/EditorLauncher.h"

#include "Editor/Inspector.h"
#include "Editor/HierarchyView.h"
#include "Editor/ProjectSettings.h"
#include "Editor/ProjectResource.h"
#include "Editor/SceneView.h"
#include "Editor/ConsoleLog.h"
#include "Editor/PerformanceLog.h"
#include "Editor/PLogger.h"
#include "Editor/ColorScheme.h"
#include "Editor/StyleScheme.h"
#include "Editor/SpritePreviewer.h"
#include "Editor/SpriteEditor.h"
#include "Editor/BehaviourTreeEditor.h"
#include "Editor/GameView.h"
#include "Editor/FlowChart.h"
#include "Editor/DataSheetEditor.h"
#include "Editor/ParticleEditor.h"
#include "Editor/ShaderView.h"

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

#include "System/Database/DataSheet.h"

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
	mCurState{}, mNextState{}, mfDelta{}, mProjFolder{}, mTimer{}, mfOverlayAlpha{ 1.f }
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

	ProjectLauncher();
	UpdatePaths();

	for (auto& p : mArrPanels)
		p->Load();

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
	mfDelta = mTimer.Elapsed();
	mTimer.Lap();

	for (auto& s : mArrSystems)
		s->StartFrame();
}

void Editor::EditorMain::Update(void)
{
	for (unsigned int i = 0; i < mArrSystems.size(); ++i)
	{
		EGUI::PushID(i);
		mArrSystems[i]->Update(mfDelta);
		EGUI::PopID();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, mfOverlayAlpha);
	for (unsigned int i = 0; i < mArrPanels.size(); ++i)
	{
		EGUI::PushID(i);
		EditorPanel *p = mArrPanels[i];
		p->SetSize(EGUI::Docking::GetTabSize(p->GetLabel().c_str()) - Math::Vec2{ 8, 12 });
		p->SetPosition(EGUI::Docking::GetTabPosition(p->GetLabel().c_str()));
		{
			p->Update(mfDelta);
		}
		{
			//Dystopia::ScopedTimer<Dystopia::ProfilerAction> scopeT{ p->GetLabel().c_str(), "Editor UI" };
			ImGuiWindowFlags f = p->IsScrollEnabled() ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
			if (p->IsHorizontalEnabled())
				f |= ImGuiWindowFlags_HorizontalScrollbar;

			bool open = p->IsOpened();
			if (EGUI::StartTab(p->GetLabel().c_str(), &open, f))
			{
				EGUI::Indent(4);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
				p->EditorUI();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
				EGUI::UnIndent(4);
			}
			p->SetOpened(open);
			EGUI::EndTab();
		}
		EGUI::PopID();
	}
	ImGui::PopStyleVar();
}

void Editor::EditorMain::EndFrame(void)
{
	LogPerformance();

	if (mCurState != mNextState)
	{
		mfOverlayAlpha = (mNextState == eState::PLAY) ? 0.4f : 1.f;
		mCurState = mNextState;
		for (auto& s : mArrSystems)
			s->Message(eEMessage::STATE_CHANGED);
		for (auto& p : mArrPanels)
			p->Message(eEMessage::STATE_CHANGED);
	}

	const auto sceneSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
	const bool sceneChanged = (&sceneSys->GetCurrentScene() != &sceneSys->GetNextScene());

	if (sceneChanged)
	{
		for (auto& s : mArrSystems)
			s->Message(eEMessage::SCENE_ABOUT_TO_CHANGE);
		for (auto& p : mArrPanels)
			p->Message(eEMessage::SCENE_ABOUT_TO_CHANGE);
	}

	Dystopia::EngineCore::GetInstance()->PostUpdate();

	if (sceneChanged)
	{
		for (auto& s : mArrSystems)
			s->Message(eEMessage::SCENE_CHANGED);
		for (auto& p : mArrPanels)
			p->Message(eEMessage::SCENE_CHANGED);
	}

	for (auto& s : mArrSystems)
		s->EndFrame();

	if (mMsgPending.size())
	{
		for (auto& m : mMsgPending)
		{
			for (auto& s : mArrSystems)
				s->Message(m);
			for (auto& p : mArrPanels)
				p->Message(m);
		}
		mMsgPending.clear();
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

void Editor::EditorMain::Broadcast(eEMessage _msg)
{
	mMsgPending.push_back(_msg);
}

float Editor::EditorMain::GetDeltaTime(void) const
{
	return mfDelta;
}

Editor::eState Editor::EditorMain::GetCurState(void) const
{
	return mCurState;
}

Editor::eState Editor::EditorMain::GetNextState(void) const
{
	return mNextState;
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
	auto fs = Dystopia::EngineCore::GetInstance()->Get<Dystopia::FileSystem>();
	fs->CreateFiles("Dystopia", Dystopia::eFileDir::eAppData);

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
	auto fs = Dystopia::EngineCore::GetInstance()->Get<Dystopia::FileSystem>();

	HashString subFolder = mProjFile;
	size_t pos = subFolder.find_last_of(".");
	subFolder.erase(pos);
	subFolder += "DLL";
	HashString dllFolder{ "Dystopia" };
	dllFolder += "\\";
	dllFolder += subFolder;

	fs->CreateFiles(dllFolder.c_str(), Dystopia::eFileDir::eAppData);
	HashString fullPath = (fs->GetFullPath(subFolder.c_str(), Dystopia::eFileDir::eAppData)).c_str();

	fs->ChangeDirPath(Dystopia::eFileDir::eCurrent, mProjFolder);

	HashString resFolder{ mProjFolder };
	resFolder += "/";
	resFolder += "Resource";
	fs->ChangeDirPath(Dystopia::eFileDir::eResource, resFolder);
	fs->ChangeDirPath(Dystopia::eFileDir::eAppData, fullPath);
}

void Editor::EditorMain::LoadProjSettings(void)
{
	HashString fp = mProjFolder;
	fp += "/";
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

void Editor::EditorMain::ExternalFile(const wchar_t* const _p)
{
	HashString a; 
	a = _p;
	ExternalFile(a);
}

void Editor::EditorMain::ExternalFile(const HashString& _p)
{
	GetSystem<EditorResource>()->ExternalFileHandling(_p.c_str());
}

void Editor::EditorMain::ExternalFile(const AutoArray<HashString>& _arrExternals)
{
	for (const auto & elem : _arrExternals)
		ExternalFile(elem);
}

void Editor::EditorMain::LogPerformance(void)
{
	static constexpr float intervalS = 0.1f;
	static float deltaAccu = 0.f;
	deltaAccu += mfDelta;
	if (deltaAccu > intervalS)
	{
		auto profiler = Dystopia::EngineCore::Get<Dystopia::Profiler>();
		deltaAccu = 0;
		auto& data = profiler->GetInfo();
		for (auto& d : data)
		{
			auto& info = d.second.mTimes;
			for (const auto& i : info)
			{
				Performance::LogDataS(d.first.c_str(), i.first.c_str(), static_cast<float>(info[i.first]));
				Performance::LogDataG(d.first.c_str(), static_cast<float>(d.second.mTotal));
			}
		}
		PLogTaskManager p;
		p.mFrameRate = mfDelta;
		p.mCPUIdle = profiler->GetCPUPercentageIdle();
		p.mCPUBusy = profiler->GetCPUPercentageBusy();
		p.mCPUOS = profiler->GetCPUPercentageOS();
		p.mCPUProc = profiler->GetCPUPercentageProcess();
		p.mPageFaults = profiler->GetNumPageFaults();
		p.mMemUsed = profiler->GetUsedMemory();
		p.mRamUsed = profiler->GetUsedPhysicalMemory();
		p.mMemAvail = profiler->GetAvailablePhysicalMemory();
		p.mMemLoad = profiler->GetSystemMemoryLoad();
		Performance::LogTaskMgr(p);
	}
}

#endif





