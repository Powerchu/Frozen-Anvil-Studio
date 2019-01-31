/* HEADER *********************************************************************************/
/*!
\file	ColorAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _COLOR_AFFECTOR_H_
#define _COLOR_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{
	/*
		location affector bytes and bits:

		data[0,1,2,3] for x min
		data[4,5,6,7] for y min
		data[8,9,10,11] for x max
		data[12,13,14,15] for y max

		reserved[0]
			- 1 << 1 randome velocity

	*/
	
	struct InitialColorAffector : ParticleAffector
	{
		InitialColorAffector(void);
		~InitialColorAffector(void);

		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

	struct ColorOverLifeAffector : ParticleAffector
	{
		ColorOverLifeAffector(void);
		~ColorOverLifeAffector(void);

		void AffectorUpdate(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif





