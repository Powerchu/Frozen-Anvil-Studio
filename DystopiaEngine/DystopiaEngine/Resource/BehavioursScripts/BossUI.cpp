/* HEADER *********************************************************************************/
/*!
\file	BossUI.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "BossUI.h"
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

//custom includes
#include "Component/SpriteRenderer.h"
#include "Component/TextRenderer.h"
#include "System/Graphics/GraphicsSystem.h" 
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/File/FileSystem.h"

#include <string>

namespace Dystopia
{
	namespace BossUI_MSG
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
	
	BossUI::BossUI()
		: mpHealtbar{nullptr},
		mfCurHealth{1.f},
		mfCurDec{1.f},
		mfSpeed{1.f},
		mfAccuDt{0.f},
		mfTargetHealth{-1.f},
		mpPlayer{nullptr},
		mpUICam{nullptr}
	{
	}

	BossUI::~BossUI()
	{ 
	}

	void BossUI::Awake()
	{
		mpHealtbar = nullptr;
		mpPlayer = nullptr;
		mpUICam = nullptr;
	}

	void BossUI::Init()
	{
		mpHealtbar = nullptr;
		mpPlayer = EngineCore::Get<SceneSystem>()->FindGameObject("Salamander");
        mpUICam = EngineCore::Get<SceneSystem>()->FindGameObject("BossUI");

		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& c : children)
		{
			if (c->GetOwner()->GetName() == "B_Health")
				mpHealtbar = c->GetOwner();
		}
    }

	void BossUI::Update(const float _fDeltaTime)
	{
		mfAccuDt += _fDeltaTime;
		if (mfAccuDt > 2.f)
			mfAccuDt -= 2.f;
		
		HealthbarShader(_fDeltaTime);
	}

	void BossUI::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	BossUI * BossUI::Duplicate() const
	{
		return new BossUI{*this};
	}

	const char * const BossUI::GetBehaviourName() const
	{
		return BossUI::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData BossUI::GetMetaData()
	{
		static MetaData<Dystopia::BossUI> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const BossUI::GetMetaData() const
	{
		static MetaData<Dystopia::BossUI> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void BossUI::HealthbarShader(float _dt)
	{
		if (mfTargetHealth >= 0.f && mfTargetHealth <= 1.f)
		{
			if (Math::ApproxEq(mfTargetHealth, mfCurHealth))
			{
				mfCurHealth = mfTargetHealth;
				mfTargetHealth = -1.f;
			}
			else
			{
				if (mfTargetHealth > mfCurHealth)
					mfCurHealth += (mfSpeed * _dt);
				else
					mfCurHealth -= (mfSpeed * _dt);
				
				if (mfTargetHealth < mfCurHealth)
				{
					mfCurHealth = mfTargetHealth;
					mfTargetHealth = -1.f;
				}
			}
		}
		
		// try lerp to
		if (Math::ApproxEq(mfCurDec, mfCurHealth))
			mfCurDec = mfCurHealth;
		else
		{
			if (mfCurHealth > mfCurDec)
				mfCurDec += (mfSpeed * _dt);
			else
				mfCurDec -= (mfSpeed * _dt);
			
			if (mfCurDec < mfCurHealth)
				mfCurDec = mfCurHealth;
		}
		
		if (mpHealtbar)
		{
			if (auto sr = mpHealtbar->GetComponent<SpriteRenderer>())
			{
				if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Health Bar"))
				{
					sr->SetShader(shader);
					sr->SetManualShaderOverride("fHealth", Math::ApproxEq(mfCurHealth, 1.f) ? 1.1f : mfCurHealth);
					sr->SetManualShaderOverride("fRedZone",Math::ApproxEq(mfCurDec, 1.f) ? 1.1f : mfCurDec);
					sr->SetManualShaderOverride("fWave", mfAccuDt);
					sr->SetManualShaderOverride("vColor", Math::Vec4{1,1,1,1});
					sr->SetManualShaderOverride("vDmgCol", Math::Vec4{1,0,0,1});
				}
			}
		} 
	}
	
	void BossUI::BossSetHealthPercentage(float _perc)
	{
		_perc = Math::Clamp(_perc, 0.f, 1.f);
		mfCurHealth = _perc;
		mfTargetHealth = -1.f;
	}
	
	void BossUI::BossGoToHealthPercentage(float _perc)
	{
		_perc = Math::Clamp(_perc, 0.f, 1.f);
		mfTargetHealth = _perc;
		
	}
}




