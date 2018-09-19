/* HEADER *********************************************************************************/
/*!
\file	Vector4_old.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Plain old struct for generic math vectors in 4 space

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Vector4_old.h"		// File Header
#include "MathUtility.h"
#include "DebugAssert.h"

#include <cmath>

namespace Math
{
	Vector4& Vector4::Normalise()
	{
#if _DEBUG
		DEBUG_LOG(IsZero(MagnitudeSqr()), "Vector4 Error: Cannot normalise zero vector!\n");
#endif
		return *this /= Magnitude();
	}

	float Vector4::Dot(const Vector4& _rhs) const
	{
		return
			x * _rhs.x +
			y * _rhs.y +
			z * _rhs.z +
			w * _rhs.w;
	}

	Vector4& Vector4::Project(const Vector4& _rhs)
	{
#if _DEBUG
		DEBUG_LOG(IsZero(_rhs.MagnitudeSqr()), "Vector4 Error: Cannot project onto zero vector!\n");
#endif
		return *this = (Dot(_rhs) / _rhs.MagnitudeSqr()) * _rhs;
	}

	float Vector4::Magnitude(void) const
	{
		return std::sqrtf(MagnitudeSqr());
	}

	float Vector4::MagnitudeSqr(void) const
	{
		return Dot(*this);
	}

	Vector4 Normalise(const Vector4& _v)
	{
#if _DEBUG
		DEBUG_LOG(IsZero(_v.MagnitudeSqr()), "Vector4 Error: Cannot normalise zero vector!\n");
#endif
		Vector4 temp{ _v };
		return temp.Normalise();
	}

	float Dot(const Vector4& _lhs, const Vector4& _rhs)
	{
		Vector4 temp{ _lhs };
		return temp.Dot(_rhs);
	}

	Vector4 Cross(const Vector4& _lhs, const Vector4& _rhs)
	{
		return Vector4{
			_lhs.y * _rhs.z - _lhs.z * _rhs.y,
			_lhs.z * _rhs.x - _lhs.x * _rhs.z,
			_lhs.x * _rhs.y - _lhs.y * _rhs.x,
			.0f
		};
	}

	Vector4 Project(const Vector4& _lhs, const Vector4& _rhs)
	{
#if _DEBUG
		DEBUG_LOG(IsZero(_rhs.MagnitudeSqr()), "Vector4 Error: Cannot project onto zero vector!\n");
#endif
		Vector4 temp{ _lhs };
		return temp.Project(_rhs);
	}

	// ===========================  MEMBER OPERATOR OVERLOADING =========================== //

	float& Vector4::operator[](const unsigned _nIndex)
	{
		return *(&x + _nIndex);
	}

	const float& Vector4::operator[](const unsigned _nIndex) const
	{
		return *(&x + _nIndex);
	}

	Vector4& Vector4::operator*=(const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;
		z *= _rhs;
		w *= _rhs;

		return *this;
	}

	Vector4& Vector4::operator/=(const float _rhs)
	{
		return *this *= (1.f / _rhs);
	}

	Vector4& Vector4::operator+=(const Vector4& _rhs)
	{
		x += _rhs.x;
		y += _rhs.y;
		z += _rhs.z;
		w += _rhs.w;

		return *this;
	}

	Vector4& Vector4::operator-=(const Vector4& _rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;
		z -= _rhs.z;
		w -= _rhs.w;

		return *this;
	}

	// =========================  NON MEMBER OPERATOR OVERLOADING ========================= //

	Vector4 operator- (const Vector4& _rhs)
	{
		return Vec4(_rhs) *= -1;
	}

	Vector4 operator- (const Vector4& _lhs, const Vector4& _rhs)
	{
		return Vec4(_lhs) -= _rhs;
	}

	Vector4 operator+ (const Vector4& _lhs, const Vector4& _rhs)
	{
		return Vec4(_lhs) += _rhs;
	}

	Vector4 operator* (const float _lhs, const Vector4& _rhs)
	{
		return Vec4(_rhs) *= _lhs;
	}

	Vector4 operator* (const Vector4& _lhs, const float _rhs)
	{
		return Vec4(_lhs) *= _rhs;
	}

	Vector4 operator/ (const Vector4& _lhs, const float _rhs)
	{
		return Vec4(_lhs) /= _rhs;
	}
}

