/* HEADER *********************************************************************************/
/*!
\file	ProxyAlloc.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Allocator that simply sends allocation requests to a different allocator
	Helps in tracking allocations and debugging

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PROXYALLOC_H_
#define _PROXYALLOC_H_

#include "Utility/Utility.h"
#include "Utility/DebugAssert.h"

#include "System/Logger/FileLogger.h"

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

		void WriteFreeMemoryImpl(FileLogger&) const;
		void WriteActiveAllocations(FileLogger&) const;

		Alloc_t const& Get(void) const;


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
		FileLogger out = LoggerSystem::FileLog("Memleak.dystor");
		if (out.Good())
		{
			out.Write("Proxy Allocator: Detected Memory leaks!\n");
			WriteActiveAllocations(out);
		}
	}
}

template <typename A>
[[nodiscard]] void* Dystopia::ProxyAlloc<A>::Allocate(size_t _sz, size_t _align)
{
	if (void* ptr = mAlloc.Allocate(_sz, _align))
	{
		_sz = Math::Max(_sz, A::MIN_SIZE);

		auto left = reinterpret_cast<uintptr_t>(ptr) - sizeof(A::METADATA_SZ);
		auto right = reinterpret_cast<uintptr_t>(ptr) + _sz;

		for (auto& e : mData)
		{
			auto data_left = reinterpret_cast<uintptr_t>(e.mPointer) - A::METADATA_SZ;
			auto data_right = reinterpret_cast<uintptr_t>(e.mPointer) + e.mReqSz;

			if (right < data_left) continue;
			if (left > data_right) continue;

			DEBUG_BREAK(true, "Alloc Error: Invalid Allocation!");
		}

		DEBUG_BREAK((_align - 1) & reinterpret_cast<uintptr_t>(ptr), "Alloc Error: Pointer is misaligned!");

		mData.push_back(AllocInfo{ ptr, _sz, _align });
		return ptr;
	}

	return nullptr;
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

template<typename Alloc_t>
inline void Dystopia::ProxyAlloc<Alloc_t>::WriteFreeMemoryImpl(FileLogger& _out) const
{
	mAlloc.WriteFreeMemoryImpl(_out);
}

template<typename Alloc_t>
inline void Dystopia::ProxyAlloc<Alloc_t>::WriteActiveAllocations(FileLogger& _out) const
{
	for (auto& e : mData)
	{
		_out.Write("Allocation at %p: %zu bytes with %zu alignment.\n", e.mPointer, e.mReqSz, e.mAlignment);
	}
}

template<typename A>
inline A const& Dystopia::ProxyAlloc<A>::Get(void) const
{
	return mAlloc;
}



#endif

