/* HEADER *********************************************************************************/
/*!
\file	LifetimeAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LIFETIME_AFFECTOR_H_
#define _LIFETIME_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{
	/*
		location affector bytes and bits:

		data[0,1,2,3] for delta counting
		data[4,5,6,7] for min lifetime 
		data[8,9,10,11] for max lifetime 

	*/

	struct LifetimeAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnSpawn;

		LifetimeAffector(void);
		~LifetimeAffector(void);

		/* duration */
		void SetMin(float _val);
		void SetMax(float _val);

		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif





