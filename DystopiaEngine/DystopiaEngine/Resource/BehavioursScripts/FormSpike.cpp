/* HEADER *********************************************************************************/
/*!
\file	FormSpike.cpp
\author Shannon (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "FormSpike.h"
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
#include "System/Graphics/Texture.h"
#include "System/File/FileSystem.h"

namespace Dystopia
{
	namespace FormSpike_MSG
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
	FormSpike::FormSpike()
		: mForceDir{0,1},
		mfIntervals{0.5f},
		mfTimeCounter{0.f},
		mnCurrentSpike{0},
		mbFacing{true},
		mfSpacing{1},
		mArrFoundEnemies{},
		mfDelayStart{0},
		mfStartX{0.f}
	{
	}

	FormSpike::~FormSpike()
	{
	}
	
	void FormSpike::Awake()
	{
		mbFacing = true;
		mfTimeCounter = mfIntervals;
		mfStartX = 0.f;
	}

	void FormSpike::Init()
	{
		mbFacing = true;
		mfStartX = 0.f;
		HashString database{"Form_Skills.ddata"};
		auto db =  EngineCore::Get<DatabaseSystem>();
		if (db->OpenData(database))
		{
			mfIntervals = db->GetDatabase(database)->GetElement<float>(HashString{"Spike_Intervals"});
			mfSpacing = db->GetDatabase(database)->GetElement<float>(HashString{"Spike_Spacing"});
		}
		mfTimeCounter = mfIntervals;
	}

	void FormSpike::Update(const float _fDeltaTime)
	{
		if (mbActivate)
		{
			if (mfDelayStart <= 0.f) 
			{
				if (mnCurrentSpike > 0 && mnCurrentSpike < 4)
				{
					mfTimeCounter -= _fDeltaTime;
					if (mfTimeCounter <= 0.f || mnCurrentSpike == 1)
					{
						mfTimeCounter = mfIntervals;
						SpawnSpike();
						mnCurrentSpike++;
					}
				}
			}
			else
				mfDelayStart -= _fDeltaTime;
		}
	}
	
	void FormSpike::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	FormSpike * FormSpike::Duplicate() const
	{
		return new FormSpike{*this};
	}
	
	void Dystopia::FormSpike::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj && _obj->HasTag("Enemy"))
		{
			for (const auto& elem : mArrFoundEnemies)
				if (elem == _obj->GetID())
					return;
				
			mArrFoundEnemies.Insert(_obj->GetID());
		}
	}

	void Dystopia::FormSpike::OnTriggerExit(GameObject * const _obj)
	{
		if (_obj && _obj->HasTag("Enemy"))
		{
			for (size_t i = 0; i < mArrFoundEnemies.size(); i++)
			{
				if (mArrFoundEnemies[i] == _obj->GetID())
				{
					mArrFoundEnemies.FastRemove(i);
					return;
				}
			}
		}
	}

	const char * const FormSpike::GetBehaviourName() const
	{
		return FormSpike::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData FormSpike::GetMetaData() 
	{
		static MetaData<Dystopia::FormSpike> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const FormSpike::GetMetaData() const
	{
		static MetaData<Dystopia::FormSpike> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void FormSpike::SetKnockDirection(Math::Vec2 _dir)
	{
		mForceDir = _dir;
	}
	
	void FormSpike::StartSkill(void)
	{
		mbActivate = true;
		mfTimeCounter = mfIntervals;
		mnCurrentSpike = 1;
	}

	void FormSpike::SetFacing(bool _right)
	{
		mbFacing = _right;
	}
	
	void FormSpike::SpawnSpike(void)
	{
        float w = 0;
        switch (mnCurrentSpike)
        {
            case 3:
                w += EngineCore::Get<GraphicsSystem>()->LoadTexture(
                     EngineCore::Get<FileSystem>()->GetFromResource("Form_Spike_3.png").c_str()
                     )->GetWidth();
            case 2:
                w += EngineCore::Get<GraphicsSystem>()->LoadTexture(
                     EngineCore::Get<FileSystem>()->GetFromResource("Form_Spike_2.png").c_str()
                     )->GetWidth();
		}
        
        auto db =  EngineCore::Get<DatabaseSystem>();
        HashString database{"Form_Skills.ddata"};
        if (db->OpenData(database))
            w *= db->GetDatabase(database)->GetElement<float>("Spike_Universal_Scale");
        
		auto spawnPt = GetOwner()->GetComponent<Transform>()->GetGlobalPosition(); 
		float maxRange = GetOwner()->GetComponent<Transform>()->GetGlobalScale().x * 0.5f;
		float space = mbFacing ? (mfSpacing * w) : (mfSpacing * -1.f * w);
		spawnPt.x = spawnPt.x + maxRange;
		
		if (mnCurrentSpike == 1)
		{
			int closest = -1;
			float prevDiff = -FLT_MAX;
			for (int i = 0; i < mArrFoundEnemies.size(); i++)
			{
				auto obj = EngineCore::Get<SceneSystem>()->FindGameObject(mArrFoundEnemies[i]);
				float currX = obj->GetComponent<Transform>()->GetGlobalPosition().x;
				float currDiff = spawnPt.x - currX;
				if (currDiff >= prevDiff)
				{
					closest = i;
					prevDiff = currDiff;
				}
			}
			mArrFoundEnemies.clear();
			mfStartX = closest >= 0 ? spawnPt.x - prevDiff : spawnPt.x + space;
		}
		
		spawnPt.x = mfStartX + space;

		auto inst = EngineCore::Get<SceneSystem>()->Instantiate(HashString{"Form_Spike_Child.dobj"}, spawnPt);
		if (inst)
		{
			FormSpike_MSG::SendExternalMessage(inst, "SetSpikeID", mnCurrentSpike);
			FormSpike_MSG::SendExternalMessage(inst, "SetDeathCountdown", mfIntervals * 7.f);
			FormSpike_MSG::SendExternalMessage(inst, "SetKnockDirection", mForceDir);
			FormSpike_MSG::SendExternalMessage(inst, "SetFacing", mbFacing);
			FormSpike_MSG::SendExternalMessage(inst, "StartSkill");
		}
	}
	
}




