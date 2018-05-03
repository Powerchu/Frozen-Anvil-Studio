/* HEADER *********************************************************************************/
/*!
\file	GameObject.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Base class for all GameObjects.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Object\GameObject.h"		// File Header
#include "Component\Component.h"	// Component
#include "Behaviour\Behaviour.h"	// Behaviour
#include "Utility\Utility.h"		// Move

Dystopia::GameObject::GameObject(bool _bActive) :
	mbActive{ _bActive }, mComponents{}, mBehaviours{}, mnID{ 0xFFFFFFFF }
{

}

Dystopia::GameObject::GameObject(unsigned _ID, bool _bActive) :
	mbActive{ _bActive }, mComponents{}, mBehaviours{}, mnID{ _ID }
{

}

Dystopia::GameObject::GameObject(GameObject&& _obj) :
	mbActive{ _obj.mbActive }, mnID{ _obj.mnID },
	mComponents{ Utility::Move(_obj.mComponents) },
	mBehaviours{ Utility::Move(_obj.mBehaviours) }
{
	_obj.mComponents.clear();
	_obj.mBehaviours.clear();

	_obj.mnID = 0xFFFFFFFF;
}

Dystopia::GameObject::~GameObject(void)
{
	PurgeComponents();
}


bool Dystopia::GameObject::IsActive(void) const
{
	return mbActive;
}

void Dystopia::GameObject::SetActive(const bool _bEnable)
{
	mbActive = _bEnable;
}


void Dystopia::GameObject::Load(void)
{
	ForcePing(mComponents, &Component::Load);
	ForcePing(mBehaviours, &Behaviour::Load);
}

void Dystopia::GameObject::Init(void)
{
	ForcePing(mComponents, &Component::Init);
	ForcePing(mBehaviours, &Behaviour::Init);
}

void Dystopia::GameObject::Update(const float _fDeltaTime)
{
	Ping(mComponents, &Component::Update, _fDeltaTime);
	Ping(mBehaviours, &Behaviour::Update, _fDeltaTime);
}

void Dystopia::GameObject::FixedUpdate(const float _fFixedDT)
{
	Ping(mComponents, &Component::FixedUpdate, _fFixedDT);
	Ping(mBehaviours, &Behaviour::FixedUpdate, _fFixedDT);
}

void Dystopia::GameObject::PostUpdate(void)
{
	Ping(mComponents, &Component::PostUpdate);
	Ping(mBehaviours, &Behaviour::PostUpdate);
}

void Dystopia::GameObject::Destroy(void)
{
	mbActive = false;

	Ping(mComponents, &Component::OnDestroy);
	Ping(mBehaviours, &Behaviour::OnDestroy);
}

void Dystopia::GameObject::Unload(void)
{
	ForcePing(mComponents, &Component::Unload);
	ForcePing(mBehaviours, &Behaviour::Unload);
}


void Dystopia::GameObject::OnCollisionEnter(CollisionEvent& _pEvent)
{
	Ping(mBehaviours, &Behaviour::OnCollisionEnter, _pEvent);
}

void Dystopia::GameObject::OnCollisionStay(CollisionEvent& _pEvent)
{
	Ping(mBehaviours, &Behaviour::OnCollisionStay, _pEvent);
}

void Dystopia::GameObject::OnCollisionExit(CollisionEvent& _pEvent)
{
	Ping(mBehaviours, &Behaviour::OnCollisionExit, _pEvent);
}

void Dystopia::GameObject::PurgeComponents(void)
{
	for (Component* e : mComponents)
		delete e;
	mComponents.clear();

	for (Behaviour* e : mBehaviours)
		delete e;
	mBehaviours.clear();
}

void Dystopia::GameObject::Serialise()
{
	ForcePing(mComponents, &Component::Serialise);
	ForcePing(mBehaviours, &Behaviour::Serialise);
}
void Dystopia::GameObject::Unserialise()
{
	ForcePing(mComponents, &Component::Unserialise);
	ForcePing(mBehaviours, &Behaviour::Unserialise);
}

Dystopia::GameObject* Dystopia::GameObject::Duplicate(void) const
{
	return nullptr;
}


std::string Dystopia::GameObject::GetName(void) const
{
	return mName;
}

void Dystopia::GameObject::SetName(const std::string& _strName)
{
	mName = _strName;
}

