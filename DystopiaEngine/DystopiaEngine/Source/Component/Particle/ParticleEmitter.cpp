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
#include "System/Particle/SpawnAffector.h"
#include "System/Particle/Emitter.h"
#include "System/Particle/TrailEmitter.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/Graphics/TextureSystem.h"
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
#include "Editor/EditorMain.h"
#include "Editor/ParticleEditor.h"
#endif 


Dystopia::ParticleEmitter::ParticleEmitter(void) noexcept
	: mEmitters{}
{
}

Dystopia::ParticleEmitter::ParticleEmitter(Dystopia::ParticleEmitter const& _rhs) noexcept
	: mEmitters{ _rhs.mEmitters }
{
	for (auto& e : mEmitters)
		e.SetOwner(this);
}

Dystopia::ParticleEmitter::~ParticleEmitter(void) noexcept
{
	mEmitters.clear();
}

void Dystopia::ParticleEmitter::Awake(void)
{
	for (auto& e : mEmitters)
	{
		e.Awake();
		e.SetOwner(this);
	}
}

void Dystopia::ParticleEmitter::Init(void)
{
	for(auto& e : mEmitters)
	{
		e.Init();
		e.SetOwner(this);
	}
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
	if (_n >= mEmitters.size() || _n < 0)
		return nullptr;

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
	_out.InsertStartBlock("Particle Emitter");
	Component::Serialise(_out);
	_out << mEmitters.size();
	_out.InsertEndBlock("Particle Emitter");

	for(auto& e : mEmitters)
		e.Serialise(_out);
}

void Dystopia::ParticleEmitter::Unserialise(TextSerialiser& _in)
{
	unsigned size = 0;

	_in.ConsumeStartBlock();
	Component::Unserialise(_in);
	_in >> size;
	_in.ConsumeEndBlock();

	mEmitters.clear();
	for (unsigned i = 0; i < size; ++i)
	{
		mEmitters.EmplaceBack(this);
		mEmitters.back().Unserialise(_in);
	}
}

void Dystopia::ParticleEmitter::EditorUI(void) noexcept
{
#if EDITOR
	EGUI::PushLeftAlign(130);

	if (EGUI::Display::Button("Add Emitter", { 100, 24 }))
	{
		mEmitters.EmplaceBack(this);
		mEmitters.back().Awake();
	}
	if (EGUI::Display::Button("Add Trail Emitter", { 100, 24 }))
	{
		mEmitters.EmplaceBackAs<TrailEmitter>(this);
		mEmitters.back().Awake();
	}

	for (int i = 0; i < mEmitters.size(); ++i)
	{
		char buffer[50]{ "Emitter" };
		sprintf_s(buffer, "Emitter %d", i);

		EGUI::PushID(i);

		EGUI::Display::HorizontalSeparator();
		bool open = EGUI::Display::StartTreeNode(buffer);
		if (ImGui::BeginPopupContextItem())
		{
			if (EGUI::Display::SelectableTxt("Remove Emitter"))
			{
				mEmitters.FastRemove(i--);
				open = false;
			}
			ImGui::EndPopup();
		}
		if (open)
		{
			if (EGUI::Display::Button("Edit", { 150, 24 }))
			{
				Editor::EditorMain::GetInstance()->GetPanel<Editor::ParticleEditor>()->SetParticleEmitter(this, i);
			}
			mEmitters[i].EditorUI();
			EGUI::Display::EndTreeNode();
		}

		EGUI::PopID();
	}

	EGUI::PopLeftAlign();
#endif
}
