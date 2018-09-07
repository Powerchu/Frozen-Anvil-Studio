/* HEADER *********************************************************************************/
/*!
\file	CommandList.h
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
#ifndef _COMMAND_LIST_H_
#define _COMMAND_LIST_H_
#include "Object/GameObject.h"
#include "Editor.h"
#include <tuple>
#include <utility>

namespace Dystopia
{
	struct Commands
	{
		virtual void ExecuteDo() = 0;
		virtual void ExecuteUndo() = 0;
		virtual bool Unchanged() const = 0;
		virtual ~Commands() {}
	};

	struct RecordBase : Commands
	{
		virtual void EndRecord() = 0;
		virtual ~RecordBase() {}
	};

	template <typename T>
	struct ComdModifyValue : Commands
	{
		ComdModifyValue(T* _value, const T& _newV)
			: mValue{ _value }, mOldValue{ *_value }, mNewValue{ _newV }
		{}

		~ComdModifyValue()
		{
			mValue = nullptr;
		}

		void	ExecuteDo() override	{ *mValue = mNewValue; }
		void	ExecuteUndo() override	{ *mValue = mOldValue; } 
		bool	Unchanged() const		{ return (mOldValue == mNewValue); }
	private:
		T mOldValue;
		T mNewValue;
		T* mValue;
	};

	template <typename T>
	struct ComdRecord : RecordBase
	{
		ComdRecord(T*& rhs)
			: mpTarget{ rhs }, mOldValue{ *mpTarget }, mNewValue{ mOldValue }
		{}

		~ComdRecord()
		{
			mpTarget = nullptr;
		}

		void	EndRecord()				{ mNewValue = *mpTarget; }
		T*		GetPointer()			{ return mpTarget; }
		void	ExecuteDo() override	{ *mpTarget = mNewValue; }
		void	ExecuteUndo() override	{ *mpTarget = mOldValue; }
		bool	Unchanged() const		{ return (mOldValue == mNewValue); }

	private:
		T* mpTarget;
		T mOldValue;
		T mNewValue;
	};

	template<class Component, typename ... Params>
	struct FunctionModWrapper
	{
		FunctionModWrapper(void(Component::*_fnptr)(Params ...), Params ... pack)
			: mfptr{ _fnptr }, 
			mTupleVariables{ std::make_tuple<Params...>(pack...) }
		{}

		void Execute(Component * const _toMod) const
		{
			(_toMod->*mfptr)(std::get<0>(mTupleVariables));
		}

	private:
		void(Component::*mfptr)(Params ...);
		std::tuple<Params...> mTupleVariables;
		static constexpr size_t size = sizeof...(Params);
	};

	template<class Do, class UDo>
	struct ComdModifyComponent;

	template<class Component, typename ... P1s, typename ... P2s >
	struct ComdModifyComponent<FunctionModWrapper<Component, P1s ...>, FunctionModWrapper<Component, P2s...>> : Commands
	{
		ComdModifyComponent(const unsigned long& _objID, 
							const FunctionModWrapper<Component, P1s ...>& _do, 
							const FunctionModWrapper<Component, P2s ...>& _undo)
			: mID{ _objID }, mDoFunc{ _do }, mUnDoFunc{ _undo }
		{}

		void ExecuteDo() override
		{
			GameObject * pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return; 

			Component * pCom = pObj->GetComponent<Component>();
			if (!pCom) return;

			mDoFunc.Execute(pCom);
		}

		void ExecuteUndo() override
		{
			GameObject * pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return;

			Component * pCom = pObj->GetComponent<Component>();
			if (!pCom) return;

			mUnDoFunc.Execute(pCom);
		}

		bool Unchanged() const
		{
			return false;
		}

	private:
		FunctionModWrapper<Component, P1s ...> mDoFunc;
		FunctionModWrapper<Component, P2s ...> mUnDoFunc;
		unsigned long mID;
	};

	template<typename ... P1s, typename ... P2s >
	struct ComdModifyComponent<FunctionModWrapper<GameObject, P1s ...>, FunctionModWrapper<GameObject, P2s...>> : Commands
	{
		ComdModifyComponent(const unsigned long& _objID,
			const FunctionModWrapper<GameObject, P1s ...>& _do,
			const FunctionModWrapper<GameObject, P2s ...>& _undo)
			: mID{ _objID }, mDoFunc{ _do }, mUnDoFunc{ _undo }
		{}

		void ExecuteDo() override
		{
			GameObject * pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return;
			mDoFunc.Execute(pObj);
		}

		void ExecuteUndo() override
		{
			GameObject * pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return;
			mUnDoFunc.Execute(pObj);
		}

		bool Unchanged() const
		{
			return false;
		}

	private:
		FunctionModWrapper<GameObject, P1s ...> mDoFunc;
		FunctionModWrapper<GameObject, P2s ...> mUnDoFunc;
		unsigned long mID;
	};
}

#endif //_COMMAND_LIST_H_
#endif //EDITOR
