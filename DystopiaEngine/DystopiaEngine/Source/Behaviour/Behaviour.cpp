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

void Dystopia::Behaviour::OnCollisionEnter(const CollisionEvent&)
{

}

void Dystopia::Behaviour::OnCollisionStay(const CollisionEvent&)
{

}

void Dystopia::Behaviour::OnCollisionExit(const CollisionEvent&)
{

}

