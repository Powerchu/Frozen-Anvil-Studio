/* HEADER *********************************************************************************/
/*!
\file	Editor.cpp
\author Digipen (100%)
\par    email: digipen\@digipen.edu
\brief
	INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
#include "System\Graphics\GraphicsSystem.h"
#include "System\Time\Timer.h"
#include "System\Driver\Driver.h"
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

	auto driver = Dystopia::EngineCore::GetInstance();
	Dystopia::Editor *editor = Dystopia::Editor::GetInstance();
	Dystopia::Timer *timer = new Dystopia::Timer{};
	
	driver->LoadSettings();

	driver->Init();
	editor->Init(driver->GetSystem<Dystopia::WindowManager>(),
				 driver->GetSystem<Dystopia::GraphicsSystem>());

	while (!editor->IsClosing())
	{
		float dt = timer->Elapsed();
		timer->Lap();
	
		editor->StartFrame(dt);
	
		editor->UpdateFrame(dt);
		
		editor->EndFrame();
	}

	editor->Shutdown();
	driver->Shutdown();
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
		mpInput{ new EditorInput{} },
		mpEditorEventSys{ new EditorEventHandler{} },
		mpComdHandler{ new CommandHandler{} },
		mpGuiSystem{ new GuiSystem{} }
	{}

	Editor::~Editor(void)
	{
	}

	void Editor::Init(WindowManager *_pWin, GraphicsSystem *_pGfx)
	{ 
		mpWin = _pWin;
		mpGfx = _pGfx;
		mpInput->Init();
		mpEditorEventSys->Init();
		EGUI::SetContext(mpComdHandler);

		LoadDefaults();
		for (auto& e : mTabsArray)
		{
			e->SetComdContext(mpComdHandler);
			e->SetEventSysContext(mpEditorEventSys);
			e->Init();
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
		BinarySerializer serial = BinarySerializer::OpenFile("SaveSettingsFile", std::ios::out);

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

	void Editor::UpdateHotkeys()
	{
		if (mpInput->IsKeyTriggered(KEY_LMOUSE))
			mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_LCLICK)->Fire();

		if (mpInput->IsKeyTriggered(KEY_RMOUSE))
			mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_RCLICK)->Fire();

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_Z))
			mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_UNDO)->Fire();

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_Y))
			mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_REDO)->Fire();

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_C))
			mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_COPY)->Fire();

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_X))
			mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_CUT)->Fire();

		if (mpInput->IsKeyPressed(KEY_CTRL) && mpInput->IsKeyTriggered(KEY_V))
			mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_PASTE)->Fire();
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
}

#endif		// EDITOR ONLY
