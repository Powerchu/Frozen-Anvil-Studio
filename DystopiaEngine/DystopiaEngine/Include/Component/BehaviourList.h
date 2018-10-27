#pragma once
#ifndef BEHAVIOUR_LIST_H
#define BEHAVIOUR_LIST_H

#include "Utility/Meta.h"
#include "Utility/MetaAlgorithms.h"

namespace Dystopia
{
	using AllBehaviours = Ut::MetaAutoIndexer <
		class Milestone,
		class ParentToHero
	>::result;
}



#endif
