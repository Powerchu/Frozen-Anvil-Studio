/* HEADER *********************************************************************************/
/*!
\file	SoundTypes.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SOUND_TYPES_H_
#define _SOUND_TYPES_H_

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
	class ChannelGroup;
}

namespace Dystopia
{
	enum eSoundType
	{
		eSOUND_BGM = 0,
		eSOUND_FX,

		eSOUND_MASTER,
		eSOUND_LAST
	};

	class Sound
	{
	public:
		Sound(FMOD::Sound* _p) : mpSound{ _p } {}
		FMOD::Sound *mpSound;
	};

}

#endif	// _SOUND_TYPES_H_ GUARD

