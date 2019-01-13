/* HEADER *********************************************************************************/
/*!
\file	Vector4.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Experimental
	Seems to be slower on member access but faster on function calls
	All the inlining means that everything has to be in the header file

	Inform me if you need any utility functions

	Reference: https://software.intel.com/sites/landingpage/IntrinsicsGuide

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include "Globals.h"
#include "Math/MathFwd.h"
#include "Math/MathUtility.h"

#include "Utility/Meta.h"	            // EnableIf
#include "Utility/MetaAlgorithms.h"     // MetaSortV
#include "Utility/MetaDataStructures.h" // IntegralList

#if defined(DEBUG) | defined(_DEBUG)
#include "Utility/DebugAssert.h"
#endif // Debug only includes

#if !defined(_WIN64)	// We need these for win32 - pending fix in auto array
#include <new>		    // nothrow_t
#include <exception>    // bad_alloc
#endif

#include <iostream>
#include <xmmintrin.h>  // SSE
#include <emmintrin.h>  // SSE 2
#include <tmmintrin.h>  // SSE 3
#include <smmintrin.h>  // SSE 4.1
#include <immintrin.h>  // FMA

namespace Math
{
	#define _CALL	__vectorcall
	#define ALLIGN	16
	#define USE_DP	1

	struct Matrix4;

	enum class NegateFlag : unsigned char
	{
		X = 1 << 0,
		Y = 1 << 1,
		Z = 1 << 2,
		W = 1 << 3,

		XY = X | Y,
		XZ = X | Z,
		XW = X | W,
		YZ = Y | Z,
		YW = Y | W,
		ZW = Z | W,

		XYZ = XY | Z,
		XYW = XY | W,
		XZW = XZ | W,
		YZW = YZ | Z,

		XYZW = XY | ZW
	};

	/*!
	\struct Vector4
	\brief
		Generic Math 4 Dimensional Space Vector. 
	*/
	union alignas(ALLIGN) _DLL_EXPORT Vector4
	{
		// ====================================== CONSTRUCTORS ======================================= // 

		inline Vector4(void) noexcept;
		inline Vector4 (const Vector4&) noexcept;
		inline Vector4(float x, float y, float z, float w = 0) noexcept;
		inline explicit Vector4(__m128) noexcept;


		// ==================================== VECTOR OPERATIONS ==================================== // 

		// Converts the vector to unit length
		inline Vector4& _CALL Normalise(void);

		// Computes the dot product of this vector and a given vector
		inline float _CALL Dot(const Vector4) const;

		// Computes the outer product of two vectors
		Matrix4 _CALL Outer(const Vector4) const;

		// Computes the cross product of this vector and a given vector 
		inline Vector4& _CALL Cross(const Vector4);

		// Projects the Vector4 onto the given vector
		inline Vector4& _CALL Project(const Vector4);

		inline float _CALL Magnitude(void) const;
		inline float _CALL MagnitudeSqr(void) const;

		inline Vector4& _CALL Reciprocal(void);

		template <NegateFlag>
		inline Vector4& _CALL Negate(void) noexcept;


		// ======================================== OPERATORS ======================================= // 

		// Potentially Slow?, please try to avoid!
		inline float& _CALL operator[](const unsigned _nIndex);
		inline const float _CALL operator[](const unsigned _nIndex) const;

		inline Vector4  _CALL operator-(void) const;
		inline Vector4& _CALL operator*=(const float);
		inline Vector4& _CALL operator*=(const Vector4);
		inline Vector4& _CALL operator/=(const float);
		inline Vector4& _CALL operator+=(const Vector4);
		inline Vector4& _CALL operator-=(const Vector4);

		inline bool _CALL operator==(const Vector4&) const;
		inline bool _CALL operator!=(const Vector4& _rhs) const;

		// Alternate + and -
		// x1 - x2 , y1 + y2 , z1 - z2 , w1 + w2
		inline Vector4& _CALL AddSub(const Vector4);

		// x1 + y1 , z1 + w1 , x2 + y2 , z2 + w2
		inline Vector4& _CALL HorizontalAdd(const Vector4);
		// x1 - y1 , z1 - w1 , x2 - y2 , z2 - w2
		inline Vector4& _CALL HorizontalSub(const Vector4);

		// this = u * v + this
		inline Vector4& _CALL MultiplyAdd(const Vector4 u, const Vector4 v);

		// this = this - u * v
		inline Vector4& _CALL MultiplySub(const Vector4 u, const Vector4 v);

#if !defined(_WIN64)	// We need these for win32 - pending fix in auto array

		//inline void* operator new (std::size_t);
		//inline void* operator new (std::size_t, const std::nothrow_t&) noexcept;
		//inline void* operator new (std::size_t, void*) noexcept;

		//inline void* operator new[] (std::size_t);
		//inline void* operator new[] (std::size_t, const std::nothrow_t&) noexcept;
		//inline void* operator new[] (std::size_t, void*) noexcept;

		//inline void operator delete (void*) noexcept;
		//inline void operator delete (void*, const std::nothrow_t&) noexcept;
		//inline void operator delete (void*, void*) noexcept;

		//inline void operator delete[] (void*) noexcept;
		//inline void operator delete[] (void*, const std::nothrow_t&) noexcept;
		//inline void operator delete[] (void*, void*) noexcept;

#endif	// ! WIN64

	private:
		__m128 mData;

		template <unsigned ... Vals>
		struct _DLL_EXPORT SwizzleMask;

		template <unsigned N>
		struct _DLL_EXPORT SwizzleMask<N>
		{
			inline SwizzleMask<N>& _CALL operator= (float _rhs);
			inline _CALL operator float(void) const;

		private:
			__m128 mData;

			static constexpr unsigned shuffleMask = _MM_SHUFFLE(N == 3 ? 0 : 3, N == 2 ? 0 : 2, N == 1 ? 0 : 1, N);
		};

		template <unsigned X, unsigned Y, unsigned Z, unsigned W>
		struct IsLvalueSwizzle
		{
			static constexpr bool value = (X != Y) && (X != Z) && (X != W) && (Y != Z) && (Y != W) && (Z != W);
		};
		
		// While we're at it, might as well add swizzle masks
		// SwizzleMask  -> 1 shuffle (read)
		// SwizzleMask -> 1 shuffle (read & write)
		template <unsigned X, unsigned Y, unsigned Z, unsigned W>
		struct SwizzleMask<X, Y, Z, W>
		{
			SwizzleMask() = default;
			SwizzleMask(__m128 _data) : mData{ _data } {}

			template<bool = IsLvalueSwizzle<X, Y, Z, W>::value>
			inline SwizzleMask& _CALL operator = (Vector4 _rhs);

			template <>
			inline SwizzleMask& _CALL operator = <false> (Vector4)
			{
				static_assert(false, "Vector4 Error: lvalue cannot be duplicate.");
				return *this;
			}

			template<unsigned Q, unsigned R, unsigned S, unsigned T, typename ret_t = Ut::EnableIf_t<IsLvalueSwizzle<X, Y, Z, W>::value, SwizzleMask>>
			inline ret_t& _CALL operator = (SwizzleMask<Q, R, S, T> _rhs);

			inline __m128 _CALL GetRaw(void) const noexcept;
			inline __m128 _CALL _GetRawUnshuf(void) const noexcept;

			inline _CALL operator Math::Vector4 (void) const;

		private:
			__m128 mData;

			static constexpr unsigned shuffleRead = _MM_SHUFFLE(W, Z, Y, X);
			static constexpr unsigned shuffleWrite = (3 << (2*W)) | (2 << (2*Z)) | (1 << (2*Y)) | (0 << (2*X));
		};

		static inline __m128 _CALL InvSqrt(__m128);
		static inline __m128 _CALL Dot(__m128, __m128);

		// Matrix needs the __m128 member
		friend struct Math::Matrix4;

	public:
		SwizzleMask<0> x;
		SwizzleMask<1> y;
		SwizzleMask<2> z;
		SwizzleMask<3> w;

		SwizzleMask<0, 0, 0, 0> xxxx;
		SwizzleMask<0, 0, 1, 1> xxyy;
		SwizzleMask<0, 0, 1, 2> xxyz;
		SwizzleMask<0, 0, 2, 2> xxzz;
		SwizzleMask<0, 1, 0, 1> xyxy;
		SwizzleMask<0, 1, 1, 1> xyyy;
		SwizzleMask<0, 1, 2, 3> xyzw;
		SwizzleMask<0, 1, 3, 2> xywz;
		SwizzleMask<0, 2, 1, 1> xzyy;
		SwizzleMask<0, 2, 1, 3> xzyw;
		SwizzleMask<0, 3, 0, 3> xwxw;
		SwizzleMask<0, 3, 1, 2> xwyz;
		SwizzleMask<0, 3, 2, 3> xwzw;
		SwizzleMask<1, 0, 1, 0> yxyx;
		SwizzleMask<1, 0, 2, 3> yxzw;
		SwizzleMask<1, 0, 3, 2> yxwz;
		SwizzleMask<1, 1, 1, 1> yyyy;
		SwizzleMask<1, 3, 0, 2> ywxz;
		SwizzleMask<1, 3, 1, 3> ywyw;
		SwizzleMask<1, 3, 2, 0> ywzx;
		SwizzleMask<2, 0, 1, 3> zxyw;
		SwizzleMask<2, 0, 3, 1> zxwy;
		SwizzleMask<2, 1, 2, 1> zyzy;
		SwizzleMask<2, 2, 2, 3> zzzw;
		SwizzleMask<2, 2, 2, 2> zzzz;
		SwizzleMask<2, 3, 0, 0> zwxx;
		SwizzleMask<2, 3, 0, 1> zwxy;
		SwizzleMask<2, 3, 1, 0> zwyx;
		SwizzleMask<2, 3, 2, 3> zwzw;
		SwizzleMask<2, 3, 3, 2> zwwz;
		SwizzleMask<3, 1, 2, 0> wyzx;
		SwizzleMask<3, 2, 0, 1> wzxy;
		SwizzleMask<3, 2, 1, 0> wzyx;
		SwizzleMask<3, 2, 2, 3> wzzw;
		SwizzleMask<3, 2, 3, 2> wzwz;
		SwizzleMask<3, 3, 3, 3> wwww;

		inline __m128 _CALL GetRaw(void) const noexcept;
	};

	// Converts a vector into unit length
	inline Vector4 _CALL Normalise(const Vector4);

	// Computes the dot product of two vectors
	inline float _CALL Dot(const Vector4, const Vector4);

	// Computes the outer product of two vectors
	Matrix4 _CALL Outer(const Vector4, const Vector4);

	// Computes the cross product of two vectors
	inline Vector4 _CALL Cross(const Vector4, const Vector4);

	// Projects lhs onto rhs
	inline Vector4 _CALL Project(const Vector4, const Vector4);

	inline Vector4 _CALL Reciprocal(const Vector4);

	template <unsigned FLAGS>
	inline Vector4 _CALL Negate(const Vector4) noexcept;


	// ====================================== MATH UTILITY ======================================= // 
	// Manually overload the math utility functions which cannot be called for type Vector4

	inline Vector4 _CALL Abs(const Vector4);

	inline Vector4 _CALL Min(const Vector4, const Vector4);
	inline Vector4 _CALL Max(const Vector4, const Vector4);


	// ==================================== VECTOR GENERATORS ==================================== // 

	inline Vector4 _CALL MakePoint3D(float _x, float _y, float _z);
	inline Vector4 _CALL MakeVector3D(float _x, float _y, float _z);


	// ======================================== OPERATORS ======================================== // 

	inline Vector4 _CALL operator-(const Vector4, const Vector4);
	inline Vector4 _CALL operator+(const Vector4, const Vector4);
	inline Vector4 _CALL operator*(const Vector4, const Vector4);
	inline Vector4 _CALL operator*(const float, const Vector4);
	inline Vector4 _CALL operator*(const Vector4, const float);
	inline Vector4 _CALL operator/(const Vector4, const float);

	// Alternate + and -
	// x1 + x2 , y1 - y2 , z1 + z2 , w1 - w2
	inline Vector4 _CALL AddSub(const Vector4, const Vector4);

	// x1 + y1 , z1 + w1 , x2 + y2 , z2 + w2
	inline Vector4 _CALL HorizontalAdd(const Vector4, const Vector4);
	// x1 - y1 , z1 - w1 , x2 - y2 , z2 - w2
	inline Vector4 _CALL HorizontalSub(const Vector4, const Vector4);


	using Vec4		= Vector4;
	using Vector3D	= Vector4;
	using Vec3D		= Vector4;
	using Point3D	= Vector4;
	using Pt3D		= Vector4;
	using Color		= Vector4;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline Math::Vector4::Vector4(void) noexcept
	: mData( _mm_setzero_ps() )
{

}

