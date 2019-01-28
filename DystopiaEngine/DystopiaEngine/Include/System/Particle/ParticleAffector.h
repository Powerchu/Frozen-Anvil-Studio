/* HEADER *********************************************************************************/
/*!
\file	ParticleAffector.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Functors that act on particle emitters to change their behaviour

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PARTICLEAFFECTOR_H_
#define _PARTICLEAFFECTOR_H_

#include "Utility/MetaAlgorithms.h"


namespace Dystopia
{
	namespace AffectorTag
	{
		struct OnSpawn       {};
		struct OnDeath       {};
		struct OnUpdate      {};
		struct OnFixedUpdate {};
	}

	using AffectorList = Ut::MetaAutoIndexer_t<
	>;

	class Emitter;

	struct alignas(16) ParticleAffector
	{
		using UPDATE = AffectorTag::OnUpdate;

		inline void Update(Emitter& e, float _dt) noexcept
		{
			(this->*Updator)(e, _dt);
		};

		inline int GetID(void) const noexcept
		{
			return mID;
		}

	protected:
		using UpdateFunc_t = void(ParticleAffector::*)(Emitter&, float);

		template <typename C>
		ParticleAffector(void(C::*_upd)(Emitter&, float)) noexcept
			: mID{ Ut::MetaFind_t<C, AffectorList>::value }, 
			Updator{ reinterpret_cast<UpdateFunc_t>(_upd) }
		{}

		alignas(16) char data[16];

	private:
		UpdateFunc_t Updator;
		int const mID;

	protected:
		int mReserved;
	};
}



#endif		// INCLUDE GUARD


