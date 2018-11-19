/* HEADER *********************************************************************************/
/*!
\file	Fireball.h
\author keith.goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Fireball_H_
#define _Fireball_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include <map>
#include "Behaviour/BehaviourMemberFunc.h"
#include "Utility/DebugAssert.h"
#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class Fireball : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(Fireball);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "Fireball"; }
		static uint64_t constexpr mFireballID = 18446744073442368000;

		Fireball();
		~Fireball();
		
		virtual void Load(void) override;
		virtual void Init(void) override;
		virtual void Awake(void) override; 
		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;

		virtual void OnCollisionEnter(const CollisionEvent&);
		virtual void OnCollisionStay (const CollisionEvent&);
		virtual void OnCollisionExit (const CollisionEvent&);

		virtual void OnTriggerEnter(const GameObject *);
		virtual void OnTriggerStay (const GameObject *);
		virtual void OnTriggerExit (const GameObject *);

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual Fireball * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

        virtual void ReceiveMessage(const char * const _FuncName, BehaviourMessage _msg);
        
		std::map<const char *, BehaviourMemberFunc<Fireball>> mMemberFunc;
		
	private:
		friend MetaData<Fireball>;
	};

	extern "C"
	{
		DllExport Fireball * FireballClone()
		{
			return new Fireball;
		}
	}
}

PP_REFLECT_EMPTY(Dystopia::Fireball)
#endif //_Fireball_H_


