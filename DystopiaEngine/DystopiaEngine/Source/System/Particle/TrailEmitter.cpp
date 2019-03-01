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
	: Emitter{ _owner, GL_LINE_STRIP, 2, "Def Trail Particle" }
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
//	//Emitter::BaseInit();
//	
//	//mInitialLife.clear();
//	//mLifetime   .clear();
//	//mRotVel     .clear();
//	//mRotAcc     .clear();
//	//mUV         .clear();
//	//mSize       .clear();
//	//mColour     .clear();
//	//mAccel      .clear();
//	//mVelocity   .clear();
//	//mPosition   .clear();
//	//mRotation   .clear();
//	//
//	//mInitialLife.reserve(mParticle.mnLimit);
//	//mLifetime   .reserve(mParticle.mnLimit * mParticle.mRotation);
//	//mRotVel     .reserve(mParticle.mnLimit);
//	//mRotAcc     .reserve(mParticle.mnLimit);
//	//mUV         .reserve(mParticle.mnLimit);
//	//mSize       .reserve(mParticle.mnLimit);
//	//mColour     .reserve(mParticle.mnLimit);
//	//mAccel      .reserve(mParticle.mnLimit);
//	//mVelocity   .reserve(mParticle.mnLimit);
//	//mPosition   .reserve(mParticle.mnLimit);
//	//mRotation   .reserve(mParticle.mnLimit);
//
//	//Emitter::InitBuffers();
//	//glVertexAttribDivisor(0, static_cast<int>(mParticle.mRotation));
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
	for (auto b = mPosition.begin(), e = mPosition.end(); b != e; b += stride)
	{
		auto t = b + stride - 1;

		*pDelay += _dt;

		while (*pDelay > 0.25f && t != b)
			[&t](void) { auto r = t - 1; *t = *r; t = r; }();

		*pDelay = *pDelay > 0.1f ? *pDelay - .1f : *pDelay;

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

