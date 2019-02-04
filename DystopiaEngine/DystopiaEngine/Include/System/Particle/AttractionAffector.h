/* HEADER *********************************************************************************/
/*!
\file	AttractionAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ATTRACTION_AFFECTOR_H_
#define _ATTRACTION_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{

	/*
		bytes and bits:

		data[0,1] = strength
		data[2,3] = range
		data[4,5,6,7] = dt acc

		reserved[0] flags
			- 1 << 0 point
			- 1 << 1 hexlix

	*/

	struct AttractionAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnFixedUpdate;

		AttractionAffector(void);
		~AttractionAffector(void);

		/* generic settings */
		void SetStrength(unsigned short);
		void SetRange(unsigned short);
		unsigned short GetStrength(void) const;
		unsigned short GetRange(void) const;

		/* point attraction */
		void TogglePoint(bool _enabled);
		bool IsPoint(void) const;

		/* line attraction */
		void ToggleHelix(bool _enabled);
		bool IsHelix(void) const;

		/* requirement for affector */
		void AffectorUpdate(Emitter&, float);

		void HelixAttract(Emitter&, float);
		void PointAttract(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif 






