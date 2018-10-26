#pragma once
#ifndef _MAP_H
#define _MAP_H
#include "Allocator/DefaultAlloc.h"
#include "DataStructure/AutoArray.h"
#include <utility>
#include "Utility/MetaDataStructures.h"

template<typename Key_t, typename Val_t, typename Comp = std::less<Key_t>, typename Alloc = Dystopia::DefaultAllocator<std::pair<Key_t, Val_t>>>
struct DumbMap
{
	using Pair_t = std::pair<Key_t, Val_t>;
	Val_t&        operator[](Key_t const & _key);
	Val_t const & operator[](Key_t const & _key) const;
private:
	AutoArray<Pair_t, Alloc> mInternalStructure;
};

template<typename Key_t, typename Val_t, typename Comp = std::less<Key_t>, typename Alloc = Dystopia::DefaultAllocator<std::pair<Key_t,Val_t>>>
struct Map
{
	Val_t&        operator[](Key_t const & _key);
	Val_t const & operator[](Key_t const & _key) const;
	/*RED BLACK TREE*/
};



#endif