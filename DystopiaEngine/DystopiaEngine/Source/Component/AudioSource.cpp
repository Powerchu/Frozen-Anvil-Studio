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
	: Component{}, 
	mpSound{ nullptr }, mSoundName{ "Samsara.mp3" }
{
}

Dystopia::AudioSource::AudioSource(const AudioSource& _rhs)
	: Component{ _rhs }, 
	mpSound{ nullptr }, mSoundName{ _rhs.mSoundName }
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
	SetSound(EngineCore::GetInstance()->GetSystem<SoundSystem>()->LoadSound(mSoundName));
}

void Dystopia::AudioSource::Update(float)
{
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


#endif 
}

Dystopia::Sound* Dystopia::AudioSource::GetSound(void)
{
	return mpSound;
}

void Dystopia::AudioSource::SetSound(Sound* _s)
{
	mpSound = _s;
}

unsigned Dystopia::AudioSource::GetChannel(void) const
{
	return mChannelID;
}

void Dystopia::AudioSource::SetChannel(unsigned _i)
{
	mChannelID = _i;
}

