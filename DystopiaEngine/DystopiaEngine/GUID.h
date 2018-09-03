#pragma once
#ifndef GUID_H
#define GUID_H
#include "Component/ComponentList.h"

namespace Dystopia
{
	/*
	First 45 Bits are just ID Numbers
	Next  19 Bits are used for status ( Not comfirmed how we want to use this yet)
	*/
	using ulong = unsigned long;

	struct GUIDGenerator
	{
		static ulong GetUniqueID(void);
	};
}


#endif