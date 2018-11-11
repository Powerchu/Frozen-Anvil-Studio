/* HEADER *********************************************************************************/
/*!
\file	Behaviour.h      
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Base class for all Behaviours

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/


#ifndef _BEHAVIOUR_H_
#define _BEHAVIOUR_H_

#include "Globals.h"
#include "Component/ComponentList.h"	// eComponents
#include "Component/Component.h"		// Component
#include "Reflection/ReflectionTypeErasure.h"
#include "Object/GameObject.h"
#include "System/Behaviour/BehaviourMessage.h"

namespace Dystopia
{
	struct CollisionEvent;

	class _DLL_EXPORT Behaviour : public Component
	{
	public:

		using TAG = BehaviourTag;
		virtual unsigned GetComponentType(void) const { return unsigned(-2); };
		virtual const std::string GetEditorName(void) const { return "Generic Behaviour"; }

		// ====================================== CONSTRUCTORS ======================================= // 

		Behaviour(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		virtual void Load(void);
		virtual void Awake(void);
		virtual void Init(void);

		virtual void Update(const float _fDeltaTime);
		virtual void FixedUpdate(const float _fDeltaTime);
		virtual void PostUpdate(void);

		virtual void GameObjectDestroy(void);
		virtual void Unload(void);

		virtual const char * const GetBehaviourName() const { return nullptr; }

		virtual void OnCollisionEnter(const CollisionEvent&) = 0;
		virtual void OnCollisionStay (const CollisionEvent&) = 0;
		virtual void OnCollisionExit (const CollisionEvent&) = 0;

		virtual void OnTriggerEnter(const GameObject *) = 0;
		virtual void OnTriggerStay(const GameObject *)  = 0;
		virtual void OnTriggerExit(const GameObject *)  = 0;

		virtual void Serialise(TextSerialiser&) const {};
		virtual void Unserialise(TextSerialiser&) {};

		virtual TypeErasure::TypeEraseMetaData       GetMetaData()       = 0;
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const = 0;

		virtual void ReceiveMessage(const char * const _FuncName, BehaviourMessage _msg);

		virtual Behaviour * Duplicate() const;
	};
}



#endif		// INCLUDE GUARD

