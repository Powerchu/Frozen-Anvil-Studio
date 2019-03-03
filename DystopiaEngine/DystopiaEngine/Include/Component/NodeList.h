/* HEADER *********************************************************************************/
/*!
\file	ComponentList.h
\author Aaron Chu, m.chu@digipen.edu
\brief
Enum listing all existing components. Please create an enum for new components and
set the component to the corresponding enum so that the search function
can correctly find the components!

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _NODELIST_H_
#define _NODELIST_H_

#include "Utility/MetaAlgorithms.h"
#include "System/AI/AISystem.h"

namespace Dystopia
{
	using AllNode = Ut::MetaAutoIndexer <
		class Selector,
		class Sequence,
		class StatefulSelector,
		class StatefulSequence,
		class RandomSelector,
		class RandomSequence,
		class Succeeder,
		class Failer,
		class Inverter,
		class Repeater,
		class UntilSuccess,
		class UntilFailure,
		class HardLimiter,
		class Limiter,
		class TimeHolder,
		class Succeeder
	>::result;
}

#endif

