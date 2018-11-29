/* HEADER *********************************************************************************/
/*!
\file	Behaviour.cpp 
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Base class for all Behaviours

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Behaviour/Behaviour.h"		// File Header

Dystopia::Behaviour::Behaviour(void) :
	Component{}
{

}

void Dystopia::Behaviour::Load(void)
{
}

void Dystopia::Behaviour::Awake(void)
{
}

void Dystopia::Behaviour::Init(void)
{
}

void Dystopia::Behaviour::Update(const float)
{

}

void Dystopia::Behaviour::FixedUpdate(const float)
{

}

void Dystopia::Behaviour::PostUpdate(void)
{

}

void Dystopia::Behaviour::GameObjectDestroy(void)
{
	Component::GameObjectDestroy();
}

void Dystopia::Behaviour::Unload(void)
{
}

void Dystopia::Behaviour::OnCollisionEnter(const CollisionEvent&)
{
	__debugbreak();
}

void Dystopia::Behaviour::OnCollisionStay(const CollisionEvent&)
{

}

void Dystopia::Behaviour::OnCollisionExit(const CollisionEvent&)
{

}

void Dystopia::Behaviour::OnTriggerEnter(GameObject * const)
{
	
}

void Dystopia::Behaviour::OnTriggerStay(GameObject * const)
{
}

void Dystopia::Behaviour::OnTriggerExit(GameObject * const)
{
}

Dystopia::TypeErasure::TypeEraseMetaData Dystopia::Behaviour::GetMetaData()
{
	return TypeErasure::TypeEraseMetaData{};
}

Dystopia::TypeErasure::TypeEraseMetaData const Dystopia::Behaviour::GetMetaData() const
{
	return TypeErasure::TypeEraseMetaData{};
}

void Dystopia::Behaviour::ReceiveMessage(const char * const , BehaviourMessage)
{
}

Dystopia::Behaviour * Dystopia::Behaviour::Duplicate() const
{
	return nullptr;
}

void Dystopia::Behaviour::BehaviourDestroy()
{
	Component::GameObjectDestroy();
	GameObjectDestroy();
}

