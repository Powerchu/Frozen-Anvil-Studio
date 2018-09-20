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

#include "Editor.h"
#include <tuple>
#include <utility>

namespace Dystopia
{
	class Scene;
	class GameObject;
	struct Commands
	{
		virtual bool ExecuteDo() = 0;
		virtual bool ExecuteUndo() = 0;
		virtual bool Unchanged() const = 0;
		virtual ~Commands() {}
	};

	struct RecordBase : Commands
	{
		virtual bool EndRecord() = 0;
		virtual ~RecordBase() {}
	};

	struct ComdInsertObject : Commands
	{
		ComdInsertObject(GameObject* _pObj, Scene * _pScene, bool * _notify = nullptr);
		~ComdInsertObject();
		bool ExecuteDo() override;
		bool ExecuteUndo() override;
		bool Unchanged() const;

	private:
		bool *mpNotify;
		bool mFocusBack;
		uint64_t mObjID;
		GameObject *mpObj;
		Scene *mpScene;
	};

	struct ComdDeleteObject : Commands
	{
		ComdDeleteObject(GameObject* _pObj, Scene * _pScene, bool * _notify = nullptr);
		~ComdDeleteObject();
		bool ExecuteDo() override;
		bool ExecuteUndo() override;
		bool Unchanged() const;

	private:
		bool *mpNotify;
		bool mFocusBack;
		uint64_t mObjID;
		GameObject *mpObj;
		Scene *mpScene;
	};

	template <typename T, class Component>
	struct ComdModifyValue : Commands
	{
		ComdModifyValue(const uint64_t& _objID, T* _pmData, const T& _oldV, bool * _notify = nullptr)
			: mID{ _objID }, mpData{ _pmData }, 
			mNewValue{*_pmData }, mOldValue{ _oldV }, mpNotify{ _notify }
		{}

		bool ExecuteDo() override 
		{
			if (!FindComponent()) return false;
			if (mpNotify) *mpNotify = true;
			*mpData = mNewValue;
			return true; 
		}

		bool ExecuteUndo() override 
		{
			if (!FindComponent()) return false;
			if (mpNotify) *mpNotify = true;
			*mpData = mOldValue;
			return true;
		}

