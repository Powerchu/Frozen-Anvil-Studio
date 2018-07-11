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
#include "System\Window\WindowManager.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Input\InputSystem.h"
#include "System\Time\Timer.h"
#include "Editor\Editor.h"
#include "Editor\EGUI.h"
#include "Editor\ResourceView.h"
#include "Editor\Inspector.h"
#include "Editor\HierarchyView.h"
#include "Editor\Commands.h"
#include "Editor\CommandList.h"
#include "Editor\EditorTab.h"
#include "IO\BinarySerializer.h"
#include "System\Driver\Driver.h"
#include <iostream>
#include <bitset>
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

// Entry point for editor
int WinMain(HINSTANCE hInstance, HINSTANCE, char *, int)
{
	hInstance;

	auto driver = Dystopia::EngineCore::GetInstance();
	Dystopia::Editor *editor = new Dystopia::Editor{};
	Dystopia::Timer *timer = new Dystopia::Timer{};

	driver->LoadSettings();
	driver->Init();
	editor->Init(driver->GetSystem<Dystopia::WindowManager>(),
				 driver->GetSystem<Dystopia::GraphicsSystem>(),
				 driver->GetSystem<Dystopia::InputManager>());

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
	delete editor;

	_CrtDumpMemoryLeaks();
	return 0;
}


/*///////////////////////////////////////////////////////////////////// EDITOR CLASS ////////////////////////////////////////////////////////////////////////////////*/

namespace Dystopia
{
	Editor::Editor(void)
		: mCurrentState{ EDITOR_MAIN }, mNextState{ mCurrentState }, mPrevFrameTime{ 0 }, mpComdHandler{ new CommandHandler{} },
		mpWin{ nullptr }, mpGfx{ nullptr }, mpInput{ nullptr }, mpGuiSystem{ new GuiSystem{} }, mExtraTabCounter{ 0 }
	{}

	Editor::~Editor(void)
	{}

	void Editor::Init(WindowManager *_pWin, GraphicsSystem *_pGfx, InputManager *_pInput)
	{ 
		mpWin = _pWin;
		mpGfx = _pGfx;
		mpInput = _pInput;

		mTabsArray.push_back(new Inspector{});
		mTabsArray.push_back(new ResourceView{});
		mTabsArray.push_back(new HierarchyView{});

		EGUI::SetContext(mpComdHandler);

		for (auto e : mTabsArray)
			e->Init();

		if (!mpGuiSystem->Init(mpWin, mpGfx, mpInput))
			mCurrentState = EDITOR_EXIT;
	}

	void Editor::StartFrame(const float& _dt)
	{
		mpWin->Update(_dt);
		mpInput->Update(_dt);

		mpGuiSystem->StartFrame(static_cast<float>(mPrevFrameTime));
		MainMenu();
	}

	void Editor::UpdateFrame(const float& _dt)
	{
		//mpGfx->Update(_dt);

		for (unsigned int i = 0; i < mTabsArray.size(); ++i)
		{
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

			if (EGUI::StartTab(pTab->GetLabel().c_str()))
			{
				pTab->SetSize(EGUI::Docking::GetTabSize(pTab->GetLabel().c_str()));
				pTab->Window();
			}
			EGUI::EndTab();
		}

		char buffer1[8];
		char buffer2[8];
		for (int i = 0; i < mExtraTabCounter; ++i)
		{
			char tempBuff[128];
			_itoa_s(i, buffer1, 10);
			_itoa_s(i, buffer2, 10);
			strcpy_s(tempBuff, "Spawned Tab ");
			strcat_s(tempBuff, buffer1);
			EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), EGUI::Docking::eDOCK_SLOT_NONE);
			if (EGUI::StartTab(tempBuff))
			{
				strcpy_s(tempBuff, "I am Tab ");
				strcat_s(tempBuff, buffer2);
				EGUI::Display::Label(tempBuff);
			}
			EGUI::EndTab();
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
		EGUI::Docking::ShutdownTabs();
		for (auto e : mTabsArray)
		{
			if (e)
			{
				e->Shutdown();
				delete e;
				e = nullptr;
			}
		}

		mpWin = nullptr;
		mpGfx = nullptr;
		mpInput = nullptr;

		delete mpComdHandler;
		mpComdHandler = nullptr;

		mpGuiSystem->Shutdown();
		delete mpGuiSystem;
		mpGuiSystem = nullptr;
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

	double Editor::PreviousFrameTime() const
	{
		return mPrevFrameTime;
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

	void Editor::MainMenu()
	{
		if (EGUI::StartMainMenuBar())
		{
			if (EGUI::StartMenuHeader("File"))
			{
				if (EGUI::StartMenuBody("New (Spawn Tabs)"))
				{
					// TODO: Some actual function
					mpComdHandler->InvokeCommand(new ComdModifyValue<int> { &mExtraTabCounter, mExtraTabCounter + 1 });
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
			// TODO: Some actual function for all the bottom
			if (EGUI::StartMenuHeader("Edit"))
			{
				if (EGUI::StartMenuBody("Undo","Ctrl + Z")) { mpComdHandler->UndoCommand(); }
				if (EGUI::StartMenuBody("Redo", "Ctrl + Y")) { mpComdHandler->RedoCommand(); } 
				if (EGUI::StartMenuBody("Cut")) {}
				if (EGUI::StartMenuBody("Copy")) {}
				if (EGUI::StartMenuBody("Paste")) {}
				EGUI::EndMenuHeader();
			}
			EGUI::EndMainMenuBar();
		}
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
}

#endif		// EDITOR ONLY
