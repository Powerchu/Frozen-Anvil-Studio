/* HEADER *********************************************************************************/
/*!
\file	Vector3.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	3 Vector

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATHVECTOR3_H_
#define _MATHVECTOR3_H_

#include "Globals.h"
#include "Math/MathFwd.h"
#include "Math/MathUtility.h"

#include "Utility/Meta.h"	            // EnableIf
#include "Utility/MetaAlgorithms.h"     // MetaSortV
#include "Utility/MetaDataStructures.h" // IntegralList

#include "Utility/DebugAssert.h"


namespace Math
{
#define _CALL	__vectorcall

	/*!
	\struct Vector3
	\brief
		Generic Math 3 Dimensional Space Vector.
	*/
	union _DLL_EXPORT Vector3
	{
		// ====================================== CONSTRUCTORS ======================================= // 

		inline constexpr Vector3(void) noexcept;
		inline constexpr Vector3(float x, float y, float z) noexcept;

		Vector3(Vector2 const&, float z) noexcept;


		// ==================================== VECTOR OPERATIONS ==================================== // 

		// Converts the vector to unit length
		inline Vector3& _CALL Normalise(void);

		// Computes the dot product of this vector and a given vector
		inline float _CALL Dot(Vector3 const&) const noexcept;

		// Computes the cross product of this vector and a given vector 
		inline Vector3& _CALL Cross(Vector3 const&) noexcept;

		// Projects the Vector4 onto the given vector
		inline Vector3& _CALL Project(Vector3 const&);

		inline float _CALL Magnitude(void) const noexcept;
		inline float _CALL MagnitudeSqr(void) const noexcept;

		inline Vector3& _CALL Reciprocal(void);

		template <NegateFlag>
		inline Vector3& _CALL Negate(void) noexcept;


		// ======================================== OPERATORS ======================================= // 

		inline constexpr float& _CALL operator[](const unsigned _nIndex) noexcept;
		inline constexpr float const& _CALL operator[](const unsigned _nIndex) const noexcept;

		inline Vector3  _CALL operator -  (void) const noexcept;
		inline Vector3& _CALL operator *= (const float   ) noexcept;
		inline Vector3& _CALL operator *= (Vector3 const&) noexcept;
		inline Vector3& _CALL operator /= (const float   );
		inline Vector3& _CALL operator += (Vector3 const&) noexcept;
		inline Vector3& _CALL operator -= (Vector3 const&) noexcept;

		inline bool _CALL operator == (Vector3 const&) const noexcept;
		inline bool _CALL operator != (Vector3 const& _rhs) const noexcept;

	private:
		float mData[3];

		struct _DLL_EXPORT DataMembers
		{
		protected:
			float mData[3];
		};

		template <unsigned ... Vals>
		struct _DLL_EXPORT Swizzle;

		template <unsigned N>
		struct _DLL_EXPORT Swizzle<N> : DataMembers
		{
			inline Swizzle<N>& _CALL operator = (float _rhs) noexcept;
			inline _CALL operator float& (void) const noexcept;
		};

		template <unsigned X, unsigned Y>
		struct _DLL_EXPORT Swizzle<X, Y> : DataMembers
		{
			template<bool = Ut::IsUniqueV<X, Y>::value>
			inline Swizzle<X, Y>& _CALL operator = (Vector2 const& _rhs) noexcept;

			template<>
			inline Swizzle<X, Y>& _CALL operator = <false> (Vector2 const& _rhs) noexcept
			{
				static_assert(false, "Vector3 Error: lvalue cannot be duplicate.");
				return *this;
			}

			inline constexpr _CALL operator Math::Vector2 (void) const noexcept;
		};

		template <unsigned X, unsigned Y, unsigned Z>
		struct Swizzle<X, Y, Z> : DataMembers
		{
			template<bool = Ut::IsUniqueV<X, Y, Z>::value>
			inline Swizzle& _CALL operator = (Vector3 const& _rhs) noexcept;

			template <>
			inline Swizzle& _CALL operator = <false> (Vector3 const&) noexcept
			{
				static_assert(false, "Vector3 Error: lvalue cannot be duplicate.");
				return *this;
			}

			inline _CALL operator Math::Vector3 (void) const noexcept;
		};

		template <unsigned X, unsigned Y, unsigned Z, unsigned W>
		struct _DLL_EXPORT Swizzle<X, Y, Z, W> : DataMembers
		{
			inline _CALL operator Math::Vector4 (void) const noexcept;
		};

	public:
		Swizzle<0> x;
		Swizzle<1> y;
		Swizzle<2> z;

		Swizzle<0, 0> xx; Swizzle<1, 0> yx; Swizzle<2, 0> zx;
		Swizzle<0, 1> xy; Swizzle<1, 1> yy; Swizzle<2, 1> zy;
		Swizzle<0, 2> xz; Swizzle<1, 2> yz; Swizzle<2, 2> zz;

		Swizzle<0, 0, 0> xxx; Swizzle<1, 0, 0> yxx; Swizzle<2, 0, 0> zxx;
		Swizzle<0, 0, 1> xxy; Swizzle<1, 0, 1> yxy; Swizzle<2, 0, 1> zxy;
		Swizzle<0, 0, 2> xxz; Swizzle<1, 0, 2> yxz; Swizzle<2, 0, 2> zxz;
		Swizzle<0, 1, 0> xyx; Swizzle<1, 1, 0> yyx; Swizzle<2, 1, 0> zyx;
		Swizzle<0, 1, 1> xyy; Swizzle<1, 1, 1> yyy; Swizzle<2, 1, 1> zyy;
		Swizzle<0, 1, 2> xyz; Swizzle<1, 1, 2> yyz; Swizzle<2, 1, 2> zyz;
		Swizzle<0, 2, 0> xzx; Swizzle<1, 2, 0> yzx; Swizzle<2, 2, 0> zzx;
		Swizzle<0, 2, 1> xzy; Swizzle<1, 2, 1> yzy; Swizzle<2, 2, 1> zzy;
		Swizzle<0, 2, 2> xzz; Swizzle<1, 2, 2> yzz; Swizzle<2, 2, 2> zzz;

		Swizzle<0, 0, 0, 0> xxxx; Swizzle<1, 0, 0, 0> yxxx; Swizzle<2, 0, 0, 0> zxxx;
		Swizzle<0, 0, 0, 1> xxxy; Swizzle<1, 0, 0, 1> yxxy; Swizzle<2, 0, 0, 1> zxxy;
		Swizzle<0, 0, 0, 2> xxxz; Swizzle<1, 0, 0, 2> yxxz; Swizzle<2, 0, 0, 2> zxxz;
		Swizzle<0, 0, 0, 3> xxx0; Swizzle<1, 0, 0, 3> yxx0; Swizzle<2, 0, 0, 3> zxx0;
		Swizzle<0, 0, 0, 4> xxx1; Swizzle<1, 0, 0, 4> yxx1; Swizzle<2, 0, 0, 4> zxx1;
		Swizzle<0, 0, 1, 0> xxyx; Swizzle<1, 0, 1, 0> yxyx; Swizzle<2, 0, 1, 0> zxyx;
		Swizzle<0, 0, 1, 1> xxyy; Swizzle<1, 0, 1, 1> yxyy; Swizzle<2, 0, 1, 1> zxyy;
		Swizzle<0, 0, 1, 2> xxyz; Swizzle<1, 0, 1, 2> yxyz; Swizzle<2, 0, 1, 2> zxyz;
		Swizzle<0, 0, 1, 3> xxy0; Swizzle<1, 0, 1, 3> yxy0; Swizzle<2, 0, 1, 3> zxy0;
		Swizzle<0, 0, 1, 4> xxy1; Swizzle<1, 0, 1, 4> yxy1; Swizzle<2, 0, 1, 4> zxy1;
		Swizzle<0, 0, 2, 0> xxzx; Swizzle<1, 0, 2, 0> yxzx; Swizzle<2, 0, 2, 0> zxzx;
		Swizzle<0, 0, 2, 1> xxzy; Swizzle<1, 0, 2, 1> yxzy; Swizzle<2, 0, 2, 1> zxzy;
		Swizzle<0, 0, 2, 2> xxzz; Swizzle<1, 0, 2, 2> yxzz; Swizzle<2, 0, 2, 2> zxzz;
		Swizzle<0, 0, 2, 3> xxz0; Swizzle<1, 0, 2, 3> yxz0; Swizzle<2, 0, 2, 3> zxz0;
		Swizzle<0, 0, 2, 4> xxz1; Swizzle<1, 0, 2, 4> yxz1; Swizzle<2, 0, 2, 4> zxz1;
		Swizzle<0, 1, 0, 0> xyxx; Swizzle<1, 1, 0, 0> yyxx; Swizzle<2, 1, 0, 0> zyxx;
		Swizzle<0, 1, 0, 1> xyxy; Swizzle<1, 1, 0, 1> yyxy; Swizzle<2, 1, 0, 1> zyxy;
		Swizzle<0, 1, 0, 2> xyxz; Swizzle<1, 1, 0, 2> yyxz; Swizzle<2, 1, 0, 2> zyxz;
		Swizzle<0, 1, 0, 3> xyx0; Swizzle<1, 1, 0, 3> yyx0; Swizzle<2, 1, 0, 3> zyx0;
		Swizzle<0, 1, 0, 4> xyx1; Swizzle<1, 1, 0, 4> yyx1; Swizzle<2, 1, 0, 4> zyx1;
		Swizzle<0, 1, 1, 0> xyyx; Swizzle<1, 1, 1, 0> yyyx; Swizzle<2, 1, 1, 0> zyyx;
		Swizzle<0, 1, 1, 1> xyyy; Swizzle<1, 1, 1, 1> yyyy; Swizzle<2, 1, 1, 1> zyyy;
		Swizzle<0, 1, 1, 2> xyyz; Swizzle<1, 1, 1, 2> yyyz; Swizzle<2, 1, 1, 2> zyyz;
		Swizzle<0, 1, 1, 3> xyy0; Swizzle<1, 1, 1, 3> yyy0; Swizzle<2, 1, 1, 3> zyy0;
		Swizzle<0, 1, 1, 4> xyy1; Swizzle<1, 1, 1, 4> yyy1; Swizzle<2, 1, 1, 4> zyy1;
		Swizzle<0, 1, 2, 0> xyzx; Swizzle<1, 1, 2, 0> yyzx; Swizzle<2, 1, 2, 0> zyzx;
		Swizzle<0, 1, 2, 1> xyzy; Swizzle<1, 1, 2, 1> yyzy; Swizzle<2, 1, 2, 1> zyzy;
		Swizzle<0, 1, 2, 2> xyzz; Swizzle<1, 1, 2, 2> yyzz; Swizzle<2, 1, 2, 2> zyzz;
		Swizzle<0, 1, 2, 3> xyz0; Swizzle<1, 1, 2, 3> yyz0; Swizzle<2, 1, 2, 3> zyz0;
		Swizzle<0, 1, 2, 4> xyz1; Swizzle<1, 1, 2, 4> yyz1; Swizzle<2, 1, 2, 4> zyz1;
		Swizzle<0, 2, 0, 0> xzxx; Swizzle<1, 2, 0, 0> yzxx; Swizzle<2, 2, 0, 0> zzxx;
		Swizzle<0, 2, 0, 1> xzxy; Swizzle<1, 2, 0, 1> yzxy; Swizzle<2, 2, 0, 1> zzxy;
		Swizzle<0, 2, 0, 2> xzxz; Swizzle<1, 2, 0, 2> yzxz; Swizzle<2, 2, 0, 2> zzxz;
		Swizzle<0, 2, 0, 3> xzx0; Swizzle<1, 2, 0, 3> yzx0; Swizzle<2, 2, 0, 3> zzx0;
		Swizzle<0, 2, 0, 4> xzx1; Swizzle<1, 2, 0, 4> yzx1; Swizzle<2, 2, 0, 4> zzx1;
		Swizzle<0, 2, 1, 0> xzyx; Swizzle<1, 2, 1, 0> yzyx; Swizzle<2, 2, 1, 0> zzyx;
		Swizzle<0, 2, 1, 1> xzyy; Swizzle<1, 2, 1, 1> yzyy; Swizzle<2, 2, 1, 1> zzyy;
		Swizzle<0, 2, 1, 2> xzyz; Swizzle<1, 2, 1, 2> yzyz; Swizzle<2, 2, 1, 2> zzyz;
		Swizzle<0, 2, 1, 3> xzy0; Swizzle<1, 2, 1, 3> yzy0; Swizzle<2, 2, 1, 3> zzy0;
		Swizzle<0, 2, 1, 4> xzy1; Swizzle<1, 2, 1, 4> yzy1; Swizzle<2, 2, 1, 4> zzy1;
		Swizzle<0, 2, 2, 0> xzzx; Swizzle<1, 2, 2, 0> yzzx; Swizzle<2, 2, 2, 0> zzzx;
		Swizzle<0, 2, 2, 1> xzzy; Swizzle<1, 2, 2, 1> yzzy; Swizzle<2, 2, 2, 1> zzzy;
		Swizzle<0, 2, 2, 2> xzzz; Swizzle<1, 2, 2, 2> yzzz; Swizzle<2, 2, 2, 2> zzzz;
		Swizzle<0, 2, 2, 3> xzz0; Swizzle<1, 2, 2, 3> yzz0; Swizzle<2, 2, 2, 3> zzz0;
		Swizzle<0, 2, 2, 4> xzz1; Swizzle<1, 2, 2, 4> yzz1; Swizzle<2, 2, 2, 4> zzz1;
	};

	// Converts a vector into unit length
	inline Vector3 _CALL Normalise(Vector3 const&);

	// Computes the dot product of two vectors
	inline float _CALL Dot(Vector3 const&, Vector3 const&) noexcept;

	// Computes the cross product of two vectors
	inline Vector3 _CALL Cross(Vector3 const&, Vector3 const&) noexcept;

	// Projects lhs onto rhs
	inline Vector3 _CALL Project(Vector3 const&, Vector3 const&);

	inline Vector3 _CALL Reciprocal(Vector3 const&);

	template <unsigned FLAGS>
	inline Vector3 _CALL Negate(Vector3 const&) noexcept;


	// ====================================== MATH UTILITY ======================================= // 
	// Manually overload the math utility functions which cannot be called for type Vector4

	inline Vector3 _CALL Abs(Vector3 const&) noexcept;

	inline Vector3 _CALL Min(Vector3 const&, Vector3 const&) noexcept;
	inline Vector3 _CALL Max(Vector3 const&, Vector3 const&) noexcept;


	// ==================================== VECTOR GENERATORS ==================================== // 

	inline Vector3 _CALL MakePoint2D (float _x, float _y) noexcept;
	inline Vector3 _CALL MakeVector2D(float _x, float _y) noexcept;


	// ======================================== OPERATORS ======================================== // 

	inline Vector3 _CALL operator - (Vector3 const&, Vector3 const&) noexcept;
	inline Vector3 _CALL operator + (Vector3 const&, Vector3 const&) noexcept;
	inline Vector3 _CALL operator * (Vector3 const&, Vector3 const&) noexcept;
	inline Vector3 _CALL operator * (float const   , Vector3 const&) noexcept;
	inline Vector3 _CALL operator * (Vector3 const&, float const   ) noexcept;
	inline Vector3 _CALL operator / (Vector3 const&, float const   );


	using Vec3    = Vector3;
	using Pt2D    = Vector3;
	using Vec2D   = Vector3;
	using Point2D = Vector3;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline constexpr Math::Vector3::Vector3(void) noexcept
	: mData{}
{

}

