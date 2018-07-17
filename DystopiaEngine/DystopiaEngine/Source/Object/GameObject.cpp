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
#include "Object\ObjectFlags.h"		// eObjFlags

#define Ping(_ARR, _FUNC, ...)			\
for (auto& e : _ARR)					\
	if (e->IsActive())					\
		e-> ## _FUNC ##( __VA_ARGS__ )

#define ForcePing(_ARR, _FUNC, ...)		\
for (auto& e : _ARR)					\
	e-> ## _FUNC ##( __VA_ARGS__ )

Dystopia::GameObject::GameObject(void) :
	mComponents{}, mBehaviours{}, mnID{ 0xFFFFFFFF }, mnFlags{ FLAG_NONE }
{

}

Dystopia::GameObject::GameObject(unsigned _ID) :
	mComponents{}, mBehaviours{}, mnID{ _ID }, mnFlags{ FLAG_NONE }
{

}

Dystopia::GameObject::GameObject(GameObject&& _obj) :
	mnID{ _obj.mnID }, mnFlags{ _obj.mnFlags },
	mComponents{ Utility::Move(_obj.mComponents) },
	mBehaviours{ Utility::Move(_obj.mBehaviours) }
{
	_obj.mComponents.clear();
	_obj.mBehaviours.clear();

	_obj.mnID = 0xFFFFFFFF;
	_obj.mnFlags = FLAG_REMOVE;
}

Dystopia::GameObject::~GameObject(void)
{
	PurgeComponents();
}


bool Dystopia::GameObject::IsActive(void) const
{
	return mnFlags & FLAG_ACTIVE;
}

void Dystopia::GameObject::SetActive(const bool _bEnable)
{
	if(_bEnable)
		mnFlags |=  FLAG_ACTIVE;
	else
		mnFlags &= ~FLAG_ACTIVE;
}


void Dystopia::GameObject::Load(void)
{
	ForcePing(mComponents, Load);
	ForcePing(mBehaviours, Load);
}

void Dystopia::GameObject::Init(void)
{
	ForcePing(mComponents, Init);
	ForcePing(mBehaviours, Init);
}

void Dystopia::GameObject::Update(const float _fDeltaTime)
{
//	Ping(mComponents, Update, _fDeltaTime);
	Ping(mBehaviours, Update, _fDeltaTime);
}

void Dystopia::GameObject::FixedUpdate(const float _fFixedDT)
{
//	Ping(mComponents, FixedUpdate, _fFixedDT);
	Ping(mBehaviours, FixedUpdate, _fFixedDT);
}

void Dystopia::GameObject::PostUpdate(void)
{
//	Ping(mComponents, &Component::PostUpdate);
	Ping(mBehaviours, PostUpdate);
}

void Dystopia::GameObject::Destroy(void)
{
	Ping(mComponents, OnDestroy);
	Ping(mBehaviours, OnDestroy);
}

void Dystopia::GameObject::Unload(void)
{
	ForcePing(mComponents, Unload);
	ForcePing(mBehaviours, Unload);
}


void Dystopia::GameObject::OnCollisionEnter(const CollisionEvent& _pEvent)
{
	Ping(mBehaviours, OnCollisionEnter, _pEvent);
}

void Dystopia::GameObject::OnCollisionStay(const CollisionEvent& _pEvent)
{
	Ping(mBehaviours, OnCollisionStay, _pEvent);
}

void Dystopia::GameObject::OnCollisionExit(const CollisionEvent& _pEvent)
{
	Ping(mBehaviours, OnCollisionExit, _pEvent);
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


void Dystopia::GameObject::Serialise(TextSerialiser& _in) const
{
	ForcePing(mComponents, Serialise, _in);
	ForcePing(mBehaviours, Serialise, _in);
}

void Dystopia::GameObject::Unserialise(TextSerialiser& _out)
{
	ForcePing(mComponents, Unserialise, _out);
	ForcePing(mBehaviours, Unserialise, _out);
}

Dystopia::GameObject* Dystopia::GameObject::Duplicate(void) const
{
	return nullptr;
}


size_t Dystopia::GameObject::GetID()
{
	return mnID;
}

std::string Dystopia::GameObject::GetName(void) const
{
	return mName;
}

void Dystopia::GameObject::SetName(const std::string& _strName)
{
	mName = _strName;
}


#undef Ping
#undef ForcePing


