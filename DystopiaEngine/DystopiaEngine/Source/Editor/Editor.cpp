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
#include "IO/BinarySerializer.h"
#include "Utility/GUID.h"
#include "System/File/FileSystem.h"
#include "System//Behaviour/BehaviourSystem.h"
#include "System/Physics/PhysicsSystem.h"

#include "Component/Component.h"

/* Editor includes */
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#include "Editor/ProjectSettings.h"
#include "Editor/EditorInputs.h"
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

#include "Allocator/DefaultAlloc.h"

/* library includes */
#include <iostream>
#include <windows.h>
#include <bitset>
#include <ShlObj_core.h>
#include <tchar.h>
#include <objbase.h>

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
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
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
		mpSceneSystem{ nullptr }, mpProfiler{ nullptr }, mTempSaveFile{},
		mpEditorEventSys{ new EditorEventHandler{} }, mpInput{ new EditorInput{} },
		mpComdHandler{ new CommandHandler{} }, mpGuiSystem{ new GuiSystem{} }, mpTimer{ new Timer{} },
		mpClipBoard{ new Clipboard{} }, mCtrlKey{ false }, mArrSelectedObj{ 100 }, mUpdateSelection{ true }
	{}

	Editor::~Editor(void)
	{
	}

	void Editor::LoadTabs()
	{
		mArrTabs.push_back(ProjectSettings::GetInstance());
		mArrTabs.push_back(ConsoleLog::GetInstance());
		mArrTabs.push_back(PerformanceLog::GetInstance());
		mArrTabs.push_back(ColorScheme::GetInstance());
		mArrTabs.push_back(StyleScheme::GetInstance());
		mArrTabs.push_back(Inspector::GetInstance());
		mArrTabs.push_back(ProjectResource::GetInstance());
		mArrTabs.push_back(HierarchyView::GetInstance());
		mArrTabs.push_back(SceneView::GetInstance());
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
		mpDriver->Init();

		mpWin			= mpDriver->GetSystem<WindowManager>();		// driver init-ed
		mpGfx			= mpDriver->GetSystem<GraphicsSystem>();	// driver init-ed
		mpSceneSystem	= mpDriver->GetSystem<SceneSystem>();		// driver init-ed
		mpProfiler		= mpDriver->GetSystem<Profiler>();			// driver init-ed
		mpBehaviourSys	= mpDriver->GetSystem<BehaviourSystem>();	// driver init-ed

		if (!mpGuiSystem->Init(mpWin, mpGfx, mpInput))
		{
			mCurrentState = EDITOR_EXIT;
			return;
		}
		
		LoadDefaults();
		LoadTabs();
		mpInput->Init();
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
		EGUI::SetContext(mpComdHandler);
	}

	void Editor::StartFrame()
	{
		mCtrlKey = false;
		mDeltaTime = mpTimer->Elapsed();
		mpTimer->Lap();
		if (mpWin->GetMainWindow().GetWindowHandle() == GetActiveWindow())
		{
			mpInput->Update(mDeltaTime);
		}
		switch (mCurrentState)
		{
		case EDITOR_MAIN:
			mpProfiler->Update(mDeltaTime);
			mpWin->Update(mDeltaTime);
			mpBehaviourSys->PollChanges();
			UpdateKeys();
			UpdateHotkeys();
			break;
		case EDITOR_PLAY:
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
				if (EGUI::StartTab(pTab->GetLabel().c_str(), pTab->GetOpenedBool()))
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
		if (mUpdateSelection)
		{
			UpdateSelections();
			mUpdateSelection = false;
		}

		LogTabPerformance();
		EngineCore::GetInstance()->PostUpdate();
		mpBehaviourSys->PostUpdate();
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
		mpInput->Shutdown();
		mpEditorEventSys->Shutdown();
		mpComdHandler->Shutdown();
		mpGuiSystem->Shutdown();
		EGUI::Docking::ShutdownTabs();
		mpClipBoard->Close();
		delete mpInput;
		delete mpEditorEventSys;
		delete mpComdHandler;
		delete mpGuiSystem;
		delete mpTimer;
		delete mpClipBoard;
		mpEditorEventSys	= nullptr;
		mpInput				= nullptr;
		mpComdHandler		= nullptr;
		mpGuiSystem			= nullptr;
		mpTimer				= nullptr;
		mpWin				= nullptr;
		mpGfx				= nullptr;
		mpProfiler			= nullptr;
		mpBehaviourSys		= nullptr;
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
				TempLoad();
			break;
		case EDITOR_PLAY:
			TempSave();
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
			MMDebug();
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

	void Editor::MMDebug()
	{
		static constexpr float icon = 10.f;
		static GraphicsSystem *pGraphic = EngineCore::GetInstance()->GetSystem<GraphicsSystem>();
		if (EGUI::StartMenuHeader("Debug"))
		{

				if (pGraphic->GetDebugDraw())	EGUI::Display::IconTick(icon, icon);
				else							EGUI::Display::Dummy(icon, icon);
				EGUI::SameLine();
				if (EGUI::StartMenuBody("Draw Debug Collision"))
					pGraphic->ToggleDebugDraw();
			
			EGUI::EndMenuHeader();
		}
	}

	void Editor::MMGame()
	{
		if (EGUI::StartMenuHeader("Game"))
		{
			if (EGUI::StartMenuBody("Play", "Ctrl + P", mCurrentState == EDITOR_MAIN))
			{
				GamePlay();
			}
			if (EGUI::StartMenuBody("Stop", "Ctrl + P", mCurrentState == EDITOR_PLAY))
			{
				GameStop();
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
		mpClipBoard->ClearData();
		for (const auto& o : mArrSelectedObj)
		{
			mpClipBoard->InsertData(eCLIP_GAME_OBJECT, reinterpret_cast<void*>(o), sizeof(GameObject));
		}
	}

	void Editor::EditorCut()
	{
	}

	void Editor::EditorPaste()
	{
		auto toPaste = mpClipBoard->RetrieveDatas(eCLIP_GAME_OBJECT, sizeof(GameObject));
		if (!toPaste.size()) return;

		AutoArray<GameObject*> mToInsert{ toPaste.size() };
		auto& existingObj = mpSceneSystem->GetCurrentScene().GetAllGameObjects();
		int replicas = 0;
		ClearSelections();
		for (auto& elem : toPaste)
		{
			GameObject *pDup = static_cast<GameObject*>(elem)->Duplicate();

			for (const auto& o : existingObj)
				if (o.GetName() == pDup->GetName())
					replicas++;

			if (replicas > 0)
				pDup->SetName(pDup->GetName() + "_Clone");

			mToInsert.Insert(pDup);
			AddSelection(pDup->GetID());
			replicas = 0;
		}
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
			auto pos = name.find('.');
			if (pos != std::string::npos)
			{
				name.erase(pos);
				mpWin->GetMainWindow().SetTitle(name.c_str());
			}
		}
		else SaveAsProc();
	}

	void Editor::SaveAsProc()
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileSaveDialog *pFileSave;
			hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
				IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
			if (SUCCEEDED(hr))
			{
				pFileSave->SetDefaultExtension(DYSTOPIA_SCENE_EXTENSION.c_str());
				pFileSave->SetFileTypes(1, DYSTOPIA_SCENE_FILTER_EXTENSION);
				if (SUCCEEDED(pFileSave->Show(mpWin->GetMainWindow().GetWindowHandle())))
				{
					IShellItem *pItem;
					if (SUCCEEDED(pFileSave->GetResult(&pItem)))
					{
						PWSTR pszFilePath, pszFileName;
						if (SUCCEEDED(pItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath)) &&
							SUCCEEDED(pItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszFileName)))
						{
							std::wstring path{ pszFilePath };
							std::wstring name{ pszFileName };
							auto pos = name.find('.');
							if (pos != std::string::npos)
							{
								name.erase(pos);
								mpWin->GetMainWindow().SetTitle(name.c_str());
							}
							mpSceneSystem->SaveScene(std::string{ path.begin(), path.end() }, 
													 std::string{ name.begin(), name.end() });
							mpComdHandler->SaveCallback();
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileSave->Release();
			}
			CoUninitialize();
		}
	}

	void Editor::LoadProc()
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog *pFileOpen;
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
			if (SUCCEEDED(hr))
			{
				pFileOpen->SetFileTypes(1, DYSTOPIA_SCENE_FILTER_EXTENSION);
				if (SUCCEEDED(pFileOpen->Show(NULL)))
				{
					IShellItem *pItem;					
					if (SUCCEEDED(pFileOpen->GetResult(&pItem)))
					{
						PWSTR pszFilePath, pszFileName;
						hr = pItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath);
						if (SUCCEEDED(hr) && 
							SUCCEEDED(pItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszFileName)))
						{
							std::wstring path{ pszFilePath };
							std::wstring name{ pszFileName };
							auto pos = name.find('.');
							if (pos != std::string::npos)
							{
								name.erase(pos);
							}
							OpenScene(path, name);
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
	}

	void Editor::OpenScene(const std::wstring& _path, const std::wstring& _name)
	{
		mpSceneSystem->LoadScene(std::string{ _path.begin(), _path.end() });
		for (auto& e : mArrTabs)
			e->SetSceneContext(&mpSceneSystem->GetCurrentScene());
		mpEditorEventSys->FireNow(EDITOR_SCENE_CHANGED);
		ClearSelections();
		mpWin->GetMainWindow().SetTitle(_name);
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
		ClearSelections();
		mpSceneSystem->RestartScene();
		remove(mTempSaveFile.c_str());
		mTempSaveFile.clear();
		for (auto& e : mArrTabs)
			e->SetSceneContext(&mpSceneSystem->GetCurrentScene());
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
		bool caps = mpInput->IsKeyPressed(KEY_SHIFT);
		const auto& queue = mpWin->GetMainWindow().GetInputQueue();

		mCtrlKey = mpInput->IsKeyPressed(KEY_CTRL);
		if (!mCtrlKey)
		{
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
					mpGuiSystem->UpdateChar(k - 48);
				// arithmetics
				//else if (k >= eButton::KEYBOARD_OEM_1 && k <= eButton::KEYBOARD_OEM_PERIOD)
				//	mpGuiSystem->UpdateChar(k);
				else if (k == eButton::KEYBOARD_OEM_PERIOD)
					mpGuiSystem->UpdateChar(46);
				else if (k == eButton::KEYBOARD_OEM_MINUS)
					mpGuiSystem->UpdateChar(caps ? 95 : 45);
				else if (k == eButton::KEYBOARD_SPACEBAR)
					mpGuiSystem->UpdateChar(32);
				// misc keys like ctrl, del, back etc
				else
					mpGuiSystem->UpdateKey(k, true);
			}
		}
	}

	void Editor::UpdateHotkeys()
	{
		if (mpInput->IsKeyTriggered(KEY_LMOUSE))
		{
			mpGuiSystem->UpdateMouse(KEY_LMOUSE, true);
			mpEditorEventSys->Fire(EDITOR_LCLICK);
		}
		if (mpInput->IsKeyTriggered(KEY_RMOUSE))
		{
			mpGuiSystem->UpdateMouse(KEY_RMOUSE, true);
			mpEditorEventSys->Fire(EDITOR_RCLICK);
		}
		float scrollV = mpInput->GetMouseWheel();
		if (scrollV)
		{
			mpGuiSystem->UpdateScroll(0, scrollV);
			mpEditorEventSys->Fire(scrollV > 0 ? EDITOR_SCROLL_UP : EDITOR_SCROLL_DOWN);
		}
		if (mCtrlKey)
		{
			if (mpInput->IsKeyTriggered(KEY_Z))			mpEditorEventSys->Fire(EDITOR_HOTKEY_UNDO);
			else if (mpInput->IsKeyTriggered(KEY_Y))	mpEditorEventSys->Fire(EDITOR_HOTKEY_REDO);
			else if (mpInput->IsKeyTriggered(KEY_C))	mpEditorEventSys->Fire(EDITOR_HOTKEY_COPY);
			else if (mpInput->IsKeyTriggered(KEY_X))	mpEditorEventSys->Fire(EDITOR_HOTKEY_CUT);
			else if (mpInput->IsKeyTriggered(KEY_V))	mpEditorEventSys->Fire(EDITOR_HOTKEY_PASTE);
			else if (mpInput->IsKeyTriggered(KEY_S))	mpEditorEventSys->Fire(EDITOR_HOTKEY_SAVE);
			else if (mpInput->IsKeyPressed(KEY_SHIFT))
			{
				if (mpInput->IsKeyTriggered(KEY_S))		mpEditorEventSys->Fire(EDITOR_HOTKEY_SAVEAS);
			}
			else if (mpInput->IsKeyTriggered(KEY_P))	mpEditorEventSys->Fire(EDITOR_HOTKEY_PLAY);
			else if (mpInput->IsKeyTriggered(KEY_O))	mpEditorEventSys->Fire(EDITOR_HOTKEY_OPEN);
			else if (mpInput->IsKeyTriggered(KEY_N))	mpEditorEventSys->Fire(EDITOR_HOTKEY_NEW);
		}
		else if (mpInput->IsKeyTriggered(KEY_DELETE))	
			mpEditorEventSys->Fire(EDITOR_HOTKEY_DELETE);
	}

	void Editor::UpdateGameModeKeys()
	{
		if (mpInput->IsKeyPressed(KEY_CTRL) && 
			mpInput->IsKeyTriggered(KEY_P))	
			mpEditorEventSys->Fire(EDITOR_HOTKEY_STOP);
	}
	
	void Editor::InstallHotkeys()
	{
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_OPEN)->Bind(&Editor::LoadProc, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_NEW)->Bind(&Editor::NewScene, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_UNDO)->Bind(&Editor::EditorUndo, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_REDO)->Bind(&Editor::EditorRedo, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_COPY)->Bind(&Editor::EditorCopy, this);
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_CUT)->Bind(&Editor::EditorCut, this);
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
		mpEditorEventSys->GetEvent(EDITOR_HOTKEY_CUT)->Unbind(this);
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
		return mArrSelectedObj;
	}

	void Editor::AddSelection(const uint64_t& _id)
	{
		mUpdateSelection = true;
		mpClipBoard->InsertSelection(_id);
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
			{
				mArrSelectedObj.Insert(temp);
			}
		}
	}

	bool Editor::IsCtrlDown(void) const
	{
		return mCtrlKey;
	}
}

#endif		// EDITOR ONLY
