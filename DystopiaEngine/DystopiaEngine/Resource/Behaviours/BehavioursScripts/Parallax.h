/* HEADER *********************************************************************************/
/*!
\file	Parallax.h
\author keith (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Parallax_H_
#define _Parallax_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Math/MathLib.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class Parallax : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(Parallax);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "Parallax"; }
		static uint64_t constexpr mParallaxID = 18446744071816374272;

		Parallax();
		~Parallax();
		
		virtual void Load(void) override;
		virtual void Init(void) override;

		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;

		virtual void OnCollisionEnter(const CollisionEvent&);
		virtual void OnCollisionStay (const CollisionEvent&);
		virtual void OnCollisionExit (const CollisionEvent&);

		virtual void OnTriggerEnter(const GameObject *);
		virtual void OnTriggerStay (const GameObject *);
		virtual void OnTriggerExit (const GameObject *);

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual Parallax * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;
		
		int mLayerDiff = 0;
		GameObject * mFocalTarget = nullptr;

	private:
		Math::Point3D mTargetOldPos;
		Math::Point3D mTargetNewPos;
		friend MetaData<Parallax>;
	};

	extern "C"
	{
		DllExport Parallax * ParallaxClone()
		{
			return new Parallax;
		}
	}
}

PP_REFLECT(Dystopia::Parallax, mLayerDiff);
#endif //_Parallax_H_


