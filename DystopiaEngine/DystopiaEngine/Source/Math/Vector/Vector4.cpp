/* HEADER *********************************************************************************/
/*!
\file	Vector4.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Function definitions for non-lined functions and/or functions that has to include
	other constructs that were made in terms of Vector4

	Reference: https://software.intel.com/sites/landingpage/IntrinsicsGuide

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Math/Vector/Vector4.h"
#include "Math/Vector/Vector3.h"
#include "Math/Vector/Vector2.h"
#include "Math/Matrix4.h"

#include <cmath>		// sqrtf
#include <xmmintrin.h>  // SSE


Math::Vector4::Vector4(Vector2 const& _v, float z, float w) noexcept
	: Vector4{ _v[0], _v[1], z, w }
{

}

Math::Vector4::Vector4(Vector3 const& _v, float w) noexcept
	: Vector4{ _v[0], _v[1], _v[2], w }
{

}


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


