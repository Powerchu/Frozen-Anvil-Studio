/* HEADER *********************************************************************************/
/*!
\file	EditorCommands.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_COMMANDS_H_
#define _EDITOR_COMMANDS_H_
#include "Editor/EditorSystem.h"
#include "Editor/CommandTypes.h"

#include "Allocator/DefaultAlloc.h"

#include "Utility//Utility.h"

#include <deque>

namespace Editor
{
	class EditorCommands : public EditorSystem
	{
	public:
		EditorCommands(void);
		~EditorCommands(void);

		void Load(void);
		bool Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);

		void SavedChanges(void);
		bool HasChanges(void) const;

		void Undo(void);
		void Redo(void);

		void InsertNewGameObject(void);
		void RemoveGameObject(const uint64_t&);

		template <class Component, typename ... Ts>
		void FunctionCommand(const uint64_t& _objID, const ComponentFunction<Component, Ts ...>& _oldFn, 
													 const ComponentFunction<Component, Ts ...>& _newFn);

		template<class Component, typename ... Ts>
		auto MakeFnCommand(void(Component::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params);

	private:

		std::deque<Command*> mDeqRedo;
		std::deque<Command*> mDeqUndo;
		bool mbChangesMade;
		bool mbDisableCommands;
		size_t mnUndo;
		size_t mnRedo;

		void RemoveStray(std::deque<Command*>&);
	};
}


/***************************************************************** Template Function Definitions *****************************************************************/

template <class Component, typename ... Ts>
inline void Editor::EditorCommands::FunctionCommand(const uint64_t& _objID, const ComponentFunction<Component, Ts ...>& _oldFn, 
																			const ComponentFunction<Component, Ts ...>& _newFn)
{
	using fnType = FunctionComd<ComponentFunction<Component, Ts ...>, ComponentFunction<Component, Ts ...>>;
	if (mbDisableCommands)
		return;

	Command *pCommand = Dystopia::DefaultAllocator<fnType>::ConstructAlloc(_objID, _newFn, _oldFn);

	if (pCommand->Do())
		mDeqUndo.push_back(pCommand);
	else
		Dystopia::DefaultAllocator<Command>::DestructFree(pCommand);
}

template<class Component, typename ... Ts>
inline auto Editor::EditorCommands::MakeFnCommand(void (Component::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params)
{
	return ComponentFunction<Component, Ts...>{ _function, _params ... };
}

#endif
#endif 








