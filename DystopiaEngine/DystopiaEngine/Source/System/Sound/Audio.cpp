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

Dystopia::Audio::Audio(FMOD::Sound* _s)
	: mpSound{ _s }
{}

Dystopia::Audio::~Audio(void)
{
}

FMOD::Sound*& Dystopia::Audio::GetSound(void)
{
	return mpSound;
}
