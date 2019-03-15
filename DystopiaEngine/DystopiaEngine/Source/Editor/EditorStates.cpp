/* HEADER *********************************************************************************/
/*!
\file	EditorStates.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorStates.h"
#include "Editor/EditorMain.h"
#include "Editor/EInput.h"
#include "Editor/EditorClipboard.h"
#include "Editor/EditorCommands.h"
#include "Editor/EGUI.h"
#include "Editor/EditorPanel.h"
#include "Editor/EditorProc.h"
#include "Editor/RuntimeMeta.h"
#include "Editor/SceneView.h"
#include "Editor/ConsoleLog.h"

#include "System/Driver/Driver.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"
#include "System/Profiler/Profiler.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "System/File/FileSystem.h"
#include "System/Input/InputSystem.h"

#include "Utility/GUID.h"

Editor::EditorStates::EditorStates(void)
	: mState{ eState::LAUNCHER }, mbQuitAttempt{ false }, mbNewAttempt{ false },
	mbLoadAttempt{false}, mPendingLoad{}, mnPlay{}, mnNew{}, mnOpen{}, mnSave{}, mnSaveAs{}, mnQuit{},
	mnClear{}, mnSelectedGizmo(0), mArrTempFile{}
{
}

Editor::EditorStates::~EditorStates(void)
{
}

void Editor::EditorStates::Load(void)
{

}

bool Editor::EditorStates::Init(void)
{
	auto sceneName = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().GetSceneName();
	Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().SetTitle(std::wstring{ sceneName.begin(), sceneName.end() });

	Hotkey<2> toggle{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_P },
					  Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	Hotkey<2> newScene{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_N },
						Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	Hotkey<2> load{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_O },
					Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	Hotkey<2> save{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_S },
					Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	Hotkey<3> saveAs{ Array<eButton, 3>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_SHIFT, eButton::KEYBOARD_S },
					  Array<eHKState, 3>{ eHKState::eHK_HOLD, eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	Hotkey<3> quit{ Array<eButton, 3>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_ALT, eButton::KEYBOARD_Q },
					Array<eHKState, 3>{ eHKState::eHK_HOLD, eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	Hotkey<2> clear{ Array<eButton, 2>{ eButton::KEYBOARD_CTRL, eButton::KEYBOARD_1},
					Array<eHKState, 2>{ eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED } };

	auto input = EditorMain::GetInstance()->GetSystem<EInput>();
	mnPlay	= input->RegisterHotkey(toggle);
	mnNew	= input->RegisterHotkey(newScene);
	mnOpen	= input->RegisterHotkey(load);
	mnSave	= input->RegisterHotkey(save);
	mnSaveAs = input->RegisterHotkey(saveAs);
	mnQuit	= input->RegisterHotkey(quit);
	mnClear = input->RegisterHotkey(clear);

	return true;
}

void Editor::EditorStates::StartFrame(void)
{
	/******************************** NO UI TO BE CALLED HERE ********************************/

	const float dt = Editor::EditorMain::GetInstance()->GetDeltaTime();
	switch (mState)
	{
	case eState::LAUNCHER:
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->Update(dt);
		break;

	case eState::MAIN:
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->Update(dt);
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::Profiler>()->Update(dt);
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::BehaviourSystem>()->PollChanges();
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::FileSystem>()->Update(dt);
		break;
	default: ;
	}

	auto& win = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow();
	if (EditorMain::GetInstance()->GetSystem<EditorCommands>()->HasChanges())
	{
		if (win.GetTitle().back() != '*')
		{
			auto sceneSystem = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
			HashString name{ sceneSystem->GetCurrentScene().GetSceneName().c_str() };
			name += '*';
			win.SetTitle(std::wstring{ name.begin(), name.end() });
		}
	}

	/******************************** NO UI TO BE CALLED HERE ********************************/
}

