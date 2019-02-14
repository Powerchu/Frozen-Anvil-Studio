#pragma once
#ifndef _AUDIO_LISTENER_H
#define _AUDIO_LISTENER_H
#include "Component/Component.h"
#include "System/Sound/SoundTypes.h"
#include "Math/MathFwd.h"
struct FMOD_VECTOR;

namespace Dystopia
{
	class SoundSystem;

	class _DLL_EXPORT AudioListener : public Component
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
		static const std::string GetCompileName(void) { return "Audio Listener"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }

		// ====================================== CONSTRUCTORS ======================================= // 

		AudioListener(void);
		AudioListener(float _MaxRange);
		AudioListener(const AudioListener&);
		~AudioListener(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void Awake(void);
		void Load(void);
		void Init(void);

		void GameObjectDestroy(void);
		void Update(float);

		AudioListener* Duplicate() const;

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);
#if EDITOR
		void EditorUI(void) noexcept;
#endif

		float GetMaxRange() const;
		void  SetMaxRange(float const & _Range);
		FMOD_VECTOR* GetFMODPos() const;
		void         UpdateFMODPos(Math::Vec4 const & _vec);
	private:
		float mMaxRange;
		FMOD_VECTOR * mPos;
	};
}



#endif