inline Math::Vector4::Vector4(const Vector4& v) noexcept 
	: mData( v.mData )
{

}

inline Math::Vector4::Vector4(float x, float y, float z, float w) noexcept 
	: mData( _mm_set_ps(w, z, y, x) )
{

}

inline Math::Vector4::Vector4(__m128 v) noexcept
	: mData( v )
{

}

inline __m128 _CALL Math::Vector4::GetRaw(void) const noexcept
{
	return mData;
}

inline Math::Vector4& _CALL Math::Vector4::Normalise(void)
{
#if defined(DEBUG) | defined(_DEBUG)
	//DEBUG_ASSERT(IsZero(Dot(*this)), "Vector4 Error: Normalising a zero vector.\n");
#endif

	__m128 invSqrt = InvSqrt(Dot(mData, mData));
	mData = _mm_mul_ps(invSqrt, mData);

	return *this;
}

inline Math::Vector4 _CALL Math::Normalise(Vector4 _vec)
{
	return _vec.Normalise();
}

inline Math::Vector4& _CALL Math::Vector4::Reciprocal(void)
{
	__m128 temp = _mm_rcp_ps(mData);
	__m128 const mask = _mm_or_ps(
		_mm_cmplt_ps(mData, _mm_set_ps1(-epsilon)),
		_mm_cmpgt_ps(mData, _mm_set_ps1(epsilon))
	);
	temp = _mm_and_ps(temp, mask);

	mData = _mm_mul_ps(mData, _mm_mul_ps(temp, temp));
	mData = _mm_sub_ps(_mm_add_ps(temp, temp), mData);

	return *this;
}

