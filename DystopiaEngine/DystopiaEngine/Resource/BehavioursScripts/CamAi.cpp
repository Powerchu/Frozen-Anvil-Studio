/* HEADER *********************************************************************************/
/*!
\file	CamAi.cpp
\author Aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "CamAi.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Component/Collider.h"
#include "Component/RigidBody.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"



namespace Dystopia
{
	namespace CamAi_MSG
	{
		template<typename ... Ts>
		void SendInternalMessage(Behaviour * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendInternalMessage(_ptr, _FuncName, _Params...);
		}
		
		template<typename ... Ts>
		void SendExternalMessage(uint64_t _ObjectID, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ObjectID, _FuncName, _Params...);
		}
		template<typename ... Ts>
		void SendExternalMessage(const GameObject * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
		}
		
		template<typename ... Ts>
		void SendExternalMessage(GameObject * _ptr, const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
		}
		
		template<typename ... Ts>
		void SendAllMessage(const char * _FuncName, Ts ... _Params)
		{
			EngineCore::GetInstance()->Get<BehaviourSystem>()->SendAllMessage(_FuncName, _Params...);
		}
	}
	CamAi::CamAi()
	{
	}

	CamAi::~CamAi()
	{
	}

	void CamAi::Load()
	{
	}

	void CamAi::Awake()
	{
		HeroObj = EngineCore::Get<SceneSystem>()->FindGameObject("HeroFollow");
		CharacterObj = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController");
		CamObj  = EngineCore::Get<SceneSystem>()->FindGameObject("Main Camera");
		FocusAreaObj = EngineCore::Get<SceneSystem>()->FindGameObject("FocusArea");
		CenterYObj   = EngineCore::Get<SceneSystem>()->FindGameObject("CenterY");
		PanicZoneObj = EngineCore::Get<SceneSystem>()->FindGameObject("PanicZone");

		if (HeroObj)
			heroPos = HeroObj->GetComponent<Transform>()->GetGlobalPosition();
		
		if (CharacterObj)
			charPos = CharacterObj->GetComponent<Transform>()->GetGlobalPosition();
		mpInput = EngineCore::GetInstance()->GetSystem<InputManager>();
	}

	void CamAi::Init()
	{
		HeroObj = EngineCore::Get<SceneSystem>()->FindGameObject("HeroFollow");
		CharacterObj = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController");
		CamObj  = EngineCore::Get<SceneSystem>()->FindGameObject("Main Camera");
		FocusAreaObj = EngineCore::Get<SceneSystem>()->FindGameObject("FocusArea");
		CenterYObj   = EngineCore::Get<SceneSystem>()->FindGameObject("CenterY");
		PanicZoneObj = EngineCore::Get<SceneSystem>()->FindGameObject("PanicZone");
		
		mpInput = EngineCore::GetInstance()->GetSystem<InputManager>();
		heroPos = HeroObj->GetComponent<Transform>()->GetGlobalPosition();
		charPos = CharacterObj->GetComponent<Transform>()->GetGlobalPosition();
	}

	void CamAi::Update(const float _fDeltaTime)
	{ 
		
	}

	void CamAi::FixedUpdate(const float _fDeltaTime)
	{
		mfTimer -= _fDeltaTime;

		if (CamObj == nullptr ||
		CharacterObj == nullptr ||
		HeroObj == nullptr ||
		FocusAreaObj == nullptr ||
		CenterYObj == nullptr ||
		PanicZoneObj == nullptr) return;

		// Vars
		float smooth = 1.0f - std::powf(0.4f, _fDeltaTime * CamSmoothFactor);
		auto camPos = CamObj->GetComponent<Transform>()->GetGlobalPosition() + Math::Point3D{X_Offset, Y_Offset, 0};
		
		auto focusAreaPos = FocusAreaObj->GetComponent<Transform>()->GetGlobalPosition();
		if (!isStolen)
		{
		heroPos = HeroObj->GetComponent<Transform>()->GetGlobalPosition();
		charPos = CharacterObj->GetComponent<Transform>()->GetGlobalPosition();
		}
		float panicZonePosY = PanicZoneObj->GetComponent<Transform>()->GetGlobalPosition().y;
		auto heroScaleX = CharacterObj->GetComponent<Transform>()->GetGlobalScale().x;
		auto heroScaleY = CharacterObj->GetComponent<Transform>()->GetGlobalScale().y;
		auto heroBody = CharacterObj->GetComponent<RigidBody>(); 
		float temp = focusAreaPos.x;

		// Timer to return to neutral
		if (mpInput->GetAxis("Horizontal") != 0.0f || mpInput->GetAxis("L Stick Horizontal") != 0.0f || mpInput->GetButton("Roll") || mpInput->GetAxis("C_Roll") != 0.0f)
		{
			if (Math::Abs(static_cast<float>(heroBody->GetLinearVelocity().x)) > 50.f )
				mfTimer = mfMaxTimerCount;	
		}

		if (mpInput->GetButtonDown("Attack") || mpInput->GetButtonDown("Skill B") || mpInput->GetButtonDown("Skill Y"))
		{
			mfTimer = 0.f;
		}

		if (mfTimer <= 0.f)
		{
			mfTimer = 0.f;
		}

		// Camera Panning
		//x-axis
		float camPanX = mpInput->GetAxis("Camera X") * MaxCameraPanDist;
		float camPanY = mpInput->GetAxis("Camera Y") * MaxCameraPanDist;

		/*
		*	Update Stuff
		*/
		FocusAreaObj->GetComponent<Transform>()->SetGlobalScale(FocusAreaWidth, 1500,0);
		PanicZoneObj->GetComponent<Transform>()->SetGlobalScale(2500, PanicZoneHeight,0); 

		float xNewMove = camPos.x;
		float yNewMove = camPos.y;
		float smooth2 = 1.0f - std::powf(0.4f, _fDeltaTime * CamSmoothFactor*2);
		float smooth3 = 1.0f - std::powf(0.4f, _fDeltaTime * CamSmoothFactor/4);

		// X-Axis Lerp
		if (mfTimer > 0.f)
		{
			if ((charPos.x > (focusAreaPos.x + FocusAreaWidth/2) || charPos.x < (focusAreaPos.x - FocusAreaWidth/2)) && Math::Abs(static_cast<float>(heroBody->GetLinearVelocity().x)) > 10.f)
			{
				xNewMove = Math::Lerp(static_cast<float>(camPos.x), static_cast<float>(heroPos.x)+camPanX, smooth2);
			}
			else if (charPos.x < (focusAreaPos.x + FocusAreaWidth/2) || charPos.x > (focusAreaPos.x - FocusAreaWidth/2))
			{
				xNewMove = Math::Lerp(static_cast<float>(camPos.x), static_cast<float>(heroPos.x)+camPanX, smooth3/3);
			}
		}
		else
		{
			xNewMove = Math::Lerp(static_cast<float>(camPos.x), static_cast<float>(heroPos.x)+camPanX, smooth2/3);
		}
		
		// Y-Axis Lerp
		// Only Lerp when going UP out of Panic Zone or when landed (TODO: use character's mbLanded to help precisely tune)
		if ((heroBody->GetLinearVelocity().y <= 5.f && heroBody->GetLinearVelocity().y >= -5.f) ||
			(charPos.y+heroScaleY/4 > panicZonePosY+PanicZoneHeight/2))
		{
			yNewMove = Math::Lerp(static_cast<float>(camPos.y), static_cast<float>(charPos.y)+PanicZoneY_Offset+camPanY, smooth2*0.75f);
		}
		// Falling Down
		else if (charPos.y-heroScaleY/4 < panicZonePosY-PanicZoneHeight/1.3f)
		{
			yNewMove = Math::Lerp(static_cast<float>(camPos.y), static_cast<float>(charPos.y)-PanicZoneY_Offset+camPanY, smooth);
		}
		// Normal case
		else
		{
			yNewMove = Math::Lerp(static_cast<float>(camPos.y), static_cast<float>(charPos.y)+PanicZoneY_Offset+camPanY, smooth3);
		}
		
		CenterYObj->GetComponent<Transform>()->SetGlobalPosition(camPos);
		PanicZoneObj->GetComponent<Transform>()->SetGlobalPosition(camPos);
		CamObj->GetComponent<Transform>()->SetGlobalPosition(xNewMove, yNewMove, camPos.z);

		if (heroScaleX > 0)
		{
			auto newFollowPos = Math::Point3D(charPos.x+FocusAreaWidth/3+heroBody->GetLinearVelocity().x*0.75f, charPos.y+PanicZoneY_Offset,0);
			auto finalPos = Math::Lerp(heroPos, newFollowPos, smooth2);	
			//auto newCharPosX = Math::Lerp(static_cast<float>(charPos.x), static_cast<float>(charPos.x)-FocusAreaWidth/2+FocusAreaXOffset-heroBody->GetLinearVelocity().x*1.3f/CamSmoothFactor, smooth);
			HeroObj->GetComponent<Transform>()->SetGlobalPosition(finalPos);
			FocusAreaObj->GetComponent<Transform>()->SetGlobalPosition(static_cast<float>(charPos.x)-FocusAreaWidth/2+FocusAreaXOffset-heroBody->GetLinearVelocity().x*0.75f/CamSmoothFactor, camPos.y,0);
		}
		else
		{
			auto newFollowPos = Math::Point3D(charPos.x-FocusAreaWidth/3+heroBody->GetLinearVelocity().x*0.75f, charPos.y+PanicZoneY_Offset,0);
			auto finalPos = Math::Lerp(heroPos, newFollowPos, smooth2);	
			//auto newCharPosX = Math::Lerp(static_cast<float>(charPos.x), static_cast<float>(charPos.x)+FocusAreaWidth/2-FocusAreaXOffset-heroBody->GetLinearVelocity().x*1.3f/CamSmoothFactor, smooth);
			HeroObj->GetComponent<Transform>()->SetGlobalPosition(finalPos);
			FocusAreaObj->GetComponent<Transform>()->SetGlobalPosition(static_cast<float>(charPos.x)+FocusAreaWidth/2-FocusAreaXOffset-heroBody->GetLinearVelocity().x*0.75f/CamSmoothFactor, camPos.y,0);
		}

		
	}

	void CamAi::SetNewTarget(Math::Vector3D pos)
	{
		heroPos = charPos = pos;
		isStolen = true;
	}
	void CamAi::ResetTarget()
	{
		isStolen = false;

		heroPos = HeroObj->GetComponent<Transform>()->GetGlobalPosition();
		charPos = CharacterObj->GetComponent<Transform>()->GetGlobalPosition();
	}

	void CamAi::PostUpdate(void)
	{
	}

	void CamAi::GameObjectDestroy(void)
	{
	}

	void CamAi::Unload(void)
	{
	}

	void Dystopia::CamAi::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CamAi::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CamAi::OnCollisionExit(const CollisionEvent& _colEvent)
	{
		
	}

	void Dystopia::CamAi::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::CamAi::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::CamAi::OnTriggerExit(GameObject * const _obj)
	{

	}

	CamAi * CamAi::Duplicate() const
	{
		return new CamAi{*this};
	}

	void CamAi::Serialise(TextSerialiser& _ser) const
	{
	}

	void CamAi::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const CamAi::GetBehaviourName() const
	{
		return CamAi::BehaviourName;
	}

	void CamAi::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData CamAi::GetMetaData()
	{
		static MetaData<Dystopia::CamAi> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const CamAi::GetMetaData() const
	{
		static MetaData<Dystopia::CamAi> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




