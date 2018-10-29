/* HEADER *********************************************************************************/
/*!
\file	Missle.h
\author keith.goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Missle_H_
#define _Missle_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class Missle : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(Missle);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "Missle"; }
		static uint64_t constexpr mMissleID = 18446744072780316160;

		Missle();
		~Missle();
		
		virtual void Load(void) override;
		virtual void Init(void) override;

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

		virtual Missle * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

	private:
	    bool isColliding = false;
		friend MetaData<Missle>;
	};

	extern "C"
	{
		DllExport Missle * MissleClone()
		{
			return new Missle;
		}
	}
}

PP_REFLECT_EMPTY(Dystopia::Missle)
#endif //_Missle_H_


