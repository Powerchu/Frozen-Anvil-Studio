/* HEADER *********************************************************************************/
/*!
\file	ParticleAffector.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Functors that act on particle emitters to change their behaviour

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
		struct OnAttach      {}; // DT is 1 on attach and 0 on detach
		struct OnUpdate      {};
		struct OnFixedUpdate {};
	}

	using AffectorList = Ut::MetaAutoIndexer_t<
	>;

	class Emitter;

	struct ParticleAffector
	{
		using UPDATE = AffectorTag::OnUpdate;

		inline void Update(Emitter& e, float _dt) const noexcept
		{
			Updator(e, _dt);
		};

		inline int GetID(void) const noexcept
		{
			return mID;
		}

	protected:
		using UpdateFunc_t = void(*)(Emitter&, float);

		ParticleAffector(int _nID, UpdateFunc_t _upd) noexcept
			: mID{ _nID }, Updator{ _upd }
		{}

		alignas(16) char data[128];
		UpdateFunc_t Updator;
		int const mID;
	};
}



#endif		// INCLUDE GUARD


