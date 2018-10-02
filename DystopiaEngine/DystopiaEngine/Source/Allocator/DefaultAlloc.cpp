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

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <exception>

#pragma warning(push)
#pragma warning(disable : 4996)

namespace
{
	static constexpr size_t MEBIBYTE = 1048576;
	static constexpr size_t DEFAULT_HEAP = 512 * MEBIBYTE;
	static Dystopia::DefaultAlloc mAllocator{};

#if defined(DEBUGALLOC)
	static auto output = std::fopen("allocator_log.dystor", "w");
#endif

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
	mpBlock{ reinterpret_cast<std::byte*>(std::malloc(DEFAULT_HEAP)) }, 
	mpFree{ reinterpret_cast<std::byte*>(Align<uintptr_t>(reinterpret_cast<uintptr_t>(mpBlock), 8)) }
{
	reinterpret_cast<MetaData_t*>(mpFree)[0] = 0;
	reinterpret_cast<MetaData_t*>(mpFree)[1] = DEFAULT_HEAP;
}

Dystopia::DefaultAlloc::~DefaultAlloc(void) noexcept
{
	std::free(mpBlock);

#if defined(DEBUGALLOC)
	std::fclose(output);
#endif
}

void* Dystopia::DefaultAlloc::Allocate(size_t _sz, size_t _align)
{
	void *pSeek = mpFree, *pPrev = nullptr;

	// Force size and alignment to be at least the minimum
	_sz = Math::Max(_sz, MIN_SIZE);
	_sz = Align(_sz, Utility::Constant<decltype(_sz), MIN_ALIGN>::value);
	_align = Math::Max(_align, Utility::Constant<decltype(_align), MIN_ALIGN>::value);

	while (pSeek)
	{
		MetaData_t blkSz = GetBlockSize(pSeek);
		void* pRet = reinterpret_cast<void*>(Align<uintptr_t>(reinterpret_cast<uintptr_t>(pSeek) + sizeof(MetaData_t), _align));
		_align = static_cast<std::byte*>(pRet) - static_cast<std::byte*>(pSeek);
		MetaData_t adjSz = static_cast<MetaData_t>(_sz + _align);
		
		if (blkSz >= adjSz)
		{
			blkSz = blkSz - adjSz;
			std::byte* temp;

			if (blkSz > Utility::Constant<size_t, sizeof(MetaData_t) * 2 - 1>::value)
			{
				temp = static_cast<std::byte*>(pRet) + adjSz;
				reinterpret_cast<MetaData_t*>(temp)[0] = GetNextOffset(pSeek);
				reinterpret_cast<MetaData_t*>(temp)[1] = blkSz;
			}
			else
			{
				adjSz += blkSz;
				temp = static_cast<std::byte*>(GetBlockFromOffset(GetNextOffset(pSeek)));
			}

			if (pPrev)
			{
				*static_cast<MetaData_t*>(pPrev) = temp ? static_cast<MetaData_t>(temp - static_cast<decltype(temp)>(mpBlock)) : 0;
			}

			mpFree = pSeek == mpFree ? temp : mpFree;
			reinterpret_cast<MetaData_t*>(pRet)[-1] = ((adjSz & ~0x3) << 6) + static_cast<unsigned char>(_align - 1);

#        if defined(DEBUGALLOC)
			printf("Alloc   [Actual: %p, Adjusted: %p, Size: %u, Adjust: %zu, Block: %u]\n", pSeek, pRet, adjSz, _align, blkSz);
			fprintf(output, "Alloc   [Actual: %p, Adjusted: %p, Size: %u, Adjust: %zu, Block: %u]\n", pSeek, pRet, adjSz, _align, blkSz);
			fflush(output);
#        endif

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

void Dystopia::DefaultAlloc::Deallocate(void* _ptr)
{
	if (nullptr == _ptr) return;
	
	MetaData_t sz     = static_cast<MetaData_t*>(_ptr)[-1];
	MetaData_t offset = (sz & 0xFF) + 1;
	sz >>= 6;

#if defined (DEBUGALLOC)
	auto adjust = offset;
#endif

	std::byte* actual = static_cast<std::byte*>(_ptr) - offset;
	offset = static_cast<MetaData_t>(actual - mpBlock);

	std::byte* pSeek = mpFree, *pPrev = nullptr;

	while (pSeek)
	{
		if (offset < pSeek - mpBlock)
			break;

		pPrev = pSeek;
		pSeek = static_cast<std::byte*>(GetBlockFromOffset(GetNextOffset(pSeek)));
	}

	if (pSeek == (actual + sz))
	{
		sz += GetBlockSize(pSeek);
		pSeek = static_cast<std::byte*>(GetBlockFromOffset(GetNextOffset(pSeek)));
	}

#if defined(DEBUGALLOC)
	printf("Dealloc [Actual: %p, Adjusted: %p, Size: %u, Adjust: %u]\n", actual, _ptr, sz, adjust);
	fprintf(output, "Dealloc [Actual: %p, Adjusted: %p, Size: %u, Adjust: %u]\n", actual, _ptr, sz, adjust);
	fflush(output);
#endif

	if (pPrev)
	{
		if (pPrev + GetBlockSize(pPrev) != actual)
		{
			reinterpret_cast<MetaData_t*>(actual)[0] = pSeek ? static_cast<MetaData_t>(pSeek - mpBlock) : 0;
			reinterpret_cast<MetaData_t*>(actual)[1] = sz;
			reinterpret_cast<MetaData_t*>(pPrev)[0] = offset;
		}
		else
		{
			auto old_sz = reinterpret_cast<MetaData_t*>(pPrev)[1];

			reinterpret_cast<MetaData_t*>(pPrev)[0] += pSeek ? static_cast<MetaData_t>(pSeek - mpBlock) : 0;
			reinterpret_cast<MetaData_t*>(pPrev)[1] += sz;

			auto new_sz = reinterpret_cast<MetaData_t*>(pPrev)[1];

#		if defined(DEBUGALLOC)
			printf("Join1   [Joined: %p, With: %p, New: %u, Old: %u]\n", pSeek, _ptr, new_sz, old_sz);
			fprintf(output, "Join1   [Actual: %p, Adjusted: %p, New: %u, Old: %u]\n", pSeek, _ptr, new_sz, old_sz);
#		endif
		}
	}
	else
	{
		if (mpFree == (actual + sz))
		{
			reinterpret_cast<MetaData_t*>(actual)[0] = GetNextOffset(mpFree);
			sz += GetBlockSize(mpFree);

#		if defined(DEBUGALLOC)
			printf("Join2   [Joined: %p, With: %p, New: %u, Old: %u]\n", _ptr, mpFree, sz, GetBlockSize(mpFree));
			fprintf(output, "Join2   [Actual: %p, Adjusted: %p, New: %u, Old: %u]\n", _ptr, mpFree, sz, GetBlockSize(mpFree));
#		endif
		}
		else
		{
			reinterpret_cast<MetaData_t*>(actual)[0] = mpFree ? static_cast<MetaData_t>(mpFree - mpBlock) : 0;
		}

		reinterpret_cast<MetaData_t*>(actual)[1] = sz;
		mpFree = actual;
	}

#if defined(DEBUGALLOC)
	fflush(output);
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
		return mpBlock + _nOffset;

	return nullptr;
}

#pragma warning(pop)


