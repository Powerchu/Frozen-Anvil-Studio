/* HEADER *********************************************************************************/
/*!
\file	Utility.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains commonly used (utility) functions to make life easier.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "Utility/Meta.h"
#include "Math/MathUtility.h"
#include "Utility/UtilityInternals.h"

namespace Ut
{
	template <typename T>
	constexpr inline typename RemoveRef<T>::type&& Move(T&& _obj) noexcept
	{
		return static_cast<Ut::RemoveRef_t<T>&&>(_obj);
	}


	template <typename T>
	constexpr inline T&& Forward(RemoveRef_t<T>& _obj) noexcept
	{
		return static_cast<T&&>(_obj);
	}

	template <typename T>
	constexpr inline T&& Forward(RemoveRef_t<T>&& _obj) noexcept
	{
		static_assert(NotLvalueRef<T>::value, "Invalid forwarding call.");
		return static_cast<T&&>(_obj);
	}


	// Increments the value by 1 and
	// wraps the value back to 0 when it reaches Limit
	// ie. within the range of 0 inclusive and Limit exclusive
	template <typename T>
	inline T LoopIncrement(T _nToIncrement, const T _nLimit) noexcept
	{
		static_assert(IsIntegral<T>::value, "Value to increment must be integral");
		return ++_nToIncrement < _nLimit ? _nToIncrement : _nToIncrement = 0;
	}


	// Checks if *positive* number is a power of 2
	template <typename T>
	inline constexpr bool IsPowerOf2(T _nValue)
	{
		static_assert(IsIntegral<T>::value, "Value must be integral");
		return _nValue && 0 == (_nValue & _nValue - 1);
	}
	

	template <typename Itor_t, typename Obj_t>
	Itor_t Find(const Obj_t& _obj, Itor_t _begin, Itor_t const& _end)
	{
		while (_end != _begin)
		{
			if (_obj == *_begin)
				break;
			++_begin;
		}

		return _begin;
	}

	template <typename Itor_t, typename Cond_t>
	Itor_t Find(Itor_t _begin, Itor_t const& _end, Cond_t&& _cond)
	{
		while (_end != _begin)
		{
			if (_cond(*_begin))
				break;
			++_begin;
		}

		return _begin;
	}


	// Sorts a given array using insertion sort. 
	// Defaults to sorting the array in ascending order
	template<class Itor, class Obj = typename RemoveRef<decltype(*declval<Itor>())>::type, 
		typename Comparator = bool(*)(const Obj&, const Obj&)>
	void InsertionSort(Itor const& _begin, Itor const& _end, Comparator&& _pTest = [](const Obj& _lhs, const Obj& _rhs)->bool { return _lhs < _rhs; })
	{
		Obj temp;

		Itor pStart = _begin;
		while (++pStart != _end)
		{
			temp = Move(*pStart);
			Itor pIndex = pStart;
			Itor pPrev = pIndex;

			while (pIndex != _begin && _pTest(temp, *--pIndex))
			{
				*pPrev = Move(*pIndex);
				--pPrev;
			}

			*pPrev = Move(temp);
		}
	}


	template <class Itor, class Ty = Ut::Decay_t<decltype(*Ut::declval<Itor>())>>
	Itor& MoveInit(Itor _begin, const Itor& _end, Itor& _dest)
	{
		while (_begin != _end)
		{
			new (&*_dest) Ty { Ut::Move(*_begin) };
			++_dest; ++_begin;
		}

		return _dest;
	}

	template <class Itor, class Ty = Ut::Decay_t<decltype(*Ut::declval<Itor>())>>
	Itor& CopyInit(Itor _begin, const Itor& _end, Itor& _dest)
	{
		while(_begin != _end)
		{
			new (&*_dest) Ty { *_begin };
			++_dest; ++_begin;
		}

		return _dest;
	}

	template <class OutIt, class InIt, class = EnableIf_t<
		IsConvertible<Decay_t<decltype(*declval<OutIt>())>, Decay_t<decltype(*declval<InIt>())>>::value>>
	InIt Copy(OutIt _begin, const OutIt& _end, InIt _dest)
	{
		while (_begin != _end)
		{
			*_dest = *_begin;
			++_dest; ++_begin;
		}

		return _dest;
	}

	template <class Container, class It>
	RemoveRef_t<It> Copy(Container&& _container, It&& _dest)
	{
		return Copy(_container.begin(), _container.end(), Forward<It>(_dest));
	}


	// Swaps lhs with rhs
	template<typename T>
	inline void Swap(T& _lhs, T& _rhs)
	{
		T tmp{ Ut::Move(_lhs) };

		_lhs = Ut::Move(_rhs);
		_rhs = Ut::Move(tmp);
	}


	template <typename T>
	Interns::RangeMaker<T> Range(T&& _begin, T&& _end)
	{
		return Interns::RangeMaker<T>{ Forward<T>(_begin), Forward<T>(_end) };
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

		auto operator== (const ReverseItor& _rhs) const
		{
			return mItor == _rhs.mItor;
		}

		auto operator!= (const ReverseItor& _rhs) const
		{
			return mItor != _rhs.mItor;
		}
	};
}



#endif		// INCLUDE GUARD

