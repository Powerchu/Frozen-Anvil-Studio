/* HEADER *********************************************************************************/
/*!
\file	Milestone.cpp
\author keith (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Milestone.h"
#include "Math/Vector2.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"
	
namespace Dystopia
{
	Milestone::Milestone()
	{ 
	}

	Milestone::~Milestone()
	{
	}

	void Milestone::Load()
	{
	}

	void Milestone::Init()
	{
	}

	void Milestone::Update(const float _fDeltaTime)
	{
	}

	void Milestone::FixedUpdate(const float _fDeltaTime)
	{
	}
	
	void Milestone::PostUpdate(void)
	{
	}

	void Milestone::GameObjectDestroy(void)
	{
	}

	void Milestone::Unload(void)
	{
	}

	Milestone * Milestone::Duplicate() const
	{
		return new Milestone{};
	}

	void Milestone::Serialise(TextSerialiser& _ser) const
	{
	}

	void Milestone::Unserialise(TextSerialiser& _ser)
	{
	}


	const char * const Milestone::GetBehaviourName() const
	{
		return Milestone::BehaviourName;
	}

	void Milestone::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData& Milestone::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Milestone> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const Milestone::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		MetaData<Milestone> mMetaData;
		return TypeErasure::TypeEraseMetaData{mMetaData};
	}
}




