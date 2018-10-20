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
#include "Object/ObjectFlags.h"
#include "fmod.hpp"

#include <Windows.h>

Dystopia::SoundSystem::SoundSystem(void)
	: mpFMOD{ nullptr }, 
	mDefaultSoundFolder{ "Resource/Audio/" },
	mMapOfSounds{}
{
}

Dystopia::SoundSystem::~SoundSystem(void)
{
}

void Dystopia::SoundSystem::PreInit(void)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	if (!SUCCEEDED(hr)) 
		exit(-1);

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
}

void Dystopia::SoundSystem::FixedUpdate(float)
{
}

void Dystopia::SoundSystem::Update(float _dt)
{
	mpFMOD->update();
	for (auto& c : mComponents)
	{
#if EDITOR
		if (c.GetFlags() & FLAG_EDITOR_OBJ)
			continue;
#endif
		if (c.GetFlags() & FLAG_ACTIVE)
		{
			c.Update(_dt);
		}
	}
}

void Dystopia::SoundSystem::PostUpdate(void)
{
	for (auto& c : mComponents)
	{
		if (c.GetFlags() & FLAG_REMOVE)
		{
			mComponents.Remove(&c);
		}
	}
}

void Dystopia::SoundSystem::Shutdown(void)
{
	for (auto& elem : mMapOfSounds)
	{
		elem.second->mpSound->release();
		delete elem.second;
	}

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

void Dystopia::SoundSystem::PlayAudio(Dystopia::AudioSource*)
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

	if (mMapOfSounds.find(soundName) != mMapOfSounds.end())
		return mMapOfSounds[soundName];

	FMOD::Sound *pSound;
	FMOD_RESULT result = mpFMOD->createSound(path.c_str(), FMOD_CREATESAMPLE | FMOD_LOWMEM, nullptr, &pSound);
	if (result == FMOD_OK)
		return mMapOfSounds[soundName] = new Sound{ pSound };

	return nullptr;
}
