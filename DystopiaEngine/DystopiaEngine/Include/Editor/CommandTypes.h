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
#include "Editor/EditorResource.h"
#include "Editor/EditorFactory.h"

#include "Component/Component.h"
#include "Behaviour/Behaviour.h"

#include "System/Driver/Driver.h"
#include "System/Base/Systems.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"

#include "Object/GameObject.h"

#include "Utility/Meta.h"

#include "DataStructure/MagicArray.h"
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
		ModifyValue(const uint64_t& _id, T C::* _ptrToMemVar, const T& _oldVal, const T& _newVal, bool C::* _ptrChangesNotify = nullptr)
			: mnID{ _id }, mpMemVar{ _ptrToMemVar }, mOldV{ _oldVal }, mNewV{ _newVal }, mpNotify{ _ptrChangesNotify }
		{}

		~ModifyValue(void) {}

		bool Do(void)
		{
			auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
			if (auto obj = curScene.FindGameObject(mnID))
			{
				obj->GetComponent<C>()->*mpMemVar = mNewV;
				if (mpNotify)
					obj->GetComponent<C>()->*mpNotify = true;
				return true;
			}
			return false;
		}

		bool Undo(void)
		{
			auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
			if (auto obj = curScene.FindGameObject(mnID))
			{
				obj->GetComponent<C>()->*mpMemVar = mOldV;
				if (mpNotify)
					obj->GetComponent<C>()->*mpNotify = true;
				return true;
			}
			return false;
		}

		bool Unchanged(void) const
		{
			return mOldV == mNewV;
		}

	private:
		uint64_t mnID;
		T C::* mpMemVar;
		T mOldV;
		T mNewV;
		bool C::* mpNotify;
	};

	template<class S, typename T>
	class ModifyValue<S, T, Ut::Type_t<Ut::EnableIf_t<std::is_base_of<Dystopia::Systems, S>::value>>> : public Command
	{
	public:
		ModifyValue(T S::* _ptrToMemVar, const T& _oldVal, const T& _newVal, bool S::* _ptrChangesNotify = nullptr)
			: mpMemVar{ _ptrToMemVar }, mOldV{ _oldVal }, mNewV{ _newVal }, mpNotify{ _ptrChangesNotify }
		{}

		~ModifyValue(void) {}

		bool Do(void)
		{
			if (auto sys = Dystopia::EngineCore::GetInstance()->GetSystem<S>())
			{
				sys->*mpMemVar = mNewV;
				if (mpNotify)
					sys->*mpNotify = true;
				return true;
			}
			return false;
		}

		bool Undo(void)
		{
			if (auto sys = Dystopia::EngineCore::GetInstance()->GetSystem<S>())
			{
				sys->*mpMemVar = mOldV;
				if (mpNotify)
					sys->*mpNotify = true;
				return true;
			}
			return false;
		}

		bool Unchanged(void) const
		{
			return mOldV == mNewV;
		}

	private:
		T S::* mpMemVar;
		T mOldV;
		T mNewV;
		bool S::* mpNotify;
	};

	template<typename T>
	class ModifyValue<Dystopia::GameObject, T, Dystopia::GameObject> : public Command
	{
	public:
		ModifyValue(const uint64_t& _id, T Dystopia::GameObject::* _ptrToMemVar, const T& _oldVal, const T& _newVal, bool Dystopia::GameObject::* _ptrChangesNotify = nullptr)
			: mnID{ _id }, mpMemVar { _ptrToMemVar }, mOldV{ _oldVal }, mNewV{ _newVal }, mpNotify{ _ptrChangesNotify }
		{}

		~ModifyValue(void) {}

		bool Do(void)
		{
			auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
			if (auto obj = curScene.FindGameObject(mnID))
			{
				obj->*mpMemVar = mNewV;
				if (mpNotify)
					obj->*mpNotify = true;
				return true;
			}
			return false;
		}

		bool Undo(void)
		{
			auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
			if (auto obj = curScene.FindGameObject(mnID))
			{
				obj->*mpMemVar = mOldV;
				if (mpNotify)
					obj->*mpNotify = true;
				return true;
			}
			return false;
		}

		bool Unchanged(void) const
		{
			return mOldV == mNewV;
		}

	private:
		uint64_t mnID;
		T Dystopia::GameObject::* mpMemVar;
		T mOldV;
		T mNewV;
		bool Dystopia::GameObject::* mpNotify;
	};

	class InsertGameObject : public Command
	{
	public:
		InsertGameObject(uint64_t _id, const Math::Pt3D& = Math::Pt3D{ 0,0,0 });

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		uint64_t mnObjID;
		Math::Pt3D mSpawnPt;
	};

	class DeleteGameObject : public Command
	{
	public:
		DeleteGameObject(uint64_t _id);

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;
		~DeleteGameObject(void);

	private:
		uint64_t mnObjID;
		uint64_t mnTempFileID;
	};

	class BatchExecute : public Command
	{
	public:
		BatchExecute(AutoArray<Command*>& _arr);
		~BatchExecute(void);

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		AutoArray<Command*> mArrCommands;
	};

	class SpawnPrefab : public Command
	{
	public:
		SpawnPrefab(const HashString& _prefab, const Math::Pt3D& = Math::Pt3D{ 0,0,0,1 });

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		uint64_t mnID;
		HashString mPrefab;
		Math::Pt3D mSpawnPt;
	};

	class DuplicateGameObject : public Command
	{
	public:
		DuplicateGameObject(uint64_t _id);
		~DuplicateGameObject(void);

		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;

	private:
		uint64_t mnObjID;
		uint64_t mnDupedID;
	};

	template<typename C>
	class AddComponent : public Command
	{
	public:
		AddComponent(const uint64_t& _ownerID, C* _pComToAdd);
		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;
		~AddComponent(void);

	private:
		uint64_t mnID;
		uint64_t mnCID;
	};

	template<typename C>
	class RemoveComponent : public Command
	{
	public:
		RemoveComponent(const uint64_t& _ownerID, C* _pComToAdd);
		bool Do(void);
		bool Undo(void);
		bool Unchanged(void) const;
		~RemoveComponent(void);

	private:
		uint64_t mnID;
		uint64_t mnCID;
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
		
		std::tuple<std::remove_const_t<std::remove_reference_t<Ts>>...> mTupleParams;

	private:
		void (C::* mpComponentFn)(Ts...);
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

	class Recordings
	{
	public:
		virtual void GetValue(void*) = 0;
		~Recordings(void){}
	};

	template<typename ... Ts>
	class RecordValue;

	template<typename T>
	class RecordValue<T> : public Recordings
	{
	public:
		RecordValue(T _val);
		~RecordValue(void);
		void GetValue(void*);

	private:
		T mStoredValue;
	}; 

	template<class C, typename ... Ts>
	class RecordValue<ComponentFunction<C, Ts...>> : public Recordings
	{
	public:
		RecordValue(const ComponentFunction<C, Ts ...>& _val);
		~RecordValue(void);
		void GetValue(void*);

	private:
		ComponentFunction<C, Ts ...> mStoredValue;
	};
}

