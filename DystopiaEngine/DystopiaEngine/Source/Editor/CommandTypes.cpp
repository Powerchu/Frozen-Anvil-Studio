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
#include "Allocator/DefaultAlloc.h"

Editor::InsertGameObject::InsertGameObject(uint64_t _id)
	: mnObjID{ _id }
{}

bool Editor::InsertGameObject::Do(void)
{
	return true;
}

bool Editor::InsertGameObject::Undo(void)
{
	return true;
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
	return true;
}

bool Editor::DeleteGameObject::Undo(void)
{
	return true;
}

bool Editor::DeleteGameObject::Unchanged(void) const
{
	return false;
}


Editor::BatchExecute::BatchExecute(AutoArray<Command*>&& _arr)
	: mArrCommands{ _arr }
{}

Editor::BatchExecute::~BatchExecute(void)
{
	for (auto& elem : mArrCommands)
	{
		Dystopia::DefaultAllocator<Editor::Command>::DestructFree(elem);
	}
}

bool Editor::BatchExecute::Do(void)
{
	bool pass = true;

	for (auto& c : mArrCommands)
	{
		if (!c->Do())
			pass = false;
	}
	return pass;
}

bool Editor::BatchExecute::Undo(void)
{
	bool pass = true;

	for (auto& c : mArrCommands)
	{
		if (!c->Undo())
			pass = false;
	}
	return pass;
}

bool Editor::BatchExecute::Unchanged(void) const
{
	return false;
}

#endif







