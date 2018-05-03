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

		explicit GameObject(bool _bActive = true);
		GameObject(unsigned _ID, bool _bActive = true);
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

		void OnCollisionEnter(CollisionEvent&);
		void OnCollisionStay (CollisionEvent&);
		void OnCollisionExit (CollisionEvent&);

		template <typename T>
		void AddComponent(T*);
		template <typename T>
		void RemoveComponent(T*);

		void PurgeComponents(void);

		void Serialise();
		void Unserialise();

		// Creates an exact copy of the Game Object
		GameObject* Duplicate(void) const; 

		std::string GetName(void) const;
		void SetName(const std::string&);

		template<class T>
		T* GetComponent(void) const
		{
			for (Component * e : mComponents)
			{
				if (T::TYPE == e->GetComponentType())
				{
					return dynamic_cast<T*>(e);
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
					temp.Insert(dynamic_cast<T*>(e));
				}
			}

			return temp;
		}


	protected:

		bool mbActive;
		unsigned mnID;
		std::string mName;

	private:

		Transform mTransform;
		AutoArray<Component*> mComponents;
		AutoArray<Behaviour*> mBehaviours;

		GameObject(const GameObject&) = delete;

		void AddComponent(Component*, ComponentTag);
		void AddComponent(Component*, BehaviourTag);
		void RemoveComponent(Component*, ComponentTag);
		void RemoveComponent(Component*, BehaviourTag);

		template<typename T, typename U, typename Ret, typename ...Params, typename ...Args>
		inline void Ping(AutoArray<T*>& _arr, Ret(U::*_pfunc)(Params ...), Args&& ..._args);

		template<typename T, typename U, typename Ret, typename ...Params, typename ...Args>
		inline void ForcePing(AutoArray<T*>& _arr, Ret(U::*_pfunc)(Params ...), Args&& ..._args);
	};






	// ============================================ FUNCTION DEFINITIONS ============================================ // 


	template <typename T>
	inline void GameObject::AddComponent(T* _pComponent)
	{
		AddComponent(_pComponent, typename T::TAG{});
	}

	template <typename T>
	inline void GameObject::RemoveComponent(T* _pComponent)
	{
		RemoveComponent(_pComponent, typename T::TAG{});
	}

	template<typename T, typename U, typename Ret, typename ...Params, typename ...Args>
	inline void GameObject::Ping(AutoArray<T*>& _arr, Ret(U::*_pfunc)(Params ...), Args&& ..._args)
	{
		for (T* e : _arr)
		{
			if (e->IsActive())
				(e->*_pfunc)(static_cast<Args&&>(_args)...);
		}
	}

	template<typename T, typename U, typename Ret, typename ...Params, typename ...Args>
	inline void GameObject::ForcePing(AutoArray<T*>& _arr, Ret(U::*_pfunc)(Params ...), Args&& ..._args)
	{
		for (T* e : _arr)
		{
			(e->*_pfunc)(static_cast<Args&&>(args)...);
		}
	}
}



#endif		// INCLUDE GUARD

