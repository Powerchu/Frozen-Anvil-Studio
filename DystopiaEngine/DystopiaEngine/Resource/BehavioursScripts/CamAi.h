/* HEADER *********************************************************************************/
/*!
\file	CamAi.h
\author Aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _CamAi_H_
#define _CamAi_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"


#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class CamAi : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(CamAi);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "CamAi"; }
		static uint64_t constexpr mCamAiID = 18446744072572989440;

		// Default Constructor for this Behaviour - init your variables here
		CamAi();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~CamAi();

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
		virtual CamAi * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override;
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		float X_Offset = 0.f;
		float Y_Offset = 0.f;
		float FocusAreaWidth = 300.f;
		float FocusAreaXOffset = -5.f;
		float PanicZoneHeight = 500.f;
		float PanicZoneY_Offset = 30.f;
		float FocusArea_xPos = 0.f;
		float MaxCameraSpeed = 600.f;
		float MaxCameraPanDist = 30.f;
		float CamSmoothFactor = 4.0f;
		float mfMaxTimerCount = 3.0f;
		float mfTimer = 0.0f;

		bool isStolen = false;

		Math::Vector3D heroPos;
		Math::Vector3D charPos;

		void SetNewTarget(Math::Vector3D pos);
		void ResetTarget();
		PP_MEMBERFUNC(Dystopia::CamAi, SetNewTarget, ResetTarget)

	private:
		// Don't touch
		friend MetaData<CamAi>;

		

		GameObject* FocusAreaObj = nullptr;
		GameObject* CenterYObj = nullptr;
		GameObject* PanicZoneObj = nullptr;
		GameObject* HeroObj = nullptr;
		GameObject* CharacterObj = nullptr;
		GameObject* CamObj = nullptr;
		InputManager*   mpInput = nullptr;
		bool shouldYLerp = false;
	};

	extern "C"
	{
		inline DllExport CamAi * CamAiClone()
		{
			return new CamAi;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::CamAi)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT_STUFF(Dystopia::CamAi, X_Offset, Y_Offset, FocusAreaWidth, FocusAreaXOffset, PanicZoneHeight, PanicZoneY_Offset, FocusArea_xPos, MaxCameraSpeed, MaxCameraPanDist, CamSmoothFactor, mfMaxTimerCount, mfTimer)

#endif //_CamAi_H_


