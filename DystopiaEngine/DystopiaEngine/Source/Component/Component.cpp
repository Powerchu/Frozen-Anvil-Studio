/* HEADER *********************************************************************************/
/*!
\file	Component.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
Base class for all components

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Component.h"		// File Header
#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"
#include "System/Driver/Driver.h"
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#include "Utility/Utility.h"
#include "Utility/GUID.h"

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#endif

Dystopia::Component::Component(void) noexcept
	: mnFlags{ FLAG_NONE }, mpOwner{ nullptr }, mID{ GUIDGenerator::GetUniqueID() }
{

}

Dystopia::Component::Component(GameObject * _pOwner) noexcept
	: mnFlags{ FLAG_NONE }, mpOwner{ _pOwner }, mID{ GUIDGenerator::GetUniqueID() }
{
}

Dystopia::Component::Component(const Component& _rhs) noexcept
	: mnFlags{ _rhs.mnFlags }, mpOwner{ nullptr }, mID{ GUIDGenerator::GetUniqueID() }
{
}

Dystopia::Component::~Component(void)
{

}

bool Dystopia::Component::IsActive(void) const
{
	return mnFlags & FLAG_ACTIVE;
}

void Dystopia::Component::SetActive(const bool _bEnable)
{
	mnFlags = _bEnable ? mnFlags | FLAG_ACTIVE : mnFlags & ~FLAG_ACTIVE;
}


void Dystopia::Component::Load(void)
{

}

void Dystopia::Component::Awake(void)
{

}

void Dystopia::Component::Init(void)
{

}

void Dystopia::Component::GameObjectDestroy(void)
{
	DestroyComponent();
}

void Dystopia::Component::Unload(void)
{

}


void Dystopia::Component::DestroyComponent(void)
{
	mnFlags = FLAG_REMOVE;
}


void Dystopia::Component::SetOwner(GameObject* _pOwner)
{
	mpOwner = _pOwner;
}

Dystopia::GameObject* Dystopia::Component::GetOwner(void) const
{
	return mpOwner;
}

uint64_t Dystopia::Component::GetOwnerID(void) const
{
	if (mpOwner) return mpOwner->GetID();
	return GUIDGenerator::INVALID;
}


Dystopia::Component* Dystopia::Component::Duplicate(void) const
{
	return nullptr;
}

void Dystopia::Component::EditorUI(void) noexcept
{

}

uint64_t Dystopia::Component::GetID() const
{
	return mID;
}

unsigned Dystopia::Component::GetFlags(void) const
{
	return mnFlags;
}

void Dystopia::Component::SetFlags(eObjFlag _flags)
{
	mnFlags |= _flags;
}

void Dystopia::Component::RemoveFlags(eObjFlag _flags)
{
	mnFlags &= ~_flags;
}

void Dystopia::Component::Serialise(TextSerialiser& _out) const
{
	_out << mID;
	_out << mnFlags;
	_out << GetOwnerID();
}

void Dystopia::Component::Unserialise(TextSerialiser& _in)
{
	uint64_t nOwner;
	_in >> mID;
	_in >> mnFlags;
	_in >> nOwner;

	auto sceneSys = EngineCore::GetInstance()->GetSystem<SceneSystem>();
	GameObject* owner = sceneSys->GetActiveScene().FindGameObject(nOwner);

	if (owner)
	{
		// dont need init cuz next scene will get init-ed when the scene inits
		owner->AddComponent(this, Component::TAG{});
	}
#if EDITOR
	else if (mnFlags & eObjFlag::FLAG_EDITOR_OBJ)
	{
		Editor::GetInstance()->ReAttachComponent(this);
	}
#endif 
}

