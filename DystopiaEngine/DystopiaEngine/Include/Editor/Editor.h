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
#include <memory>

namespace Dystopia
{
	class WindowManager;
	class GraphicsSystem;
	class InputManager;
	class GuiSystem;
	class Inspector;
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
		Editor(void);
		~Editor();

		void Init(WindowManager*, GraphicsSystem*, InputManager*);
		void StartFrame();
		void UpdateFrame(const float&);
		void EndFrame();
		void Shutdown();

		void ChangeState(eEditorState);
		eEditorState CurrentState() const;
		bool IsClosing() const;
		double PreviousFrameTime() const;

	private:
		eEditorState mCurrentState;
		eEditorState mNextState;
		std::chrono::high_resolution_clock::time_point mStartTime;
		std::chrono::high_resolution_clock::time_point mEndTime;
		double mPrevFrameTime;

		WindowManager *mpWin;
		GraphicsSystem *mpGfx;
		InputManager *mpInput;
		// SceneManager *mpSceneMgr;

		AutoArray<SharedPtr<Inspector>> mInspectorArray;
		
		AutoArray<GuiSystem*> mGuiSysArray;
		void UpdateState();
		void Play();
		void Save();
		void Load();
		void TempSave();
		void TempLoad();
	};
}

#endif	// _EDITOR_H_
#endif	// Editor


