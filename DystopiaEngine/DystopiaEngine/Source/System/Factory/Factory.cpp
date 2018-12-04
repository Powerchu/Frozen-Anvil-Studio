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

#include "DataStructure/MagicArray.h"

Dystopia::Factory::Factory(void)
{}

Dystopia::Factory::~Factory(void)
{
}

Dystopia::GameObject* Dystopia::Factory::SpawnPrefab(const HashString& _prefab, const Math::Pt3D& _pos)
{
	auto fs = EngineCore::GetInstance()->GetSubSystem<FileSystem>();
	auto fp = fs->GetFullPath(_prefab.c_str(), eFileDir::eResource);
	if (!fp.size())
		return nullptr;
	auto _in = Dystopia::TextSerialiser::OpenFile(fp.c_str(), Dystopia::Serialiser::MODE_READ);

	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	//const size_t currentIndex = curScene.GetAllGameObjects().size();

	AutoArray<uint64_t> oldIDs;
	AutoArray<GameObject*> allInstantiated;

	auto& obj = *curScene.InsertGameObject();;
	allInstantiated.push_back(&obj);
	unsigned count = LoadSegment(obj, _in);
	LoadSegmentC(obj, count, _in);
	LoadSegmentB(obj, _in);

	oldIDs.push_back(obj.GetID());
	obj.SetID(GUIDGenerator::GetUniqueID());

	unsigned childCounter = 0;
	_in.ConsumeStartBlock();
	_in >> childCounter;
	_in.ConsumeEndBlock();

	for (unsigned i = 0; i < childCounter; ++i)
	{
		auto& childObj = *curScene.InsertGameObject();
		allInstantiated.push_back(&childObj);
		LoadSegmentC(childObj, LoadSegment(childObj, _in), _in);
		LoadSegmentB(childObj, _in);

		oldIDs.push_back(childObj.GetID());
		obj.SetID(GUIDGenerator::GetUniqueID());
	}

	for (size_t i = 0; i < allInstantiated.size(); ++i)
	{
		auto transform = allInstantiated[i]->GetComponent<Dystopia::Transform>();
		uint64_t parentID = transform->GetParentID();
		
		for (size_t j = 0; j < allInstantiated.size(); ++j)
		{
			if (parentID == oldIDs[j])
			{
				transform->SetParentID(allInstantiated[j]->GetID());
				transform->SetParent(allInstantiated[j]->GetComponent<Dystopia::Transform>());
				break;
			}
		}
	}
	for (size_t i = 0; i < allInstantiated.size(); ++i)
	{
		allInstantiated[i]->Awake();
		allInstantiated[i]->RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	}
	for (size_t i = 0; i < allInstantiated.size(); ++i)
		Dystopia::SystemList<std::make_index_sequence<Ut::SizeofList<Dystopia::UsableComponents>::value>>::InitDonors(allInstantiated[i]->GetID());

	//auto& allGameObject = curScene.GetAllGameObjects();
	//for (size_t index = currentIndex; index < allGameObject.size(); ++index)
	//{
	//	auto transform = allGameObject[index].GetComponent<Dystopia::Transform>();
	//	uint64_t parentID = transform->GetParentID();
	//
	//	for (size_t subIndex = currentIndex; subIndex < allGameObject.size(); ++subIndex)
	//	{
	//		if (parentID == oldIDs[subIndex - currentIndex])
	//		{
	//			transform->SetParentID(allGameObject[subIndex].GetID());
	//			transform->SetParent(allGameObject[subIndex].GetComponent<Dystopia::Transform>());
	//			break;
	//		}
	//	}
	//}


	//for (size_t index = currentIndex; index < curScene.GetAllGameObjects().size(); ++index)
	//	curScene.GetAllGameObjects()[index].Awake();
	//for (size_t index = currentIndex; index < curScene.GetAllGameObjects().size(); ++index)
	//{
	//	curScene.GetAllGameObjects()[index].RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	//	Dystopia::SystemList<std::make_index_sequence<Ut::SizeofList<Dystopia::UsableComponents>::value>>::InitDonors(curScene.GetAllGameObjects()[index].GetID());
	//}

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
		sceneSys->GetCurrentScene().FindGameObject(_obj.GetID())->AddComponent(pComponent, Component::TAG{});
		_in.ConsumeEndBlock();
	}
}

void Dystopia::Factory::LoadSegmentB(GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned num;
	_in.ConsumeStartBlock();
	_in >> num;
	_in.ConsumeEndBlock();

	for (unsigned i = 0; i < num; ++i)
	{
		HashString name;

		_in.ConsumeStartBlock();
		_in >> name;
		_in.ConsumeEndBlock();

		unsigned size = 0;

		_in.ConsumeStartBlock();
		_in >> size;
		_in.ConsumeEndBlock();

		if (auto ptr = Dystopia::EngineCore::GetInstance()->Get<Dystopia::BehaviourSystem>()->RequestBehaviour(_obj.GetID(), name.c_str()))
		{
			auto BehaviourMetadata = ptr->GetMetaData();
			if (BehaviourMetadata)
			{
				for (unsigned u = 0; u < size; ++u)
				{
					std::string MemVarName;

					_in.ConsumeStartBlock();
					_in >> MemVarName;
					_in.ConsumeEndBlock();

					if (BehaviourMetadata[MemVarName.c_str()])
					{
						_in.ConsumeStartBlock();
						/*Call Unserialise*/
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
		}
		else
		{
			for (unsigned j = 0; j < size; ++j)
			{
				_in.ConsumeStartBlock();
				_in.ConsumeEndBlock();
				_in.ConsumeStartBlock();
				_in.ConsumeEndBlock();
			}
		}
	}
	_obj.Identify();
}








