/* HEADER *********************************************************************************/
/*!
\file	EnvironmentalResponse.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "EnvironmentalResponse.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

// custom includes
#include "Component/SpriteRenderer.h"
#include "System/Tag/Tags.h"
#include "System/Database/DatabaseSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "Component/RigidBody.h"
#include "Component/Convex.h"
#include "Component/Collider.h"
#include "Math/MathLib.h"

namespace Dystopia
{
	namespace EnvironmentalResponse_MSG
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
	
	EnvironmentalResponse::EnvironmentalResponse()
		: AmbienceSmall{true},
		ForegroundReactions{false},
		AmbienceIntensity{1.f},
		AmbienceDuration{1.f},
		AmbienceFreq{1.f},
		ForegroundIntensity{1.f},
		ForegroundDuration{1.f},
		ForegroundFreq{1.f},
		mfAccuDt{0.f}, 
		mfDurationCounter{0.f},
		mfIntensityCounter{0.f},
		Direction{false},
		mbDoShake{false}
	{
	}

	EnvironmentalResponse::~EnvironmentalResponse()
	{
	}
	
	void EnvironmentalResponse::Init()
	{
		mfAccuDt = 0.f;
		ForegroundReactions = !AmbienceSmall;
		//if (ForegroundReactions)
		//	if (!GetOwner()->GetComponent<Convex>())
		//		DEBUG_PRINT(eLog::MESSAGE, "Missing convex trigger for script to work properly!");
	}

	void EnvironmentalResponse::Update(const float _fDeltaTime)
	{
		if (AmbienceSmall)
			Ambience(_fDeltaTime);
		else if (mbDoShake)
			Foreground(_fDeltaTime);
	}
	
	void EnvironmentalResponse::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	void Dystopia::EnvironmentalResponse::OnTriggerEnter(GameObject * const _obj)
	{
		if (!AmbienceSmall && _obj && (_obj->HasTag("Player") || _obj->HasTag("Enemy")))
		{
			mfIntensityCounter = ForegroundIntensity;
			mfDurationCounter = ForegroundDuration;
			mbDoShake = true;
		}
	}

	void Dystopia::EnvironmentalResponse::OnTriggerExit(GameObject * const _obj)
	{
		if (!AmbienceSmall && _obj && (_obj->HasTag("Player") || _obj->HasTag("Enemy")))
		{
			mfIntensityCounter = ForegroundIntensity;
			mfDurationCounter = ForegroundDuration;
			mbDoShake = true;
		}
	}

	EnvironmentalResponse * EnvironmentalResponse::Duplicate() const
	{
		return new EnvironmentalResponse{*this};
	}

	const char * const EnvironmentalResponse::GetBehaviourName() const
	{
		return EnvironmentalResponse::BehaviourName;
	}

	TypeErasure::TypeEraseMetaData EnvironmentalResponse::GetMetaData()
	{
		static MetaData<Dystopia::EnvironmentalResponse> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const EnvironmentalResponse::GetMetaData() const
	{
		static MetaData<Dystopia::EnvironmentalResponse> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
		
	void Dystopia::EnvironmentalResponse::Ambience(float _dt)
	{
		mfAccuDt += (_dt);
		if (mfAccuDt > 2.f)
			mfAccuDt -= 2.f;
		if (/*mfIntensityCounter > 0.f &&*/  mfDurationCounter > 0.f)
		{
			mfDurationCounter -= _dt;
			//mfIntensityCounter = Math::Lerp( 0.f, AmbienceIntensity, mfDurationCounter / AmbienceDuration);
		}
		else
		{
			mfIntensityCounter = AmbienceIntensity;
			mfDurationCounter = AmbienceDuration;
		}
		
		if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
		{
			if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Environment Shear"))
			{
				float val =  mfAccuDt * AmbienceFreq;
				val *= Direction ? 1.f : -1.f;
				sr->SetShader(shader);
				sr->SetManualShaderOverride("Intensity", mfIntensityCounter);
				sr->SetManualShaderOverride("Time", val);
			}
		}
	}
	
	void Dystopia::EnvironmentalResponse::Foreground(float _dt)
	{
		mfAccuDt += (_dt);
		if (mfAccuDt > 2.f)
			mfAccuDt -= 2.f;
		if (mfIntensityCounter > 0.f && mfDurationCounter > 0.f)
		{
			mfDurationCounter -= _dt;
			mfIntensityCounter = Math::Lerp( 0.f, ForegroundIntensity, mfDurationCounter / ForegroundDuration);
		}
		else
			mbDoShake = false;
		
		if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
		{
			if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Environment Shear"))
			{
				float val =  mfAccuDt * ForegroundFreq;
				val *= Direction ? 1.f : -1.f;
				sr->SetShader(shader);
				sr->SetManualShaderOverride("Intensity", mfIntensityCounter);
				sr->SetManualShaderOverride("Time", val);
			}
		}
	}
}