inline constexpr Math::Vector3::Vector3(float x, float y, float z) noexcept
	: mData{ x, y, z }
{

}


inline Math::Vector3& _CALL Math::Vector3::Normalise(void)
{
	DEBUG_ASSERT(IsZero(Dot(*this)), "Vector4 Error: Normalising a zero vector.\n");

	return *this /= Magnitude();
}

inline Math::Vector3 _CALL Math::Normalise(Vector3 const& _v)
{
	return Vector3( _v ).Normalise();
}


inline Math::Vector3& _CALL Math::Vector3::Reciprocal(void)
{
	mData[0] = 1.f / mData[0];
	mData[1] = 1.f / mData[1];
	mData[2] = 1.f / mData[2];

	return *this;
}

inline Math::Vector3 _CALL Math::Reciprocal(Vector3 const& _v)
{
	return Vector3( _v ).Reciprocal();
}


inline float _CALL Math::Vector3::MagnitudeSqr(void) const noexcept
{
	return Dot(*this);
}

inline float _CALL Math::Vector3::Dot(Vector3 const& _rhs) const noexcept
{
	return mData[0] * _rhs[0] + mData[1] * _rhs[1] + mData[2] * _rhs[2];
}

// Computes the dot product of two vectors
inline float _CALL Math::Dot(Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return _lhs.Dot(_rhs);
}

