/* HEADER *********************************************************************************/
/*!
\file	SwizzleConversions.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains the functions definitions for vector swizzles that
	converts between the different vectors

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATHSWIZZLE_OPS_H_
#define _MATHSWIZZLE_OPS_H_

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#define _CALL __vectorcall


namespace Math
{
// ================================================== VECTOR 2 ================================================== // 

	template <unsigned X, unsigned Y, unsigned Z>
	inline _CALL Vector2::Swizzle<X, Y, Z>::operator Vector3 (void) const noexcept
	{
		return Vector3{ mData[X], mData[Y], mData[Z] };
	}

	template <unsigned X, unsigned Y, unsigned Z, unsigned W>
	inline _CALL Vector2::Swizzle<X, Y, Z, W>::operator Vector4 (void) const noexcept
	{
		return Vector4{ mData[X], mData[Y], mData[Z], mData[W] };
	}


// ================================================== VECTOR 3 ================================================== // 

	template <unsigned X, unsigned Y> template <bool>
	inline Vector3::Swizzle<X, Y>&_CALL Vector3::Swizzle<X, Y>::operator = (Vector2 const& _rhs) noexcept
	{
		mData[X] = _rhs[0];
		mData[Y] = _rhs[1];

		return *this;
	}

	template <unsigned X, unsigned Y>
	inline constexpr _CALL Vector3::Swizzle<X, Y>::operator Vector2 (void) const noexcept
	{
		return Vector2{ mData[X], mData[Y] };
	}

	template <unsigned X, unsigned Y, unsigned Z, unsigned W>
	inline _CALL Vector3::Swizzle<X, Y, Z, W>::operator Vector4 (void) const noexcept
	{
		float w;
		if constexpr (W < 3)
			w = mData[W];
		else
			w = W - 3;

		return Vector4{ mData[X], mData[Y], mData[Z], w };
	}


// ================================================== VECTOR 4 ================================================== // 

	template <unsigned X, unsigned Y> template <bool>
	Vector4::Swizzle<X, Y>& __vectorcall Vector4::Swizzle<X, Y>::operator = (Vector2 _v) noexcept
	{
		(*this)[X] = _v[0];
		(*this)[Y] = _v[1];

		return *this;
	}

	template <unsigned X, unsigned Y>
	Vector4::Swizzle<X, Y>::operator Vector2 (void) const noexcept
	{
		alignas(16) float out[4];
		_mm_store_ps(out, mData);

		return Math::Vector2{ out[X], out[Y] };
	}

	template <unsigned X, unsigned Y, unsigned Z> template <bool>
	Vector4::Swizzle<X, Y, Z>& __vectorcall Vector4::Swizzle<X, Y, Z>::operator = (Vector3 _v) noexcept
	{
		(*this)[X] = _v[0];
		(*this)[Y] = _v[1];
		(*this)[Z] = _v[2];

		return *this;
	}

	template <unsigned X, unsigned Y, unsigned Z>
	Vector4::Swizzle<X, Y, Z>::operator Vector3 (void) const noexcept
	{
		alignas(16) float out[4];
		_mm_store_ps(out, mData);

		return Math::Vec3{ out[X], out[Y], out[Z] };
	}
}

#undef _CALL



#endif		// INCLUDE GUARD

