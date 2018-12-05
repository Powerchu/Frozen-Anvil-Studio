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

#include <string>

Editor::EditorFactory::EditorFactory(void)
	: mpSceneSys{ nullptr }, mArrFactoryObj{}, mnLastSceneID{0}
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
{
	mnLastSceneID = Dystopia::EngineCore::Get<Dystopia::SceneSystem>()->GetCurrentScene().GetSceneID();
}

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
		//for (auto& object : mArrFactoryObj)
		//{
		//	auto& allC = object.GetAllComponents();
		//	for (unsigned n = 0; n < allC.size(); n++)
		//		object.RemoveComponent(allC[n--]);
		//	auto& allB = object.GetAllBehaviours();
		//	for (unsigned n = 0; n < allB.size(); n++)
		//		object.RemoveComponent(allB[n--]);
		//}
	}
	else if (_msg == eEMessage::SCENE_CHANGED)
	{
		//if (Dystopia::EngineCore::Get<Dystopia::SceneSystem>()->GetCurrentScene().GetSceneID() != mnLastSceneID)
		//{
		//	AutoArray<HashString> mArrTempPrefab;
		//	for (auto& dat : mArrPrefabData)
		//		mArrTempPrefab.push_back(dat.mPrefabFile);
		//
		//	mArrPrefabData.clear();
		//	mArrFactoryObj.clear();
		//
		//	for (auto& p : mArrTempPrefab)
		//		LoadAsPrefab(p);
		//}
		//else
		//{
			ValidatePrefabInstances();
			//for (size_t i = 0; i < mArrPrefabData.size(); ++i)
			//{
			//	if (!mArrPrefabData[i].mArrInstanced.size())
			//	{
			//		HashString prefName{ mArrPrefabData[i].mPrefabFile };
			//
			//		for (size_t j = mArrPrefabData[i].mnStart; j < mArrPrefabData[i].mnEnd; j++)
			//			mArrFactoryObj.Remove(&mArrFactoryObj[j]);
			//
			//		mArrPrefabData.Remove(&mArrPrefabData[i]);
			//		LoadAsPrefab(prefName.c_str());
			//	}
			//}
		//}
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
	cam->SetSurface(3);
	cam->SetActive(true);

	Dystopia::GameObject *pObject = scene.InsertGameObject(Dystopia::GUIDGenerator::GetUniqueID());
	pObject->SetName("___Scene_Camera___");
	pObject->AddComponent(cam, typename Dystopia::Camera::TAG{});
	pObject->Identify();
	pObject->SetFlag(Dystopia::eObjFlag::FLAG_LAYER_WORLD);
	pObject->SetActive(true);
	cam->Awake();
}

void Editor::EditorFactory::DefaultGameCamera(void)
{
	const auto pCore = Dystopia::EngineCore::GetInstance();
	auto& scene = pCore->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	auto cam = pCore->GetSystem<Dystopia::CameraSystem>()->RequestComponent();
	cam->SetSurface(0);
	cam->SetMasterCamera();
	cam->SetActive(true);

	Dystopia::GameObject *pObject = scene.InsertGameObject(Dystopia::GUIDGenerator::GetUniqueID());
	pObject->SetName("Main Camera");
	pObject->AddComponent(cam, typename Dystopia::Camera::TAG{});
	pObject->Identify();
	pObject->SetFlag(Dystopia::eObjFlag::FLAG_LAYER_WORLD);
	pObject->SetActive(true);
	cam->Awake();
}

void Editor::EditorFactory::DefaultUICamera(void)
{
	auto pCore = Dystopia::EngineCore::GetInstance();
	auto& scene = pCore->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	auto cam = pCore->GetSystem<Dystopia::CameraSystem>()->RequestComponent();
	cam->SetSurface(1);
	cam->SetActive(true);

	Dystopia::GameObject *pObject = scene.InsertGameObject(Dystopia::GUIDGenerator::GetUniqueID());
	pObject->SetName("UI Camera");
	pObject->AddComponent(cam, typename Dystopia::Camera::TAG{});
	pObject->Identify();
	pObject->SetFlag(Dystopia::eObjFlag::FLAG_LAYER_UI);
	pObject->SetActive(true);
	cam->Awake();
}

