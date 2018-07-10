/* HEADER *********************************************************************************/
/*!
\file	Vector2.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Struct for generic math vectors in 2 space

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#if defined(DEBUG) | defined(_DEBUG)
#include "Utility\DebugAssert.h"
#endif // Debug only includes

#include "Math\MathUtility.h"

#include <cmath>

namespace Math
{
	#define _CALL	__vectorcall

	/*!
	\struct Vector2
	\brief
		Generic Math 2 Dimensional Space Vector. 
	*/
	struct Vector2
	{
		// ====================================== CONSTRUCTORS ======================================= // 

		inline Vector2(void) noexcept;
		inline Vector2(const Vector2&) noexcept;
		inline Vector2(float x, float y) noexcept;


		// ==================================== VECTOR OPERATIONS ==================================== // 

		// Converts the vector to unit length
		inline Vector2& _CALL Normalise(void);

		// Computes the dot product of this vector and a given vector
		inline float _CALL Dot(const Vector2) const;

		// Computes the cross product of this vector and a given vector 
		inline float _CALL Cross(const Vector2);

		// Projects the Vector4 onto the given vector
		inline Vector2& _CALL Project(const Vector2);

		inline float _CALL Magnitude(void) const;
		inline float _CALL MagnitudeSqr(void) const;

		// ======================================== OPERATORS ======================================= // 

		inline float& _CALL operator[](const unsigned _nIndex);
		inline const float _CALL operator[](const unsigned _nIndex) const;

		inline Vector2& _CALL operator*=(const float);
		inline Vector2& _CALL operator*=(const Vector2);
		inline Vector2& _CALL operator/=(const float);
		inline Vector2& _CALL operator+=(const Vector2);
		inline Vector2& _CALL operator-=(const Vector2);

		float x, y;
	};

	// Converts a vector into unit length
	inline Vector2 _CALL Normalise(const Vector2);

	// Computes the dot product of two vectors
	inline float _CALL Dot(const Vector2, const Vector2);

	// Computes the cross product of two vectors
	inline float _CALL Cross(const Vector2, const Vector2);

	// Projects lhs onto rhs
	inline Vector2 _CALL Project(const Vector2, const Vector2);


	// ====================================== MATH UTILITY ======================================= // 
	// Manually overload the math utility functions which cannot be called for type Vector4

	inline Vector2 _CALL Abs(const Vector2);


	// ======================================== OPERATORS ======================================== // 

	inline Vector2 _CALL operator-(const Vector2);
	inline Vector2 _CALL operator-(const Vector2, const Vector2);
	inline Vector2 _CALL operator+(const Vector2, const Vector2);
	inline Vector2 _CALL operator*(const Vector2, const Vector2);
	inline Vector2 _CALL operator*(const float, const Vector2);
	inline Vector2 _CALL operator*(const Vector2, const float);
	inline Vector2 _CALL operator/(const Vector2, const float);

	using Vec2		= Vector2;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline Math::Vector2::Vector2(void) noexcept
	: x{ 0 }, y{ 0 }
{

}

inline Math::Vector2::Vector2(const Vector2& v) noexcept
	: x{ v.x }, y{ v.y }
{

}

inline Math::Vector2::Vector2(float _x, float _y) noexcept
	: x{ _x }, y{ _y }
{

}

inline Math::Vector2& _CALL Math::Vector2::Normalise(void)
{
#if defined(DEBUG) | defined(_DEBUG)
	DEBUG_ASSERT(IsZero(Dot(*this)), "Vector2 Error: Normalising a zero vector.\n");
#endif

	float invSqrt = 1.f / MagnitudeSqr();

	return *this *= invSqrt;
}

inline Math::Vector2 _CALL Math::Normalise(Vector2 _vec)
{
	return _vec.Normalise();
}

inline float _CALL Math::Vector2::Dot(const Vector2 _rhs) const
{
	return x * _rhs.x + y * _rhs.y;
}

// Computes the dot product of two vectors
inline float _CALL Math::Dot(const Vector2 _lhs, const Vector2 _rhs)
{
	return _lhs.Dot(_rhs);
}

inline float _CALL Math::Vector2::Cross(const Vector2 _rhs)
{
	return x * _rhs.y + y * _rhs.x;
}

inline float _CALL Math::Cross(Vector2 _lhs, Vector2 _rhs)
{
	return _lhs.Cross(_rhs);
}

inline Math::Vector2& _CALL Math::Vector2::Project(const Vector2 _rhs)
{
#if defined(DEBUG) | defined(_DEBUG)
	DEBUG_ASSERT(IsZero(_rhs.Dot(_rhs)), "Vector2 Error: Projection onto zero vector.\n");
#endif

	float mag = Dot(*this) / _rhs.Dot(_rhs);

	return *this = mag * _rhs;
}

// Projects lhs onto rhs
inline Math::Vector2 _CALL Math::Project(Vector2 _lhs, Vector2 _rhs)
{
	return _lhs.Project(_rhs);
}


inline float _CALL Math::Vector2::Magnitude(void) const
{
	return std::sqrtf(MagnitudeSqr());
}

inline float _CALL Math::Vector2::MagnitudeSqr(void) const
{
	return Dot(*this);
}


inline Math::Vector2 _CALL Math::Abs(const Vector2 _v)
{
	return Vector2{ Math::Abs(_v.x), Math::Abs(_v.y) };
}


// ============================================ OPERATOR OVERLOADING ============================================ // 


inline Math::Vector2& _CALL Math::Vector2::operator*=(const float _fScalar)
{
	x *= _fScalar;
	y *= _fScalar;

	return *this;
}

inline Math::Vector2&_CALL Math::Vector2::operator*=(const Vector2 _rhs)
{
	x *= _rhs.x;
	y *= _rhs.y;

	return *this;
}

inline Math::Vector2& _CALL Math::Vector2::operator/=(float _fScalar)
{
	return *this *= (1.f / _fScalar);
}

inline Math::Vector2& _CALL Math::Vector2::operator+=(const Vector2 _rhs)
{
	x += _rhs.x;
	y += _rhs.y;

	return *this;
}

inline Math::Vector2& _CALL Math::Vector2::operator-=(const Vector2 _rhs)
{
	x -= _rhs.x;
	y -= _rhs.y;

	return *this;
}

inline Math::Vector2 _CALL Math::operator-(Math::Vector2 _v)
{
	return Vector2{ -_v.x, -_v.y };
}

inline Math::Vector2 _CALL Math::operator-(Vector2 _lhs, const Vector2 _rhs)
{
	return _lhs -= _rhs;
}

inline Math::Vector2 _CALL Math::operator+(Vector2 _lhs, const Vector2 _rhs)
{
	return _lhs += _rhs;
}

inline Math::Vector2 _CALL Math::operator*(Vector2 _lhs, const Vector2 _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Vector2 _CALL Math::operator*(const float _lhs, Vector2 _rhs)
{
	return _rhs *= _lhs;
}

inline Math::Vector2 _CALL Math::operator*(Vector2 _lhs, const float _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Vector2 _CALL Math::operator/(Vector2 _lhs, const float _rhs)
{
	return _lhs /= _rhs;
}



// Remove all our defines
#ifdef _CALL
#undef _CALL
#endif



#endif		// INCLUDE GUARD

