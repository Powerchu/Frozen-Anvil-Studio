/* HEADER *********************************************************************************/
/*!
\file	CharacterHitBox.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "CharacterHitBox.h"
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
#include "Component/Transform.h"
#include "Component/Convex.h"
#include "Component/AudioSource.h"
#include "DataStructure/HashString.h"
#include "System/Tag/Tags.h"

namespace Dystopia
{
	namespace CharacterHitBox_MSG
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
	CharacterHitBox::CharacterHitBox()
		: mArrFoundEnemies{},
		mnEnemyCount{0},
		mArrInteractable{},
		mnInteractCount{0}
	{}
	
	CharacterHitBox::~CharacterHitBox()
	{}
	
	void CharacterHitBox::Awake()
	{
	}

	void CharacterHitBox::Init()
	{
		// if (GetOwner()->GetName().id() != StringHasher("CharacterHitBox"))
		// 	GetOwner()->SetName(HashString{"CharacterHitBox"});
		mArrFoundEnemies.clear();
		mArrInteractable.clear();
		
        mpAudioSrc = GetOwner()->GetComponent<AudioSource>();
		mnEnemyCount=0;
	}

	void CharacterHitBox::Update(const float)
	{
		mnEnemyCount = mArrFoundEnemies.size();
		mnInteractCount = mArrInteractable.size();
	}

	void CharacterHitBox::FixedUpdate(const float)
	{
	} 

	void CharacterHitBox::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}
	
	void Dystopia::CharacterHitBox::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj)
		{
			if (_obj->HasTag("Enemy"))
				EnemyEnteredHitbox(_obj->GetID());
			if (_obj->HasTag("Interactable"))
				InteractablesEnteredHitbox(_obj->GetID());
		}
	}

	void Dystopia::CharacterHitBox::OnTriggerStay(GameObject * const)
	{
	}

	void Dystopia::CharacterHitBox::OnTriggerExit(GameObject * const _obj)
	{
		if (_obj)
		{
			if ( _obj->HasTag("Enemy"))
				EnemyExitHitbox(_obj->GetID());
			if (_obj->HasTag("Interactable"))
				InteractablesExitHitbox(_obj->GetID());
		}
	}

	CharacterHitBox * CharacterHitBox::Duplicate() const
	{ 
		return new CharacterHitBox{*this};
	}
	
	const char * const CharacterHitBox::GetBehaviourName() const
	{
		return CharacterHitBox::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData CharacterHitBox::GetMetaData()
	{
		static MetaData<Dystopia::CharacterHitBox> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	} 
	TypeErasure::TypeEraseMetaData const CharacterHitBox::GetMetaData() const
	{
		static MetaData<Dystopia::CharacterHitBox> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	void CharacterHitBox::EnemyEnteredHitbox(const uint64_t& _id)
	{
		for (const auto& elem : mArrFoundEnemies)
			if (elem == _id)
				return;
		mArrFoundEnemies.Insert(_id);
	}
	
	void CharacterHitBox::SendDamage(float _amount)
	{
		for (size_t i = 0; i < mArrFoundEnemies.size(); i++)
		{
			GameObject * obj = EngineCore::Get<SceneSystem>()->FindGameObject(mArrFoundEnemies[i]);
			if (obj && !(obj->GetFlags() & FLAG_REMOVE))
				CharacterHitBox_MSG::SendExternalMessage(obj, "TakeDamage", _amount);
			else
				mArrFoundEnemies.FastRemove(i--);
		}
		for (size_t i = 0; i < mArrInteractable.size(); i++)
		{
			GameObject * obj = EngineCore::Get<SceneSystem>()->FindGameObject(mArrInteractable[i]);
			if (obj && !(obj->GetFlags() & FLAG_REMOVE))
				CharacterHitBox_MSG::SendExternalMessage(obj, "TakeDamage", _amount);
			else
				mArrInteractable.FastRemove(i--);
		}
	}
	
	void CharacterHitBox::SendForce(float _force, Math::Vec2 _dir)
	{
		for (size_t i = 0; i < mArrFoundEnemies.size(); i++)
		{
			GameObject * obj = EngineCore::Get<SceneSystem>()->FindGameObject(mArrFoundEnemies[i]);
			if (obj && !(obj->GetFlags() & FLAG_REMOVE))
				CharacterHitBox_MSG::SendExternalMessage(obj, "TakeForce", _force, _dir);
			else
				mArrFoundEnemies.FastRemove(i--);
		}
		for (size_t i = 0; i < mArrInteractable.size(); i++)
		{
			GameObject * obj = EngineCore::Get<SceneSystem>()->FindGameObject(mArrInteractable[i]);
			if (obj && !(obj->GetFlags() & FLAG_REMOVE))
				CharacterHitBox_MSG::SendExternalMessage(obj, "TakeForce", _force, _dir);
			else
				mArrInteractable.FastRemove(i--);
		}
	}
	
	void CharacterHitBox::EnemyExitHitbox(const uint64_t& _id)
	{
		for (size_t i = 0; i < mArrFoundEnemies.size(); i++)
		{
			if (mArrFoundEnemies[i] == _id)
			{
				mArrFoundEnemies.FastRemove(i);
				break;
			}
		}
	}

	void CharacterHitBox::InvokeAudio(const char* _audioName)
	{
		if (mpAudioSrc)
        {
            mpAudioSrc->Stop();
            mpAudioSrc->SetVolume(1.0f);
            mpAudioSrc->SetFrequency(1.0f);
            mpAudioSrc->SetPitch(1.0f);
            mpAudioSrc->ChangeAudio(_audioName);
            mpAudioSrc->Play();
        }
	}

	void CharacterHitBox::InteractablesEnteredHitbox(const uint64_t& _id)
	{
		for (const auto& elem : mArrInteractable)
			if (elem == _id)
				return;
		mArrInteractable.Insert(_id);
	}

	void CharacterHitBox::InteractablesExitHitbox(const uint64_t& _id)
	{
		for (size_t i = 0; i < mArrInteractable.size(); i++)
		{
			if (mArrInteractable[i] == _id)
			{
				mArrInteractable.FastRemove(i);
				break;
			}
		}
	}
}




