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
#include "Utility/Utility.h"
#include "Utility/GUID.h"

Dystopia::Component::Component(void) noexcept
	: mnFlags{ FLAG_NONE }, mnOwner{ Utility::Constant<decltype(mnOwner), -1>::value }, mID{ GUIDGenerator::GetUniqueID() }
{

}

Dystopia::Component::Component(GameObject * _pOwner) noexcept
	: mnFlags{ FLAG_NONE }, mnOwner{ _pOwner->GetID() }, mID{ _pOwner->GetID() }
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
	mnOwner = _pOwner->GetID();
	mID = _pOwner->GetID();
}

Dystopia::GameObject* Dystopia::Component::GetOwner(void) const
{
	return EngineCore::GetInstance()->GetSystem<SceneSystem>()->FindGameObject(mnOwner);
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

