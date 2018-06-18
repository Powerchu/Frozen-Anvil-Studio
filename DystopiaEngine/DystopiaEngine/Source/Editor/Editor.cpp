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
#include "IO\BinarySerializer.h"
#include <iostream>
#include <bitset>

// Entry point for editor
int WinMain(HINSTANCE hInstance, HINSTANCE, char *, int)
{
	hInstance;

	Dystopia::WindowManager *win = new Dystopia::WindowManager{};
	Dystopia::GraphicsSystem *gfx = new Dystopia::GraphicsSystem{};
	Dystopia::InputManager *input = new Dystopia::InputManager{};
	Dystopia::Editor *editor = new Dystopia::Editor{};
	Dystopia::Timer timer{};

	/* =======================================================================================*/
	/* =========================Start of Binary Serializer Test Case =========================*/
	/* Serialize */
	// TOGGLE the binSer open file and manual endian override to test between little and big endian

	Dystopia::BinarySerializer binSer = Dystopia::BinarySerializer::OpenFile("TestBinSerLittle", 2);
	binSer.ManualEndainOverride(Dystopia::eEndianess::eENDIAN_LITTLE);

	// Dystopia::BinarySerializer binSer = Dystopia::BinarySerializer::OpenFile("TestBinSerBig", 2);
	// binSer.ManualEndainOverride(Dystopia::eEndianess::eENDIAN_BIG);

	int testVar_int = 5;
	float testVar_float = 99.23f;
	double testVar_double = 1255.3245;
	char testVar_char = 'A';

	binSer.Write(testVar_char);
	binSer.Write(testVar_int);
	binSer.Write(testVar_float);
	binSer.Write(testVar_double);
	binSer.~BinarySerializer();

	/* Alter values */
	testVar_int = 12312;
	testVar_float = 3.21f;
	testVar_double = 123.009;
	testVar_char = 'J';

	/* Deserialize*/
	// TOGGLE the binSer open file and manual endian override to test between little and big endian

	Dystopia::BinarySerializer binSer2 = Dystopia::BinarySerializer::OpenFile("TestBinSerLittle", 1);
	binSer2.ManualEndainOverride(Dystopia::eEndianess::eENDIAN_LITTLE);

	// Dystopia::BinarySerializer binSer2 = Dystopia::BinarySerializer::OpenFile("TestBinSerBig", 1);
	// binSer2.ManualEndainOverride(Dystopia::eEndianess::eENDIAN_BIG);

	binSer2.Read(testVar_char);
	binSer2.Read(testVar_int);
	binSer2.Read(testVar_float);
	binSer2.Read(testVar_double);
	binSer2.~BinarySerializer();
	/* =========================End of Binary Serializer Test Case ===========================*/
	/* =======================================================================================*/

	win->LoadDefaults();
	win->Init();
	gfx->InitOpenGL(win->GetMainWindow());
	input->LoadDefaults();
	input->Init();

	win->DestroySplash();

	editor->Init(win, gfx, input);
	while (!editor->IsClosing())
	{
		float dt = timer.Elapsed();	//static_cast<float>(editor->PreviousFrameTime());
		timer.Lap();

		win->Update(dt);
		input->Update(dt);

		editor->StartFrame();

		editor->UpdateFrame(dt);
		
		editor->EndFrame();
	}
	editor->Shutdown();

	input->Shutdown();
	gfx->Shutdown();
	win->Shutdown();

	delete win;
	delete gfx;
	delete input;

	return 0;
}


/*///////////////////////////////////////////////////////////////////// EDITOR CLASS ////////////////////////////////////////////////////////////////////////////////*/

namespace Dystopia
{
	Editor::Editor(void)
		: mCurrentState{ EDITOR_MAIN }, mNextState{ mCurrentState }, mStartTime{}, mEndTime{}, mPrevFrameTime{ 0 },
		mpWin{ nullptr }, mpGfx{ nullptr }, mpInput{ nullptr }, mGuiSysArray{ 0 }, mExtraTabCounter{ 0 },
		mpHierarchy{ nullptr }, mpInspector{ nullptr }, mpResource{ nullptr }, mpDockableSpace{ "Dockable Space" }
	{}

	Editor::~Editor(void)
	{}

	void Editor::Init(WindowManager *_pWin, GraphicsSystem *_pGfx, InputManager *_pInput)
	{
		mpWin = _pWin;
		mpGfx = _pGfx;
		mpInput = _pInput;
		mpHierarchy = new HierarchyView{};
		mpInspector = new Inspector{};
		mpResource = new ResourceView{};

		GuiSystem *pGui =new GuiSystem{};
		if (!pGui->Init(mpWin, mpGfx, mpInput)) mCurrentState = EDITOR_EXIT;
		mGuiSysArray.push_back(pGui);
	}

	void Editor::StartFrame()
	{
		mStartTime = std::chrono::high_resolution_clock::now();
		for (auto e : mGuiSysArray)
			e->StartFrame(mPrevFrameTime);

		MainMenu();
	}

	void Editor::UpdateFrame(const float _dt)
	{
		//mpWin->Update(_dt);
		//mpInput->Update(_dt);
		//mpGfx->Update(_dt);

		mpInspector->Update(_dt);
		EGUI::Docking::SetNextTabs(mpDockableSpace, EGUI::Docking::eDOCK_SLOT_RIGHT);
		if (EGUI::StartTab("Inspector"))
			mpInspector->Window();
		EGUI::EndTab();

		mpResource->Update(_dt);
		EGUI::Docking::SetNextTabs(mpDockableSpace, EGUI::Docking::eDOCK_SLOT_LEFT);
		if (EGUI::StartTab("Resource"))
			mpResource->Window();
		EGUI::EndTab();

		mpHierarchy->Update(_dt);
		EGUI::Docking::SetNextTabs(mpDockableSpace, EGUI::Docking::eDOCK_SLOT_TOP);
		if (EGUI::StartTab("Hierarchy"))
			mpHierarchy->Window();
		EGUI::EndTab();

		EGUI::Docking::SetNextTabs(mpDockableSpace, EGUI::Docking::eDOCK_SLOT_RIGHT);
		if (EGUI::StartTab("Tab1"))
			EGUI::Display::Label("I'm ahahahaa!");
		EGUI::EndTab();


		char buffer1[8];
		char buffer2[8];
		for (int i = 0; i < mExtraTabCounter; ++i)
		{
			char tempBuff[128];
			_itoa_s(i, buffer1, 10);
			_itoa_s(i, buffer2, 10);
			strcpy_s(tempBuff, "Spawned Tab ");
			strcat_s(tempBuff, buffer1);
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
		for (auto e : mGuiSysArray)
			e->EndFrame();

		if (mCurrentState != mNextState) UpdateState();

		mEndTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time = mEndTime - mStartTime;
		mPrevFrameTime = time.count();
	}

	void Editor::Shutdown()
	{
		mpWin = nullptr;
		mpGfx = nullptr;
		mpInput = nullptr;
		if (mpHierarchy) delete mpHierarchy;
		if (mpInspector) delete mpInspector;
		if (mpResource) delete mpResource;
		while (!mGuiSysArray.IsEmpty())
		{
			mGuiSysArray.back()->Shutdown();
			mGuiSysArray.pop_back();
		}
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
					++mExtraTabCounter;
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
				}
				EGUI::EndMenuHeader();
			}
			// TODO: Some actual function for all the bottom
			if (EGUI::StartMenuHeader("Edit"))
			{
				if (EGUI::StartMenuBody("Undo")) {}
				if (EGUI::StartMenuBody("Redo")) {}  // Disabled ite
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
