/* HEADER *********************************************************************************/
/*!
\file	EditorMain.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_MAIN_H_
#define _EDITOR_MAIN_H_
#include "Globals.h"

#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"

#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"

#include "System/Time/Timer.h"

#include "Editor/EditorStatesTypes.h"
#include "Editor/EditorSystem.h"
#include "Editor/EditorPanel.h"

namespace Editor
{
	class EditorMain
	{
	public:
		using ESystems = typename Ut::MetaAutoIndexer <
			class EditorStates,
			class EInput,
			class EditorUI,
			class EditorClipboard,
			class EditorResource,
			class EditorCommands,
			class EditorFactory
		>::result;

		using EPanels = typename Ut::MetaAutoIndexer<
			class Inspector,
			class ConsoleLog,
			class HierarchyView,
			class ColorScheme,
			class StyleScheme,
			class ProjectResource,
			class PerformanceLog,
			class SceneView,
			class SpritePreviewer,
			class SpriteEditor,
			class ProjectSettings,
			class BehaviourTreeEditor
		>::result;

		static EditorMain* GetInstance(void);
		~EditorMain(void);

		void Init(void);

		void StartFrame(void);
		void Update(void);
		void EndFrame(void);
		void Shutdown(void);

		void ChangeState(eState);
		void Broadcast(eEMessage);

		float GetDeltaTime(void) const;
		eState GetCurState(void) const;
		bool IsClosing(void);
		
		template<typename T>
		T* const GetSystem(void) const;

		template<typename T>
		T* const GetPanel(void) const;

		HashString GetCurProjFolder(void) const;
		HashString GetCurProjFile(void) const;

		AutoArray<EditorSystem*>& GetAllSystems(void);
		AutoArray<EditorPanel*>& GetAllPanels(void);

		void ExternalFile(const wchar_t* const _p);
		void ExternalFile(const HashString&);
		void ExternalFile(const AutoArray<HashString>&);

	private:
		EditorMain(void);

		void ProjectLauncher(void);
		void UpdatePaths(void);
		void LoadProjSettings(void);
		void LogPerformance(void);

		AutoArray<EditorSystem*> mArrSystems;
		AutoArray<EditorPanel*> mArrPanels;

		eState	mCurState;
		eState  mNextState;
		float	mfDelta;
		float	mfOverlayAlpha;

		HashString mProjFolder;
		HashString mProjFile;

		Dystopia::Timer mTimer;

		AutoArray<eEMessage> mMsgPending;
	};
}

template<typename T>
inline T* const Editor::EditorMain::GetSystem(void) const
{
	using type = Ut::MetaFind<T, ESystems>;
	static_assert(type::value, "ESystem not found!");

	return static_cast<T*>(mArrSystems[type::result::value]);
}

template<typename T>
inline T* const Editor::EditorMain::GetPanel(void) const
{
	using type = Ut::MetaFind<T, EPanels>;
	static_assert(type::value, "EPanel not found!");

	return static_cast<T*>(mArrPanels[type::result::value]);
}

#endif
#endif 




