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

#include "System/Logger/FileLogger.h"

#include <cstddef>
#include <cstdint>


namespace Dystopia
{
	class DefaultAlloc
	{
		using MetaData_t = uint32_t;

	public:
		DefaultAlloc(void);
		~DefaultAlloc(void) noexcept;

		[[nodiscard]] void* Allocate(size_t, size_t);
		void Deallocate(void*);

		static void WriteFreeMemory(FileLogger&);
		static void WriteActiveAllocations(FileLogger&);

	private:
		std::byte* mpBlock;
		std::byte* mpFree;

		static constexpr size_t MIN_ALIGN   = 16;
		static constexpr size_t MIN_SIZE    = 8;
		static constexpr size_t METADATA_SZ = sizeof(MetaData_t);

		static MetaData_t GetBlockSize(void*);
		static MetaData_t GetNextOffset(void*);
		void* GetBlockFromOffset(MetaData_t) const;

		void WriteFreeMemoryImpl(FileLogger&) const;
	};
}



#endif