inline Math::Vector3& _CALL Math::Vector3::Cross(Vector3 const& _rhs) noexcept
{
	return *this = Math::Cross(*this, _rhs);
}

inline Math::Vector3 _CALL Math::Cross(Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3{
		_lhs[1] * _rhs[2] - _lhs[2] * _rhs[1],
		_lhs[2] * _rhs[0] - _lhs[0] * _rhs[2],
		_lhs[0] * _rhs[1] - _lhs[1] * _rhs[0],
	};
}

inline Math::Vector3& _CALL Math::Vector3::Project(Vector3 const& _rhs)
{
	return *this = Math::Project(*this, _rhs);
}

// Projects lhs onto rhs
inline Math::Vector3 _CALL Math::Project(Vector3 const& _lhs, Vector3 const& _rhs)
{
	DEBUG_ASSERT(IsZero(_rhs.MagnitudeSqr()), "Vector3 Error: Projection onto zero vector.\n");

	float scale = Dot(_lhs, _rhs) / _rhs.MagnitudeSqr();
	return scale * _rhs;
}


inline Math::Vector3 _CALL Math::Abs(Vector3 const& _v) noexcept
{
	return Vector3{ Math::Abs(_v[0]), Math::Abs(_v[1]), Math::Abs(_v[2]) };
}

