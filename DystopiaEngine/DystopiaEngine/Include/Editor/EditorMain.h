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
		using ESystems = Ut::MetaSortT_t < Ut::MetaLessThan, Ut::Collection <
			Ut::Indexer<0u, class EditorStates>,
			Ut::Indexer<1u, class EInput>,
			Ut::Indexer<2u, class EditorUI>,
			Ut::Indexer<3u, class EditorClipboard>,
			Ut::Indexer<4u, class EditorResource>
		>>;

		using EPanels = Ut::MetaSortT_t<Ut::MetaLessThan, Ut::Collection<
			>>;

		static EditorMain* GetInstance(void);
		~EditorMain(void);

		void Init(void);

		void ProjectLauncher(void);

		void StartFrame(void);
		void Update(void);
		void EndFrame(void);
		void Shutdown(void);

		void ChangeState(eState);

		float GetDeltaTime(void) const;
		eState GetCurState(void) const;
		bool IsClosing(void);
		
		template<typename T>
		T* const GetSystem(void) const;

		template<typename T>
		T* const GetPanel(void) const;

		HashString GetCurProjectFullPath(void) const;

	private:
		EditorMain(void);

		AutoArray<EditorSystem*> mArrSystems;
		AutoArray<EditorPanel*> mArrPanels;

		eState	mCurState;
		eState  mNextState;
		float	mDelta;

		HashString mProjectPath;

		Dystopia::Timer mTimer;
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




