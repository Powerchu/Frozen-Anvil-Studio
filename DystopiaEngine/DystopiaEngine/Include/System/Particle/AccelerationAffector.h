/* HEADER *********************************************************************************/
/*!
\file	AccelerationAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ACCELERATION_AFFECTOR_H_
#define _ACCELERATION_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{
	/*
		bytes and bits:

		data[0,1,2,3] for x min
		data[4,5,6,7] for y min
		data[8,9,10,11] for x max
		data[12,13,14,15] for y max

		reserved[0]
			- 1 << 1 randome acceleration
	*/

	struct InitialAccelerationAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnSpawn;

		InitialAccelerationAffector(void);
		~InitialAccelerationAffector(void);

		/* velocities */
		void SetMinX(float _val);
		void SetMinY(float _val);
		void SetMaxX(float _val);
		void SetMaxY(float _val);
		float GetMinX(void) const;
		float GetMinY(void) const;
		float GetMaxX(void) const;
		float GetMaxY(void) const;
		void EnableRandom(bool _enabled);

		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif




