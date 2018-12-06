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
		, facingRight(true)
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

	void SkillManager::FixedUpdate(const float )
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

	void Dystopia::SkillManager::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::SkillManager::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::SkillManager::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::SkillManager::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::SkillManager::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::SkillManager::OnTriggerExit(GameObject * const )
	{
	}

	SkillManager * SkillManager::Duplicate() const
	{
		return new SkillManager{*this};
	}

	void SkillManager::Serialise(TextSerialiser& ) const
	{
	}

	void SkillManager::Unserialise(TextSerialiser& )
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

		facingRight = isFacingRight;
		DEBUG_PRINT(eLog::MESSAGE, "%d", isFacingRight);

		if (_skillNum == 0)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeAnim.dobj", Math::Vec3D{ currX, currY, currZ} + Math::Vec3D{500, -50, 0}))
					DEBUG_PRINT(eLog::MESSAGE, "spike anim spawned");
				else
					DEBUG_PRINT(eLog::MESSAGE, "spike anim spawned nullptr");
			}

			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeAnim.dobj", Math::Vec3D{ currX, currY, currZ} - Math::Vec3D{500, 50, 0}))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z);
				}

				else
					DEBUG_PRINT(eLog::MESSAGE, "spike anim spawned nullptr");
			}
			
			spawningSpike = true;
		}

		if (_skillNum == 1)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSlamTwo.dobj", Math::Vec3D{x, y, z} + Math::Vec3D{150, 350, 0}))
				{
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
				}
			}

			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSlamTwo.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{-150, 350, 0 }))
				{	
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z);
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
				}
			}
		}

		if (_skillNum == 2)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashAnim.dobj", Math::Vec3D{ x, y, z } +Math::Vec3D{ 500, -50, 0 }))
				{
					DEBUG_PRINT(eLog::MESSAGE, "SPAWNED ANIMATOR");
				}

				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{150, -50, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{850, -50, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
			}

			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashAnim.dobj", Math::Vec3D{ x, y, z } +Math::Vec3D{ -500, -50, 0 }))
				{
					DEBUG_PRINT(eLog::MESSAGE, "SPAWNED ANIMATOR");
				}

				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{-150, -50, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 1);
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSmashThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{-850, -50, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
			}
		}
	}

	void SkillManager::CastForce(int _skillNum, bool isFacingRight, float x, float y, float z)
	{
		facingRight = isFacingRight;

		if (_skillNum == 0)
		{
			if (isFacingRight)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceFlameOne.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{ 500, 0, 0}))
				{
					DEBUG_PRINT(eLog::MESSAGE, "FIRED A FLAMEEE");
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
				}
			}
			
			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceFlameOne.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{ -500, 0, 0}))
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
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceExplosionTwo.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{ 350, 0,0 }))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
			}
			
			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceExplosionTwo.dobj", Math::Vec3D{ x, y, z } + Math::Vec3D{ -350, 0,0 }))
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
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceBarrageAnim.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{700, 75, 0}))
					DEBUG_PRINT(eLog::MESSAGE, "Spawned Nbarrageee");
				else
					DEBUG_PRINT(eLog::MESSAGE, "cant find"); 

				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceBarrageThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{900, -200, 0}))
					SkillManager_MSG::SendExternalMessage(ptr, "SetDirection", 2);
			}

			else
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceBarrageAnim.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{-700, 75, 0}))
				{	
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(-scale.x, scale.y, scale.z);
				}
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("ForceBarrageThree.dobj", Math::Vec3D{ x, y, z} + Math::Vec3D{-900, -200, 0}))
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
		if (facingRight)
		{
			if (!oneSpawned)
			{
				oneSpawned = true;
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } +Math::Vec3D{ 250, -200, 0 }))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(scale.x * 1.5f, scale.y * 1.5f, scale.z); 
				}
			}

			else if (spawnCount > 0.2f && !twoSpawned)
			{
				twoSpawned = true;
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } + Math::Vec3D{ 450, -150, 0 }))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(scale.x * 1.75f, scale.y * 2.5f, scale.z); 
				}
			}

			else if (spawnCount > 0.4f)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } + Math::Vec3D{ 600, -100, 0 }))
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

		else
		{
			if (!oneSpawned)
			{
				oneSpawned = true;
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } - Math::Vec3D{ 250, 200, 0 }))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(scale.x * 1.5f, scale.y * 1.5f, scale.z); 
				}
			}

			else if (spawnCount > 0.2f && !twoSpawned)
			{
				twoSpawned = true;
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } - Math::Vec3D{ 450, 150, 0 }))
				{
					auto scale = ptr->GetComponent<Transform>()->GetGlobalScale();
					ptr->GetComponent<Transform>()->SetScale(scale.x * 1.75f, scale.y * 2.5f, scale.z); 
				}
			}

			else if (spawnCount > 0.4f)
			{
				if (const auto ptr = EngineCore::GetInstance()->Get<SceneSystem>()->Instantiate("FormSpikeOne.dobj", Math::Vec3D{ currX, currY, currZ } - Math::Vec3D{ 600, 100, 0 }))
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
}




