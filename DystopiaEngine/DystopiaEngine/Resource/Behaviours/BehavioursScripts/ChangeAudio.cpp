/* HEADER *********************************************************************************/
/*!
\file	ChangeAudio.cpp
\author mchu (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "ChangeAudio.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

#include "Component/AudioSource.h"
#include "Object/GameObject.h"


namespace Dystopia
{
	ChangeAudio::ChangeAudio()
	{
	}

	ChangeAudio::~ChangeAudio()
	{
	}

	void ChangeAudio::Load()
	{
	}

	void ChangeAudio::Init()
	{
	}

	void ChangeAudio::Update(const float )
	{
	}

	void ChangeAudio::FixedUpdate(const float )
	{
	}
	
	void ChangeAudio::PostUpdate(void)
	{
	}

	void ChangeAudio::GameObjectDestroy(void)
	{
	}

	void ChangeAudio::Unload(void)
	{
	}

	void Dystopia::ChangeAudio::OnCollisionEnter(const CollisionEvent & )
	{

	}

	void Dystopia::ChangeAudio::OnCollisionStay(const CollisionEvent & )
	{

	}

	void Dystopia::ChangeAudio::OnCollisionExit(const CollisionEvent & )
	{

	}

	void Dystopia::ChangeAudio::OnTriggerEnter(GameObject * const _obj)
	{
		if (_obj->GetNamePtr() == nullptr)
			return;
		else
		{
			const char * name = _obj->GetNamePtr();
			if(strcmp(name,"Player"))
			{
				return;
			}
		}
		
		auto audioSrc = GetOwner()->GetComponent<AudioSource>();
		if (audioSrc)
		{  
			audioSrc->ChangeAudio("Attack On Titan OP 1.mp3");
			audioSrc->Play();
		} 
	}

	void Dystopia::ChangeAudio::OnTriggerStay(GameObject * const )
	{ 
	}

	void Dystopia::ChangeAudio::OnTriggerExit(GameObject * const _obj)
	{
		if (_obj->GetNamePtr() == nullptr)
			return;
		else
		{
			const char * name = _obj->GetNamePtr();
			if(strcmp(name,"Player"))
			{
				return;
			}
		}  
	
		auto audioSrc = GetOwner()->GetComponent<AudioSource>();
		if (audioSrc)
		{
			audioSrc->ChangeAudio("wilomenu.wav"); 
			audioSrc->Play();
		}
	}

	ChangeAudio * ChangeAudio::Duplicate() const
	{
		return new ChangeAudio{*this};
	}

	void ChangeAudio::Serialise(TextSerialiser& ) const
	{
	}

	void ChangeAudio::Unserialise(TextSerialiser& )
	{
	}


	const char * const ChangeAudio::GetBehaviourName() const
	{
		return ChangeAudio::BehaviourName;
	}

	void ChangeAudio::EditorUI(void) noexcept
	{
		
		
	}
	
	TypeErasure::TypeEraseMetaData ChangeAudio::GetMetaData()
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::ChangeAudio> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
	TypeErasure::TypeEraseMetaData const ChangeAudio::GetMetaData() const
	{
		/*TO DO*/
		/*REMEMBER TO RETURN YOUR REFLECTED DATA HERE*/
		static MetaData<Dystopia::ChangeAudio> mMetaData;
		static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
		return mReturn;
	}
}




