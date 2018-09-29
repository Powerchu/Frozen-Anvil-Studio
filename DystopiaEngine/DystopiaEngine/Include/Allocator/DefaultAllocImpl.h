/* HEADER *********************************************************************************/
/*!
\file	DefaultAllocImpl.h
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
#ifndef _DEFAULTALLOCIMPL_H_
#define _DEFAULTALLOCIMPL_H_

#include <cstddef>
#include <cstdint>


namespace Dystopia
{
	class DefaultAlloc
	{
	public:
		using MetaData_t = uint32_t;

		DefaultAlloc(void);
		~DefaultAlloc(void) noexcept;

		[[nodiscard]] void* Allocate(size_t, size_t);
		void Deallocate(void*);

	private:
		std::byte* mpBlock;
		std::byte* mpFree;

		static constexpr auto MIN_ALIGN = alignof(MetaData_t);
		static constexpr auto MIN_SIZE  = sizeof(MetaData_t) * 2 - MIN_ALIGN;

		MetaData_t GetBlockSize(void*);
		MetaData_t GetNextOffset(void*);
		void* GetBlockFromOffset(MetaData_t);
	};
}



#endif

