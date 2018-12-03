/* HEADER *********************************************************************************/
/*!
\file	EnergyShader.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "EnergyShader.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Component/SpriteRenderer.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h" 

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

#include "Math/MathUtility.h"


namespace Dystopia
{
	namespace EnergyShader_MSG
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
	EnergyShader::EnergyShader()
		: MaxHP{1}, CurrentHP{1}, Fun{0}, Speed{1}, TargetLerp{-1.f}, ThreshCut{0.4f}
	{
	}

	EnergyShader::~EnergyShader()
	{
	}

	void EnergyShader::Load()
	{
	}

	void EnergyShader::Awake()
	{
	}

	void EnergyShader::Init()
	{
		myShader = EngineCore::Get<GraphicsSystem>()->GetShader("Energy");
		 
		if(!myShader)
		{
			DEBUG_PRINT(eLog::ERROR, "INIT FAILED!");
		}
		
		if (auto p = GetOwner()->GetComponent<SpriteRenderer>())
		{
			p->SetShader(myShader);
		}
		else
		{
			GetOwner()->AddComponent<SpriteRenderer>();
			p = GetOwner()->GetComponent<SpriteRenderer>();
			p->SetShader(myShader); 
		}
	}

	void EnergyShader::Update(const float _fDeltaTime)
	{
		float hp = LerpHP(_fDeltaTime); //(CurrentHP / MaxHP);
		if (hp < 1.f && Math::ApproxEq(hp, 1.f))
			hp = 1.001f;
		Fun += _fDeltaTime * Speed; 
		if (Fun >= 0.5)
			Fun -= 0.5;
		
		if (myShader) 
		{
			myShader->Bind();
			myShader->UploadUniform("fHealth", hp);
			myShader->UploadUniform("fWave", Fun);
			myShader->Unbind();
		} 
		else
		{
			DEBUG_PRINT(eLog::ERROR, "DEATH!");
		}
	}

	void EnergyShader::FixedUpdate(const float )
	{
	}

	void EnergyShader::PostUpdate(void)
	{
	}

	void EnergyShader::GameObjectDestroy(void)
	{
	}

	void EnergyShader::Unload(void)
	{
	}

	void Dystopia::EnergyShader::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::EnergyShader::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::EnergyShader::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::EnergyShader::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::EnergyShader::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::EnergyShader::OnTriggerExit(GameObject * const )
	{
	}

	EnergyShader * EnergyShader::Duplicate() const
	{
		return new EnergyShader{ *this };
	}

	void EnergyShader::Serialise(TextSerialiser& ) const
	{
	}

	void EnergyShader::Unserialise(TextSerialiser& )
	{
	}


	const char * const EnergyShader::GetBehaviourName() const
	{
		return EnergyShader::BehaviourName;
	}

	void EnergyShader::EditorUI(void) noexcept
	{


	}

	void EnergyShader::EnergyLerpTo(float _target)
	{
		TargetLerp = _target;
	}
	
	float EnergyShader::LerpHP(float _dt)
	{	
		float ratio = (CurrentHP / MaxHP);
		
		if (TargetLerp != -1.f && TargetLerp < ThreshCut)
			TargetLerp = ThreshCut;
			
		if (Math::ApproxEq(ratio + 0.5f, TargetLerp))
		{
			TargetLerp = -1.f;
			return ratio;
		}
		
		if (TargetLerp > -1.f)
		{
			if (ratio > TargetLerp)
			{
				CurrentHP -= ((ratio - TargetLerp) * _dt * MaxHP);
			}
			else if (ratio < TargetLerp)
			{
				CurrentHP += ((TargetLerp - ratio) * _dt * MaxHP);
			}
		}
		return CurrentHP / MaxHP;
	}
	
	TypeErasure::TypeEraseMetaData EnergyShader::GetMetaData()
	{
		static MetaData<Dystopia::EnergyShader> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const EnergyShader::GetMetaData() const
	{
		static MetaData<Dystopia::EnergyShader> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




