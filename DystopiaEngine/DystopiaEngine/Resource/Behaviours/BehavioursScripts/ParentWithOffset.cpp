/* HEADER *********************************************************************************/
/*!
\file	ParentWithOffset.cpp
\author aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ParentWithOffset.h"
#include "Math/Vector2.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

namespace Dystopia
{
	ParentWithOffset::ParentWithOffset()
	{
	}

	ParentWithOffset::~ParentWithOffset()
	{
	}

	void ParentWithOffset::Load()
	{
	}

	void ParentWithOffset::Init()
	{
	}

	void ParentWithOffset::Update(const float _fDeltaTime)
	{
	}

	void ParentWithOffset::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void ParentWithOffset::PostUpdate(void)
	{
	}

	void ParentWithOffset::GameObjectDestroy(void)
	{
	}

	void ParentWithOffset::Unload(void)
	{
	}

	void Dystopia::ParentWithOffset::OnCollisionEnter(const CollisionEvent&)
	{

	}

	void Dystopia::ParentWithOffset::OnCollisionStay(const CollisionEvent&)
	{

	}

	void Dystopia::ParentWithOffset::OnCollisionExit(const CollisionEvent&)
	{

	}

	void Dystopia::ParentWithOffset::OnTriggerEnter(const GameObject *)
	{
		
	}

	void Dystopia::ParentWithOffset::OnTriggerStay(const GameObject *)
	{

	}

	void Dystopia::ParentWithOffset::OnTriggerExit(const GameObject *)
	{

	}

	ParentWithOffset * ParentWithOffset::Duplicate() const
	{
		return new ParentWithOffset{};
	}

	void ParentWithOffset::Serialise(TextSerialiser& _ser) const
	{
	}

	void ParentWithOffset::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const ParentWithOffset::GetBehaviourName() const
	{
		return ParentWithOffset::BehaviourName;
	}

	void ParentWithOffset::EditorUI(void) noexcept
	{
		
		
	}
    
    TypeErasure::TypeEraseMetaData ParentWithOffset::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::ParentWithOffset> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ParentWithOffset::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::ParentWithOffset> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




