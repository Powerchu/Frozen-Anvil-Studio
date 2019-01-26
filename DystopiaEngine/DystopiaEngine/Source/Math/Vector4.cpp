/* HEADER *********************************************************************************/
/*!
\file	Vector4.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Function definitions for non-lined functions and/or functions that has to include
	other constructs that were made in terms of Vector4

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Math/Vector4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Matrix4.h"

#include <cmath>		// sqrtf
#include <xmmintrin.h>  // SSE


float __vectorcall Math::Vector4::Magnitude(void) const
{
	return std::sqrtf(MagnitudeSqr());
}

Math::Matrix4 __vectorcall Math::Vector4::Outer(const Vector4 _rhs) const
{
	return ::Math::Outer(*this, _rhs);
}

Math::Matrix4 __vectorcall Math::Outer(const Math::Vector4 _lhs, const Math::Vector4 _rhs)
{
	return Math::Mat4{
		_lhs.xxxx * _rhs,
		_lhs.yyyy * _rhs,
		_lhs.zzzz * _rhs,
		_lhs.wwww * _rhs
	};
}


// Conversions
template <unsigned X, unsigned Y, unsigned Z>
Math::Vector4::SwizzleMask<X, Y, Z>& __vectorcall Math::Vector4::SwizzleMask<X, Y, Z>::operator = (Math::Vector3)
{

	return *this;
}

template <unsigned X, unsigned Y, unsigned Z>
Math::Vector4::SwizzleMask<X, Y, Z>::operator Math::Vector3(void) const noexcept
{
	float out[4];
	_mm_store_ps(out, GetRaw());

	return Math::Vec3{ out[0], out[1], out[2] };
}

