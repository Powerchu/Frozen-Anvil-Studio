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
		virtual bool ExecuteDo() = 0;
		virtual bool ExecuteUndo() = 0;
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

		bool	ExecuteDo() override	{ *mValue = mNewValue; return true; }
		bool	ExecuteUndo() override	{ *mValue = mOldValue; return true; }
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
		bool	ExecuteDo() override	{ *mpTarget = mNewValue; return true; }
		bool	ExecuteUndo() override	{ *mpTarget = mOldValue; return true; }
		bool	Unchanged() const		{ return (mOldValue == mNewValue); }

	private:
		T* mpTarget;
		T mOldValue;
		T mNewValue;
	};

	template<class Component, typename ... Params>
	struct FunctionModWrapper
	{
		FunctionModWrapper(void(Component::*_fnptr)(Params ...), std::remove_reference_t<Params> ... pack)
			: mfptr{ _fnptr }, 
			mTupleVariables{ pack... }
		{}

		void Execute(Component * const _toMod) const
		{
			(_toMod->*mfptr)(std::get<0>(mTupleVariables));
		}

		FunctionModWrapper& operator=(const FunctionModWrapper& _rhs)
		{
			mfptr = _rhs.mfptr;
			mTupleVariables = _rhs.mTupleVariables;
		}

	private:
		void(Component::*mfptr)(Params ...);
		std::tuple<std::remove_reference_t<Params>...> mTupleVariables;
		static constexpr size_t size = sizeof...(Params);
	};

	template<class Do, class UDo>
	struct ComdModifyComponent;

	template<class Component, typename ... P1s, typename ... P2s >
	struct ComdModifyComponent<FunctionModWrapper<Component, P1s ...>, FunctionModWrapper<Component, P2s ...>> : Commands
	{
		ComdModifyComponent(const unsigned long& _objID, 
							const FunctionModWrapper<Component, P1s ...>& _do, 
							const FunctionModWrapper<Component, P2s ...>& _undo)
			: mID{ _objID }, mDoFunc{ _do }, mUnDoFunc{ _undo }
		{}

		bool ExecuteDo() override
		{
			GameObject * pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;

			Component * pCom = pObj->GetComponent<Component>();
			if (!pCom) return false;

			mDoFunc.Execute(pCom);
			return true;
		}

		bool ExecuteUndo() override
		{
			GameObject * pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;

			Component * pCom = pObj->GetComponent<Component>();
			if (!pCom) return false;

			mUnDoFunc.Execute(pCom);
			return true;
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
	struct ComdModifyComponent<FunctionModWrapper<GameObject, P1s ...>, FunctionModWrapper<GameObject, P2s ...>> : Commands
	{
		ComdModifyComponent(const unsigned long& _objID,
			const FunctionModWrapper<GameObject, P1s ...>& _do,
			const FunctionModWrapper<GameObject, P2s ...>& _undo)
			: mID{ _objID }, mDoFunc{ _do }, mUnDoFunc{ _undo }
		{}

		bool ExecuteDo() override
		{
			GameObject * pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;
			mDoFunc.Execute(pObj);
			return true;
		}

		bool ExecuteUndo() override
		{
			GameObject * pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;
			mUnDoFunc.Execute(pObj);
			return true;
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
