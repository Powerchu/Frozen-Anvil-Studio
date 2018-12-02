/* HEADER *********************************************************************************/
/*!
\file	Goblin.h
\author Dan (100%)
\par    email: Dan.Kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Goblin_H_
#define _Goblin_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "System/AI/AISystem.h"
#include "Math/Vector4.h"
#include "Object/GameObject.h"
#include "Component/RigidBody.h"
#include "Utility/DebugAssert.h"
#include <map>
#include "Behaviour/BehaviourMemberFunc.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class Goblin : public Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(Goblin);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "Goblin"; }
		static uint64_t constexpr mGoblinID = 708352512;

		Goblin();
		~Goblin();
		
		virtual void Load(void) override;
		virtual void Init(void) override;
		virtual void Awake(void) override;
		
		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;

		virtual void OnCollisionEnter(const CollisionEvent&) override;
		virtual void OnCollisionStay (const CollisionEvent&) override;
		virtual void OnCollisionExit (const CollisionEvent&) override;

		virtual void OnTriggerEnter(GameObject * const) override;
		virtual void OnTriggerStay (GameObject * const) override;
		virtual void OnTriggerExit (GameObject * const) override;

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual Goblin * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;
		
		void TakeDamage(int _dmg);
		void Knock(int _amt, int _direction);
		void Attacking();
		
		// Reflection Stuff
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		int mHealth = 50;
		RigidBody * rBody;
		float theMass;
		float damageDelay;
		float damageCount;
		float attackDelay;
		float attackCount;
		bool takenDamage;
		bool isWinding;
		bool isColliding = false;
		
		PP_MEMBERFUNC(Dystopia::Goblin, TakeDamage, Knock, Attacking)
		
	private:
		friend MetaData<Goblin>;
	};

	extern "C"
	{
		inline DllExport Goblin * GoblinClone()
		{
			return new Goblin;
		}
	}
}

PP_REFLECT(Dystopia::Goblin, mHealth, takenDamage, attackCount)

#endif //_Goblin_H_


