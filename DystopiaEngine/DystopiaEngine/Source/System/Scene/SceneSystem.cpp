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
#include "DataStructure/Array.h"
#include "IO/TextSerialiser.h"
#include "Utility/DebugAssert.h"
#include "Editor/Editor.h"

Dystopia::SceneSystem::SceneSystem(void) :
	mpCurrScene{ nullptr }, mpNextScene{ nullptr }, mLastSavedData{ "" }
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
	if (mpNextScene == mpCurrScene)
	{
		mpCurrScene->FixedUpdate(_dt);
	}
}

void Dystopia::SceneSystem::Update(float _dt)
{
	if (mpNextScene == mpCurrScene)
	{
		mpCurrScene->Update(_dt);
	}
}

void Dystopia::SceneSystem::PostUpdate(void)
{
	if (mpNextScene == mpCurrScene)
	{
		mpCurrScene->PostUpdate();
	}
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

void Dystopia::SceneSystem::SceneChanged(void)
{
	mpCurrScene->PostUpdate();
	mpCurrScene->Shutdown();
	delete mpCurrScene;
	mpCurrScene = mpNextScene;
	mpCurrScene->Init();
}

void Dystopia::SceneSystem::RestartScene(void)
{
	if (mLastSavedData.length())
	{
		mpCurrScene->PostUpdate();
		mpCurrScene->Shutdown();

		static constexpr size_t size = Utility::SizeofList<UsableComponents>::value;
		auto SerialObj = TextSerialiser::OpenFile(mLastSavedData, TextSerialiser::MODE_READ);
		SerialObj.ConsumeStartBlock();
		mpCurrScene->Unserialise(SerialObj);
		SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemUnserialise(SerialObj);
		SerialObj.ConsumeEndBlock();
		mpCurrScene->Init();
	}
}

void Dystopia::SceneSystem::LoadScene(const std::string& _strFile)
{
	mpNextScene = new Scene{};

	static constexpr size_t size = Utility::SizeofList<UsableComponents>::value;
	auto SerialObj = TextSerialiser::OpenFile(_strFile, TextSerialiser::MODE_READ);

	SerialObj.ConsumeStartBlock();
	mpNextScene->Unserialise(SerialObj);
	SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemUnserialise(SerialObj);
	SerialObj.ConsumeEndBlock();

	mLastSavedData = _strFile;
	SceneChanged();
}

void Dystopia::SceneSystem::SaveScene(const std::string & _strFile, const std::string & _strSceneName)
{
	static constexpr size_t size = Utility::SizeofList<UsableComponents>::value;

	mpCurrScene->SetSceneName(_strSceneName);
	auto SerialObj = TextSerialiser::OpenFile(_strFile, TextSerialiser::MODE_WRITE);

	SerialObj.InsertStartBlock("Scene");
	mpCurrScene->Serialise(SerialObj);
	SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemSerialise(SerialObj);
	SerialObj.InsertEndBlock("Scene");

	mLastSavedData = _strFile;
}


