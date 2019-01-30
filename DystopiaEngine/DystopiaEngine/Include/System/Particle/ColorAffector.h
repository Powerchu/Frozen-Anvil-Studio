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

	*/

	struct ColorAffector : ParticleAffector
	{
		ColorAffector(void);
		~ColorAffector(void);


		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif





