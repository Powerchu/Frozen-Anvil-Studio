/* HEADER *********************************************************************************/
/*!
\file	SkillManager.cpp
\author Dan Kang (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SkillManager.h"
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
	namespace SkillManager_MSG
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
	SkillManager::SkillManager()
		: spawnCount(0.0f)
		, spawningSpike(false)
		, oneSpawned(false)
		, twoSpawned(false)
		, currX(0.0f)
		, currY(0.0f)
		, currZ(0.0f)
	{
	}

	SkillManager::~SkillManager()
	{
	}

	void SkillManager::Load()
	{
	}

	void SkillManager::Awake()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void SkillManager::Init()
	{
		SetFlags(FLAG_ACTIVE);
	}

	void SkillManager::Update(const float _fDeltaTime)
	{
		if (spawningSpike)
		{
			spawnCount = spawnCount + _fDeltaTime;
			CheckSpawn();
		}
	}

	void SkillManager::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void SkillManager::PostUpdate(void)
	{
	}

	void SkillManager::GameObjectDestroy(void)
	{
	}

	void SkillManager::Unload(void)
	{
	}

	void Dystopia::SkillManager::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SkillManager::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SkillManager::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::SkillManager::OnTriggerEnter(GameObject * const _obj)
	{
	}

	void Dystopia::SkillManager::OnTriggerStay(GameObject * const _obj)
	{
	}

	void Dystopia::SkillManager::OnTriggerExit(GameObject * const _obj)
	{
	}

	SkillManager * SkillManager::Duplicate() const
	{
		return new SkillManager{*this};
	}

	void SkillManager::Serialise(TextSerialiser& _ser) const
	{
	}

	void SkillManager::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const SkillManager::GetBehaviourName() const
	{
		return SkillManager::BehaviourName;
	}

	void SkillManager::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData SkillManager::GetMetaData()
	{
		static MetaData<Dystopia::SkillManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SkillManager::GetMetaData() const
	{
		static MetaData<Dystopia::SkillManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}

	void SkillManager::CastForm(int _skillNum, bool isFacingRight, float x, float y, float z)
	{
		currX = x;
		currY = y;
		currZ = z;

		if (_skillNum == 0)
		{
			if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeAnim.dobj", Math::Vec3D{ currX, currY, currZ} + Math::Vec3D{40, 9, 0}))
				DEBUG_PRINT(eLog::MESSAGE, "spike anim spawned");
			else
				DEBUG_PRINT(eLog::MESSAGE, "spike anim spawned nullptr");

			spawningSpike = true;
		}

		if (_skillNum == 1)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSlamTwo.dobj", Math::Vec3D{x, y, z} + Math::Vec3D{5, 30, 0}))
				{
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
				}
			}

			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSlamTwo.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{-5, 30, 0 }))
				{
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
				}
			}
		}

		if (_skillNum == 2)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashAnim.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{45, 12, 0}))
					DEBUG_PRINT(eLog::MESSAGE, "SPAWNED ANIMATOR");
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{14, 8, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{64, 8, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
			}

			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashAnim.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{45, 12, 0}))
					DEBUG_PRINT(eLog::MESSAGE, "SPAWNED ANIMATOR");
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{14, 8, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{64, 8, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
			}
		}
	}

	void SkillManager::CastForce(int _skillNum, bool isFacingRight, float x, float y, float z)
	{
		if (_skillNum == 0)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceFlameOne.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{ 35, 11.5, 0}))
				{
					DEBUG_PRINT(eLog::MESSAGE, "FIRED A FLAMEEE");
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
				}
			}
			
			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceFlameOne.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{ -35, 11.5, 0}))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z);
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
				}
			}
		}

		else if (_skillNum == 1)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceExplosionTwo.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{ 40, 10,0 }))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
			}
			
			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceExplosionTwo.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{ -40, 10,0 }))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z);
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
				}
			}
		}

		else if (_skillNum == 2)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceBlastThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{2, 0, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
			}

			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceBlastThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{2, 0, 0}))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z);
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
				}
			}
		}

	}

	void SkillManager::CheckSpawn()
	{
		if (!oneSpawned)
		{
			oneSpawned = true;
			if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } +Math::Vec3D{ 20, 1.5, 0 }))
			{
				auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
				ptr->GetComponent<Transform>()->SetScale(scale.x * 1.5f, scale.y * 1.5f, scale.z); 
			}
		}

		else if (spawnCount > 0.2f && !twoSpawned)
		{
			twoSpawned = true;
			if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } + Math::Vec3D{ 33, 5, 0 }))
			{
				auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
				ptr->GetComponent<Transform>()->SetScale(scale.x * 1.75f, scale.y * 2.5f, scale.z); 
			}
		}

		else if (spawnCount > 0.4f)
		{
			if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } + Math::Vec3D{ 50, 7, 0 }))
			{
				auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
				ptr->GetComponent<Transform>()->SetScale(scale.x * 2.5f, scale.y * 3.5f, scale.z);
				spawningSpike = false;
				spawnCount = 0.0f;
				oneSpawned = false;
				twoSpawned = false;
			}
		}
	}
}




