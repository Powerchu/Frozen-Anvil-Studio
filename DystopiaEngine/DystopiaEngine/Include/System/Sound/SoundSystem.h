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
#include "System/Sound/SoundTypes.h"

#include "DataStructure/AutoArray.h"
#include "DataStructure/Array.h"

#include "Globals.h"

#include <string>
#include <map>
#include <unordered_map>

namespace Dystopia
{
	static const std::string g_AudioCategory[Dystopia::eSOUND_LAST - 1] =
	{
		"BGM",
		"FX"
	};
	class AudioSource;
	class AudioListener;
	class SoundSystem : public Systems, public ComponentDonor<AudioSource>, public ComponentDonor<AudioListener>
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
		Sound* LoadSound(const HashString& _file);
		void   CreateSound(const char * _data, int _buffersize, int _noChannels, int _format, int _freq = 44100);
		char RegisterNewListener(AudioListener * const & _Listener);

		_DLL_EXPORT void SetMaster(float);
		_DLL_EXPORT void SetBGM(float);
		_DLL_EXPORT void SetFX(float);

		_DLL_EXPORT float GetMaster() const;
		_DLL_EXPORT float GetBGM() const;
		_DLL_EXPORT float GetFX() const;
		_DLL_EXPORT void StopAll(void);

	private:
		float mMasterVol;
		float mBGMVol;
		float mFXVol;
		bool  mbUpdateVol;

		FMOD::System *                           mpFMOD;
		std::string	                             mDefaultSoundFolder;
		std::map<std::string, Sound*>            mMapOfSounds;
		std::unordered_map<AudioListener*, char> mMapOfListeners;
		Array<FMOD::ChannelGroup*, eSOUND_LAST>  mArrGroups;

		AutoArray<FMOD::Channel *> mArrActiveChannels;

		void PlayAudio(AudioSource&);
	};
}


#endif	// _SOUND_SYSTEM_H_

