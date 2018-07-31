/* HEADER *********************************************************************************/
/*!
\file	UtilityInternals.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Stuff internal to Utility and is hidden from the user for various reasons.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _UTIL_INTERNALS_H_
#define _UTIL_INTERNALS_H_



namespace Utility {

	template <typename T>
	struct ReverseItor;

namespace Interns {


	template <typename Itor>
	struct RangeMaker
	{
	public:

		template <typename B, typename E>
		explicit RangeMaker(B&& _beg, E&& _end) :
			b{ std::forward<B>(_beg) }, e{ std::forward<E>(_end) }
		{}

		inline Itor begin()
		{
			return (b);
		}

		inline Itor end()
		{
			return (e);
		}

	private:
		Itor b;
		Itor e;
	};

	template <typename T>
	struct RangeMaker<T*>
	{
		template <typename B, typename E>
		explicit RangeMaker(B&& _beg, E&& _end) :
			b{ std::forward<B>(_beg) }, e{ std::forward<E>(_end) }
		{}

		using rItor_t = ReverseItor<T*>;

		inline T* begin()
		{
			return b;
		}

		inline T* end()
		{
			return e;
		}

		inline rItor_t rbegin()
		{
			return rItor_t{ e };
		}

		inline rItor_t rend()
		{
			return rItor_t{ b };
		}

		inline RangeMaker<rItor_t> Reverse(void) const
		{
			return RangeMaker<rItor_t>{ rItor_t{ e }, rItor_t{ b } };
		}

	private:
		T* b;
		T* e;
	};

	template <typename T>
	struct RangeMaker<const T*>
	{
		template <typename B, typename E>
		explicit RangeMaker(B&& _beg, E&& _end) :
			b{ std::forward<B>(_beg) }, e{ std::forward<E>(_end) }
		{}

		using rItor_t = ReverseItor<const T*>;

		inline T* begin()
		{
			return b;
		}

		inline T* end()
		{
			return e;
		}

		inline T* rbegin()
		{
			return rItor_t{ --e };
		}

		inline T* rend()
		{
			return rItor_t{ --b };
		}

		inline RangeMaker<rItor_t> Reverse(void) const
		{
			return RangeMaker<rItor_t>{ rItor_t{ e }, rItor_t{ b } };
		}

	private:
		T* b;
		T* e;
	};

}}



#endif		// INCLUDE GUARD

