/* HEADER *********************************************************************************/
/*!
\file	AudioSource.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _AUDIO_SOURCE_H_
#define _AUDIO_SOURCE_H_

#include "Component/ComponentList.h"	
#include "Component/Component.h"

#include "System/Sound/SoundTypes.h"

#include <string>

namespace Dystopia
{
	class Sound;
	class SoundSystem;
	class AudioSource : public Component
	{
	public:
		using SYSTEM = SoundSystem;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Audio Source"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }

		// ====================================== CONSTRUCTORS ======================================= // 

		AudioSource(void);
		AudioSource(const AudioSource&);
		~AudioSource(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Awake(void);
		void Load(void);
		void Init(void);

		void GameObjectDestroy(void);
		void Update(float);

		AudioSource* Duplicate() const;

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);
		void EditorUI(void) noexcept;

		// =============================== SOUND SPECIFIC FUNCTIONS =============================== // 
		
		Sound*&		GetSound(void);
		void		SetSound(Sound*);
		eSoundType	GetSoundType(void) const;
		void		SetSoundType(eSoundType);
		Channel&	GetChannel(void);
		void		SetChannel(Channel);

		void Play(void);
		void Pause(void);
		void Stop(void);
		void SetVolume(float);

		bool IsReady(void) const;
		bool IsPaused(void) const;
		bool IsPlaying(void) const;
		void SetReady(bool);

	private:
		Sound		*mpSound;
		Channel		mChannel;

		/* allow UI edit */
		int			mSoundType;
		bool		mPlayOnStart;
		bool		mLoop;
		float		mVolume;
		float		mFrequency;
		float		mPitch;

		/* no UI edit allowed*/
		std::string mSoundName;
		bool		mPaused;
		bool		mIsPlaying;
		bool		mReady;
		bool		mChanged;

	};
}


#endif	// _AUDIO_SOURCE_H_