void Editor::EditorStates::Update(float)
{
	const auto input = EditorMain::GetInstance()->GetSystem<EInput>();

	if (input->IsHotkeyTriggered(mnPlay))
	{
		if (mState == eState::MAIN)
			Play();
		else if (mState == eState::PLAY)
			Stop();
	}

	if (EditorMain::GetInstance()->GetCurState() == eState::MAIN)
	{
		if (input->IsHotkeyTriggered(mnNew))
			mbNewAttempt = true;
		else if (input->IsHotkeyTriggered(mnOpen))
			LoadScene();
		else if (input->IsHotkeyTriggered(mnSaveAs))
			SaveAs();
		else if (input->IsHotkeyTriggered(mnSave))
			Save();
		else if (input->IsHotkeyTriggered(mnQuit))
			mbQuitAttempt = true;
		else if (input->IsHotkeyTriggered(mnClear))
			EditorMain::GetInstance()->GetSystem<EditorCommands>()->ClearAllCommmands();

		if (EditorMain::GetInstance()->GetSystem<EInput>()->GetInputManager()->GetKeyDown(eButton::KEYBOARD_W))
			EditorMain::GetInstance()->GetPanel<SceneView>()->SetGizmoTranslate();
		else if (EditorMain::GetInstance()->GetSystem<EInput>()->GetInputManager()->GetKeyDown(eButton::KEYBOARD_E))
			EditorMain::GetInstance()->GetPanel<SceneView>()->SetGizmoScaler();
		else if (EditorMain::GetInstance()->GetSystem<EInput>()->GetInputManager()->GetKeyDown(eButton::KEYBOARD_HOME))
			EditorMain::GetInstance()->GetPanel<SceneView>()->ResetSceneCam();
		else if (EditorMain::GetInstance()->GetSystem<EInput>()->GetInputManager()->GetKeyDown(eButton::KEYBOARD_F))
			EditorMain::GetInstance()->GetPanel<SceneView>()->FocusGobj();
	}

	switch (mState)
	{
	case eState::PLAY:
		Dystopia::EngineCore::GetInstance()->FixedUpdate();
		Dystopia::EngineCore::GetInstance()->Update();
		break;
	default: ;
	}

	ToolBar();
}

void Editor::EditorStates::EndFrame(void)
{
	bool changes = EditorMain::GetInstance()->GetSystem<EditorCommands>()->HasChanges();
	if (mbNewAttempt)
	{
		if (changes)
			PromptSaveN();
		else
			New();
	}
	else if (mbQuitAttempt)
	{
		if (changes)
			PromptSaveQ();
		else
		{
			EditorMain::GetInstance()->ChangeState(eState::EXIT);
			mbQuitAttempt = false;
		}
	}
	else if (mbLoadAttempt)
	{
		if (changes)
			PromptSaveO();
		else
		{
			if (mPendingLoad.size())
			{
				Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->LoadScene(std::string{ mPendingLoad.cbegin(), mPendingLoad.cend() });
				EditorMain::GetInstance()->GetSystem<EditorCommands>()->ClearAllCommmands();
				mPendingLoad.clear();
			}
			else
				LoadScene();
			mbLoadAttempt = false;
		}
	}
}

void Editor::EditorStates::Shutdown(void)
{
	for (auto& e : mArrTempFile)
		remove(e.c_str());
	mArrTempFile.clear();
}

void Editor::EditorStates::Message(eEMessage _msg)
{
	if (_msg == eEMessage::STATE_CHANGED)
	{
		auto oldState = mState;
		mState = Editor::EditorMain::GetInstance()->GetCurState();
		if (oldState == eState::MAIN && mState == eState::PLAY)
		{
			TempSave();
			Dystopia::EngineCore::Get<Dystopia::SceneSystem>()->GetCurrentScene().Init();
		}
		else if (oldState == eState::PLAY && mState == eState::MAIN)
			TempLoad();
	}
	else if (_msg == eEMessage::SCENE_CHANGED)
	{
		auto sceneSystem = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
		auto& win = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow();
		HashString name{ sceneSystem->GetCurrentScene().GetSceneName().c_str() };
		win.SetTitle(std::wstring{ name.begin(), name.end() });
	}
	else if (_msg == eEMessage::WINDOWS_QUIT)
	{
		mbQuitAttempt = true;
	}
}

void Editor::EditorStates::SaveSettings(Dystopia::TextSerialiser&) const
{
}

void Editor::EditorStates::LoadSettings(Dystopia::TextSerialiser&)
{
}