inline Math::Vector4 _CALL Math::Reciprocal(Vector4 _v)
{
	return _v.Reciprocal();
}

template <Math::NegateFlag FLAGS>
inline Math::Vector4& _CALL Math::Vector4::Negate(void) noexcept
{
	static const __m128i Negator = _mm_set_epi32(
		static_cast<unsigned char>(FLAGS) & 0x8 ? 0x80000000 : 0,
		static_cast<unsigned char>(FLAGS) & 0x4 ? 0x80000000 : 0,
		static_cast<unsigned char>(FLAGS) & 0x2 ? 0x80000000 : 0,
		static_cast<unsigned char>(FLAGS) & 0x1 ? 0x80000000 : 0
	);

	mData = _mm_xor_ps(mData, _mm_castsi128_ps(Negator));

	return *this;
}

template <unsigned FLAGS>
inline Math::Vector4 _CALL Math::Negate(Vector4 _v) noexcept
{
	return v.Negate<FLAGS>();
}

inline float _CALL Math::Vector4::Dot(const Vector4 _rhs) const
{
	return _mm_cvtss_f32(Dot(mData, _rhs.mData));
}

// Computes the dot product of two vectors
inline float _CALL Math::Dot(const Vector4 _lhs, const Vector4 _rhs)
{
	return _lhs.Dot(_rhs);
}

