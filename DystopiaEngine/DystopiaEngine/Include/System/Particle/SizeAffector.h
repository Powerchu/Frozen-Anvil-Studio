/* HEADER *********************************************************************************/
/*!
\file	SizeAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SIZE_AFFECTOR_H_
#define _SIZE_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{
	/*
		bytes and bits:

		data[0,1,2,3] for delta
		data[4,5,6,7] for inital size min 
		data[8,9,10,11] for inital size max

		reserved[0]
			- 1 << 1 for bounce to target 2
	*/
	
	struct InitialSizeAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnSpawn;

		InitialSizeAffector(void);
		~InitialSizeAffector(void);

		/* sizes */
		void SetInitialSizeMin(float _val);
		void SetInitialSizeMax(float _val);
		float GetInitialSizeMin(void) const;
		float GetInitialSizeMax(void) const;
		void EnableRandomInitial(bool _enable);

		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

	/*
		bytes and bits:

		data[0,1,2,3] for delta
		data[4,5,6,7] for target size
		data[8,9,10,11] for duration to hit (0 - 1)
		data[12,13,14,15] for target 2

		reserved[0] 
			- 1 << 1 for bounce to target 2
	*/

	struct SizeOverLifeAffector : ParticleAffector
	{
		SizeOverLifeAffector(void);
		~SizeOverLifeAffector(void);

		/* sizes */
		void SetTargetSize(float _val);
		void SetTargetDuration(float _val);
		void SetTargetSizeAfter(float _val);
		void EnableBounceback(bool);

		void AffectorUpdate(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};
}

#endif