/***************************************************************** Template Function Definitions *****************************************************************/

template<class C, typename ... Ts>
::Editor::ComponentFunction<C, Ts...>::ComponentFunction(void(C::*_fnptr)(Ts ...), std::remove_reference_t<Ts> ... params)
	: mpComponentFn{ _fnptr }, mTupleParams{ params... }, mAuxCaller{ *this }
{}

template<class C, typename ... Ts>
::Editor::ComponentFunction<C, Ts...>::ComponentFunction(const ::Editor::ComponentFunction<C, Ts...>& _rhs)
	: mpComponentFn{ _rhs.mpComponentFn }, mTupleParams{ _rhs.mTupleParams }, mAuxCaller{ *this }
{}

template<class C, typename ... Ts>
void ::Editor::ComponentFunction<C, Ts...>::Execute(C * const _toMod) const
{
	mAuxCaller.Execute(_toMod);
}

template<class C, typename ... Ts>
::Editor::ComponentFunction<C, Ts...>& ::Editor::ComponentFunction<C, Ts...>::operator=(const ::Editor::ComponentFunction<C, Ts...>& _rhs)
{
	mpComponentFn = _rhs.mpComponentFn;
	mTupleParams = _rhs.mTupleParams;
	return *this;
}

template<class C, typename ... Ts>
template<size_t ... Ns>
::Editor::ComponentFunction<C, Ts...>::AuxEx<std::index_sequence<Ns ...>>::AuxEx(ComponentFunction<C, Ts...>& _p)
	: mParent{ _p }
{}

template<class C, typename ... Ts>
template<size_t ... Ns>
void ::Editor::ComponentFunction<C, Ts...>::AuxEx<std::index_sequence<Ns ...>>::Execute(C * const _toMod) const
{
	(_toMod->*(mParent.mpComponentFn))(std::get<Ns>(mParent.mTupleParams) ...);
}

