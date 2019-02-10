/* HEADER *********************************************************************************/
/*!
\file	CamShake.cpp
\author Aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "CamShake.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Component/Transform.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"



namespace Dystopia
{
	namespace CamShake_MSG
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
	CamShake::CamShake()
	{
	}

	CamShake::~CamShake()
	{
	}

	void CamShake::Load()
	{
	}

	void CamShake::Awake()
	{
		defaultPos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
	}

	void CamShake::Init()
	{
		defaultPos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
	}

	void CamShake::Update(const float _fDeltaTime)
	{
		if (!IsStationary)
			defaultPos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();

		if (trauma > 1.0f) trauma = 1.0f;
		else if (trauma < 0.0f) trauma = 0.0f;

		/* CAMERA SHAKE PERLIN NOISE LOGIC */
		if (camShakeActive && trauma > 0)
        {
            //increase the time counter (how fast the position changes) based off the traumaMult and some root of the Trauma
            shakeCounter += _fDeltaTime * std::powf(trauma,0.3f) * traumaMult;
            //Bind the movement to the desired range
            auto newPos = GetVec3D() * traumaMag * trauma; 
			auto eulerZ = GetFloat(2) * 90.f * traumaRotMag * trauma;
		    GetOwner()->GetComponent<Transform>()->SetGlobalPosition(defaultPos+newPos);
			//const auto newNewPos = newPos * traumaRotMag;
			//Math::Quaternion quat(0, 0, newNewPos.z, 1);
            //rotation modifier applied here
			GetOwner()->GetComponent<Transform>()->SetRotation(Math::Quaternion::FromEuler(0_rad,0_rad,Math::Degrees{eulerZ}));
    
            //decay faster at higher values
            trauma -= _fDeltaTime * traumaDecay * (trauma + 0.3f);
        }
        else
        {
         	//lerp back towards default position and rotation once shake is done
         	const auto newPos = Math::Lerp(GetOwner()->GetComponent<Transform>()->GetGlobalPosition(), defaultPos, _fDeltaTime);
         	GetOwner()->GetComponent<Transform>()->SetGlobalPosition(newPos);
         	const auto newNewPos = newPos * traumaRotMag;
		 	Math::Quaternion quat(0, 0, 0, 1);
         	//rotation modifier applied here
		 	GetOwner()->GetComponent<Transform>()->SetRotation(quat); 
			
        }

	}

	void CamShake::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void CamShake::PostUpdate(void)
	{
	}

	void CamShake::GameObjectDestroy(void)
	{
	}

	void CamShake::Unload(void)
	{
	}

	Math::Vector3D CamShake::GetVec3D()
	{
		return Math::Vector3D(GetFloat(1), GetFloat(10), GetFloat(100) * traumaDepthMag);
	}

	void Dystopia::CamShake::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CamShake::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CamShake::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::CamShake::OnTriggerEnter(GameObject * const _obj)
	{
	} 

	void Dystopia::CamShake::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::CamShake::OnTriggerExit(GameObject * const _obj)
	{
	}

	CamShake * CamShake::Duplicate() const
	{
		return new CamShake{*this};
	}

	void CamShake::Serialise(TextSerialiser& _ser) const
	{
	}

	void CamShake::Unserialise(TextSerialiser& _ser)
	{
	}

	void CamShake::InvokeShake(float _trauma, float _mult, float _mag, float _decay)
	{
		trauma = _trauma;
		traumaMult = _mult;
		traumaMag = _mag;
		traumaDecay = _decay;
	}


	const char * const CamShake::GetBehaviourName() const
	{
		return CamShake::BehaviourName;
	}

	void CamShake::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData CamShake::GetMetaData()
	{
		static MetaData<Dystopia::CamShake> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const CamShake::GetMetaData() const
	{
		static MetaData<Dystopia::CamShake> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




