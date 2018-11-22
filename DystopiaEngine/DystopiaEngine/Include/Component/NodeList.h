/* HEADER *********************************************************************************/
/*!
\file	ComponentList.h
\author Aaron Chu, m.chu@digipen.edu
\brief
Enum listing all existing components. Please create an enum for new components and
set the component to the corresponding enum so that the search function
can correctly find the components!

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _NODELIST_H_
#define _NODELIST_H_

#include "Utility/MetaAlgorithms.h"

namespace Dystopia
{
	using AllNode = Ut::MetaAutoIndexer <
		class Transform,
		class Camera,
		class Renderer,
		class RigidBody,
		class Collider,
		class AudioSource,
		class AiController
	>::result;
}

#endif
