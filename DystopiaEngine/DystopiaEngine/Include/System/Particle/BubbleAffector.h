/* HEADER *********************************************************************************/
/*!
\file	BubbleAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _BUBBLE_AFFECTOR_H_
#define _BUBBLE_AFFECTOR_H_
#include "ParticleAffector.h"

#include "Math/Vectors.h"

namespace Dystopia
{

	/*
		bytes and bits:
		data[0,1] = radius
		data[2,3,4,5] = offset x
		data[6,7,8,9] = offset y
		
		reserved[0] for flags
			- 1 << 0 for solid

	*/

	struct BubbleAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnFixedUpdate;

		BubbleAffector(void);
		~BubbleAffector(void);

		/* solid state or permeable */
		void ToggleSolid(void);
		bool IsSolid(void) const;

		/* properties */
		void SetRadius(unsigned short);
		unsigned short GetRadius(void) const;
		void SetOffsetX(float);
		void SetOffsetY(float);
		float GetOffsetX(void) const;
		float GetOffsetY(void) const;

		Math::Vector2 GetIntersection(const Math::Vec2& _pt1, const Math::Vec2& _pt2) const;

		/* requirement for affector */
		void AffectorUpdate(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif


