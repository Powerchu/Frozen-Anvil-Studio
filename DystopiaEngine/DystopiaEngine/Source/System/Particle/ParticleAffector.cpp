#include "System/Particle/ParticleAffector.h"

#include <cstring>

Dystopia::ParticleAffector& Dystopia::ParticleAffector::operator = (Dystopia::ParticleAffector const& _rhs) noexcept
{
	std::memcpy(this, &_rhs, sizeof(ParticleAffector));
	return *this;
}

