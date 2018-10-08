/* HEADER *********************************************************************************/
/*!
\file	CommandList.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/CommandList.h"
#include "Behaviour/Behaviour.h"
#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "Object/GameObject.h"

/* Insert Game Object Command  ****************************************************************************/

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
	{
		if (&ss->GetCurrentScene() != mpScene)
		{
			return false;
		}
	}
	GameObject* p = mpScene->FindGameObject(mObjID);
	if (p || !mpObj) return false;

	mpScene->GetAllGameObjects().EmplaceBack(Ut::Move(*mpObj));
	auto& obj = mpScene->GetAllGameObjects().back();
	obj.Init();
	obj.RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : mpObj->GetAllComponents())
		c->RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	if (mFocusBack)
	{
		Editor *e = Editor::GetInstance();
		GameObject *temp = mpScene->FindGameObject(mObjID);
		if (temp) e->SetFocus(*temp);
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
	{
		if (&ss->GetCurrentScene() != mpScene)
		{
			return false;
		}
	}
	GameObject* p = mpScene->FindGameObject(mObjID);
	if (!p) return false;

	Editor *e = Editor::GetInstance();
	if (e->GetCurrentFocusGameObj() == p)
	{
		e->RemoveFocus();
		mFocusBack = true;
	}

	if (mpNotify) *mpNotify = true;
	mpObj = p->Duplicate();
	mpObj->SetID(mObjID);
	mpObj->Init();
	mpObj->SetFlag(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : mpObj->GetAllComponents())
		c->SetFlags(eObjFlag::FLAG_EDITOR_OBJ);
	p->Destroy();
	return true;
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
	{
		if (&ss->GetCurrentScene() != mpScene)
		{
			return false;
		}
	}
	GameObject* p = mpScene->FindGameObject(mObjID);
	if (!p) return false;

	Editor* e = Editor::GetInstance();
	if (e->GetCurrentFocusGameObj() == p)
	{
		e->RemoveFocus();
		mFocusBack = true;
	}

	if (mpNotify) *mpNotify = true;
	mpObj = p->Duplicate();
	mpObj->SetID(mObjID);
	mpObj->Init();
	mpObj->SetFlag(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : mpObj->GetAllComponents())
		c->SetFlags(eObjFlag::FLAG_EDITOR_OBJ);
	p->Destroy();
	return true;
}

bool Dystopia::ComdDeleteObject::ExecuteUndo()
{
	if (auto ss = EngineCore::GetInstance()->GetSystem<SceneSystem>())
	{
		if (&ss->GetCurrentScene() != mpScene)
		{
			return false;
		}
	}
	GameObject* p = mpScene->FindGameObject(mObjID);
	if (p || !mpObj) return false;

	mpScene->GetAllGameObjects().EmplaceBack(Ut::Move(*mpObj));
	auto& obj = mpScene->GetAllGameObjects().back();
	obj.Init();
	obj.RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	for (auto& c : mpObj->GetAllComponents())
		c->RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
	if (mFocusBack)
	{
		Editor* e = Editor::GetInstance();
		GameObject *temp = mpScene->FindGameObject(mObjID);
		if (temp) e->SetFocus(*temp);
		mFocusBack = false;
	}
	if (mpNotify) *mpNotify = true;
	delete mpObj;
	mpObj = nullptr;
	return true;
}

bool Dystopia::ComdDeleteObject::Unchanged() const
{
	return false;
}


#endif //EDITOR
