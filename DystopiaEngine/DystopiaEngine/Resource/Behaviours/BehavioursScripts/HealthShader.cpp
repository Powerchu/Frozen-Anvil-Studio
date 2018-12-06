/* HEADER *********************************************************************************/
/*!
\file	HealthShader.cpp
\author JackyTan (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "HealthShader.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/GraphicsSystem.h"

#include "Component/SpriteRenderer.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

#include "Math/MathUtility.h"

namespace Dystopia
{
	namespace HealthShader_MSG
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
	HealthShader::HealthShader()
		: MaxHP{1}, CurrentHP{1}, Fun{0}, Speed{1}, TargetLerp{-1.f}
	{
	}

	HealthShader::~HealthShader()
	{
	}

	void HealthShader::Load()
	{
	}

	void HealthShader::Awake()
	{
		myShader = EngineCore::Get<GraphicsSystem>()->GetShader("Health");
		
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

	void HealthShader::Init()
	{
		myShader = EngineCore::Get<GraphicsSystem>()->GetShader("Health");
		 
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
		
		TargetLerp = 1.f;
	}

	void HealthShader::Update(const float _fDeltaTime)
	{
		float hp = LerpHP(_fDeltaTime);//(CurrentHP / MaxHP);
		Fun += _fDeltaTime * Speed; 
		if (Fun >= 0.5)
			Fun -= 0.5;
		
		if (myShader)
		{
			myShader->Bind();
			myShader->UploadUniform("fHealth", TargetLerp);
			myShader->UploadUniform("fRed", hp);
			myShader->UploadUniform("fWave", Fun);
			myShader->Unbind();
		}
		else
		{
			DEBUG_PRINT(eLog::ERROR, "DEATH!");
		}
	}

	void HealthShader::FixedUpdate(const float )
	{
	}

	void HealthShader::PostUpdate(void)
	{
	}

	void HealthShader::GameObjectDestroy(void)
	{
	}

	void HealthShader::Unload(void)
	{
	}

	void Dystopia::HealthShader::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::HealthShader::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::HealthShader::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::HealthShader::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::HealthShader::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::HealthShader::OnTriggerExit(GameObject * const )
	{
	}

	HealthShader * HealthShader::Duplicate() const
	{
		return new HealthShader{ *this };
	}

	void HealthShader::Serialise(TextSerialiser& ) const
	{
	}

	void HealthShader::Unserialise(TextSerialiser& )
	{
	}


	const char * const HealthShader::GetBehaviourName() const
	{
		return HealthShader::BehaviourName;
	}

	void HealthShader::EditorUI(void) noexcept
	{
	}
			
	void HealthShader::ChangeHP(float _dir)
	{
		CurrentHP += _dir;
	}
	
	void HealthShader::HPLerpTo(float _target)
	{
		TargetLerp = _target;
	}
	
	float HealthShader::LerpHP(float _dt)
	{	
		float ratio = (CurrentHP / MaxHP);
		if (Math::ApproxEq(ratio, TargetLerp))
		{
			TargetLerp = ratio;
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

	TypeErasure::TypeEraseMetaData HealthShader::GetMetaData()
	{
		static MetaData<Dystopia::HealthShader> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const HealthShader::GetMetaData() const
	{
		static MetaData<Dystopia::HealthShader> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{ mMetaData };
		return mReturn;
	}
}