template<class C, typename ... T1s, typename ... T2s >
::Editor::FunctionComd<::Editor::ComponentFunction<C, T1s...>, 
					 ::Editor::ComponentFunction<C, T2s...>>::
	FunctionComd(const uint64_t& _objID, const ComponentFunction<C, T1s ...>& _do, const ComponentFunction<C, T2s ...>& _undo)
	: mUnDoFunc{ _undo }, mDoFunc{ _do }, mID{ _objID }
{}

template<class C, typename ... T1s, typename ... T2s >
bool ::Editor::FunctionComd<::Editor::ComponentFunction<C, T1s...>,
						  ::Editor::ComponentFunction<C, T2s...>>::Do(void)
{
	if (Unchanged())
		return false;

	Dystopia::GameObject* pObj = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mID);
	if (pObj)
	{
		C* pComponent = pObj->GetComponent<C>();
		if (pComponent)
		{
			mDoFunc.Execute(pComponent);
			return true;
		}
		return false;
	}
	return false;
}

template<class C, typename ... T1s, typename ... T2s >
bool ::Editor::FunctionComd<::Editor::ComponentFunction<C, T1s...>,
						  ::Editor::ComponentFunction<C, T2s...>>::Undo(void)
{
	if (Unchanged())
		return false;

	Dystopia::GameObject* pObj = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mID);
	if (pObj)
	{
		C* pComponent = pObj->GetComponent<C>();
		if (pComponent)
		{
			mUnDoFunc.Execute(pComponent);
			return true;
		}
		return false;
	}
	return false;
}

template<class C, typename ... T1s, typename ... T2s >
bool ::Editor::FunctionComd<::Editor::ComponentFunction<C, T1s...>,
						  ::Editor::ComponentFunction<C, T2s...>>::Unchanged(void) const
{
	return mDoFunc.mTupleParams == mUnDoFunc.mTupleParams;
}

template<typename ... T1s, typename ... T2s >
::Editor::FunctionComd<::Editor::ComponentFunction<Dystopia::GameObject, T1s...>,
					 ::Editor::ComponentFunction<Dystopia::GameObject, T2s...>>::
	FunctionComd(const uint64_t& _objID, const ComponentFunction<Dystopia::GameObject, T1s ...>& _do, const ComponentFunction<Dystopia::GameObject, T2s ...>& _undo)
	: mUnDoFunc{ _undo }, mDoFunc{ _do }, mID{ _objID }
{}

template<typename ... T1s, typename ... T2s >
bool ::Editor::FunctionComd<::Editor::ComponentFunction<Dystopia::GameObject, T1s...>,
						  ::Editor::ComponentFunction<Dystopia::GameObject, T2s...>>::Do(void)
{
	if (Unchanged())
		return false;

	Dystopia::GameObject* pObj = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mID);
	if (pObj)
	{
		mDoFunc.Execute(pObj);
		return true;
	}
	return false;
}

template<typename ... T1s, typename ... T2s >
bool ::Editor::FunctionComd<::Editor::ComponentFunction<Dystopia::GameObject, T1s...>,
						  ::Editor::ComponentFunction<Dystopia::GameObject, T2s...>>::Undo(void)
{
	if (Unchanged())
		return false;

	Dystopia::GameObject* pObj = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mID);
	if (pObj)
	{
		mUnDoFunc.Execute(pObj);
		return true;
	}
	return false;
}

template<typename ... T1s, typename ... T2s >
bool ::Editor::FunctionComd<::Editor::ComponentFunction<Dystopia::GameObject, T1s...>,
						  ::Editor::ComponentFunction<Dystopia::GameObject, T2s...>>::Unchanged(void) const
{
	return mDoFunc.mTupleParams == mUnDoFunc.mTupleParams;;
}

template<typename T>
::Editor::RecordValue<T>::RecordValue(T _val)
	: mStoredValue{ _val }
{}

template<typename T>
::Editor::RecordValue<T>::~RecordValue(void)
{}

template<typename T>
void ::Editor::RecordValue<T>::GetValue(void* _outPtr)
{
	*(static_cast<T*>(_outPtr)) = mStoredValue;
}

template<class C, typename ... Ts>
::Editor::RecordValue<::Editor::ComponentFunction<C, Ts...>>::RecordValue(const ::Editor::ComponentFunction<C, Ts ...>& _val)
	: mStoredValue{ _val }
{}

template<class C, typename ... Ts>
::Editor::RecordValue<::Editor::ComponentFunction<C, Ts...>>::~RecordValue(void)
{}

