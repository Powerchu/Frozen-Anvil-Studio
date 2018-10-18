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
#include "Component/AudioSource.h"
#include "fmod.hpp"

#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#endif 

Dystopia::AudioSource::AudioSource(void) 
	: mpAudio{ nullptr }, mPlay{ true }, mLoop{ false }, mIsPlaying{ false }
{
}

Dystopia::AudioSource::AudioSource(const AudioSource& _rhs)
	: mpAudio{ nullptr }, mPlay{ true }, mLoop{ false }, mIsPlaying{ false }
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
}

void Dystopia::AudioSource::Update(float)
{
	if (!mpAudio || !mpAudio->mpChannel) 
		return;

	mpAudio->mpChannel->isPlaying(&mIsPlaying);
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





#endif 
}

void Dystopia::AudioSource::SetReady(bool _b)
{
	mPlay = _b;
}

Dystopia::Audio* Dystopia::AudioSource::GetAudio(void)
{
	return mpAudio;
}

bool Dystopia::AudioSource::IsReady(void) const
{
	return mPlay;
}

bool Dystopia::AudioSource::IsLoop(void) const
{
	return mLoop;
}

bool Dystopia::AudioSource::IsPlaying(void) const
{
	return mIsPlaying;
}
