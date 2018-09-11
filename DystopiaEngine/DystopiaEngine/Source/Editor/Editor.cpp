/* HEADER *********************************************************************************/
/*!
\file	Editor.cpp
\author Digipen (100%)
\par    email: digipen\@digipen.edu
\brief
	INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR

#include <memory>
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

/* System includes */
#include "System\Window\WindowManager.h"
#include "System\Window\Window.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Scene\SceneSystem.h"
#include "System\Time\Timer.h"
#include "System\Driver\Driver.h"
#include "System/Hotload/HotloadSystem.h"
#include "IO\BinarySerializer.h"

/* Editor includes */
#include "Editor\Editor.h"
#include "Editor\EGUI.h"
#include "Editor\Inspector.h"
#include "Editor\HierarchyView.h"
#include "Editor\ProjectResource.h"
#include "Editor\SceneView.h"
#include "Editor\ConsoleLog.h"
#include "Editor\EditorInputs.h"
#include "Editor\EditorEvents.h"
#include "Editor\Commands.h"


#include "Behaviour/TestClassBase.h"

/* library includes */
#include <iostream>
#include <bitset>

// Entry point for editor
int WinMain(HINSTANCE hInstance, HINSTANCE, char *, int)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	hInstance;

	Dystopia::Editor *editor	= Dystopia::Editor::GetInstance();
	Dystopia::Timer *timer		= new Dystopia::Timer{};


	editor->Init();

	Dystopia::Hotloader<1> test;
	test.SetFileDirectoryPath<0>("C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Resource/Behaviours/");

	
	test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Behaviour");
	test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Component");
	//test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/bin/Temp/Debug");
	test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Editor");
	test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/IO");
	test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Math");
	test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Object");
	test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Utility");
	//test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/System");
	test.AddAdditionalSourcePath(L"C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/Dependancies/glew-2.1.0/lib/Release/x64");

	//test.SetFileDirectoryPath<1>("C:/Users/Owner/source/repos/Frozen-Anvil-Studio/DystopiaEngine/DystopiaEngine/Source/Behaviour/");


	test.SetDllFolderPath       ("C:/Users/Owner/AppData/Roaming/Dystopia/DLL/");
	test.Init();
	TestClassBase * p                        = nullptr;
	Dystopia::DLLWrapper * pListOfDLLChanges[100];

	auto const & ArrayDll = test.GetDlls();
	for (auto const & elem : ArrayDll)
	{
		auto func = elem.GetDllFunc<TestClassBase *>("Clone");
		if (func)
		{
			p = func();
			p->Test();
			delete p;
			p = nullptr;
		}

	}
	
	while (true)
	{
		test.Update();

		if (test.ChangesInDllFolder(100, pListOfDLLChanges))
		{
			Dystopia::DLLWrapper ** start = pListOfDLLChanges;
			while (*start != nullptr)
			{
				auto func = (*start)->GetDllFunc<TestClassBase *>("Clone");
				if (func)
				{
					p = func();
					p->Test();
					delete p;
					p = nullptr;
				}
				start++;
			}
		}
	}
	/*
	LPCWSTR ct = L"C:/Users/Keith/AppData/Roaming/Dystopia/DLL/TestClass.dll";;

	auto dll = LoadLibrary(ct);
	if (dll)
	{
		FARPROC proc = GetProcAddress(dll, "Clone");
		using fp = TestClassBase * (*)();
		fp fpp = (fp)proc;
		p = fpp();
		p->Test();
		FreeLibrary(dll);
	}

	
	Dystopia::DLLWrapper * pm = nullptr;
	while (true)
	{
		test.Update(0.f);
		if (test.isDllModified(&pm, 1))
		{
			if (pm != nullptr)
			{
				auto pp = pm->GetDllFunc<TestClassBase *>("Clone");
				p = pp();
				p->Test();
			}

		}
	}
	*/
	while (!editor->IsClosing())
	{
		float dt = timer->Elapsed();
		timer->Lap();
	
		editor->StartFrame(dt);
	
		editor->UpdateFrame(dt);
		
		editor->EndFrame();
	}
	editor->Shutdown();
	delete timer;
	delete editor;

	// Automatically called by _CRTDBG_LEAK_CHECK_DF flag when proccess ends. 
	// This will ensure static variables are not taken into account
	//_CrtDumpMemoryLeaks(); 
	return 0;
}


