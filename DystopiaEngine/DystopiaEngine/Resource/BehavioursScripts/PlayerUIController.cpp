/* HEADER *********************************************************************************/
/*!
\file	PlayerUIController.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "PlayerUIController.h"
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
	namespace PlayerUIController_MSG
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
	
	PlayerUIController::PlayerUIController()
		: mpHealtbar{nullptr},
		mpMode{nullptr},
		mpStamina{nullptr},
		mfCurHealth{1.f},
		mfCurDec{1.f},
		mfSpeed{1.f},
		mfAccuDt{0.f},
		mbInForce{true},
		mfCurStamina{1.f},
		mfStamSpeed{1.f},
		mfTargetHealth{-1.f},
		mfTargetStamina{-1.f},
		mnCombatCounter{0},
		mfCounterDt{0},
		mfCounterDuration{1.f},
		mfCounterFadeDuration{1.f},
		mbShowCounter{false},
		mbFadeCounter{false},
		mfImpactStay{1.f},
		mfImpactFade{1.f},
        mfImpactDt{0.f},
        mbImpactShow{false},
        mbImpactFadeOut{false},
        mpImpactUI{nullptr},
		mpPlayer{nullptr}
	{
	}

	PlayerUIController::~PlayerUIController()
	{ 
	}

	void PlayerUIController::Awake()
	{
		mpHealtbar = nullptr;
		mpMode = nullptr;
		mpStamina = nullptr;
        mpImpactUI = nullptr;
		mpPlayer = nullptr;
		mbInForce = true;
        mbImpactShow = mbImpactFadeOut = false;
		mnCombatCounter = 0;
		mfCounterDt = mfImpactDt = 0.f;

		if (mpImpactUI)
			if (auto sr = mpImpactUI->GetComponent<SpriteRenderer>())
			{
				sr->SetActive(false);
				 sr->SetAlpha(0.f);
			}
	}

	void PlayerUIController::Init()
	{
		mnCombatCounter = 0;
		mfCounterDt = mfImpactDt = 0.f;
		mpHealtbar = nullptr;
		mpMode = nullptr;
		mpStamina = nullptr;
        mbImpactShow = mbImpactFadeOut = false;
		mpPlayer = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterController");
		auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
		for (auto& c : children)
		{
			if (c->GetOwner()->GetName() == "Health")
				mpHealtbar = c->GetOwner();
			else if (c->GetOwner()->GetName() == "Mode")
				mpMode = c->GetOwner();
			else if (c->GetOwner()->GetName() == "Stamina")
				mpStamina = c->GetOwner();
			else if (c->GetOwner()->GetName() == "Combat Counter")
				mpCombatCount = c->GetOwner();
            else if (c->GetOwner()->GetName() == "UI_Hit_Impact")
                mpImpactUI = c->GetOwner();
		}

		if (auto sr = mpImpactUI->GetComponent<SpriteRenderer>())
			sr->SetActive(false);

		if (auto tr = mpCombatCount->GetComponent<TextRenderer>())
		{
			tr->SetText(std::to_string(mnCombatCounter).c_str());
			tr->SetActive(false);
		}

		SetForceMode(mbInForce);
    }

	void PlayerUIController::Update(const float _fDeltaTime)
	{
		mfAccuDt += _fDeltaTime;
		if (mfAccuDt > 2.f)
			mfAccuDt -= 2.f;
		
		StaminaBarShader(_fDeltaTime);
		HealthbarShader(_fDeltaTime);
		CombatCounter(_fDeltaTime);
        ImpactUI(_fDeltaTime);
	}

	void PlayerUIController::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	PlayerUIController * PlayerUIController::Duplicate() const
	{
		return new PlayerUIController{*this};
	}

	const char * const PlayerUIController::GetBehaviourName() const
	{
		return PlayerUIController::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData PlayerUIController::GetMetaData()
	{
		static MetaData<Dystopia::PlayerUIController> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const PlayerUIController::GetMetaData() const
	{
		static MetaData<Dystopia::PlayerUIController> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void PlayerUIController::CombatCounter(float _dt)
	{
		if (!mpCombatCount) return;
		
		if (mbShowCounter)
		{
			if (auto tr = mpCombatCount->GetComponent<TextRenderer>())
			{
				tr->SetText(std::to_string(mnCombatCounter).c_str());
				mfCounterDt += _dt;
				
				if (mfCounterDt >= mfCounterDuration)
				{
					mfCounterDt = 0.f;
					mbShowCounter = false;
					mbFadeCounter = true;
				}
			}
		}
		if (mbFadeCounter)
		{
			if (auto tr = mpCombatCount->GetComponent<TextRenderer>())
			{
				tr->SetAlpha(Math::Clamp(1.f - Math::Clamp(mfCounterDt / mfCounterFadeDuration, 0.f, 1.f), 0.f, 1.f));
				mfCounterDt += _dt;
				if (mpPlayer)
            			PlayerUIController_MSG::SendExternalMessage(mpPlayer, "SetComboTimer", mfCounterFadeDuration-mfCounterDt);
				if (mfCounterDt >= mfCounterFadeDuration)
				{
					mfCounterDt = 0.f;
					mbFadeCounter = false;
					tr->SetActive(false);
					
				}
			}
		}
	}

	void PlayerUIController::HealthbarShader(float _dt)
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
	
	void PlayerUIController::StaminaBarShader(float _dt) 
	{
		if (mfTargetStamina >= 0.f && mfTargetStamina <= 1.f)
		{
			if (Math::ApproxEq(mfTargetStamina, mfCurStamina))
			{
				mfCurStamina = mfTargetStamina;
				mfTargetStamina = -1.f;
			}
			else
			{
				if (mfTargetStamina > mfCurStamina)
					mfCurStamina += (mfStamSpeed * _dt);
				else
					mfCurStamina -= (mfStamSpeed * _dt);
				
				if (mfCurStamina < mfTargetStamina)
				{
					mfCurStamina = mfTargetStamina;
					mfTargetStamina = -1.f;
				}
			}
		}
		
		if (mpStamina)
		{
			if (auto sr = mpStamina->GetComponent<SpriteRenderer>())
			{
				if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Health Bar"))
				{
					sr->SetShader(shader);

					if (Math::ApproxEq(mfCurStamina, 1.f))
					{
						sr->SetManualShaderOverride("fHealth", 1.1f);
						sr->SetManualShaderOverride("fRedZone", 1.1f);
					}
					else
					{
						sr->SetManualShaderOverride("fHealth", mfCurStamina);
						sr->SetManualShaderOverride("fRedZone", mfCurStamina);
					}


					sr->SetManualShaderOverride("fWave", mfAccuDt);
					sr->SetManualShaderOverride("vColor", Math::Vec4{1,1,1,1});
					sr->SetManualShaderOverride("vDmgCol", Math::Vec4{1,1,1,1});
				}
			}
		}
	}
    
    void PlayerUIController::ImpactUI(float _dt)
    {
        if (mpImpactUI)
        {
            if (auto sr = mpImpactUI->GetComponent<SpriteRenderer>())
            {
                if (mbImpactShow)
                {
                    mfImpactDt += _dt;
                    
                    sr->SetActive(true);
                    sr->SetAlpha(1.f);
                    if (mfImpactDt >= mfImpactStay)
                    {
                        mbImpactShow = false;
                        mbImpactFadeOut = true;
                        mfImpactDt = 0.f;
                    }
                }
                else if (mbImpactFadeOut)
                {
                    mfImpactDt += _dt;
                    sr->SetAlpha(Math::Clamp(Math::Clamp(1 - mfImpactDt/mfImpactFade, 0.f, 1.f), 0.f, 1.f));
                    if (mfImpactDt >= mfImpactFade)
                    {
                        sr->SetActive(false);
                        mbImpactFadeOut = false;
                        mfImpactDt = 0.f;
                    }
                }
            }
        }
    }
	
	void PlayerUIController::SetHealthPercentage(float _perc)
	{
		_perc = Math::Clamp(_perc, 0.f, 1.f);
		mfCurHealth = _perc;
		mfTargetHealth = -1.f;
	}
	
	void PlayerUIController::SetStaminaPercentage(float _perc)
	{
		_perc = Math::Clamp(_perc, 0.f, 1.f);
		mfCurStamina = _perc;
		mfTargetStamina = -1.f;
	}
	
	void PlayerUIController::SetForceMode(bool _b)
	{ 
		mbInForce = _b;
		if (mpMode)
		{  
			if (auto sr = mpMode->GetComponent<SpriteRenderer>())
			{
				HashString name = mbInForce ? "Force.png" : "Form.png";
				sr->SetTexture(CORE::Get<GraphicsSystem>()->LoadTexture(
				CORE::Get<FileSystem>()->GetFromResource(name.c_str()).c_str()));
			}
		}
		
		if (mpStamina)
		{  
			if (auto sr = mpStamina->GetComponent<SpriteRenderer>())
			{
				HashString name = mbInForce ? "force bar.png" : "form bar.png";
				sr->SetTexture(CORE::Get<GraphicsSystem>()->LoadTexture(
				CORE::Get<FileSystem>()->GetFromResource(name.c_str()).c_str()));
			}
		}
	}
	
	void PlayerUIController::GoToHealthPercentage(float _perc)
	{
		_perc = Math::Clamp(_perc, 0.f, 1.f);
		mfTargetHealth = _perc;
		
	}
	
	void PlayerUIController::GoToStaminaPercentage(float _perc)
	{
		_perc = Math::Clamp(_perc, 0.f, 1.f);
		mfTargetStamina = _perc;
	}
		
	void PlayerUIController::SetCounter(unsigned _n)
	{
		mnCombatCounter = _n;
		mbShowCounter = true;
		mbFadeCounter = false;
		mfCounterDt = 0.f;
		
		if (mpCombatCount)
			if (auto tr = mpCombatCount->GetComponent<TextRenderer>())
			{
				tr->SetAlpha(1.f);
				tr->SetActive(true);
			}
	}
    
    void PlayerUIController::TakeImpact(void)
    {
        mbImpactShow = true;
        mbImpactFadeOut = false;
        mfImpactDt = 0.f;
    }
}




