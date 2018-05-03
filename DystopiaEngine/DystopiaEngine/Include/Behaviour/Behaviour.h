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
		static const eComponents TYPE = eComponents::BASE_COMPONENT;
		virtual const eComponents GetComponentType(void) const { return TYPE; };


		// ====================================== CONSTRUCTORS ======================================= // 

		Behaviour(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void OnCollisionEnter(const CollisionEvent&);
		void OnCollisionStay (const CollisionEvent&);
		void OnCollisionExit (const CollisionEvent&);

		virtual void Serialise()	= 0;
		virtual void Unserialise()	= 0;
	};
}



#endif		// INCLUDE GUARD