/*!
\brief
	Piece-wise Min of two Vector3

\return Vector3
	Returns a Vector3 where
	x = Min( lhs.x, rhs.x ), y = Min( lhs.y, rhs.y ) ...
	etc.
*/
inline Math::Vector3 _CALL Math::Min(Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3{
		Math::Min(_lhs[0], _rhs[0]),
		Math::Min(_lhs[1], _rhs[1]),
		Math::Min(_lhs[2], _rhs[2])
	};
}

/*!
\brief
	Piece-wise Max of two Vector3

\return Vector3
	Returns a Vector3 where
	x = Max( lhs.x, rhs.x ), y = Max( lhs.y, rhs.y ) ...
	etc.
*/
inline Math::Vector3 _CALL Math::Max(Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3{
		Math::Max(_lhs[0], _rhs[0]),
		Math::Max(_lhs[1], _rhs[1]),
		Math::Max(_lhs[2], _rhs[2])
	};
}


template <Math::NegateFlag FLAGS>
inline Math::Vector3& _CALL Math::Vector3::Negate(void) noexcept
{
	if constexpr (static_cast<char>(FLAGS & NegateFlag::X))
		mData[0] = -mData[0];

	if constexpr (static_cast<char>(FLAGS & NegateFlag::Y))
		mData[1] = -mData[1];

	if constexpr (static_cast<char>(FLAGS & NegateFlag::Z))
		mData[2] = -mData[2];

	static_assert(!static_cast<char>(FLAGS & NegateFlag::W),
		"Vector3 does not have W component!");

	return *this;
}

