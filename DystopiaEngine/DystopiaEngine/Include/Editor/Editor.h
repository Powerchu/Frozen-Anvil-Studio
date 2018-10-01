/* HEADER *********************************************************************************/
/*!
\file	Editor.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_H_
#define _EDITOR_H_
#include "DataStructure\AutoArray.h"
#include <string>

namespace Dystopia
{
	class EngineCore;
	class WindowManager;
	class GraphicsSystem;
	class EditorInput;
	class GuiSystem;
	class CommandHandler;
	class EditorTab;
	class EditorEventHandler;
	class SceneSystem;
	class PhysicsSystem;
	class Timer;
	class Profiler;
	class GameObject;
	class BehaviourSystem;
	enum ePayloadTags;

	enum eEditorState
	{
		EDITOR_INVALID = -1,
		EDITOR_EXIT = 0,
		EDITOR_MAIN,
		EDITOR_PLAY,
		EDITOR_PAUSE
	};

	class Editor
	{
	public:
		static Editor*	GetInstance();
		~Editor();

		/* General Looping Funcs */
		void			Init();
		void			LoadDefaults();
		void			LoadSettings();
		void			LoadTabs();
		void			StartFrame();
		void			UpdateFrame(const float&);
		void			EndFrame();
		void			Shutdown();

		/* delta time */
		float			GetDeltaTime() const;

		/* State change stuff */
		void			ChangeState(eEditorState);
		bool			IsClosing() const;
		eEditorState	CurrentState() const;
		void			OpenScene(const std::wstring& _path, const std::wstring& _name);

		/* Game Object stuff */
		void			SetLastPayloadFocus(ePayloadTags);
		void			SetFocus(GameObject&);
		void			RemoveFocus();
		GameObject*		FindGameObject(const uint64_t& _id) const;
		GameObject*		GetCurrentFocusGameObj();

	private:
		Editor(void);

		EngineCore				*mpDriver;
		WindowManager			*mpWin;
		GraphicsSystem			*mpGfx;
		SceneSystem				*mpSceneSystem;
		PhysicsSystem			*mpPhysicsSystem;
		Profiler				*mpProfiler;
		BehaviourSystem			*mpBehaviourSys;

		EditorEventHandler		*mpEditorEventSys;
		CommandHandler			*mpComdHandler;
		GuiSystem				*mpGuiSystem;
		Timer					*mpTimer;
		EditorInput				*mpInput;

		AutoArray<EditorTab*>	mArrTabs;
		eEditorState			mCurrentState;
		eEditorState			mNextState;
		float					mDeltaTime;
		std::string				mTempSaveFile;
		GameObject				*mpFocusGameObj;
		ePayloadTags			mLatestPayloadFocus;

		/* TODO: The functions for changing into different states. */
		void			UpdateState();
		void			GamePlay();
		void			GameStop();
		void			NewScene();
		void			SaveProc();
		void			SaveAsProc();
		void			LoadProc();
		void			TempSave();
		void			TempLoad();

		/* The main menu bar functions */
		void			MainMenuBar();
		void			MMFile();
		void			MMEdit();
		void			MMView();
		void			MMGame();

		/* The edit functions */
		void			EditorUndo();
		void			EditorRedo();
		void			EditorCopy();
		void			EditorCut();
		void			EditorPaste();
		void			EditorDeleteFocus();

		/* EditorEvents */
		void			UpdateKeys();
		void			UpdateHotkeys();
		void			UpdateGameModeKeys();
		void			InstallHotkeys();
		void			UnInstallHotkeys();
		void			ReloadDLL();

		/* Misc functions */
		void			LogTabPerformance();
	};
}

#endif	// _EDITOR_H_
#endif	// Editor


