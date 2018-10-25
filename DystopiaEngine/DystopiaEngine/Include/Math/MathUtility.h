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
#include "Math/MathInternal.h"

namespace Math
{
	namespace C
	{
		template <typename Ty>
		constexpr Ty euler = Ty(2.7182818284590452353602874713526L);

		template <typename Ty>
		constexpr Ty pi    = Ty(3.1415926535897932384626433832795L);
															
		template <typename Ty>								
		constexpr Ty phi   = Ty(1.6180339887498948482045868343656L);

		template <typename Ty>
		constexpr Ty sqrt2 = Ty(1.4142135623730950488016887242096f);
	}

	// Exponential Constant
	constexpr float euler = C::euler<float>;
	// Mathematical PI
	constexpr float pi    = C::pi<float>;
	// 2 * PI
	constexpr float pi2   = 2.f * C::pi<float>;
	// Golden ratio
	constexpr float phi   = C::phi<float>;
	// Sqrt of 2
	constexpr float sqrt2 = C::sqrt2<float>;
	// Floating point error tolerance
	constexpr float epsilon = 0.0000001f;


	template<typename Num>
	inline constexpr typename Ut::EnableIf<Ut::IsNumeric<Num>::value, Num>::type Abs(const Num _x)
	{
		return _x < 0 ? -_x : _x;
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
	inline constexpr T Zero(void)
	{
		return T{ };
	}

	// Checks if two floats are approximately equal
	// Returns false if one of the numbers is zero
	inline bool ApproxEq(float _lhs, float _rhs)
	{
		float diff = Abs(_lhs - _rhs);

		_lhs = Abs(_lhs);
		_rhs = Abs(_rhs);

		float scale = Max(_lhs, _rhs);
		return diff <= (scale * epsilon);
	}

	// Prevent deduction on Min and Max types
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

	template<int exponent, class T>
	inline constexpr T Power(T _x)
	{
		return Internal::PowerCalc<T, exponent <0, Abs(exponent)>::Power(_x);
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



#endif		// INCLUDE GUARD

