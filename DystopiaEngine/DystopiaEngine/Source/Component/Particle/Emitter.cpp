/* HEADER *********************************************************************************/
/*!
\file	Emitter.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Emitter

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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

#include <GL/glew.h>

#if EDITOR
#include "Editor/EGUI.h"
#include "System/Particle/SpawnAffector.h"
#endif 


Dystopia::Emitter::Emitter(void) noexcept
	: mColour{}, mPosition{}, mVelocity{}, mAccel{}, mLifetime{}, mSpawnCount{},
	mSpawn{}, mUpdate{}, mFixedUpdate{}, mpShader{ nullptr }, mpTexture{ nullptr }
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(2, &mColourBuffer);

	glBindVertexArray(mVAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
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
		mpShader = CORE::Get<ShaderSystem>()->GetShader("Default Particle");
	}
	if (!mpTexture)
	{
		mpTexture = CORE::Get<TextureSystem>()->GetTexture("EditorStartup.png");
	}
}

void Dystopia::Emitter::Init(void)
{
	size_t limit = 100000;
	
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
	glBufferData(GL_ARRAY_BUFFER, limit * sizeof(decltype(mColour)::Val_t), nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, limit * sizeof(decltype(mColour)::Val_t), nullptr, GL_STREAM_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glVertexAttribDivisor(0, 4);
	glVertexAttribDivisor(1, 4);

	Unbind();
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
	auto sz = mPosition.size();
	glBindBuffer(GL_ARRAY_BUFFER, mColourBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mColour.size(), mColour.begin());

	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mPosition.size(), mPosition.begin());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Dystopia::Emitter::Render(void) const noexcept
{
	auto sz = mPosition.size();
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
	auto pos = transform->GetTransformMatrix() * mParticle.mPos.xyz1;

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
	Component::Serialise(_out);
	_out.InsertStartBlock("Emitter");
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
	_out.InsertEndBlock("Emitter");
}

void Dystopia::Emitter::Unserialise(TextSerialiser& _in)
{
	Component::Unserialise(_in);
	_in.ConsumeStartBlock();
	std::string buf;
	buf.reserve(128);
	_in >> buf;
	mpShader = CORE::Get<ShaderSystem>()->GetShader(buf.c_str());

	_in >> buf;
	mpTexture = CORE::Get<TextureSystem>()->GetTexture(buf.c_str());

	_in.ConsumeEndBlock();
}

void Dystopia::Emitter::EditorUI(void) noexcept
{
	EGUI::Display::Label("Particle Count: %u", mSpawnCount);
	EGUI::Display::HorizontalSeparator();

	static float delay = 0.f;
	EGUI::Display::DragFloat("Spawn Delay", &delay, 0.1f, 0.f, 1.f);
	if (EGUI::Display::Button("Add spawn affector"))
	{
		SpawnAffector sa{};

		sa.SetSpawnDelay(delay);
		AddAffector(Ut::Move(sa));
	}

}


