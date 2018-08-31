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
#include "Object\GameObject.h"		 // File Header
#include "Component\Component.h"	 // Component
#include "Behaviour\Behaviour.h"	 // Behaviour
#include "Object\ObjectFlags.h"		 // eObjFlags
#include "DataStructure\AutoArray.h" 
#include "Utility\Utility.h"		 // Move
#include "IO\TextSerialiser.h"

#define Ping(_ARR, _FUNC, ...)			\
for (auto& e : _ARR)					\
	if (e->IsActive())					\
		e-> ## _FUNC ##( __VA_ARGS__ )

#define ForcePing(_ARR, _FUNC, ...)		\
for (auto& e : _ARR)					\
	e-> ## _FUNC ##( __VA_ARGS__ )

Dystopia::GameObject::GameObject(void) :
	mComponents{}, mBehaviours{}, mnID{ ~0ull }, mnFlags{ FLAG_NONE }
{

}

Dystopia::GameObject::GameObject(size_t _ID) :
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
	if (mnFlags & FLAG_REMOVE)
		return;

	mnFlags = _bEnable ? mnFlags | FLAG_ACTIVE : mnFlags & ~FLAG_ACTIVE;
}


void Dystopia::GameObject::Load(void)
{
//	ForcePing(mComponents, Load);
	ForcePing(mBehaviours, Load);
}

void Dystopia::GameObject::Init(void)
{
//	ForcePing(mComponents, Init);
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
	Ping(mComponents, GameObjectDestroy);
	Ping(mBehaviours, GameObjectDestroy);

	mnFlags = FLAG_REMOVE;
}

void Dystopia::GameObject::Unload(void)
{
//	ForcePing(mComponents, Unload);
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
	for (Behaviour* e : mBehaviours)
		delete e;
	mBehaviours.clear();

	mComponents.clear();
}


void Dystopia::GameObject::AddComponent(Component* _p, ComponentTag)
{
	mComponents.Insert(_p);
}

void Dystopia::GameObject::AddComponent(Behaviour* _p, BehaviourTag)
{
	mBehaviours.Insert(_p);
}

void Dystopia::GameObject::RemoveComponent(Component* const _pComponent)
{
	for (unsigned n = 0; n < mComponents.size(); ++n)
	{
		if (_pComponent == mComponents[n])
		{
			mComponents[n]->DestroyComponent();
			return;
		}
	}

	for (unsigned n = 0; n < mBehaviours.size(); ++n)
	{
		if (_pComponent == mBehaviours[n])
		{
			mBehaviours[n]->DestroyComponent();
			return;
		}
	}
}

void Dystopia::GameObject::Serialise(TextSerialiser& _out) const
{
	_out << mnID << mnFlags << mName;
	_out.InsertEndBlock("GO_DATA");

//	ForcePing(mComponents, Serialise, _in);
	// _out << e.mID << e.mComponent << e.mSystem;

//	for (auto& e : mComponents)
//		_out << e->GetID();
	_out.InsertEndBlock("COMPONENT_LIST");
	ForcePing(mBehaviours, Serialise, _out);
	_out.InsertEndBlock("BEHAVIOUR_LIST");
}

void Dystopia::GameObject::Unserialise(TextSerialiser& _in)
{
	_in >> mnID >> mnFlags >> mName;
	_in.ConsumeEndBlock();

//	ForcePing(mComponents, Unserialise, _out);
	//while (!_in.EndOfInput())
	//{
	//	decltype(Utility::RemoveRef_t<decltype(mComponents[0])>::mID)        id;
	//	decltype(Utility::RemoveRef_t<decltype(mComponents[0])>::mComponent) comp;
	//	decltype(Utility::RemoveRef_t<decltype(mComponents[0])>::mSystem)    sys;

	//	_in >> id >> comp >> sys;
	//	mComponents.EmplaceBack(id, comp, sys);
	//}
	_in.ConsumeEndBlock();
	ForcePing(mBehaviours, Unserialise, _in);
	_in.ConsumeEndBlock();
}

Dystopia::GameObject* Dystopia::GameObject::Duplicate(void) const
{
	return nullptr;
}


size_t Dystopia::GameObject::GetID(void) const
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

Dystopia::GameObject& Dystopia::GameObject::operator=(GameObject&& _rhs)
{
	mnID    = _rhs.mnID;
	mnFlags = _rhs.mnFlags;
	mName   = _rhs.mName;

	mTransform = _rhs.mTransform;
	Utility::Swap(mComponents, _rhs.mComponents);
	Utility::Swap(mBehaviours, _rhs.mBehaviours);

	_rhs.mnID = ~0ull;

	return *this;
}


#undef Ping
#undef ForcePing


