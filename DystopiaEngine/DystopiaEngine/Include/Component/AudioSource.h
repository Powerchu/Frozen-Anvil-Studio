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

#include <string>

namespace Dystopia
{
	class Audio;
	class SoundSystem;
	class AudioSource : public Component
	{
	public:
		using SYSTEM = SoundSystem;
		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, UsableComponents>::value;
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

		void Update(float);

		void GameObjectDestroy(void);
		void Unload(void);

		void DestroyComponent(void);

		void SetOwner(GameObject*);

		AudioSource* Duplicate() const;

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);
		void EditorUI(void) noexcept;

		// =============================== SOUND SPECIFIC FUNCTIONS =============================== // 

		Audio* GetAudio(void);
		void SetReady(bool = true);
		bool IsReady(void) const;
		bool IsLoop(void) const;
		bool IsPlaying(void) const;

	private:

		Audio	*mpAudio;
		bool	mPlay;
		bool	mLoop;
		bool    mIsPlaying;

	};
}


#endif	// _AUDIO_SOURCE_H_

