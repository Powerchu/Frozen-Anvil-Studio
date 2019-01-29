/* HEADER *********************************************************************************/
/*!
\file	Emitter.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Emitter

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Emitter.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

#include "System/Driver/Driver.h"
#include "System/Particle/ParticleAffector.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/TextureSystem.h"

#include "Math/Vectors.h"
#include "Math/Matrix4.h"
#include "Utility/DebugAssert.h"

#include "Editor/EGUI.h"
#include <GL/glew.h>


Dystopia::Emitter::Emitter(void) noexcept
	: mColour{}, mPosition{}, mVelocity{}, mAccel{}, mLifetime{}, mSpawnCount{},
	mSpawn{}, mUpdate{}, mFixedUpdate{}, mpShader{}, mpTexture{}
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(2, &mColourBuffer);

	Bind();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribDivisor(0, 4);
	glVertexAttribDivisor(1, 4);

	Unbind();
}

Dystopia::Emitter::~Emitter(void) noexcept
{
	Bind();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteBuffers(2, &mColourBuffer);
	glDeleteVertexArrays(1, &mVAO);

	Unbind();
}

void Dystopia::Emitter::Awake(void)
{
	if (!mpShader)
	{
		CORE::Get<ShaderSystem>()->GetShader("Default Particle");
	}
	if (!mpTexture)
	{
		CORE::Get<TextureSystem>()->GetTexture("checker_dxt3.dds");
	}
}

void Dystopia::Emitter::Init(void)
{
	size_t limit = 0;
	
	mColour  .clear();
	mPosition.clear();
	mVelocity.clear();
	mAccel   .clear();
	mLifetime.clear();

	mColour  .reserve(limit);
	mPosition.reserve(limit);
	mVelocity.reserve(limit);
	mAccel   .reserve(limit);
	mLifetime.reserve(limit);

	Bind();
	glBindBuffer(GL_ARRAY_BUFFER, mColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, limit * sizeof(decltype(mColour)::Val_t), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, limit * sizeof(decltype(mColour)::Val_t), nullptr, GL_DYNAMIC_DRAW);
}

void Dystopia::Emitter::FixedUpdate(float _fDT)
{
	auto pVel   = mVelocity.begin();
	auto pAccel = mAccel   .begin();

	for (auto& e : mPosition)
	{
		*pVel +=  *pAccel       * _fDT;
		 e    += (*pAccel).xyz0 * _fDT;

		 ++pVel; ++pAccel;
	}
}

void Dystopia::Emitter::Bind(void) const noexcept
{
	glBindVertexArray(mVAO);
	mpTexture->Bind();
}

void Dystopia::Emitter::Unbind(void) const noexcept
{
	mpTexture->Unbind();
	glBindVertexArray(0);
}

void Dystopia::Emitter::UploadBuffers(void) const noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, mColourBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mColour.size(), mColour.begin());

	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mPosition.size(), mPosition.begin());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Dystopia::Emitter::Render(void) const noexcept
{
	glDrawArraysInstanced(GL_POINTS, 0, 1, static_cast<GLsizei>(mPosition.size()));
}

void Dystopia::Emitter::KillParticle(unsigned _nIdx) noexcept
{
	DEBUG_ASSERT(!mSpawnCount, "Particle System Error: No particles to kill!");
	--mSpawnCount;

	mLifetime.FastRemove(_nIdx);
	mColour  .FastRemove(_nIdx);
	mAccel   .FastRemove(_nIdx);
	mVelocity.FastRemove(_nIdx);
	mPosition.FastRemove(_nIdx);
}

void Dystopia::Emitter::SpawnParticle(void) noexcept
{
	++mSpawnCount;

	for (auto& e : mSpawn)
		e.Update(*this, 0);

	auto transform = GetOwner()->GetComponent<Transform>();
	auto pos = transform->GetGlobalPosition() + transform->GetTransformMatrix() * mParticle.mPos.xyz1;

	pos.w = mParticle.mfSize;

	mLifetime.TryEmplaceBack(mParticle.mfLifeDur);
	mColour  .TryEmplaceBack(mParticle.mColour  );
	mAccel   .TryEmplaceBack(                   );
	mVelocity.TryEmplaceBack(mParticle.mVelocity);
	mPosition.TryEmplaceBack(pos                );
}

Dystopia::Shader& Dystopia::Emitter::GetShader(void) noexcept
{
	return *mpShader;
}

Dystopia::GfxParticle& Dystopia::Emitter::GetSpawnDefaults(void) noexcept
{
	return mParticle;
}

AutoArray<Math::Vec4>& Dystopia::Emitter::GetColour(void) noexcept
{
	return mColour;
}

AutoArray<Math::Vec4>& Dystopia::Emitter::GetPosition(void) noexcept
{
	return mPosition;
}

AutoArray<Math::Vec3>& Dystopia::Emitter::GetVelocity(void) noexcept
{
	return mVelocity;
}

AutoArray<Math::Vec3>& Dystopia::Emitter::GetAcceleration(void) noexcept
{
	return mAccel;
}

AutoArray<Dystopia::ParticleAffector>& Dystopia::Emitter::GetSpawnAffectors(void) noexcept
{
	return mSpawn;
}

AutoArray<Dystopia::ParticleAffector>& Dystopia::Emitter::GetUpdateAffectors(void) noexcept
{
	return mUpdate;
}

AutoArray<Dystopia::ParticleAffector>& Dystopia::Emitter::GetFixedUpdateAffectors(void) noexcept
{
	return mFixedUpdate;
}


void Dystopia::Emitter::Serialise(TextSerialiser& _out) const
{
	if (mpShader)
		_out << mpShader->GetName();
	else
		_out << "Default Particle";

	if (mpTexture)
		_out << mpTexture->GetName();
	else
		_out << "checker_dxt3.dds";

	for (auto& e : mSpawn)
		_out << e.GetID();

	for (auto& e : mUpdate)
		_out << e.GetID();

	for (auto& e : mFixedUpdate)
		_out << e.GetID();
}

void Dystopia::Emitter::Unserialise(TextSerialiser& _in)
{
	std::string buf;
	buf.reserve(128);

	_in >> buf;
	mpShader = CORE::Get<ShaderSystem>()->GetShader(buf.c_str());

	_in >> buf;
	mpTexture = CORE::Get<TextureSystem>()->GetTexture(buf.c_str());
}

void Dystopia::Emitter::EditorUI(void) noexcept
{
	EGUI::Display::Label("Particle Count: %u", mSpawnCount);
	EGUI::Display::HorizontalSeparator();


}


