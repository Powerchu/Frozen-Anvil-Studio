/* HEADER *********************************************************************************/
/*!
\file	Vector2.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Function definitions for functions dealing with other Math objects

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <cmath>			// sqrtf


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


