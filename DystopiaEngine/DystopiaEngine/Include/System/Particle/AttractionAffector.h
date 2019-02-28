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
		data[4,5,6,7] = offsetX
		data[8,9,10,11] = offsetY
		data[12,13,14,15] = dt acc

		reserved[0] flags
			- 1 << 0 attract
			//- 1 << 1 hexlix
			- 1 << 2 repulse
			- 1 << 3 use sine wave X
			- 1 << 4 use sine wave Y

		reserved[1,2] range
		reserved[3] scaler
	*/

	struct PointAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnFixedUpdate;

		PointAffector(void);
		~PointAffector(void);

		/* generic settings */
		void SetStrength(unsigned short);
		void SetRange(unsigned short);
		unsigned short GetStrength(void) const;
		unsigned short GetRange(void) const;
		void SetOffsetX(float);
		void SetOffsetY(float);
		float GetOffsetX(void) const;
		float GetOffsetY(void) const;

		/* point attraction */
		void TogglePoint(bool _enabled);
		bool IsPoint(void) const;
		void ToggleRepulse(bool _enabled);
		bool IsRepulse(void) const;

		//* line attraction */
		//void ToggleHelix(bool _enabled);
		//bool IsHelix(void) const;

		/* sine wave */
		void ToggleSineX(void);
		void ToggleSineY(void);
		bool GetSineX(void) const;
		bool GetSineY(void) const;
		void SetSineRange(unsigned short);
		unsigned short GetSineRange(void) const;
		void SetSineSpeedScale(unsigned char);
		unsigned char GetSineSpeedScale(void) const;

		/* requirement for affector */
		void AffectorUpdate(Emitter&, float);

		//void HelixAttract(Emitter&, float);
		void PointAttract(Emitter&, float);
		void PointRepulse(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif 






