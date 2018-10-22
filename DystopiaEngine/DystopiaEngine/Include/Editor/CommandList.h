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
#include "Math/Vector4.h"
#include "Component/Component.h"
#include "System/Base/Systems.h"
#include "Utility/Meta.h"

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
		virtual GameObject* RetrieveGameObject();
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
		GameObject* RetrieveGameObject();

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
		GameObject* RetrieveGameObject();

	private:
		bool *mpNotify;
		bool mFocusBack;
		uint64_t mObjID;
		GameObject *mpObj;
		Scene *mpScene;
	};

	struct ComdBatch : Commands
	{
		ComdBatch(AutoArray<Commands*>&& _arrComds);
		~ComdBatch();
		bool ExecuteDo() override;
		bool ExecuteUndo() override;
		bool Unchanged() const;

	private:
		AutoArray<Commands*> mArrCommands;
	};

	template<typename T, class ...>
	struct ComdModifyValue;

	template <typename T, class Comp>
	struct ComdModifyValue<T, Comp, Ut::Type_t<Ut::EnableIf_t<std::is_base_of<Component, Comp>::value>>>
		: Commands
	{
		ComdModifyValue(const uint64_t& _objID, T Comp::* _pmData, const T& _oldV, bool Comp::* _notify = nullptr)
			: mID{ _objID }, mpData{ _pmData }, 
			mNewValue{ }, mOldValue{ _oldV }, mpNotify{ _notify }
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			Comp *pCom = pObj->GetComponent<Comp>();
			mNewValue = pCom->*_pmData;
		}

		bool ExecuteDo() override 
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;
			Comp *pCom = pObj->GetComponent<Comp>();
			if (!pCom) return false;

			if (mpNotify) pCom->*mpNotify = true;
			pCom->*mpData = mNewValue;
			return true; 
		}

		bool ExecuteUndo() override 
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;
			Comp *pCom = pObj->GetComponent<Comp>();
			if (!pCom) return false;

			if (mpNotify) pCom->*mpNotify = true;
			pCom->*mpData = mOldValue;
			return true;
		}

		bool Unchanged() const { return mOldValue == mNewValue; }
	private:
		bool Comp::* mpNotify;
		uint64_t mID;
		T Comp::* mpData;
		T mOldValue;
		T mNewValue;
	}; 

	template <typename T, class Sys>
	struct ComdModifyValue<T, Sys, Ut::Type_t<Ut::EnableIf_t<std::is_base_of<Systems, Sys>::value>>>
		: Commands
	{
		ComdModifyValue(const uint64_t&, T Sys::* _pData, const T& _oldV, bool Sys::* _notify = nullptr)
			: mpData{ _pData }, mOldValue{ _oldV }, mNewValue{ }, mpNotify{ _notify }
		{
			Sys *pSystem = EngineCore::GetInstance()->GetSystem<Sys>();
			mNewValue = pSystem->*_pData;
		}

		bool ExecuteDo() override
		{
			Sys *pSystem = EngineCore::GetInstance()->GetSystem<Sys>();
			if (!pSystem) return false;

			if (mpNotify) pSystem->*mpNotify = true;
			pSystem->*mpData = mNewValue;
			return true;
		}

		bool ExecuteUndo() override
		{
			Sys *pSystem = EngineCore::GetInstance()->GetSystem<Sys>();
			if (!pSystem) return false;

			if (mpNotify) pSystem->*mpNotify = true;
			pSystem->*mpData = mOldValue;
			return true;
		}

		bool Unchanged() const { return mOldValue == mNewValue; }
	private:
		bool Sys::*mpNotify;
		T Sys::* mpData;
		T mOldValue;
		T mNewValue;
	};

	template <typename T>
	struct ComdModifyValue<T, GameObject, GameObject> : Commands
	{
		ComdModifyValue(const uint64_t& _objID, T GameObject::* _pmData, const T& _oldV, bool GameObject::* _notify = nullptr)
			: mID{ _objID }, mpData{ _pmData },
			mNewValue{ }, mOldValue{ _oldV }, mpNotify{ _notify }
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID); 
			mNewValue = pObj->*mpData;
		}

		bool ExecuteDo() override
		{
			GmaeObject *p = Editor::GetInstance()->FindGameObject(mID);
			if (!p) return false;
			if (mpNotify) p->*mpNotify = true;
			p->*mpData = mNewValue;
			return true;
		}

		bool ExecuteUndo() override
		{
			GmaeObject *p = Editor::GetInstance()->FindGameObject(mID);
			if (!p) return false;
			if (mpNotify) p->*mpNotify = true;
			p->*mpData = mOldValue;
			return true;
		}

		bool Unchanged() const { return mOldValue == mNewValue; }
	private:
		bool GameObject::* mpNotify;
		uint64_t mID;
		T GameObject::* mpData;
		T mOldValue;
		T mNewValue;
	};

	template <typename T, class Comp, typename = void>
	struct ComdRecord;

	template <typename T, class Comp>
	struct ComdRecord<T, Comp, Ut::Type_t<Ut::EnableIf_t<std::is_base_of_v<::Dystopia::Component, Comp>>>>
		: RecordBase
	{
		ComdRecord(const uint64_t& _objID, T Comp::* rhs, bool Comp::* _notify = nullptr)
			: mpTarget{ rhs }, mOldValue{}, 
			mNewValue{}, mID{ _objID }, mpNotify{ _notify }
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			Comp *pCom = pObj->GetComponent<Comp>();
			mNewValue = mOldValue = pCom->*rhs;
		}

		bool EndRecord()
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;
			Comp *pCom = pObj->GetComponent<Comp>();
			if (!pCom) return false;

			mNewValue = pCom->*mpTarget;
			return true;
		}

		bool ExecuteDo() override	
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;
			Comp *pCom = pObj->GetComponent<Comp>();
			if (!pCom) return false;
			if (mpNotify) pCom->*mpNotify = true;
			pCom->*mpTarget = mNewValue;
			return true; 
		}

		bool ExecuteUndo() override	
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;
			Comp *pCom = pObj->GetComponent<Comp>();
			if (!pCom) return false;
			if (mpNotify) pCom->*mpNotify = true;
			pCom->*mpTarget = mOldValue;
			return true; 
		}

		bool Unchanged() const 
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return true;
			Comp *pCom = pObj->GetComponent<Comp>();
			if (!pCom) return true;
			return pCom ->*mpTarget == mOldValue;
		}
		T Comp::* GetPointer() { return mpTarget; }
	private:
		bool Comp::* mpNotify;
		uint64_t mID;
		T Comp::* mpTarget;
		T mOldValue;
		T mNewValue;
	};

	template <typename T, class Sys>
	struct ComdRecord<T, Sys, Ut::Type_t<Ut::EnableIf_t<std::is_base_of<Systems, Sys>::value>>>
		: RecordBase
	{
		ComdRecord(const uint64_t&, T Sys::* _pData, bool Sys::* _notify = nullptr)
			: mpTarget{ _pData }, mOldValue{},
			mNewValue{}, mpNotify{ _notify }
		{
			Sys *pSystem = EngineCore::GetInstance()->GetSystem<Sys>();
			mNewValue = mOldValue = pSystem->*_pData;
		}
	
		bool EndRecord()
		{
			Sys *pSystem = EngineCore::GetInstance()->GetSystem<Sys>();
			if (!pSystem) return false;
	
			mNewValue = pSystem ->*mpTarget;
			return true;
		}
	
		bool ExecuteDo() override
		{
			Sys *pSystem = EngineCore::GetInstance()->GetSystem<Sys>();
			if (!pSystem) return false;
	
			if (mpNotify) pSystem->*mpNotify = true;
			pSystem->*mpTarget = mNewValue;
			return true;
		}
	
		bool ExecuteUndo() override
		{
			Sys *pSystem = EngineCore::GetInstance()->GetSystem<Sys>();
			if (!pSystem) return false;
	
			if (mpNotify) pSystem->*mpNotify = true;
			pSystem->*mpTarget = mOldValue;
			return true;
		}
	
		bool Unchanged() const 
		{
			Sys *pSystem = EngineCore::GetInstance()->GetSystem<Sys>();
			if (!pSystem) return true;
			return pSystem ->*mpTarget == mOldValue;
		}
		T Sys::* GetPointer() { return mpTarget; }
	private:
		bool Sys::* mpNotify;
		T Sys::* mpTarget;
		T mOldValue;
		T mNewValue;
	};

	template <typename T>
	struct ComdRecord<T, GameObject, void> : RecordBase
	{
		ComdRecord(const uint64_t& _objID, T GameObject::* rhs, bool GameObject::* _notify = nullptr)
			: mpTarget{ rhs }, mOldValue{}, 
			mNewValue{ mOldValue }, mID{ _objID }, mpNotify{ _notify }
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			mNewValue = pObj->*rhs;
		}

		bool EndRecord()
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;

			mNewValue = pObj->*mpTarget;
			return true;
		}

		bool ExecuteDo() override
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;

			if (mpNotify) pObj->*mpNotify = true;
			pObj->*mpTarget = mNewValue;
			return true;
		}

		bool ExecuteUndo() override
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return false;

			if (mpNotify) pObj->*mpNotify = true;
			pObj->*mpTarget = mOldValue;
			return true;
		}

		bool	Unchanged() const 
		{
			GameObject *pObj = Editor::GetInstance()->FindGameObject(mID);
			if (!pObj) return true;
			return pObj ->*mpTarget == mOldValue;
		}
		T GameObject::*	 GetPointer() { return mpTarget; }
	private:
		bool GameObject::* mpNotify;
		uint64_t mID;
		T GameObject::* mpTarget;
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
