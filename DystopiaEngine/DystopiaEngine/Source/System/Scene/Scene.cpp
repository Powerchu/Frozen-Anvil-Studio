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
#include "System\Scene\Scene.h"
#include "Object\ObjectFlags.h"
#include "IO/TextSerialiser.h"

#include "Component/ComponentList.h"
#include "System/Driver/Driver.h"

#include <string>
#include <utility>


Dystopia::Scene::Scene(void) :
	mGameObjs{ 100 }
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

Dystopia::GameObject* Dystopia::Scene::FindGameObject(const std::string& _strName)
{
	for (auto& e : mGameObjs)
		if (_strName == e.GetName())
			return &e;

	return nullptr;
}

void Dystopia::Scene::Init(void)
{
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
		auto flag = e->GetFlags();

		if (flag & eObjFlag::FLAG_REMOVE)
		{
			mGameObjs.FastRemove(b);
			--e;
		}
		else if (flag & eObjFlag::FLAG_ACTIVE)
		{
			b->PostUpdate();
			++b;
		}
	}
}

void Dystopia::Scene::Shutdown(void)
{
	mGameObjs.clear();
}


void Dystopia::Scene::Serialise(TextSerialiser & _TextSerialiser) const
{
	_TextSerialiser.Write(std::to_string(mGameObjs.size()));

	for (auto & elem : mGameObjs)
	{
		_TextSerialiser.Write(elem.GetID());
		//_TextSerialiser.Write(elem.GetName());
	}
}

void Dystopia::Scene::Unserialise(TextSerialiser & _TextUnserialiser)
{
	size_t Size;
	_TextUnserialiser.Read(Size);
	for (int i = 0; i < Size; ++i)
	{
		unsigned long ID;
		_TextUnserialiser.Read(ID);
		mGameObjs.EmplaceBack(ID);

		auto & pGameObject = mGameObjs.back();
		pGameObject.Init();
	}

}



