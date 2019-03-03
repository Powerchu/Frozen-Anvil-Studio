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
#ifndef _MATHVECTOR2_H_
#define _MATHVECTOR2_H_

#include "Utility/DebugAssert.h"

#include "../MathFwd.h"
#include "../MathUtility.h"

#include "IO/Serialiser.h"


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
		inline constexpr Vector2(float x, float y) noexcept;
		inline constexpr explicit Vector2(float xy) noexcept;


		// ==================================== VECTOR OPERATIONS ==================================== // 

		// Converts the vector to unit length
		inline Vector2& _CALL Normalise(void);

		// Computes the dot product of this vector and a given vector
		inline float _CALL Dot(const Vector2) const;

		// Computes the cross product of this vector and a given vector 
		inline float _CALL Cross(const Vector2);

		// Projects the Vector4 onto the given vector
		inline Vector2& _CALL Project(const Vector2);

		       float _CALL Magnitude   (void) const noexcept;
		inline float _CALL MagnitudeSqr(void) const noexcept;

		// ======================================== OPERATORS ======================================= // 

		inline constexpr float&       _CALL operator[](const unsigned _nIndex)       noexcept;
		inline constexpr float const& _CALL operator[](const unsigned _nIndex) const noexcept;

		inline Vector2& _CALL operator /= (float);
		inline Vector2& _CALL operator *= (float)          noexcept;
		inline Vector2& _CALL operator *= (Vector2 const&) noexcept;
		inline Vector2& _CALL operator += (Vector2 const&) noexcept;
		inline Vector2& _CALL operator -= (Vector2 const&) noexcept;

		inline bool _CALL operator == (Vector2 const&) const noexcept;
		inline bool _CALL operator != (Vector2 const&) const noexcept;

	private:

		float mData[2];

		struct _DLL_EXPORT DataMembers
		{
		protected:
			float mData[2];
		};

		template <unsigned ...Sz>
		struct Swizzle;

		template <unsigned N>
		struct _DLL_EXPORT Swizzle<N> : DataMembers
		{
			inline Swizzle<N>& operator =  (float) noexcept;
			inline Swizzle<N>& operator += (float) noexcept;
			inline Swizzle<N>& operator -= (float) noexcept;
			inline Swizzle<N>& operator *= (float) noexcept;
			inline Swizzle<N>& operator /= (float);

			inline _CALL operator float& (void);
			inline _CALL operator float const& (void) const;
		};

		template <unsigned X, unsigned Y>
		struct Swizzle<X, Y> : DataMembers
		{
			inline constexpr _CALL operator Math::Vector2 (void) const;

			template <bool = X != Y>
			inline Swizzle<X, Y>& _CALL operator = (Vector2 const&) noexcept;

			template <>
			inline Swizzle<X, Y>& _CALL operator = <false> (Vector2 const&) noexcept
			{
				static_assert(false, "Vector2 Error: lvalue cannot be duplicate.");
				return *this;
			}
		};

		template <unsigned X, unsigned Y, unsigned Z>
		struct Swizzle<X, Y, Z> : DataMembers
		{
			inline _CALL operator Math::Vector3 (void) const noexcept;
		};

		template <unsigned X, unsigned Y, unsigned Z, unsigned W>
		struct Swizzle<X, Y, Z, W> : DataMembers
		{
			inline _CALL operator Math::Vector4 (void) const noexcept;
		};


	public:

		Swizzle<0> x;
		Swizzle<1> y;

		Swizzle<0, 0> xx;
		Swizzle<0, 1> xy;
		Swizzle<1, 0> yx;
		Swizzle<1, 1> yy;

		Swizzle<0, 0, 0> xxx; Swizzle<1, 0, 0> yxx;
		Swizzle<0, 0, 1> xxy; Swizzle<1, 0, 1> yxy;
		Swizzle<0, 1, 0> xyx; Swizzle<1, 1, 0> yyx;
		Swizzle<0, 1, 1> xyy; Swizzle<1, 1, 1> yyy;

		Swizzle<0, 0, 0, 0> xxxx; Swizzle<1, 0, 0, 0> yxxx;
		Swizzle<0, 0, 0, 1> xxxy; Swizzle<1, 0, 0, 1> yxxy;
		Swizzle<0, 0, 1, 0> xxyx; Swizzle<1, 0, 1, 0> yxyx;
		Swizzle<0, 0, 1, 1> xxyy; Swizzle<1, 0, 1, 1> yxyy;
		Swizzle<0, 1, 0, 0> xyxx; Swizzle<1, 1, 0, 0> yyxx;
		Swizzle<0, 1, 0, 1> xyxy; Swizzle<1, 1, 0, 1> yyxy;
		Swizzle<0, 1, 1, 0> xyyx; Swizzle<1, 1, 1, 0> yyyx;
		Swizzle<0, 1, 1, 1> xyyy; Swizzle<1, 1, 1, 1> yyyy;
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

	inline Vector2 _CALL Abs(Vector2 const&);

	inline Vector2 _CALL Min(Vector2 const&, Vector2 const&);
	inline Vector2 _CALL Max(Vector2 const&, Vector2 const&);


	// ======================================== OPERATORS ======================================== // 

	inline Vector2 _CALL operator - (Vector2 const&) noexcept;
	inline Vector2 _CALL operator - (Vector2 const&, Vector2 const&) noexcept;
	inline Vector2 _CALL operator + (Vector2 const&, Vector2 const&) noexcept;
	inline Vector2 _CALL operator * (Vector2 const&, Vector2 const&) noexcept;
	inline Vector2 _CALL operator * (float, Vector2 const&) noexcept;
	inline Vector2 _CALL operator * (Vector2 const&, float) noexcept;
	inline Vector2 _CALL operator / (Vector2 const&, float);

	using Vec2 = Vector2;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline constexpr Math::Vector2::Vector2(void) noexcept
	: mData { 0 }
{

}

