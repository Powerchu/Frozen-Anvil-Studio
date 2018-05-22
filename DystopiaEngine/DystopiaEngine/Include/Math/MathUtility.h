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

#include "Utility\Meta.h"		// EnableIf, IsNumeric
#include "Math\MathInternal.h"

namespace Math
{
	// Exponential Function
	constexpr float e   = 2.7182818284590452353602874713526f;
	// Mathematical PI
	constexpr float pi  = 3.1415926535897932384626433832795f;
	// 2 * PI
	constexpr float pi2 = 2.f * pi;
	// Golden ratio
	constexpr float phi = 1.6180339887498948482045868343656f;
	// Floating point error tolerance
	constexpr float epsilon = 0.0000001f;

	template<typename Num>
	inline typename Utility::EnableIf<Utility::IsNumeric<Num>::value, Num>::type Abs(const Num _x)
	{
		return _x < 0 ? -_x : _x;
	}

	// Deprecated
	template<const unsigned n, class T>
	inline T Abs(T _vec)
	{
		InternalHelper::AbsHelper<T, n - 1>::CalculateAbs(_vec);
		return _vec;
	}

	inline bool IsZero(float _fScalar)
	{
		return _fScalar > -epsilon && _fScalar < epsilon;
	}

	inline float Min(float _x, float _y)
	{
		return _x > _y ? _y : _x;
	}

	inline float Max(float _x, float _y)
	{
		return _x < _y ? _y : _x;
	}

	// Checks if two floats are approximately equal
	// Returns false if one of the numbers is zero
	inline bool ApproxEq(float _lhs, float _rhs)
	{
		float diff = Abs(_lhs - _rhs);

		_lhs = Abs(_lhs);
		_rhs = Abs(_rhs);

		float scale = Max(_lhs, _rhs);
		return diff < (scale * epsilon);
	}

	inline float Clamp(float _fInput, float _fMin, float _fMax)
	{
		return Max(_fMin, Min(_fMax, _fInput));
	}

	inline float DegToRad(float _fDegrees)
	{
		constexpr float DegRadRatio = pi / 180.f;

		return _fDegrees * DegRadRatio;
	}

	inline float RadToDeg(float _fRadians)
	{
		constexpr float RadDegRatio = 180.f / pi;

		return _fRadians * RadDegRatio;
	}

	template<class T>
	inline T Lerp(T _start, T _end, float _fRatio)
	{
		return (1.f - _fRatio) * _start + _fRatio * _end;
	}

	template<int exponent, class T>
	inline constexpr T Power(T _x)
	{
		return InternalHelper::PowerCalc<T, exponent < 0, exponent < 0 ? -exponent : exponent>::Power(_x);
	}
}



#endif		// INCLUDE GUARD

