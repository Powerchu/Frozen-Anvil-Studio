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
#if EDITOR
#include "Editor/CommandTypes.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorClipboard.h"
#include "Editor/EditorFactory.h"

#include "Allocator/DefaultAlloc.h"

#include "Object/ObjectFlags.h"

#include "DataStructure/HashString.h"

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
		o->GetComponent<Dystopia::Transform>()->SetPosition(mSpawnPt);
		return true;
	}
	return false;
}

bool Editor::InsertGameObject::Undo(void)
{
	auto obj = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mnObjID);
	if (obj)
	{
		obj->Destroy();
		EditorMain::GetInstance()->GetSystem<EditorClipboard>()->RemoveGameObject(mnObjID);
		return true;
	}
	return false;
}

bool Editor::InsertGameObject::Unchanged(void) const
{
	return false;
}

Editor::DeleteGameObject::DeleteGameObject(uint64_t _id)
	: mnObjID{ _id }
{}

bool Editor::DeleteGameObject::Do(void)
{
	if (auto o = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(mnObjID))
	{



		o->Destroy();
		EditorMain::GetInstance()->GetSystem<EditorClipboard>()->RemoveGameObject(mnObjID);
		return true;
	}
	return false;
}

bool Editor::DeleteGameObject::Undo(void)
{
	return true;
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

#endif







