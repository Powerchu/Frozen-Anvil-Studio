/* HEADER *********************************************************************************/
/*!
\file	ParticleSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Systems

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/ParticleSystem.h"

#include "Component/Emitter.h"

#include "Math/Vectors.h"


Dystopia::ParticleSystem::ParticleSystem(void) noexcept
{
}

Dystopia::ParticleSystem::~ParticleSystem(void) noexcept
{
}

bool Dystopia::ParticleSystem::Init(void)
{
	return true;
}

void Dystopia::ParticleSystem::Update(float)
{
	for (auto& e : mComponents)
	{

	}
}

void Dystopia::ParticleSystem::FixedUpdate(float)
{
	for (auto& e : mComponents)
	{

	}
}

void Dystopia::ParticleSystem::Shutdown(void)
{
}


