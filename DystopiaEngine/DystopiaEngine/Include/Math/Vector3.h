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
#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include "Globals.h"
#include "Math/MathFwd.h"
#include "Math/MathUtility.h"

#include "Utility/Meta.h"	            // EnableIf
#include "Utility/MetaAlgorithms.h"     // MetaSortV
#include "Utility/MetaDataStructures.h" // IntegralList

#if defined(DEBUG) | defined(_DEBUG)
#include "Utility/DebugAssert.h"
#endif // Debug only includes


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

		inline Vector3(void) noexcept;
		inline Vector3(Vector3 const&) noexcept;
		inline Vector3(float x, float y, float z) noexcept;


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

		template <unsigned ... Vals>
		struct _DLL_EXPORT SwizzleMask;

		template <unsigned N>
		struct _DLL_EXPORT SwizzleMask<N>
		{
			inline SwizzleMask<N>& _CALL operator = (float _rhs) noexcept;
			inline _CALL operator float& (void) const noexcept;

		private:
			float mData[3];
		};
		template <unsigned X, unsigned Y>
		struct _DLL_EXPORT SwizzleMask<X, Y>
		{
			inline SwizzleMask<Y>& _CALL operator= (Vector2 _rhs);
			inline _CALL operator Math::Vector2 (void) const;

		private:
			float mData[3];
		};

		template <unsigned X, unsigned Y, unsigned Z>
		struct IsLvalueSwizzle
		{
			static constexpr bool value = (X != Y) && (X != Z) && (Y != Z);
		};

		template <unsigned X, unsigned Y, unsigned Z>
		struct SwizzleMask<X, Y, Z>
		{
			template<bool = IsLvalueSwizzle<X, Y, Z>::value>
			inline SwizzleMask& _CALL operator = (Vector3 const& _rhs) noexcept;

			template <>
			inline SwizzleMask& _CALL operator = <false> (Vector3 const&) noexcept
			{
				static_assert(false, "Vector3 Error: lvalue cannot be duplicate.");
				return *this;
			}

			inline _CALL operator Math::Vector3 (void) const noexcept;

		private:
			float mData[3];
		};

		template <unsigned X, unsigned Y, unsigned Z, unsigned W>
		struct _DLL_EXPORT SwizzleMask<X, Y, Z, W>
		{
			inline SwizzleMask<Y>& _CALL operator= (Vector4 _rhs);
			inline _CALL operator Math::Vector4 (void) const;

		private:
			float mData[3];
		};

	public:
		SwizzleMask<0> x;
		SwizzleMask<1> y;
		SwizzleMask<2> z;

		SwizzleMask<0, 0, 0> xxx; SwizzleMask<1, 0, 0> yxx; SwizzleMask<2, 0, 0> zxx;
		SwizzleMask<0, 0, 1> xxy; SwizzleMask<1, 0, 1> yxy; SwizzleMask<2, 0, 1> zxy;
		SwizzleMask<0, 0, 2> xxz; SwizzleMask<1, 0, 2> yxz; SwizzleMask<2, 0, 2> zxz;
		SwizzleMask<0, 1, 0> xyx; SwizzleMask<1, 1, 0> yyx; SwizzleMask<2, 1, 0> zyx;
		SwizzleMask<0, 1, 1> xyy; SwizzleMask<1, 1, 1> yyy; SwizzleMask<2, 1, 1> zyy;
		SwizzleMask<0, 1, 2> xyz; SwizzleMask<1, 1, 2> yyz; SwizzleMask<2, 1, 2> zyz;
		SwizzleMask<0, 2, 0> xzx; SwizzleMask<1, 2, 0> yzx; SwizzleMask<2, 2, 0> zzx;
		SwizzleMask<0, 2, 1> xzy; SwizzleMask<1, 2, 1> yzy; SwizzleMask<2, 2, 1> zzy;
		SwizzleMask<0, 2, 2> xzz; SwizzleMask<1, 2, 2> yzz; SwizzleMask<2, 2, 2> zzz;

		SwizzleMask<0, 0, 0, 0> xxxx; SwizzleMask<1, 0, 0, 0> yxxx; SwizzleMask<2, 0, 0, 0> zxxx;
		SwizzleMask<0, 0, 0, 1> xxxy; SwizzleMask<1, 0, 0, 1> yxxy; SwizzleMask<2, 0, 0, 1> zxxy;
		SwizzleMask<0, 0, 0, 2> xxxz; SwizzleMask<1, 0, 0, 2> yxxz; SwizzleMask<2, 0, 0, 2> zxxz;
		SwizzleMask<0, 0, 0, 3> xxx0; SwizzleMask<1, 0, 0, 3> yxx0; SwizzleMask<2, 0, 0, 3> zxx0;
		SwizzleMask<0, 0, 0, 4> xxx1; SwizzleMask<1, 0, 0, 4> yxx1; SwizzleMask<2, 0, 0, 4> zxx1;
		SwizzleMask<0, 0, 1, 0> xxyx; SwizzleMask<1, 0, 1, 0> yxyx; SwizzleMask<2, 0, 1, 0> zxyx;
		SwizzleMask<0, 0, 1, 1> xxyy; SwizzleMask<1, 0, 1, 1> yxyy; SwizzleMask<2, 0, 1, 1> zxyy;
		SwizzleMask<0, 0, 1, 2> xxyz; SwizzleMask<1, 0, 1, 2> yxyz; SwizzleMask<2, 0, 1, 2> zxyz;
		SwizzleMask<0, 0, 0, 3> xxy0; SwizzleMask<1, 0, 0, 3> yxy0; SwizzleMask<2, 0, 0, 3> zxy0;
		SwizzleMask<0, 0, 0, 4> xxy1; SwizzleMask<1, 0, 0, 4> yxy1; SwizzleMask<2, 0, 0, 4> zxy1;
		SwizzleMask<0, 0, 2, 0> xxzx; SwizzleMask<1, 0, 2, 0> yxzx; SwizzleMask<2, 0, 2, 0> zxzx;
		SwizzleMask<0, 0, 2, 1> xxzy; SwizzleMask<1, 0, 2, 1> yxzy; SwizzleMask<2, 0, 2, 1> zxzy;
		SwizzleMask<0, 0, 2, 2> xxzz; SwizzleMask<1, 0, 2, 2> yxzz; SwizzleMask<2, 0, 2, 2> zxzz;
		SwizzleMask<0, 0, 0, 3> xxz0; SwizzleMask<1, 0, 0, 3> yxz0; SwizzleMask<2, 0, 0, 3> zxz0;
		SwizzleMask<0, 0, 0, 4> xxz1; SwizzleMask<1, 0, 0, 4> yxz1; SwizzleMask<2, 0, 0, 4> zxz1;
		SwizzleMask<0, 1, 0, 0> xyxx; SwizzleMask<1, 1, 0, 0> yyxx; SwizzleMask<2, 1, 0, 0> zyxx;
		SwizzleMask<0, 1, 0, 1> xyxy; SwizzleMask<1, 1, 0, 1> yyxy; SwizzleMask<2, 1, 0, 1> zyxy;
		SwizzleMask<0, 1, 0, 2> xyxz; SwizzleMask<1, 1, 0, 2> yyxz; SwizzleMask<2, 1, 0, 2> zyxz;
		SwizzleMask<0, 1, 0, 3> xyx0; SwizzleMask<1, 1, 0, 3> yyx0; SwizzleMask<2, 1, 0, 3> zyx0;
		SwizzleMask<0, 1, 0, 4> xyx1; SwizzleMask<1, 1, 0, 4> yyx1; SwizzleMask<2, 1, 0, 4> zyx1;
		SwizzleMask<0, 1, 1, 0> xyyx; SwizzleMask<1, 1, 1, 0> yyyx; SwizzleMask<2, 1, 1, 0> zyyx;
		SwizzleMask<0, 1, 1, 1> xyyy; SwizzleMask<1, 1, 1, 1> yyyy; SwizzleMask<2, 1, 1, 1> zyyy;
		SwizzleMask<0, 1, 1, 2> xyyz; SwizzleMask<1, 1, 1, 2> yyyz; SwizzleMask<2, 1, 1, 2> zyyz;
		SwizzleMask<0, 1, 0, 3> xyy0; SwizzleMask<1, 1, 0, 3> yyy0; SwizzleMask<2, 1, 0, 3> zyy0;
		SwizzleMask<0, 1, 0, 4> xyy1; SwizzleMask<1, 1, 0, 4> yyy1; SwizzleMask<2, 1, 0, 4> zyy1;
		SwizzleMask<0, 1, 2, 0> xyzx; SwizzleMask<1, 1, 2, 0> yyzx; SwizzleMask<2, 1, 2, 0> zyzx;
		SwizzleMask<0, 1, 2, 1> xyzy; SwizzleMask<1, 1, 2, 1> yyzy; SwizzleMask<2, 1, 2, 1> zyzy;
		SwizzleMask<0, 1, 2, 2> xyzz; SwizzleMask<1, 1, 2, 2> yyzz; SwizzleMask<2, 1, 2, 2> zyzz;
		SwizzleMask<0, 1, 0, 3> xyz0; SwizzleMask<1, 1, 0, 3> yyz0; SwizzleMask<2, 1, 0, 3> zyz0;
		SwizzleMask<0, 1, 0, 4> xyz1; SwizzleMask<1, 1, 0, 4> yyz1; SwizzleMask<2, 1, 0, 4> zyz1;
		SwizzleMask<0, 2, 0, 0> xzxx; SwizzleMask<1, 2, 0, 0> yzxx; SwizzleMask<2, 2, 0, 0> zzxx;
		SwizzleMask<0, 2, 0, 1> xzxy; SwizzleMask<1, 2, 0, 1> yzxy; SwizzleMask<2, 2, 0, 1> zzxy;
		SwizzleMask<0, 2, 0, 2> xzxz; SwizzleMask<1, 2, 0, 2> yzxz; SwizzleMask<2, 2, 0, 2> zzxz;
		SwizzleMask<0, 2, 0, 3> xzx0; SwizzleMask<1, 2, 0, 3> yzx0; SwizzleMask<2, 2, 0, 3> zzx0;
		SwizzleMask<0, 2, 0, 4> xzx1; SwizzleMask<1, 2, 0, 4> yzx1; SwizzleMask<2, 2, 0, 4> zzx1;
		SwizzleMask<0, 2, 1, 0> xzyx; SwizzleMask<1, 2, 1, 0> yzyx; SwizzleMask<2, 2, 1, 0> zzyx;
		SwizzleMask<0, 2, 1, 1> xzyy; SwizzleMask<1, 2, 1, 1> yzyy; SwizzleMask<2, 2, 1, 1> zzyy;
		SwizzleMask<0, 2, 1, 2> xzyz; SwizzleMask<1, 2, 1, 2> yzyz; SwizzleMask<2, 2, 1, 2> zzyz;
		SwizzleMask<0, 2, 0, 3> xzy0; SwizzleMask<1, 2, 0, 3> yzy0; SwizzleMask<2, 2, 0, 3> zzy0;
		SwizzleMask<0, 2, 0, 4> xzy1; SwizzleMask<1, 2, 0, 4> yzy1; SwizzleMask<2, 2, 0, 4> zzy1;
		SwizzleMask<0, 2, 2, 0> xzzx; SwizzleMask<1, 2, 2, 0> yzzx; SwizzleMask<2, 2, 2, 0> zzzx;
		SwizzleMask<0, 2, 2, 1> xzzy; SwizzleMask<1, 2, 2, 1> yzzy; SwizzleMask<2, 2, 2, 1> zzzy;
		SwizzleMask<0, 2, 2, 2> xzzz; SwizzleMask<1, 2, 2, 2> yzzz; SwizzleMask<2, 2, 2, 2> zzzz;
		SwizzleMask<0, 2, 0, 3> xzz0; SwizzleMask<1, 2, 0, 3> yzz0; SwizzleMask<2, 2, 0, 3> zzz0;
		SwizzleMask<0, 2, 0, 4> xzz1; SwizzleMask<1, 2, 0, 4> yzz1; SwizzleMask<2, 2, 0, 4> zzz1;
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


