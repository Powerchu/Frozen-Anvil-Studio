/* HEADER *********************************************************************************/
/*!
\file	ParticleEmitter.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Emitter

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_

#include "Component/Component.h"
#include "Component/ComponentList.h"
#include "System/Particle/Particle.h"
#include "System/Particle/ParticleAffector.h"

#include "DataStructure/AutoArray.h"

#include "Math/MathFwd.h"
#include "Utility/Meta.h"
#include "Utility/Utility.h"

#include "Globals.h"

namespace Dystopia
{
	class Emitter;
	class ParticleSystem;
	struct ParticleAffector;

	class _DLL_EXPORT ParticleEmitter : public Component
	{
	public:
		using SYSTEM = ParticleSystem;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Particle Emitter"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }

		ParticleEmitter(void) noexcept;
		ParticleEmitter(ParticleEmitter const&) noexcept;
		~ParticleEmitter(void) noexcept;

		void Awake(void);
		void Init(void);

		void FixedUpdate(float _dt);

		void Bind(void) const noexcept;
		void Unbind(void) const noexcept;

		void Render(void) const noexcept;

		Emitter* GetEmitter(unsigned) const noexcept;
		AutoArray<Emitter>& GetEmitters(void) noexcept;

		ParticleEmitter* Duplicate(void) const;

		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;
		void EditorUI(void) noexcept override;

	private:
		AutoArray<Emitter> mEmitters;
	};
}




#endif		// INCLUDE GUARD

