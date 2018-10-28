/* HEADER *********************************************************************************/
/*!
\file	CommandList.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/CommandList.h"
#include "Editor/Editor.h"
#include "Editor/Clipboard.h"

#include "Behaviour/Behaviour.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "Object/GameObject.h"

/* Insert Game Object Command  ****************************************************************************/

Dystopia::GameObject* Dystopia::Commands::RetrieveGameObject()
{
	return nullptr;
}

Dystopia::ComdInsertObject::ComdInsertObject(GameObject* _pObj, Scene * _pScene, bool * _notify)
	: mObjID{ _pObj->GetID() }, mpObj{ _pObj }, mpScene{ _pScene }, mFocusBack{ false },
	mpNotify{ _notify }
{}

Dystopia::ComdInsertObject::~ComdInsertObject()
{
	if (mpObj)
	{
		mpObj->Destroy();
		delete mpObj;
	}
}

bool Dystopia::ComdInsertObject::ExecuteDo()
{
	if (auto ss = EngineCore::GetInstance()->GetSystem<SceneSystem>())
		if (&ss->GetCurrentScene() != mpScene)
			return false;

	GameObject* p = mpScene->FindGameObject(mObjID);
	if (p || !mpObj) return false;

	auto& obj = *mpScene->GetAllGameObjects().Emplace(Ut::Move(*mpObj));
	obj.Identify();
	obj.Init();
	obj.RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : obj.GetAllComponents())
		c->RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : obj.GetAllBehaviours())
		c->RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);

	if (mFocusBack)
	{
		Editor *e = Editor::GetInstance();
		GameObject *temp = mpScene->FindGameObject(mObjID);
		if (temp) e->NewSelection(mObjID);//e->SetFocus(*temp);
		mFocusBack = false;
	}

	if (mpNotify) *mpNotify = true;
	delete mpObj;
	mpObj = nullptr;
	return true;
}

bool Dystopia::ComdInsertObject::ExecuteUndo()
{
	if (auto ss = EngineCore::GetInstance()->GetSystem<SceneSystem>())
		if (&ss->GetCurrentScene() != mpScene)
			return false;

	GameObject* p = mpScene->FindGameObject(mObjID);
	if (!p) return false;

	Editor *e = Editor::GetInstance();
	auto all = e->GetSelectionObjects();
	for (auto & elem : all)
	{
		if (elem == p)
		{
			e->RemoveSelection(mObjID);
			mFocusBack = true;
			break;
		}
	}

	if (mpNotify) *mpNotify = true;
	mpObj = p->Duplicate();
	EngineCore::GetInstance()->GetSystem<BehaviourSystem>()->ReplaceID(mpObj->GetID(), mObjID, mpObj);
	mpObj->SetID(mObjID);
	mpObj->Identify();
	mpObj->SetFlag(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : mpObj->GetAllComponents())
		c->SetFlags(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : mpObj->GetAllBehaviours())
		c->SetFlags(eObjFlag::FLAG_EDITOR_OBJ);
	p->Destroy();
	return true;
}

Dystopia::GameObject* Dystopia::ComdInsertObject::RetrieveGameObject()
{
	return mpObj;
}

bool Dystopia::ComdInsertObject::Unchanged() const 
{
	return false; 
}

/* Delete Game Object Command  ****************************************************************************/

Dystopia::ComdDeleteObject::ComdDeleteObject(GameObject* _pObj, Scene * _pScene, bool * _notify)
	: mObjID{ _pObj->GetID() }, mpObj{ _pObj }, mpScene{ _pScene }, mFocusBack{ false },
	mpNotify{ _notify }
{}

Dystopia::ComdDeleteObject::~ComdDeleteObject()
{
	if (mpObj)
	{
		mpObj->Destroy();
		delete mpObj;
	}
}

bool Dystopia::ComdDeleteObject::ExecuteDo()
{
	if (auto ss = EngineCore::GetInstance()->GetSystem<SceneSystem>())
		if (&ss->GetCurrentScene() != mpScene)
			return false;

	GameObject* p = mpScene->FindGameObject(mObjID);
	if (!p) return false;

	Editor* e = Editor::GetInstance();
	auto all = e->GetSelectionObjects();
	for (auto & elem : all)
	{
		if (elem == p)
		{
			e->RemoveSelection(mObjID);
			mFocusBack = true;
			break;
		}
	}

	if (mpNotify) *mpNotify = true;
	mpObj = p->Duplicate();
	EngineCore::GetInstance()->GetSystem<BehaviourSystem>()->ReplaceID(mpObj->GetID(), mObjID, mpObj);
	mpObj->SetID(mObjID);
	mpObj->Identify();
	mpObj->SetFlag(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : mpObj->GetAllComponents())
		c->SetFlags(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : mpObj->GetAllBehaviours())
		c->SetFlags(eObjFlag::FLAG_EDITOR_OBJ);
	p->Destroy();
	return true;
}

bool Dystopia::ComdDeleteObject::ExecuteUndo()
{
	if (auto ss = EngineCore::GetInstance()->GetSystem<SceneSystem>())
		if (&ss->GetCurrentScene() != mpScene)
			return false;

	GameObject* p = mpScene->FindGameObject(mObjID);
	if (p || !mpObj) return false;

	auto& obj = *mpScene->GetAllGameObjects().Emplace(Ut::Move(*mpObj));
	obj.Identify();
	obj.Init();
	obj.RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : obj.GetAllComponents())
		c->RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : obj.GetAllBehaviours())
		c->RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);

	if (mFocusBack)
	{
		Editor* e = Editor::GetInstance();
		GameObject *temp = mpScene->FindGameObject(mObjID);
		if (temp) e->NewSelection(mObjID);// e->SetFocus(*temp);
		mFocusBack = false;
	}

	if (mpNotify) *mpNotify = true;
	delete mpObj;
	mpObj = nullptr;
	return true;
}

Dystopia::GameObject* Dystopia::ComdDeleteObject::RetrieveGameObject()
{
	return mpObj;
}

bool Dystopia::ComdDeleteObject::Unchanged() const
{
	return false;
}

Dystopia::ComdBatch::ComdBatch(AutoArray<Commands*>&& _arrComds)
	: mArrCommands{ _arrComds }
{
}

Dystopia::ComdBatch::~ComdBatch()
{
	for (auto e : mArrCommands)
		delete e;
	mArrCommands.clear();
}

bool Dystopia::ComdBatch::ExecuteDo()
{
	int passes = 0;
	for (const auto& c : mArrCommands)
		passes = (c->ExecuteDo()) ? passes + 1 : passes;
	return (passes > 0);
}

bool Dystopia::ComdBatch::ExecuteUndo()
{
	int passes = 0;
	for (const auto& c : mArrCommands)
		passes = (c->ExecuteUndo()) ? passes + 1 : passes;
	return (passes > 0);
}

bool Dystopia::ComdBatch::Unchanged() const
{
	return false;
}

#endif //EDITOR
