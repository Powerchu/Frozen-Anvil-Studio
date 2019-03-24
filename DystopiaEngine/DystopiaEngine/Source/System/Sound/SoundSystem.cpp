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
#include "System/File/FileSystem.h"
#include "System/Driver/Driver.h"

#include "Object/GameObject.h"

#include "Component/AudioSource.h"
#include "Component/AudioListener.h"
#include "Component/Transform.h"
#include "Object/ObjectFlags.h"
#include "fmod.hpp"
#include "Math/MathUtility.h"

#include <Windows.h>

#define RESERVE_CHANNELS 100

static const HashString g_GroupNames[Dystopia::eSOUND_LAST] =
{
	"BGM",
	"FX",
	"Master"
};

Dystopia::SoundSystem::SoundSystem(void)
	: mpFMOD{ nullptr }, 
	mDefaultSoundFolder{ "Resource/Audio/" },
	mMapOfSounds{}, mArrGroups{ nullptr }, mMasterVol{ 1 },
	mBGMVol{ 1 }, mFXVol{ 1 }, mbUpdateVol{ true },
	mArrActiveChannels{}
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
	mArrActiveChannels.reserve(RESERVE_CHANNELS);
}

bool Dystopia::SoundSystem::Init(void)
{
	if (!mpFMOD || mpFMOD->init(512, FMOD_INIT_NORMAL, 0) != FMOD_OK)
		return false;

	auto && Sounds = EngineCore::Get<FileSystem>()->GetAllFiles((EngineCore::Get<FileSystem>()->GetProjectFolders<std::string>(eFileDir::eResource) + "/Audio/").c_str());
	for (auto && elem : Sounds)
	{
		LoadSound(elem.c_str());
	}

	return true;
}

void Dystopia::SoundSystem::PostInit(void)
{
	for (unsigned int i = 0; i < eSOUND_LAST; ++i)
		mpFMOD->createChannelGroup(g_GroupNames[i].c_str(), &mArrGroups[i]);

	for (unsigned int i = 0; i < eSOUND_MASTER; ++i)
		mArrGroups[eSOUND_MASTER]->addGroup(mArrGroups[i]);
}

void Dystopia::SoundSystem::FixedUpdate(float)
{
}

void Dystopia::SoundSystem::Update(float _dt)
{
	mpFMOD->update();


	if (mbUpdateVol)
	{
		mArrGroups[eSOUND_MASTER]->setVolume(mMasterVol);
		mArrGroups[eSOUND_BGM]->setVolume(mBGMVol);
		mArrGroups[eSOUND_FX]->setVolume(mFXVol);
		mbUpdateVol = false;
	}
	for (auto& c : ComponentDonor<AudioListener>::mComponents)
	{
#if EDITOR
		if (c.GetFlags() & FLAG_EDITOR_OBJ)
			continue;
#endif
		if (c.GetFlags() & FLAG_ACTIVE)
		{
	
			if (GameObject* pOwner = c.GetOwner())
			{
				if (Transform* pTrans = pOwner->GetComponent<Transform>())
				{
					c.UpdateFMODPos(pTrans->GetGlobalPosition());
					mpFMOD->set3DListenerAttributes(mMapOfListeners[&c], c.GetFMODPos(), NULL, NULL, NULL);
				}
			}
		}
	}
	for (auto& c : ComponentDonor<AudioSource>::mComponents)
	{
#if EDITOR
		if (c.GetFlags() & FLAG_EDITOR_OBJ)
			continue;
#endif
		if (c.GetFlags() & FLAG_ACTIVE)
		{
			if (GameObject* pOwner = c.GetOwner())
			{
				if (Transform* pTrans = pOwner->GetComponent<Transform>())
				{
					auto && pos = pTrans->GetGlobalPosition();
					c.UpdateFMODPos(pos);
				}
			}
			c.Update(_dt);
			if (c.IsReady() && c.GetSound())
				 PlayAudio(c);
		}
	}
}

