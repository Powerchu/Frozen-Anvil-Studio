/* HEADER *********************************************************************************/
/*!
\file	EditorFactory.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorFactory.h"
#include "Editor/EditorMain.h"
#include "Editor/RuntimeMeta.h"

#include "System/Driver/Driver.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Behaviour/BehaviourSystemHelper.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Behaviour/Behaviour.h"
#include "Component/Component.h"
#include "Component/Camera.h"
#include "Component/Transform.h"

#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#include "Utility/GUID.h"
#include "Utility/DebugAssert.h"

#include "Allocator/DefaultAlloc.h"

#include <string>
#include <fstream>
#include <iostream>

Editor::EditorFactory::EditorFactory(void)
	: mpSceneSys{ nullptr }, mArrFactoryObj{}
{}

Editor::EditorFactory::~EditorFactory(void)
{}

void Editor::EditorFactory::Load(void)
{}

bool Editor::EditorFactory::Init(void)
{
	mpSceneSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>();
	return true;
}

void Editor::EditorFactory::StartFrame(void)
{}

void Editor::EditorFactory::Update(float)
{}

void Editor::EditorFactory::EndFrame(void)
{}

void Editor::EditorFactory::Shutdown(void)
{}

void Editor::EditorFactory::Message(eEMessage)
{}

void Editor::EditorFactory::SaveSettings(Dystopia::TextSerialiser&) const
{}

void Editor::EditorFactory::LoadSettings(Dystopia::TextSerialiser&)
{}

void Editor::EditorFactory::DefaultSceneCamera(void)
{
	auto pCore = Dystopia::EngineCore::GetInstance();
	auto& scene = pCore->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	auto cam = pCore->GetSystem<Dystopia::CameraSystem>()->RequestComponent();
	cam->SetSurface(&(pCore->GetSystem<Dystopia::GraphicsSystem>()->GetView(3)));
	
	Dystopia::GameObject *pObject = scene.InsertGameObject(Dystopia::GUIDGenerator::GetUniqueID());
	pObject->SetName("Scene Camera");
	pObject->SetActive(true);
	pObject->AddComponent(cam, typename Dystopia::Camera::TAG{});
	pObject->Identify();
	pObject->SetFlag(Dystopia::eObjFlag::FLAG_ALL_LAYERS);
	cam->Awake();
}

bool Editor::EditorFactory::SpawnPrefab(const HashString& _prefName, const Math::Pt3D& _pos)
{
	HashString check{ _prefName };
	size_t pos = check.rfind(".");
	check.erase(pos);
	for (const auto& p : mArrPrefabData)
	{
		if (p.mPrefabFile == _prefName)
		{
			PutToScene(_prefName, _pos);
			return true;
		}
	}
	return false;
}

bool Editor::EditorFactory::SaveAsPrefab(const uint64_t& _objID, Dystopia::TextSerialiser& _out)
{
	Dystopia::GameObject *pObject = mpSceneSys->GetCurrentScene().FindGameObject(_objID);
	if (pObject)
	{
		if (SavePrefab(_objID, _out))
		{
			unsigned childCounter = 0;
			auto& allChild = pObject->GetComponent<Dystopia::Transform>()->GetAllChild();

			for (auto& c : allChild)
				if (c->GetOwner())
					RecursiveCounter(*c->GetOwner(), childCounter);

			_out.InsertStartBlock("Recursion Counter");
			_out << childCounter;
			_out.InsertEndBlock("Recursion Counter");

			for (auto& c : allChild)
				if (c->GetOwner())
					SaveChild(*c->GetOwner(), _out);

			return true;
		}
	}
	return false;
}

bool Editor::EditorFactory::LoadAsPrefab(const HashString& _path)
{
	auto _in = Dystopia::TextSerialiser::OpenFile(_path.c_str(), Dystopia::TextSerialiser::MODE_READ);
	size_t currentIndex = mArrFactoryObj.size();
	auto& obj = *mArrFactoryObj.Emplace(Dystopia::GUIDGenerator::GetUniqueID());
	if (LoadPrefab(obj, _in))
	{
		unsigned childCounter = 0;
		_in.ConsumeStartBlock();
		_in >> childCounter;
		_in.ConsumeEndBlock();

		for (unsigned i = 0; i < childCounter; ++i)
		{
			auto& childObj = *mArrFactoryObj.Emplace(Dystopia::GUIDGenerator::GetUniqueID());
			LoadChild(childObj, _in);
		}

		for (size_t index = currentIndex; index < mArrFactoryObj.size(); ++index)
		{
			auto transform = mArrFactoryObj[index].GetComponent<Dystopia::Transform>();
			uint64_t parentID = transform->GetParentID();

			for (size_t subIndex = currentIndex; subIndex < mArrFactoryObj.size(); ++subIndex)
			{
				if (parentID == mArrFactoryObj[subIndex].GetID())
				{
					transform->SetParent(mArrFactoryObj[subIndex].GetComponent<Dystopia::Transform>());
					break;
				}
			}
		}
		size_t cut = _path.rfind("\\");
		if (cut == HashString::nPos)
			cut = _path.rfind('/');
		cut++;

		mArrPrefabData.Emplace(PrefabData{HashString{ _path.cbegin() + cut, _path.cend() }, currentIndex, mArrFactoryObj.size() });
		return true;
	}
	return false;
}

void Editor::EditorFactory::SaveChild(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _out)
{
	const auto& allComponents = _obj.GetAllComponents();
	const auto& allBehaviours = _obj.GetAllBehaviours();
	SaveSegment(_obj, _out);
	SaveSegment(allComponents, _out);
	SaveSegment(allBehaviours, _out);

	auto& allChild = _obj.GetComponent<Dystopia::Transform>()->GetAllChild();
	for (auto& c : allChild)
		if (c->GetOwner())
			SaveChild(*c->GetOwner(), _out);
}

void Editor::EditorFactory::LoadChild(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	LoadPrefab(_obj, _in);
}

bool Editor::EditorFactory::SavePrefab(const uint64_t& _objID, Dystopia::TextSerialiser& _out)
{
	Dystopia::GameObject *pObject = mpSceneSys->GetCurrentScene().FindGameObject(_objID);

	if (pObject)
	{
		const auto& allComponents = pObject->GetAllComponents();
		const auto& allBehaviours = pObject->GetAllBehaviours();

		SaveSegment(*pObject, _out);
		SaveSegment(allComponents, _out);
		SaveSegment(allBehaviours, _out);

		return true;
	}
	return false;
}

bool Editor::EditorFactory::LoadPrefab(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned count = LoadSegment(_obj, _in);
	LoadSegmentC(_obj, count, _in);
	LoadSegmentB(_obj, _in);

	_obj.SetFlag(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
	auto& allC = _obj.GetAllComponents();
	auto& allB = _obj.GetAllBehaviours();
	for (auto& c : allC)
		c->SetFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& b: allB)
		b->SetFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
	return true;
}

void Editor::EditorFactory::SaveSegment(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _out)
{
	const auto& allComponents = _obj.GetAllComponents();
	_out.InsertStartBlock("GO_PREFAB");
	_out << allComponents.size();
	_obj.Serialise(_out);
	_out.InsertEndBlock("GO_PREFAB");
}

void Editor::EditorFactory::SaveSegment(const AutoArray<Dystopia::Component*>& _arrComponents, Dystopia::TextSerialiser& _out)
{
	for (const auto& c : _arrComponents)
	{
		_out.InsertStartBlock("Component");
		_out << c->GetRealComponentType();
		c->Serialise(_out);
		_out.InsertEndBlock("Component");
	}
}

void Editor::EditorFactory::SaveSegment(const AutoArray<Dystopia::Behaviour*>& _arrBehaviours, Dystopia::TextSerialiser& _out)
{
	_out.InsertStartBlock("All Behaviours Block");
	_out << _arrBehaviours.size();
	for (const auto& b : _arrBehaviours)
	{
		HashString name{ b->GetBehaviourName() };
		_out << name;
		_out.InsertStartBlock("Behaviour Block");
		auto metaData = b->GetMetaData();
		if (metaData)
		{
			auto allNames = metaData.GetAllNames();
			_out << allNames.size();
			for (const auto& n : allNames)
			{
				if (metaData[n])
				{
					std::string memVarName{ n };
					_out << memVarName;
					_out.InsertStartBlock("Member Variable");
					metaData[n].Serialise(b, _out, Dystopia::BehaviourHelper::SuperSerialiseFunctor{});
					_out.InsertEndBlock("Member Variable");
				}
			}
		}
		_out.InsertEndBlock("Behaviour Block");
	}
	_out.InsertEndBlock("All Behaviours Block");
}

unsigned Editor::EditorFactory::LoadSegment(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned n = 0;
	_in.ConsumeStartBlock();
	_in >> n;
	_obj.Unserialise(_in);
	_in.ConsumeEndBlock();
	return n;
}

void Editor::EditorFactory::LoadSegmentC(Dystopia::GameObject& _obj, unsigned _count, Dystopia::TextSerialiser& _in)
{
	Dystopia::ComponentList cList;
	unsigned sysID = 0;
	for (unsigned i = 0; i < _count; ++i)
	{
		_in.ConsumeStartBlock();
		_in >> sysID;
		Dystopia::Component *pComponent = cList.GetComponent(sysID, &_obj);
		_obj.AddComponent(pComponent, typename Dystopia::Component::TAG{});
		pComponent->Unserialise(_in);
		_in.ConsumeEndBlock();
	}
}

void Editor::EditorFactory::LoadSegmentB(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned num;
	_in.ConsumeStartBlock();
	_in >> num;
	for (unsigned i = 0; i < num; ++i)
	{
		HashString name;
		_in >> name;
		if (auto ptr = Dystopia::EngineCore::GetInstance()->Get<Dystopia::BehaviourSystem>()->RequestBehaviour(_obj.GetID(), name.c_str()))
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

void Editor::EditorFactory::RecursiveCounter(Dystopia::GameObject& _obj, unsigned& _counter)
{
	_counter++;

	auto& allChild = _obj.GetComponent<Dystopia::Transform>()->GetAllChild();
	for (auto& c : allChild)
		if (c->GetOwner())
			RecursiveCounter(*c->GetOwner(), _counter);
}

void Editor::EditorFactory::PutToScene(const HashString& _fileName, const Math::Pt3D& _pos)
{
	for (auto& p : mArrPrefabData)
	{
		if (p.mPrefabFile == _fileName)
		{
			p.mArrInstanced.push_back(AutoArray<uint64_t>{});
			PutToScene(p, _pos);
			break;
		}
	}
}

void Editor::EditorFactory::PutToScene(PrefabData& _prefab, const Math::Pt3D& _pos)
{
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	auto& arrInstance = _prefab.mArrInstanced.back();
	for (size_t i = _prefab.mnStart; i < _prefab.mnEnd; ++i)
	{
		auto& loadedO = mArrFactoryObj[i];
		auto insertedID = Dystopia::GUIDGenerator::GetUniqueID();
		arrInstance.push_back(insertedID);
		auto& insertedO = *(curScene.InsertGameObject(insertedID));

		insertedO.SetName(loadedO.GetName());
		auto loadedTransform = loadedO.GetComponent<Dystopia::Transform>();
		auto insertedTransform = insertedO.GetComponent<Dystopia::Transform>();
		insertedTransform->SetScale(loadedTransform->GetGlobalScale());
		insertedTransform->SetPosition(_pos);
		insertedTransform->SetRotation(loadedTransform->GetGlobalRotation());
		insertedO.ReplaceFlag(loadedO.GetFlag());
		insertedTransform->SetParentID(loadedTransform->GetParentID());

		auto& allC = loadedO.GetAllComponents();
		auto& allB = loadedO.GetAllBehaviours();

		insertedO.RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& c : allC)
		{
			auto dup = c->Duplicate();
			dup->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			insertedO.AddComponent(dup, Dystopia::Component::TAG{});
		}
		for (auto& b : allB)
		{
			auto dup = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::BehaviourSystem>()->RequestDuplicate(b, insertedO.GetID());
			dup->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			insertedO.AddComponent(dup, Dystopia::Behaviour::TAG{});
		}
	}
	
	for (size_t i = 0; i < _prefab.mnEnd - _prefab.mnStart; ++i)
	{
		auto transform = curScene.FindGameObject(arrInstance[i])->GetComponent<Dystopia::Transform>();
		uint64_t parentID = transform->GetParentID();

		for (size_t innerID = _prefab.mnStart; innerID < _prefab.mnEnd; ++innerID)
		{
			if (mArrFactoryObj[innerID].GetID() == parentID)
			{
				auto parent = curScene.FindGameObject(arrInstance[innerID - _prefab.mnStart]);
				if (parent)
					transform->SetParent(parent->GetComponent<Dystopia::Transform>());
				break;
			}
		}
	}
}

Editor::EditorFactory::PrefabData::PrefabData(const HashString& _fileName, size_t _start, size_t _end)
	: mPrefabFile{ _fileName }, mnStart{ _start }, mnEnd{ _end }
{}

#endif
