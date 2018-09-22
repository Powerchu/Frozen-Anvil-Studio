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
#include "System\Profiler\ProfilerAction.h"
#include "System\Time\ScopedTimer.h"
#include "IO\BinarySerializer.h"
#include "Utility\GUID.h"
#include "System/File/FileSystem.h"
#include "System//Behaviour/BehaviourSystem.h"
#include <System/Physics/PhysicsSystem.h>
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
#include "Editor\PLogger.h"

/* library includes */
#include <iostream>
#include <windows.h>
#include <bitset>
#include <ShlObj_core.h>
#include <tchar.h>
#include <objbase.h>

static const std::string DYSTOPIA_SCENE_LOAD = "Resource/Scene/";
static const std::wstring DYSTOPIA_SCENE_EXTENSION = L"dscene";
static constexpr COMDLG_FILTERSPEC DYSTOPIA_SCENE_FILTER_EXTENSION[1] =
{
	{ L"DystopiaScene", L"*.dscene" }
};

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
		mpPhysicsSystem{nullptr},
		mpProfiler{ nullptr },
		mpEditorEventSys{ new EditorEventHandler{} },
		mpInput{ new EditorInput{} },
		mpComdHandler{ new CommandHandler{} },
		mpGuiSystem{ new GuiSystem{} },
		mpTimer{ new Timer{} },
		mpFocusGameObj{ nullptr }

	{}

	Editor::~Editor(void)
	{
	}

	void Editor::LoadDefaults()
	{
		mArrTabs.push_back(Inspector::GetInstance());
		mArrTabs.push_back(ProjectResource::GetInstance());
		mArrTabs.push_back(HierarchyView::GetInstance());
		mArrTabs.push_back(SceneView::GetInstance());
		mArrTabs.push_back(ConsoleLog::GetInstance());
		mArrTabs.push_back(PerformanceLog::GetInstance());
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
		mpPhysicsSystem = mpDriver->GetSystem<PhysicsSystem>();		// driver init-ed
		mpBehaviourSys	= mpDriver->GetSystem<BehaviourSystem>();	// driver init-ed

		LoadDefaults();
		mpInput->Init();
		mpEditorEventSys->Init();
		InstallHotkeys();
		for (auto& e : mArrTabs)
		{
			e->SetComdContext(mpComdHandler);
			e->SetEventContext(mpEditorEventSys);
			e->SetSceneContext(&(mpSceneSystem->GetCurrentScene()));
			e->Init();
			e->RemoveFocus();
		}
		EGUI::SetContext(mpComdHandler);
		if (!mpGuiSystem->Init(mpWin, mpGfx, mpInput))
			mCurrentState = EDITOR_EXIT;
		PrintToConsoleLog("Ho");
	}

	void Editor::StartFrame()
	{
		mDeltaTime = mpTimer->Elapsed();
		mpTimer->Lap();
		mpProfiler->Update(mDeltaTime);
		mpInput->Update(mDeltaTime);
		mpWin->Update(mDeltaTime);
		mpBehaviourSys->Update(mDeltaTime);

		if (mpBehaviourSys->hasDllChanges())
		{
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_DLL_CHANGED);
		}


		/*This is for testing if Behvaiour Hotreloading works*/

		/*
				auto & arr = mpSceneSystem->GetCurrentScene().GetAllGameObjects();
		for (auto & gobj : arr)
		{
			auto & gobjBehaviours = gobj.GetAllBehaviours();
			for (auto & behave : gobjBehaviours)
				behave->Update(0.f);
		}
		*/

		UpdateKeys();
		UpdateHotkeys();
		mpEditorEventSys->FireAllPending();
		mpGuiSystem->StartFrame(mDeltaTime);
		MainMenuBar();
	}

	void Editor::UpdateFrame(const float& _dt)
	{ 
		switch (mCurrentState)
		{
		case EDITOR_PLAY:		mpDriver->FixedUpdate();
								mpDriver->Update();			
								break;
		}
		if (ImGui::Begin("Scroll Test"))
		{
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
			ImGui::Text("Gi");
		}
		ImGui::End();
		return;
		for (unsigned int i = 0; i < mArrTabs.size(); ++i)
		{
			EGUI::PushID(i);
			switch (i)
			{
			case 0: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(),
											   EGUI::Docking::eDOCK_SLOT_RIGHT);
				break;
			case 1: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), 
											   EGUI::Docking::eDOCK_SLOT_LEFT);
				break;
			case 2: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), 
											   EGUI::Docking::eDOCK_SLOT_TOP);
				break;
			case 3: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), 
											   EGUI::Docking::eDOCK_SLOT_RIGHT);
				break;
			default: EGUI::Docking::SetNextTabs(mpGuiSystem->GetMainDockspaceName(), 
												EGUI::Docking::eDOCK_SLOT_NONE);
			}

			EditorTab *pTab = mArrTabs[i];
			pTab->SetSize(EGUI::Docking::GetTabSize(pTab->GetLabel().c_str()));
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
					pTab->EditorUI();
				}
			}
			EGUI::EndTab();
			EGUI::PopID();
		}
	}

	void Editor::EndFrame()
	{
		LogTabPerformance();
		mpProfiler->PostUpdate();
		mpDriver->GetSystem<BehaviourSystem>()->PostUpdate();
		mpGuiSystem->EndFrame(); 
		if (mCurrentState != mNextState)  UpdateState();
	}

	void Editor::Shutdown()
	{
		UnInstallHotkeys();
		mpDriver->GetSubSystem<LoggerSystem>()->RedirectOutput(nullptr);
		EGUI::Docking::ShutdownTabs();
		for (auto& e : mArrTabs)
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
		mpEditorEventSys	= nullptr;
		mpInput				= nullptr;
		mpComdHandler		= nullptr;
		mpGuiSystem			= nullptr;
		mpTimer				= nullptr;
		mpWin				= nullptr;
		mpGfx				= nullptr;
		mpProfiler			= nullptr;
		mpFocusGameObj		= nullptr;
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
			break;
		case EDITOR_PLAY:
			break;
		case EDITOR_PAUSE:
			break;
		case EDITOR_EXIT:
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
			if (EGUI::StartMenuBody("New")) NewScene();
			if (EGUI::StartMenuBody("Open")) LoadProc();
			// if (EGUI::StartMenuHeader("Open Recent"))
			// {
			// 	if (EGUI::StartMenuBody("some_recent_crap.cpp"))
			// 	{
			// 		// TODO: Some actual function
			// 	}
			// 	if (EGUI::StartMenuBody("some_recent_crap.h"))
			// 	{
			// 		// TODO: Some actual function
			// 	}
			// 	if (EGUI::StartMenuHeader("More.."))
			// 	{
			// 		EGUI::StartMenuBody("surprise_theres_more_crap.h");
			// 		EGUI::EndMenuHeader();
			// 	}
			// 	EGUI::EndMenuHeader();
			// }
			if (EGUI::StartMenuBody("Save"))		SaveProc();
			if (EGUI::StartMenuBody("Save As.."))	SaveAsProc();
			if (EGUI::StartMenuBody("Quit"))		ChangeState(EDITOR_EXIT);
			EGUI::EndMenuHeader();
		}
	}

	void Editor::MMEdit()
	{
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

	void Editor::EditorDeleteFocus()
	{
		if (!mpFocusGameObj) return;

		mpComdHandler->InvokeCommandDelete(*mpFocusGameObj, mpSceneSystem->GetCurrentScene());
		RemoveFocus();
	}

	void Editor::NewScene()
	{
		
	}

	void Editor::Play()
	{

	}

	void Editor::SaveProc()
	{
		std::string sceneName = mpSceneSystem->GetCurrentScene().GetSceneName();
		if (sceneName != "Untitled")
		{
			std::string ext{ DYSTOPIA_SCENE_EXTENSION.begin(), DYSTOPIA_SCENE_EXTENSION.end() };
			std::string file{ DYSTOPIA_SCENE_LOAD + sceneName + "." + ext };
			mpSceneSystem->SaveScene(file, sceneName);
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
							if (pos != std::string::npos) name.erase(pos);
							mpSceneSystem->SaveScene(std::string{ path.begin(), path.end() }, 
													 std::string{ name.begin(), name.end() });
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

							RemoveFocus();
							mpSceneSystem->LoadScene(std::string{ path.begin(), path.end() });
							if (pos != std::string::npos)
							{
								name.erase(pos);
								mpWin->GetMainWindow().SetTitle(name.c_str());
							}
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
			else if (k >= eButton::KEYBOARD_OEM_1 && k <= eButton::KEYBOARD_OEM_PERIOD)
				mpGuiSystem->UpdateChar(k);
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
		mpGuiSystem->UpdateScroll(0, mpInput->GetMouseWheel());
		if (mpInput->IsKeyPressed(KEY_CTRL))
		{
			if (mpInput->IsKeyTriggered(KEY_Z))			mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_UNDO);
			else if (mpInput->IsKeyTriggered(KEY_Y))	mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_REDO);
			else if (mpInput->IsKeyTriggered(KEY_C))	mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_COPY);
			else if (mpInput->IsKeyTriggered(KEY_X))	mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_CUT);
			else if (mpInput->IsKeyTriggered(KEY_V))	mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_PASTE);
			else if (mpInput->IsKeyTriggered(KEY_S))	mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_SAVE);
			
			if (mpInput->IsKeyTriggered(KEY_SHIFT))
			{
				if (mpInput->IsKeyTriggered(KEY_S))		mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_SAVEAS);
			}
		}
		else if (mpInput->IsKeyTriggered(KEY_DELETE))	
			mpEditorEventSys->Fire(eEditorEvents::EDITOR_HOTKEY_DELETE);
	}
	
	void Editor::InstallHotkeys()
	{
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_DLL_CHANGED)->Bind(&Editor::ReloadDLL, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_UNDO)->Bind(&Editor::EditorUndo, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_REDO)->Bind(&Editor::EditorRedo, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_COPY)->Bind(&Editor::EditorCopy, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_CUT)->Bind(&Editor::EditorCut, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_PASTE)->Bind(&Editor::EditorPaste, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_DELETE)->Bind(&Editor::EditorDeleteFocus, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_SAVE)->Bind(&Editor::SaveProc, this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_SAVEAS)->Bind(&Editor::SaveAsProc, this);
	}

	void Editor::UnInstallHotkeys()
	{
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_DLL_CHANGED)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_UNDO)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_REDO)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_COPY)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_CUT)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_PASTE)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_DELETE)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_SAVE)->Unbind(this);
		mpEditorEventSys->GetEvent(eEditorEvents::EDITOR_HOTKEY_SAVEAS)->Unbind(this);
	}

	void Editor::SetFocus(GameObject& _rObj)
	{
		for (auto& e : mArrTabs)
			e->SetFocus(_rObj);
		mpFocusGameObj = &_rObj;
	}
	
	void Editor::RemoveFocus()
	{
		for (auto& e : mArrTabs)
			e->RemoveFocus();
		mpFocusGameObj = nullptr;
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

	GameObject* Editor::GetCurrentFocusGameObj()
	{
		return mpFocusGameObj;
	}

	void Editor::SetLastPayloadFocus(ePayloadTags e)
	{
		mLatestPayloadFocus = e;
	}

	void Editor::ReloadDLL()
	{
		auto & arr = mpSceneSystem->GetCurrentScene().GetAllGameObjects();
		auto BehaviourSys = EngineCore::GetInstance()->GetSystem<BehaviourSystem>();
		auto const & BehaviourArr = BehaviourSys->GetDllChanges();
		for (auto & elem : BehaviourArr)
		{
			for (auto & gobj : arr)
			{
				auto & gobjBehaviours = gobj.GetAllBehaviours();
				for (auto & behave : gobjBehaviours)
				{
					std::string Name = behave->GetBehaviourName();
					if (Name == elem->mName)
					{
						delete behave;
						behave = nullptr;
						Behaviour * temp = elem->mpBehaviour? nullptr : elem->mpBehaviour->Duplicate();
						behave = temp;
					}
				}
			}
		}
	}
}

#endif		// EDITOR ONLY
