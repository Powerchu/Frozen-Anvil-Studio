#ifndef _EDITOR_MAIN_H_
#define _EDITOR_MAIN_H_
#include "DataStructure/AutoArray.h"
#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"
#include "Globals.h"

#include "EditorStates.h"
#include "EditorSystem.h"
#include "EditorPanel.h"

namespace Editor
{
	class EditorMain
	{
	public:
		using ESystems = Ut::MetaSortT_t < Ut::MetaLessThan, Ut::Collection <
			Ut::Indexer<0u, class EInput>
		>>;

		using EPanels = Ut::MetaSortT_t<Ut::MetaLessThan, Ut::Collection<
			>>;

		static EditorMain* GetInstance(void);
		~EditorMain(void);

		void Init(void);
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


	private:
		EditorMain(void);

		AutoArray<EditorSystem*> mArrSystems;
		AutoArray<EditorPanel*> mArrPanels;

		eState	mCurState;
		eState  mNextState;
		float	mDelta;
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




