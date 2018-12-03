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
#include "System/Tag/TagSystem.h"
#include "Editor/Payloads.h"

//duplicate should immediately put into scene
#include "Component/Transform.h"
#include "System/Driver/Driver.h"		
#include "System/Scene/SceneSystem.h"   
#include "System/Scene/Scene.h"

#if EDITOR
#include "Component/ComponentList.h"
#include "System/Base/ComponentDonor.h"
#include "Editor/RuntimeMeta.h"
#endif

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
	,mbIsStatic(false), mTags{0}
{

}

Dystopia::GameObject::GameObject(GameObject&& _obj) noexcept
	: mnID{ _obj.mnID }, mnFlags{ _obj.mnFlags }, mName{ _obj.mName },
	mComponents{ Ut::Move(_obj.mComponents) },
	mBehaviours{ Ut::Move(_obj.mBehaviours) },
	mTransform{ _obj.mTransform }
	, mbIsStatic(false),
	mTags(0)
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

	for (auto& mComponent : mComponents)
	{
		mComponent->SetActive(_bEnable);
	}

	for (auto& mBehaviour : mBehaviours)
	{
		mBehaviour->SetActive(_bEnable);
	}
}

bool Dystopia::GameObject::IsDragging() const
{
	return mnFlags & FLAG_DRAGGING;
}

void Dystopia::GameObject::SetDragging(bool _bEnable)
{
	if (mnFlags & FLAG_REMOVE)
		return;

	mnFlags = _bEnable ? mnFlags | FLAG_DRAGGING : mnFlags & ~FLAG_DRAGGING;
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
#if EDITOR
	if (_flags == FLAG_EDITOR_OBJ)
	{
		for (auto c : mComponents)
			c->RemoveFlags(_flags);
		for (auto b : mBehaviours)
			b->RemoveFlags(_flags);
	}
#endif

	mnFlags &= ~_flags;
}

unsigned Dystopia::GameObject::GetFlag() const
{
	return mnFlags;
}

unsigned Dystopia::GameObject::GetTags() const
{
	return mTags;
}

AutoArray<Tags> Dystopia::GameObject::GetAllTags() const
{
	return EngineCore::GetInstance()->Get<TagSystem>()->GetTagsAsArray(static_cast<Tags>(mTags));
}

AutoArray<HashString> Dystopia::GameObject::GetAllTags_Hashstr() const
{
	return EngineCore::GetInstance()->Get<TagSystem>()->ConvertTagsToHash(static_cast<Tags>(mTags));
}

AutoArray<std::string> Dystopia::GameObject::GetAllTags_str() const
{
	return EngineCore::GetInstance()->Get<TagSystem>()->ConvertTagsToString(static_cast<Tags>(mTags));
}

void Dystopia::GameObject::AddTag(HashString const& _TagName)
{
	mTags |= static_cast<unsigned>(EngineCore::GetInstance()->Get<TagSystem>()->GetTag(_TagName));
}

void Dystopia::GameObject::AddTag(Tags _tag)
{
	mTags |= static_cast<unsigned>(_tag);
}

void Dystopia::GameObject::AddTag(std::string const& _TagName)
{
	mTags |= static_cast<unsigned>(EngineCore::GetInstance()->Get<TagSystem>()->GetTag(HashString{ _TagName.c_str() }));
}

void Dystopia::GameObject::AddTag(const char* _TagName)
{
	mTags |= static_cast<unsigned>(EngineCore::GetInstance()->Get<TagSystem>()->GetTag(_TagName));
}

void Dystopia::GameObject::RemoveTag(Tags _Tag)
{
	mTags &= ~static_cast<unsigned>(_Tag);
}

void Dystopia::GameObject::ClearTags()
{
	mTags = 0;
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
	//ForcePing(mComponents, Init);
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
	ForcePing(mBehaviours, BehaviourDestroy);

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

void Dystopia::GameObject::OnTriggerEnter(GameObject* const _pOther)
{
	if (!_pOther) return;
	Ping(mBehaviours, OnTriggerEnter, _pOther);
}

void Dystopia::GameObject::OnTriggerStay(GameObject* const _pOther)
{
	if (!_pOther) return;
	Ping(mBehaviours, OnTriggerStay, _pOther);
}

void Dystopia::GameObject::OnTriggerExit(GameObject* const _pOther)
{
	if (!_pOther) return;
	Ping(mBehaviours, OnTriggerExit, _pOther);
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
	_out << mnID << mnFlags << mName << mTags;
	_out.InsertEndBlock("END_GO_DATA");

	_out.InsertStartBlock("START_GO_TRANSFORM");
	mTransform.Serialise(_out);
	_out.InsertEndBlock("END_GO_TRANSFORM");
}

void Dystopia::GameObject::Unserialise(TextSerialiser& _in)
{
	_in.ConsumeStartBlock();
	_in >> mnID >> mnFlags >> mName >> mTags;
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
		p->AddComponent(c->Duplicate(), Component::TAG{});
#if EDITOR
	for (auto& b : mBehaviours)
		p->AddComponent(EngineCore::GetInstance()->GetSystem<BehaviourSystem>()->RequestDuplicate(b, p->mnID), Behaviour::TAG{});
#endif

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
	for (auto& c : mComponents)
		c->SetOwner(this);
	for (auto& b : mBehaviours)
		b->SetOwner(this);
}

uint64_t Dystopia::GameObject::GetID(void) const
{
	return mnID;
}

//HashString Dystopia::GameObject::GetName(void) const
//{
//	return mName;
//}

const HashString& Dystopia::GameObject::GetName(void) const
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

#if EDITOR
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

Dystopia::GameObject& Dystopia::GameObject::operator=(const GameObject& _rhs)
{
	if (!(_rhs.GetFlags() & FLAG_EDITOR_OBJ))
		__debugbreak();

	if (mComponents.size() != _rhs.mComponents.size())
		__debugbreak();

	mnFlags = _rhs.mnFlags;
	mnFlags &= ~FLAG_EDITOR_OBJ;

	for (size_t i = 0; i < mComponents.size(); ++i)
	{
		auto c1 = mComponents[i];
		auto c2 = _rhs.mComponents[i];

		if (c1->GetRealComponentType() != c2->GetRealComponentType())
			__debugbreak();
	}

	for (unsigned n = 0; n < mComponents.size(); ++n)
	{
		mComponents[n]->DestroyComponent();
		mComponents.FastRemove(n);
	}
	for (unsigned n = 0; n < mBehaviours.size(); ++n)
	{
		mBehaviours[n]->DestroyComponent();
		mBehaviours.FastRemove(n);
	}

	for (size_t i = 0; i < _rhs.mComponents.size(); ++i)
		AddComponent(_rhs.mComponents[i]->Duplicate(), Component::TAG{});

	for (auto& b : _rhs.mBehaviours)
		AddComponent(EngineCore::GetInstance()->GetSystem<BehaviourSystem>()->RequestDuplicate(b, mnID), Behaviour::TAG{});

	for (auto c : mComponents)
		c->RemoveFlags(FLAG_EDITOR_OBJ);
	for (auto b: mComponents)
		b->RemoveFlags(FLAG_EDITOR_OBJ);

	return *this;
}
#endif 

#undef Ping
#undef ForcePing