template<class C, typename ... Ts>
void ::Editor::RecordValue<::Editor::ComponentFunction<C, Ts...>>::GetValue(void* _outPtr)
{
	*(static_cast<::Editor::ComponentFunction<C, Ts ...>*>(_outPtr)) = mStoredValue;
}

template<typename C>
::Editor::AddComponent<C>::AddComponent(const uint64_t& _ownerID, C* _pComToAdd)
	: mnID{ _ownerID }, mnCID{ _pComToAdd->GetID() }
{
	EditorMain::GetInstance()->GetSystem<EditorResource>()->AddComponent(_pComToAdd, _ownerID, Ut::IsSame<typename C::TAG, typename Dystopia::Component::TAG>::value);
}

template<typename C>
::Editor::AddComponent<C>::~AddComponent(void)
{
	EditorMain::GetInstance()->GetSystem<EditorResource>()->RemoveComponent(mnCID);
}

template<typename C>
bool ::Editor::AddComponent<C>::Do(void)
{
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	if (auto obj = curScene.FindGameObject(mnID))
	{
		if (auto c = static_cast<C*>(EditorMain::GetInstance()->GetSystem<EditorResource>()->GetComponent(mnCID)))
		{
			EditorMain::GetInstance()->GetSystem<EditorResource>()->RemoveComponent(mnCID);
			obj->AddComponent(c, typename Dystopia::Component::TAG{});
			c->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			c->SetOwner(obj);
			c->SetActive(obj->IsActive());
			EditorMain::GetInstance()->GetSystem<EditorFactory>()->DettachPrefab(mnID);
			return true;
		}
		return false;
	}
	return false;
}

template<typename C>
bool ::Editor::AddComponent<C>::Undo(void)
{
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	if (auto obj = curScene.FindGameObject(mnID))
	{
		if (obj->GetComponent<C>())
		{
			auto c = obj->GetComponent<C>()->Duplicate();
			mnCID = c->GetID();
			c->SetFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			c->SetOwner(obj);
			c->SetActive(obj->IsActive());
			EditorMain::GetInstance()->GetSystem<EditorResource>()->AddComponent(c, mnID, Ut::IsSame<typename C::TAG, typename Dystopia::Component::TAG>::value);
			obj->RemoveComponent(obj->GetComponent<C>());
			return true;
		}
		return false;
	}
	return false;
}

template<typename C>
bool ::Editor::AddComponent<C>::Unchanged(void) const
{
	return false;
}

template<typename C>
::Editor::RemoveComponent<C>::RemoveComponent(const uint64_t& _ownerID, C* _pComToAdd)
	: mnID{ _ownerID }, mnCID{ _pComToAdd->GetID() }
{
	//EditorMain::GetInstance()->GetSystem<EditorResource>()->AddComponent(_pComToAdd);
}

template<typename C>
::Editor::RemoveComponent<C>::~RemoveComponent(void)
{
	EditorMain::GetInstance()->GetSystem<EditorResource>()->RemoveComponent(mnCID);
}

template<typename C>
bool ::Editor::RemoveComponent<C>::Do(void)
{
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	if (auto obj = curScene.FindGameObject(mnID))
	{
		if (obj->GetComponent<C>())
		{
			auto c = obj->GetComponent<C>()->Duplicate();
			mnCID = c->GetID();
			c->SetFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			c->SetOwner(obj);
			c->SetActive(true);
			EditorMain::GetInstance()->GetSystem<EditorResource>()->AddComponent(c, mnID, Ut::IsSame<typename C::TAG, typename Dystopia::Component::TAG>::value);
			obj->RemoveComponent(obj->GetComponent<C>());
			EditorMain::GetInstance()->GetSystem<EditorFactory>()->DettachPrefab(mnID);
			return true;
		}
		return false;
	}
	return false;
}

template<typename C>
bool ::Editor::RemoveComponent<C>::Undo(void)
{
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	if (auto obj = curScene.FindGameObject(mnID))
	{
		if (auto c = static_cast<C*>(EditorMain::GetInstance()->GetSystem<EditorResource>()->GetComponent(mnCID)))
		{
			EditorMain::GetInstance()->GetSystem<EditorResource>()->RemoveComponent(mnCID);
			obj->AddComponent(c, typename Dystopia::Component::TAG{});
			c->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			c->SetOwner(obj);
			c->SetActive(obj->IsActive());
			c->Awake();
			return true;
		}
	}
	return false;
}

template<typename C>
bool ::Editor::RemoveComponent<C>::Unchanged(void) const
{
	return false;
}



#endif
#endif