inline constexpr Math::Vector2::Vector2(float _xy) noexcept
	: mData{ _xy, _xy }
{

}

inline constexpr Math::Vector2::Vector2(float _x, float _y) noexcept
	: mData{ _x, _y }
{

}

inline Math::Vector2& _CALL Math::Vector2::Normalise(void)
{
	DEBUG_ASSERT(IsZero(Dot(*this)), "Vector2 Error: Normalising a zero vector.\n");

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
	DEBUG_ASSERT(IsZero(_rhs.Dot(_rhs)), "Vector2 Error: Projection onto zero vector.\n");

	float mag = Dot(*this) / _rhs.Dot(_rhs);

	return *this = mag * _rhs;
}

// Projects lhs onto rhs
inline Math::Vector2 _CALL Math::Project(Vector2 _lhs, Vector2 _rhs)
{
	return _lhs.Project(_rhs);
}


inline float _CALL Math::Vector2::MagnitudeSqr(void) const noexcept
{
	return Dot(*this);
}


inline Math::Vector2 _CALL Math::Abs(Vector2 const& _v)
{
	return Vector2{ Math::Abs<float>(_v.x), Math::Abs<float>(_v.y) };
}

inline Math::Vector2 _CALL Math::Min(Vector2 const& _lhs, Vector2 const& _rhs)
{
	return Vector2{ Min(_lhs[0], _rhs[0]), Min(_lhs[1], _rhs[1]) };
}

inline Math::Vector2 _CALL Math::Max(Vector2 const& _lhs, Vector2 const& _rhs)
{
	return Vector2{ Max(_lhs[0], _rhs[0]), Max(_lhs[1], _rhs[1]) };
}



// =============================================== NESTED CLASSES =============================================== // 


template <unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator = (float _fScalar) noexcept
{
	mData[N] = _fScalar;
	return *this;
}

template<unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator += (float _fScalar) noexcept
{
	mData[N] += _fScalar;
	return *this;
}

template<unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator -= (float _fScalar) noexcept
{
	mData[N] -= _fScalar;
	return *this;
}

template<unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator *= (float _fScalar) noexcept
{
	mData[N] *= _fScalar;
	return *this;
}

template<unsigned N>
inline Math::Vector2::Swizzle<N>& Math::Vector2::Swizzle<N>::operator /= (float _fScalar)
{
	mData[N] /= _fScalar;
	return *this;
}


template<unsigned N>
inline bool operator == (Math::Vector2::Swizzle<N> const& _v, float _fScalar)
{
	return static_cast<float>(_v) == _fScalar;
}

template<unsigned N>
inline bool operator == (float _fScalar, Math::Vector2::Swizzle<N> const& _v)
{
	return _v == _fScalar;
}

template<unsigned N>
inline bool operator != (Math::Vector2::Swizzle<N> const& _v, float _fScalar) noexcept
{
	return !(_v == _fScalar);
}

template<unsigned N>
inline bool operator != (float _fScalar, Math::Vector2::Swizzle<N> const& _v)
{
	return _v != _fScalar;
}