void Editor::EditorStates::ToolBar(void)
{
	if (EGUI::StartMainMenuBar())
	{
		bool isMain = mState == eState::MAIN;
		if (EGUI::StartMenuHeader("File", isMain))
		{
			if (EGUI::StartMenuBody("New", "Ctrl + N"))
				mbNewAttempt = true;
			if (EGUI::StartMenuBody("Open", "Ctrl + O"))
				LoadScene(); 
			if (EGUI::StartMenuBody("Save", "Ctrl + S"))
				Save();
			if (EGUI::StartMenuBody("Save As..", "Ctrl + Shift + S"))
				SaveAs(); 
			if (EGUI::StartMenuBody("Quit", "Ctrl + Alt + Q"))
				mbQuitAttempt = true; 
			EGUI::EndMenuHeader();
		}
		if (EGUI::StartMenuHeader("Edit", isMain))
		{
			if (EGUI::StartMenuBody("Undo ", "Ctrl + Z"))
				EditorMain::GetInstance()->GetSystem<EditorCommands>()->Undo(); 
			if (EGUI::StartMenuBody("Redo ", "Ctrl + Y"))	
				EditorMain::GetInstance()->GetSystem<EditorCommands>()->Redo(); 
			if (EGUI::StartMenuBody("Copy ", "Ctrl + C"))	
				EditorMain::GetInstance()->GetSystem<EditorClipboard>()->Copy(); 
			if (EGUI::StartMenuBody("Paste ", "Ctrl + V"))	
				EditorMain::GetInstance()->GetSystem<EditorClipboard>()->Paste();
			auto command = EditorMain::GetInstance()->GetSystem<EditorCommands>();
			HashString clearString = "Clear (";
			clearString += command->ComdSize();
			clearString += ")";
			if (EGUI::StartMenuBody(clearString.c_str(), "Ctrl + 1"))
				command->ClearAllCommmands();
			EGUI::EndMenuHeader();
		}
		static constexpr float icon = 10.f;
		if (EGUI::StartMenuHeader("View"))
		{
			auto& allPanels = EditorMain::GetInstance()->GetAllPanels();
			for (auto& p : allPanels)
			{
				if (p->IsOpened())
					EGUI::Display::IconTick(icon, icon);
				else
					EGUI::Display::Dummy(icon, icon);

				EGUI::SameLine();
				if (EGUI::StartMenuBody(p->GetLabel().c_str()))
					p->SetOpened(!(p->IsOpened()));
			}
			EGUI::EndMenuHeader();
		}
		if (EGUI::StartMenuHeader("Game"))
		{
			if (EGUI::StartMenuBody("Play", "Ctrl + P", isMain))
			{ 
				Play();
			}
			if (EGUI::StartMenuBody("Stop", "Ctrl + P", mState == eState::PLAY))
			{ 
				Stop();
			}
			EGUI::EndMenuHeader();
		}

		EditorPlayBar();
		GizmoRadios();

		EGUI::EndMainMenuBar(); // end of main menu
	}
}

void Editor::EditorStates::Save(void)
{
	auto sceneSystem = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
	HashString sceneName{ sceneSystem->GetCurrentScene().GetSceneName().c_str() };
	if (sceneName != "Untitled")
	{
		HashString sceneFile{ sceneName };
		sceneFile += ".";
		sceneFile += Gbl::SCENE_EXT;
		auto fs = Dystopia::EngineCore::GetInstance()->Get<Dystopia::FileSystem>();
		auto fp = fs->FindFilePath(sceneFile, Dystopia::eFileDir::eResource);

		sceneSystem->SaveScene(fp.c_str(), sceneName.c_str());

		EditorMain::GetInstance()->GetSystem<EditorCommands>()->SavedChanges();
		auto& win = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow();
		win.SetTitle(std::wstring{ sceneName.begin(), sceneName.end() });
	}
	else SaveAs();
}

void Editor::EditorStates::OpenScene(const HashString& _path)
{
	mPendingLoad = _path;
	mbLoadAttempt = true;
}

void Editor::EditorStates::ChangeGizmo(const int& _gizmoVal)
{
	if (_gizmoVal < 0) mnSelectedGizmo = 0;
	else if (_gizmoVal > 2) mnSelectedGizmo = 2;
	else mnSelectedGizmo = _gizmoVal;
}

void Editor::EditorStates::LoadScene(void)
{
	Dystopia::EditorProc p;
	HashString path;
	HashString name;
	if (p.Load(path, eFileTypes::eSCENE))
	{
		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->LoadScene(std::string{ path.begin(), path.end() });
		EditorMain::GetInstance()->GetSystem<EditorCommands>()->ClearAllCommmands();
	}
}