bool Editor::EditorFactory::ReattachToPrefab(Dystopia::Component* _p, uint64_t c, bool _amComponent)
{
	for (auto& object : mArrFactoryObj)
	{
		if (_amComponent)
		{
			if (c == object.GetID())
			{
				object.AddComponent(_p, Dystopia::Component::TAG{});
				return true;
			}
		}
		else
		{
			if (c == object.GetID())
			{
				auto b = static_cast<Dystopia::Behaviour*>(_p);
				object.AddComponent(b, Dystopia::Behaviour::TAG{});
				return true;
			}
		}
	}

	return false;
}

bool Editor::EditorFactory::SpawnPrefab(const HashString& _prefName, const Math::Pt3D& _pos, uint64_t& _outRootObjID)
{
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

bool Editor::EditorFactory::LoadAsPrefab(const HashString& _name)
{
	auto _path = Dystopia::EngineCore::Get<Dystopia::FileSystem>()->FindFilePath(_name.c_str(), Dystopia::eFileDir::eResource);
	auto _in = Dystopia::TextSerialiser::OpenFile(_path.c_str(), Dystopia::TextSerialiser::MODE_READ);
	AutoArray<Dystopia::GameObject*> mLoadedObj;
	mLoadedObj.push_back(mArrFactoryObj.Emplace(Dystopia::GUIDGenerator::GetUniqueID()));
	auto& obj = *mLoadedObj.back();
	if (LoadPrefab(obj, _in))
	{
		unsigned childCounter = 0;
		_in.ConsumeStartBlock();
		_in >> childCounter;
		_in.ConsumeEndBlock();

		for (unsigned i = 0; i < childCounter; ++i)
		{
			mLoadedObj.push_back(mArrFactoryObj.Emplace(Dystopia::GUIDGenerator::GetUniqueID()));
			auto& childObj = *mLoadedObj.back();
			LoadChild(childObj, _in);
		}

		for (size_t index = 0; index < mLoadedObj.size(); ++index)
		{
			auto transform = mLoadedObj[index]->GetComponent<Dystopia::Transform>();
			uint64_t parentID = transform->GetParentID();

			for (size_t subIndex = 0; subIndex < mLoadedObj.size(); ++subIndex)
			{
				if (subIndex == index)
					continue;

				if (parentID == mLoadedObj[subIndex]->GetID())
				{
					transform->SetParent(mLoadedObj[subIndex]->GetComponent<Dystopia::Transform>());
					break;
				}
			}
		}

		for (auto& data : mArrPrefabData)
		{
			if (data.mPrefabFile == _name)
			{
				for (auto p : data.mArrObjects)
					mArrFactoryObj.Remove(&*p);

				data.mArrObjects.clear();
				for (auto p : mLoadedObj)
					data.mArrObjects.push_back(p);
				return true;
			}
		}

		size_t cut = _path.rfind("\\");
		if (cut == HashString::nPos)
			cut = _path.rfind('/');
		cut++;

		auto pref = mArrPrefabData.Emplace(PrefabData{HashString{ _path.cbegin() + cut, _path.cend() }});
		for (auto p : mLoadedObj)
			pref->mArrObjects.push_back(p);
		return true;
	}
	return false;
}

void Editor::EditorFactory::LoadIntoScene(Dystopia::TextSerialiser& _in)
{
	AutoArray<Dystopia::GameObject*> allSpawned;
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	//size_t currentIndex = curScene.GetAllGameObjects().size();

	auto& obj = *curScene.InsertGameObject();
	allSpawned.push_back(&obj);
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
		allSpawned.push_back(&childObj);
		LoadSegmentC(childObj, LoadSegment(childObj, _in), _in);
		LoadSegmentB(childObj, _in);

		childObj.RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& c : childObj.GetAllComponents())
			c->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& b : childObj.GetAllBehaviours())
			b->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
	}

	for (size_t index = 0; index < allSpawned.size(); ++index)
	{
		auto transform = allSpawned[index]->GetComponent<Dystopia::Transform>();
		uint64_t parentID = transform->GetParentID();
		for (size_t subIndex = 0; subIndex < allSpawned.size(); ++subIndex)
		{
			if (parentID == allSpawned[subIndex]->GetID())
			{
				transform->SetParent(allSpawned[subIndex]->GetComponent<Dystopia::Transform>());
				break;
			}
		}
	}
	
	for (auto o : allSpawned)
		o->Awake();

	//auto& allObj = curScene.GetAllGameObjects();
	//for (size_t index = currentIndex; index < allObj.size(); ++index)
	//{
	//	auto transform = allObj[index].GetComponent<Dystopia::Transform>();
	//	uint64_t parentID = transform->GetParentID();
	//	for (size_t subIndex = currentIndex; subIndex < allObj.size(); ++subIndex)
	//	{
	//		if (parentID == allObj[subIndex].GetID())
	//		{
	//			transform->SetParent(allObj[subIndex].GetComponent<Dystopia::Transform>());
	//			break;
	//		}
	//	}
	//}
	//
	//for (size_t index = currentIndex; index < allObj.size(); ++index)
	//	allObj[index].Awake();
}

