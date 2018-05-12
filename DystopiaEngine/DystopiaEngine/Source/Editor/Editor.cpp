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

	win->LoadDefaults();
	win->Init();
	gfx->InitOpenGL(win->GetWindow());
	input->LoadDefaults();
	input->Init();

	editor->Init(win, gfx, input);
	while (!editor->IsClosing())
	{
		editor->StartFrame();

		editor->UpdateFrame(editor->PreviousFrameTime());
		
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

		SharedPtr<GuiSystem> pGui = CreateShared<GuiSystem>(new GuiSystem{});
		if (!pGui->Init(mpWin, mpGfx, mpInput)) mCurrentState = EDITOR_EXIT;
		//mGuiSysArray.push_back(pGui);
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