/*///////////////////////////////////////////////////////////////////// EDITOR CLASS ////////////////////////////////////////////////////////////////////////////////*/
namespace Dystopia
{
	static Editor* gpInstance = 0;
	Editor* Editor::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new Editor{};
		return gpInstance;
	}

	Editor::Editor(void)
		: mCurrentState{ EDITOR_MAIN }, mNextState{ mCurrentState }, 
		mpWin{ nullptr }, 
		mpGfx{ nullptr },
		mpSceneSystem{ nullptr },
		mpInput{ new EditorInput{} },
		mpEditorEventSys{ new EditorEventHandler{} },
		mpComdHandler{ new CommandHandler{} },
		mpGuiSystem{ new GuiSystem{} }
	{}

	Editor::~Editor(void)
	{
	}

	void Editor::Init()
	{
		mpDriver		= Dystopia::EngineCore::GetInstance();
		mpDriver->LoadSettings();
		mpDriver->Init();

		mpWin			= mpDriver->GetSystem<WindowManager>();		// driver init-ed
		mpGfx			= mpDriver->GetSystem<GraphicsSystem>();	// driver init-ed
		mpSceneSystem	= mpDriver->GetSystem<SceneSystem>();		// driver init-ed

		LoadDefaults();
		mpInput->Init();
		mpEditorEventSys->Init();
		EGUI::SetContext(mpComdHandler);

		for (auto& e : mTabsArray)
		{
			e->SetComdContext(mpComdHandler);
			e->SetEventSysContext(mpEditorEventSys);
			e->SetSceneContext(&(mpSceneSystem->GetCurrentScene()));
			e->Init();
			e->RemoveFocus();
		}

		InstallHotkeys();

		if (!mpGuiSystem->Init(mpWin, mpGfx, mpInput))
			mCurrentState = EDITOR_EXIT;
	}

	void Editor::LoadDefaults()
	{
		mTabsArray.push_back(Inspector::GetInstance());
		mTabsArray.push_back(ProjectResource::GetInstance());
		mTabsArray.push_back(HierarchyView::GetInstance());
		mTabsArray.push_back(SceneView::GetInstance());
		mTabsArray.push_back(ConsoleLog::GetInstance());
	}

	void Editor::StartFrame(const float& _dt)
	{
		mpWin->Update(_dt);
		mpInput->Update(_dt);
		mpGuiSystem->StartFrame(_dt);

		UpdateKeys();
		UpdateHotkeys();
		
		mpEditorEventSys->FireAllPending();
		MainMenuBar();
	}

	void Editor::UpdateFrame(const float& _dt)
	{
		//mpGfx->Update(_dt); // causes double frame buffer swap per frame, need determine either editor gfx or driver gfx do
		for (unsigned int i = 0; i < mTabsArray.size(); ++i)
		{
			EGUI::PushID(i);
			EditorTab *pTab = mTabsArray[i];
			pTab->SetSceneContext(&(mpSceneSystem->GetCurrentScene()));
			pTab->Update(_dt);

			switch (i)
			{
			case 0: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), EGUI::Docking::eDOCK_SLOT_RIGHT);
				break;
			case 1: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), EGUI::Docking::eDOCK_SLOT_LEFT);
				break;
			case 2: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), EGUI::Docking::eDOCK_SLOT_TOP);
				break;
			case 3: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), EGUI::Docking::eDOCK_SLOT_RIGHT);
				break;
			default: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), EGUI::Docking::eDOCK_SLOT_NONE);
			}

			if (EGUI::StartTab(pTab->GetLabel().c_str(), pTab->GetOpenedBool()))
			{
				pTab->SetSize(EGUI::Docking::GetTabSize(pTab->GetLabel().c_str()));
				pTab->SetPosition(EGUI::Docking::GetTabPosition(pTab->GetLabel().c_str()));
				pTab->Window();
			}
			EGUI::EndTab();
			EGUI::PopID();
		}

		// if (mCurrentState == EDITOR_PLAY) call for update of current scene
		if (mCurrentState == EDITOR_PAUSE) return;
	}

	void Editor::EndFrame()
	{
		mpGuiSystem->EndFrame();

		if (mCurrentState != mNextState) 
			UpdateState();
	}

	void Editor::Shutdown()
	{
		UnInstallHotkeys();

		EGUI::Docking::ShutdownTabs();
		for (auto& e : mTabsArray)
		{
			e->Shutdown();
			delete e;
			e = nullptr;
		}

		mpInput->Shutdown();
		delete mpInput;
		mpInput = nullptr;

		mpEditorEventSys->Shutdown();
		delete mpEditorEventSys;
		mpEditorEventSys = nullptr;

		mpComdHandler->Shutdown();
		delete mpComdHandler;
		mpComdHandler = nullptr;

		mpGuiSystem->Shutdown();
		delete mpGuiSystem;
		mpGuiSystem = nullptr;

		mpDriver->Shutdown();

		mpWin = nullptr;
		mpGfx = nullptr;

		EGUI::RemoveContext();
	}

	eEditorState Editor::CurrentState() const
	{
		return mCurrentState;
	}

	void Editor::ChangeState(eEditorState _state)
	{
		mNextState = _state;
	}

	bool Editor::IsClosing() const
	{
		return !mCurrentState;
	}

	void Editor::UpdateState()
	{
		switch (mNextState)
		{
		case EDITOR_MAIN:
			if (mCurrentState == EDITOR_PLAY || mCurrentState == EDITOR_PAUSE)
				TempLoad(); 
			break;
		case EDITOR_PLAY:
			TempSave();
			Play();
			break;
		case EDITOR_PAUSE:
			break;
		case EDITOR_SAVE:
			Save();
			break;
		case EDITOR_LOAD:
			Load();
			break;
		}
		mCurrentState = mNextState;
	}

	void Editor::MainMenuBar()
	{
		if (EGUI::StartMainMenuBar())
		{
			MMFile();
			MMEdit();
			MMView();

			EGUI::EndMainMenuBar();
		}
	}

	void Editor::MMFile()
	{
		if (EGUI::StartMenuHeader("File"))
		{
			if (EGUI::StartMenuBody("New"))
			{
				// TODO: Some actual function
			}
			if (EGUI::StartMenuBody("Open"))
			{
				// TODO: Some actual function
			}
			if (EGUI::StartMenuHeader("Open Recent"))
			{
				if (EGUI::StartMenuBody("some_recent_crap.cpp"))
				{
					// TODO: Some actual function
				}
				if (EGUI::StartMenuBody("some_recent_crap.h"))
				{
					// TODO: Some actual function
				}
				if (EGUI::StartMenuHeader("More.."))
				{
					EGUI::StartMenuBody("surprise_theres_more_crap.h");
					EGUI::EndMenuHeader();
				}
				EGUI::EndMenuHeader();
			}
			if (EGUI::StartMenuBody("Save"))
			{
				// TODO: Some actual function
			}
			if (EGUI::StartMenuBody("Save As.."))
			{
				// TODO: Some actual function
			}
			if (EGUI::StartMenuBody("Quit"))
			{
				// TODO: Some actual function
				Save();
				ChangeState(Dystopia::EDITOR_EXIT);
			}
			EGUI::EndMenuHeader();
		}
	}

	void Editor::MMEdit()
	{
		// TODO: Some actual function for all the bottom
		if (EGUI::StartMenuHeader("Edit"))
		{
			if (EGUI::StartMenuBody("Undo ", "Ctrl + Z"))	EditorUndo();
			if (EGUI::StartMenuBody("Redo ", "Ctrl + Y"))	EditorRedo();
			if (EGUI::StartMenuBody("Cut ", "Ctrl + X"))	EditorCut();
			if (EGUI::StartMenuBody("Copy ", "Ctrl + C"))	EditorCopy();
			if (EGUI::StartMenuBody("Paste ", "Ctrl + V"))	EditorPaste();

			EGUI::EndMenuHeader();
		}
	}

	void Editor::MMView()
	{
		static constexpr float icon = 10.f;
		if (EGUI::StartMenuHeader("View"))
		{
			for (auto& e : mTabsArray)
			{
				if (*(e->GetOpenedBool())) 
					EGUI::Display::IconTick(icon, icon);
				else 
					EGUI::Display::Dummy(icon, icon);
				EGUI::SameLine();
				if (EGUI::StartMenuBody(e->GetLabel()))
					*(e->GetOpenedBool()) = !*(e->GetOpenedBool());
			}
			EGUI::EndMenuHeader();
		}
	}

	void Editor::EditorUndo()
	{
		mpComdHandler->UndoCommand();
	}

	void Editor::EditorRedo()
	{
		mpComdHandler->RedoCommand();
	}

	void Editor::EditorCopy()
	{
	}

	void Editor::EditorCut()
	{
	}

	void Editor::EditorPaste()
	{
	}

	void Editor::Play()
	{
		// call for Init of the current scene. Assuming scene manager knows which is the current scene 
		// something like , mpSceneMgr->CurrentScene()->Init() using the temp file
	}

	void Editor::Save()
	{
		// call for serialization of all in current scene
		//BinarySerializer serial = BinarySerializer::OpenFile("SaveSettingsFile", std::ios::out);

	}

	void Editor::Load()
	{
		// call for deserilization of all in current scene
	}

	void Editor::TempSave()
	{
		// save all current values to a temp file or something to be reloaded after editor_play ends
	}

	void Editor::TempLoad()
	{
		// reset all current values to temp file values
	}

	void Editor::UpdateKeys()
	{
		const auto& queue = mpWin->GetMainWindow().GetInputQueue();

		mpGuiSystem->UpdateKey(eButton::KEYBOARD_ENTER, false);
		mpGuiSystem->UpdateKey(eButton::KEYBOARD_ESCAPE, false);

		for (int i = eButton::KEYBOARD_BACKSPACE; i <= eButton::KEYBOARD_TAB; ++i)
			mpGuiSystem->UpdateKey(i, false);
		for (int i = eButton::KEYBOARD_SPACEBAR; i <= eButton::KEYBOARD_HOME; ++i)
			mpGuiSystem->UpdateKey(i, false);
		for (int i = eButton::KEYBOARD_LEFT; i <= eButton::KEYBOARD_DOWN; ++i)
			mpGuiSystem->UpdateKey(i, false);
		for (int i = eButton::KEYBOARD_INSERT; i <= eButton::KEYBOARD_DELETE; ++i)
			mpGuiSystem->UpdateKey(i, false);

		bool caps = mpInput->IsKeyPressed(KEY_SHIFT);

		for (const auto& k : queue)
		{
			// 0 to 9
			if (k >= eButton::KEYBOARD_0 && k <= eButton::KEYBOARD_9)
				mpGuiSystem->UpdateChar(k);
			// A to Z
			else if (k >= eButton::KEYBOARD_A && k <= eButton::KEYBOARD_Z)
				mpGuiSystem->UpdateChar(caps ? k : k + 32);
			// numpad 0 to 9
			else if (k >= eButton::KEYBOARD_NUMPAD_0 && k <= eButton::KEYBOARD_NUMPAD_9)
				mpGuiSystem->UpdateChar(k - 49);
			// misc keys like ctrl, del, back etc
			else
				mpGuiSystem->UpdateKey(k, true);
		}
	}

	void Editor::UpdateHotkeys()
	{
		if (mpInput->IsKeyTriggered(KEY_LMOUSE))
		{
			mpGuiSystem->UpdateMouse(KEY_LMOUSE, true);
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_LCLICK);
		}

		if (mpInput->IsKeyTriggered(KEY_RMOUSE))
		{
			mpGuiSystem->UpdateMouse(KEY_RMOUSE, true);
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_RCLICK);
		}

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_Z))
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_UNDO);

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_Y))
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_REDO);

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_C))
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_COPY);

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_X))
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_CUT);

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_V))
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_PASTE);
	}
	
	void Editor::InstallHotkeys()
	{
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_UNDO)->Bind(&Editor::EditorUndo, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_REDO)->Bind(&Editor::EditorRedo, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_COPY)->Bind(&Editor::EditorCopy, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_CUT)->Bind(&Editor::EditorCut, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_PASTE)->Bind(&Editor::EditorPaste, this);
	}

	void Editor::UnInstallHotkeys()
	{
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_UNDO)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_REDO)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_COPY)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_CUT)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_PASTE)->Unbind(this);
	}

	void Editor::SetFocus(GameObject& _rObj)
	{
		for (auto& e : mTabsArray)
			e->SetFocus(_rObj);
	}
	
	void Editor::RemoveFocus()
	{
		for (auto& e : mTabsArray)
			e->RemoveFocus();
	}
}

#endif		// EDITOR ONLY
