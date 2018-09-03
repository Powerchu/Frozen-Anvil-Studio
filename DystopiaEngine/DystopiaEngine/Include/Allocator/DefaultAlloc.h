/* HEADER *********************************************************************************/
/*!
\file	Defaults.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	TODO: Think of a description for this file

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _DEFAULTS_H_
#define _DEFAULTS_H_

namespace Dystopia
{
	template <typename Ty = void>
	struct DefaultAllocator : public DefaultAllocator<void>
	{
		using Base = DefaultAllocator<void>;

		static inline Ty* Alloc(void)
		{
			return static_cast<Ty*>(Base::Alloc(sizeof(Ty)));
		}

		static inline void Free(Ty* _ptr) noexcept
		{
			Base::Free(static_cast<void*>(_ptr));
		}

		template <typename ... Ps>
		static inline Ty* ConstructAlloc(Ps&& ... _Args)
		{
			// Use static cast to avoid includes
			return new Ty{ static_cast<Ps&&>(_Args) ... };
		}

		static inline void DestructFree(Ty* _ptr) noexcept
		{
			delete _ptr;
		}
	};

	template <>
	struct DefaultAllocator<void>
	{
		static inline void* Alloc(size_t sz)
		{
			return ::operator new (sz);
		}

		static inline void Free(void* _ptr) noexcept
		{
			::operator delete (_ptr);
		}
	};

	template <typename Ty>
	struct DefaultAllocator<Ty[]>
	{
		static inline Ty* Alloc(size_t sz)
		{
			return static_cast<Ty*>(::operator new[](sz * sizeof(Ty)));
		}

		static inline void Free(Ty* _ptr) noexcept
		{
			::operator delete[] (static_cast<void*>(_ptr));
		}
	};
}



#endif

