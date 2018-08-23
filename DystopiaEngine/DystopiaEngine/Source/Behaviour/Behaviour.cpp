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
#include "Behaviour\Behaviour.h"		// File Header

Dystopia::Behaviour::Behaviour(void) :
	Component{}
{

}

void Dystopia::Behaviour::Load(void)
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

void Dystopia::Behaviour::OnDestroy(void)
{
}

void Dystopia::Behaviour::Unload(void)
{
}

void Dystopia::Behaviour::OnCollisionEnter(const CollisionEvent&)
{

}

void Dystopia::Behaviour::OnCollisionStay(const CollisionEvent&)
{

}

void Dystopia::Behaviour::OnCollisionExit(const CollisionEvent&)
{

}

