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

	//	HOT_FIX : 
	//		need manually specify affector tag in ParticleEditor.cpp 
	//	TODO :
	//		automatically deduce affector tag somehow 
	using AffectorList = Ut::MetaAutoIndexer_t<
		struct SpawnAffector,
		struct LocationAffector,
		struct LifetimeAffector,
		struct InitialColorAffector,
		struct ColorOverLifeAffector,
		struct InitialVelocityAffector,
		struct VelocityOverLifeAffector,
		struct InitialSizeAffector,
		struct SizeOverLifeAffector,
		struct InitialAccelerationAffector
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

			const unsigned long long* p1 = reinterpret_cast<const unsigned long long *>(data);
			const unsigned long long* p2 = reinterpret_cast<const unsigned long long *>(data + 8);
			const unsigned int* p3 = reinterpret_cast<const unsigned int *>(reserved);

			_out << *p1;
			_out << *p2;
			_out << *p3;

			// for (unsigned i = 0; i < 16; ++i)
			// 	_out << data[i];
			// for (unsigned i = 0; i < 4; ++i)
			// 	_out << reserved[i];
			_out.InsertEndBlock("Affector");
		}

		inline void UnSerialise(TextSerialiser& _in)
		{
			_in.ConsumeStartBlock();

			unsigned long long ull = 0;
			unsigned int ui = 0;
			_in >> ull;
			*reinterpret_cast<unsigned long long *>(data) = ull;
			_in >> ull;
			*reinterpret_cast<unsigned long long *>(data + 8) = ull;
			_in >> ui;
			*reinterpret_cast<unsigned int *>(reserved) = ui;

			// char c = '\0';
			// for (unsigned i = 0; i < 16; ++i)
			// {
			// 	_in >> c;
			// 	data[i] = c;
			// }
			// for (unsigned i = 0; i < 4; ++i)
			// {
			// 	_in >> c;
			// 	reserved[i] = c;
			// }
			_in.ConsumeEndBlock();
		}

		inline ParticleAffector(ParticleAffector&&) noexcept;
		inline ParticleAffector(ParticleAffector const&) noexcept;
		inline ParticleAffector& operator = (ParticleAffector&&) noexcept;
		ParticleAffector& operator = (ParticleAffector const&) noexcept;

	protected:
		using UpdateFunc_t = void(ParticleAffector::*)(Emitter&, float) noexcept;

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
	: ParticleAffector{ _obj }
{
}

inline Dystopia::ParticleAffector::ParticleAffector(Dystopia::ParticleAffector const& _obj) noexcept
	: mID{ _obj.mID }
{
	*this = _obj;
}

inline Dystopia::ParticleAffector& Dystopia::ParticleAffector::operator = (Dystopia::ParticleAffector&& _rhs) noexcept
{
	return *this = _rhs;
}



#endif		// INCLUDE GUARD