inline Math::Vector3::Vector3(void) noexcept
	: mData{}
{

}

inline Math::Vector3::Vector3(const Vector3& _v) noexcept
	: mData{ _v[0], _v[1], _v[2] }
{

}

inline Math::Vector3::Vector3(float x, float y, float z) noexcept
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
	return Vector3{ _v }.Normalise();
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
	return Vector3{ _v }.Reciprocal();
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


inline float _CALL Math::Vector3::MagnitudeSqr(void) const noexcept
{
	return Dot(*this);
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
inline Math::Vector3::SwizzleMask<N>& _CALL Math::Vector3::SwizzleMask<N>::operator= (float _rhs) noexcept
{
	mData[N] = _rhs;
	return *this;
}

template <unsigned N>
inline _CALL Math::Vector3::SwizzleMask<N>::operator float& (void) const noexcept
{
	return mData[N];
}


template <unsigned X, unsigned Y, unsigned Z> template <bool>
inline Math::Vector3::SwizzleMask<X, Y, Z>& _CALL Math::Vector3::SwizzleMask<X, Y, Z>::operator = (Math::Vector3 const& _rhs) noexcept
{
	mData[X] = _rhs[0];
	mData[Y] = _rhs[1];
	mData[Z] = _rhs[2];

	return *this;
}

template<unsigned X, unsigned Y, unsigned Z>
inline _CALL Math::Vector3::SwizzleMask<X, Y, Z>::operator Math::Vector3 (void) const noexcept
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
	DEBUG_ASSERT(_nIndex > 2, "Vector4 Error: Index out of range.");
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
	return Vector3{ _lhs } -= _rhs;
}

inline Math::Vector3 _CALL Math::operator + (Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3{ _lhs } += _rhs;
}

inline Math::Vector3 _CALL Math::operator * (Vector3 const& _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3{ _lhs } *= _rhs;
}

inline Math::Vector3 _CALL Math::operator * (float _lhs, Vector3 const& _rhs) noexcept
{
	return Vector3{ _rhs } *= _lhs;
}

inline Math::Vector3 _CALL Math::operator * (Vector3 const& _lhs, float _rhs) noexcept
{
	return Vector3{ _lhs } *= _rhs;
}

inline Math::Vector3 _CALL Math::operator / (Vector3 const& _lhs, float _rhs)
{
	return Vector3{ _lhs } /= _rhs;
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

