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
		mTargetNewPos = mFocalTarget->GetComponent<Transform>()->GetGlobalPosition();
		mTargetOldPos = mTargetNewPos;
	}

	void Parallax::Update(const float )
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
					newPos.x =  newPos.x + -(Vector.x + mLayerDiff) * mXSpeed;
			}
			else if(Vector.x < 0)
			{
				auto diff = Vector.x - mLayerDiff;
				if(diff < 0)
					newPos.x =  newPos.x + -(Vector.x - mLayerDiff) * mXSpeed;
			}
			
			if(Vector.y > 0)
			{
				auto diff = Vector.y + mLayerDiff;
				if(diff > 0)
					newPos.y =  newPos.y + -(Vector.y + mLayerDiff) * mYSpeed;
			}
			else if(Vector.y < 0)
			{
				auto diff = Vector.y - mLayerDiff;
				if(diff < 0) 
					newPos.y =  newPos.y + -(Vector.y - mLayerDiff) * mYSpeed;
			}
			OwnerTransform->SetGlobalPosition(newPos);
			mTargetOldPos = mTargetNewPos;
		}

	}

	void Parallax::FixedUpdate(const float )
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

	void Dystopia::Parallax::OnCollisionEnter(const CollisionEvent& )
	{

	}

	void Dystopia::Parallax::OnCollisionStay(const CollisionEvent& )
	{

	}

	void Dystopia::Parallax::OnCollisionExit(const CollisionEvent& )
	{

	}

	void Dystopia::Parallax::OnTriggerEnter(GameObject * const )
	{
	}

	void Dystopia::Parallax::OnTriggerStay(GameObject * const )
	{
	}

	void Dystopia::Parallax::OnTriggerExit(GameObject * const )
	{
	}

	Parallax * Parallax::Duplicate() const
	{
		return new Parallax{*this};
	}

	void Parallax::Serialise(TextSerialiser& ) const
	{
	}

	void Parallax::Unserialise(TextSerialiser& )
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




