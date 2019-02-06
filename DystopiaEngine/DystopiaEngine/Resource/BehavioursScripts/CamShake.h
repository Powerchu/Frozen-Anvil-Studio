/* HEADER *********************************************************************************/
/*!
\file	CamShake.h
\author Aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _CamShake_H_
#define _CamShake_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"
#include "Math/MathLib.h"
#include "Math/Vectors.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class CamShake : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(CamShake);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "CamShake"; }
		static uint64_t constexpr mCamShakeID = 18446744072858706432;

		// Default Constructor for this Behaviour - init your variables here
		CamShake();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~CamShake();

		// Load will never be called
		virtual void Load(void) override; 
		// Awake occurs when the editor is first started
		virtual void Awake(void) override; 
		// Init occurs when the scene is played
		virtual void Init(void) override; 

		// Update occurs every frame
		virtual void Update(const float _fDeltaTime) override; 
		// FixedUpdate occurs every 0.02ms
		virtual void FixedUpdate(const float _fDeltaTime) override; 
		// PostUpdate occurs after the above two updates
		virtual void PostUpdate(void) override;

		// GameObjectDestroy occurs when the gameobject is destroyed
		virtual void GameObjectDestroy(void) override;
		// Unload will never occur
		virtual void Unload(void) override;

		// If Owner has a Collider, and enters another collider, this occurs
		virtual void OnCollisionEnter(const CollisionEvent&);
		// If Owner has a Collider, and stays in another collider, this occurs
		virtual void OnCollisionStay (const CollisionEvent&);
		// If Owner has a Collider, and exits another collider, this occurs
		virtual void OnCollisionExit (const CollisionEvent&);

		// If Owner has a Trigger, and enters another trigger, this occurs
		virtual void OnTriggerEnter(GameObject * const);
		// If Owner has a Trigger, and stays in another trigger, this occurs
		virtual void OnTriggerStay (GameObject * const);
		// If Owner has a Trigger, and exit another trigger, this occurs
		virtual void OnTriggerExit (GameObject * const);

		virtual void Serialise(TextSerialiser&) const override;
		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		// Don't touch
		virtual CamShake * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		void InvokeShake(float _trauma = 1.0f, float _mult = 16.0f, float _mag = 2.0f, float _decay = 1.3f);

		bool  camShakeActive = false;
		bool  IsStationary = false;
		float trauma = 0.0f;
		float traumaMult = 16; //power of the shake
		float traumaMag = 0.8f; // range of movement
		float traumaRotMag = 0.3f; // rotation power
		float traumaDepthMag = 0.3f; // depth mult
		float traumaDecay = 1.3f; // how quickly the shake falls off
		Math::Vector3D defaultPos;

		PP_MEMBERFUNC(Dystopia::CamShake, InvokeShake)

	private:
		// Don't touch
		friend MetaData<CamShake>;
		float shakeCounter = 0;

		float GetFloat(float seed)
		{
			Math::PerlinNoise p;
			return p.noise(seed, shakeCounter) * 2.f;
		}

		Math::Vector3D GetVec3D();
	};

	extern "C"
	{
		inline DllExport CamShake * CamShakeClone()
		{
			return new CamShake;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::CamShake)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT_STUFF(Dystopia::CamShake, IsStationary, camShakeActive, trauma, traumaMult, traumaMag, traumaRotMag, traumaDepthMag, traumaDecay)

#endif //_CamShake_H_


