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
#include "Object/GameObject.h"		 // File Header
#include "Object/ObjectFlags.h"		 // eObjFlags
#include "Component/Component.h"	 // Component
#include "Behaviour/Behaviour.h"	 // Behaviour

#include "Utility/GUID.h"			// Global UniqueID
#include "Utility/Utility.h"		 // Move
#include "Utility/DebugAssert.h"
#include "DataStructure/AutoArray.h" 

#include "IO/TextSerialiser.h"
#include "System/Behaviour/BehaviourSystem.h"

//duplicate should immediately put into scene
#include "Component/Transform.h"
#include "System/Driver/Driver.h"		
#include "System/Scene/SceneSystem.h"   
#include "System/Scene/Scene.h"

#define Ping(_ARR, _FUNC, ...)			\
for (auto& e : _ARR)					\
	if (e->IsActive())					\
		e-> ## _FUNC ##( __VA_ARGS__ )

#define ForcePing(_ARR, _FUNC, ...)		\
for (auto& e : _ARR)					\
	e-> ## _FUNC ##( __VA_ARGS__ )

Dystopia::GameObject::GameObject(void) noexcept
	: GameObject{ GUIDGenerator::INVALID }
{

}

Dystopia::GameObject::GameObject(uint64_t _ID) noexcept
	: mnID{ _ID }, mnFlags{ FLAG_NONE },
	mTransform{ this }, mComponents{}, mBehaviours{}
	,mbIsStatic(false)
{

}

Dystopia::GameObject::GameObject(GameObject&& _obj) noexcept
	: mnID{ _obj.mnID }, mnFlags{ _obj.mnFlags }, mName{ _obj.mName },
	mComponents{ Ut::Move(_obj.mComponents) },
	mBehaviours{ Ut::Move(_obj.mBehaviours) },
	mTransform{ _obj.mTransform }
	, mbIsStatic(false)
{
	_obj.mComponents.clear();
	_obj.mBehaviours.clear();

	_obj.mnID = GUIDGenerator::INVALID;
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

void Dystopia::GameObject::SetFlag(eObjFlag _flag)
{
	mnFlags |= _flag;
}

void Dystopia::GameObject::ReplaceFlag(unsigned _fullFlag)
{
	mnFlags = _fullFlag;
}

void Dystopia::GameObject::RemoveFlags(eObjFlag _flags)
{
	mnFlags &= ~_flags;
}

unsigned Dystopia::GameObject::GetFlag() const
{
	return mnFlags;
}

void Dystopia::GameObject::Load(void)
{
//	ForcePing(mComponents, Load);
	ForcePing(mBehaviours, Load);
}

void Dystopia::GameObject::Awake(void)
{
	if (mnFlags & FLAG_EDITOR_OBJ)
		return;

	mTransform.Awake();
	ForcePing(mComponents, Awake);
	ForcePing(mBehaviours, Awake);
}

void Dystopia::GameObject::Init(void)
{
	if (mnFlags & FLAG_EDITOR_OBJ)
		return;

	mTransform.Init();
	ForcePing(mComponents, Init);
	ForcePing(mBehaviours, Init);
}

void Dystopia::GameObject::Update(const float)
{
//	Ping(mComponents, Update, _fDeltaTime);
//	Ping(mBehaviours, Update, _fDeltaTime);
}

void Dystopia::GameObject::FixedUpdate(const float)
{
//	Ping(mComponents, FixedUpdate, _fFixedDT);
//	Ping(mBehaviours, FixedUpdate, _fFixedDT);
}

void Dystopia::GameObject::PostUpdate(void)
{
//	Ping(mComponents, &Component::PostUpdate);
//	Ping(mBehaviours, PostUpdate);
}

void Dystopia::GameObject::Destroy(void)
{
	for (auto& c : mTransform.GetAllChild())
		c->GetOwner()->Destroy();

	ForcePing(mComponents, GameObjectDestroy);
	ForcePing(mBehaviours, GameObjectDestroy);

	mnFlags = FLAG_REMOVE;
}

void Dystopia::GameObject::Unload(void)
{
//	ForcePing(mComponents, Unload);
	ForcePing(mBehaviours, Unload);
}


void Dystopia::GameObject::OnCollisionEnter(const CollisionEvent& _pEvent)
{
	ForcePing(mBehaviours, OnCollisionEnter, _pEvent);
}

void Dystopia::GameObject::OnCollisionStay(const CollisionEvent& _pEvent)
{
	ForcePing(mBehaviours, OnCollisionStay, _pEvent);
}

void Dystopia::GameObject::OnCollisionExit(const CollisionEvent& _pEvent)
{
	ForcePing(mBehaviours, OnCollisionExit, _pEvent);
}

void Dystopia::GameObject::OnTriggerEnter(const GameObject* _pOther)
{
	ForcePing(mBehaviours, OnTriggerEnter, _pOther);
}

void Dystopia::GameObject::OnTriggerStay(const GameObject* _pOther)
{
	ForcePing(mBehaviours, OnTriggerStay, _pOther);
}

void Dystopia::GameObject::OnTriggerExit(const GameObject* _pOther)
{
	ForcePing(mBehaviours, OnTriggerExit, _pOther);
}

void Dystopia::GameObject::PurgeComponents(void)
{
	//for (Behaviour* e : mBehaviours)
	//	delete e;
	mBehaviours.clear();
	mComponents.clear();
}


void Dystopia::GameObject::AddComponent(Component* _p, ComponentTag)
{
	mComponents.Insert(_p);
	_p->SetOwner(this);
	_p->Awake();
}

void Dystopia::GameObject::AddComponent(Behaviour* _p, BehaviourTag)
{
	mBehaviours.Insert(_p);
	_p->SetOwner(this);
	_p->Awake();
}

void Dystopia::GameObject::RemoveComponent(Component* const _pComponent)
{
	for (unsigned n = 0; n < mComponents.size(); ++n)
	{
		if (_pComponent == mComponents[n])
		{
			mComponents[n]->DestroyComponent();
			mComponents.FastRemove(n);
			return;
		}
	}

	for (unsigned n = 0; n < mBehaviours.size(); ++n)
	{
		if (_pComponent == mBehaviours[n])
		{
			mBehaviours[n]->DestroyComponent();
			mBehaviours[n] = nullptr;
			mBehaviours.FastRemove(n);
			return;
		}
	}
}

void Dystopia::GameObject::Serialise(TextSerialiser& _out) const
{
	_out.InsertStartBlock("START_GO_DATA");
	_out << mnID << mnFlags << mName;
	_out.InsertEndBlock("END_GO_DATA");

	_out.InsertStartBlock("START_GO_TRANSFORM");
	mTransform.Serialise(_out);
	_out.InsertEndBlock("END_GO_TRANSFORM");
}

void Dystopia::GameObject::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	_in >> mnID >> mnFlags >> mName;
	_in.ConsumeEndBlock();

	_in.ConsumeStartBlock();
	mTransform.Unserialise(_in);
	mTransform.SetOwner(this);
	_in.ConsumeEndBlock();
}

