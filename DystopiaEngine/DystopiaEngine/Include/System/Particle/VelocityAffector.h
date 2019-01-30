/* HEADER *********************************************************************************/
/*!
\file	VelocityAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _VELOCITY_AFFECTOR_H_
#define _VELOCITY_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{
	/*
		location affector bytes and bits:

	*/

	struct VelocityAffector : ParticleAffector
	{
		VelocityAffector(void);
		~VelocityAffector(void);


		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif




