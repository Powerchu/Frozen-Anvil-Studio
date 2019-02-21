/* HEADER *********************************************************************************/
/*!
\file	TrailEmitter.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Trail Particle Emitter

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/TrailEmitter.h"

Dystopia::TrailEmitter::TrailEmitter(ParticleEmitter* _owner) noexcept
	: Emitter{ _owner }
{
}

Dystopia::TrailEmitter::TrailEmitter(TrailEmitter const& _o) noexcept
	: Emitter{ _o }
{
}

Dystopia::TrailEmitter::~TrailEmitter(void) noexcept
{
}

void Dystopia::TrailEmitter::Init(void)
{
	Emitter::BaseInit();
	
	mInitialLife.clear();
	mLifetime   .clear();
	mRotVel		.clear();
	mRotAcc		.clear();
	mUV         .clear();
	mSize       .clear();
	mColour     .clear();
	mAccel      .clear();
	mVelocity   .clear();
	mPosition   .clear();
	mRotation   .clear();

	mInitialLife.reserve(mParticle.mnLimit);
	mLifetime   .reserve(mParticle.mnLimit);
	mRotVel		.reserve(mParticle.mnLimit);
	mRotAcc		.reserve(mParticle.mnLimit);
	mUV         .reserve(mParticle.mnLimit);
	mSize       .reserve(mParticle.mnLimit);
	mColour     .reserve(mParticle.mnLimit);
	mAccel      .reserve(mParticle.mnLimit);
	mVelocity   .reserve(mParticle.mnLimit);
	mPosition   .reserve(mParticle.mnLimit);
	mRotation	.reserve(mParticle.mnLimit);

	Emitter::InitBuffers();
}

void Dystopia::TrailEmitter::FixedUpdate(float _dt) noexcept
{
}

void Dystopia::TrailEmitter::KillParticle(size_t _nIdx) noexcept
{
}

void Dystopia::TrailEmitter::SpawnParticle(void) noexcept
{
}

