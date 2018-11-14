/* HEADER *********************************************************************************/
/*!
\file	CommandTypes.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _COMMAND_TYPES_H_
#define _COMMAND_TYPES_H_
#include "Editor/EditorMain.h"

#include "Component/Component.h"
#include "System/Base/Systems.h"
#include "Object/GameObject.h"
#include "Utility/Meta.h"

#include "DataStructure/AutoArray.h"

#include <utility>
#include <tuple>

namespace Editor
{
	class Command
	{
	public:
		virtual bool Do(void) = 0;
		virtual bool Undo(void) = 0;
		virtual bool Unchanged(void) const = 0;
		virtual ~Command(void) {}
	};

	template<class C, typename ... T>
	class ModifyValue;

	template<class C, typename T>
	class ModifyValue<C, T, Ut::Type_t<Ut::EnableIf_t<std::is_base_of<Dystopia::Component, C>::value>>> : public Command
	{
	public:
		ModifyValue(T C::* _ptrToMemVar, const T& _oldVal, const T& _newVal, bool C::* _ptrUndoNotify = nullptr)
			: mpMemVar{ _ptrToMemVar }, mOldV{ _oldVal }, mNewV{ _newVal }, mpUndoNotify{ _ptrUndoNotify }
		{}

		bool Do(void)
		{
			return true;
		}

		bool Undo(void)
		{
			return true;
		}

		bool Unchanged(void) const
		{
			return mOldV == mNewV;
		}

	private:
		T C::* mpMemVar;
		T mOldV;
		T mNewV;
		bool C::* mpUndoNotify;
	};

	template<class S, typename T>
	class ModifyValue<S, T, Ut::Type_t<Ut::EnableIf_t<std::is_base_of<Dystopia::Systems, S>::value>>> : public Command
	{
	public:
		ModifyValue(T S::* _ptrToMemVar, const T& _oldVal, const T& _newVal, bool S::* _ptrUndoNotify = nullptr)
			: mpMemVar{ _ptrToMemVar }, mOldV{ _oldVal }, mNewV{ _newVal }, mpUndoNotify{ _ptrUndoNotify }
		{}

		bool Do(void)
		{
			return true;
		}

		bool Undo(void)
		{
			return true;
		}
		 
		bool Unchanged(void) const
		{
			return mOldV == mNewV;
		}

	private:
		T S::* mpMemVar;
		T mOldV;
		T mNewV;
		bool S::* mpUndoNotify;
	};

	template<typename T>
	class ModifyValue<Dystopia::GameObject, T, Dystopia::GameObject> : public Command
	{
	public:
		ModifyValue(T Dystopia::GameObject::* _ptrToMemVar, const T& _oldVal, const T& _newVal, bool Dystopia::GameObject::* _ptrUndoNotify = nullptr)
			: mpMemVar{ _ptrToMemVar }, mOldV{ _oldVal }, mNewV{ _newVal }, mpUndoNotify{ _ptrUndoNotify }
		{}

		bool Do(void)
		{
			return true;
		}

		bool Undo(void)
		{
			return true;
		}

		bool Unchanged(void) const
		{
			return mOldV == mNewV;
		}

	private:
		T Dystopia::GameObject::* mpMemVar;
		T mOldV;
		T mNewV;
		bool Dystopia::GameObject::* mpUndoNotify;
	};

	class InsertGameObject : public Command
	{
	public:
		InsertGameObject(uint64_t _id);

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		uint64_t mnObjID;
	};

	class DeleteGameObject : public Command
	{
	public:
		DeleteGameObject(uint64_t _id);

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		uint64_t mnObjID;
	};

	class BatchExecute : public Command
	{
	public:
		BatchExecute(AutoArray<Command*>&& _arr);
		~BatchExecute(void);

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		AutoArray<Command*> mArrCommands;
	};

	template<class C, typename ... Ts>
	class ComponentFunction
	{
	public:
		ComponentFunction(void(C::*_fnptr)(Ts ...), std::remove_reference_t<Ts> ... params);
		ComponentFunction(const ComponentFunction& _rhs);

		void Execute(C * const _toMod) const;		

		ComponentFunction& operator=(const ComponentFunction& _rhs);

		template<typename A>
		struct AuxEx;

		template<size_t ... Ns>
		struct AuxEx<std::index_sequence<Ns ...>>
		{
			AuxEx(ComponentFunction<C, Ts...>& _p);
			void Execute(C * const _toMod) const;
			ComponentFunction<C, Ts...> &mParent;
		};

	private:
		void (C::* mpComponentFn)(Ts...);
		std::tuple<std::remove_reference<Ts>...> mTupleParams;
		AuxEx<std::make_index_sequence<sizeof...(Ts)>> mAuxCaller;
	};

	template<class D, class UD>
	class FunctionComd;

	template<class C, typename ... T1s, typename ... T2s >
	class FunctionComd<ComponentFunction<C, T1s...>, ComponentFunction<C, T2s...>> : public Command
	{
	public:
		FunctionComd(const uint64_t& _objID, const ComponentFunction<C, T1s ...>& _do, const ComponentFunction<C, T2s ...>& _undo);

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		ComponentFunction<C, T1s ...> mDoFunc;
		ComponentFunction<C, T2s ...> mUnDoFunc;
		uint64_t mID;
	};

	template<typename ... T1s, typename ... T2s >
	class FunctionComd<ComponentFunction<Dystopia::GameObject, T1s...>, ComponentFunction<Dystopia::GameObject, T2s...>> : public Command
	{
	public:
		FunctionComd(const uint64_t& _objID, const ComponentFunction<Dystopia::GameObject, T1s ...>& _do, const ComponentFunction<Dystopia::GameObject, T2s ...>& _undo);

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		ComponentFunction<Dystopia::GameObject, T1s ...> mDoFunc;
		ComponentFunction<Dystopia::GameObject, T2s ...> mUnDoFunc;
		uint64_t mID;
	};

}

template<class C, typename ... Ts>
Editor::ComponentFunction<C, Ts...>::ComponentFunction(void(C::*_fnptr)(Ts ...), std::remove_reference_t<Ts> ... params)
	: mpComponentFn{ _fnptr }, mTupleParams{ params... }, mAuxCaller{ *this }
{}

template<class C, typename ... Ts>
Editor::ComponentFunction<C, Ts...>::ComponentFunction(const Editor::ComponentFunction<C, Ts...>& _rhs)
	: mpComponentFn{ _rhs.mpComponentFn }, mTupleParams{ _rhs.mTupleParams }, mAuxCaller{ *this }
{}

template<class C, typename ... Ts>
void Editor::ComponentFunction<C, Ts...>::Execute(C * const _toMod) const
{
	mAuxCaller.Execute(_toMod);
}

template<class C, typename ... Ts>
Editor::ComponentFunction<C, Ts...>& Editor::ComponentFunction<C, Ts...>::operator=(const Editor::ComponentFunction<C, Ts...>& _rhs)
{
	mpComponentFn = _rhs.mpComponentFn;
	mTupleParams = _rhs.mTupleParams;
}

template<class C, typename ... Ts>
template<size_t ... Ns>
Editor::ComponentFunction<C, Ts...>::AuxEx<std::index_sequence<Ns ...>>::AuxEx(ComponentFunction<C, Ts...>& _p)
	: mParent{ _p }
{}

template<class C, typename ... Ts>
template<size_t ... Ns>
void Editor::ComponentFunction<C, Ts...>::AuxEx<std::index_sequence<Ns ...>>::Execute(C * const _toMod) const
{
	(_toMod->*(mParent.mpComponentFn))(std::get<Ns>(mParent.mTupleParams) ...);
}

template<class C, typename ... T1s, typename ... T2s >
Editor::FunctionComd<Editor::ComponentFunction<C, T1s...>, 
					 Editor::ComponentFunction<C, T2s...>>::
	FunctionComd(const uint64_t& _objID, const ComponentFunction<C, T1s ...>& _do, const ComponentFunction<C, T2s ...>& _undo)
	: mUnDoFunc{ _undo }, mDoFunc{ _do }, mID{ _objID }
{}

template<class C, typename ... T1s, typename ... T2s >
bool Editor::FunctionComd<Editor::ComponentFunction<C, T1s...>,
						  Editor::ComponentFunction<C, T2s...>>::Do(void)
{
	return true;
}

template<class C, typename ... T1s, typename ... T2s >
bool Editor::FunctionComd<Editor::ComponentFunction<C, T1s...>,
						  Editor::ComponentFunction<C, T2s...>>::Undo(void)
{
	return true;
}

template<class C, typename ... T1s, typename ... T2s >
bool Editor::FunctionComd<Editor::ComponentFunction<C, T1s...>,
						  Editor::ComponentFunction<C, T2s...>>::Unchanged(void) const
{
	return false;
}

template<typename ... T1s, typename ... T2s >
Editor::FunctionComd<Editor::ComponentFunction<Dystopia::GameObject, T1s...>,
					 Editor::ComponentFunction<Dystopia::GameObject, T2s...>>::
	FunctionComd(const uint64_t& _objID, const ComponentFunction<Dystopia::GameObject, T1s ...>& _do, const ComponentFunction<Dystopia::GameObject, T2s ...>& _undo)
	: mUnDoFunc{ _undo }, mDoFunc{ _do }, mID{ _objID }
{}

template<typename ... T1s, typename ... T2s >
bool Editor::FunctionComd<Editor::ComponentFunction<Dystopia::GameObject, T1s...>,
						  Editor::ComponentFunction<Dystopia::GameObject, T2s...>>::Do(void)
{
	return true;
}

template<typename ... T1s, typename ... T2s >
bool Editor::FunctionComd<Editor::ComponentFunction<Dystopia::GameObject, T1s...>,
						  Editor::ComponentFunction<Dystopia::GameObject, T2s...>>::Undo(void)
{
	return true;
}

template<typename ... T1s, typename ... T2s >
bool Editor::FunctionComd<Editor::ComponentFunction<Dystopia::GameObject, T1s...>,
						  Editor::ComponentFunction<Dystopia::GameObject, T2s...>>::Unchanged(void) const
{
	return false;
}




#endif
#endif