inline Math::Vector4& _CALL Math::Vector4::Cross(const Vector4 _rhs)
{
	// x = (a.y * b.z) - (a.z * b.y)
	// y = (a.z * b.x) - (a.x * b.z)
	// z = (a.x * b.y) - (a.y * b.x)
	// w = (a.w * b.w) - (a.w * b.w)

	return *this = ((zxyw * _rhs) - (*this * _rhs.zxyw)).zxyw;
}

inline Math::Vector4 _CALL Math::Cross(Vector4 _lhs, Vector4 _rhs)
{
	return _lhs.Cross(_rhs);
}

inline Math::Vector4& _CALL Math::Vector4::Project(const Vector4 _rhs)
{
#if defined(DEBUG) | defined(_DEBUG)
	//DEBUG_ASSERT(IsZero(_rhs.Dot(_rhs)), "Vector4 Error: Projection onto zero vector.\n");
#endif

	Vector4 dotB{ Dot(_rhs.mData, _rhs.mData) };
	*this = Vector4 { Dot(mData, _rhs.mData) };

	return *this *= dotB.Reciprocal() * _rhs;
}

// Projects lhs onto rhs
inline Math::Vector4 _CALL Math::Project(Vector4 _lhs, Vector4 _rhs)
{
	return _lhs.Project(_rhs);
}


inline float _CALL Math::Vector4::MagnitudeSqr(void) const
{
	return Dot(*this);
}


inline Math::Vector4 _CALL Math::Abs(const Vector4 _v)
{
	__m128i signBits = _mm_set1_epi32(0x7FFFFFFF); // Set all the bits except for the sign bit to 1

	// Bitwise AND operator
	return Vector4{ _mm_and_ps(_v.GetRaw(), _mm_castsi128_ps(signBits)) };
}

