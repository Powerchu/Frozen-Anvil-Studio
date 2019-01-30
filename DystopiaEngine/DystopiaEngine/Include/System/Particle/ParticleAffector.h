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

#include "Utility/Utility.h"
#include "Utility/MetaAlgorithms.h"
#include "IO/TextSerialiser.h"

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
		struct SpawnAffector
	>;

	class Emitter;

	struct alignas(16) ParticleAffector
	{
		using UPDATE = AffectorTag::OnUpdate;

		inline void Update(Emitter& e, float _dt) noexcept
		{
			(this->*Updator)(e, _dt);
		}

		inline int GetID(void) const noexcept
		{
			return mID;
		}

		inline void Serialise(TextSerialiser& _out) const
		{
			_out.InsertStartBlock("Affector");
			for (unsigned i = 0; i < 16; ++i)
				_out << data[i];
			for (unsigned i = 0; i < 4; ++i)
				_out << reserved[i];
			_out.InsertEndBlock("Affector");
		}

		inline void UnSerialise(TextSerialiser& _in)
		{
			_in.ConsumeStartBlock();
			char c = '\0';
			for (unsigned i = 0; i < 16; ++i)
			{
				_in >> c;
				data[i] = c;
			}
			for (unsigned i = 0; i < 4; ++i)
			{
				_in >> c;
				reserved[i] = c;
			}
			_in.ConsumeEndBlock();
		}

		inline ParticleAffector(ParticleAffector&&) noexcept;
		ParticleAffector& operator = (ParticleAffector&&) noexcept;

	protected:
		using UpdateFunc_t = void(ParticleAffector::*)(Emitter&, float);

		template <typename C>
		ParticleAffector(void(C::*_upd)(Emitter&, float)) noexcept
			: mID{ Ut::MetaFind_t<C, AffectorList>::value }, 
			Updator{ reinterpret_cast<UpdateFunc_t>(_upd) },
			data{}, reserved{}
		{}

		alignas(16) char data[16];

	private:
		UpdateFunc_t Updator;
		int const mID;

	protected:
		char reserved[4];
	};
}

inline Dystopia::ParticleAffector::ParticleAffector(Dystopia::ParticleAffector&& _obj) noexcept
	: mID{ _obj.mID }
{
	*this = Ut::Move(_obj);
}



#endif		// INCLUDE GUARD