void Dystopia::SoundSystem::PostUpdate(void)
{
	for (auto& c : ComponentDonor<AudioSource>::mComponents)
	{
		if (c.GetFlags() & FLAG_REMOVE)
		{
			if (c.GetChannel().mpChannel)
			{
				//c.GetChannel().mpChannel->stop();
				for (int i = 0; i < mArrActiveChannels.size(); ++i)
				{
					if (mArrActiveChannels[i] == c.GetChannel().mpChannel)
					{
						mArrActiveChannels[i]->stop();
						mArrActiveChannels.FastRemove(i);
						break;
					}
				}
			}
			ComponentDonor<AudioSource>::mComponents.Remove(&c);
		}
	}
	for (auto& c : ComponentDonor<AudioListener>::mComponents)
	{
		if (c.GetFlags() & FLAG_REMOVE)
		{
			ComponentDonor<AudioListener>::mComponents.Remove(&c);
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
	mMapOfSounds.clear();
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

void Dystopia::SoundSystem::PlayAudio(Dystopia::AudioSource& _a)
{
	if (_a.GetChannel() && _a.GetChannel().mpChannel)
	{
		for (int i = 0; i < mArrActiveChannels.size(); ++i)
		{
			if (mArrActiveChannels[i] == _a.GetChannel().mpChannel)
			{
				mArrActiveChannels[i]->stop();
				mArrActiveChannels.FastRemove(i);
				break;
			}
		}
	}

	FMOD::Channel *channel;
	mpFMOD->playSound(_a.GetSound()->mpSound, mArrGroups[_a.GetSoundType()], false, &channel);
	_a.SetChannel(Channel{ channel });
	_a.SetReady(false);

	mArrActiveChannels.push_back(channel);
}

void Dystopia::SoundSystem::SaveSettings(TextSerialiser&)
{
}

void Dystopia::SoundSystem::ReceiveMessage(const eSysMessage&)
{
}

Dystopia::Sound* Dystopia::SoundSystem::LoadSound(const HashString& _file)
{
	size_t pos = _file.find_last_of("/");
	auto path = EngineCore::Get<FileSystem>()->GetFullPath(_file.c_str(), eFileDir::eResource);
	std::string soundName = _file.c_str();

	if (pos != HashString::nPos)
	{
		path = std::string{ _file.c_str() };
		soundName = std::string{ _file.c_str() + pos + 1 };
	}

	if (mMapOfSounds.find(soundName) != mMapOfSounds.end())
		return mMapOfSounds[soundName];

	FMOD::Sound *pSound;
	FMOD_RESULT result = mpFMOD->createSound(path.c_str(), FMOD_CREATESAMPLE | FMOD_LOWMEM | FMOD_3D | FMOD_3D_LINEARROLLOFF |  FMOD_3D_INVERSEROLLOFF, nullptr, &pSound);
	if (result == FMOD_OK)
		return mMapOfSounds[soundName] = new Sound{ pSound, soundName };

	return nullptr;
}

void Dystopia::SoundSystem::CreateSound(const char * _data, int _buffersize, int _noChannels, int _format, int _freq)
{
	/*Default freq : 4100*/
	FMOD_SOUND_FORMAT format[8]
	{
		FMOD_SOUND_FORMAT_NONE,
		FMOD_SOUND_FORMAT_PCM8,
		FMOD_SOUND_FORMAT_PCM16,
		FMOD_SOUND_FORMAT_PCM24,
		FMOD_SOUND_FORMAT_PCM32,
		FMOD_SOUND_FORMAT_PCMFLOAT,
		FMOD_SOUND_FORMAT_BITSTREAM,
		FMOD_SOUND_FORMAT_MAX
	};

	FMOD::Sound * pSound = nullptr;
	DEBUG_PRINT(eLog::MESSAGE, "No of Channels %d", _noChannels);
	for (auto elem : format)
	{
		FMOD_CREATESOUNDEXINFO info;
		SecureZeroMemory(&info, sizeof(FMOD_CREATESOUNDEXINFO));
		info.defaultfrequency   = _freq;
		info.numchannels        = _noChannels;
		info.format             = elem;//static_cast<FMOD_SOUND_FORMAT>(_format);
		info.filebuffersize     = _buffersize;
		info.suggestedsoundtype = FMOD_SOUND_TYPE_VORBIS;
		auto result = mpFMOD->createSound(_data , FMOD_CREATESAMPLE | FMOD_LOWMEM | FMOD_3D | FMOD_3D_LINEARROLLOFF | FMOD_3D_INVERSEROLLOFF | FMOD_OPENRAW, &info, &pSound);
		if (result == FMOD_OK)
		{
			DEBUG_PRINT(eLog::MESSAGE, "Sound is created");
			__debugbreak();
		}
		else
		{

		}
	}
	//FMOD_CREATESOUNDEXINFO info;
	//info.defaultfrequency = _freq;
	//info.numchannels      = _noChannels;
	//info.format = FMOD_SOUND_FORMAT_PCM24;//static_cast<FMOD_SOUND_FORMAT>(_format);
	//auto result = mpFMOD->createSound(_data, FMOD_OPENRAW, &info, &pSound);
	//if (result == FMOD_OK)
	//{
	//	DEBUG_PRINT(eLog::MESSAGE, "Sound is created");
	//}
	//else
	//{
	//	__debugbreak();
	//}

}

char Dystopia::SoundSystem::RegisterNewListener(AudioListener * const & _Listener)
{
	//auto && _fn = [_Listener](AudioListener* _l)->bool {return _l == _Listener; };
	//
	//auto result = std::find_if(mMapOfListeners.begin(), mMapOfListeners.end(), _fn);
	//if (result  != mMapOfListeners.end())
	//{
	//	return result->first;
	//}
	//else
	//{
	//	mMapOfListeners[mMapOfListeners.size() + 1] = _Listener;
	//	return mMapOfListeners.size();
	//}
	auto result = mMapOfListeners.find(_Listener);
	if (result != mMapOfListeners.end())
	{
		return result->second;
	}
	else
	{
		char size = static_cast<char>(mMapOfListeners.size());
		mpFMOD->set3DNumListeners(size);
		mMapOfListeners[_Listener] =  size;
		return size;
	}
}

void Dystopia::SoundSystem::SetMaster(float _f)
{
	if (Math::ApproxEq(_f, mMasterVol))
		return;

	mMasterVol = _f;
	mbUpdateVol = true;
}

void Dystopia::SoundSystem::SetBGM(float _f)
{
	if (Math::ApproxEq(_f, mBGMVol))
		return;

	mBGMVol = _f;
	mbUpdateVol = true;
}

void Dystopia::SoundSystem::SetFX(float _f)
{
	if (Math::ApproxEq(_f, mFXVol))
		return;

	mFXVol = _f;
	mbUpdateVol = true;
}

_DLL_EXPORT float Dystopia::SoundSystem::GetMaster() const
{
	return mMasterVol;
}

_DLL_EXPORT float Dystopia::SoundSystem::GetBGM() const
{
	return mBGMVol;
}

_DLL_EXPORT float Dystopia::SoundSystem::GetFX() const
{
	return mFXVol;
}

_DLL_EXPORT void Dystopia::SoundSystem::StopAll(void)
{
	for (auto& c : ComponentDonor<AudioSource>::mComponents)
		if (c.GetChannel().mpChannel)
			c.GetChannel().mpChannel->stop();
}





