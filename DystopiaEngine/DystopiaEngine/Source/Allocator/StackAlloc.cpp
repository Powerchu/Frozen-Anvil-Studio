/* HEADER *********************************************************************************/
/*!
\file	StackAlloc.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Allocator for temporary objects.
	Anyone can free the allocated objects.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Allocator/StackAlloc.h"
#include "Allocator/StackAllocImpl.h"

#include <cstdlib>


namespace
{
	static Dystopia::StackAllocImpl mAllocator{};
}

[[nodiscard]] void* Dystopia::StackAlloc<void>::Alloc(size_t _sz, void(*_dtor)(void*))
{
	return mAllocator.Allocate(_sz, _dtor);
}

void Dystopia::StackAlloc<void>::Free(unsigned _n) noexcept
{
	mAllocator.Deallocate(_n);
}

[[nodiscard]] void* Dystopia::StackAlloc<void>::GetBuffer(void) noexcept
{
	return mAllocator.GetBuffer();
}

size_t Dystopia::StackAlloc<void>::GetUsableSize(void) noexcept
{
	return mAllocator.GetUsableSize();
}


Dystopia::StackAllocImpl::StackAllocImpl(void)
	: mpBuffer{ reinterpret_cast<std::byte*>(std::malloc(DEFAULT_SZ)) }, mpNext{ mpBuffer + DEFAULT_SZ }
{
	if (!mpBuffer) std::terminate();
}

Dystopia::StackAllocImpl::~StackAllocImpl(void) noexcept
{
	Deallocate(Ut::Constant<unsigned, -1>::value);
	std::free(mpBuffer);
}


void* Dystopia::StackAllocImpl::GetBuffer(void) noexcept
{
	return mpBuffer;
}

size_t Dystopia::StackAllocImpl::GetUsableSize(void) noexcept
{
	return mpNext - mpBuffer;
}


[[nodiscard]] void* Dystopia::StackAllocImpl::Allocate(size_t _sz, Dtor_t _pDtor)
{
	_sz = (_sz & ~0xFull) + 0x10 * !!(_sz & 0xF);

	if (GetUsableSize() >= (_sz + 0x10))
	{
		auto ret = mpNext - _sz;
		mpNext -= _sz + sizeof(Dtor_t) * 2;

		reinterpret_cast<size_t*>(ret)[-1] = _sz;
		reinterpret_cast<Dtor_t*>(ret)[-2] = _pDtor;

		return ret;
	}

	throw;
}

void Dystopia::StackAllocImpl::Deallocate(unsigned _n) noexcept
{
	while (_n && GetUsableSize() != DEFAULT_SZ)
	{
		auto dtor = reinterpret_cast<Dtor_t*>(mpNext)[0];
		auto sz   = reinterpret_cast<size_t*>(mpNext)[1];

		dtor(mpNext + sizeof(Dtor_t) * 2);
		mpNext += sizeof(Dtor_t) * 2 + sz;

		--_n;
	}
}


