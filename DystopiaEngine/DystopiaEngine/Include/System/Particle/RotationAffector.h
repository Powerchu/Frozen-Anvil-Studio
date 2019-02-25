/* HEADER *********************************************************************************/
/*!
\file	RotationAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ROTATION_AFFECTOR_H_
#define _ROTATION_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{
	/*
		bytes and bits:
		data[0,1,2,3] used for min deg
		data[4,5,6,7] used for max deg

		reserved[0] used for flags
			- 1 << 0 used for random rotation
	*/

	struct InitialRotationAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnSpawn;

		InitialRotationAffector(void);
		~InitialRotationAffector(void);

		/* Degrees */
		void SetMinDeg(float);
		void SetMaxDeg(float);
		float GetMinDeg(void) const;
		float GetMaxDeg(void) const;

		/* random */
		void ToggleRandom(void);
		bool IsRandom(void) const;

		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

	/*
		bytes and bits:
		data[0,1,2,3] used for min rate
		data[4,5,6,7] used for max rate

		reserved[0] used for flags
			- 1 << 0 used for random rate
	*/

	struct RotationRateAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnSpawn;

		RotationRateAffector(void);
		~RotationRateAffector(void);

		/* Rotation rate */
		void SetMinRate(float);
		void SetMaxRate(float);
		float GetMinRate(void) const;
		float GetMaxRate(void) const;

		/* Random */
		void ToggleRandom(void);
		bool IsRandom(void) const;
	
		void AffectorSpawn(Emitter&, float);
	
		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif




