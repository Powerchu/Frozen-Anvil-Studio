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
#include "DataStructure\SharedPtr.h"
#include <chrono>

namespace Dystopia
{
	class WindowManager;
	class GraphicsSystem;
	class InputManager;
	class GuiSystem;
	class CommandHandler;
	class EditorTab;
	//class SceneManager;
	//class Scene;

	enum eEditorState
	{
		EDITOR_INVALID = -1,
		EDITOR_EXIT = 0,
		EDITOR_MAIN,
		EDITOR_PLAY,
		EDITOR_PAUSE,
		EDITOR_SAVE,
		EDITOR_LOAD
	};

	class Editor
	{
	public:
		static Editor*	GetInstance();
		~Editor();

		void			Init(WindowManager*, GraphicsSystem*, InputManager*);
		void			StartFrame(const float&);
		void			UpdateFrame(const float&);
		void			EndFrame();
		void			Shutdown();
		void			ChangeState(eEditorState);
		eEditorState	CurrentState() const;
		bool			IsClosing() const;
		double			PreviousFrameTime() const;

	private:
		Editor(void);

		eEditorState	mCurrentState;
		eEditorState	mNextState;
		int				mExtraTabCounter;
		double			mPrevFrameTime;
		WindowManager	*mpWin;
		GraphicsSystem	*mpGfx;
		InputManager	*mpInput;
		CommandHandler	*mpComdHandler;
		GuiSystem		*mpGuiSystem;
		// SceneManager *mpSceneMgr;

		AutoArray<EditorTab*> mTabsArray;
		void			UpdateState();
		void			Play();
		void			Save();
		void			Load();
		void			TempSave();
		void			TempLoad();
		void			MainMenu();
	};
}

#endif	// _EDITOR_H_
#endif	// Editor


