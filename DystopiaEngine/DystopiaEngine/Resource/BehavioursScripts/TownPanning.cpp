/* HEADER *********************************************************************************/
/*!
\file	TownPanning.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "TownPanning.h"
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

#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/AudioSource.h"
#include "Component/SpriteRenderer.h"
#include "Component/TextRenderer.h"
#include "Math/Mathlib.h"

namespace Dystopia
{
	namespace TownPanning_MSG
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
	TownPanning::TownPanning()
		: mfHorizontalSpeed{1.f},
		mfZoomSpeed{1.f},
		mfDelayZoom{1.f},
		mbPlay{false},
		mfHorizonCap{5000},
		mfZoomOut{1.f},
		mfPeakY{0.f},
		mfLinearChangeY{0.f},
		mbBounce{false},
		mfEndHorizontal{0.f},
		mfZoomIn{0.f},
		mfFadeEndDuration{1.f},
		mbFadeAway{false},
		mfFadeAcc{0.f},
		mfFadeEndDelay{0.f},
		mpOverlay{nullptr},
		mfFadeStart{1.f},
		mbEnding{false},
		mStringText{},
		mfTextGenDelay{0.f},
		mnTextGenID{0},
		mfDelayRestart{1.f}
	{
	}

	TownPanning::~TownPanning()
	{
	}
	
	void TownPanning::Awake()
	{
	}

	void TownPanning::Init()
	{
		mStringText = "To be continued ...";
		mpOverlay = nullptr;
		mpText = nullptr;
		if (auto t = GetOwner()->GetComponent<Transform>())
		{
			mfLinearChangeY = mfPeakY - t->GetGlobalPosition().y;
			mfLinearChangeY /= (mfZoomOut / mfZoomSpeed);
			
			auto& children = t->GetAllChild();
			for (auto& child : children)
			{
				if (child->GetOwner()->GetName().id() == StringHasher("Overlay"))
					mpOverlay = child->GetOwner();
				else if (child->GetOwner()->GetName().id() == StringHasher("Text"))
					mpText = child->GetOwner();
			}
		}
		mbBounce = mbFadeAway = mbEnding = false;
		mfFadeAcc=0.f;
		mnTextGenID = 0;
	
	}

	void TownPanning::Update(const float _fDeltaTime)
	{
		if (!mbPlay) return;
		
		if (mfFadeStart > 0.f)
		{
			mfFadeAcc = Math::Min(mfFadeAcc + _fDeltaTime, mfFadeStart);
			float alpha = Math::Clamp(mfFadeAcc/mfFadeStart, 0.f, 1.f);
			
			if (mpOverlay)
				if (auto sr = mpOverlay->GetComponent<SpriteRenderer>())
					sr->SetAlpha(1 - alpha);
				
			if (alpha >= 1.f)
			{
				mfFadeStart = 0.f;
				mfFadeAcc = 0.f;
			}
			return;
		}
		
		if (mbFadeAway)
		{
			mfFadeAcc = Math::Min(mfFadeAcc + _fDeltaTime, mfFadeEndDuration);
			float alpha = Math::Clamp(mfFadeAcc/mfFadeEndDuration, 0.f, 1.f);
			if (auto as = GetOwner()->GetComponent<AudioSource>())
				as->SetVolume(1 - alpha);
			if (mpOverlay)
				if (auto sr = mpOverlay->GetComponent<SpriteRenderer>())
					sr->SetAlpha(alpha);
			
			if (alpha >= 1.f)
			{
				mbFadeAway = false;
				mbEnding = true;
				mfFadeAcc = 0.f;
			}
		}
		else if (mbEnding && mpText)
		{
			if (mnTextGenID < mStringText.length())
			{
				mfFadeAcc += _fDeltaTime;
				if (mfFadeAcc > mfTextGenDelay)
				{
					mfFadeAcc -= mfTextGenDelay;
					if (auto tr = mpText->GetComponent<TextRenderer>())
						tr->SetText(HashString{mStringText.begin(), mStringText.begin() + ++mnTextGenID}.c_str());
				}
			}
			else
			{
				mfDelayRestart -= _fDeltaTime;
				if (mfDelayRestart < 0.f)
					EngineCore::Get<SceneSystem>()->LoadScene("MainMenuFinal.dscene");
			}
		}
		else
		{
			if (!mbBounce)
			{
				MoveHorizontal(_fDeltaTime);
				ZoomOut(_fDeltaTime);
				if (mfHorizonCap < 0.f && mfZoomOut < 0.f)
					mbBounce = true;
			}
			else
			{ 
				MoveHorizontalEnd(_fDeltaTime);
				ZoomIn(_fDeltaTime);
				if (mfEndHorizontal < 0.f && mfZoomIn < 0.f)
				{
					if (mfFadeEndDelay < 0.f)
						mbFadeAway = true;
					else 
						mfFadeEndDelay -= _fDeltaTime;
				}
			}
		}
	}

	void TownPanning::GameObjectDestroy(void)
	{
		Behaviour::GameObjectDestroy();
	}

	TownPanning * TownPanning::Duplicate() const
	{
		return new TownPanning{*this};
	}

	const char * const TownPanning::GetBehaviourName() const
	{
		return TownPanning::BehaviourName;
	}
		
	void TownPanning::MoveHorizontal(float _dt)
	{
		if (auto t = GetOwner()->GetComponent<Transform>())
		{
			if (mfHorizonCap > 0.f)
			{
				auto globalPos = t->GetGlobalPosition();
				float change = mfHorizontalSpeed * _dt;
				mfHorizonCap -= change;
				globalPos.x = globalPos.x + change;
				t->SetGlobalPosition(globalPos);
			}
		}
	}
	
	void TownPanning::ZoomOut(float _dt)
	{
		if (mfDelayZoom > 0.f)
			mfDelayZoom -= _dt;
		else if (mfZoomOut > 0.f)
		{
			if (auto t = GetOwner()->GetComponent<Transform>())
			{
				auto c = GetOwner()->GetComponent<Camera>();
				float change =  mfZoomSpeed * _dt;
				mfZoomOut -= change;
				auto gScale = t->GetGlobalScale();
				gScale.x = gScale.x + change;
				gScale.y = gScale.y + change;
				t->SetGlobalScale(gScale);
				
				auto gPos = t->GetGlobalPosition();
				gPos.y = gPos.y + mfLinearChangeY*_dt;
				t->SetGlobalPosition(gPos);
			}
		}	
	}
	
	void TownPanning::MoveHorizontalEnd(float _dt)
	{
		if (auto t = GetOwner()->GetComponent<Transform>())
		{
			if (mfEndHorizontal > 0.f)
			{
				auto globalPos = t->GetGlobalPosition();
				float change = mfHorizontalSpeed * _dt;
				mfEndHorizontal -= change;
				globalPos.x = globalPos.x + change;
				t->SetGlobalPosition(globalPos);
			}
		}
	}
		
	void TownPanning::ZoomIn(float _dt)
	{
		if (mfZoomIn > 0.f)
		{
			if (auto t = GetOwner()->GetComponent<Transform>())
			{
				auto c = GetOwner()->GetComponent<Camera>();
				float change =  mfZoomSpeed * _dt;
				mfZoomIn -= change;
				auto gScale = t->GetGlobalScale();
				gScale.x = gScale.x - change;
				gScale.y = gScale.y - change;
				t->SetGlobalScale(gScale);
				
				auto gPos = t->GetGlobalPosition();
				gPos.y = gPos.y - mfLinearChangeY*_dt;
				t->SetGlobalPosition(gPos);
			}
		}
	}

	TypeErasure::TypeEraseMetaData TownPanning::GetMetaData()
	{
		static MetaData<Dystopia::TownPanning> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const TownPanning::GetMetaData() const
	{
		static MetaData<Dystopia::TownPanning> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




