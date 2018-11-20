/* HEADER *********************************************************************************/
/*!
\file	CommandTypes.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Utility/GUID.h"
#if EDITOR
#include "Editor/CommandTypes.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorClipboard.h"
#include "Editor/EditorFactory.h"

#include "Allocator/DefaultAlloc.h"

#include "Object/ObjectFlags.h"

#include "DataStructure/HashString.h"

#include "IO/TextSerialiser.h"

#include "System/File/FileSystem.h"

Editor::InsertGameObject::InsertGameObject(uint64_t _id, const Math::Pt3D& _pos)
	: mnObjID{ _id }, mSpawnPt{ _pos }
{}

bool Editor::InsertGameObject::Do(void)
{
	if (auto o = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().InsertGameObject(mnObjID))
	{
		HashString name{ "Game Object" };
		size_t endP = name.size() - 1;
		unsigned counter = 1;
		const auto& allObj = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().GetAllGameObjects();
		for (const auto& obj : allObj)
		{
			HashString tempName{ obj.GetNamePtr() };
			if (tempName == name)
			{
				if (endP + 1 < name.size())
					name.erase(endP + 1);
				name += counter++;
			}
		}
		o->SetFlag(Dystopia::eObjFlag::FLAG_LAYER_WORLD);
		o->SetFlag(Dystopia::eObjFlag::FLAG_ACTIVE);
		o->SetName(name.c_str());
		o->GetComponent<Dystopia::Transform>()->SetGlobalPosition(mSpawnPt);
		return true;
	}
	return false;
}

bool Editor::InsertGameObject::Undo(void)
{
	auto obj = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mnObjID);
	if (obj)
	{
		EditorMain::GetInstance()->GetSystem<EditorClipboard>()->RemoveGameObjectP(mnObjID);
		obj->Destroy();
		return true;
	}
	return false;
}

bool Editor::InsertGameObject::Unchanged(void) const
{
	return false;
}

Editor::DeleteGameObject::DeleteGameObject(uint64_t _id)
	: mnObjID{ _id }, mnTempFileID { Dystopia::GUIDGenerator::GetUniqueID() }
{
}

Editor::DeleteGameObject::~DeleteGameObject(void)
{
	auto fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
	auto fp = fs->GetFullPath("Temp", Dystopia::eFileDir::eResource);
	HashString file{ fp.c_str() };
	file += "\\";
	file += mnTempFileID;
	file += ".";
	file += Gbl::PREFAB_EXT;
	remove(file.c_str());
}

bool Editor::DeleteGameObject::Do(void)
{
	if (auto o = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mnObjID))
	{
		auto fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
		auto fp = fs->GetFullPath("Temp", Dystopia::eFileDir::eResource);
		HashString file{ fp.c_str() };
		file += "\\";
		file += mnTempFileID;
		file += ".";
		file += Gbl::PREFAB_EXT;
		auto serial = Dystopia::TextSerialiser::OpenFile(file.c_str(), Dystopia::Serialiser::MODE_WRITE);
		EditorMain::GetInstance()->GetSystem<EditorClipboard>()->RemoveGameObjectP(mnObjID);
		EditorMain::GetInstance()->GetSystem<EditorFactory>()->SaveAsPrefabTemp(mnObjID, serial);
		EditorMain::GetInstance()->GetSystem<EditorFactory>()->DettachPrefab(mnObjID);
		o->Destroy();
		return true;
	}
	return false;
}

bool Editor::DeleteGameObject::Undo(void)
{
	if (!Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mnObjID))
	{
		auto fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
		auto fp = fs->GetFullPath("Temp", Dystopia::eFileDir::eResource);
		HashString file{ fp.c_str() };
		file += "\\";
		file += mnTempFileID;
		file += ".";
		file += Gbl::PREFAB_EXT;
		auto serial = Dystopia::TextSerialiser::OpenFile(file.c_str(), Dystopia::Serialiser::MODE_READ);
		EditorMain::GetInstance()->GetSystem<EditorFactory>()->LoadIntoScene(serial);
		return true;
	}
	return false;
}

bool Editor::DeleteGameObject::Unchanged(void) const
{
	return false;
}


Editor::BatchExecute::BatchExecute(AutoArray<Command*>& _arr)
	: mArrCommands{ Ut::Move(_arr) }
{}

Editor::BatchExecute::~BatchExecute(void)
{
	for (auto& elem : mArrCommands)
		Dystopia::DefaultAllocator<Editor::Command>::DestructFree(elem);
}

bool Editor::BatchExecute::Do(void)
{
	bool pass = true;

	for (auto& c : mArrCommands)
		if (!c->Do())
			pass = false;
	return pass;
}

bool Editor::BatchExecute::Undo(void)
{
	bool pass = true;

	for (auto& c : mArrCommands)
		if (!c->Undo())
			pass = false;
	return pass;
}

bool Editor::BatchExecute::Unchanged(void) const
{
	return false;
}

Editor::SpawnPrefab::SpawnPrefab(const HashString& _prefab, const Math::Pt3D& _spawn)
	: mPrefab{ _prefab }, mSpawnPt{ _spawn }, mnID{ Dystopia::GUIDGenerator::INVALID }
{}

bool Editor::SpawnPrefab::Do(void)
{
	return EditorMain::GetInstance()->GetSystem<EditorFactory>()->SpawnPrefab(mPrefab, mSpawnPt, mnID);
}

bool Editor::SpawnPrefab::Undo(void)
{
	if (auto obj = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mnID))
	{
		if (EditorMain::GetInstance()->GetSystem<EditorFactory>()->DettachPrefab(mnID))
		{
			EditorMain::GetInstance()->GetSystem<EditorClipboard>()->RemoveGameObjectP(mnID);
			obj->Destroy();
			return true;
		}
		return false;
	}
	return false;
}

bool Editor::SpawnPrefab::Unchanged(void) const
{
	return false;
}

#endif







