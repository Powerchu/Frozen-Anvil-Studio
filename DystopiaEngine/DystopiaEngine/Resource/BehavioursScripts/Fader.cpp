/* HEADER *********************************************************************************/
/*!
\file	Fader.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Fader.h"
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
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "Math/MathLib.h"

namespace Dystopia
{
	namespace Fader_MSG
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
	Fader::Fader()
		: FadeTowards{ 0.f },
		FadeDuration{ 0.5f },
		mfCounter{0.f},
		StartFadeOut{false},
		FadeFrom{1.f},
		Bounce{true},
		StopFade{false}
	{
	}

	Fader::~Fader()
	{
	}

	void Fader::Awake()
	{
	}

	void Fader::Init()
	{
		mfCounter = 0.f;
		if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
			sr->SetAlpha(StartFadeOut ? FadeFrom : FadeTowards);
		StopFade = false;
	}

	void Fader::Update(const float _fDeltaTime)
	{
		if (StopFade)
			return;
		
		mfCounter += _fDeltaTime;
		//while (mfCounter >= FadeDuration)
		//	mfCounter -= FadeDuration;
		
		float timeAlpha = Math::Clamp(mfCounter / FadeDuration, 0.f, 1.f);
		if (StartFadeOut)
		{
			if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
				sr->SetAlpha(Math::Lerp(FadeFrom, FadeTowards, timeAlpha));
		}
		else
		{ 
			if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
				sr->SetAlpha(Math::Lerp(FadeFrom, FadeTowards, 1 - timeAlpha));
		}
		
		if (mfCounter >= FadeDuration)
		{
			mfCounter = 0.f;
			StopFade = Bounce ? false : true;
			StartFadeOut = !StartFadeOut;
		}
	}

	void Fader::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	Fader * Fader::Duplicate() const
	{
		return new Fader{*this};
	}

	const char * const Fader::GetBehaviourName() const
	{
		return Fader::BehaviourName;
	}
	
	TypeErasure::TypeEraseMetaData Fader::GetMetaData()
	{
		static MetaData<Dystopia::Fader> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Fader::GetMetaData() const
	{
		static MetaData<Dystopia::Fader> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	
}