/*!
\brief
	Piece-wise Min of two Vector4

\return Vector4
	Returns a Vector4 where 
	x = Min( lhs.x, rhs.x ), y = Min( lhs.y, rhs.y ) ... 
	etc.
*/
inline Math::Vector4 _CALL Math::Min(const Vector4 _lhs, const Vector4 _rhs)
{
	return Vector4{ _mm_min_ps(_lhs.GetRaw(), _rhs.GetRaw()) };
}

/*!
\brief
	Piece-wise Max of two Vector4

\return Vector4
	Returns a Vector4 where 
	x = Max( lhs.x, rhs.x ), y = Max( lhs.y, rhs.y ) ... 
	etc.
*/
inline Math::Vector4 _CALL Math::Max(const Vector4 _lhs, const Vector4 _rhs)
{
	return Vector4{ _mm_max_ps(_lhs.GetRaw(), _rhs.GetRaw()) };
}

// Helper function to reduce approx. error on rsqrt
inline __m128  _CALL Math::Vector4::InvSqrt(__m128 _v)
{
	static const __m128 three = _mm_set_ps1(3.f);
	static const __m128 half  = _mm_set_ps1(.5f);

	// 1 iteration newton's method
	__m128 temp  = _mm_rsqrt_ps(_v);
	__m128 iter  = _mm_mul_ps(_mm_mul_ps(temp, temp), _v);

	temp = _mm_mul_ps(half, temp);
	iter = _mm_sub_ps(three, iter);

	return _mm_mul_ps(temp, iter);
}

// Internal function for returning the dot product as a Vector4 type
inline __m128 _CALL Math::Vector4::Dot(__m128 _lhs, __m128 _rhs)
{
#if USE_DP & defined(_INCLUDED_SMM)	// SSE 4.1

	// 0x71 -> High (F) Multiply and sum all components
	//      -> Low  (F) Store result of sum into all components
	return _mm_dp_ps(_lhs, _rhs, 0xFF);

#elif defined(_INCLUDED_PMM)		// SSE 3

	__m128 temp = _mm_mul_ps(_lhs, _rhs);
	temp = _mm_hadd_ps(temp, temp);
	return _mm_hadd_ps(temp, temp);

#else								// FALL BACK

	Vector4 temp = _lhs * _rhs;
	Vector4 shuf = temp + temp.zwxy;
	return (shuf + shuf.yxwz).GetRaw();
	/*
	__m128 temp = _mm_mul_ps(_lhs, _rhs);
	__m128 shuf = _mm_add_ps(temp, _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(2, 3, 0, 1)));
	return _mm_add_ps(shuf, _mm_shuffle_ps(shuf, shuf, _MM_SHUFFLE(1, 0, 3, 2)));
	*/
#endif
}



// ============================================= VECTOR GENERATORS ============================================== // 


inline Math::Point3D _CALL Math::MakePoint3D(float _x, float _y, float _z)
{
	return Vector4{ _x, _y, _z, 1.f };
}

inline Math::Vector4 _CALL Math::MakeVector3D(float _x, float _y, float _z)
{
	return Vector4{ _x, _y, _z, .0f };
}



// =============================================== NESTED CLASSES =============================================== // 


template <unsigned N>
inline Math::Vector4::SwizzleMask<N>& _CALL Math::Vector4::SwizzleMask<N>::operator= (float _rhs)
{
#if defined(_INCLUDED_SMM)	// SSE4.1 supported

	// Hopefully faster
	mData = _mm_insert_ps(mData, _mm_set_ss(_rhs), N << 4);

#else						// No SSE 4.1

	// There's probably a better way but I can't think of one
	// Shuffle the affected subset to low bits
	// move assigned float into it and shuffle it back
	mData = _mm_shuffle_ps(mData, mData, shuffleMask);
	mData = _mm_move_ss(mData, _mm_set_ss(_rhs));
	mData = _mm_shuffle_ps(mData, mData, shuffleMask);

#endif

	return *this;
}

template<>
inline Math::Vector4::SwizzleMask<0>& _CALL Math::Vector4::SwizzleMask<0>::operator= (float _rhs)
{
	mData = _mm_move_ss(mData, _mm_set_ss(_rhs));

	return *this;
}

