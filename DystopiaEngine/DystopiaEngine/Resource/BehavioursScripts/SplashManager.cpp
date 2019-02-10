/* HEADER *********************************************************************************/
/*!
\file	SplashManager.cpp
\author Keith Goh (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "SplashManager.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Graphics/GraphicsSystem.h"

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

#include "Component/SpriteRenderer.h"
#include "Component/TextRenderer.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/ShaderSystem.h"
#include "System/Graphics/Texture.h"
#include "System/File/FileSystem.h"

#include "Math/MathLib.h"

#include <utility>

namespace Dystopia
{
	namespace SplashManager_MSG
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
	SplashManager::SplashManager()
	:DigipenLogo{nullptr},
	 SplashIcon{nullptr},
	 mbBounce{false},
	 mfPauseDur{0.f}
	{
	}

	SplashManager::~SplashManager()
	{
	}
	
	void SplashManager::InitShader(void)
	{
		if(SplashIcon)
		{
			if(auto sr = SplashIcon->GetComponent<SpriteRenderer>())
			{
				if (auto shader = CORE::Get<ShaderSystem>()->GetShader("Burn"))
				{
					HashString file{CORE::Get<FileSystem>()->GetFromResource("fractal.dds").c_str()};
					if (file.length())
					{
						if(auto text = CORE::Get<GraphicsSystem>()->LoadTexture(file.c_str()))
						{
							sr->SetShader(shader);
							sr->SetManualShaderOverride("killMask", std::make_pair(text, 1));
							sr->SetManualShaderOverride("fSeam", 0.03f);
							sr->SetManualShaderOverride("fRatio", 0.f);
							sr->SetManualShaderOverride("vBurn", Math::Vec4{1.f,1.f,1.f,1.f});
						}
					}
				}
			}
		}

	}
	void SplashManager::Load()
	{

	}

	void SplashManager::Awake()
	{

	}

	void SplashManager::Init()
	{
		hasRunOnce =mbBounce= false;
		Toggle = false;
	    DigipenLogo = EngineCore::Get<SceneSystem>()->FindGameObject("DigipenLogo");
		SplashIcon= EngineCore::Get<SceneSystem>()->FindGameObject("EngineSplash");
		
		if(DigipenLogo)
			DigipenLogo->SetActive(true);
		
		if(SplashIcon)
			SplashIcon->SetActive(false);
		mfCounter = 0.f;
		SplashTime = 2.5f;
		mfPauseDur = 1.f;
	}

	void SplashManager::Update(const float _fDeltaTime)
	{
		if (Toggle)
		{
			//SplashTime -= _fDeltaTime;
			
			if(SplashIcon)
			{
			
				if (!mbBounce)
				{
					mfCounter += _fDeltaTime;
					if(auto sr = SplashIcon->GetComponent<SpriteRenderer>())
						sr->SetManualShaderOverride("fRatio", mfCounter / SplashTime * 1.06f - .03f);
					if (mfCounter >= SplashTime)
					{
						mbBounce = true;
						if(DigipenLogo)
							DigipenLogo->SetActive(false);
					}
				}
				else
				{
					mfPauseDur -= _fDeltaTime;
					if (mfPauseDur <= 0.f)
					{
						mfCounter -= _fDeltaTime;
						if(auto sr = SplashIcon->GetComponent<SpriteRenderer>())
							sr->SetManualShaderOverride("fRatio", mfCounter / SplashTime * 1.06f - .03f);
						if (mfCounter <= 0.f)
							EngineCore::Get<SceneSystem>()->LoadScene("MainMenuFinal");
					}
				}
			}
		}
		else
		{
			Time -= _fDeltaTime;
			if(Time <= 0)
			{
				Toggle= true;
				if(SplashIcon)
					SplashIcon->SetActive(true);
				InitShader();
			}
		}
	}

	void SplashManager::FixedUpdate(const float )
	{

	}
	
	void SplashManager::PostUpdate(void)
	{
		if (!hasRunOnce)
		{
			EngineCore::Get<GraphicsSystem>()->LoadAllTexture();
			hasRunOnce = true;
		}

	}

	void SplashManager::GameObjectDestroy(void)
	{
	}

	void SplashManager::Unload(void)
	{
	}

	void Dystopia::SplashManager::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::SplashManager::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::SplashManager::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::SplashManager::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::SplashManager::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::SplashManager::OnTriggerExit(GameObject * const )
	{
	}

	SplashManager * SplashManager::Duplicate() const
	{
		return new SplashManager{*this};
	}

	void SplashManager::Serialise(TextSerialiser& ) const
	{
	}

	void SplashManager::Unserialise(TextSerialiser& )
	{
	}


	const char * const SplashManager::GetBehaviourName() const
	{
		return SplashManager::BehaviourName;
	}

	void SplashManager::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData SplashManager::GetMetaData()
	{
		static MetaData<Dystopia::SplashManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const SplashManager::GetMetaData() const
	{
		static MetaData<Dystopia::SplashManager> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