		bool Unchanged() const { return (mOldValue == mNewValue); }
	private:
		Component* FindComponent()
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return nullptr;
			Component *pCom = pObj->GetComponent<Component>();
			if (!pCom) return nullptr;
			return pCom;
		}
		bool *mpNotify;
		uint64_t mID;
		T* mpData;
		T mOldValue;
		T mNewValue;
	};

	template <typename T>
	struct ComdModifyValue<T, GameObject> : Commands
	{
		ComdModifyValue(const uint64_t& _objID, T * _pmData, const T& _oldV, bool * _notify = nullptr)
			: mID{ _objID }, mpData{ _pmData },
			mNewValue{ *mpData }, mOldValue{ _oldV }, mpNotify{ _notify }
		{}

		bool ExecuteDo() override
		{
			if (!Editor::GetInstance()->FindGameObject(mID)) return false;
			if (mpNotify) *mpNotify = true;
			*mpData = mNewValue;
			return true;
		}

		bool ExecuteUndo() override
		{
			if (!Editor::GetInstance()->FindGameObject(mID)) return false;
			if (mpNotify) *mpNotify = true;
			*mpData = mOldValue;
			return true;
		}

		bool Unchanged() const { return (mOldValue == mNewValue); }
	private:
		bool *mpNotify;
		uint64_t mID;
		T* mpData;
		T mOldValue;
		T mNewValue;
	};

	template <typename T, class Component>
	struct ComdRecord : RecordBase
	{
		ComdRecord(const uint64_t& _objID, T* rhs, bool * _notify = nullptr)
			: mpTarget{ rhs }, mOldValue{ *rhs }, 
			mNewValue{ mOldValue }, mID{ _objID }, mpNotify{ _notify }
		{}

		bool EndRecord()				
		{ 
			if (!FindComponent()) return false;
			mNewValue = *mpTarget;
			return true;
		}

		bool ExecuteDo() override	
		{
			if (!FindComponent()) return false;
			if (mpNotify) *mpNotify = true;
			*mpTarget = mNewValue; 
			return true; 
		}

		bool ExecuteUndo() override	
		{
			if (!FindComponent()) return false;
			if (mpNotify) *mpNotify = true;
			*mpTarget = mOldValue; 
			return true; 
		}

		bool Unchanged() const { return false; }
		T* GetPointer() { return mpTarget; }
	private:
		Component* FindComponent()
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return nullptr;
			Component *pCom = pObj->GetComponent<Component>();
			if (!pCom) return nullptr;
			return pCom;
		}
		bool *mpNotify;
		uint64_t mID;
		T* mpTarget;
		T mOldValue;
		T mNewValue;
	};

	template <typename T>
	struct ComdRecord<T, GameObject> : RecordBase
	{
		ComdRecord(const uint64_t& _objID, T* rhs, bool * _notify = nullptr)
			: mpTarget{ rhs }, mOldValue{ *rhs }, 
			mNewValue{ mOldValue }, mID{ _objID }, mpNotify{ _notify }
		{}

		bool EndRecord()
		{
			if (!Editor::GetInstance()->FindGameObject(mID)) return false;
			mNewValue = *mpTarget;
			return true;
		}

		bool ExecuteDo() override
		{
			if (!Editor::GetInstance()->FindGameObject(mID)) return false;
			if (mpNotify) *mpNotify = true;
			*mpTarget = mNewValue;
			return true;
		}

		bool ExecuteUndo() override
		{
			if (!Editor::GetInstance()->FindGameObject(mID)) return false;
			if (mpNotify) *mpNotify = true;
			*mpTarget = mOldValue;
			return true;
		}

		bool	Unchanged() const { return false; }
		T*		GetPointer() { return mpTarget; }
	private:
		bool *mpNotify;
		uint64_t mID;
		T* mpTarget;
		T mOldValue;
		T mNewValue;
	};

	template<class Component, typename ... Params>
	struct FunctionModWrapper
	{
	private:
		template<typename A>
		struct AuxExecution;

		template<size_t ... Ns>
		struct AuxExecution<std::index_sequence<Ns ...>>
		{
			AuxExecution(FunctionModWrapper<Component, Params...>& p)
				: parent{ p }
			{}

			void Execute(Component * const _toMod) const
			{
				(_toMod->*(parent.mfptr))(std::get<Ns>(parent.mTupleVariables) ...);
			}

			FunctionModWrapper<Component, Params...> &parent;
		};

		void(Component::*mfptr)(Params ...);
		std::tuple<std::remove_reference_t<Params>...> mTupleVariables;
		AuxExecution<std::make_index_sequence<sizeof...(Params)>> auxCaller;

	public:
		FunctionModWrapper(void(Component::*_fnptr)(Params ...), std::remove_reference_t<Params> ... pack)
			: mfptr{ _fnptr },
			mTupleVariables{ pack... }, 
			auxCaller{ *this }
		{}

		FunctionModWrapper(const FunctionModWrapper& _rhs)
			: mfptr{ _rhs.mfptr },
			mTupleVariables{ _rhs.mTupleVariables },
			auxCaller{ *this }
		{}

		void Execute(Component * const _toMod) const
		{
			auxCaller.Execute(_toMod);
		}

		FunctionModWrapper& operator=(const FunctionModWrapper& _rhs)
		{
			mfptr = _rhs.mfptr;
			mTupleVariables = _rhs.mTupleVariables;
		}

	};

	template<class Do, class UDo>
	struct ComdModifyComponent;

	template<class Component, typename ... P1s, typename ... P2s >
	struct ComdModifyComponent<FunctionModWrapper<Component, P1s ...>, FunctionModWrapper<Component, P2s ...>> : Commands
	{
		ComdModifyComponent(const uint64_t& _objID,
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
		using FMW1 = FunctionModWrapper<Component, P1s ...>;
		using FMW2 = FunctionModWrapper<Component, P2s ...>;
		FMW1 mDoFunc;
		FMW2 mUnDoFunc;
		uint64_t mID;
	};

	template<typename ... P1s, typename ... P2s >
	struct ComdModifyComponent<FunctionModWrapper<GameObject, P1s ...>, FunctionModWrapper<GameObject, P2s ...>> : Commands
	{
		ComdModifyComponent(const uint64_t& _objID,
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
		using FMW1 = FunctionModWrapper<GameObject, P1s ...>;
		using FMW2 = FunctionModWrapper<GameObject, P2s ...>;
		FMW1 mDoFunc;
		FMW2 mUnDoFunc;
		uint64_t mID;
	};
}

#endif //_COMMAND_LIST_H_
#endif //EDITOR
