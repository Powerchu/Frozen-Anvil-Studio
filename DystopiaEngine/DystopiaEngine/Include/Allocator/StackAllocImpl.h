/* HEADER *********************************************************************************/
/*!
\file	StackAllocImpl.h
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
#ifndef _DEFAULTALLOCIMPL_H_
#define _DEFAULTALLOCIMPL_H_

#include "System/Logger/FileLogger.h"

#include <cstddef>
#include <cstdint>


namespace Dystopia
{
	class StackAllocImpl
	{
	public:

		using Dtor_t = void(*)(void*);

		StackAllocImpl(void);
		~StackAllocImpl(void) noexcept;

		[[nodiscard]] void* Allocate(size_t, Dtor_t);

		//void Deallocate(void*) noexcept;
		void Deallocate(unsigned) noexcept;

		void* GetBuffer(void) noexcept;
		size_t GetUsableSize(void) noexcept;

	private:
		std::byte* mpNext;
		std::byte* mpBuffer;

		static constexpr size_t DEFAULT_SZ = 8 * 1048576ull;
	};
}



#endif

