/* HEADER *********************************************************************************/
/*!
\file	IntroductionScene.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "IntroductionScene.h"
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
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/File/FileSystem.h"

#include <utility>

namespace Dystopia
{
	namespace IntroductionScene_MSG
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
	IntroductionScene::IntroductionScene()
		: mfStartDelay{1.f},
		mfTransitionSpeed{1.f},
		mfFrameDuration{1.f},
		mfCounter{0.f},
		mImages{},
		mnStage{0},
		mbFadeOut{false},
		mbFadeIn{false},
		mpRing{nullptr},
		mfSkipThresh{1.f},
		mfSkipHeld{0.f},
		mbSkip{false},
		mpSkipText{nullptr},
		mfSkipFade{0.f},
		mfSkipFadeCounter{0.f},
		mbBlink{false}
	{
	}

	IntroductionScene::~IntroductionScene()
	{
	}

	void IntroductionScene::Awake()
	{
		mbFadeOut = mbFadeIn = mbSkip = mbBlink = false;
		mfCounter = mfSkipHeld = mfSkipFadeCounter = 0.f;
		mpRing = nullptr;
		mpSkipText = nullptr;
	}

	void IntroductionScene::Init()
	{
		mpSkipText = CORE::Get<SceneSystem>()->FindGameObject("HoldSkip");
		mpRing = CORE::Get<SceneSystem>()->FindGameObject("HoldSkipRing");
		if (mpRing)
		{
			if (auto sr = mpRing->GetComponent<SpriteRenderer>())
			{
				if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Loading"))
				{
					sr->SetShader(shader);
					sr->SetManualShaderOverride("fRatio", 1.f);
				}
			}
		}
		
		mbFadeOut = mbFadeIn = mbSkip = mbBlink = false;
		mfCounter = mfSkipHeld = mfSkipFadeCounter = 0.f;
		mImages[0] = "intro_01.dds";
		mImages[1] = "intro_02.dds";
		mImages[2] = "intro_03.dds";
		//mImages[3] = "intro_03_glow.dds";
		mImages[3] = "intro_04.dds";
		mImages[4] = "intro_05.dds";
		mImages[5] = "intro_06.dds";
		if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
			sr->SetAlpha(0.f);
	}

	void IntroductionScene::Update(const float _fDeltaTime)
	{
		if (mpRing && mpSkipText && !mbBlink)
		{
			if (!mbSkip && mnStage < 6 && Skip(_fDeltaTime))
			{
				mbSkip = mbBlink = true;
				mnStage = 7;
				mfCounter = 0.f;
				mfStartDelay = 0.f;
			}
			FadeOutSkip(_fDeltaTime);
		}
		
		if (mfStartDelay > 0.f)
			mfStartDelay -= _fDeltaTime;
		else 
		{
			switch (mnStage)
			{
			case 0: 
				if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
					sr->SetAlpha(1.f);
				ChangeImage();
				mnStage = 1;
				break;
			case 1:
				mfCounter += _fDeltaTime;
				if (mfCounter > mfFrameDuration)
				{
					ChangeImage();
					mnStage = 2;
					mfCounter = 0.f;
				}
				break;
			case 2:
				FadeSequence(_fDeltaTime);
				break;
			case 3:
				FadeSequence(_fDeltaTime);
				break;
			case 4:
				FadeSequence(_fDeltaTime);
				break;
			case 5:
				mfCounter += _fDeltaTime;
				if (mfCounter > mfFrameDuration)
				{
					ChangeImage();
					mnStage++;
					mfCounter = 0.f;
				}
				break;
			case 6:
				mfCounter += _fDeltaTime;
				if (mfCounter > mfFrameDuration)
				{
					FadeOut(mfCounter - mfFrameDuration);
					if (auto sr = mpRing->GetComponent<SpriteRenderer>())
						sr->SetAlpha(sr->GetTint().w - ((mfCounter - mfFrameDuration) / (mfTransitionSpeed * 0.5f)));
					if (auto sr = mpSkipText->GetComponent<SpriteRenderer>())
						sr->SetAlpha(sr->GetTint().w - ((mfCounter - mfFrameDuration) / (mfTransitionSpeed * 0.5f)));
					if ((mfCounter- mfFrameDuration) > (mfTransitionSpeed * 0.5f))
					{
						// change scene
						EngineCore::Get<SceneSystem>()->LoadScene("ForestFinal.dscene");
					}
				}
				break;
			case 7:	
				if (auto sr = mpRing->GetComponent<SpriteRenderer>())
				{
					if (mbBlink)
					{
						sr->SetColor({0.f, 1.f, 0.f});
						mnStage = 6;
					}
				}
				break;
			}
		}
		
	}

	void IntroductionScene::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	IntroductionScene * IntroductionScene::Duplicate() const
	{
		return new IntroductionScene{*this};
	}

	const char * const IntroductionScene::GetBehaviourName() const
	{
		return IntroductionScene::BehaviourName;
	}
	 
	TypeErasure::TypeEraseMetaData IntroductionScene::GetMetaData()
	{
		static MetaData<Dystopia::IntroductionScene> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	TypeErasure::TypeEraseMetaData const IntroductionScene::GetMetaData() const
	{
		static MetaData<Dystopia::IntroductionScene> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	
	void IntroductionScene::FadeOutSkip(float)
	{
		if (auto sr = mpSkipText->GetComponent<SpriteRenderer>())
			sr->SetAlpha(1 - Math::Clamp(mfSkipFadeCounter/mfSkipThresh, 0.f, 1.f));
	}
	
	void IntroductionScene::ChangeImage(void)
	{
		HashString file{CORE::Get<FileSystem>()->GetFromResource(mImages[mnStage].c_str()).c_str()};
		if (file.length())
			if (auto tex = CORE::Get<GraphicsSystem>()->LoadTexture(file.c_str()))
				if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
					sr->SetTexture(tex);
	}

	bool IntroductionScene::Skip(float _dt)
	{
		if (auto input = CORE::Get<InputManager>())
		{
			if (input->GetButton("Jump"))
			{
				mfSkipHeld = Math::Min(mfSkipHeld + _dt, mfSkipThresh);
				mfSkipFadeCounter = 0.f;
			}
			else
			{
				mfSkipHeld = Math::Max(mfSkipHeld - _dt, 0.f);
				mfSkipFadeCounter = Math::Min(mfSkipFadeCounter + _dt, mfSkipFade);
			}
		}
		
		if (auto sr = mpRing->GetComponent<SpriteRenderer>())
			sr->SetManualShaderOverride("fRatio", 1.f - Math::Clamp(mfSkipHeld / mfSkipThresh, 0.f, 1.f));
		
		return mfSkipHeld >= mfSkipThresh;
	}
	
	void IntroductionScene::FadeOut(float _accDt)
	{
		if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
			sr->SetAlpha(1 - (_accDt / (mfTransitionSpeed * 0.5f)));
	}
	
	void IntroductionScene::FadeIn(float _accDt)
	{
		if (auto sr = GetOwner()->GetComponent<SpriteRenderer>())
			sr->SetAlpha(_accDt / (mfTransitionSpeed * 0.5f));
	}
	
	void IntroductionScene::FadeSequence(float _dt)
	{
		mfCounter += _dt;
		if (!mbFadeOut && !mbFadeIn && (mfCounter > mfFrameDuration))
		{
			mbFadeOut = true;
			mfCounter = 0.f;
		}
		else if (mbFadeOut && !mbFadeIn)
		{
			FadeOut(mfCounter);
			if (mfCounter > (mfTransitionSpeed * 0.5f))
			{
				ChangeImage(); 
				mbFadeOut = false;
				mbFadeIn = true;
				mfCounter = 0.f;
			}
		}
		else if (!mbFadeOut && mbFadeIn)
		{
			FadeIn(mfCounter);
			if (mfCounter > (mfTransitionSpeed * 0.5f))
			{
				mbFadeOut = false;
				mbFadeIn = false;
				mfCounter = 0.f;
				mnStage++;
			}
		}
	}

}





