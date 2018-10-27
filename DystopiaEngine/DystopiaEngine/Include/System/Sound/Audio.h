/* HEADER *********************************************************************************/
/*!
\file	Audio.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
		Audio(FMOD::Sound*);
		~Audio(void);
		
		FMOD::Sound*& GetSound(void);

	private:
		FMOD::Sound		*mpSound;
	};
}


#endif //_AUDIO_H_