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
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <memory>
/* System includes */
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Driver/Driver.h"
#include "System/Profiler/Profiler.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "System/File/FileSystem.h"
#include "System//Behaviour/BehaviourSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Input/InputSystem.h"
#include "IO/BinarySerializer.h"
#include "Utility/GUID.h"

#include "Component/Component.h"

/* Editor includes */
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#include "Editor/ProjectSettings.h"
#include "Editor/EditorEvents.h"
#include "Editor/Commands.h"
#include "Editor/Inspector.h"
#include "Editor/HierarchyView.h"
#include "Editor/ProjectResource.h"
#include "Editor/SceneView.h"
#include "Editor/ConsoleLog.h"
#include "Editor/PerformanceLog.h"
#include "Editor/PLogger.h"
#include "Editor/ColorScheme.h"
#include "Editor/StyleScheme.h"
#include "Editor/Clipboard.h"
#include "Editor/SpritePreviewer.h"
#include "Editor/SpriteEditor.h"

#include "Allocator/DefaultAlloc.h"
#include "DataStructure/HashString.h"

/* library includes */
#include <iostream>
#include <windows.h>
#include <bitset>
#include <ShlObj_core.h>
#include <tchar.h>
#include <objbase.h>

#include "System/Input/XGamePad.h"

#include "Editor/EditorProc.h"
#include "Editor/EditorInput.h"

namespace
{
	static const std::string DYSTOPIA_EDITOR_SETTINGS = "EditorSettings.dyst";
	static const std::string DYSTOPIA_SCENE_LOAD = "Resource/Scene/";
	static const std::string DYSTOPIA_SCENE_TEMP = "Resource/Temp/";
	static const std::wstring DYSTOPIA_SCENE_EXTENSION = L"dscene";
	static constexpr COMDLG_FILTERSPEC DYSTOPIA_SCENE_FILTER_EXTENSION[1] =
	{
		{ L"DystopiaScene", L"*.dscene" }
	};
}

