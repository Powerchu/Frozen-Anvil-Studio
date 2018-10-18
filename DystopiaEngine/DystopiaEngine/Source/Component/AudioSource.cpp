/* HEADER *********************************************************************************/
/*!
\file	AudioSource.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Sound/SoundSystem.h"
#include "System/Sound/Audio.h"
#include "System/Driver/Driver.h"
#include "Component/AudioSource.h"
#include "fmod.hpp"

#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#endif 

Dystopia::AudioSource::AudioSource(void) 
	: mpAudio{ nullptr }, mReady{ true }, mLoop{ false }, mIsPlaying{ false }
{
}

Dystopia::AudioSource::AudioSource(const AudioSource& _rhs)
	: mpAudio{ nullptr }, mReady{ true }, mLoop{ false }, mIsPlaying{ false }
{
}

Dystopia::AudioSource::~AudioSource(void)
{
}

void Dystopia::AudioSource::Awake(void)
{
}

void Dystopia::AudioSource::Load(void)
{
}

void Dystopia::AudioSource::Init(void)
{
	auto *p = EngineCore::GetInstance()->GetSystem<SoundSystem>()->LoadSound("Samsara.mp3");
	SetAudio(p);
}

void Dystopia::AudioSource::Update(float)
{
	if (!mpAudio || !mpAudio->mpChannel) 
		return;

	mpAudio->mpChannel->isPlaying(&mIsPlaying);

	if (mIsPlaying) 
		SetReady(false);
}

void Dystopia::AudioSource::GameObjectDestroy(void)
{
}

void Dystopia::AudioSource::Unload(void)
{
}

void Dystopia::AudioSource::DestroyComponent(void)
{
}

void Dystopia::AudioSource::SetOwner(GameObject*)
{
}

Dystopia::AudioSource* Dystopia::AudioSource::Duplicate() const
{
	return nullptr;
}

void Dystopia::AudioSource::Serialise(TextSerialiser&) const
{
}

void Dystopia::AudioSource::Unserialise(TextSerialiser&)
{
}

void Dystopia::AudioSource::EditorUI(void) noexcept
{
#if EDITOR
	if (!mpAudio) return;

	bool b = mReady;
	if (EGUI::Display::CheckBox("Play On Start", &mReady))
	{
	}



#endif 
}

void Dystopia::AudioSource::SetReady(bool _b)
{
	mReady = _b;
}

void Dystopia::AudioSource::SetAudio(Dystopia::Audio* _p)
{
	mpAudio = _p;
}

Dystopia::Audio* Dystopia::AudioSource::GetAudio(void)
{
	return mpAudio;
}

bool Dystopia::AudioSource::IsReady(void) const
{
	return mReady;
}

bool Dystopia::AudioSource::IsLoop(void) const
{
	return mLoop;
}

bool Dystopia::AudioSource::IsPlaying(void) const
{
	return mIsPlaying;
}
