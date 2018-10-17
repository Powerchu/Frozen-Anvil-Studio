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

#include "Component/Component.h"		// Base Class
#include "Component/ComponentList.h"	// TRANSFORM
#include <string>

namespace FMOD
{
	class Sound;
	class Channel;
	class System;
}

namespace Dystopia
{
	class AudioSource : public Component
	{
	public:
		using SYSTEM = class SoundSystem;
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

		AudioSource(void) noexcept;
		AudioSource(GameObject* _pOwner) noexcept;
		AudioSource(const AudioSource&) noexcept;
		~AudioSource(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Awake(void);
		void Load(void);
		void Init(void);

		void GameObjectDestroy(void);
		void Unload(void);

		void DestroyComponent(void);

		void SetOwner(GameObject*);

		AudioSource* Duplicate() const;

		void Serialise(TextSerialiser&) const = 0;
		void Unserialise(TextSerialiser&) = 0;
		void EditorUI(void) noexcept;

	private:
		
		std::string name;
	
	};
}


#endif	// _AUDIO_SOURCE_H_

