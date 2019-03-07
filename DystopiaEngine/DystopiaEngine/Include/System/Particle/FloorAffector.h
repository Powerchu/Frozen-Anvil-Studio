/* HEADER *********************************************************************************/
/*!
\file	FloorAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _FLOOR_AFFECTOR_H_
#define _FLOOR_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{
	/*
		bytes and bits:

		data[0,1,2,3] offset x 
		data[4,5,6,7] offset y 
		data[8,9,10,11] width
		data[12,13,14,15] height

	*/

	struct FloorAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnFixedUpdate;

		FloorAffector(void);
		~FloorAffector(void);

		void SetOffset(float, float);
		float GetOffsetX(void) const;
		float GetOffsetY(void) const;
		void SetSize(float, float);
		float GetSizeX(void) const;
		float GetSizeY(void) const;

		const char * EditorDisplayLabel(void) const;

		void AffectorUpdate(Emitter&, float);
		void EditorUI(void);
	};

}

#endif 







