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
#include "System\Scene\SceneSystem.h" // File Header



Dystopia::SceneSystem::SceneSystem(void) :
	mpCurrScene{ nullptr }, mpNextScene{ nullptr }, mpPrevScene{ nullptr }
{
}

Dystopia::SceneSystem::~SceneSystem(void)
{
}

void Dystopia::SceneSystem::PreInit(void)
{
}

bool Dystopia::SceneSystem::Init(void)
{
	return true;
}

void Dystopia::SceneSystem::PostInit(void)
{
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
	else
	{
		
	}
}

void Dystopia::SceneSystem::Shutdown(void)
{
}

void Dystopia::SceneSystem::LoadDefaults(void)
{
}

void Dystopia::SceneSystem::LoadSettings(TextSerialiser&)
{
}

void Dystopia::SceneSystem::LoadScene(const std::string& _strFile)
{

}


