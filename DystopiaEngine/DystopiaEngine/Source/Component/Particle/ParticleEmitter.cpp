/* HEADER *********************************************************************************/
/*!
\file	ParticleEmitter.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Emitter

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/ParticleEmitter.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

#include "System/Driver/Driver.h"
#include "System/Particle/ParticleAffector.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Particle/SpawnAffector.h"
#include "System/Time/ScopedTimer.h"
#include "System/Profiler/ProfilerAction.h"

#include "Math/Vectors.h"
#include "Math/Matrix4.h"
#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"
#include "Utility/DebugAssert.h"
#include "DataStructure/AutoArray.h"

#include "Editor/RuntimeMeta.h"

#include <GL/glew.h>

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#include "Editor/Payloads.h"
#endif 


Dystopia::ParticleEmitter::ParticleEmitter(void) noexcept
	: mEmitters{}
{
}

Dystopia::ParticleEmitter::ParticleEmitter(Dystopia::ParticleEmitter const& _rhs) noexcept
	: mEmitters{ _rhs.mEmitters }
{
}

Dystopia::ParticleEmitter::~ParticleEmitter(void) noexcept
{
}

void Dystopia::ParticleEmitter::Awake(void)
{
}

void Dystopia::ParticleEmitter::Init(void)
{
}

void Dystopia::ParticleEmitter::FixedUpdate(float)
{
}

void Dystopia::ParticleEmitter::Bind(void) const noexcept
{
}

void Dystopia::ParticleEmitter::Unbind(void) const noexcept
{
}

void Dystopia::ParticleEmitter::Render(void) const noexcept
{
}

Dystopia::Emitter* Dystopia::ParticleEmitter::GetEmitter(unsigned _n) const noexcept
{
	return mEmitters.begin() + _n;
}

AutoArray<Dystopia::Emitter>& Dystopia::ParticleEmitter::GetEmitters(void) noexcept
{
	return mEmitters;
}

Dystopia::ParticleEmitter* Dystopia::ParticleEmitter::Duplicate(void) const
{
	return static_cast<ComponentDonor<ParticleEmitter>*>(CORE::Get<SYSTEM>())->RequestComponent(*this);
}


void Dystopia::ParticleEmitter::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("Emitter");
	Component::Serialise(_out);

	//for(auto& e : mEmitters)
	// e.Serialise(_out);
}

void Dystopia::ParticleEmitter::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	Component::Unserialise(_in);

	//for(auto& e : mEmitters)
	// e.Unserialise(_out);
}

void Dystopia::ParticleEmitter::EditorUI(void) noexcept
{
}


