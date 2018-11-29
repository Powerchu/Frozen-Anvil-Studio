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

#pragma warning(push)
#pragma warning(disable : 4251)

#include "System/Driver/Driver.h"
#include "DataStructure/AutoArray.h"		// AutoArray
#include "Component/ComponentList.h"		// AllComponents
#include "Component/Transform.h"			// Transform
#include "Utility/MetaAlgorithms.h"			// MetaFind_t
#include "IO/TextSerialiser.h"
#include "Object/ObjectFlags.h"
#include "Globals.h"
#include "System/Tag/Tags.h"

#include <string>


namespace Dystopia
{
	class Component;
	class Behaviour;
	struct CollisionEvent;

	class _DLL_EXPORT GameObject
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		GameObject(void) noexcept;
		explicit GameObject(uint64_t _ID) noexcept;
		GameObject(GameObject&&) noexcept;

		~GameObject(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		bool IsActive(void) const;
		void SetActive(const bool _bEnable);
		bool IsStatic() const;
		void SetStatic(bool _bEnable);

		void Load(void);
		void Awake(void);
		void Init(void);

		void Update(const float _fDeltaTime);
		void FixedUpdate(const float _fFixedDT);
		void PostUpdate(void);

		void Destroy(void);
		void Unload(void);

		void OnCollisionEnter(const CollisionEvent&);
		void OnCollisionStay (const CollisionEvent&);
		void OnCollisionExit (const CollisionEvent&);

		void OnTriggerEnter(GameObject* const);
		void OnTriggerStay (GameObject* const);
		void OnTriggerExit (GameObject* const);

		template <typename T>
		void AddComponent(void);
		void AddComponent(Component*, ComponentTag);
		void AddComponent(Behaviour*, BehaviourTag);
		template <typename T>
		void RemoveComponent(void);
		void RemoveComponent(Component*);

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		// Creates an exact copy of the Game Object
		GameObject* Duplicate(void) const; 

		// Tells all components/behaviours > I am their owner
		void Identify(void);
		
		void SetID(const uint64_t&); //explicit purposes only
		uint64_t GetID(void) const;
		inline unsigned GetFlags(void) const;
		HashString GetName(void) const;
		const char * GetNamePtr(void) const;

		void SetName(const HashString&);

		void RemoveFlags(eObjFlag);
		void SetFlag(eObjFlag);
		void ReplaceFlag(unsigned);
		unsigned GetFlag() const;

		unsigned GetTags() const;
		AutoArray<Tags>        GetAllTags()         const;
		AutoArray<HashString>  GetAllTags_Hashstr() const;
		AutoArray<std::string> GetAllTags_str()     const;
		void AddTag(HashString const & _TagName);
		void AddTag(Tags _tag);
		void AddTag(std::string const & _TagName);
		void AddTag(const char * _TagName);
		void RemoveTag(Tags _Tag);
		void ClearTags();


		template<class T>
		T* GetComponent(void) const;

		template<class T>
		inline AutoArray<T*> GetComponents(void) const;

		inline const AutoArray<Component*>& GetAllComponents(void) const noexcept;
		inline const AutoArray<Behaviour*>& GetAllBehaviours(void) const noexcept;

		// ======================================== OPERATORS ======================================== // 

#if EDITOR
		GameObject& operator = (GameObject&&);
		GameObject& operator=(const GameObject&);
#endif
	private:
		bool mbIsStatic;					/* Static bodies do not need to be integrated/updated*/

		uint64_t mnID;
		unsigned mTags;
		unsigned mnFlags;
		HashString mName;

		Transform mTransform;

		AutoArray<Component*> mComponents;
		AutoArray<Behaviour*> mBehaviours;


		GameObject(const GameObject&) = delete;

		template <typename> inline void AddComponent(BehaviourTag);
		template <typename> inline void AddComponent(ComponentTag);
		template <typename> inline void RemoveComponent(BehaviourTag);
		template <typename> inline void RemoveComponent(ComponentTag);

		template <typename T> T* GetComponent(ComponentTag) const;
		template <typename T> T* GetComponent(BehaviourTag) const;

		template <typename T> AutoArray<T*> GetComponents(ComponentTag) const;
		template <typename T> AutoArray<T*> GetComponents(BehaviourTag) const;

		void PurgeComponents(void);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 



inline unsigned Dystopia::GameObject::GetFlags(void) const
{
	return mnFlags;
}

template <typename T>
inline void Dystopia::GameObject::AddComponent(void)
{
	AddComponent<T>(typename T::TAG{});
}

template <typename Ty>
inline void Dystopia::GameObject::AddComponent(ComponentTag)
{
	auto Comp = static_cast<ComponentDonor<Ty>*>(
		EngineCore::GetInstance()->Get<typename Ty::SYSTEM>()
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
inline void Dystopia::GameObject::RemoveComponent(void)
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
		if constexpr (Ut::MetaFind<T, AllComponents>::value)
		{
			if (Ut::MetaFind_t<T, AllComponents>::value == e->GetComponentType())
			{
				return static_cast<T*>(e);
			}
		}
		else
		{
			if (Ut::MetaFind_t<T, UsableComponents>::value == e->GetRealComponentType())
			{
				return static_cast<T*>(e);
			}
		}
	}

	return nullptr;
}

template <typename T>
T* Dystopia::GameObject::GetComponent(BehaviourTag) const
{
	for (auto& e : mBehaviours)
	{
		if (Ut::MetaFind_t<T, AllBehaviours>::value == e->GetComponentType())
		{
			return static_cast<T*>(e);
		}
	}

	return nullptr;
}

template <typename T>
inline AutoArray<T*> Dystopia::GameObject::GetComponents(void) const
{
	return GetComponents<T>(T::TAG{});
}

template <typename T>
AutoArray<T*> Dystopia::GameObject::GetComponents(ComponentTag) const
{
	AutoArray<T*> temp{};
	for (Component* e : mComponents)
	{
		if constexpr (Ut::MetaFind<T, AllComponents>::value)
		{
			if (Ut::MetaFind_t<T, AllComponents>::value == e->GetComponentType())
			{
				temp.Insert(static_cast<T*>(e));
			}
		}
		else
		{
			if (Ut::MetaFind_t<T, UsableComponents>::value == e->GetRealComponentType())
			{
				temp.Insert(static_cast<T*>(e));
			}
		}
	}

	return temp;
}

template<typename T>
inline AutoArray<T*> Dystopia::GameObject::GetComponents(BehaviourTag) const
{
	static_assert(false, "Function " __FUNCSIG__ " not implemented yet!");
	return AutoArray<T*>{};
}

inline const AutoArray<Dystopia::Component*>& Dystopia::GameObject::GetAllComponents(void) const noexcept
{
	return mComponents;
}

inline const AutoArray<Dystopia::Behaviour*>& Dystopia::GameObject::GetAllBehaviours(void) const noexcept
{
	return mBehaviours;
}


template <typename Ty>
inline void Dystopia::GameObject::RemoveComponent(ComponentTag)
{
	for (unsigned n = 0; n < mComponents.size(); ++n)
	{
		if (Ut::MetaFind_t<Ty, AllComponents>::value == mComponents[n]->GetComponentType())
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
		if (Ut::MetaFind_t<Ty, AllBehaviours>::value == mBehaviours[n]->GetComponentType())
		{
			mBehaviours.FastRemove(n);
			break;
		}
	}
}


#pragma warning(pop)
#endif		// INCLUDE GUARD

