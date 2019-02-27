/* HEADER *********************************************************************************/
/*!
\file	ParticleSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Systems

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PARTICLESYS_H_
#define _PARTICLESYS_H_

#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"



namespace Dystopia
{
	class ParticleEmitter;

	class ParticleSystem : public Systems, public ComponentDonor<ParticleEmitter>
	{
	public:

		ParticleSystem(void) noexcept;
		~ParticleSystem(void) noexcept;

		bool Init(void) override;
		void Update(float) override;
		void FixedUpdate(float) override;
		void PostUpdate(void) override;
		void Shutdown(void) override;

	private:


	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 




#endif		// INCLUDE GUARD

