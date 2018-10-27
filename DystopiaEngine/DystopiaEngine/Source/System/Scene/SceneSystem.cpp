/* HEADER *********************************************************************************/
/*!
\file	SceneSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Scene/SceneSystem.h"              // File Header
#include "System/Scene/SceneSysMetaHelper.h"
#include "System/SystemTypes.h"
#include "System/Driver/Driver.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "DataStructure/Array.h"
#include "IO/TextSerialiser.h"
#include "Utility/DebugAssert.h"

#include "Utility/Utility.h"
#include "Editor/CommandList.h"
#include "Editor/Commands.h"
#include "Editor/Editor.h"
#include "Editor/DefaultFactory.h"

#include "Component/Component.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"
#include "System/Behaviour/BehaviourSystem.h"

Dystopia::SceneSystem::SceneSystem(void) :
	mpCurrScene{ nullptr }, mpNextScene{ nullptr }, mLastSavedData{ "" }, mNextSceneFile{ "" }
{
}

Dystopia::SceneSystem::~SceneSystem(void)
{

}

void Dystopia::SceneSystem::PreInit(void)
{
	mpCurrScene = new Scene{};
}

bool Dystopia::SceneSystem::Init(void)
{
	return true;
}

void Dystopia::SceneSystem::PostInit(void)
{
	mpNextScene = mpCurrScene;
}

void Dystopia::SceneSystem::FixedUpdate(float _dt)
{
	mpCurrScene->FixedUpdate(_dt);
}

void Dystopia::SceneSystem::Update(float _dt)
{
	mpCurrScene->Update(_dt);
}

void Dystopia::SceneSystem::PostUpdate(void)
{
	mpCurrScene->PostUpdate();
	if (mpCurrScene != mpNextScene)
		SceneChanged();
}

void Dystopia::SceneSystem::Shutdown(void)
{
	if (mpNextScene != mpCurrScene)
	{
		mpNextScene->Shutdown();
		delete mpNextScene;
	}
	mpCurrScene->Shutdown();
	delete mpCurrScene;

	mpNextScene = mpCurrScene = nullptr;
}

void Dystopia::SceneSystem::LoadDefaults(void)
{

}

void Dystopia::SceneSystem::LoadSettings(TextSerialiser&)
{

}

bool Dystopia::SceneSystem::Instantiate(const std::string& _prefabName, const Math::Pt3D& _position)
{
	if (!mpCurrScene) return false;

	GameObject *pDupl = Factory::LoadFromPrefab(_prefabName);
	if (pDupl)
	{
		pDupl->GetComponent<Transform>()->SetPosition(_position);
		mpCurrScene->GetAllGameObjects().EmplaceBack(Ut::Move(*pDupl));
		auto& obj = mpCurrScene->GetAllGameObjects().back();
		obj.Identify();
		obj.Init();
		obj.RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
		for (auto& c : obj.GetAllComponents())
			c->RemoveFlags(eObjFlag::FLAG_EDITOR_OBJ);
		delete pDupl;
		return true;
	}
	return false;
}

void Dystopia::SceneSystem::SceneChanged(void)
{
	mpCurrScene->Shutdown();
	delete mpCurrScene;

	EngineCore::GetInstance()->GetSystem<CollisionSystem>()->PostUpdate();
	EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->PostUpdate();

	mpCurrScene = mpNextScene;
	static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;
	auto SerialObj = TextSerialiser::OpenFile(mNextSceneFile, TextSerialiser::MODE_READ);
	SerialObj.ConsumeStartBlock();
	mpNextScene->Unserialise(SerialObj);
	SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemUnserialise(SerialObj);
	EngineCore::GetInstance()->Get<BehaviourSystem>()->Unserialise(SerialObj);
	SerialObj.ConsumeEndBlock();
	mNextSceneFile.clear();
	mpCurrScene->Init();
}

void Dystopia::SceneSystem::RestartScene(void)
{
	if (mLastSavedData.length())
	{
		mpCurrScene->PostUpdate();
		mpCurrScene->Shutdown();

		EngineCore::GetInstance()->PostUpdate();

		static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;
		auto SerialObj = TextSerialiser::OpenFile(mLastSavedData, TextSerialiser::MODE_READ);
		SerialObj.ConsumeStartBlock();
		mpCurrScene->Unserialise(SerialObj);
		SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemUnserialise(SerialObj);
		EngineCore::GetInstance()->Get<BehaviourSystem>()->Unserialise(SerialObj);
		SerialObj.ConsumeEndBlock();
		mpCurrScene->Init();
	}
}

void Dystopia::SceneSystem::LoadScene(const std::string& _strFile)
{
	TextSerialiser::OpenFile(_strFile, TextSerialiser::MODE_READ); // just to check if file valid

	mNextSceneFile = _strFile;
	mpNextScene = new Scene{};
}

void Dystopia::SceneSystem::SaveScene(const std::string & _strFile, const std::string & _strSceneName)
{
	static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;

	mpCurrScene->SetSceneName(_strSceneName);
	auto SerialObj = TextSerialiser::OpenFile(_strFile, TextSerialiser::MODE_WRITE);

	SerialObj.InsertStartBlock("Scene");
	mpCurrScene->Serialise(SerialObj);
	SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemSerialise(SerialObj);
	EngineCore::GetInstance()->Get<BehaviourSystem>()->Serialise(SerialObj);
	SerialObj.InsertEndBlock("Scene");

	mLastSavedData = _strFile;
}