Dystopia::GameObject* Dystopia::GameObject::Duplicate(void) const
{
	DEBUG_ASSERT((mnFlags & FLAG_EDITOR_OBJ), "Should not duplicate editor objects");

	auto& curScene = EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene();

	DEBUG_ASSERT(!curScene.FindGameObject(mnID), "Should not duplicate object not in scene");

	auto id = GUIDGenerator::GetUniqueID();
	GameObject *p	= curScene.InsertGameObject(id);
	p->mnFlags		= mnFlags;
	p->mName		= mName;
	p->mName		+= "_clone";
	p->mTransform	= mTransform;

	p->mTransform.SetOwner(p);

	for (auto& c : mComponents)
		p->mComponents.Insert(c->Duplicate());
	for (auto& b : mBehaviours)
		p->mBehaviours.Insert(EngineCore::GetInstance()->GetSystem<BehaviourSystem>()->RequestDuplicate(b, p->mnID));

	const auto& children = mTransform.GetAllChild();
	for (const auto& child : children)
	{
		auto o = child->GetOwner()->Duplicate();
		o->mTransform.SetParentID(p->GetID());
		o->Awake();
	}
	p->Awake();
	return p;
}

void Dystopia::GameObject::SetID(const uint64_t& _id)
{
	mnID = _id;
}

uint64_t Dystopia::GameObject::GetID(void) const
{
	return mnID;
}

HashString Dystopia::GameObject::GetName(void) const
{
	return mName;
}

const char* Dystopia::GameObject::GetNamePtr() const
{
	return mName.c_str();
}

void Dystopia::GameObject::SetName(const HashString& _strName)
{
	mName = _strName;
}

void Dystopia::GameObject::Identify(void)
{
	mTransform.SetOwner(this);
	for (auto c : mComponents)
		c->SetOwner(this);
	for (auto b : mBehaviours)
		b->SetOwner(this);
}

Dystopia::GameObject& Dystopia::GameObject::operator=(GameObject&& _rhs)
{
	mnID    = _rhs.mnID;
	mnFlags = _rhs.mnFlags;
	mName   = _rhs.mName;

	mTransform = _rhs.mTransform;
	Ut::Swap(mComponents, _rhs.mComponents);
	Ut::Swap(mBehaviours, _rhs.mBehaviours);

	_rhs.mnID = GUIDGenerator::INVALID;

	return *this;
}


#undef Ping
#undef ForcePing


