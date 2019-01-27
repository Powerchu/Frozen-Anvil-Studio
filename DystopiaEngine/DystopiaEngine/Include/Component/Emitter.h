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
#include "System/Particle/ParticleAffector.h"

#include "DataStructure/AutoArray.h"

#include "Math/MathFwd.h"
#include "Utility/Meta.h"
#include "Utility/Utility.h"


namespace Dystopia
{
	class Shader;
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

		void Bind(void) const noexcept;
		void Unbind(void) const noexcept;
		void UploadBuffers(void) const noexcept;
		void Render(void) const noexcept;

		template <typename Ty>
		auto AddEmitter(Ty&&) noexcept -> Ut::EnableIf_t<std::is_base_of_v<ParticleAffector, Ty>>;

		Shader& GetShader(void) noexcept;
		AutoArray<Math::Vec4>& GetColour(void) noexcept;
		AutoArray<Math::Vec4>& GetPosition(void) noexcept;
		AutoArray<Math::Vec3>& GetVelocity(void) noexcept;
		AutoArray<Math::Vec3>& GetAcceleration(void) noexcept;
		AutoArray<ParticleAffector>& GetUpdateAffectors(void) noexcept;
		AutoArray<ParticleAffector>& GetFixedUpdateAffectors(void) noexcept;

		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;
		void EditorUI(void) noexcept override;

	private:

		AutoArray<Math::Vec4> mColour;
		AutoArray<Math::Vec3> mAccel;
		AutoArray<Math::Vec3> mVelocity;
		AutoArray<Math::Vec4> mPosition;
		AutoArray<float> mLifetime;

		AutoArray<ParticleAffector> mAttach;
		AutoArray<ParticleAffector> mUpdate;
		AutoArray<ParticleAffector> mFixedUpdate;

		Shader* mpShader;
		unsigned mVAO, mColourBuffer, mPosBuffer;

		template <typename Ty>
		auto AddEmitter(Ty&&, AffectorTag::OnAttach) noexcept;
		template <typename Ty>
		auto AddEmitter(Ty&&, AffectorTag::OnUpdate) noexcept;
		template <typename Ty>
		auto AddEmitter(Ty&&, AffectorTag::OnFixedUpdate) noexcept;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 

template <typename Ty>
inline auto Dystopia::Emitter::AddEmitter(Ty&& _affector) noexcept -> Ut::EnableIf_t<std::is_base_of_v<ParticleAffector, Ty>>
{
	AddEmitter(Ut::Fwd<Ty>(emitter), typename Ut::RemoveRef_t<Ty>::UPDATE{});
}

template <typename Ty>
inline auto Dystopia::Emitter::AddEmitter(Ty&& _affector, AffectorTag::OnAttach) noexcept
{
	_affector.Update(*this, 1);
	mUpdate.EmplaceBack(Ut::Fwd<Ty>(_affector));
}

template <typename Ty>
inline auto Dystopia::Emitter::AddEmitter(Ty&& _affector, AffectorTag::OnUpdate) noexcept
{
	mUpdate.EmplaceBack(Ut::Fwd<Ty>(_affector));
}

template <typename Ty>
inline auto Dystopia::Emitter::AddEmitter(Ty&& _affector, AffectorTag::OnFixedUpdate) noexcept
{
	mFixedUpdate.EmplaceBack(Ut::Fwd<Ty>(_affector));
}




#endif		// INCLUDE GUARD

