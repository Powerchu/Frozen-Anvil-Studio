/* HEADER *********************************************************************************/
/*!
\file	Audio.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <string>

namespace FMOD
{
	class Sound;
	class Channel;
}

namespace Dystopia
{
	class Audio
	{
	public:
		Audio(const std::string&, FMOD::Sound*);
		~Audio(void);
		
		bool IsPlaying(void) const;

		void Play(void);
		void Pause(void);
		void Stop(void);

		void Release(void);

		FMOD::Sound*& GetSound(void);
		FMOD::Channel*& GetChannel(void);

	private:
		bool			mPaused;
		std::string     mName;
		FMOD::Sound		*mpSound;
		FMOD::Channel	*mpChannel;
	};
}


#endif //_AUDIO_H_
