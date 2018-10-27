/* HEADER *********************************************************************************/
/*!
\file	DefaultAlloc.h
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
#ifndef _DEFAULTALLOC_H_
#define _DEFAULTALLOC_H_
#include "Globals.h"
namespace Dystopia
{
	template <typename Ty = void>
	struct _DLL_EXPORT DefaultAllocator : public DefaultAllocator<void>
	{
		using Base = DefaultAllocator<void>;

		[[nodiscard]] static inline Ty* Alloc(void)
		{
			return static_cast<Ty*>(Base::Alloc(sizeof(Ty), alignof(Ty)));
		}

		static inline void Free(Ty* _ptr) noexcept
		{
			Base::Free(static_cast<void*>(_ptr));
		}

		template <typename ... Ps>
		[[nodiscard]] static inline Ty* ConstructAlloc(Ps&& ... _Args)
		{
			Ty* temp = Alloc();
			return ::new(temp) Ty{ static_cast<Ps&&>(_Args) ... };
		}

		static inline void DestructFree(Ty* _ptr) noexcept
		{
			_ptr->~Ty();
			Free(_ptr);
		}
	};

	template <>
	struct  _DLL_EXPORT DefaultAllocator<void>
	{
		[[nodiscard]] static void* Alloc(size_t _sz, size_t _align = 8);
		static void Free(void* _ptr) noexcept;
	};

	template <typename Ty>
	struct _DLL_EXPORT DefaultAllocator<Ty[]> : DefaultAllocator<void>
	{
		using Base = DefaultAllocator<void>;

		[[nodiscard]] static inline Ty* Alloc(size_t _sz)
		{
			return static_cast<Ty*>(Base::Alloc(_sz * sizeof(Ty), alignof(Ty)));
		}

		static inline void Free(Ty* _ptr) noexcept
		{
			Base::Free(static_cast<void*>(_ptr));
		}
	};
}



#endif

