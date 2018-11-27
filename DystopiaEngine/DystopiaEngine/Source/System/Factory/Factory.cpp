/* HEADER *********************************************************************************/
/*!
\file	Factory.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Factory/Factory.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/File/FileSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Behaviour/BehaviourSystemHelper.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Utility/GUID.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#include "Component/Component.h"
#include "Component/Transform.h"
#include "Behaviour/Behaviour.h"

#include "Editor/RuntimeMeta.h"

Dystopia::Factory::Factory(void)
{}

Dystopia::Factory::~Factory(void)
{
}

Dystopia::GameObject* Dystopia::Factory::SpawnPrefab(const HashString& _prefab, const Math::Pt3D& _pos)
{
	auto fs = EngineCore::GetInstance()->GetSubSystem<FileSystem>();
	auto fp = fs->GetFullPath("Prefab", eFileDir::eResource);
	HashString file{ fp.c_str() };
	file += "\\";
	file += _prefab;
	auto _in = Dystopia::TextSerialiser::OpenFile(file.c_str(), Dystopia::Serialiser::MODE_READ);

	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	size_t currentIndex = curScene.GetAllGameObjects().size();

	auto& obj = *curScene.InsertGameObject();
	unsigned count = LoadSegment(obj, _in);
	LoadSegmentC(obj, count, _in);
	LoadSegmentB(obj, _in);

	unsigned childCounter = 0;
	_in.ConsumeStartBlock();
	_in >> childCounter;
	_in.ConsumeEndBlock();

	for (unsigned i = 0; i < childCounter; ++i)
	{
		auto& childObj = *curScene.InsertGameObject();
		LoadSegmentC(childObj, LoadSegment(childObj, _in), _in);
		LoadSegmentB(childObj, _in);
	}

	for (size_t index = currentIndex; index < curScene.GetAllGameObjects().size(); ++index)
	{
		auto transform = curScene.GetAllGameObjects()[index].GetComponent<Dystopia::Transform>();
		uint64_t parentID = transform->GetParentID();
		for (size_t subIndex = currentIndex; subIndex < curScene.GetAllGameObjects().size(); ++subIndex)
		{
			if (parentID == curScene.GetAllGameObjects()[subIndex].GetID())
			{
				transform->SetParent(curScene.GetAllGameObjects()[subIndex].GetComponent<Dystopia::Transform>());
				break;
			}
		}
	}
	obj.Awake();
	obj.GetComponent<Transform>()->SetGlobalPosition(_pos);
	return &obj;
}

unsigned Dystopia::Factory::LoadSegment(GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned n = 0;
	_in.ConsumeStartBlock();
	_in >> n;
	_obj.Unserialise(_in);
	_in.ConsumeEndBlock();
	return n;
}

void Dystopia::Factory::LoadSegmentC(GameObject& _obj, unsigned _count, Dystopia::TextSerialiser& _in)
{
	ComponentList cList;
	unsigned sysID = 0;
	for (unsigned i = 0; i < _count; ++i)
	{
		_in.ConsumeStartBlock();
		_in >> sysID;
		Component *pComponent = cList.GetComponentA(sysID, &_obj);
		cList.IsolateUnserialise(pComponent, _in);

		pComponent->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		auto sceneSys = EngineCore::GetInstance()->GetSystem<SceneSystem>();
		sceneSys->GetCurrentScene().FindGameObject(pComponent->GetOwnerID())->AddComponent(pComponent, Component::TAG{});

		_in.ConsumeEndBlock();
	}
}

void Dystopia::Factory::LoadSegmentB(GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned num;
	_in.ConsumeStartBlock();
	_in >> num;
	for (unsigned i = 0; i < num; ++i)
	{
		HashString name;
		_in >> name;
		if (auto ptr = Dystopia::EngineCore::GetInstance()->Get<BehaviourSystem>()->RequestBehaviour(_obj.GetID(), name.c_str()))
		{
			unsigned size = 0;
			_in.ConsumeStartBlock();
			_in >> size;
			auto BehaviourMetadata = ptr->GetMetaData();
			if (BehaviourMetadata)
			{
				for (unsigned u = 0; u < size; ++u)
				{
					std::string MemVarName;
					_in >> MemVarName;
					if (BehaviourMetadata[MemVarName.c_str()])
					{
						_in.ConsumeStartBlock();
						BehaviourMetadata[MemVarName.c_str()].Unserialise(ptr, _in, Dystopia::BehaviourHelper::SuperUnserialiseFunctor{});
						_in.ConsumeEndBlock();
					}
					else
					{
						_in.ConsumeStartBlock();
						_in.ConsumeEndBlock();
					}
				}
			}
			ptr->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			ptr->SetOwner(&_obj);
			_obj.AddComponent(ptr, Dystopia::BehaviourTag{});
			_in.ConsumeEndBlock();
		}
		else
		{
			_in.ConsumeStartBlock();
			_in.ConsumeEndBlock();
		}
	}
	_in.ConsumeEndBlock();
	_obj.Identify();
}








