/* HEADER *********************************************************************************/
/*!
\file	Parallax.cpp
\author keith (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Parallax.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "System/Collision/CollisionEvent.h"
#include "Math/MathLib.h"
#include "Component/Transform.h"
namespace Dystopia
{
	Parallax::Parallax()
	{
	}

	Parallax::~Parallax()
	{
	}

	void Parallax::Load()
	{
	}

	void Parallax::Init()
	{
		mFocalTarget = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject_cstr("Scene Camera");
	}

	void Parallax::Update(const float _fDeltaTime)
	{
		if(mFocalTarget)
		{
			mTargetNewPos = mFocalTarget->GetComponent<Transform>()->GetGlobalPosition();
			auto Vector  = mTargetNewPos - mTargetOldPos;
			auto * OwnerTransform = GetOwner()->GetComponent<Transform>();
			Math::Point3D newPos = OwnerTransform->GetGlobalPosition();
			if(Vector.x > 0)
			{
				auto diff = Vector.x + mLayerDiff;
				if(diff > 0)
					newPos.x =  newPos.x + -(Vector.x + mLayerDiff);
			}
			else if(Vector.x < 0)
			{
				auto diff = Vector.x - mLayerDiff;
				if(diff < 0)
					newPos.x =  newPos.x + -(Vector.x + mLayerDiff);
			}
			OwnerTransform->SetGlobalPosition(newPos);
			mTargetOldPos = mTargetNewPos;
		}

	}

	void Parallax::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Parallax::PostUpdate(void)
	{
		if(mFocalTarget)
		{
			
		}

	}

	void Parallax::GameObjectDestroy(void)
	{
	}

	void Parallax::Unload(void)
	{
		
	}

	void Dystopia::Parallax::OnCollisionEnter(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Parallax::OnCollisionStay(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Parallax::OnCollisionExit(const CollisionEvent& _colEvent)
	{

	}

	void Dystopia::Parallax::OnTriggerEnter(const GameObject * _obj)
	{
	}

	void Dystopia::Parallax::OnTriggerStay(const GameObject * _obj)
	{
	}

	void Dystopia::Parallax::OnTriggerExit(const GameObject * _obj)
	{
	}

	Parallax * Parallax::Duplicate() const
	{
		return new Parallax{};
	}

	void Parallax::Serialise(TextSerialiser& _ser) const
	{
	}

	void Parallax::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Parallax::GetBehaviourName() const
	{
		return Parallax::BehaviourName;
	}

	void Parallax::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData Parallax::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::Parallax> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Parallax::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::Parallax> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




