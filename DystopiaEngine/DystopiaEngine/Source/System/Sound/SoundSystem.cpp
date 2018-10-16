/* HEADER *********************************************************************************/
/*!
\file	SoundSystem.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Sound/SoundSystem.h"
#include "fmod.hpp"
#include <Windows.h>

Dystopia::SoundSystem::SoundSystem(void)
{
}

Dystopia::SoundSystem::~SoundSystem(void)
{
}

void Dystopia::SoundSystem::PreInit(void)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
}

bool Dystopia::SoundSystem::Init(void)
{
	FMOD_RESULT result;
	FMOD::System *pSystem = nullptr;

	result = FMOD::System_Create(&pSystem);
	if (result != FMOD_OK)
		exit(-1);

	result = pSystem->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (result != FMOD_OK)
		exit(-1);

	FMOD::Sound *sound;
	result = pSystem->createSound("Resource/Audio/Samsara.mp3", FMOD_CREATECOMPRESSEDSAMPLE, nullptr, &sound);
	if (result == FMOD_OK)
	{
		FMOD::Channel *channel;
		pSystem->playSound(sound, nullptr, false, &channel);
		while (true)
		{

		}
	}
	return true;
}

void Dystopia::SoundSystem::PostInit(void)
{
}

void Dystopia::SoundSystem::FixedUpdate(float)
{
}

void Dystopia::SoundSystem::Update(float)
{
}

void Dystopia::SoundSystem::PostUpdate(void)
{
}

void Dystopia::SoundSystem::Shutdown(void)
{
	CoUninitialize();
}

void Dystopia::SoundSystem::LoadDefaults(void)
{
}

void Dystopia::SoundSystem::LoadSettings(TextSerialiser&)
{
}

void Dystopia::SoundSystem::SaveSettings(TextSerialiser&)
{
}

void Dystopia::SoundSystem::ReceiveMessage(const eSysMessage&)
{
}

