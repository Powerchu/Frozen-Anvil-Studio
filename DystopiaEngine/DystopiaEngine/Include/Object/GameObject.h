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

#include "DataStructure\AutoArray.h"		// AutoArray
#include "Component\Component.h"			// Component
#include "Behaviour\Behaviour.h"			// Behaviour
#include "Component\Transform.h"			// Transform
#include "Component\ComponentList.h"		// eComponents

#include <string>

namespace Dystopia
{
	class CollisionEvent;

	class GameObject
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		GameObject(void);
		explicit GameObject(unsigned _ID);
		GameObject(GameObject&&);

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
		void AddComponent(T*);
		template <typename T>
		void RemoveComponent(T*);

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

		// Creates an exact copy of the Game Object
		GameObject* Duplicate(void) const; 

		size_t GetID();
		std::string GetName(void) const;
		void SetName(const std::string&);

		template<class T>
		T* GetComponent(void) const
		{
			for (Component * e : mComponents)
			{
				if (T::TYPE == e->GetComponentType())
				{
					return static_cast<T*>(e);
				}
			}
			return nullptr;
		}

		template<class T>
		AutoArray<T*> GetComponents(void) const
		{
			AutoArray<T*> temp{};
			for (Component * e : mComponents)
			{
				if (T::TYPE == e->GetComponentType())
				{
					temp.Insert(static_cast<T*>(e));
				}
			}

			return temp;
		}


	private:

		size_t mnID;
		unsigned mnFlags;
		std::string mName;

		Transform mTransform;
		AutoArray<Component*> mComponents;
		AutoArray<Behaviour*> mBehaviours;

		GameObject(const GameObject&) = delete;

		inline void AddComponent(Component*, ComponentTag);
		inline void AddComponent(Behaviour*, BehaviourTag);
		inline void RemoveComponent(Component*, ComponentTag);
		inline void RemoveComponent(Behaviour*, BehaviourTag);

		void PurgeComponents(void);
	};
}






	// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T>
inline void Dystopia::GameObject::AddComponent(T* _pComponent)
{
	AddComponent(_pComponent, typename T::TAG{});
}

inline void Dystopia::GameObject::AddComponent(Component* _pComponent, ComponentTag)
{
	mComponents.push_back(_pComponent);
	_pComponent->SetOwner(this);
}

inline void Dystopia::GameObject::AddComponent(Behaviour* _pBehaviour, BehaviourTag)
{
	mBehaviours.push_back(static_cast<Behaviour*>(_pBehaviour));
	_pBehaviour->SetOwner(this);
}

template <typename T>
inline void Dystopia::GameObject::RemoveComponent(T* _pComponent)
{
	RemoveComponent(_pComponent, typename T::TAG{});
}

inline void Dystopia::GameObject::RemoveComponent(Component* _pComponent, ComponentTag)
{
	for (unsigned n = 0; n < mComponents.size(); ++n)
	{
		if (_pComponent == mComponents[n])
		{
			mComponents.FastRemove(n);
			break;
		}
	}
}

inline void Dystopia::GameObject::RemoveComponent(Behaviour* _pComponent, BehaviourTag)
{
	Behaviour* const pRemove = static_cast<Behaviour*>(_pComponent);

	for (unsigned n = 0; n < mBehaviours.size(); ++n)
	{
		if (pRemove == mBehaviours[n])
		{
			mBehaviours.FastRemove(n);
			break;
		}
	}
}



#endif		// INCLUDE GUARD