template <unsigned FLAGS>
inline Math::Vector3 _CALL Math::Negate(Vector3 const& _v) noexcept
{
	return Vector3{ _v }.Negate<FLAGS>();
}



// ============================================= VECTOR GENERATORS ============================================== // 


inline Math::Point2D _CALL Math::MakePoint2D(float _x, float _y) noexcept
{
	return Vector3{ _x, _y, 1.f };
}

inline Math::Vec2D _CALL Math::MakeVector2D(float _x, float _y) noexcept
{
	return Vector3{ _x, _y, .0f };
}



// =============================================== NESTED CLASSES =============================================== // 


template <unsigned N>
inline Math::Vector3::Swizzle<N>& _CALL Math::Vector3::Swizzle<N>::operator = (float _rhs) noexcept
{
	mData[N] = _rhs;
	return *this;
}

template <unsigned N>
inline _CALL Math::Vector3::Swizzle<N>::operator float& (void) const noexcept
{
	return mData[N];
}


template <unsigned X, unsigned Y, unsigned Z> template <bool>
inline Math::Vector3::Swizzle<X, Y, Z>& _CALL Math::Vector3::Swizzle<X, Y, Z>::operator = (Math::Vector3 const& _rhs) noexcept
{
	mData[X] = _rhs[0];
	mData[Y] = _rhs[1];
	mData[Z] = _rhs[2];

	return *this;
}

