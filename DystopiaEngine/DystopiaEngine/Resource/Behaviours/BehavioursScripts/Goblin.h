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

		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;

		virtual void OnCollisionEnter(const CollisionEvent&) override;
		virtual void OnCollisionStay (const CollisionEvent&) override;
		virtual void OnCollisionExit (const CollisionEvent&) override;

		virtual void OnTriggerEnter(const GameObject *) override;
		virtual void OnTriggerStay (const GameObject *) override;
		virtual void OnTriggerExit (const GameObject *) override;

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual Goblin * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		int mHealth = 20;
		
		bool isColliding = false;
	private:
		friend MetaData<Goblin>;
	};

	extern "C"
	{
		DllExport Goblin * GoblinClone()
		{
			return new Goblin;
		}
	}
}

PP_REFLECT(Dystopia::Goblin, mHealth)

#endif //_Goblin_H_