bool Editor::EditorFactory::FindMasterPrefab(const HashString& _prefabName, PrefabData*& _outP)
{
	for (auto& prefData : mArrPrefabData)
	{
		if (prefData.mPrefabFile == _prefabName)
		{
			_outP = &prefData;
			return true;
		}
	}
	return false;
}

MagicArray<Dystopia::GameObject>& Editor::EditorFactory::GetAllFactoryObjects(void)
{
	return mArrFactoryObj;
}

//Editor::EditorFactory::PrefabData* Editor::EditorFactory::GetPrefabData(const int& _id)
//{
//	//for (auto& data : mArrPrefabData)
//	//{
//	//	if (data.mnStart == _id)
//	//		return &data;
//	//}
//	return nullptr;
//}

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
	const unsigned count = LoadSegment(_obj, _in);
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
		_out.InsertEndBlock("Component");
	}
}

void Editor::EditorFactory::SaveSegment(const AutoArray<Dystopia::Behaviour*>& _arrBehaviours, Dystopia::TextSerialiser& _out)
{
	_out.InsertStartBlock("Number of Behaviours");
	_out << _arrBehaviours.size();
	_out.InsertEndBlock("Number of Behaviours");

	for (const auto& b : _arrBehaviours)
	{
		_out.InsertStartBlock("Behaviour Names");
		HashString name{ b->GetBehaviourName() };
		_out << name;
		_out.InsertEndBlock("Behaviour Names");

		auto metaData = b->GetMetaData();
		if (metaData)
		{
			auto allNames = metaData.GetAllNames();

			_out.InsertStartBlock("Behaviour Meta Count");
			_out << allNames.size();
			_out.InsertEndBlock("Behaviour  Meta Count");

			for (const auto& n : allNames)
			{
				if (metaData[n])
				{
					std::string memVarName{ n };

					_out.InsertStartBlock(memVarName.c_str());
					_out << memVarName;
					_out.InsertEndBlock(memVarName.c_str());

					_out.InsertStartBlock("Member Variable");
					metaData[n].Serialise(b, _out, Dystopia::BehaviourHelper::SuperSerialiseFunctor{});
					_out.InsertEndBlock("Member Variable");
					
				}
			}
		}
		else
		{
			_out.InsertStartBlock("Behaviour Meta Count");
			_out << 0;
			_out.InsertEndBlock("Behaviour  Meta Count");
		}
	}
}

