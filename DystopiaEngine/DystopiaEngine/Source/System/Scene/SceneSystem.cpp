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

#include "System/SystemTypes.h"
#include "System/Scene/SceneSysMetaHelper.h"
#include "System/Driver/Driver.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "IO/TextSerialiser.h"

#include "Utility/DebugAssert.h"
#include "Utility/Utility.h"
#include "Utility/GUID.h"

#include "Component/Component.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

#include "Factory/Factory.h"
#include "Editor/RuntimeMeta.h"

Dystopia::SceneSystem::SceneSystem(void) :
	mpCurrScene{ nullptr }, mpNextScene{ nullptr }, mLastSavedData{ "" }, mNextSceneFile{ "" }, mbRestartScene { false }
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
	if (mpCurrScene != mpNextScene || mbRestartScene)
		SceneChanged();
}

void Dystopia::SceneSystem::Shutdown(void)
{
	if (mpNextScene != mpCurrScene && mpNextScene)
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

AutoArray<Dystopia::GameObject*>& Dystopia::SceneSystem::FindGameObjectByTag(const HashString& _tag)
{
	return mpCurrScene->FindGameObjectByTag(_tag);
}

Dystopia::GameObject* Dystopia::SceneSystem::FindGameObject_cstr(const char* const _str)
{
	return FindGameObject(HashString{ _str });
}

Dystopia::GameObject * Dystopia::SceneSystem::Instantiate(const HashString& _prefabName, const Math::Pt3D& _position)
{
	HashString file = _prefabName;
	if (_prefabName.rfind(".dobj") == HashString::nPos)
		file += ".dobj";

	auto obj = EngineCore::GetInstance()->GetSubSystem<Factory>()->SpawnPrefab(file, _position);
	if (obj)
		obj->GetComponent<Transform>()->SetGlobalPosition(_position);
	return obj;
}

void Dystopia::SceneSystem::SceneChanged(void)
{
	if (mbRestartScene)
	{
		//delete mpNextScene;
		mpNextScene = mpCurrScene;
		mbRestartScene = false;
		if (mLastSavedData.length())
		{
			mpCurrScene->PostUpdate();
			mpCurrScene->Shutdown();

			EngineCore::GetInstance()->PostUpdate();

			static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;
			auto SerialObj = TextSerialiser::OpenFile(mLastSavedData.c_str(), TextSerialiser::MODE_READ);
			SerialObj.ConsumeStartBlock();
			mpCurrScene->Unserialise(SerialObj);
			SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemUnserialise(SerialObj);
			EngineCore::GetInstance()->Get<BehaviourSystem>()->Unserialise(SerialObj);
			SerialObj.ConsumeEndBlock();
			for (auto& obj : mpCurrScene->GetAllGameObjects())
				obj.Awake();
#if !EDITOR
			mpCurrScene->Init();
#endif
			Dystopia::SystemList<std::make_index_sequence<Ut::SizeofList<Dystopia::UsableComponents>::value>>::InitDonors();
		}
	}
	else
	{
		mpCurrScene->Shutdown();
		//delete mpCurrScene;

		EngineCore::GetInstance()->GetSystem<CollisionSystem>()->PostUpdate();
		EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->PostUpdate();

		//mpCurrScene = mpNextScene;
		mpNextScene = mpCurrScene;
		static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;
		auto SerialObj = TextSerialiser::OpenFile(mNextSceneFile.c_str(), TextSerialiser::MODE_READ);
		SerialObj.ConsumeStartBlock();
		mpNextScene->Unserialise(SerialObj);
		SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemUnserialise(SerialObj);
		EngineCore::GetInstance()->Get<BehaviourSystem>()->Unserialise(SerialObj);
		SerialObj.ConsumeEndBlock();
		mNextSceneFile.clear();
		for (auto& obj : mpCurrScene->GetAllGameObjects())
			obj.Awake();
		mpCurrScene->Init();
		Dystopia::SystemList<std::make_index_sequence<Ut::SizeofList<Dystopia::UsableComponents>::value>>::InitDonors();
	}
}

void Dystopia::SceneSystem::RestartScene(void)
{
	mbRestartScene = true;
	mpNextScene = nullptr;//new Scene{};
}

void Dystopia::SceneSystem::LoadScene(const char* _strName)
{
	auto fs = EngineCore::GetInstance()->GetSubSystem<FileSystem>();

	HashString scene{ _strName };
	if (scene.find(".dscene") == std::string::npos)
		scene += ".dscene";
	LoadScene(std::string{ fs->GetFullPath(scene.c_str(), eFileDir::eResource) });
}

void Dystopia::SceneSystem::LoadScene(const std::string& _strFile)
{
	TextSerialiser::OpenFile(_strFile.c_str(), TextSerialiser::MODE_READ); // just to check if file valid

	mNextSceneFile = _strFile;
	mLastSavedData = _strFile;
	mpNextScene = nullptr;//new Scene{};
}

void Dystopia::SceneSystem::SaveScene(const std::string & _strFile, const std::string & _strSceneName)
{
	static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;

	mpCurrScene->SetSceneName(_strSceneName);
	auto SerialObj = TextSerialiser::OpenFile(_strFile.c_str(), TextSerialiser::MODE_WRITE);

	SerialObj.InsertStartBlock("Scene");
	mpCurrScene->Serialise(SerialObj);
	SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemSerialise(SerialObj);
	EngineCore::GetInstance()->Get<BehaviourSystem>()->Serialise(SerialObj);
	SerialObj.InsertEndBlock("Scene");

	mLastSavedData = _strFile;
}


