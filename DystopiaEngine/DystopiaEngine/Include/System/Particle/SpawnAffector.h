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
	struct SpawnAffector : ParticleAffector
	{
		SpawnAffector(void);
		~SpawnAffector(void);


		void SetSpawnDelay(float);
		void AffectorUpdate(Emitter&, float);
	};

}

#endif 






