/* HEADER *********************************************************************************/
/*!
\file	DefaultAlloc.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Default allocator with a limited pool size.
	Exceeding the pool size from the default allocator will forcefully crash the program.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Allocator\DefaultAlloc.h"
#include "Allocator\DefaultAllocImpl.h"

#include "Utility\Meta.h"
#include "Utility\Utility.h"
#include "Math\MathUtility.h"
#include "Utility\DebugAssert.h"

#include <cstdlib>
#include <cstddef>
#include <exception>


namespace
{
	static constexpr size_t MEBIBYTE = 1048576;
	static constexpr size_t DEFAULT_HEAP = 512 * MEBIBYTE;
	static Dystopia::DefaultAlloc mAllocator{};

	template <typename T>
	inline auto Align(T _in, T _align) -> Utility::EnableIf_t<Utility::IsIntegral<T>::value, T>
	{
		return _in + (_align-1) & ~T(_align-1);
	}
}

[[nodiscard]] void* Dystopia::DefaultAllocator<void>::Alloc(size_t _sz, size_t _align)
{
	return mAllocator.Allocate(_sz, _align);
}

void Dystopia::DefaultAllocator<void>::Free(void* _ptr) noexcept
{
	mAllocator.Deallocate(_ptr);
}

Dystopia::DefaultAlloc::DefaultAlloc(void) :
	mpBlock{ std::malloc(DEFAULT_HEAP) }, 
	mpFree{ reinterpret_cast<void*>(Align<uintptr_t>(reinterpret_cast<uintptr_t>(mpBlock), 8)) }
{
	reinterpret_cast<MetaData_t*>(mpFree)[0] = 0;
	reinterpret_cast<MetaData_t*>(mpFree)[1] = DEFAULT_HEAP;
}

Dystopia::DefaultAlloc::~DefaultAlloc(void) noexcept
{
	std::free(mpBlock);
}

void* Dystopia::DefaultAlloc::Allocate(size_t _sz, size_t _align)
{
	void *pSeek = mpFree, *pPrev = nullptr;

	// Force size and alignment to be at least the minimum
	_sz = Math::Max(_sz, MIN_SIZE);
	_align = Math::Max(_align, Utility::Constant<decltype(_align),8>::value);

	while (pSeek)
	{
		MetaData_t blkSz = GetBlockSize(pSeek);
		void* pRet = reinterpret_cast<void*>(Align<uintptr_t>(reinterpret_cast<uintptr_t>(pSeek), _align));
		MetaData_t adjSz = static_cast<MetaData_t>(_sz + (pRet != pSeek) * _align);
		
		if (blkSz >= adjSz)
		{
			blkSz = blkSz - adjSz;
			std::byte* temp;

			if (blkSz)
			{
				temp = static_cast<std::byte*>(pRet) + adjSz;
				reinterpret_cast<MetaData_t*>(temp)[0] = GetNextOffset(pSeek);
				reinterpret_cast<MetaData_t*>(temp)[1] = blkSz;
			}
			else
			{
				temp = static_cast<std::byte*>(GetBlockFromOffset(GetNextOffset(pSeek)));
			}

			if (pPrev)
			{
				*static_cast<MetaData_t*>(pPrev) = temp ? static_cast<MetaData_t>(temp - static_cast<decltype(temp)>(mpBlock)) : 0;
			}

			mpFree = pSeek == mpFree ? temp : mpFree;

			return pRet;
		}
		else
		{
			pPrev = pSeek;
			pSeek = GetBlockFromOffset(GetNextOffset(pSeek));
		}
	}

	// Crash the program with exception
	// So we produce a stack trace for debug purposes
	throw std::bad_alloc{};
}

void Dystopia::DefaultAlloc::Deallocate(void*)
{
	// TODO

#if __INTELLISENSE__
#error Dystopia::DefaultAlloc::Deallocate(void*) "not implmented!"
#endif
}

typename Dystopia::DefaultAlloc::MetaData_t Dystopia::DefaultAlloc::GetBlockSize(void* _p)
{
	return *(static_cast<MetaData_t*>(_p) + 1);
}

typename Dystopia::DefaultAlloc::MetaData_t Dystopia::DefaultAlloc::GetNextOffset(void* _p)
{
	return *static_cast<MetaData_t*>(_p);
}

void* Dystopia::DefaultAlloc::GetBlockFromOffset(MetaData_t _nOffset)
{
	if (_nOffset)
		return static_cast<std::byte*>(mpBlock) + _nOffset;

	return nullptr;
}


