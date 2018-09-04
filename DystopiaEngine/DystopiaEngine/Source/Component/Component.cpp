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
#include "Component\Component.h"		// File Header
#include "Object\ObjectFlags.h"

Dystopia::Component::Component(void) noexcept
	: mnFlags{ FLAG_NONE }, mpOwner{ nullptr }
{

}

Dystopia::Component::Component(GameObject * _pOwner) noexcept
	: mnFlags{ FLAG_NONE }, mpOwner{ _pOwner }
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


Dystopia::Component* Dystopia::Component::Duplicate(void) const
{
	return nullptr;
}

void Dystopia::Component::EditorUI(void) noexcept
{
}

