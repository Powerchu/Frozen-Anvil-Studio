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

#include "System\Window\WindowManager.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Input\InputSystem.h"
#include "System\Time\Timer.h"
#include "Editor\Editor.h"
#include "Editor\EGUI.h"
#include <iostream>

// Entry point for editor
int WinMain(HINSTANCE hInstance, HINSTANCE, char *, int)
{
	hInstance;

	Dystopia::WindowManager *win = new Dystopia::WindowManager{};
	Dystopia::GraphicsSystem *gfx = new Dystopia::GraphicsSystem{};
	Dystopia::InputManager *input = new Dystopia::InputManager{};
	Dystopia::Editor *editor = new Dystopia::Editor{};
	Dystopia::Timer timer{};

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

		GuiSystem* pGui =new GuiSystem{};
		if (!pGui->Init(mpWin, mpGfx, mpInput)) mCurrentState = EDITOR_EXIT;
			mGuiSysArray.push_back(pGui);
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
		std::cout << _dt << "\n";
	}

	void Editor::EndFrame()
	{
		EGUI::Display::MainMenuBar();
		ImGui::SetNextWindowPos(ImVec2{ 0,0 });
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		const ImGuiWindowFlags flags = (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | 
										ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | 
										ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		
		const float oldWindowRounding = ImGui::GetStyle().WindowRounding;
		const bool visible = ImGui::Begin("imguidock window (= lumix engine's dock system)", NULL, flags);
		ImGui::GetStyle().WindowRounding = oldWindowRounding;
		ImGui::GetStyle().WindowRounding = 0;

		if (visible)
		{
			EGUI::Dock::BeginSpace(); //ImGui::BeginDockspace();
			static char tmp[128];
			for (int i = 0; i< 1; i++)
			{
				sprintf(tmp, "Dock %d", i);
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

