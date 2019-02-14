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
		bytes and bits:
		
		data[0,1] for red 1
		data[2,3] for green 1
		data[4,5] for blue 1
		data[6,7] for alpha 1
		data[8,9] for red 2
		data[10,11] for green 2
		data[12,13] for blue 2
		data[14,15] for alpha 2

		reserved[0] flags:
			- 1 << 0 random initial color

	*/
	
	struct InitialColorAffector : ParticleAffector
	{
		using UPDATE = AffectorTag::OnSpawn;

		InitialColorAffector(void);
		~InitialColorAffector(void);

		/* Colors */
		void SetR1(unsigned short);
		void SetG1(unsigned short);
		void SetB1(unsigned short);
		void SetA1(unsigned short);
		void SetR2(unsigned short);
		void SetG2(unsigned short);
		void SetB2(unsigned short);
		void SetA2(unsigned short);

		unsigned int GetR1(void) const;
		unsigned int GetG1(void) const;
		unsigned int GetB1(void) const;
		unsigned int GetA1(void) const;
		unsigned int GetR2(void) const;
		unsigned int GetG2(void) const;
		unsigned int GetB2(void) const;
		unsigned int GetA2(void) const;

		void EnableRandom(bool);


		void AffectorSpawn(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};



	/*
		bytes and bits:

		data[0,1] for red 
		data[2,3] for green 
		data[4,5] for blue
		data[6,7] for alpha 
		data[8,9,10,11] for life target 
	*/

	struct ColorOverLifeAffector : ParticleAffector
	{
		ColorOverLifeAffector(void);
		~ColorOverLifeAffector(void);

		/* target color */
		void SetTargetR(unsigned short);
		void SetTargetG(unsigned short);
		void SetTargetB(unsigned short);
		void SetTargetA(unsigned short);
		void SetTargetLife(float);

		unsigned short GetTargetR(void) const;
		unsigned short GetTargetG(void) const;
		unsigned short GetTargetB(void) const;
		unsigned short GetTargetA(void) const;
		float GetTargetLife(void) const;

		void AffectorUpdate(Emitter&, float);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif





