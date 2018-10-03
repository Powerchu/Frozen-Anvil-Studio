/* HEADER *********************************************************************************/
/*!
\file	DefaultAlloc.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Default allocator with a limited pool size.
	Exceeding the pool size from the default allocator will forcefully crash the program.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
	static constexpr uint32_t MEBIBYTE = 1048576;
	static constexpr uint32_t DEFAULT_HEAP = 1024 * MEBIBYTE;
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
			std::byte* temp = nullptr;

			const auto offs = static_cast<uint32_t>(static_cast<std::byte*>(pRet) - mpBlock);

			if (blkSz > Utility::Constant<size_t, sizeof(MetaData_t) * 2 - 1>::value)
			{
				temp = static_cast<std::byte*>(pRet) + _sz;

				if (temp - mpBlock < DEFAULT_HEAP)
				{
					reinterpret_cast<MetaData_t*>(temp)[0] = GetNextOffset(pSeek);
					reinterpret_cast<MetaData_t*>(temp)[1] = blkSz;
				}
			}
			else
			{
				adjSz += blkSz;
				blkSz = 0;
				temp = static_cast<std::byte*>(GetBlockFromOffset(GetNextOffset(pSeek)));
			}

			if (pPrev)
			{
				static_cast<MetaData_t*>(pPrev)[0] = temp ? static_cast<MetaData_t>(temp - mpBlock) : 0;
			}

			mpFree = pSeek == mpFree ? temp : mpFree;
			reinterpret_cast<MetaData_t*>(pRet)[-1] = ((adjSz & ~0x3) << 6) + static_cast<unsigned char>(_align - 1);

#        if defined(DEBUGALLOC)
			printf("Alloc   [Actual: %p, Adjusted: %p, Size: %u, Adjust: %zu, Block: %u, %u]\n", pSeek, pRet, adjSz, _align, blkSz, offs);
			fprintf(output, "Alloc   [Actual: %p, Adjusted: %p, Size: %u, Adjust: %zu, Block: %u, %u]\n", pSeek, pRet, adjSz, _align, blkSz, offs);
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

#if defined(DEBUGALLOC)
	printf("Dealloc [Actual: %p, Adjusted: %p, Size: %u, Adjust: %u]\n", actual, _ptr, sz, adjust);
	fprintf(output, "Dealloc [Actual: %p, Adjusted: %p, Size: %u, Adjust: %u]\n", actual, _ptr, sz, adjust);
	fflush(output);
#endif

	while (pSeek && offset > static_cast<MetaData_t>(pSeek - mpBlock))
	{
		pPrev = pSeek;
		pSeek = static_cast<std::byte*>(GetBlockFromOffset(GetNextOffset(pSeek)));
	}

	if (pPrev)
	{
		if (pSeek == (actual + sz))
		{
#		if defined(DEBUGALLOC)
			const auto oldSz = sz;
#		endif

			sz += GetBlockSize(pSeek);

#		if defined(DEBUGALLOC)
			printf("JoinNxt [Joined: %p, With: %p, New: %u, Old: %u]\n", pSeek, _ptr, sz, oldSz);
			fprintf(output, "JoinNxt [Joined: %p, With: %p, New: %u, Old: %u]\n", pSeek, _ptr, sz, oldSz);
#		endif

			const auto tmp = GetNextOffset(pSeek);
			reinterpret_cast<MetaData_t*>(pSeek)[0] = 0;
			pSeek = static_cast<std::byte*>(GetBlockFromOffset(tmp));
		}

		if (pPrev + GetBlockSize(pPrev) != actual)
		{
			reinterpret_cast<MetaData_t*>(actual)[0] = pSeek ? static_cast<MetaData_t>(pSeek - mpBlock) : 0;
			reinterpret_cast<MetaData_t*>(actual)[1] = sz;
			reinterpret_cast<MetaData_t*>(pPrev)[0] = offset;
		}
		else
		{
#		if defined(DEBUGALLOC)
			const auto old_sz = reinterpret_cast<MetaData_t*>(pPrev)[1];
#		endif

			reinterpret_cast<MetaData_t*>(pPrev)[0]  = pSeek ? static_cast<MetaData_t>(pSeek - mpBlock) : 0;
			reinterpret_cast<MetaData_t*>(pPrev)[1] += sz;

#		if defined(DEBUGALLOC)
			const auto new_sz = old_sz + sz;
			printf("JoinPrv [Joined: %p, With: %p, New: %u, Old: %u]\n", pSeek, _ptr, new_sz, old_sz);
			fprintf(output, "JoinPrv [Joined: %p, With: %p, New: %u, Old: %u]\n", pSeek, _ptr, new_sz, old_sz);
#		endif
		}
	}
	else
	{
		if (mpFree == (actual + sz))
		{
			reinterpret_cast<MetaData_t*>(actual)[0] = GetNextOffset(mpFree);
			reinterpret_cast<MetaData_t*>(mpFree)[0] = 0;
			sz += GetBlockSize(mpFree);
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

Dystopia::DefaultAlloc::MetaData_t Dystopia::DefaultAlloc::GetBlockSize(void* _p)
{
	return *(static_cast<MetaData_t*>(_p) + 1);
}

Dystopia::DefaultAlloc::MetaData_t Dystopia::DefaultAlloc::GetNextOffset(void* _p)
{
	return *static_cast<MetaData_t*>(_p);
}

void* Dystopia::DefaultAlloc::GetBlockFromOffset(MetaData_t const _nOffset) const
{
	if (_nOffset)
		return mpBlock + _nOffset;

	return nullptr;
}

#pragma warning(pop)


