/* HEADER *********************************************************************************/
/*!
\file	Audio.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Sound/Audio.h"
#include "fmod.hpp"

Dystopia::Audio::Audio(const std::string& _name, FMOD::Sound* _s)
	: mpSound{ _s }, mpChannel{ nullptr }, mPaused{ false }, mName{ _name }
{}

Dystopia::Audio::~Audio(void)
{
}

bool Dystopia::Audio::IsPlaying(void) const
{
	bool ret = false;
	if (mpChannel)
		mpChannel->isPlaying(&ret);
	return ret;
}

void Dystopia::Audio::Play(void)
{
}

void Dystopia::Audio::Pause(void)
{
}

void Dystopia::Audio::Stop(void)
{

}

void Dystopia::Audio::Release(void)
{
	mpSound->release();
	mpSound = nullptr;
	mpChannel = nullptr;
}

FMOD::Sound*& Dystopia::Audio::GetSound(void)
{
	return mpSound;
}

FMOD::Channel*& Dystopia::Audio::GetChannel(void) 
{
	return mpChannel;
}




