/* HEADER *********************************************************************************/
/*!
\file	SpawnAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SPAWN_AFFECTOR_H_
#define _SPAWN_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{

	/*
		Spawn affector bytes and bits:

		data[0,1,2,3] used for float dt
		data[4,5] used for spawn rate
		data[6,7] used for burst count
		data[8,9] used for burst low  // only if enabled random burst
		data[10, 11, 12, 13] used for delay

		reserved[0] used for flags
			- 1 << 0 continuous
			- 1 << 1 burst
			- 1 << 2 use burst low to range to count
			- 1 << 3 burst done 
	*/

	struct SpawnAffector : ParticleAffector
	{
		SpawnAffector(void);
		~SpawnAffector(void);

		/* Continuous spawn */
		void SetSpawnRate(unsigned short _particlePerSecond);
		void EnableContinuous(bool _enabled);

		/* Burst spawn */
		void SetBurstCount(unsigned short _particlePerBurst);
		void SetBurstLow(unsigned short _particlePerBurstLow);
		void EnableRandomBurst(bool _enabled);
		void EnableBurst(bool _enabled);

		/* affects both */
		void SetInitialDelay(float _delay);

		/* requirement for affector */
		void AffectorUpdate(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif 