template<unsigned X, unsigned Y, unsigned Z>
inline _CALL Math::Vector3::Swizzle<X, Y, Z>::operator Math::Vector3 (void) const noexcept
{
	return Vector3{ mData[X], mData[Y], mData[Z] };
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


inline constexpr float& _CALL Math::Vector3::operator [] (const unsigned _nIndex) noexcept
{
	return const_cast<float&>(const_cast<Vector3 const&>(*this)[_nIndex]);
}

inline constexpr float const& _CALL Math::Vector3::operator [] (const unsigned _nIndex) const noexcept
{
	DEBUG_ASSERT(_nIndex > 3, "Vector3 Error: Index out of range.");
	return mData[_nIndex];
}


inline Math::Vector3& _CALL Math::Vector3::operator *= (const float _fScalar) noexcept
{
	mData[0] *= _fScalar;
	mData[1] *= _fScalar;
	mData[2] *= _fScalar;

	return *this;
}

inline Math::Vector3& _CALL Math::Vector3::operator *= (Vector3 const& _rhs) noexcept
{
	mData[0] *= _rhs[0];
	mData[1] *= _rhs[1];
	mData[2] *= _rhs[2];
	return *this;
}

inline Math::Vector3& _CALL Math::Vector3::operator /= (const float _fScalar)
{
	DEBUG_ASSERT(IsZero(_fScalar), "Vector3 Error: Division by zero!\n");
	return *this *= 1.f / _fScalar;
}

inline Math::Vector3& _CALL Math::Vector3::operator += (Vector3 const& _rhs) noexcept
{
	mData[0] += _rhs[0];
	mData[1] += _rhs[1];
	mData[2] += _rhs[2];
	return *this;
}

inline Math::Vector3& _CALL Math::Vector3::operator -= (Vector3 const& _rhs) noexcept
{
	mData[0] -= _rhs[0];
	mData[1] -= _rhs[1];
	mData[2] -= _rhs[2];
	return *this;
}


inline Math::Vector3 _CALL Math::Vector3::operator - (void) const noexcept
{
	return *this * -1;
}

inline Math::Vector3 _CALL Math::operator-(Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3( _lhs ) -= _rhs;
}

inline Math::Vector3 _CALL Math::operator + (Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3( _lhs ) += _rhs;
}

inline Math::Vector3 _CALL Math::operator * (Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3( _lhs ) *= _rhs;
}

inline Math::Vector3 _CALL Math::operator * (float _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3( _rhs ) *= _lhs;
}

inline Math::Vector3 _CALL Math::operator * (Vector3 const& _lhs, float _rhs) noexcept
{
	return Vector3( _lhs ) *= _rhs;
}

inline Math::Vector3 _CALL Math::operator / (Vector3 const& _lhs, float _rhs)
{
	return Vector3( _lhs ) /= _rhs;
}

inline bool _CALL Math::Vector3::operator == (Math::Vector3 const& _rhs) const noexcept
{
	return
		ApproxEq((*this)[0], _rhs[0]) &&
		ApproxEq((*this)[1], _rhs[1]) &&
		ApproxEq((*this)[2], _rhs[2]);
}

inline bool _CALL Math::Vector3::operator != (Math::Vector3 const& _rhs) const noexcept
{
	return !(*this == _rhs);
}

inline std::istream& operator >> (std::istream& _lhs, Math::Vector3& _rhs)
{
	_lhs >> _rhs[0];
	_lhs.ignore(1) >> _rhs[1];
	_lhs.ignore(1) >> _rhs[2];

	return _lhs;
}

inline std::ostream& operator << (std::ostream& _lhs, Math::Vector3 const& _rhs)
{
	_lhs << _rhs[0] << "," << _rhs[1] << "," << _rhs[2];

	return _lhs;
}



// Remove all our defines
#ifdef _CALL
#undef _CALL
#endif



#endif		// INCLUDE GUARD

