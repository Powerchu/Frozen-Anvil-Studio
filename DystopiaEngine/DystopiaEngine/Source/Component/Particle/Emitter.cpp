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

#include "System/Driver/Driver.h"
#include "System/Particle/ParticleAffector.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"

#include "Math/Vectors.h"

#include <GL/glew.h>


Dystopia::Emitter::Emitter(void) noexcept
	: mColour{}, mPosition{}, mVelocity{}, mAccel{}, mLifetime{}, 
	mUpdate{}, mFixedUpdate{}
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

	glBindVertexArray(0);
}

void Dystopia::Emitter::Awake(void)
{
	if (!mpShader)
	{
		CORE::Get<ShaderSystem>()->GetShader("Default Particle");
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

void Dystopia::Emitter::Bind(void) const noexcept
{
	glBindVertexArray(mVAO);
}

void Dystopia::Emitter::Unbind(void) const noexcept
{
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

Dystopia::Shader& Dystopia::Emitter::GetShader(void) noexcept
{
	return *mpShader;
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
	{
		_out << mpShader->GetName();
	}
	else
	{
		_out << "Default Particle";
	}

	for (auto& e : mAttach)
	{
		_out << e.GetID();
	}

	for (auto& e : mUpdate)
	{
		_out << e.GetID();
	}

	for (auto& e : mFixedUpdate)
	{
		_out << e.GetID();
	}
}

void Dystopia::Emitter::Unserialise(TextSerialiser& _in)
{
	char buf[128];

	_in >> buf;


	mpShader = CORE::Get<ShaderSystem>()->GetShader(buf);
}

void Dystopia::Emitter::EditorUI(void) noexcept
{
}