template <unsigned N>
inline _CALL Math::Vector4::SwizzleMask<N>::operator float(void) const
{
	// Copy the wanted value everywhere, and return the first 32 bits as a float
	return _mm_cvtss_f32(_mm_shuffle_ps(mData, mData, _MM_SHUFFLE(N, N, N, N)));
}

template<>
inline _CALL Math::Vector4::SwizzleMask<0>::operator float(void) const
{
	// We happen to want the value that's already the first
	// so we can skip the shuffle and just return it
	// - Compiler seems to do this special case optimisation even without this
	//   But I'll leave it here anyway
	return _mm_cvtss_f32(mData);
}



template<unsigned X, unsigned Y, unsigned Z, unsigned W> template<bool>
inline Math::Vector4::SwizzleMask<X, Y, Z, W>& _CALL Math::Vector4::SwizzleMask<X, Y, Z, W>::operator = (Math::Vector4 _rhs)
{
	mData = _mm_shuffle_ps(_rhs.mData, _rhs.mData, shuffleWrite);
	return *this;
}

template<> template<>
inline Math::Vector4::SwizzleMask<0, 1, 2, 3>& _CALL Math::Vector4::SwizzleMask<0, 1, 2, 3>::operator = <true> (Math::Vector4 _rhs)
{
	mData = _rhs.mData;
	return *this;
}

namespace
{
	template <typename, typename>
	struct SwizzleCompare;

	template <typename T, T lhs, T rhs, T ... ls, T ... rs>
	struct SwizzleCompare<Ut::IntegralList<T, lhs, ls...>, Ut::IntegralList<T, rhs, rs...>>
	{
		static constexpr bool value = lhs < rhs;
	};
}

template<unsigned X, unsigned Y, unsigned Z, unsigned W> template<unsigned Q, unsigned R, unsigned S, unsigned T, typename ret_t>
inline ret_t& _CALL Math::Vector4::SwizzleMask<X, Y, Z, W>::operator = (SwizzleMask<Q, R, S, T> _rhs)
{
	using ReSwizzle_t = Ut::MetaSortT_t <SwizzleCompare, 
		Ut::Collection<
			Ut::IntegralList<unsigned, X, Q>, 
			Ut::IntegralList<unsigned, Y, R>,
			Ut::IntegralList<unsigned, Z, S>,
			Ut::IntegralList<unsigned, W, T>
		>
	>;

	using Result_t = Ut::MetaExtract_t< 1,
		Ut::MetaExtract_t<0, ReSwizzle_t>,
		Ut::MetaExtract_t<1, ReSwizzle_t>,
		Ut::MetaExtract_t<2, ReSwizzle_t>,
		Ut::MetaExtract_t<3, ReSwizzle_t>
	>;

	using NewSwizzle_t = Math::Vector4::SwizzleMask <
		Ut::MetaExtractV<0, Result_t>::value,
		Ut::MetaExtractV<1, Result_t>::value,
		Ut::MetaExtractV<2, Result_t>::value,
		Ut::MetaExtractV<3, Result_t>::value
	>;

	mData = NewSwizzle_t { _rhs._GetRawUnshuf() }.GetRaw();
	return *this;
}

template<unsigned X, unsigned Y, unsigned Z, unsigned W>
inline _CALL Math::Vector4::SwizzleMask<X, Y, Z, W>::operator Math::Vector4(void) const
{
	return Vector4{ GetRaw() };
}

template <unsigned X, unsigned Y, unsigned Z, unsigned W>
inline __m128 _CALL Math::Vector4::SwizzleMask<X, Y, Z, W>::GetRaw(void) const noexcept
{
	return _mm_shuffle_ps(mData, mData, shuffleRead);
}

template<unsigned X, unsigned Y, unsigned Z, unsigned W>
inline __m128 _CALL Math::Vector4::SwizzleMask<X, Y, Z, W>::_GetRawUnshuf(void) const noexcept
{
	return mData;
}

template<>
inline __m128 _CALL Math::Vector4::SwizzleMask<0, 1, 2, 3>::GetRaw(void) const noexcept
{
	return mData;
}

// No automatic compiler instruction change for the following special shuffles
// I guess since intel guide says it's the same
// Although there seems to be a consensus that these are faster than shuffle
// Perhaps on older CPUs and moot on newer ones

template<>
inline __m128 _CALL Math::Vector4::SwizzleMask<0, 0, 1, 1>::GetRaw(void) const noexcept
{
	return _mm_unpacklo_ps(mData, mData);
}

