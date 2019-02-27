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

		data[0,1,2,3] for min x
		data[4,5,6,7] for min y
		data[8,9,10,11] for max x
		data[12,13,14,15] for max y

		reserved[0]
			- 1 << 0 for random
	*/
	
	struct InitialSizeAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnSpawn;

		InitialSizeAffector(void);
		~InitialSizeAffector(void);

		/* sizes */
		void SetMinX(float _val);
		void SetMinY(float _val);
		void SetMaxX(float _val);
		void SetMaxY(float _val);
		float GetMinX(void) const;
		float GetMinY(void) const;
		float GetMaxX(void) const;
		float GetMaxY(void) const;
		void ToggleRandom(void);
		bool IsRandom(void) const;

		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

	/*
		bytes and bits:

		data[0,1,2,3] linear X size change per second
		data[4,5,6,7] linear Y size change per second

	*/

	struct SizeOverLifeAffector : ParticleAffector
	{
		SizeOverLifeAffector(void);
		~SizeOverLifeAffector(void);

		/* sizes */
		void SetDeltaX(float);
		void SetDeltaY(float);
		float GetDeltaX(void) const;
		float GetDeltaY(void) const;

		void AffectorUpdate(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};
}

#endif




