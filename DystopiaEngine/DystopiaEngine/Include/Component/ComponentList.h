/* HEADER *********************************************************************************/
/*!
\file	ComponentList.h
\author Floating Point
\brief
	Enum listing all existing components. Please create an enum for new components and 
	set the component to the corresponding enum so that the search function
	can correctly find the components! 

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _COMPONENTLIST_H_
#define _COMPONENTLIST_H_

#include "Utility\MetaAlgorithms.h"

namespace Dystopia
{
	using AllComponents = Utility::MetaAutoIndexer <
		class Transform,
		class Camera,
		class Collider,
		class RigidBody,
		class Renderer,
		class Particle
	>::result;

	// Generic Tag for Components
	struct ComponentTag {};

	// Tag for Components that are behaviours
	struct BehaviourTag : ComponentTag {};

	//using SysRef_t  = unsigned;
	//using CompRef_t = unsigned;
	//using GUID_t    = unsigned long long;

	//struct ComponentID
	//{
	//	GUID_t mID;
	//	CompRef_t mComponent;
	//	SysRef_t  mSystem;
	//};
}



#endif

