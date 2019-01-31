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

#pragma warning(push)
#pragma warning(disable : 4251)
#include "Component/ComponentList.h"	
#include "Component/Component.h"
#include "Math/Vectors.h"
#include "System/Sound/SoundTypes.h"

#include <string>
#include "Utility/MetaAlgorithms.h"

struct FMOD_VECTOR;

namespace Dystopia
{
	class  Sound;
	class  SoundSystem;

	class _DLL_EXPORT AudioSource : public Component
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
		void		SetChannel(const Channel &);
		void        SetMinMax(float _Min, float _Max = 10000.f);
		void        ResetMinMax();

		void		SetFrequency(float _freq);
		float		GetFrequency(void) const;
		void		SetPitch(float _pitch);
		float		GetPitch(void) const;

		void Play(void);
		void Pause(void);
		void Stop(void);
		void SetVolume(float);
		void ChangeAudio(const char * _s);

		bool IsReady(void) const;
		bool IsPaused(void) const;
		bool IsPlaying(void) const;
		bool IsALoop(void) const;
		void SetReady(bool);
		FMOD_VECTOR* GetFMODPos() const;
		void         UpdateFMODPos(Math::Vec3D const & _vec);
	private:
		Sound		* mpSound;
		Channel		  mChannel;
		FMOD_VECTOR * mPos;
		/* allow UI edit */
		int			mSoundType;
		bool		mPlayOnStart;
		bool		mLoop;
		float		mVolume;
		float		mFrequency;
		float		mPitch;
		float       mRange;
		float       mMaxRange;
		/* no UI edit allowed*/
		std::string mSoundName;
		bool		mPaused;
		bool		mIsPlaying;
		bool		mReady;
		bool		mChanged;

	};
}

#pragma warning(pop)
#endif	// _AUDIO_SOURCE_H_



