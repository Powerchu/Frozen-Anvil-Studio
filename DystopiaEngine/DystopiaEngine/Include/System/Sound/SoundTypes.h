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
	enum eSoundType : unsigned
	{
		eSOUND_BGM = 0,
		eSOUND_FX,

		/* speshal case */
		eSOUND_MASTER,
		eSOUND_LAST
	};

	class Sound
	{
	public:
		Sound(FMOD::Sound* _p, std::string _name) : mpSound{ _p }, mName{ _name } {}
		FMOD::Sound *mpSound;
		std::string mName;
	};

	class Channel
	{
	public:
		Channel(void) : mpChannel{ nullptr } {}
		Channel(FMOD::Channel* _p) : mpChannel{ _p } {}
		Channel& operator=(const Channel& _c) { mpChannel = _c.mpChannel; return *this; }
		Channel& operator=(FMOD::Channel* _c) { mpChannel = _c; return *this; }
		operator FMOD::Channel*() { return mpChannel; }
		FMOD::Channel *mpChannel;
	};
}

#endif	// _SOUND_TYPES_H_ GUARD

