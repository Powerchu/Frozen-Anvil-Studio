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
#ifndef _EMITTER_H_
#define _EMITTER_H_

#include "System/Particle/Particle.h"
#include "System/Particle/ParticleAffector.h"

#include "DataStructure/AutoArray.h"

#include "Math/MathFwd.h"
#include "Utility/Meta.h"
#include "Utility/Utility.h"

#include "Globals.h"

namespace Dystopia
{
	class Shader;
	class Texture;
	class Transform;
	class ParticleEmitter;
	struct ParticleAffector;

	class Emitter
	{
	public:
		explicit Emitter(ParticleEmitter* _owner = nullptr) noexcept;
		Emitter(const Emitter&) noexcept;
		~Emitter(void) noexcept;

		void Awake(void);
		void Init(void);

		// Update "physics"!
		virtual void FixedUpdate(float _dt);

		void Bind(void) const noexcept;
		void Unbind(void) const noexcept;
		void UploadBuffers(void) const noexcept;
		void Render(void) const noexcept;

		virtual void KillParticle(size_t _nIdx) noexcept;
		virtual void SpawnParticle(void) noexcept;
		void SetTexture(Texture*) noexcept;
		void SetOwner(ParticleEmitter*) noexcept;

		template <typename Ty>
		auto AddAffector(Ty&&) noexcept -> Ut::EnableIf_t<std::is_base_of_v<ParticleAffector, Ty>>;

		Shader& GetShader(void) noexcept;
		Transform const& GetOwnerTransform(void) const noexcept;

		size_t GetSpawnCount(void) const noexcept;
		GfxParticle& GetSpawnDefaults(void) noexcept;
		AutoArray<float>& GetLifetime(void) noexcept;
		AutoArray<float>& GetInitialLifetime(void) noexcept;
		AutoArray<Math::Vec3>& GetPosition(void) noexcept;
		AutoArray<Math::Vec4>& GetColour(void) noexcept;
		AutoArray<Math::Vec3>& GetVelocity(void) noexcept;
		AutoArray<Math::Vec3>& GetAcceleration(void) noexcept;
		AutoArray<float>& GetRotationalVelocity(void) noexcept;
		AutoArray<float>& GetRotationalAcceleration(void) noexcept;

		template <typename Ty>
		inline auto GetAffector(void) noexcept -> Ut::EnableIf_t<std::is_base_of_v<ParticleAffector, Ty>, Ty* const>;

		AutoArray<ParticleAffector>& GetSpawnAffectors(void) noexcept;
		AutoArray<ParticleAffector>& GetUpdateAffectors(void) noexcept;
		AutoArray<ParticleAffector>& GetFixedUpdateAffectors(void) noexcept;

		void Serialise(TextSerialiser&) const noexcept;
		void Unserialise(TextSerialiser&) noexcept;
		void EditorUI(void) noexcept;

		void StopEmission(void) noexcept;
		void StartEmission(void) noexcept;
		bool IsAlive(void) const noexcept;

	private:

		AutoArray<float>      mInitialLife;
		AutoArray<float>      mLifetime;
		AutoArray<float>      mRotVel;
		AutoArray<float>      mRotAcc;
		AutoArray<Math::Vec2> mSize;
		AutoArray<Math::Vec4> mColour;
		AutoArray<Math::Vec3> mAccel;
		AutoArray<Math::Vec3> mVelocity;
		AutoArray<Math::Vec3> mPosition;
		AutoArray<float>      mRotation;

		AutoArray<ParticleAffector> mSpawn;
		AutoArray<ParticleAffector> mUpdate;
		AutoArray<ParticleAffector> mFixedUpdate;
		AutoArray<ParticleAffector> mDeath;

		GfxParticle mParticle;

		ParticleEmitter* mpOwner;  Transform* mpTransform;
		Shader* mpShader; Texture* mpTexture;
		size_t mSpawnCount;
		HashString mTextureName, mShaderName;
		unsigned mVAO, mClrBuffer, mPosBuffer, mSzBuffer, mRotBuffer;

		int mnParticleLimit;
		bool mbUpdatedPositions;
		bool mbIsAlive;

		template <typename Ty, typename U>
		bool AffectorExists(Ty&, U);

		template <typename Ty>
		void AddAffector(Ty&&, AffectorTag::OnSpawn) noexcept;
		template <typename Ty>
		void AddAffector(Ty&&, AffectorTag::OnUpdate) noexcept;
		template <typename Ty>
		void AddAffector(Ty&&, AffectorTag::OnFixedUpdate) noexcept;
		template <typename Ty>
		void AddAffector(Ty&&, AffectorTag::OnDeath) noexcept;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 

template <typename Ty>
inline auto Dystopia::Emitter::AddAffector(Ty&& _affector) noexcept -> Ut::EnableIf_t<std::is_base_of_v<ParticleAffector, Ty>>
{
	AddAffector(Ut::Fwd<Ty>(_affector), typename Ut::RemoveRef_t<Ty>::UPDATE{});
}

template<typename Ty>
inline auto Dystopia::Emitter::GetAffector(void) noexcept -> Ut::EnableIf_t<std::is_base_of_v<ParticleAffector, Ty>, Ty* const>
{
	static const auto finder = [](AutoArray<ParticleAffector>& pool) -> Ty* {
		for (auto& e : pool)
			if (Ut::MetaFind_t<Ty, AffectorList>::value == e.GetID())
				return static_cast<Ty*>(&e);
		return { nullptr };
	};

	if constexpr (Ut::IsSame<AffectorTag::OnSpawn, typename Ty::UPDATE>::value)
		return finder(mSpawn);

	else if constexpr (Ut::IsSame<AffectorTag::OnUpdate, typename Ty::UPDATE>::value)
		return finder(mUpdate);

	else if constexpr (Ut::IsSame<AffectorTag::OnFixedUpdate, typename Ty::UPDATE>::value)
		return finder(mFixedUpdate);

	else if (Ut::IsSame<AffectorTag::OnDeath, typename Ty::UPDATE>::value)
		return finder(mDeath);

	return { nullptr };
}

template<typename Ty, typename U>
inline bool Dystopia::Emitter::AffectorExists(Ty& _arr, U _nID)
{
	for (auto& e : _arr)
		if (e.GetID() == _nID)
			return true;

	return false;
}

template <typename Ty>
inline void Dystopia::Emitter::AddAffector(Ty&& _affector, AffectorTag::OnSpawn) noexcept
{
	if (!AffectorExists(mSpawn, _affector.GetID()))
		mSpawn.EmplaceBack(Ut::Fwd<Ty>(_affector));
}

template <typename Ty>
inline void Dystopia::Emitter::AddAffector(Ty&& _affector, AffectorTag::OnUpdate) noexcept
{
	if (!AffectorExists(mUpdate, _affector.GetID()))
		mUpdate.EmplaceBack(Ut::Fwd<Ty>(_affector));
}

template <typename Ty>
inline void Dystopia::Emitter::AddAffector(Ty&& _affector, AffectorTag::OnFixedUpdate) noexcept
{
	if (!AffectorExists(mFixedUpdate, _affector.GetID()))
		mFixedUpdate.EmplaceBack(Ut::Fwd<Ty>(_affector));
}

template <typename Ty>
inline void Dystopia::Emitter::AddAffector(Ty&& _affector, AffectorTag::OnDeath) noexcept
{
	if (!AffectorExists(mDeath, _affector.GetID()))
		mDeath.EmplaceBack(Ut::Fwd<Ty>(_affector));
}




#endif		// INCLUDE GUARD

