/* HEADER *********************************************************************************/
/*!
\file	CheckPoint.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "CheckPoint.h"
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
#include "Component/AudioSource.h"
#include "Component/Emitter.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/File/FileSystem.h"
#include "System/Tag/Tags.h"

namespace Dystopia
{
	namespace CheckPoint_MSG
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
	CheckPoint::CheckPoint()
        : mfRadialDuration{1.f},
        mfRadialDt{0.f},
        mbActivated{false},
        mpGlow{nullptr},
        mpGlowBG{nullptr},
        mpEmitters{nullptr,nullptr,nullptr}
	{
	}

	CheckPoint::~CheckPoint()
	{
	}
    
	void CheckPoint::Awake()
	{
        mfRadialDt = 0.f;
        mbActivated = mbCompleted = false;
        mpGlow = nullptr;
        mpGlowBG = nullptr;
        mpEmitters[0] = mpEmitters[1] = mpEmitters[2] = nullptr;
	}

	void CheckPoint::Init()
	{
        mfRadialDt = 0.f;
        mbActivated = mbCompleted = false;
        auto& children = GetOwner()->GetComponent<Transform>()->GetAllChild();
        for (auto& c : children)
        {
            if (c->GetOwner()->GetName() == "Glow")
            {
                mpGlow = c->GetOwner();
                if (auto sr = mpGlow->GetComponent<SpriteRenderer>())
                {
                    if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Ping"))
                    {
                        sr->SetShader(shader);
                        sr->SetManualShaderOverride("Gamma", 1.f);
                        sr->SetManualShaderOverride("fRatio", 0.f);
                    }
                }  
            }
            else if (c->GetOwner()->GetName() == "Glow_BG")
            {
                mpGlowBG = c->GetOwner();
                if (auto sr = mpGlowBG->GetComponent<SpriteRenderer>())
                {
                    if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Ping"))
                    {
                        sr->SetShader(shader);
                        sr->SetManualShaderOverride("Gamma", 0.f);
                        sr->SetManualShaderOverride("fRatio", 1.f);
                    }
                }   
            }
            else if (c->GetOwner()->GetName() == "BaseEmitterBack")
            {
                mpEmitters[0] = c->GetOwner();
                if (auto e = mpEmitters[0]->GetComponent<Emitter>())
                {
                    e->StopEmission();
                    e->SetActive(true);
                }
            }
            else if (c->GetOwner()->GetName() == "BaseEmitterFront")
            {
                mpEmitters[1] = c->GetOwner();
                if (auto e = mpEmitters[1]->GetComponent<Emitter>())
                {
                    e->StopEmission();
                    e->SetActive(true);
                }
            }
            else if (c->GetOwner()->GetName() == "RadialEmitter")
            {
                mpEmitters[2] = c->GetOwner();
                if (auto e = mpEmitters[2]->GetComponent<Emitter>())
                {
                    e->StopEmission();
                    e->SetActive(true);
                }
            }
        }  
        //mbActivated = true;
    }

	void CheckPoint::Update(const float _fDeltaTime)
	{
        if (mbActivated && !mbCompleted)
        {
            mfRadialDt += _fDeltaTime;
            
            float alpha = Math::Clamp(mfRadialDt / mfRadialDuration, 0.f, 1.f);
            
            if (mpGlow)
            {
                if (auto sr = mpGlow->GetComponent<SpriteRenderer>())
                {
                    if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Ping"))
                    {
                        sr->SetShader(shader);
                        sr->SetManualShaderOverride("fRatio", alpha);
                    }
                }  
            }
            
            if (mfRadialDt >= mfRadialDuration)
                mbCompleted = true;
        }
	}

	void CheckPoint::GameObjectDestroy(void)
	{
        Behaviour::GameObjectDestroy();
	}
    
	void Dystopia::CheckPoint::OnTriggerEnter(GameObject * const _obj)
	{
        ActivateVfx(_obj);
	}

	void Dystopia::CheckPoint::OnTriggerStay(GameObject * const _obj)
	{
        ActivateVfx(_obj);
	}

	void Dystopia::CheckPoint::OnTriggerExit(GameObject * const _obj)
	{
        ActivateVfx(_obj);
	}

	CheckPoint * CheckPoint::Duplicate() const
	{
		return new CheckPoint{*this};
	}

	const char * const CheckPoint::GetBehaviourName() const
	{
		return CheckPoint::BehaviourName;
	}

	TypeErasure::TypeEraseMetaData CheckPoint::GetMetaData()
	{
		static MetaData<Dystopia::CheckPoint> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const CheckPoint::GetMetaData() const
	{
		static MetaData<Dystopia::CheckPoint> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
    
    void CheckPoint::ActivateVfx(GameObject * const _obj)
    {
        if (!mbActivated && _obj && _obj->HasTag("Player"))
        {
            for (unsigned i = 0; i < 3; i++)
                if (auto e = mpEmitters[i]->GetComponent<Emitter>())
                    e->StartEmission();
            mbActivated = true;
            CheckPoint_MSG::SendExternalMessage(_obj, "SetSpawnPoint", GetOwner()->GetComponent<Transform>()->GetGlobalPosition());
		    auto Fader = EngineCore::Get<SceneSystem>()->FindGameObject("White_Fader");
            CheckPoint_MSG::SendExternalMessage(Fader, "StartFadeReverse");
            if (auto audioSrc = GetOwner()->GetComponent<AudioSource>())
            {
                audioSrc->Play();
            }
        }
    }
     
}




