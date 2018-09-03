#pragma once
#ifndef ComponentGUID_H
#define ComponentGUID_H
#include "GUID.h"
#include "Component/ComponentList.h"

namespace Dystopia
{
	struct ComponentGUIDGenerator : GUIDGenerator
	{
		template <class Component>
		static ulong GetUniqueID(void)
		{
			auto   BaseID = GUIDGenerator::GetUniqueID();
			return BaseID + Utility::MetaFind_t<Component, AllComponents>::value;
		}
	};
}


#endif