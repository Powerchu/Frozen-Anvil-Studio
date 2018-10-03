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

Dystopia::SceneSystem::SceneSystem(void) :
	mpCurrScene{ nullptr }, mpNextScene{ nullptr }
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
	else
	{
	}
}

void Dystopia::SceneSystem::Update(float _dt)
{
	if (mpNextScene == mpCurrScene)
	{
		mpCurrScene->Update(_dt);
	}
	else
	{
	}
}

void Dystopia::SceneSystem::PostUpdate(void)
{
	if (mpNextScene == mpCurrScene)
	{
		mpCurrScene->PostUpdate();
	}
	else
	{
	}
}

void Dystopia::SceneSystem::Shutdown(void)
{
	if (mpNextScene != mpCurrScene)
		delete mpNextScene;
	delete mpCurrScene;

	mpNextScene = mpCurrScene = nullptr;
}

void Dystopia::SceneSystem::LoadDefaults(void)
{

}

void Dystopia::SceneSystem::LoadSettings(TextSerialiser&)
{

}

void Dystopia::SceneSystem::ChangeScene()
{
	mpCurrScene->Shutdown();
	delete mpCurrScene;
	mpCurrScene = mpNextScene;
	//mpCurrScene->Init();
}


void Dystopia::SceneSystem::RestartScene()
{
//	mpCurrScene->Shutdown();
	/* unserialize current scene */
	//mpCurrScene->Init();
}

void Dystopia::SceneSystem::LoadScene(const std::string& _strFile)
{
	static constexpr size_t size = Utility::SizeofList<UsableComponents>::value;

	delete mpCurrScene;
	mpCurrScene = mpNextScene = new Scene{};

	/*Open File*/
	auto SerialObj = TextSerialiser::OpenFile(_strFile, TextSerialiser::MODE_READ);
	/*Consume Start Block*/
	SerialObj.ConsumeStartBlock();
	/*Get Next Scene to Unserialise*/
	mpNextScene->Unserialise(SerialObj);
	/*Get all System who are ComponentDonor to unserialise*/
	SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemUnserialise(SerialObj);
	/*Consume End Block*/
	SerialObj.ConsumeEndBlock();

	//ChangeScene();
}

void Dystopia::SceneSystem::SaveScene(const std::string & _strFile, const std::string& _strSceneName)
{
	static constexpr size_t size = Utility::SizeofList<UsableComponents>::value;

	/*Open File*/
	auto SerialObj = TextSerialiser::OpenFile(_strFile, TextSerialiser::MODE_WRITE);
	/*Consume Start Block*/
	SerialObj.InsertStartBlock("Scene");
	/*Get Current Scene to Serialize*/
	mpCurrScene->SetSceneName(_strSceneName);
	mpCurrScene->Serialise(SerialObj);
	/*Get all System who are ComponentDonor to unserialise*/
	SceneSystemHelper::SystemFunction< std::make_index_sequence< size >>::SystemSerialise(SerialObj);
	/*Consume End Block*/
	SerialObj.InsertEndBlock("Scene");
}


