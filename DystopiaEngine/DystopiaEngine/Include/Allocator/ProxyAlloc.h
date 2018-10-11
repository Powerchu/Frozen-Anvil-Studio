/* HEADER *********************************************************************************/
/*!
\file	ProxyAlloc.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Allocator that simply sends allocation requests to a different allocator
	Helps in tracking allocations and debugging

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PROXYALLOC_H_
#define _PROXYALLOC_H_

#include "Utility/Utility.h"
#include "Utility/DebugAssert.h"

#include <list>
#include <cstdio>
#include <algorithm>


namespace Dystopia
{
	template <typename Alloc_t>
	class ProxyAlloc
	{
	public:

		template <typename...Ps>
		explicit ProxyAlloc(Ps&&...);
		~ProxyAlloc(void) noexcept;

		[[nodiscard]] void* Allocate(size_t, size_t);
		void Deallocate(void*);


	private:

		Alloc_t mAlloc;

		struct AllocInfo
		{
			void* mPointer;
			size_t mReqSz;
			size_t mAlignment;
		};

		std::list<AllocInfo> mData;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename A> template <typename ... Ps>
inline Dystopia::ProxyAlloc<A>::ProxyAlloc(Ps&& ... _args) :
	mAlloc{ Ut::Forward<Ps>(_args)... }
{

}

template<typename Alloc_t>
inline Dystopia::ProxyAlloc<Alloc_t>::~ProxyAlloc(void) noexcept
{
	if (mData.size())
	{
		DEBUG_BREAK(true, "Alloc Warning: Detected Memory Leaks!");
	}
}

template <typename A>
[[nodiscard]] void* Dystopia::ProxyAlloc<A>::Allocate(size_t _sz, size_t _align)
{
	void* ptr = mAlloc(_sz, _align);
	mData.emplace(ptr, _sz, _align);

	// Do a check for invalid allocation?

	return ptr;
}

template <typename A>
void Dystopia::ProxyAlloc<A>::Deallocate(void* _ptr)
{
	if (!_ptr) return;

	auto it = std::find_if(mData.begin(), mData.end(), [&](AllocInfo& _obj) {
		return _obj.mPointer == _ptr;
	});

	if (it == mData.end())
	{
		// Do something
		DEBUG_BREAK(true, "Alloc Error: Attempted dellocation of non-allocated pointer!");
	}
	else
	{
		mData.erase(it);
	}

	mAlloc.Deallocate(_ptr);
}



#endif

