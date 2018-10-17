/* HEADER *********************************************************************************/
/*!
\file	SoundSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Wrapper class for FMOD

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SOUND_SYSTEM_H_
#define _SOUND_SYSTEM_H_

#include "System/Base/Systems.h"
#include "System/Sound/SoundTypes.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/SharedPtr.h"

#include <string>

#define MAX_SOUND_CHANNELS 16

namespace FMOD
{
	class Sound;
	class System;
	class Channel;
	class ChannelGroup;
}

namespace Dystopia
{
	class AudioSource;
	struct SoundInfo;

	class SoundSystem : public Systems
	{
		struct SoundInfo
		{
			SoundInfo(const std::string&, FMOD::Sound*);
			~SoundInfo(void);

			std::string strName;
			FMOD::Sound* mpSound;
		};

	public:

		explicit SoundSystem(int = MAX_SOUND_CHANNELS);
		~SoundSystem(void);

		bool Init();
		void Update(float _dt);
		void Shutdown(void);

		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);

		// Loads audio to memory and returns the handle to it
		SharedPtr<SoundInfo> LoadAudio(const std::string& _strFile, eSoundType = eSoundType::AUDIOTYPE_MASTER);
		SharedPtr<SoundInfo> LoadAudioStreaming(const std::string& _strFile, eSoundType = eSoundType::AUDIOTYPE_MASTER);

		// Attempts to the play a sound and returns the channel the sound is using
		FMOD::Channel* Play(SharedPtr<SoundInfo>&, eSoundType, const float _fVolume = 1.f);

		// Sets the volume of all sounds of a specific type.
		// Volume has to be in the closed range of [0, 1] with 0 being silent.
		void SetVolume(eSoundType, float _fVolume);

		void ToggleMute(eSoundType, bool _bDisable);
		bool IsMute(eSoundType) const;
		bool IsDisabled(void) const;

		void UnloadSound(const SharedPtr<SoundInfo>&);
		void UnloadAllSound(void);

	private:

		bool mbIsDisabled;
		FMOD::System* mpFMOD;
		AutoArray<FMOD::ChannelGroup*> mpChannelGroups;

		AutoArray<SharedPtr<SoundInfo>> LoadedSounds;
	};
}



// Remove our defines!
#ifdef MAX_SOUND_CHANNELS
#undef MAX_SOUND_CHANNELS
#endif



#endif		// INCLUDE GUARD

