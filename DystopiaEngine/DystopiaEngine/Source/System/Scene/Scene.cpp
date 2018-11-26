/* HEADER *********************************************************************************/
/*!
\file	Scene.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Scene

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Scene/Scene.h"
#include "Object/ObjectFlags.h"
#include "IO/TextSerialiser.h"

#include "Component/ComponentList.h"
#include "System/Driver/Driver.h"
#include "Utility/GUID.h"

#include <string>
#include <utility>


Dystopia::Scene::Scene(void) :
	mGameObjs{ }, mName{ "Untitled" }, mID{ GUIDGenerator::GetUniqueID() }
{
}

Dystopia::Scene::~Scene(void)
{
}

Dystopia::GameObject* Dystopia::Scene::FindGameObject(const uint64_t _id)
{
	for (auto& e : mGameObjs)
		if (_id == e.GetID())
			return &e;

	return nullptr;
}

Dystopia::GameObject* Dystopia::Scene::FindGameObject(const HashString& _strName)
{
	for (auto& e : mGameObjs)
		if (_strName == e.GetName())
			return &e;

	return nullptr;
}

void Dystopia::Scene::Init(void)
{
	for (auto& e : mGameObjs)
		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE)
			e.Init();
}

void Dystopia::Scene::FixedUpdate(float _dt)
{
	for (auto& e : mGameObjs)
		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE)
			e.FixedUpdate(_dt);
}

void Dystopia::Scene::Update(float _dt)
{
	for (auto& e : mGameObjs)
		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE)
			e.Update(_dt);
}

void Dystopia::Scene::PostUpdate(void)
{
	auto b = mGameObjs.begin();
	auto e = mGameObjs.end();

	while (b != e)
	{
		auto flag = b->GetFlags();

		if (flag & eObjFlag::FLAG_REMOVE)
		{
			b->GetComponent<Transform>()->RemoveParent();
			mGameObjs.Remove(&*b);
		}
		else if (flag & eObjFlag::FLAG_ACTIVE)
		{
			b->PostUpdate();
		}

		++b;
	}
}

void Dystopia::Scene::Shutdown(void)
{
	for (auto& g : mGameObjs)
		g.Destroy();
	mGameObjs.clear();
}

void Dystopia::Scene::Serialise(TextSerialiser & _TextSerialiser) const
{
	_TextSerialiser << mID;
	_TextSerialiser << mName;
	_TextSerialiser << mGameObjs.size();

	for (auto & elem : mGameObjs)
		elem.Serialise(_TextSerialiser);
}

void Dystopia::Scene::Unserialise(TextSerialiser & _TextUnserialiser)
{
	size_t Size;
	_TextUnserialiser >> mID;
	_TextUnserialiser >> mName;
	_TextUnserialiser >> Size;

	if (!mID)
		mID = GUIDGenerator::GetUniqueID();
	if (!mName.size())
		mName = "Untitled";

	for (int i = 0; i < Size; ++i)
	{
		auto pGameObj = InsertGameObject();
		pGameObj->Unserialise(_TextUnserialiser);
	}
}

uint64_t Dystopia::Scene::GetSceneID(void) const
{
	return mID;
}

void Dystopia::Scene::SetSceneName(const std::string& _name)
{
	mName = _name;
}

std::string Dystopia::Scene::GetSceneName(void) const
{
	return mName;
}