void Editor::EditorStates::SaveAs(void)
{
	Dystopia::EditorProc p;
	HashString path;
	HashString name;
	auto& win = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow();
	if (p.SaveAs(name, path, win.GetWindowHandle(), eFileTypes::eSCENE))
	{
		win.SetTitle(std::wstring{ name.begin(), name.end() });
		auto sceneSystem = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
		sceneSystem->SaveScene(std::string{ path.begin(), path.end() },
							   std::string{ name.begin(), name.end() });

		EditorMain::GetInstance()->GetSystem<EditorCommands>()->SavedChanges();
	}
}

void Editor::EditorStates::New(void)
{
	auto sceneSystem = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
	auto fs = Dystopia::EngineCore::GetInstance()->Get<Dystopia::FileSystem>();
	auto fp = fs->GetFullPath("Temp", Dystopia::eFileDir::eResource);

	HashString file{ fp.c_str() };
	file += "/";
	file += Dystopia::GUIDGenerator::GetUniqueID();
	file += ".";
	file += Gbl::SCENE_EXT;

	FILE *pfile;
	if (!fopen_s(&pfile, file.c_str(), "w"))
		fclose(pfile);

	sceneSystem->LoadScene(std::string{ file.begin(), file.end() });
	EditorMain::GetInstance()->GetSystem<EditorCommands>()->ClearAllCommmands();
	mbNewAttempt = false;
}

void Editor::EditorStates::Play(void)
{
	EditorMain::GetInstance()->ChangeState(eState::PLAY);
	Dystopia::EngineCore::GetInstance()->InterruptContinue();
	if (Editor::EditorMain::GetInstance()->GetPanel<Editor::ConsoleLog>()->GetIsClearedOnPlay())
		Editor::EditorMain::GetInstance()->GetPanel<Editor::ConsoleLog>()->Clear();

	Dystopia::SystemList<std::make_index_sequence<Ut::SizeofList<Dystopia::UsableComponents>::value>>::InitDonors();
}

void Editor::EditorStates::Pause()
{
	//EditorMain::GetInstance()->ChangeState(eState::PAUSED);
	Dystopia::EngineCore::GetInstance()->Interrupt();
}

void Editor::EditorStates::Resume()
{
	//EditorMain::GetInstance()->ChangeState(eState::PLAY);
	Dystopia::EngineCore::GetInstance()->InterruptContinue();
}

void Editor::EditorStates::Stop(void)
{
	EditorMain::GetInstance()->ChangeState(eState::MAIN);
}

void Editor::EditorStates::TempSave(void)
{
	auto fs = Dystopia::EngineCore::GetInstance()->Get<Dystopia::FileSystem>();
	auto fp = fs->GetFullPath("Temp", Dystopia::eFileDir::eResource);
	auto sceneSystem = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();

	HashString file{ fp.c_str() };
	file += "\\";
	file += Dystopia::GUIDGenerator::GetUniqueID();
	file += ".";
	file += Gbl::SCENE_EXT;

	sceneSystem->SaveScene(file.c_str(), sceneSystem->GetCurrentScene().GetSceneName());
	mArrTempFile.Insert(file);
}

void Editor::EditorStates::TempLoad(void)
{
	auto sceneSystem = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
	sceneSystem->RestartScene();
}

