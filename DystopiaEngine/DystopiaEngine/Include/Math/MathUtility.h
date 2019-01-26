/* HEADER *********************************************************************************/
/*!
\file	MathUtility.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains commonly used Math functions to make life easier.
	Tell me if more is needed and I'll add them! (or you can DIY)

	All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
	Reproduction or disclosure of this file or its contents without the
	prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATH_UTILS_H_
#define _MATH_UTILS_H_

#include "Utility/Meta.h"		// EnableIf, IsNumeric

#include "Math/MathFwd.h"
#include "Math/MathInternal.h"
#include "Math/MathConstants.h"

#include <cstdint>


namespace Math
{
	template<typename Num>
	inline constexpr typename Ut::EnableIf<Ut::IsNumeric<Num>::value, Num>::type Abs(const Num _x)
	{
		return _x < 0 ? -_x : _x;
	}

	template<int64_t exponent, class T>
	inline constexpr T Power(T _x)
	{
		return Internal::PowerCalc<T, (exponent < 0), Abs(exponent)>::Power(_x);
	}

	inline constexpr bool IsZero(float _fScalar)
	{
		return _fScalar > -epsilon && _fScalar < epsilon;
	}

	template <typename T>
	inline constexpr T Min(T _x, T _y)
	{
		return _y < _x ? _y : _x;
	}

	template <typename T>
	inline constexpr T Max(T _x, T _y)
	{
		return _x < _y ? _y : _x;
	}

	template <typename T, typename R = float>
	inline constexpr auto Reciprocal(T _x) -> Ut::EnableIf_t<Ut::IsNumeric<T>::value, R>
	{
		return R(1) / _x;
	}

	template <typename T>
	inline constexpr auto Zero(void)
	{
		return T{ };
	}

	// Checks if two floats are approximately equal
	// Returns false if one of the numbers is zero
	template <typename T>
	inline constexpr auto ApproxEq(T _lhs, T _rhs) -> Ut::EnableIf_t<Ut::IsFloatType<T>::value, bool>
	{
		float diff = Abs(_lhs - _rhs);

		_lhs = Abs(_lhs);
		_rhs = Abs(_rhs);

		float scale = Max(_lhs, _rhs);
		return diff <= (scale * C::epsilon<T>);
	}

	template <typename T>
	inline constexpr T Clamp(T _input, Ut::Type_t<T> _min, Ut::Type_t<T> _max)
	{
		return Max(_min, Min(_max, _input));
	}

	template <typename T>
	inline constexpr T Wrap(T _input, Ut::Type_t<T> _min, Ut::Type_t<T> _max)
	{
		auto range = _max - _min;
		_input -= size_t(_input / range) * range;
		return _input < 0 ? _input + _max : _input + _min;
	}

	template<class T>
	inline constexpr T Lerp(T _start, T _end, float _fRatio)
	{
		return _start + _fRatio * (_end - _start);
	}

	template <size_t _val, size_t _Base = 2>
	struct Log
	{
		static constexpr size_t value = 1 + Log<_val? (_val<_Base)?1:(_val/_Base) : 0, _Base>::value;
	};

	template <size_t _Base>
	struct Log<1, _Base>
	{
		static constexpr size_t value = 0;
	};

	template <size_t _Base>
	struct Log<0, _Base>
	{
		
	};
}

namespace Ut
{
	using Math::Abs;
	using Math::Min;
	using Math::Max;
	using Math::Wrap;
	using Math::Clamp;
	using Math::Power;
	using Math::IsZero;
	using Math::ApproxEq;

	using Math::Log;
}



#endif		// INCLUDE GUARD