unsigned Editor::EditorFactory::LoadSegment(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
{
	unsigned n = 0;
	_in.ConsumeStartBlock();
	_in >> n;
	_obj.Unserialise(_in);
	_in.ConsumeEndBlock();
	_obj.GetComponent<Dystopia::Transform>()->SetOpacity(1.f);
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
		cList.IsolateUnserialise(pComponent, _in);
		_in.ConsumeEndBlock();
	}
}

void Editor::EditorFactory::LoadSegmentB(Dystopia::GameObject& _obj, Dystopia::TextSerialiser& _in)
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
	AutoArray<Dystopia::GameObject*> allSpawned;
	auto& curScene = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene();
	//auto initalIndex = curScene.GetAllGameObjects().size();
	auto& arrInstance = _prefab.mArrInstanced.back();
	for (size_t i = 0; i < _prefab.mArrObjects.size(); ++i)
	{
		auto& loadedO = *_prefab.mArrObjects[i];
		auto insertedID = Dystopia::GUIDGenerator::GetUniqueID();
		arrInstance.push_back(insertedID);
		auto& insertedO = *(curScene.InsertGameObject(insertedID));
		allSpawned.push_back(&insertedO);

		insertedO.SetName(loadedO.GetName());
		auto loadedTransform = loadedO.GetComponent<Dystopia::Transform>();
		auto insertedTransform = insertedO.GetComponent<Dystopia::Transform>();
		insertedTransform->SetScale(loadedTransform->GetGlobalScale());
		insertedTransform->SetPosition(loadedTransform->GetGlobalPosition());
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
			if(dup == nullptr)
				dup = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::BehaviourSystem>()->RequestBehaviour(insertedO.GetID(), b->GetBehaviourName());
			if (dup)
			{
				dup->RemoveFlags(Dystopia::eObjFlag::FLAG_EDITOR_OBJ);
				insertedO.AddComponent(dup, Dystopia::Behaviour::TAG{});
			}
		}
	}
	
	for (size_t i = 0; i < _prefab.mArrObjects.size(); ++i)
	{
		auto transform = curScene.FindGameObject(arrInstance[i])->GetComponent<Dystopia::Transform>();
		uint64_t parentID = transform->GetParentID();

		for (size_t innerID = 0; innerID < _prefab.mArrObjects.size(); ++innerID)
		{
			if (i == innerID)
				continue;

			if (_prefab.mArrObjects[innerID]->GetID() == parentID)
			{
				auto parent = curScene.FindGameObject(arrInstance[innerID]);
				if (parent)
					transform->SetParent(parent->GetComponent<Dystopia::Transform>());
				break;
			}
		}
	}

	if (allSpawned.size())
		allSpawned[0]->GetComponent<Dystopia::Transform>()->SetGlobalPosition(_pos);

	//if (initalIndex < curScene.GetAllGameObjects().size())
	//	curScene.GetAllGameObjects()[initalIndex].GetComponent<Dystopia::Transform>()->SetGlobalPosition(_pos);
	DEBUG_ASSERT(!arrInstance.size(), "Error at spawning prefab");
	return arrInstance[0];
}

void Editor::EditorFactory::ApplyChanges(Editor::EditorFactory::PrefabData* _p)
{
	if (!_p) return;

	auto& curScene = Dystopia::EngineCore::Get<Dystopia::SceneSystem>()->GetCurrentScene();
	auto& arrInstances = _p->mArrInstanced;

	for (auto& instArr : arrInstances)
	{
		for (size_t i = 0; i < instArr.size(); ++i)
		{
			auto pObj = curScene.FindGameObject(instArr[i]);
	
			if (!pObj)  break;
			if (i < _p->mArrObjects.size())
				*pObj = *_p->mArrObjects[i];
		}
	}
}

Editor::EditorFactory::PrefabData::PrefabData(const HashString& _fileName)
	: mPrefabFile{ _fileName }, mArrObjects{}
{}

#endif
