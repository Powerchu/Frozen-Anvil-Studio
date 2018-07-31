/* HEADER *********************************************************************************/
/*!
\file	Utility.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains commonly used (utility) functions to make life easier.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "Utility\Meta.h"
#include "Utility\UtilityInternals.h"

namespace Utility
{
	template <typename T>
	constexpr inline typename RemoveRef<T>::type&& Move(T&& _obj) noexcept
	{
		return static_cast<typename RemoveRef<T>::type&&>(_obj);
	}


	template <typename T>
	constexpr inline T&& Forward(typename RemoveRef<T>::type& _obj) noexcept
	{
		// T -> T&& or T& -> T&
		return static_cast<T&&>(_obj);
	}

	template <typename T>
	constexpr inline T&& Forward(typename RemoveRef<T>::type&& _obj) noexcept
	{
		static_assert(NotLvalueRef<T>::value, "%s (%d): Invalid forwarding call.", __FILE__, __LINE__ );
		
		// T&& -> T&&
		return static_cast<T&&>(_obj);
	}


	// Increments the value by 1 and
	// wraps the value back to 0 when it reaches Limit
	// ie. within the range of 0 inclusive and Limit exclusive
	inline int LoopIncrement(int _nToIncrement, const int _nLimit) noexcept
	{
		return ++_nToIncrement < _nLimit ? _nToIncrement : _nToIncrement = 0;
	}

	// Increments the value by 1 and
	// wraps the value back to 0 when it reaches Limit
	// ie. within the range of 0 inclusive and Limit exclusive
	inline unsigned LoopIncrement(unsigned _nToIncrement, const unsigned _nLimit) noexcept
	{
		return ++_nToIncrement < _nLimit ? _nToIncrement : _nToIncrement = 0;
	}


	// Sorts a given array using insertion sort. 
	// Defaults to sorting the array in ascending order
	template<class Itor, typename Comparator, class Obj = typename RemoveRef<decltype(*Itor{})>::type >
	void Sort(Itor _begin, Itor _end, Comparator _pTest = [](Obj& lhs, Obj& rhs)->bool { return lhs < rhs; })
	{
		std::sort(_begin, _end);
		Obj temp;

		Itor pStart = _begin;
		while (_begin != _end)
		{
			temp = Move(*++pStart);
			Itor pIndex = pStart;
			Itor pPrev = pIndex;

			while (pIndex != _begin && _pTest(temp, *pIndex))
			{
				pPrev = pIndex;
				*pPrev = Move(*--_pIndex);
			}

			*pPrev = Move(temp);
		}
	}


	// Swaps lhs with rhs
	template<typename T>
	inline void Swap(T& lhs, T& rhs)
	{
		T tmp{ Move(lhs) };

		lhs = Move(rhs);
		rhs = Move(tmp);
	}

	template <typename T>
	Interns::RangeMaker<T> Range(T&& begin, T&& end)
	{
		return Interns::RangeMaker<T>{ Forward<T>(begin), Forward<T>(end) };
	}

	/*!
	\brief
		Wrapper struct for iterator to increment it in the reverse direction.
		Iterators must point to the correct values for reverse traversal.
	*/
	template <typename T>
	struct ReverseItor
	{
		T mItor;

		template <typename U>
		explicit ReverseItor(U&& _it) : mItor{ std::forward<U>(_it) }
		{
			--mItor; 
		}

		ReverseItor& operator ++ (void)
		{
			--mItor;
			return *this;
		}

		ReverseItor operator ++ (int)
		{
			auto ret = *this;
			--mItor;
			return ret;
		}

		ReverseItor& operator -- (void)
		{
			++mItor;
			return *this;
		}

		ReverseItor operator -- (int)
		{
			auto ret = *this;
			++mItor;
			return ret;
		}

		decltype(auto) operator*(void) const
		{
			return *mItor;
		}

		decltype(auto) operator->(void) const
		{
			return mItor;
		}

		auto operator== (const ReverseItor& rhs) const
		{
			return mItor == rhs.mItor;
		}

		auto operator!= (const ReverseItor& rhs) const
		{
			return mItor != rhs.mItor;
		}
	};
}



#endif		// INCLUDE GUARD

