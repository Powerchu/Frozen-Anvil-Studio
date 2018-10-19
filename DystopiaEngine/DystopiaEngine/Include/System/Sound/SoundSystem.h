/* HEADER *********************************************************************************/
/*!
\file	SoundSystem.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SOUND_SYSTEM_H_
#define _SOUND_SYSTEM_H_

#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"

#include "DataStructure/AutoArray.h"
#include "DataStructure/SharedPtr.h"

#include <string>
#include <map>

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
}

namespace Dystopia
{
	class Sound
	{
	public:
		Sound(FMOD::Sound* _p) : mpSound{ _p }{}
		FMOD::Sound *mpSound;
	};

	class AudioSource;
	class SoundSystem : public Systems, public ComponentDonor<AudioSource>
	{
	public:
		SoundSystem(void);
		~SoundSystem(void);

		void PreInit(void);
		bool Init(void);
		void PostInit(void);

		void FixedUpdate(float);
		void Update(float);
		void PostUpdate(void);
		void Shutdown(void);

		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);
		void SaveSettings(TextSerialiser&);

		void ReceiveMessage(const eSysMessage&);
		Sound* LoadSound(const std::string& _file);

	private:
		FMOD::System *mpFMOD;
		std::string	 mDefaultSoundFolder;

		std::map<std::string, Sound*> mMapOfSounds;
		AutoArray<FMOD::Channel*> mArrOfChannels;

		void PlayAudio(AudioSource*);
	};
}


#endif	// _SOUND_SYSTEM_H_

