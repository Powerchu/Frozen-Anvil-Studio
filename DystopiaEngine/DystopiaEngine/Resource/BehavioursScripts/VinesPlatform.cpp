/* HEADER *********************************************************************************/
/*!
\file	VinesPlatform.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "VinesPlatform.h"
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
	namespace VinesPlatform_MSG
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
	VinesPlatform::VinesPlatform()
		: mbFall{false},
		mbShake{false},
		mfShakeDuration{},
		mfShakeIntensity{},
		mfFrequency{},
		mfAccuDt{},
		mfDurationCounter{},
		mfIntensityCounter{},
		mbPlayerFaceRight{false},
		mpPlatform{nullptr}
	{
	}

	VinesPlatform::~VinesPlatform()
	{
	}
	
	void VinesPlatform::Awake()
	{
		mbShake = mbFall = false;
		mpPlatform = nullptr;
	}

	void VinesPlatform::Init()
	{
		mbShake = mbFall = false;
		HashString database{"Interactable"};
		auto db =  EngineCore::Get<DatabaseSystem>();
		if (db->OpenData(database))
		{
			mfShakeIntensity = db->GetDatabase(database)->GetElement<float>(HashString{"Vines_Intensity"});
			mfShakeDuration = db->GetDatabase(database)->GetElement<float>(HashString{"Vines_Shake_Duration"});
			mfFrequency = db->GetDatabase(database)->GetElement<float>(HashString{"Vines_Frequency"});
		}
		for (auto& childTransform : GetOwner()->GetComponent<Transform>()->GetAllChild())
		{
			if (childTransform->GetOwner()->GetName().id() == StringHasher("Platform"))
			{
				mpPlatform = childTransform->GetOwner();
				break;
			}
		}
	}

	void VinesPlatform::Update(const float _fDeltaTime)
	{
		if (mbShake)
			ShakeUpdate(_fDeltaTime);
	}

	void VinesPlatform::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	void Dystopia::VinesPlatform::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::VinesPlatform::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::VinesPlatform::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::VinesPlatform::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::VinesPlatform::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::VinesPlatform::OnTriggerExit(GameObject * const )
	{
	}

	VinesPlatform * VinesPlatform::Duplicate() const
	{
		return new VinesPlatform{*this};
	}

	const char * const VinesPlatform::GetBehaviourName() const
	{
		return VinesPlatform::BehaviourName;
	}

	TypeErasure::TypeEraseMetaData VinesPlatform::GetMetaData()
	{
		static MetaData<Dystopia::VinesPlatform> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const VinesPlatform::GetMetaData() const
	{
		static MetaData<Dystopia::VinesPlatform> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void VinesPlatform::ExecuteMechanic(void)
	{
		if (!mbFall)
		{
			if (mpPlatform)
			{
				if (auto col = mpPlatform->GetComponent<Convex>())
				{
					col->SetScale({0.9f, 0.75f, 1.f});
				}
				if (auto rigid = mpPlatform->GetComponent<RigidBody>())
				{
					rigid->SetSleeping(false);
					rigid->Set_ToggleGravity(true);
					rigid->AddLinearImpulse({0, 10.f, 0});
				}
			}
				
			mbFall = true;
			mfAccuDt = 0.f;
			if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
			{
				sr->Stop();
				sr->SetAnimation(1);
				sr->SetSpeed(0.032f);
				sr->Play();
				for (auto& childTransform : GetOwner()->GetComponent<Transform>()->GetAllChild())
				{
					if (childTransform->GetOwner()->GetName().id() == StringHasher("Vines"))
					{
						if (auto cSr = childTransform->GetOwner()->GetComponent<SpriteRenderer>())
						{
							cSr->Stop();
							cSr->SetAnimation(1);
							cSr->SetSpeed(0.032f);
							cSr->Play();
						}
					}
				}
			}
		}
	}
	
	void VinesPlatform::TakeForce(float, Math::Vec2 _dir)
	{
		if (!mbShake && !mbFall)
		{
			mbShake = true;
			mfAccuDt = 0.f;
			mfIntensityCounter = mfShakeIntensity;
			mfDurationCounter = mfShakeDuration;
			mbPlayerFaceRight = _dir.x > 0.f;
		}
	}
	
	void VinesPlatform::ShakeUpdate(float _dt)
	{
		mfAccuDt += (_dt);
		if (mfAccuDt > 2.f)
			mfAccuDt -= 2.f;
		
		if (mfIntensityCounter > 0.f && mfDurationCounter > 0.f)
		{
			mfDurationCounter -= _dt;
			mfIntensityCounter = Math::Lerp( 0.f, mfShakeIntensity, mfDurationCounter / mfShakeDuration);
		}
		else 
			mbShake = false;
		
		if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
		{
			if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Vine Shear"))
			{
				float val =  mfAccuDt * mfFrequency;
				val*= mbPlayerFaceRight ? 1.f : -1.f;
				sr->SetShader(shader);
				sr->SetManualShaderOverride("Intensity", mfIntensityCounter);
				sr->SetManualShaderOverride("Time", val);
				for (auto& childTransform : GetOwner()->GetComponent<Transform>()->GetAllChild())
				{
                    HashString shaderName;
					float intensity = mfIntensityCounter;
                    auto nameID = childTransform->GetOwner()->GetName().id();
					if (nameID == StringHasher("Vines"))
                        shaderName = "Vine Shear";
                    else if (nameID == StringHasher("Platform"))
					{
						intensity *= 0.5f;
                        shaderName = "Shake Quad";
					}
                    else 
                        continue;
                    
                    if (auto shader2 = CORE::Get<ShaderSystem>()->GetShader(shaderName.c_str()))
                    {
                        if (auto cSr = childTransform->GetOwner()->GetComponent<SpriteRenderer>())
                        {
                            cSr->SetShader(shader2);
                            cSr->SetManualShaderOverride("Intensity", intensity);
                            cSr->SetManualShaderOverride("Time", val);
                        }   
                    }
				}
			}
		}
	}
}