template<unsigned N>
inline _CALL Math::Vector2::Swizzle<N>::operator float& (void)
{
	return const_cast<float&>(operator float const&());
}

template <unsigned N>
inline _CALL Math::Vector2::Swizzle<N>::operator float const& (void) const
{
	return mData[N];
}

template <unsigned X, unsigned Y> template <bool>
inline Math::Vector2::Swizzle<X, Y>& _CALL Math::Vector2::Swizzle<X, Y>::operator = (Vector2 const& _rhs) noexcept
{
	mData[X] = _rhs.mData[0];
	mData[Y] = _rhs.mData[1];

	return *this;
}

template <unsigned X, unsigned Y>
inline constexpr _CALL Math::Vector2::Swizzle<X, Y>::operator Math::Vector2 (void) const
{
	return Vector2{ mData[X], mData[Y] };
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


inline constexpr float& _CALL Math::Vector2::operator [] (unsigned const _nIndex) noexcept
{
	return const_cast<float&>(const_cast<Vector2 const&>(*this)[_nIndex]);
}

inline constexpr float const& _CALL Math::Vector2::operator [] (unsigned const _nIndex) const noexcept
{
	return mData[_nIndex];
}


inline Math::Vector2& _CALL Math::Vector2::operator *= (float _fScalar) noexcept
{
	mData[0] *= _fScalar;
	mData[1] *= _fScalar;

	return *this;
}

inline Math::Vector2&_CALL Math::Vector2::operator *= (Vector2 const& _rhs) noexcept
{
	mData[0] *= _rhs.x;
	mData[1] *= _rhs.y;

	return *this;
}

inline Math::Vector2& _CALL Math::Vector2::operator /= (float _fScalar)
{
	DEBUG_ASSERT(IsZero(_fScalar), "Vector2 Error: Attempted divide by zero!");
	return *this *= (1.f / _fScalar);
}

inline Math::Vector2& _CALL Math::Vector2::operator += (Vector2 const& _rhs) noexcept
{
	mData[0] += _rhs.x;
	mData[1] += _rhs.y;

	return *this;
}

inline Math::Vector2& _CALL Math::Vector2::operator -= (Vector2 const& _rhs) noexcept
{
	mData[0] -= _rhs.x;
	mData[1] -= _rhs.y;

	return *this;
}

inline Math::Vector2 _CALL Math::operator - (Math::Vector2 const& _v) noexcept
{
	return Vector2( -_v.x, -_v.y );
}


inline bool _CALL Math::Vector2::operator == (Vector2 const& _rhs) const noexcept
{
	return
		Math::ApproxEq((*this)[0], _rhs[0]) &&
		Math::ApproxEq((*this)[1], _rhs[1]);
}

inline bool _CALL Math::Vector2::operator != (Vector2 const& _rhs) const noexcept
{
	return !(*this == _rhs);
}


inline Math::Vector2 _CALL Math::operator - (Vector2 const& _lhs, Vector2 const& _rhs) noexcept
{
	return Vector2( _lhs ) -= _rhs;
}

inline Math::Vector2 _CALL Math::operator + (Vector2 const& _lhs, Vector2 const& _rhs) noexcept
{
	return Vector2( _lhs ) += _rhs;
}

inline Math::Vector2 _CALL Math::operator * (Vector2 const& _lhs, Vector2 const& _rhs) noexcept
{
	return Vector2( _lhs ) *= _rhs;
}

inline Math::Vector2 _CALL Math::operator * (float _lhs, Vector2 const& _rhs) noexcept
{
	return Vector2( _rhs ) *= _lhs;
}

inline Math::Vector2 _CALL Math::operator * (Vector2 const& _lhs, float _rhs) noexcept
{
	return Vector2( _lhs ) *= _rhs;
}

inline Math::Vector2 _CALL Math::operator / (Vector2 const& _lhs, float _rhs)
{
	return Vector2( _lhs ) /= _rhs;
}

inline std::istream& operator >> (std::istream& _lhs, Math::Vector2& _rhs)
{
	_lhs >> _rhs.x;
	_lhs.ignore(1) >> _rhs.y;

	return _lhs;
}

inline std::ostream& operator << (std::ostream& _lhs, const Math::Vector2& _rhs)
{
	_lhs << _rhs[0] << "," << _rhs[1];

	return _lhs;
}


// Remove all our defines
#ifdef _CALL
#undef _CALL
#endif



#endif		// INCLUDE GUARD

