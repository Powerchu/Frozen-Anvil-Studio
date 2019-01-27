/* HEADER *********************************************************************************/
/*!
\file	ComponentList.h
\author Jacky Tan
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

#include "Utility/MetaAlgorithms.h"

namespace Dystopia
{
	using AllComponents = Ut::MetaAutoIndexer <
		class Transform,
		class Camera,
		class Renderer,
		class RigidBody,
		class Collider,
		class AudioSource,
		class AiController,
		class AudioListener
	>::result;

	using UsableComponents = Ut::MetaAutoIndexer <
		class Camera,
		class Renderer,
		class RigidBody,
		class AABB,
		class Convex,
		class Circle,
		class PointCollider,
		class AudioSource,
		class SpriteRenderer,
		class TextRenderer,
		class AiController,
		class RayCollider,
		class AudioListener
		//class Particle
	>::result;

	// Generic Tag for Components
	struct ComponentTag {};

	// Tag for Components that are behaviours
	struct BehaviourTag : ComponentTag {};

	//using GUID_t    = uint64_t;
}



#endif

