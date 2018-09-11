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
#include "System\Driver\Driver.h"
#include "System\Profiler\Profiler.h"
#include "IO\BinarySerializer.h"
#include "Utility\GUID.h"

/* Editor includes */
#include "Editor\EGUI.h"
#include "Editor\Editor.h"
#include "Editor\EditorInputs.h"
#include "Editor\EditorEvents.h"
#include "Editor\Commands.h"
#include "Editor\Inspector.h"
#include "Editor\HierarchyView.h"
#include "Editor\ProjectResource.h"
#include "Editor\SceneView.h"
#include "Editor\ConsoleLog.h"
#include "Editor\PerformanceLog.h"

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

	Dystopia::Editor *editor = Dystopia::Editor::GetInstance();
	editor->Init();
	while (!editor->IsClosing())
	{
		editor->StartFrame();
	
		editor->UpdateFrame(editor->GetDeltaTime());
		
		editor->EndFrame();
	}
	editor->Shutdown();
	delete editor;
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
		mpGuiSystem{ new GuiSystem{} },
		mpTimer{ new Timer{} }
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
			e->SetEventContext(mpEditorEventSys);
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
		mTabsArray.push_back(PerformanceLog::GetInstance());
	}

	void Editor::StartFrame()
	{
		/* Set delta time of frame */
		mDeltaTime = mpTimer->Elapsed();
		mpTimer->Lap();

		Profiler& profiler = *mpDriver->GetSystem<Dystopia::Profiler>();
		profiler.Update(mDeltaTime);

		mpWin->Update(mDeltaTime);
		mpInput->Update(mDeltaTime);
		mpGuiSystem->StartFrame(mDeltaTime);

		UpdateKeys();
		UpdateHotkeys();
		
		mpEditorEventSys->FireAllPending();
		MainMenuBar();
	}

	void Editor::UpdateFrame(const float& _dt)
	{
		mpGfx->Update(mDeltaTime); 
		for (unsigned int i = 0; i < mTabsArray.size(); ++i)
		{
			EGUI::PushID(i);
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

			EditorTab *pTab = mTabsArray[i];
			if (EGUI::StartTab(pTab->GetLabel().c_str(), pTab->GetOpenedBool()))
			{
				pTab->SetSize(EGUI::Docking::GetTabSize(pTab->GetLabel().c_str()));
				pTab->SetPosition(EGUI::Docking::GetTabPosition(pTab->GetLabel().c_str()));
				pTab->SetSceneContext(&(mpSceneSystem->GetCurrentScene()));
				pTab->Update(_dt);
				pTab->EditorUI();
			}
			EGUI::EndTab();
			EGUI::PopID();
		}
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
		}

		mpInput->Shutdown();
		mpEditorEventSys->Shutdown();
		mpComdHandler->Shutdown();
		mpGuiSystem->Shutdown();

		delete mpInput;
		delete mpEditorEventSys;
		delete mpComdHandler;
		delete mpGuiSystem;
		delete mpTimer;

		mpEditorEventSys = nullptr;
		mpInput = nullptr;
		mpComdHandler = nullptr;
		mpGuiSystem = nullptr;
		mpTimer = nullptr;
		mpWin = nullptr;
		mpGfx = nullptr;

		mpDriver->Shutdown();
		EGUI::RemoveContext();
	}

	float Editor::GetDeltaTime() const
	{
		return mDeltaTime;
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

	GameObject* Editor::FindGameObject(const unsigned long& _id) const
	{
		return mpSceneSystem->GetCurrentScene().FindGameObject(_id);
	}
}

#endif		// EDITOR ONLY