template<>
inline __m128 _CALL Math::Vector4::SwizzleMask<2, 2, 3, 3>::GetRaw(void) const noexcept
{
	return _mm_unpackhi_ps(mData, mData);
}

template<>
inline __m128 _CALL Math::Vector4::SwizzleMask<0, 1, 0, 1>::GetRaw(void) const noexcept
{
	return _mm_movelh_ps(mData, mData);
}

template<>
inline __m128 _CALL Math::Vector4::SwizzleMask<2, 3, 2, 3>::GetRaw(void) const noexcept
{
	return _mm_movelh_ps(mData, mData);
}

#if defined(_INCLUDED_PMM)		// SSE 3

template<>
inline __m128 _CALL Math::Vector4::SwizzleMask<0, 0, 2, 2>::GetRaw(void) const noexcept
{
	return _mm_moveldup_ps(mData);
}

template<>
inline __m128 _CALL Math::Vector4::SwizzleMask<1, 1, 3, 3>::GetRaw(void) const noexcept
{
	return _mm_movehdup_ps(mData);
}

#endif							// SSE 3



// ============================================ OPERATOR OVERLOADING ============================================ // 


inline float& _CALL Math::Vector4::operator[] (const unsigned _nIndex)
{
#if defined(DEBUG) | defined(_DEBUG)
	//DEBUG_ASSERT(_nIndex > 3, "Vector4 Error: Array index out of range.");
#endif

	// Visual Studio compiler only?
	return mData.m128_f32[_nIndex];
}

inline const float _CALL Math::Vector4::operator[] (const unsigned _nIndex) const
{
#if defined(DEBUG) | defined(_DEBUG)
	//DEBUG_ASSERT(_nIndex > 3, "Vector4 Error: Array index out of range.");
#endif

	// Visual Studio compiler only?
	return mData.m128_f32[_nIndex];
}


inline Math::Vector4& _CALL Math::Vector4::AddSub(const Vector4 _rhs)
{
	mData = _mm_addsub_ps(mData, _rhs.mData);
	return *this;
}

inline Math::Vector4 _CALL Math::AddSub(Vector4 _lhs, const Vector4 _rhs)
{
	return _lhs.AddSub(_rhs);
}

inline Math::Vector4& _CALL Math::Vector4::HorizontalAdd(const Vector4 _rhs)
{
	mData = _mm_hadd_ps(mData, _rhs.mData);
	return *this;
}

inline Math::Vector4 _CALL Math::HorizontalAdd(Vector4 _lhs, const Vector4 _rhs)
{
	return _lhs.HorizontalAdd(_rhs);
}

inline Math::Vector4&_CALL Math::Vector4::HorizontalSub(const Vector4 _rhs)
{
	mData = _mm_hsub_ps(mData, _rhs.mData);
	return *this;
}

inline Math::Vector4 _CALL Math::HorizontalSub(Vector4 _lhs, const Vector4 _rhs)
{
	return _lhs.HorizontalSub(_rhs);
}

inline Math::Vector4& _CALL Math::Vector4::MultiplyAdd(const Vector4 u, const Vector4 v)
{
#if defined(_INCLUDED_IMM)

	mData = _mm_fmadd_ps(u.mData, v.mData, mData);
	return *this;

#else

	return *this += u * v;

#endif
}

inline Math::Vector4& _CALL Math::Vector4::MultiplySub(const Vector4 u, const Vector4 v)
{
#if defined(_INCLUDED_IMM)

	mData = _mm_fnmadd_ps(u.mData, v.mData, mData);
	return *this;

#else

	return *this -= u * v;

#endif
}


inline Math::Vector4& _CALL Math::Vector4::operator*=(const float _fScalar)
{
	mData = _mm_mul_ps(mData, _mm_set_ps1(_fScalar));
	return *this;
}

inline Math::Vector4& _CALL Math::Vector4::operator*=(const Vector4 _rhs)
{
	mData = _mm_mul_ps(mData, _rhs.mData);
	return *this;
}

inline Math::Vector4& _CALL Math::Vector4::operator/=(const float _fScalar)
{
	mData = _mm_mul_ps(mData, _mm_set_ps1(1.f / _fScalar));
	return *this;
}

inline Math::Vector4& _CALL Math::Vector4::operator+=(const Vector4 _rhs)
{
	mData = _mm_add_ps(mData, _rhs.mData);
	return *this;
}

inline Math::Vector4& _CALL Math::Vector4::operator-=(const Vector4 _rhs)
{
	mData = _mm_sub_ps(mData, _rhs.mData);
	return *this;
}


