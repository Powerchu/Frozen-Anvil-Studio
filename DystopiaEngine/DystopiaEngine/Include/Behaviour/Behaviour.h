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

#include "Component\ComponentList.h"	// eComponents
#include "Component\Component.h"		// Component

namespace Dystopia
{
	class CollisionEvent;

	class Behaviour : public Component
	{
	public:

		using TAG = BehaviourTag;
		virtual unsigned GetComponentType(void) const { return unsigned(-2); };

		// ====================================== CONSTRUCTORS ======================================= // 

		Behaviour(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		virtual void Load(void);
		virtual void Init(void);

		virtual void Update(const float _fDeltaTime);
		virtual void FixedUpdate(const float _fDeltaTime);
		virtual void PostUpdate(void);

		virtual void GameObjectDestroy(void);
		virtual void Unload(void);

		void OnCollisionEnter(const CollisionEvent&);
		void OnCollisionStay (const CollisionEvent&);
		void OnCollisionExit (const CollisionEvent&);
	};
}



#endif		// INCLUDE GUARD

