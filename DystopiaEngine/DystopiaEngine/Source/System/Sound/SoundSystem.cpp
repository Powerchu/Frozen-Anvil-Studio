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
#include "Component/AudioSource.h"
#include "fmod.hpp"
#include <Windows.h>

Dystopia::Sound::Sound(FMOD::Sound * _p1, FMOD::Channel * _p2)
	: mpSound{ _p1 }, mpChannel{ _p2 }
{}

Dystopia::Sound::~Sound(void)
{
	if (mpSound) 
		mpSound->release();
}

Dystopia::SoundSystem::SoundSystem(void)
	: mpFMOD{ nullptr }, 
	mDefaultSoundFolder{ "Resource/Audio/" },
	mMapOfAudios{}
{
}

Dystopia::SoundSystem::~SoundSystem(void)
{
}

void Dystopia::SoundSystem::PreInit(void)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	FMOD::System_Create(&mpFMOD);
}

bool Dystopia::SoundSystem::Init(void)
{
	if (!mpFMOD || mpFMOD->init(512, FMOD_INIT_3D_RIGHTHANDED, 0) != FMOD_OK)
		return false;

	return true;
}

void Dystopia::SoundSystem::PostInit(void)
{
	Sound *p = LoadSound("Samsara.mp3");
	if (p) mpFMOD->playSound(p->mpSound, nullptr, false, &p->mpChannel);
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
	for (auto & elem : mMapOfAudios)
		delete elem.second;

	mMapOfAudios.clear();

	mpFMOD->release();
	mpFMOD->close();

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

Dystopia::Sound* Dystopia::SoundSystem::LoadSound(const std::string& _file)
{
	size_t pos = _file.find_last_of('/');
	std::string path = mDefaultSoundFolder + _file;
	std::string soundName = _file;

	if (pos != std::string::npos)
	{
		path = std::string{ _file.begin(), _file.end() };
		soundName = std::string{ _file.begin() + pos + 1, _file.end() };
	}

	if (mMapOfAudios.find(soundName) != mMapOfAudios.end())
		return mMapOfAudios[soundName];

	FMOD::Sound *pSound;
	FMOD_RESULT result = mpFMOD->createSound(path.c_str(), FMOD_CREATECOMPRESSEDSAMPLE, nullptr, &pSound);
	if (result == FMOD_OK)
		return mMapOfAudios[soundName] = new Sound{ pSound, nullptr };

	return nullptr;
}