inline Math::Vector4 _CALL Math::Vector4::operator-(void) const
{
	static __m128i signBits = _mm_set1_epi32(0x80000000);

	// Flip all sign bits
	return Vector4{ _mm_xor_ps(mData, _mm_castsi128_ps(signBits)) };
}

inline Math::Vector4 _CALL Math::operator-(Vector4 _lhs, const Vector4 _rhs)
{
	return _lhs -= _rhs;
}

inline Math::Vector4 _CALL Math::operator+(Vector4 _lhs, const Vector4 _rhs)
{
	return _lhs += _rhs;
}

inline Math::Vector4 _CALL Math::operator*(Vector4 _lhs, const Vector4 _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Vector4 _CALL Math::operator*(const float _lhs, Vector4 _rhs)
{
	return _rhs *= _lhs;
}

inline Math::Vector4 _CALL Math::operator*(Vector4 _lhs, const float _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Vector4 _CALL Math::operator/(Vector4 _lhs, const float _rhs)
{
	return _lhs /= _rhs;
}

inline bool _CALL Math::Vector4::operator==(const Math::Vector4& _rhs) const
{
	return
		ApproxEq((*this)[0], _rhs[0]) &&
		ApproxEq((*this)[1], _rhs[1]) &&
		ApproxEq((*this)[2], _rhs[2]) &&
		ApproxEq((*this)[3], _rhs[3]);
}

inline bool _CALL Math::Vector4::operator!=(const Math::Vector4& _rhs) const
{
	return !(*this == _rhs);
}

inline std::istream& operator >> (std::istream& _lhs, Math::Vector4& _rhs)
{
	_lhs >> _rhs[0];
	_lhs.ignore(1) >> _rhs[1];
	_lhs.ignore(1) >> _rhs[2];
	_lhs.ignore(1) >> _rhs[3];

	return _lhs;
}

inline std::ostream& operator << (std::ostream& _lhs, const Math::Vector4& _rhs)
{
	_lhs << _rhs[0] << "," << _rhs[1] << "," << _rhs[2] << "," << _rhs[3];

	return _lhs;
}

//inline void* Math::Vector4::operator new (std::size_t _sz)
//{
//	void* ret = _aligned_malloc(_sz, ALLIGN);
//
//	if (ret)
//		return ret;
//	
//	throw std::bad_alloc{};
//}
//
//inline void* Math::Vector4::operator new (std::size_t _sz, const std::nothrow_t&) noexcept
//{
//	return _aligned_malloc(_sz, ALLIGN);
//}
//
//inline void* Math::Vector4::operator new (std::size_t _sz, void* _ptr) noexcept
//{
//	return ::operator new(_sz, _ptr);
//}
//
//inline void* Math::Vector4::operator new[](std::size_t _sz)
//{
//	void* ret = _aligned_malloc(_sz, ALLIGN);
//
//	if (ret)
//		return ret;
//
//	throw std::bad_alloc{};
//}
//
//inline void* Math::Vector4::operator new[](std::size_t _sz, const std::nothrow_t&) noexcept
//{
//	return _aligned_malloc(_sz, ALLIGN);
//}
//
//inline void* Math::Vector4::operator new[](std::size_t _sz, void* _ptr) noexcept
//{
//	return ::operator new[](_sz, _ptr);
//}
//
//inline void  Math::Vector4::operator delete (void* _ptr) noexcept
//{
//	_aligned_free(_ptr);
//}
//
//inline void  Math::Vector4::operator delete (void* _ptr, const std::nothrow_t&) noexcept
//{
//	delete static_cast<Vector4*>(_ptr);
//}
//
//inline void  Math::Vector4::operator delete (void*, void*) noexcept
//{
//	return;
//}
//
//inline void  Math::Vector4::operator delete[](void* _ptr) noexcept
//{
//	_aligned_free(_ptr);
//}
//
//inline void  Math::Vector4::operator delete[](void* _ptr, const std::nothrow_t&) noexcept
//{
//	delete[] static_cast<Vector4*>(_ptr);
//}
//
//inline void  Math::Vector4::operator delete[](void*, void*) noexcept
//{
//	return;
//}



// Remove all our defines
#ifdef USE_DP
#undef USE_DP
#endif
#ifdef ALLIGN
#undef ALLIGN
#endif
#ifdef _CALL
#undef _CALL
#endif



#endif		// INCLUDE GUARD

