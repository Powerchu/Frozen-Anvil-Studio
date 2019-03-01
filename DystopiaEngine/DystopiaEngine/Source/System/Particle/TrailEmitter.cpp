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

#include "GL/glew.h"



Dystopia::TrailEmitter::TrailEmitter(ParticleEmitter* _owner) noexcept
	: Emitter{ _owner, GL_LINE_STRIP, 32, "Def Trail Particle" }
{
}

Dystopia::TrailEmitter::TrailEmitter(TrailEmitter const& _o) noexcept
	: Emitter{ _o }
{
}

Dystopia::TrailEmitter::~TrailEmitter(void) noexcept
{
}

//void Dystopia::TrailEmitter::Init(void)
//{
//	Emitter::BaseInit();
//	Emitter::InitArrays();
//	Emitter::InitBuffers();
//}

void Dystopia::TrailEmitter::FixedUpdate(float _dt) noexcept
{
	auto pAcc = mAccel.begin();
	for (auto& e : mVelocity)
	{
		e += *pAcc * _dt;
		++pAcc;
	}

	auto pVel = mVelocity.begin();
	auto pDelay = mRotation.begin();
	int const stride = GetStride();
	float const fDelayCap = .125f;
	for (auto b = mPosition.begin(), e = mPosition.end(); b != e; b += stride)
	{
		auto t = b + stride - 1;

		*pDelay += _dt;

		while (*pDelay > fDelayCap && t != b)
			[&t](void) { auto r = t - 1; *t = *r; t = r; }();

		*pDelay = *pDelay > fDelayCap ? *pDelay - fDelayCap : *pDelay;

		*b += *pVel * _dt;
	}

	mbUpdatedPositions = true;
}

void* Dystopia::TrailEmitter::GetVTablePtr(void)
{
	static void * const vtable = [](void)
	{
		TrailEmitter x;
		auto x_ptr = reinterpret_cast<void**>(&x);
		return *x_ptr;
	}();

	return vtable;
}

//void Dystopia::TrailEmitter::KillParticle(size_t _nIdx) noexcept
//{
//}
//
//void Dystopia::TrailEmitter::SpawnParticle(void) noexcept
//{
//}

