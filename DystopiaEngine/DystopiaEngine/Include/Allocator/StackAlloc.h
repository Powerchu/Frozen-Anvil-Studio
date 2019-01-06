/* HEADER *********************************************************************************/
/*!
\file	StackAlloc.h
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
#ifndef _STACKALLOC_H_
#define _STACKALLOC_H_

#include "Globals.h"
#include "Utility/Meta.h"
#include "Utility/Utility.h"

#include <new>


namespace Dystopia
{
	template <typename Ty = void>
	struct StackAlloc : private StackAlloc<void>
	{
		using Base = StackAlloc<void>;

		template <typename ... Ps>
		[[nodiscard]] static inline Ty* ConstructAlloc(Ps&& ... _args)
		{
			Ty* temp = static_cast<Ty*>(Base::Alloc(sizeof(Ty), [](void* _p) {
				static_cast<Ty*>(_p)->~Ty();
			}));

			return ::new(temp) Ty{ static_cast<Ps&&>(_args)... };
		}
	};

	template <>
	struct StackAlloc<void>
	{
		[[nodiscard]] static void* Alloc(size_t _sz, void(*)(void*) = nullptr);
		static void Free(unsigned _n) noexcept;

		static void* GetBuffer(void) noexcept;
		static size_t GetUsableSize(void) noexcept;
	};

	template <typename Ty, unsigned N>
	struct StackAlloc<Ty[N]> : private StackAlloc<void>
	{
		using Base = StackAlloc<void>;

		template <typename ... Ps>
		[[nodiscard]] static inline Ty* ConstructAlloc(Ps&& ... _args)
		{
			Ty* temp = Base::Alloc(sizeof(Ty) * N, [](void* _p) {
				Ty* end = static_cast<Ty*>(_p) + N;

				while (end != _p) (--end)->~Ty();
			});

			if constexpr (N)
			{
				if constexpr (N > 1)
				{
					auto e = temp + N;
					while (temp != --e) ::new (e) Ty{ Ut::AsConstRef<Ps>(_args)... };
				}

				::new (temp) Ty{ Ut::Fwd<Ps>(_args)... };
			}

			return temp;
		}
	};

	template <typename Ty>
	struct StackAlloc<Ty[]> : private StackAlloc<void>
	{
		using Base = StackAlloc<void>;

		template <typename ... Ps>
		[[nodiscard]] static inline Ty* ConstructAlloc(size_t _sz, Ps&& ... _args)
		{
			Ty* temp = Base::Alloc(sizeof(Ty) * _sz, [](void* _p) {
				auto sz = static_cast<size_t*>(_p)[-1] / sizeof(Ty);
				Ty* end = static_cast<Ty*>(_p) + sz;

				while (end != _p) (--end)->~Ty();
			}) + _sz;

			if (_sz)
			{
				while (--_sz) ::new (--e) Ty{ Ut::AsConstRef<Ps>(_args)... };

				::new (--e) Ty{ Ut::Fwd<Ps>(_args)... };
			}

			return e;
		}
	};

	using StackAlloc_t = StackAlloc<void>;
}



#endif

