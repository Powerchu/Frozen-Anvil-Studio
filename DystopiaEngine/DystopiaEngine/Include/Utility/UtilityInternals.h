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

		inline Itor&& begin()
		{
			return std::forward<Itor>(b);
		}

		inline Itor&& end()
		{
			return std::forward<Itor>(e);
		}

	private:
		Itor&& b;
		Itor&& e;
	};

	template <typename T>
	struct RangeMaker<T*>
	{
		template <typename B, typename E>
		explicit RangeMaker(B&& _beg, E&& _end) :
			b{ std::forward<B>(_beg) }, e{ std::forward<E>(_end) }
		{}

		inline T* begin()
		{
			return std::forward<Itor>(b);
		}

		inline T* end()
		{
			return std::forward<Itor>(e);
		}

		inline T* rbegin()
		{
			return --e;
		}

		inline T* rend()
		{
			return --b;
		}

		inline RangeMaker Reverse(void) const
		{
			return RangeMaker{ ReverseItor<T*>(e - 1), ReverseItor<T*>(b - 1) };
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

		inline T* begin()
		{
			return std::forward<Itor>(b);
		}

		inline T* end()
		{
			return std::forward<Itor>(e);
		}

		inline T* rbegin()
		{
			return --e;
		}

		inline T* rend()
		{
			return --b;
		}

		inline RangeMaker Reverse(void) const
		{
			return RangeMaker{ e - 1, b - 1 };
		}

	private:
		T* b;
		T* e;
	};

}}



#endif		// INCLUDE GUARD

