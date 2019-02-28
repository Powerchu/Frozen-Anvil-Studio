/* HEADER *********************************************************************************/
/*!
\file	TrailEmitter.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Trail Particle Emitter

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TRAILEMITTER_H_
#define _TRAILEMITTER_H_

#include "System/Particle/Emitter.h"
#include "System/Particle/Particle.h"
#include "System/Particle/ParticleAffector.h"

#include "DataStructure/AutoArray.h"

#include "Math/MathFwd.h"
#include "Utility/Meta.h"
#include "Utility/Utility.h"

#include "Globals.h"

namespace Dystopia
{
	class ParticleEmitter;

	class TrailEmitter : public Emitter
	{
	public:
		explicit TrailEmitter(ParticleEmitter* _owner = nullptr) noexcept;
		TrailEmitter(TrailEmitter const&) noexcept;
		~TrailEmitter(void) noexcept;

		//void Init(void) override;
		void FixedUpdate(float _dt) noexcept override;

		//void KillParticle(size_t _nIdx) noexcept;
		//void SpawnParticle(void) noexcept;
	};
}




#endif		// INCLUDE GUARD