// Entry point for editor
int WinMain(HINSTANCE, HINSTANCE, char *, int)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

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
	static const std::string g_nsPopup = "Prompt Save";
	Editor* Editor::GetInstance()
	{
		if (gpInstance) return gpInstance;
		gpInstance = new Editor{};
		return gpInstance;
	}

	Editor::Editor(void)
		: mCurrentState{ EDITOR_MAIN }, mNextState{ mCurrentState }, mpWin{ nullptr }, mpGfx{ nullptr },
		mpSceneSystem{ nullptr }, mpProfiler{ nullptr }, mTempSaveFile{}, mSceneHasChanged{ true },
		mpEditorEventSys{ new EditorEventHandler{} },
		mpComdHandler{ new CommandHandler{} }, mpGuiSystem{ new GuiSystem{} }, mpTimer{ new Timer{} },
		mpClipBoard{ new Clipboard{} }, mArrSelectedObj{ 100 }, mUpdateSelection{ true },
		mpEditorInput{ new EditorInput{}  }
	{}

	Editor::~Editor(void)
	{
	}

	void Editor::LoadTabs()
	{
		mArrTabs.push_back(Inspector::GetInstance());
		mArrTabs.push_back(ProjectResource::GetInstance());
		mArrTabs.push_back(HierarchyView::GetInstance());
		mArrTabs.push_back(SceneView::GetInstance());
		mArrTabs.push_back(ProjectSettings::GetInstance());
		mArrTabs.push_back(ConsoleLog::GetInstance());
		mArrTabs.push_back(PerformanceLog::GetInstance());
		mArrTabs.push_back(SpritePreviewer::GetInstance());
		mArrTabs.push_back(SpriteEditor::GetInstance());
		mArrTabs.push_back(ColorScheme::GetInstance());
		mArrTabs.push_back(StyleScheme::GetInstance());
	}

	void Editor::LoadDefaults()
	{
		std::string name = mpSceneSystem->GetCurrentScene().GetSceneName();
		mpWin->GetMainWindow().SetTitle(std::wstring{ name.begin(), name.end() });
	}

	void Editor::LoadSettings()
	{
		struct stat buffer;
		if (stat(DYSTOPIA_EDITOR_SETTINGS.c_str(), &buffer) == 0)
		{
			auto serial = TextSerialiser::OpenFile(DYSTOPIA_EDITOR_SETTINGS, TextSerialiser::MODE_READ);
			for (auto& e : mArrTabs)
				e->LoadSettings(serial);
		}
		else std::ofstream o{ DYSTOPIA_EDITOR_SETTINGS.c_str() };
	}

	void Editor::Init()
	{
		mpDriver		= Dystopia::EngineCore::GetInstance();
		mpDriver->LoadSettings();
		mpDriver->PreInit();

		mpWin			= mpDriver->GetSystem<WindowManager>();		// driver init-ed
		mpGfx			= mpDriver->GetSystem<GraphicsSystem>();	// driver init-ed
		mpSceneSystem	= mpDriver->GetSystem<SceneSystem>();		// driver init-ed
		mpProfiler		= mpDriver->GetSystem<Profiler>();			// driver init-ed
		mpBehaviourSys	= mpDriver->GetSystem<BehaviourSystem>();	// driver init-ed

		if (!mpGuiSystem->Init(mpWin, mpGfx, mpEditorInput))
		{
			mCurrentState = EDITOR_EXIT;
			return;
		}

		LoadDefaults();
		LoadTabs();

		mpEditorInput->Init();
		mpEditorEventSys->Init();
		InstallHotkeys();
		for (auto& e : mArrTabs)
		{
			e->SetComdContext(mpComdHandler);
			e->SetEventContext(mpEditorEventSys);
			e->SetSceneContext(&(mpSceneSystem->GetCurrentScene()));
			e->Init();
		}
		LoadSettings();
		mpDriver->Init();
		mpDriver->PostInit();

		EGUI::SetContext(mpComdHandler);
	}

	void Editor::StartFrame()
	{
		mDeltaTime = mpTimer->Elapsed();
		mpTimer->Lap();
		if (mpWin->GetMainWindow().GetWindowHandle() == GetActiveWindow())
			mpEditorInput->Poll();
		switch (mCurrentState)
		{
		case EDITOR_PROMPT_SAVE:
		case EDITOR_MAIN:
			mpProfiler->Update(mDeltaTime);
			mpWin->Update(mDeltaTime);
			mpBehaviourSys->PollChanges();
			UpdateKeys();
			UpdateHotkeys();
			break;
		case EDITOR_PLAY:
			UpdateKeys();
			UpdateGameModeKeys();
			break;
		}
		mpEditorEventSys->FireAllPending();
		mpGuiSystem->StartFrame(mDeltaTime);
		MainMenuBar();
	}

	void Editor::UpdateFrame(const float& _dt)
	{ 
		switch (mCurrentState)
		{
		case EDITOR_PLAY:
			mpDriver->FixedUpdate();
			mpDriver->Update();
			break;
		case EDITOR_PROMPT_SAVE:
			PromptSaving();
			break;
		}

		bool nightmode = EDITOR_PLAY == mCurrentState;
		
		if (nightmode) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);

		for (unsigned int i = 0; i < mArrTabs.size(); ++i)
		{
			EGUI::PushID(i);
			EditorTab *pTab = mArrTabs[i];
			pTab->SetSize(EGUI::Docking::GetTabSize(pTab->GetLabel().c_str()) - Math::Vec2{ 8, 4 });
			pTab->SetPosition(EGUI::Docking::GetTabPosition(pTab->GetLabel().c_str()));
			pTab->SetSceneContext(&(mpSceneSystem->GetCurrentScene()));
			{
				ScopedTimer<ProfilerAction> scopeT{ pTab->GetLabel(), "Update" };
				pTab->Update(_dt);
			}
			{
				ScopedTimer<ProfilerAction> scopeT{ pTab->GetLabel(), "Editor UI" };
				ImGuiWindowFlags f = pTab->GetVertClip() ? ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar : ImGuiWindowFlags_None;
				if (EGUI::StartTab(pTab->GetLabel().c_str(), pTab->GetOpenedBool(), f))
				{
					EGUI::Indent(4);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
					pTab->EditorUI();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
					EGUI::UnIndent(4);
				}
			}
			EGUI::EndTab();
			EGUI::PopID();
		}
		if (nightmode) ImGui::PopStyleVar();
	}

	void Editor::EndFrame()
	{
		mpEditorInput->Clear();
		if (mUpdateSelection)
		{
			UpdateSelections();
			mUpdateSelection = false;
		}
		LogTabPerformance();
		mSceneHasChanged = (mSceneHasChanged | (&mpSceneSystem->GetCurrentScene() != &mpSceneSystem->GetActiveScene()));
		EngineCore::GetInstance()->PostUpdate();
		mpBehaviourSys->PostUpdate();
		if (mSceneHasChanged)
		{
			for (auto& e : mArrTabs)
				e->SetSceneContext(&mpSceneSystem->GetActiveScene());
			mpEditorEventSys->Fire(EDITOR_SCENE_CHANGED);
			auto name = mpSceneSystem->GetActiveScene().GetSceneName();
			mpWin->GetMainWindow().SetTitle(std::wstring{ name.begin(), name.end() });
			mSceneHasChanged = false;
		}
		mpGuiSystem->EndFrame(); 
		if (mCurrentState != mNextState)  UpdateState();
	}

	void Editor::Shutdown()
	{
		UnInstallHotkeys();
		mpDriver->GetSubSystem<LoggerSystem>()->RedirectOutput(nullptr);
		auto serial = TextSerialiser::OpenFile(DYSTOPIA_EDITOR_SETTINGS, TextSerialiser::MODE_WRITE);
		for (auto& e : mArrTabs)
		{
			e->SaveSettings(serial);
			e->Shutdown();
			delete e;
		}
		mpEditorEventSys->Shutdown();
		mpComdHandler->Shutdown();
		mpGuiSystem->Shutdown();
		mpEditorInput->Shutdown();
		EGUI::Docking::ShutdownTabs();
		mpClipBoard->Close();
		delete mpEditorEventSys;
		delete mpComdHandler;
		delete mpGuiSystem;
		delete mpTimer;
		delete mpClipBoard;
		delete mpEditorInput;
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
			if (mCurrentState == EDITOR_PLAY ||
				mCurrentState == EDITOR_PAUSE)
			{
				TempLoad();
				//mpBehaviourSys->ReAttach();
			}
			break;
		case EDITOR_PLAY:
			TempSave();
			mpSceneSystem->GetActiveScene().Init();
			break;
		case EDITOR_PAUSE:
			break;
		case EDITOR_EXIT:
			if (mTempSaveFile.length()) remove(mTempSaveFile.c_str());
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
			MMGame();

			EGUI::EndMainMenuBar();
		}
	}

	void Editor::MMFile()
	{
		if (EGUI::StartMenuHeader("File", mCurrentState == EDITOR_MAIN))
		{
			if (EGUI::StartMenuBody("New", "Ctrl + N"))					mpEditorEventSys->Fire(EDITOR_HOTKEY_NEW); 
			if (EGUI::StartMenuBody("Open", "Ctrl + O"))				mpEditorEventSys->Fire(EDITOR_HOTKEY_OPEN);
			if (EGUI::StartMenuBody("Save", "Ctrl + S"))				mpEditorEventSys->Fire(EDITOR_HOTKEY_SAVE);
			if (EGUI::StartMenuBody("Save As..", "Ctrl + Shift + S"))	mpEditorEventSys->Fire(EDITOR_HOTKEY_SAVEAS);
			if (EGUI::StartMenuBody("Quit"))							ChangeState((mpComdHandler->HasUnsavedChanges()) ? 
																					 EDITOR_PROMPT_SAVE : EDITOR_EXIT);
			EGUI::EndMenuHeader();
		}
	}

	void Editor::MMEdit()
	{
		if (EGUI::StartMenuHeader("Edit", mCurrentState == EDITOR_MAIN))
		{
			if (EGUI::StartMenuBody("Undo ", "Ctrl + Z"))	mpEditorEventSys->Fire(EDITOR_HOTKEY_UNDO);
			if (EGUI::StartMenuBody("Redo ", "Ctrl + Y"))	mpEditorEventSys->Fire(EDITOR_HOTKEY_REDO);
			if (EGUI::StartMenuBody("Cut ", "Ctrl + X"))	mpEditorEventSys->Fire(EDITOR_HOTKEY_CUT);
			if (EGUI::StartMenuBody("Copy ", "Ctrl + C"))	mpEditorEventSys->Fire(EDITOR_HOTKEY_COPY);
			if (EGUI::StartMenuBody("Paste ", "Ctrl + V"))	mpEditorEventSys->Fire(EDITOR_HOTKEY_PASTE);
			EGUI::EndMenuHeader();
		}
	}

	void Editor::MMView()
	{
		static constexpr float icon = 10.f;
		if (EGUI::StartMenuHeader("View"))
		{
			for (auto& e : mArrTabs)
			{
				if (*(e->GetOpenedBool()))  EGUI::Display::IconTick(icon, icon);
				else						EGUI::Display::Dummy(icon, icon);
				EGUI::SameLine();
				if (EGUI::StartMenuBody(e->GetLabel()))
					*(e->GetOpenedBool()) = !*(e->GetOpenedBool());
			}
			EGUI::EndMenuHeader();
		}
	}

	void Editor::MMGame()
	{
		if (EGUI::StartMenuHeader("Game"))
		{
			if (EGUI::StartMenuBody("Play", "Ctrl + P", mCurrentState == EDITOR_MAIN))
				GamePlay();
			if (EGUI::StartMenuBody("Stop", "Ctrl + P", mCurrentState == EDITOR_PLAY))
				GameStop();
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
		mpClipBoard->ClearData();
		for (const auto& o : mArrSelectedObj)
			mpClipBoard->InsertData(eCLIP_GAME_OBJECT, reinterpret_cast<void*>(o), sizeof(GameObject));
	}

	void Editor::EditorPaste()
	{
		auto toPaste = mpClipBoard->RetrieveDatas(eCLIP_GAME_OBJECT, sizeof(GameObject));
		if (!toPaste.size()) return;

		AutoArray<GameObject*> mToInsert{ toPaste.size() };
		auto& existingObj = mpSceneSystem->GetCurrentScene().GetAllGameObjects();
		ClearSelections();
		for (auto& elem : toPaste)
		{
			if (mpSceneSystem->GetCurrentScene().FindGameObject(static_cast<GameObject*>(elem)->GetID()))
			{
				GameObject *pDup = static_cast<GameObject*>(elem)->Duplicate();
				for (const auto& o : existingObj)
				{
					if (o.GetName() == pDup->GetName())
					{
						pDup->SetName(pDup->GetName() + "_Clone");
						break;
					}
				}
				pDup->Identify();
				mToInsert.Insert(pDup);
				AddSelection(pDup->GetID());
			}
		}

		if (mToInsert.size())
			mpComdHandler->InvokeCommandInsert(mToInsert, mpSceneSystem->GetCurrentScene());
	}

	void Editor::EditorDelete()
	{
		auto& allObj = GetSelectionObjects();
		if (!allObj.size()) return;

		mpComdHandler->InvokeCommandDelete(allObj, mpSceneSystem->GetCurrentScene());
		ClearSelections();
	}

	void Editor::NewScene()
	{

	}

	void Editor::GamePlay()
	{
		ChangeState(EDITOR_PLAY);
		mpDriver->InterruptContinue();
	}

	void Editor::GameStop()
	{
		ChangeState(EDITOR_MAIN);
	}

	void Editor::SaveProc()
	{
		std::string sceneName = mpSceneSystem->GetCurrentScene().GetSceneName();
		if (sceneName != "Untitled")
		{
			std::string ext{ DYSTOPIA_SCENE_EXTENSION.begin(), DYSTOPIA_SCENE_EXTENSION.end() };
			std::string file{ DYSTOPIA_SCENE_LOAD + sceneName + "." + ext };
			mpSceneSystem->SaveScene(file, sceneName);
			mpComdHandler->SaveCallback();
			std::wstring name{ sceneName.begin(), sceneName.end() };
			auto pos = name.find('*.');
			if (pos != std::string::npos)
				name.erase(pos);
			else
			{
				pos = name.find('.');
				if (pos != std::string::npos)
					name.erase(pos);
			}
			mpWin->GetMainWindow().SetTitle(name.c_str());
		}
		else SaveAsProc();
	}

	void Editor::SaveAsProc()
	{
		EditorProc p;
		HashString path;
		HashString name;
		if (p.SaveAs(name, path, mpWin->GetMainWindow().GetWindowHandle()))
		{
			mpWin->GetMainWindow().SetTitle(std::wstring{name.begin(), name.end()});
			mpSceneSystem->SaveScene(std::string{ path.begin(), path.end() },
				std::string{ name.begin(), name.end() });
			mpComdHandler->SaveCallback();
		}
	}

	void Editor::LoadProc()
	{
		EditorProc p;
		HashString path;
		HashString name;
		if (p.Load(path))
		{
			mArrSelectedObj.clear();
			ClearSelections();
			mpSceneSystem->LoadScene(std::string{ path.begin(), path.end() });
		}
	}

	void Editor::OpenScene(const std::wstring& _path, const std::wstring& /*_name*/)
	{
		mArrSelectedObj.clear();
		ClearSelections();
		mpSceneSystem->LoadScene(std::string{ _path.begin(), _path.end() });
	}

	void Editor::TempSave()
	{
		ClearSelections();
		std::string ext{ DYSTOPIA_SCENE_EXTENSION.begin(), DYSTOPIA_SCENE_EXTENSION.end() };
		std::string file{ DYSTOPIA_SCENE_TEMP + std::to_string(GUIDGenerator::GetUniqueID()) + "." + ext };
		mpSceneSystem->SaveScene(file, mpSceneSystem->GetCurrentScene().GetSceneName());
		mTempSaveFile = file;
	}

	void Editor::TempLoad()
	{
		mSceneHasChanged = true;
		ClearSelections();
		mpSceneSystem->RestartScene();
		remove(mTempSaveFile.c_str());
		mTempSaveFile.clear();
		for (auto& e : mArrTabs)
			e->SetSceneContext(&mpSceneSystem->GetActiveScene());
		mpEditorEventSys->FireNow(EDITOR_SCENE_CHANGED);
	}

	void Editor::UpdateKeys()
	{
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

		bool ctrl = mpEditorInput->Get()->IsKeyPressed(eButton::KEYBOARD_LCTRL);
		bool shft = mpEditorInput->Get()->IsKeyPressed(eButton::KEYBOARD_LSHIFT);
		bool alt = mpEditorInput->Get()->IsKeyPressed(eButton::KEYBOARD_LALT);
		mpGuiSystem->UpdateKey(eButton::KEYBOARD_SHIFT, shft);
		mpGuiSystem->UpdateKey(eButton::KEYBOARD_ALT, alt);
		mpGuiSystem->UpdateKey(eButton::KEYBOARD_CTRL, ctrl);

		if (ctrl)
			return;

		const auto& queue = mpWin->GetMainWindow().GetInputQueue();
		for (const auto& k : queue)
		{
			if (k >= eButton::KEYBOARD_0 && k <= eButton::KEYBOARD_9)
				mpGuiSystem->UpdateChar(k);
			else if (k >= eButton::KEYBOARD_A && k <= eButton::KEYBOARD_Z)
				mpGuiSystem->UpdateChar(shft ? k : k + 32);
			else if (k >= eButton::KEYBOARD_NUMPAD_0 && k <= eButton::KEYBOARD_NUMPAD_9)
				mpGuiSystem->UpdateChar(k - 48);
			else if (k == eButton::KEYBOARD_OEM_PERIOD)
				mpGuiSystem->UpdateChar(46);
			else if (k == eButton::KEYBOARD_OEM_MINUS)
				mpGuiSystem->UpdateChar(shft ? 95 : 45);
			else if (k == eButton::KEYBOARD_SPACEBAR)
				mpGuiSystem->UpdateChar(32);
			else
				mpGuiSystem->UpdateKey(k, true);
		}
		float scrollV = mpEditorInput->Get()->GetMouseWheel();
		if (scrollV)
		{
			mpGuiSystem->UpdateScroll(0, scrollV);
			mpEditorEventSys->Fire(scrollV > 0 ? EDITOR_SCROLL_UP : EDITOR_SCROLL_DOWN);
		}
	}

	void Editor::UpdateHotkeys()
	{
		if (mpEditorInput->Get()->IsKeyTriggered(eButton::MOUSE_LEFT))
			mpEditorEventSys->Fire(EDITOR_LCLICK);

		if (mpEditorInput->Hotkeys(Hotkey::g_KeyDelete, Hotkey::g_StateDelete, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_DELETE);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeyOpen, Hotkey::g_StateOpen, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_OPEN);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeyUndo, Hotkey::g_StateUndo, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_UNDO);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeyRedo, Hotkey::g_StateRedo, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_REDO);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeyCopy, Hotkey::g_StateCopy, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_COPY);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeyPaste, Hotkey::g_StatePaste, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_PASTE);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeyPlayStop, Hotkey::g_StatePlayStop, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_PLAY);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeySaveAs, Hotkey::g_StateSaveAs, g_CountSaveAs))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_SAVEAS);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeySave, Hotkey::g_StateSave, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_SAVE);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeyGizmoTrans, Hotkey::g_StateGizmoTrans, g_CountGizmo))
			mpEditorEventSys->Fire(EDITOR_GIZMO_TRANSLATE);

		else if (mpEditorInput->Hotkeys(Hotkey::g_KeyGizmoScale, Hotkey::g_StateGizmoScale, g_CountGizmo))
			mpEditorEventSys->Fire(EDITOR_GIZMO_SCALE);
	}

	void Editor::UpdateGameModeKeys()
	{
		if (mpEditorInput->Hotkeys(Hotkey::g_KeyPlayStop, Hotkey::g_StatePlayStop, g_CountDefault))
			mpEditorEventSys->Fire(EDITOR_HOTKEY_STOP);
	}
	
	void Editor::InstallHotkeys()
	{
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_OPEN)->Bind(&Editor::LoadProc, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_NEW)->Bind(&Editor::NewScene, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_UNDO)->Bind(&Editor::EditorUndo, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_REDO)->Bind(&Editor::EditorRedo, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_COPY)->Bind(&Editor::EditorCopy, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_PASTE)->Bind(&Editor::EditorPaste, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_DELETE)->Bind(&Editor::EditorDelete, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_SAVE)->Bind(&Editor::SaveProc, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_SAVEAS)->Bind(&Editor::SaveAsProc, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_PLAY)->Bind(&Editor::GamePlay, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_STOP)->Bind(&Editor::GameStop, this);
	}

	void Editor::UnInstallHotkeys()
	{
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_OPEN)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_NEW)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_UNDO)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_REDO)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_COPY)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_PASTE)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_DELETE)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_SAVE)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_SAVEAS)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_PLAY)->Unbind(this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_STOP)->Unbind(this);
	}

	GameObject* Editor::FindGameObject(const uint64_t& _id) const
	{
		return mpSceneSystem->GetCurrentScene().FindGameObject(_id);
	}

	void Editor::LogTabPerformance()
	{
		static constexpr float intervalS = 0.1f;
		static float deltaAccu = 0.f;
		deltaAccu += GetDeltaTime();
		if (deltaAccu > intervalS)
		{
			deltaAccu = 0;
			auto data = mpProfiler->GetInfo();
			for (const auto& d : data)
			{
				auto info = d.second.mTimes;
				for (const auto& i : info)
				{
					Performance::LogDataS(d.first, i.first, static_cast<float>(info[i.first]));
					Performance::LogDataG(d.first, static_cast<float>(d.second.mTotal));
				}
			}
			PLogTaskManager p;
			p.mFrameRate	= mDeltaTime;
			p.mCPUIdle		= mpProfiler->GetCPUPercentageIdle();
			p.mCPUBusy		= mpProfiler->GetCPUPercentageBusy();
			p.mCPUOS		= mpProfiler->GetCPUPercentageOS();
			p.mCPUProc		= mpProfiler->GetCPUPercentageProcess();
			p.mPageFaults	= mpProfiler->GetNumPageFaults();
			p.mMemUsed		= mpProfiler->GetUsedMemory();
			p.mRamUsed		= mpProfiler->GetUsedPhysicalMemory();
			p.mMemAvail		= mpProfiler->GetAvailablePhysicalMemory();
			p.mMemLoad		= mpProfiler->GetSystemMemoryLoad();
			Performance::LogTaskMgr(p);
		}
	}

	void Editor::ReAttachComponent(Component* _pComponent)
	{
		auto& undo = mpComdHandler->GetDeqUndo();
		auto& redo = mpComdHandler->GetDeqRedo();

		GameObject* pObj = nullptr;
		for (auto& e : undo)
		{
			pObj = e->RetrieveGameObject();
			if (pObj && (pObj->GetID() == _pComponent->GetOwnerID()))
			{
				for (auto & c : pObj->GetAllComponents())
				{
					if (c->GetID() == _pComponent->GetID())
					{
						c = _pComponent;
						pObj->Identify();
						pObj->Init();
						return;
					}
				}
			}
		}
		for (auto& e : redo)
		{
			pObj = e->RetrieveGameObject();
			if (pObj && (pObj->GetID() == _pComponent->GetOwnerID()))
			{
				for (auto & c : pObj->GetAllComponents())
				{
					if (c->GetID() == _pComponent->GetID())
					{
						c = _pComponent;
						pObj->Identify();
						pObj->Init();
						return;
					}
				}
			}
		}
		_pComponent->DestroyComponent();
	}

	void Editor::PromptSaving()
	{
		EGUI::Display::OpenPopup(g_nsPopup);
		if (EGUI::Display::StartPopupModal(g_nsPopup.c_str(), "Save before Exit?"))
		{
			if (EGUI::Display::Button("Save"))
			{
				EGUI::Display::CloseCurrentPopup();
				ChangeState(EDITOR_MAIN);
				mpEditorEventSys->Fire(EDITOR_HOTKEY_SAVE);
			}
			EGUI::SameLine();
			if (EGUI::Display::Button("Cancel"))
			{
				EGUI::Display::CloseCurrentPopup();
				ChangeState(EDITOR_MAIN);
			}
			EGUI::SameLine();
			if (EGUI::Display::Button("Exit"))
			{
				EGUI::Display::CloseCurrentPopup();
				ChangeState(EDITOR_EXIT);
			}
			EGUI::Display::EndPopup();
		}
	}

	const AutoArray<GameObject*>& Editor::GetSelectionObjects(void)
	{
		UpdateSelections();
		return mArrSelectedObj;
	}

	void Editor::AddSelection(const uint64_t& _id)
	{
		mUpdateSelection = true;
		if (mpEditorInput->Get()->IsKeyPressed(eButton::KEYBOARD_LCTRL))
			mpClipBoard->InsertSelection(_id);
		else
			NewSelection(_id);
	}

	void Editor::NewSelection(const uint64_t& _id)
	{
		mUpdateSelection = true;
		ClearSelections();
		mpClipBoard->InsertSelection(_id);
	}

	void Editor::RemoveSelection(const uint64_t _id)
	{
		mUpdateSelection = true;
		mpClipBoard->RemoveSelection(_id);
	}

	void Editor::ClearSelections(void)
	{
		mUpdateSelection = true;
		mpClipBoard->ClearSelection();
	}

	void Editor::UpdateSelections(void)
	{
		GameObject* temp = nullptr;
		mArrSelectedObj.clear();
		auto& selections = mpClipBoard->RetrieveSelections();
		for (auto& id : selections)
		{
			temp = mpSceneSystem->GetActiveScene().FindGameObject(id);
			if (temp)
				mArrSelectedObj.Insert(temp);
		}
	}

	Clipboard& Editor::GetClipboard(void)
	{
		return *mpClipBoard;
	}
}

#endif		// EDITOR ONLY
