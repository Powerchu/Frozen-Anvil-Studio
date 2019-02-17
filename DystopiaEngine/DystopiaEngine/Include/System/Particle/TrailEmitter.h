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
#ifndef _EMITTER_H_
#define _EMITTER_H_

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
		TrailEmitter(const TrailEmitter&) noexcept;
		~TrailEmitter(void) noexcept;

		// Update "physics"!
		void FixedUpdate(float _dt) override;

		virtual void KillParticle(size_t _nIdx) noexcept;
		virtual void SpawnParticle(void) noexcept;
	};
}




#endif		// INCLUDE GUARD

