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
#include "Editor/EditorResource.h"
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

void Editor::EditorFactory::Message(eEMessage _msg)
{
	if (_msg == eEMessage::SCENE_ABOUT_TO_CHANGE)
	{
		for (auto& object : mArrFactoryObj)
		{
			auto& allC = object.GetAllComponents();
			for (auto& c : allC)
				object.RemoveComponent(c);
			auto& allB = object.GetAllBehaviours();
			for (auto& b : allB)
				object.RemoveComponent(b);
		}
	}
	else if (_msg == eEMessage::SCENE_CHANGED)
	{
		ValidatePrefabInstances();
	}
}

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

void Editor::EditorFactory::ReattachToPrefab(Dystopia::Component* _p)
{
	for (auto& object : mArrFactoryObj)
	{
		if (_p->GetOwnerID() == object.GetID())
		{
			object.AddComponent(_p, Dystopia::Component::TAG{});
			return;
		}
	}
}

bool Editor::EditorFactory::SpawnPrefab(const HashString& _prefName, const Math::Pt3D& _pos, uint64_t& _outRootObjID)
{
	HashString check{ _prefName };
	size_t pos = check.rfind(".");
	check.erase(pos);
	for (const auto& p : mArrPrefabData)
	{
		if (p.mPrefabFile == _prefName)
		{
			_outRootObjID = PutToScene(_prefName, _pos);
			return true;
		}
	}
	return false;
}

bool Editor::EditorFactory::SaveAsPrefab(const uint64_t& _objID, Dystopia::TextSerialiser& _out, bool _temp)
{
	Dystopia::GameObject *pObject = mpSceneSys->GetCurrentScene().FindGameObject(_objID);
	if (pObject)
	{
		if (SavePrefab(_objID, _out, _temp))
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
					SaveChild(*c->GetOwner(), _out, _temp);

			return true;
		}
	}
	return false;
}

bool Editor::EditorFactory::SaveAsPrefabTemp(const uint64_t& _objID, Dystopia::TextSerialiser& _out)
{
	return SaveAsPrefab(_objID, _out, true);
}

bool Editor::EditorFactory::DettachPrefab(const uint64_t& _relatedObj)
{
	for (auto &data : mArrPrefabData)
	{
		for (size_t j = 0; j < data.mArrInstanced.size(); j++)
		{
			bool remove = false;
			for (size_t i = 0; i < data.mArrInstanced[j].size(); ++i)
			{
				if (data.mArrInstanced[j][i] == _relatedObj)
				{
					remove = true;
					break;
				}
			}
			if (remove)
			{
				data.mArrInstanced.FastRemove(j);
				j--;
				return true;
			}
		}
	}
	return false;
}

void Editor::EditorFactory::ValidatePrefabInstances(void)
{
	auto& sceneSys = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	for (auto &data : mArrPrefabData)
	{
		for (size_t j = 0; j < data.mArrInstanced.size(); j++)
		{
			bool remove = false;
			for (size_t i = 0; i < data.mArrInstanced[j].size(); ++i)
			{
				if (!sceneSys.FindGameObject(data.mArrInstanced[j][i]))
				{
					remove = true;
					break;
				}
			}
			if (remove)
			{
				data.mArrInstanced.FastRemove(j);
				j--;
			}
		}
	}
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

void Editor::EditorFactory::LoadIntoScene(Dystopia::TextSerialiser& _in)
{
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	size_t currentIndex = curScene.GetAllGameObjects().size();

	auto& obj = *curScene.InsertGameObject();
	unsigned count = LoadSegment(obj, _in);
	LoadSegmentC(obj, count, _in);
	LoadSegmentB(obj, _in);

	obj.RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : obj.GetAllComponents())
		c->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& b : obj.GetAllBehaviours())
		b->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);

	unsigned childCounter = 0;
	_in.ConsumeStartBlock();
	_in >> childCounter;
	_in.ConsumeEndBlock();

	for (unsigned i = 0; i < childCounter; ++i)
	{
		auto& childObj = *curScene.InsertGameObject();
		LoadSegmentC(childObj, LoadSegment(childObj, _in), _in);
		LoadSegmentB(childObj, _in);

		childObj.RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& c : childObj.GetAllComponents())
			c->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& b : childObj.GetAllBehaviours())
			b->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
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
}

/********************************************** Private Fn Definition **********************************************/

void Editor::EditorFactory::SaveChild(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _out, bool _temp)
{
	auto& allC = _obj.GetAllComponents();
	auto& allB = _obj.GetAllBehaviours();
	if (!_temp)
	{
		_obj.SetFlag(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& c : allC)
			c->SetFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& b : allB)
			b->SetFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
	}

	SaveSegment(_obj, _out);
	SaveSegment(_obj, allC, _out);
	SaveSegment(allB, _out);

	if (!_temp)
	{
		_obj.RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& c : allC)
			c->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& b : allB)
			b->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
	}

	auto& allChild = _obj.GetComponent<Dystopia::Transform>()->GetAllChild();
	for (auto& c : allChild)
		if (c->GetOwner())
			SaveChild(*c->GetOwner(), _out);
}

void Editor::EditorFactory::LoadChild(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	LoadPrefab(_obj, _in);
}

bool Editor::EditorFactory::SavePrefab(const uint64_t& _objID, Dystopia::TextSerialiser& _out, bool _temp)
{
	Dystopia::GameObject *pObject = mpSceneSys->GetCurrentScene().FindGameObject(_objID);

	if (pObject)
	{
		const auto& allC = pObject->GetAllComponents();
		const auto& allB = pObject->GetAllBehaviours();

		if (!_temp)
		{
			pObject->SetFlag(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			for (auto& c : allC)
				c->SetFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			for (auto& b : allB)
				b->SetFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		}

		SaveSegment(*pObject, _out);
		SaveSegment(*pObject, allC, _out);
		SaveSegment(allB, _out);

		if (!_temp)
		{
			pObject->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			for (auto& c : allC)
				c->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
			for (auto& b : allB)
				b->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		}
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

void Editor::EditorFactory::SaveSegment(Dystopia::GameObject& _obj, const AutoArray<Dystopia::Component*>& _arrComponents, Dystopia::TextSerialiser& _out)
{
	Dystopia::ComponentList cList;
	for (const auto& c : _arrComponents)
	{
		_out.InsertStartBlock("Component");
		_out << c->GetRealComponentType();
		cList.IsolateSerialise(c->GetRealComponentType(), &_obj, _out);
		//c->Serialise(_out);
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
		
		Dystopia::Component *pComponent = cList.GetComponentA(sysID, &_obj);
		/*pComponent->Unserialise(_in);*/
		cList.IsolateUnserialise(pComponent, _in);
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

uint64_t Editor::EditorFactory::PutToScene(const HashString& _fileName, const Math::Pt3D& _pos)
{
	for (auto& p : mArrPrefabData)
	{
		if (p.mPrefabFile == _fileName)
		{
			p.mArrInstanced.push_back(AutoArray<uint64_t>{});
			return PutToScene(p, _pos);
		}
	}
	return Dystopia::GUIDGenerator::INVALID;
}

uint64_t Editor::EditorFactory::PutToScene(PrefabData& _prefab, const Math::Pt3D& _pos)
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
	DEBUG_ASSERT(!arrInstance.size(), "Error at spawning prefab");
	return arrInstance[0];
}

Editor::EditorFactory::PrefabData::PrefabData(const HashString& _fileName, size_t _start, size_t _end)
	: mPrefabFile{ _fileName }, mnStart{ _start }, mnEnd{ _end }
{}

#endif
