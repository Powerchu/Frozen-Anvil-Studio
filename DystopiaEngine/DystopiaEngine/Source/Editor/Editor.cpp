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
	Dystopia::BinarySerializer binSer = Dystopia::BinarySerializer::OpenFile("TestBinSer.txt2", 2);
	int testVar_int = 5;
	float testVar_float = 99.23f;
	double testVar_double = 1255.3245f;
	char testVar_char = 'A';
	binSer.Write(testVar_char);
	binSer.Write(testVar_int);
	binSer.Write(testVar_float);
	binSer.Write(testVar_double);
	binSer.~BinarySerializer();

	/* Alter values */
	testVar_int = 12312;
	testVar_float = 3.21;
	testVar_double = 123.009f;
	testVar_char = 'J';

	/* Deserialize*/
	Dystopia::BinarySerializer binSer2 = Dystopia::BinarySerializer::OpenFile("TestBinSer.txt2", 1);
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




namespace Dystopia
{
	Editor::Editor(void)
		: mCurrentState{ EDITOR_MAIN }, mNextState{ mCurrentState }, mStartTime{}, mEndTime{}, mPrevFrameTime{ 0 },
		mpWin{ nullptr }, mpGfx{ nullptr }, mpInput{ nullptr }, mGuiSysArray{0}
	{}

	Editor::~Editor(void)
	{}

	void Editor::Init(WindowManager *_pWin, GraphicsSystem *_pGfx, InputManager *_pInput)
	{
		mpWin = _pWin;
		mpGfx = _pGfx;
		mpInput = _pInput;

		ResourceView *pResView = new ResourceView{};
		GuiSystem *pGui =new GuiSystem{};

		if (!pGui->Init(mpWin, mpGfx, mpInput)) mCurrentState = EDITOR_EXIT;
		mGuiSysArray.push_back(pGui);

		pResView->Init();

		Save();
	}

	void Editor::StartFrame()
	{
		mStartTime = std::chrono::high_resolution_clock::now();
		for (auto e : mGuiSysArray)
			e->StartFrame(mPrevFrameTime);
	}

	void Editor::UpdateFrame(const float& _dt)
	{
		//mpWin->Update(_dt);
		//mpInput->Update(_dt);
		//mpGfx->Update(_dt);

		// if (mCurrentState == EDITOR_PLAY) call for update of current scene
		if (mCurrentState == EDITOR_PAUSE) return;
	}

	void Editor::EndFrame()
	{
		EGUI::Display::MainMenuBar();
		ImGui::SetNextWindowPos(ImVec2{ 0,0 });
		//ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		//const ImGuiWindowFlags flags = (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | 
		//								ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | 
		//								ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		//
		//const float oldWindowRounding = ImGui::GetStyle().WindowRounding;
		//const bool visible = ImGui::Begin("imguidock window (= lumix engine's dock system)", NULL, flags);
		//ImGui::GetStyle().WindowRounding = oldWindowRounding;
		//ImGui::GetStyle().WindowRounding = 0;
		//
		//if (visible)
		//{
		//	EGUI::Dock::BeginSpace(); //ImGui::BeginDockspace();
		//	static char tmp[128];
		//	for (int i = 0; i< 1; i++)
		//	{
		//		sprintf_s(tmp, "Dock %d", i);
		//		if (i == 9)
		//			EGUI::Dock::SetNextDock(EGUI::Dock::eDockSlot::eDOCK_BOTTOM); //ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
		//		if (EGUI::Dock::BeginDock(tmp)) //if (ImGui::BeginDock(tmp))
		//			ImGui::Text("Content of dock window %d goes here", i);
		//
		//		EGUI::Dock::EndDock(); //ImGui::EndDock();
		//	}
		//	EGUI::Dock::EndSpace(); //ImGui::EndDockspace();
		//}
		//ImGui::End();

		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::SetNextWindowBgAlpha(0.95f);
		if (ImGui::Begin("imguidock window (= lumix engine's dock system)", NULL, ImGuiWindowFlags_NoScrollbar))
		{
			EGUI::Dock::BeginSpace(); //ImGui::BeginDockspace();
			static char tmp[128];
			for (int i = 0; i< 2; i++)
			{
				sprintf_s(tmp, "Dock %d", i);
				if (i == 9)
					EGUI::Dock::SetNextDock(EGUI::Dock::eDockSlot::eDOCK_BOTTOM); //ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
				if (EGUI::Dock::BeginDock(tmp)) //if (ImGui::BeginDock(tmp))
					ImGui::Text("Content of dock window %d goes here", i);

				EGUI::Dock::EndDock(); //ImGui::EndDock();
			}
			EGUI::Dock::EndSpace(); //ImGui::EndDockspace();
		}
		ImGui::End();

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

	void Editor::Play()
	{
		// call for Init of the current scene. Assuming scene manager knows which is the current scene 
		// something like , mpSceneMgr->CurrentScene()->Init() using the temp file
	}

	void Editor::Save()
	{
		// call for serialization of all in current scene

		std::string myString = "Hello World";
		int myInt = 5;
		float myFloat = 10.23125f;
		double myDouble = 1231053;

		for (std::size_t i = 0; i < myString.size(); ++i)
			std::cout << std::bitset<8>(myString.c_str()[i]) << std::endl;

		std::cout << std::bitset<8>(myInt) << std::endl;
		std::cout << std::bitset<8>(myFloat) << std::endl;
		std::cout << std::bitset<8>(myDouble) << std::endl;
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

