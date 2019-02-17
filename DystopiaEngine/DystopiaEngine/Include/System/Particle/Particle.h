/* HEADER *********************************************************************************/
/*!
\file	Particle.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle struct variables
	
	Should I include randomisers in the struct?

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PARTICLE_BASE_H_
#define _PARTICLE_BASE_H_

#include "Math/Vectors.h"


namespace Dystopia
{
	struct GfxParticle
	{
		Math::Vec4 mColour = Math::Vec4{ 1,1,1,1 };

		Math::Vec3 mPos;
		Math::Vec2 mSize = { 100.f, 100.f };
		Math::Vec4 mUV   = { 0, 0, 1, 1 };

		float mRotation;
		float mRotVel;
		float mRotAccel;
		Math::Vec3 mVelocity;
		Math::Vec3 mAccel;
		float mfLifeDur;

		size_t mnLimit = 10000;
	};
}



#endif		// INCLUDE GUARD

