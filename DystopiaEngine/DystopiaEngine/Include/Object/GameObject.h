/* HEADER *********************************************************************************/
/*!
\file	GameObject.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Base class for all GameObjects.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GAMEOBJ_H_
#define _GAMEOBJ_H_

#include "System\Driver\Driver.h"
#include "DataStructure\AutoArray.h"		// AutoArray
#include "Component\Component.h"			// Component
#include "Component\ComponentList.h"		// AllComponents
#include "Component\Transform.h"			// Transform
#include "Behaviour\Behaviour.h"			// Behaviour
#include "Utility\MetaAlgorithms.h"			// MetaFind_t
#include "Globals.h"

#include <string>

namespace Dystopia
{
	struct CollisionEvent;

	class _DLL_EXPORT GameObject
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		GameObject(void) noexcept;
		explicit GameObject(unsigned long _ID) noexcept;
		GameObject(GameObject&&) noexcept;

		~GameObject(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		bool IsActive(void) const;
		void SetActive(const bool _bEnable);

		void Load(void);
		void Init(void);

		void Update(const float _fDeltaTime);
		void FixedUpdate(const float _fFixedDT);
		void PostUpdate(void);

		void Destroy(void);
		void Unload(void);

		void OnCollisionEnter(const CollisionEvent&);
		void OnCollisionStay (const CollisionEvent&);
		void OnCollisionExit (const CollisionEvent&);

		template <typename T>
		void AddComponent();
		void AddComponent(Component*, ComponentTag);
		void AddComponent(Behaviour*, BehaviourTag);
		template <typename T>
		void RemoveComponent();
		void RemoveComponent(Component*);

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		// Creates an exact copy of the Game Object
		GameObject* Duplicate(void) const; 
		
		void SetID(const uint64_t&); //explicit purposes only
		uint64_t GetID(void) const;
		inline unsigned GetFlags(void) const;
		std::string GetName(void) const;
		void SetName(const std::string&);

		template<class T>
		T* GetComponent(void) const;

		// DOESNT WORK YET
		template<class T>
		AutoArray<T*> GetComponents(void) const;

		inline const AutoArray<Component*>& GetAllComponents() const;
		inline const AutoArray<Behaviour*>& GetAllBehaviours() const;


		// ======================================== OPERATORS ======================================== // 

		GameObject& operator = (GameObject&&);
	private:
		uint64_t mnID;
		Transform mTransform;

		unsigned mnFlags;
		std::string mName;

		AutoArray<Component*> mComponents;
		AutoArray<Behaviour*> mBehaviours;

		GameObject(const GameObject&) = delete;

		template <typename> inline void AddComponent(BehaviourTag);
		template <typename> inline void AddComponent(ComponentTag);
		template <typename> inline void RemoveComponent(BehaviourTag);
		template <typename> inline void RemoveComponent(ComponentTag);

		template <typename T> T* GetComponent(ComponentTag) const;
		template <typename T> T* GetComponent(BehaviourTag) const;

		void PurgeComponents(void);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 



inline unsigned Dystopia::GameObject::GetFlags(void) const
{
	return mnFlags;
}

template <typename T>
inline void Dystopia::GameObject::AddComponent()
{
	AddComponent<T>(typename T::TAG{});
}

template <typename Ty>
inline void Dystopia::GameObject::AddComponent(ComponentTag)
{
	//GUID_t get = EngineCore::GetInstance()->GetSystem<typename Ty::SYSTEM>()->RequestComponent();

	//mComponents.EmplaceBack(get, 
	//	Utility::MetaFind_t<Ty, AllComponents>::value, 
	//	Utility::MetaFind_t<typename Ty::SYSTEM, EngineCore::AllSys>::value
	//);

	auto Comp = static_cast<ComponentDonor<Ty>*>(
		EngineCore::GetInstance()->GetSystem<typename Ty::SYSTEM>()
		)->RequestComponent();

	mComponents.Insert(Comp);

	Comp->SetOwner(this);
	Comp->Init();
}

template <typename Ty>
inline void Dystopia::GameObject::AddComponent(BehaviourTag)
{
	mBehaviours.push_back(new Ty{});
	mBehaviours.back()->SetOwner(this);
	mBehaviours.back()->Init();
}

template <typename T>
inline void Dystopia::GameObject::RemoveComponent()
{
	RemoveComponent<T>(typename T::TAG{});
}

template <typename T>
T* Dystopia::GameObject::GetComponent(void) const
{
	return GetComponent<T>(typename T::TAG{});
}

template <>
inline Dystopia::Transform* Dystopia::GameObject::GetComponent<Dystopia::Transform>(void) const
{
	return const_cast<Transform*>(&mTransform);
}

template <typename T>
T* Dystopia::GameObject::GetComponent(ComponentTag) const
{
	for (auto& e : mComponents)
	{
		//if (Utility::MetaFind_t<T, AllComponents>::value == e.mComponent)
		//{
		//	return EngineCore::GetInstance()->GetSystem<typename T::SYSTEM>()->GetComponent(e.mID);
		//}

		if (Utility::MetaFind_t<T, AllComponents>::value == e->GetComponentType())
		{
			return static_cast<T*>(e);
		}
	}

	return nullptr;
}

template <typename T>
T* Dystopia::GameObject::GetComponent(BehaviourTag) const
{
	for (auto& e : mBehaviours)
	{
		if (Utility::MetaFind_t<T, AllBehaviours>::value == e->GetComponentType())
		{
			return static_cast<T*>(e);
		}
	}

	return nullptr;
}

template <typename T>
AutoArray<T*> Dystopia::GameObject::GetComponents(void) const
{
	/*AutoArray<T*> temp{};
	for (Component * e : mComponents)
	{
		if (T::TYPE == e->GetComponentType())
		{
			temp.Insert(static_cast<T*>(e));
		}
	}

	return temp;*/
	return AutoArray<T*>{};
}

inline const AutoArray<Dystopia::Component*>& Dystopia::GameObject::GetAllComponents() const
{
	return mComponents;
}

inline const AutoArray<Dystopia::Behaviour*>& Dystopia::GameObject::GetAllBehaviours() const
{
	return mBehaviours;
}


template <typename Ty>
inline void Dystopia::GameObject::RemoveComponent(ComponentTag)
{
	for (unsigned n = 0; n < mComponents.size(); ++n)
	{
		if (Utility::MetaFind_t<Ty, AllComponents>::value == mComponents[n]->GetComponentType())
		{
			mComponents.FastRemove(n);
			break;
		}
	}
}

template <typename Ty>
inline void Dystopia::GameObject::RemoveComponent(BehaviourTag)
{
	for (unsigned n = 0; n < mBehaviours.size(); ++n)
	{
		if (Utility::MetaFind_t<Ty, AllBehaviours>::value == mBehaviours[n]->GetComponentType())
		{
			mBehaviours.FastRemove(n);
			break;
		}
	}
}



#endif		// INCLUDE GUARD