void Editor::EditorStates::PromptSaveN(void)
{
	EGUI::Display::OpenPopup("Creating new scene");
	if (EGUI::Display::StartPopupModal("Creating new scene", "There are unsaved changes!"))
	{
		if (EGUI::Display::Button("Save"))
		{
			Save();
			New();
			mbNewAttempt = false;
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::SameLine(15);
		if (EGUI::Display::Button("Don't Save", {70,24}))
		{
			New();
			mbNewAttempt = false;
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::SameLine(15);
		if (EGUI::Display::Button("Cancel"))
		{
			mbNewAttempt = false;
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::Display::EndPopup();
	}
}

void Editor::EditorStates::PromptSaveQ(void)
{
	EGUI::Display::OpenPopup("Exiting");
	if (EGUI::Display::StartPopupModal("Exiting", "There are unsaved changes!"))
	{
		if (EGUI::Display::Button("Save"))
		{
			Save();
			mbQuitAttempt = false;
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::SameLine(15);
		if (EGUI::Display::Button("Cancel"))
		{
			mbQuitAttempt = false;
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::SameLine(15);
		if (EGUI::Display::Button("Exit"))
		{
			mbQuitAttempt = false;
			EditorMain::GetInstance()->ChangeState(eState::EXIT);
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::Display::EndPopup();
	}
}

void Editor::EditorStates::EditorPlayBar()
{
	float playBarPos = ImGui::GetWindowWidth() * 0.5f - 123.f;
	std::string state;

	if (mState != eState::PLAY)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f,0.5f,0.5f,1.0f });
		state = "Play";
	}
	else if (mState != eState::PAUSED)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f,0.1f,0.17f,0.5f });
		state = "Stop";
	}
	

	ImGui::SetCursorPosX(playBarPos);
	EGUI::ChangeAlignmentYOffset(0);
	if (EGUI::Display::Button(state.c_str(), Math::Vec2{ 50,23}))
	{
		if (mState != eState::PLAY)
		{
			Play();
		}
		else
		{
			Stop();
		}
	};

	EGUI::SameLine();
	if (EGUI::Display::Button("Pause", Math::Vec2{ 50,23 }))
	{
		
	};
	EGUI::SameLine();
	if (EGUI::Display::Button("Step", Math::Vec2{ 50,23 }))
	{
		
	};
	EGUI::ChangeAlignmentYOffset();
	ImGui::SetCursorPosX(0);

	ImGui::PopStyleColor();

}

void Editor::EditorStates::GizmoRadios()
{
	EGUI::SameLine(10.f);
	//ImGui::SetCursorPosX(gizmoRadioPos);
	EGUI::Display::Label("Translate (W)");
	EGUI::SameLine();
	ImGui::SetCursorPosY(2.f);
	if (EGUI::Display::RadioBtn("##transG", &mnSelectedGizmo, 0, false))
	{
		EditorMain::GetInstance()->GetPanel<SceneView>()->SetGizmoTranslate();
	};
	EGUI::SameLine(5.f);
	ImGui::SetCursorPosY(0.f);
	EGUI::Display::Label("Scale (E)");
	EGUI::SameLine();
	ImGui::SetCursorPosY(2.f);
	if (EGUI::Display::RadioBtn("##scaleG", &mnSelectedGizmo, 1, false))
	{
		EditorMain::GetInstance()->GetPanel<SceneView>()->SetGizmoScaler();
	};
	EGUI::SameLine(5.f);
	ImGui::SetCursorPosY(0.f);
	EGUI::Display::Label("Rotate (R)");
	EGUI::SameLine();
	ImGui::SetCursorPosY(2.f);
	EGUI::Display::RadioBtn("##rotG", &mnSelectedGizmo, 2, false);
	ImGui::SetCursorPosY(0.f);
	//ImGui::SetCursorPosX(0);
}

void Editor::EditorStates::PromptSaveO(void)
{
	EGUI::Display::OpenPopup("Loading");
	if (EGUI::Display::StartPopupModal("Loading", "There are unsaved changes!"))
	{
		if (EGUI::Display::Button("Save"))
		{
			Save();
			mbLoadAttempt = false;
			if (mPendingLoad.size())
			{
				Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->LoadScene(std::string{ mPendingLoad.cbegin(), mPendingLoad.cend() });
				EditorMain::GetInstance()->GetSystem<EditorCommands>()->ClearAllCommmands();
				mPendingLoad.clear();
			}
			else
				LoadScene();
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::SameLine(15);
		if (EGUI::Display::Button("Don't Save", {70,24}))
		{
			mbLoadAttempt = false;
			if (mPendingLoad.size())
			{
				Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->LoadScene(std::string{ mPendingLoad.cbegin(), mPendingLoad.cend() });
				EditorMain::GetInstance()->GetSystem<EditorCommands>()->ClearAllCommmands();
				mPendingLoad.clear();
			}
			else
				LoadScene();
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::SameLine(15);
		if (EGUI::Display::Button("Cancel"))
		{
			mbLoadAttempt = false;
			EGUI::Display::CloseCurrentPopup();
		}
		EGUI::Display::EndPopup();
	}
}

#endif




