/* HEADER *********************************************************************************/
/*!
\file	SpawnAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/SpawnAffector.h"
#include "Component/Emitter.h"

Dystopia::SpawnAffector::SpawnAffector(void)
	: ParticleAffector{ &SpawnAffector::AffectorUpdate }
{
}

Dystopia::SpawnAffector::~SpawnAffector(void)
{
}

void Dystopia::SpawnAffector::SetSpawnDelay(float _f)
{
	*reinterpret_cast<float*>(data) = _f;
}

void Dystopia::SpawnAffector::AffectorUpdate(Dystopia::Emitter& _emitter, float _dt)
{
	float& delay = *(reinterpret_cast<float*>(data));
	float& accDt = *(reinterpret_cast<float*>(data) + 1);

	accDt += _dt;
	if (accDt >= delay)
	{
		accDt -= delay;
		_emitter.SpawnParticle();
	}
}













