/* HEADER *********************************************************************************/
/*!
\file	Emitter.h
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


namespace Dystopia
{
	class Shader;
	class Texture;
	class ParticleSystem;
	struct ParticleAffector;

	class Emitter : public Component
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

		Emitter(void) noexcept;
		~Emitter(void) noexcept;

		void Awake(void);
		void Init(void);

		// Update "physics"!
		void FixedUpdate(float _dt);

		void Bind(void) const noexcept;
		void Unbind(void) const noexcept;
		void UploadBuffers(void) const noexcept;
		void Render(void) const noexcept;

		void KillParticle(unsigned _nIdx) noexcept;
		void SpawnParticle(void) noexcept;

		template <typename Ty>
		auto AddAffector(Ty&&) noexcept -> Ut::EnableIf_t<std::is_base_of_v<ParticleAffector, Ty>>;

		Shader& GetShader(void) noexcept;
		GfxParticle& GetSpawnDefaults(void) noexcept;
		AutoArray<Math::Vec4>& GetColour(void) noexcept;
		AutoArray<Math::Vec4>& GetPosition(void) noexcept;
		AutoArray<Math::Vec3>& GetVelocity(void) noexcept;
		AutoArray<Math::Vec3>& GetAcceleration(void) noexcept;
		AutoArray<ParticleAffector>& GetSpawnAffectors(void) noexcept;
		AutoArray<ParticleAffector>& GetUpdateAffectors(void) noexcept;
		AutoArray<ParticleAffector>& GetFixedUpdateAffectors(void) noexcept;

		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;
		void EditorUI(void) noexcept override;

	private:

		AutoArray<float>      mLifetime;
		AutoArray<Math::Vec4> mColour;
		AutoArray<Math::Vec3> mAccel;
		AutoArray<Math::Vec3> mVelocity;
		AutoArray<Math::Vec4> mPosition;

		AutoArray<ParticleAffector> mSpawn;
		AutoArray<ParticleAffector> mUpdate;
		AutoArray<ParticleAffector> mFixedUpdate;

		GfxParticle mParticle;

		Shader* mpShader; Texture* mpTexture;
		unsigned mVAO, mColourBuffer, mPosBuffer;
		unsigned mSpawnCount;

		template <typename Ty>
		auto AddAffector(Ty&&, AffectorTag::OnSpawn) noexcept;
		template <typename Ty>
		auto AddAffector(Ty&&, AffectorTag::OnUpdate) noexcept;
		template <typename Ty>
		auto AddAffector(Ty&&, AffectorTag::OnFixedUpdate) noexcept;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 

template <typename Ty>
inline auto Dystopia::Emitter::AddAffector(Ty&& _affector) noexcept -> Ut::EnableIf_t<std::is_base_of_v<ParticleAffector, Ty>>
{
	AddEmitter(Ut::Fwd<Ty>(emitter), typename Ut::RemoveRef_t<Ty>::UPDATE{});
}

template <typename Ty>
inline auto Dystopia::Emitter::AddAffector(Ty&& _affector, AffectorTag::OnSpawn) noexcept
{
	for (auto& e : mSpawn)
		if (_affector.GetID() == e.GetID())
			return;

	mSpawn.EmplaceBack(Ut::Fwd<Ty>(_affector));
}

template <typename Ty>
inline auto Dystopia::Emitter::AddAffector(Ty&& _affector, AffectorTag::OnUpdate) noexcept
{
	for (auto& e : mUpdate)
		if (_affector.GetID() == e.GetID())
			return;

	mUpdate.EmplaceBack(Ut::Fwd<Ty>(_affector));
}

template <typename Ty>
inline auto Dystopia::Emitter::AddAffector(Ty&& _affector, AffectorTag::OnFixedUpdate) noexcept
{
	for (auto& e : mFixedUpdate)
		if (_affector.GetID() == e.GetID())
			return;

	mFixedUpdate.EmplaceBack(Ut::Fwd<Ty>(_affector));
}




#endif		// INCLUDE GUARD

