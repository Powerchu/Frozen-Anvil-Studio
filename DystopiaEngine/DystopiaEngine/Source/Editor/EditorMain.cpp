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

#include "Editor/EditorLauncher.h"

#include "DataStructure/HashString.h"

#include "Allocator/DefaultAlloc.h"

#include "System/Driver/Driver.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Window/WindowManager.h"
#include "System/Profiler/Profiler.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/File/FileSystem.h"

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
	mCurState{}, mNextState{}, mDelta{}, mProjectPath{}, mTimer{}
{
}

Editor::EditorMain::~EditorMain(void)
{
}

void Editor::EditorMain::Init(void)
{
	Dystopia::EngineCore::GetInstance()->LoadSettings();
	Dystopia::EngineCore::GetInstance()->PreInit();

	for (auto& s : mArrSystems)
		s->Load();
	for (auto& p : mArrPanels)
		p->Load();

	Dystopia::EngineCore::GetInstance()->Init();
	Dystopia::EngineCore::GetInstance()->PostInit();

	for (auto& s : mArrSystems)
		if (!s->Init())
			__debugbreak();
		
	for (auto& p : mArrPanels)
		if (!p->Init())
			__debugbreak();
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

	mProjectPath = launcher.GetProjectPath();
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

	for (auto& p : mArrPanels)
		p->Update(mDelta);

	for (auto& p : mArrPanels)
		p->EditorUI();
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

	Dystopia::EngineCore::GetInstance()->PostUpdate();
}

void Editor::EditorMain::Shutdown(void)
{
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

HashString Editor::EditorMain::GetCurProjectFullPath(void) const
{
	return mProjectPath;
}

#endif





