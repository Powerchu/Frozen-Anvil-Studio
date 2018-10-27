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
#include "Utility/DebugAssert.h"
#endif // Debug only includes

#include "Math/Vector4.h"
#include "Math/MathUtility.h"
#include <cmath>
#undef ERROR

namespace Math
{
	#define _CALL	__vectorcall

	/*!
	\struct Vector2
	\brief
		Generic Math 2 Dimensional Space Vector. 
	*/
	union _DLL_EXPORT Vector2
	{
		// ====================================== CONSTRUCTORS ======================================= // 

		inline constexpr Vector2(void) noexcept;
//		inline Vector2(const Vector2&) noexcept;
		inline constexpr Vector2(float x, float y) noexcept;


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
		inline bool _CALL operator==(const Vector2);

	private:

		template <unsigned ...Sz>
		struct _DLL_EXPORT Swizzle : SwizzleMask
		{		};

		template <unsigned N>
		struct _DLL_EXPORT Swizzle<N>
		{
			inline Swizzle<N>& operator = (float);
			inline Swizzle<N>& operator += (float);
			inline Swizzle<N>& operator -= (float);
			inline Swizzle<N>& operator *= (float);
			inline Swizzle<N>& operator /= (float);
			inline bool operator == (float);
			inline bool operator != (float);

			inline _CALL operator float&(void);
			inline _CALL operator float(void) const;

		private:
			float elem[2];
		};

		template <unsigned X, unsigned Y>
		struct Swizzle<X, Y>
		{
			inline constexpr _CALL operator Math::Vector2 (void) const;

			template <bool = X != Y>
			inline Swizzle<X, Y>& _CALL operator = (const Vector2);

			template <>
			inline Swizzle<X, Y>& _CALL operator = <false> (const Vector2)
			{
				static_assert(false, "Vector2 Error: lvalue cannot be duplicate.");
				return *this;
			}

		private:
			float elem[2];
		};

		template <unsigned X, unsigned Y, unsigned Z, unsigned W>
		struct Swizzle<X, Y, Z, W>
		{
			inline constexpr _CALL operator Math::Vector4 (void) const;

		private:
			float elem[2];
		};

		float elem[2];

	public:

		Swizzle<0>  x;
		Swizzle<1> y;
		Swizzle<0, 0> xx;
		Swizzle<0, 1> xy;
		Swizzle<1, 0> yx;
		Swizzle<1, 1> yy;
		Swizzle<0, 0, 0, 0> xxxx;
		Swizzle<0, 0, 0, 1> xxxy;
		Swizzle<0, 0, 1, 0> xxyx;
		Swizzle<0, 0, 1, 1> xxyy;
		Swizzle<0, 1, 0, 0> xyxx;
		Swizzle<0, 1, 0, 1> xyxy;
		Swizzle<0, 1, 1, 0> xyyx;
		Swizzle<0, 1, 1, 1> xyyy;
		Swizzle<1, 0, 0, 0> yxxx;
		Swizzle<1, 0, 0, 1> yxxy;
		Swizzle<1, 0, 1, 0> yxyx;
		Swizzle<1, 0, 1, 1> yxyy;
		Swizzle<1, 1, 0, 0> yyxx;
		Swizzle<1, 1, 0, 1> yyxy;
		Swizzle<1, 1, 1, 0> yyyx;
		Swizzle<1, 1, 1, 1> yyyy;
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


inline constexpr Math::Vector2::Vector2(void) noexcept
	: elem { 0 }
{

}

inline constexpr Math::Vector2::Vector2(float _x, float _y) noexcept
	: elem{ _x, _y }
{

}

inline Math::Vector2& _CALL Math::Vector2::Normalise(void)
{
#if defined(DEBUG) | defined(_DEBUG)
	DEBUG_ASSERT(IsZero(Dot(*this)), "Vector2 Error: Normalising a zero vector.\n");
#endif

	const float invSqrt = 1.f / MagnitudeSqr();

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
	return Vector2{ Math::Abs<float>(_v.x), Math::Abs<float>(_v.y) };
}



// =============================================== NESTED CLASSES =============================================== // 


template <unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator = (float _fScalar)
{
	elem[N] = _fScalar;
	return *this;
}

template<unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator += (float _fScalar)
{
	elem[N] += _fScalar;
	return *this;
}

template<unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator -= (float _fScalar)
{
	elem[N] -= _fScalar;
	return *this;
}

template<unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator *= (float _fScalar)
{
	elem[N] *= _fScalar;
	return *this;
}

template<unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator /= (float _fScalar)
{
	elem[N] /= _fScalar;
	return *this;
}

template<unsigned N>
inline bool Math::Vector2::Swizzle<N>::operator == (float _fScalar)
{
	return elem[N] == _fScalar;
}

template<unsigned N>
inline bool Math::Vector2::Swizzle<N>::operator != (float _fScalar)
{
	return !(*this == _fScalar);
}

template<unsigned N>
inline _CALL Math::Vector2::Swizzle<N>::operator float&(void)
{
	return elem[N];
}

template <unsigned N>
inline _CALL Math::Vector2::Swizzle<N>::operator float(void) const
{
	return elem[N];
}

template <unsigned X, unsigned Y> template <bool>
inline Math::Vector2::Swizzle<X, Y>& _CALL Math::Vector2::Swizzle<X, Y>::operator = (const Vector2 _rhs)
{
	elem[X] = _rhs.elem[0];
	elem[Y] = _rhs.elem[1];

	return *this;
}

template <unsigned X, unsigned Y>
inline constexpr _CALL Math::Vector2::Swizzle<X, Y>::operator Math::Vector2(void) const
{
	return Vector2{ elem[X], elem[Y] };
}

template <unsigned X, unsigned Y, unsigned Z, unsigned W>
inline constexpr _CALL Math::Vector2::Swizzle<X, Y, Z, W>::operator Math::Vector4(void) const
{
	return Vector4{ elem[X], elem[Y], elem[Z], elem[W] };
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


inline float& _CALL Math::Vector2::operator[](const unsigned _nIndex)
{
	return elem[_nIndex];
}

inline const float _CALL Math::Vector2::operator[](const unsigned _nIndex) const
{
	return elem[_nIndex];
}


inline Math::Vector2& _CALL Math::Vector2::operator*=(const float _fScalar)
{
	elem[0] *= _fScalar;
	elem[1] *= _fScalar;

	return *this;
}

inline Math::Vector2&_CALL Math::Vector2::operator*=(const Vector2 _rhs)
{
	elem[0] *= _rhs.x;
	elem[1] *= _rhs.y;

	return *this;
}

inline Math::Vector2& _CALL Math::Vector2::operator/=(float _fScalar)
{
	return *this *= (1.f / _fScalar);
}

inline Math::Vector2& _CALL Math::Vector2::operator+=(const Vector2 _rhs)
{
	elem[0] += _rhs.x;
	elem[1] += _rhs.y;

	return *this;
}

inline Math::Vector2& _CALL Math::Vector2::operator-=(const Vector2 _rhs)
{
	elem[0] -= _rhs.x;
	elem[1] -= _rhs.y;

	return *this;
}

inline Math::Vector2 _CALL Math::operator-(Math::Vector2 _v)
{
	return Vector2{ -_v.x, -_v.y };
}


inline bool _CALL Math::Vector2::operator==(const Vector2 _rhs)
{
	return
		ApproxEq((*this)[0], _rhs[0]) &&
		ApproxEq((*this)[1], _rhs[1]);;
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

