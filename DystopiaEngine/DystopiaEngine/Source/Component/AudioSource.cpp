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
#include "Component/AudioSource.h"

#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#endif 

Dystopia::AudioSource::AudioSource(void) 
{
}


Dystopia::AudioSource::AudioSource(const AudioSource&)
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
}
