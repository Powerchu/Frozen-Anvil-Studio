/* HEADER *********************************************************************************/
/*!
\file	Vector4.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Math 4 Vector
	Inform me if you need any utility functions

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATHVECTOR4_H_
#define _MATHVECTOR4_H_

#include "Globals.h"
#include "Math/MathFwd.h"
#include "Math/MathUtility.h"

#include "Utility/Meta.h"	            // EnableIf
#include "Utility/MetaAlgorithms.h"     // MetaSortV
#include "Utility/MetaDataStructures.h" // IntegralList

#include "Utility/DebugAssert.h"

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

	/*!
	\struct Vector4
	\brief
		Generic Math 4 Dimensional Space Vector. 
	*/
	union alignas(ALLIGN) _DLL_EXPORT Vector4
	{
		// ====================================== CONSTRUCTORS ======================================= // 

		inline Vector4(void) noexcept;
		inline Vector4(float x, float y, float z, float w = 0) noexcept;
		inline explicit Vector4(__m128) noexcept;

		Vector4(Vector3 const&, float w) noexcept;
		Vector4(Vector2 const&, float z, float w) noexcept;


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

		inline Vector4  _CALL operator -  (void) const;
		inline Vector4& _CALL operator *= (const float);
		inline Vector4& _CALL operator *= (const Vector4);
		inline Vector4& _CALL operator /= (const float);
		inline Vector4& _CALL operator += (const Vector4);
		inline Vector4& _CALL operator -= (const Vector4);

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

	private:
		__m128 mData;

		struct _DLL_EXPORT DataMember
		{
			explicit DataMember(__m128 v) noexcept : mData(v) {};

		protected:
			__m128 mData;
		};

		template <unsigned ... Vals>
		struct _DLL_EXPORT Swizzle;

		template <unsigned N>
		struct _DLL_EXPORT Swizzle<N> : DataMember
		{
			inline Swizzle<N>& _CALL operator= (float _rhs);
			inline _CALL operator float(void) const;

		private:
			static constexpr unsigned shuffleMask = _MM_SHUFFLE(N == 3 ? 0 : 3, N == 2 ? 0 : 2, N == 1 ? 0 : 1, N);
		};

		template <unsigned X, unsigned Y>
		struct _DLL_EXPORT Swizzle<X, Y> : DataMember
		{
			template<bool = Ut::IsUniqueV<X, Y>::value>
			inline Swizzle<X, Y>& _CALL operator = (Vector2 _rhs) noexcept;

			template <>
			inline Swizzle& _CALL operator = <false> (Vector2) noexcept
			{
				static_assert(false, "Vector4 Error: lvalue cannot be duplicate.");
				return *this;
			}

			inline _CALL operator ::Math::Vector2 (void) const noexcept;
		};
		
		template <unsigned X, unsigned Y, unsigned Z>
		struct _DLL_EXPORT Swizzle<X, Y, Z> : DataMember
		{
			template<bool = Ut::IsUniqueV<X, Y, Z>::value>
			inline Swizzle<X, Y, Z>& _CALL operator = (Vector3 _rhs) noexcept;

			template <>
			inline Swizzle& _CALL operator = <false> (Vector3) noexcept
			{
				static_assert(false, "Vector4 Error: lvalue cannot be duplicate.");
				return *this;
			}

			inline _CALL operator Vector3 (void) const noexcept;
		};

		template <unsigned X, unsigned Y, unsigned Z, unsigned W>
		struct Swizzle<X, Y, Z, W> : DataMember
		{
			Swizzle() = default;
			Swizzle(__m128 _data) noexcept : DataMember{ _data } {}

			template<bool = Ut::IsUniqueV<X, Y, Z, W>::value>
			inline Swizzle& _CALL operator = (Vector4 _rhs) noexcept;

			template <>
			inline Swizzle& _CALL operator = <false> (Vector4) noexcept
			{
				static_assert(false, "Vector4 Error: lvalue cannot be duplicate.");
				return *this;
			}

			template<unsigned Q, unsigned R, unsigned S, unsigned T, typename ret_t = Ut::EnableIf_t<Ut::IsUniqueV<X, Y, Z, W>::value, Swizzle&>>
			inline ret_t _CALL operator = (Swizzle<Q, R, S, T> _rhs) noexcept;

			inline __m128 _CALL GetRaw(void) const noexcept;
			inline __m128 _CALL _GetRawUnshuf(void) const noexcept;

			inline _CALL operator Math::Vector4 (void) const noexcept;

		private:
			static constexpr unsigned shuffleRead = _MM_SHUFFLE(W, Z, Y, X);
			static constexpr unsigned shuffleWrite = (3 << (2*W)) | (2 << (2*Z)) | (1 << (2*Y)) | (0 << (2*X));
		};

		static inline __m128 _CALL InvSqrt(__m128);
		static inline __m128 _CALL Dot(__m128, __m128);

	public:
		Swizzle<0> x;
		Swizzle<1> y;
		Swizzle<2> z;
		Swizzle<3> w;

		Swizzle<0, 0> xx; Swizzle<1, 0> yx; Swizzle<2, 0> zx; Swizzle<3, 0> wx;
		Swizzle<0, 1> xy; Swizzle<1, 1> yy; Swizzle<2, 1> zy; Swizzle<3, 1> wy;
		Swizzle<0, 2> xz; Swizzle<1, 2> yz; Swizzle<2, 2> zz; Swizzle<3, 2> wz;
		Swizzle<0, 3> xw; Swizzle<1, 3> yw; Swizzle<2, 3> zw; Swizzle<3, 3> ww;

		Swizzle<0, 0, 0> xxx; Swizzle<1, 0, 0> yxx; Swizzle<2, 0, 0> zxx; Swizzle<3, 0, 0> wxx;
		Swizzle<0, 0, 1> xxy; Swizzle<1, 0, 1> yxy;	Swizzle<2, 0, 1> zxy; Swizzle<3, 0, 1> wxy;
		Swizzle<0, 0, 2> xxz; Swizzle<1, 0, 2> yxz;	Swizzle<2, 0, 2> zxz; Swizzle<3, 0, 2> wxz;
		Swizzle<0, 0, 3> xxw; Swizzle<1, 0, 3> yxw;	Swizzle<2, 0, 3> zxw; Swizzle<3, 0, 3> wxw;
		Swizzle<0, 1, 0> xyx; Swizzle<1, 1, 0> yyx;	Swizzle<2, 1, 0> zyx; Swizzle<3, 1, 0> wyx;
		Swizzle<0, 1, 1> xyy; Swizzle<1, 1, 1> yyy;	Swizzle<2, 1, 1> zyy; Swizzle<3, 1, 1> wyy;
		Swizzle<0, 1, 2> xyz; Swizzle<1, 1, 2> yyz;	Swizzle<2, 1, 2> zyz; Swizzle<3, 1, 2> wyz;
		Swizzle<0, 1, 3> xyw; Swizzle<1, 1, 3> yyw;	Swizzle<2, 1, 3> zyw; Swizzle<3, 1, 3> wyw;
		Swizzle<0, 2, 0> xzx; Swizzle<1, 2, 0> yzx;	Swizzle<2, 2, 0> zzx; Swizzle<3, 2, 0> wzx;
		Swizzle<0, 2, 1> xzy; Swizzle<1, 2, 1> yzy;	Swizzle<2, 2, 1> zzy; Swizzle<3, 2, 1> wzy;
		Swizzle<0, 2, 2> xzz; Swizzle<1, 2, 2> yzz;	Swizzle<2, 2, 2> zzz; Swizzle<3, 2, 2> wzz;
		Swizzle<0, 2, 3> xzw; Swizzle<1, 2, 3> yzw;	Swizzle<2, 2, 3> zzw; Swizzle<3, 2, 3> wzw;
		Swizzle<0, 3, 0> xwx; Swizzle<1, 3, 0> ywx;	Swizzle<2, 3, 0> zwx; Swizzle<3, 3, 0> wwx;
		Swizzle<0, 3, 1> xwy; Swizzle<1, 3, 1> ywy;	Swizzle<2, 3, 1> zwy; Swizzle<3, 3, 1> wwy;
		Swizzle<0, 3, 2> xwz; Swizzle<1, 3, 2> ywz;	Swizzle<2, 3, 2> zwz; Swizzle<3, 3, 2> wwz;
		Swizzle<0, 3, 3> xww; Swizzle<1, 3, 3> yww;	Swizzle<2, 3, 3> zww; Swizzle<3, 3, 3> www;

		Swizzle<0, 0, 0, 0> xxxx; Swizzle<1, 0, 0, 0> yxxx; Swizzle<2, 0, 0, 0> zxxx; Swizzle<3, 0, 0, 0> wxxx;
		Swizzle<0, 0, 0, 1> xxxy; Swizzle<1, 0, 0, 1> yxxy;	Swizzle<2, 0, 0, 1> zxxy; Swizzle<3, 0, 0, 1> wxxy;
		Swizzle<0, 0, 0, 2> xxxz; Swizzle<1, 0, 0, 2> yxxz;	Swizzle<2, 0, 0, 2> zxxz; Swizzle<3, 0, 0, 2> wxxz;
		Swizzle<0, 0, 0, 3> xxxw; Swizzle<1, 0, 0, 3> yxxw;	Swizzle<2, 0, 0, 3> zxxw; Swizzle<3, 0, 0, 3> wxxw;
		Swizzle<0, 0, 1, 0> xxyx; Swizzle<1, 0, 1, 0> yxyx;	Swizzle<2, 0, 1, 0> zxyx; Swizzle<3, 0, 1, 0> wxyx;
		Swizzle<0, 0, 1, 1> xxyy; Swizzle<1, 0, 1, 1> yxyy;	Swizzle<2, 0, 1, 1> zxyy; Swizzle<3, 0, 1, 1> wxyy;
		Swizzle<0, 0, 1, 2> xxyz; Swizzle<1, 0, 1, 2> yxyz;	Swizzle<2, 0, 1, 2> zxyz; Swizzle<3, 0, 1, 2> wxyz;
		Swizzle<0, 0, 1, 3> xxyw; Swizzle<1, 0, 1, 3> yxyw;	Swizzle<2, 0, 1, 3> zxyw; Swizzle<3, 0, 1, 3> wxyw;
		Swizzle<0, 0, 2, 0> xxzx; Swizzle<1, 0, 2, 0> yxzx;	Swizzle<2, 0, 2, 0> zxzx; Swizzle<3, 0, 2, 0> wxzx;
		Swizzle<0, 0, 2, 1> xxzy; Swizzle<1, 0, 2, 1> yxzy;	Swizzle<2, 0, 2, 1> zxzy; Swizzle<3, 0, 2, 1> wxzy;
		Swizzle<0, 0, 2, 2> xxzz; Swizzle<1, 0, 2, 2> yxzz;	Swizzle<2, 0, 2, 2> zxzz; Swizzle<3, 0, 2, 2> wxzz;
		Swizzle<0, 0, 2, 3> xxzw; Swizzle<1, 0, 2, 3> yxzw;	Swizzle<2, 0, 2, 3> zxzw; Swizzle<3, 0, 2, 3> wxzw;
		Swizzle<0, 0, 3, 0> xxwx; Swizzle<1, 0, 3, 0> yxwx;	Swizzle<2, 0, 3, 0> zxwx; Swizzle<3, 0, 3, 0> wxwx;
		Swizzle<0, 0, 3, 1> xxwy; Swizzle<1, 0, 3, 1> yxwy;	Swizzle<2, 0, 3, 1> zxwy; Swizzle<3, 0, 3, 1> wxwy;
		Swizzle<0, 0, 3, 2> xxwz; Swizzle<1, 0, 3, 2> yxwz;	Swizzle<2, 0, 3, 2> zxwz; Swizzle<3, 0, 3, 2> wxwz;
		Swizzle<0, 0, 3, 3> xxww; Swizzle<1, 0, 3, 3> yxww;	Swizzle<2, 0, 3, 3> zxww; Swizzle<3, 0, 3, 3> wxww;
		Swizzle<0, 1, 0, 0> xyxx; Swizzle<1, 1, 0, 0> yyxx;	Swizzle<2, 1, 0, 0> zyxx; Swizzle<3, 1, 0, 0> wyxx;
		Swizzle<0, 1, 0, 1> xyxy; Swizzle<1, 1, 0, 1> yyxy;	Swizzle<2, 1, 0, 1> zyxy; Swizzle<3, 1, 0, 1> wyxy;
		Swizzle<0, 1, 0, 2> xyxz; Swizzle<1, 1, 0, 2> yyxz;	Swizzle<2, 1, 0, 2> zyxz; Swizzle<3, 1, 0, 2> wyxz;
		Swizzle<0, 1, 0, 3> xyxw; Swizzle<1, 1, 0, 3> yyxw;	Swizzle<2, 1, 0, 3> zyxw; Swizzle<3, 1, 0, 3> wyxw;
		Swizzle<0, 1, 1, 0> xyyx; Swizzle<1, 1, 1, 0> yyyx;	Swizzle<2, 1, 1, 0> zyyx; Swizzle<3, 1, 1, 0> wyyx;
		Swizzle<0, 1, 1, 1> xyyy; Swizzle<1, 1, 1, 1> yyyy;	Swizzle<2, 1, 1, 1> zyyy; Swizzle<3, 1, 1, 1> wyyy;
		Swizzle<0, 1, 1, 2> xyyz; Swizzle<1, 1, 1, 2> yyyz;	Swizzle<2, 1, 1, 2> zyyz; Swizzle<3, 1, 1, 2> wyyz;
		Swizzle<0, 1, 1, 3> xyyw; Swizzle<1, 1, 1, 3> yyyw;	Swizzle<2, 1, 1, 3> zyyw; Swizzle<3, 1, 1, 3> wyyw;
		Swizzle<0, 1, 2, 0> xyzx; Swizzle<1, 1, 2, 0> yyzx;	Swizzle<2, 1, 2, 0> zyzx; Swizzle<3, 1, 2, 0> wyzx;
		Swizzle<0, 1, 2, 1> xyzy; Swizzle<1, 1, 2, 1> yyzy;	Swizzle<2, 1, 2, 1> zyzy; Swizzle<3, 1, 2, 1> wyzy;
		Swizzle<0, 1, 2, 2> xyzz; Swizzle<1, 1, 2, 2> yyzz;	Swizzle<2, 1, 2, 2> zyzz; Swizzle<3, 1, 2, 2> wyzz;
		Swizzle<0, 1, 2, 3> xyzw; Swizzle<1, 1, 2, 3> yyzw;	Swizzle<2, 1, 2, 3> zyzw; Swizzle<3, 1, 2, 3> wyzw;
		Swizzle<0, 1, 3, 0> xywx; Swizzle<1, 1, 3, 0> yywx;	Swizzle<2, 1, 3, 0> zywx; Swizzle<3, 1, 3, 0> wywx;
		Swizzle<0, 1, 3, 1> xywy; Swizzle<1, 1, 3, 1> yywy;	Swizzle<2, 1, 3, 1> zywy; Swizzle<3, 1, 3, 1> wywy;
		Swizzle<0, 1, 3, 2> xywz; Swizzle<1, 1, 3, 2> yywz;	Swizzle<2, 1, 3, 2> zywz; Swizzle<3, 1, 3, 2> wywz;
		Swizzle<0, 1, 3, 3> xyww; Swizzle<1, 1, 3, 3> yyww;	Swizzle<2, 1, 3, 3> zyww; Swizzle<3, 1, 3, 3> wyww;
		Swizzle<0, 2, 0, 0> xzxx; Swizzle<1, 2, 0, 0> yzxx;	Swizzle<2, 2, 0, 0> zzxx; Swizzle<3, 2, 0, 0> wzxx;
		Swizzle<0, 2, 0, 1> xzxy; Swizzle<1, 2, 0, 1> yzxy;	Swizzle<2, 2, 0, 1> zzxy; Swizzle<3, 2, 0, 1> wzxy;
		Swizzle<0, 2, 0, 2> xzxz; Swizzle<1, 2, 0, 2> yzxz;	Swizzle<2, 2, 0, 2> zzxz; Swizzle<3, 2, 0, 2> wzxz;
		Swizzle<0, 2, 0, 3> xzxw; Swizzle<1, 2, 0, 3> yzxw;	Swizzle<2, 2, 0, 3> zzxw; Swizzle<3, 2, 0, 3> wzxw;
		Swizzle<0, 2, 1, 0> xzyx; Swizzle<1, 2, 1, 0> yzyx;	Swizzle<2, 2, 1, 0> zzyx; Swizzle<3, 2, 1, 0> wzyx;
		Swizzle<0, 2, 1, 1> xzyy; Swizzle<1, 2, 1, 1> yzyy;	Swizzle<2, 2, 1, 1> zzyy; Swizzle<3, 2, 1, 1> wzyy;
		Swizzle<0, 2, 1, 2> xzyz; Swizzle<1, 2, 1, 2> yzyz;	Swizzle<2, 2, 1, 2> zzyz; Swizzle<3, 2, 1, 2> wzyz;
		Swizzle<0, 2, 1, 3> xzyw; Swizzle<1, 2, 1, 3> yzyw;	Swizzle<2, 2, 1, 3> zzyw; Swizzle<3, 2, 1, 3> wzyw;
		Swizzle<0, 2, 2, 0> xzzx; Swizzle<1, 2, 2, 0> yzzx;	Swizzle<2, 2, 2, 0> zzzx; Swizzle<3, 2, 2, 0> wzzx;
		Swizzle<0, 2, 2, 1> xzzy; Swizzle<1, 2, 2, 1> yzzy;	Swizzle<2, 2, 2, 1> zzzy; Swizzle<3, 2, 2, 1> wzzy;
		Swizzle<0, 2, 2, 2> xzzz; Swizzle<1, 2, 2, 2> yzzz;	Swizzle<2, 2, 2, 2> zzzz; Swizzle<3, 2, 2, 2> wzzz;
		Swizzle<0, 2, 2, 3> xzzw; Swizzle<1, 2, 2, 3> yzzw;	Swizzle<2, 2, 2, 3> zzzw; Swizzle<3, 2, 2, 3> wzzw;
		Swizzle<0, 2, 3, 0> xzwx; Swizzle<1, 2, 3, 0> yzwx;	Swizzle<2, 2, 3, 0> zzwx; Swizzle<3, 2, 3, 0> wzwx;
		Swizzle<0, 2, 3, 1> xzwy; Swizzle<1, 2, 3, 1> yzwy;	Swizzle<2, 2, 3, 1> zzwy; Swizzle<3, 2, 3, 1> wzwy;
		Swizzle<0, 2, 3, 2> xzwz; Swizzle<1, 2, 3, 2> yzwz;	Swizzle<2, 2, 3, 2> zzwz; Swizzle<3, 2, 3, 2> wzwz;
		Swizzle<0, 2, 3, 3> xzww; Swizzle<1, 2, 3, 3> yzww;	Swizzle<2, 2, 3, 3> zzww; Swizzle<3, 2, 3, 3> wzww;
		Swizzle<0, 3, 0, 0> xwxx; Swizzle<1, 3, 0, 0> ywxx;	Swizzle<2, 3, 0, 0> zwxx; Swizzle<3, 3, 0, 0> wwxx;
		Swizzle<0, 3, 0, 1> xwxy; Swizzle<1, 3, 0, 1> ywxy;	Swizzle<2, 3, 0, 1> zwxy; Swizzle<3, 3, 0, 1> wwxy;
		Swizzle<0, 3, 0, 2> xwxz; Swizzle<1, 3, 0, 2> ywxz;	Swizzle<2, 3, 0, 2> zwxz; Swizzle<3, 3, 0, 2> wwxz;
		Swizzle<0, 3, 0, 3> xwxw; Swizzle<1, 3, 0, 3> ywxw;	Swizzle<2, 3, 0, 3> zwxw; Swizzle<3, 3, 0, 3> wwxw;
		Swizzle<0, 3, 1, 0> xwyx; Swizzle<1, 3, 1, 0> ywyx;	Swizzle<2, 3, 1, 0> zwyx; Swizzle<3, 3, 1, 0> wwyx;
		Swizzle<0, 3, 1, 1> xwyy; Swizzle<1, 3, 1, 1> ywyy;	Swizzle<2, 3, 1, 1> zwyy; Swizzle<3, 3, 1, 1> wwyy;
		Swizzle<0, 3, 1, 2> xwyz; Swizzle<1, 3, 1, 2> ywyz;	Swizzle<2, 3, 1, 2> zwyz; Swizzle<3, 3, 1, 2> wwyz;
		Swizzle<0, 3, 1, 3> xwyw; Swizzle<1, 3, 1, 3> ywyw;	Swizzle<2, 3, 1, 3> zwyw; Swizzle<3, 3, 1, 3> wwyw;
		Swizzle<0, 3, 2, 0> xwzx; Swizzle<1, 3, 2, 0> ywzx;	Swizzle<2, 3, 2, 0> zwzx; Swizzle<3, 3, 2, 0> wwzx;
		Swizzle<0, 3, 2, 1> xwzy; Swizzle<1, 3, 2, 1> ywzy;	Swizzle<2, 3, 2, 1> zwzy; Swizzle<3, 3, 2, 1> wwzy;
		Swizzle<0, 3, 2, 2> xwzz; Swizzle<1, 3, 2, 2> ywzz;	Swizzle<2, 3, 2, 2> zwzz; Swizzle<3, 3, 2, 2> wwzz;
		Swizzle<0, 3, 2, 3> xwzw; Swizzle<1, 3, 2, 3> ywzw;	Swizzle<2, 3, 2, 3> zwzw; Swizzle<3, 3, 2, 3> wwzw;
		Swizzle<0, 3, 3, 0> xwwx; Swizzle<1, 3, 3, 0> ywwx;	Swizzle<2, 3, 3, 0> zwwx; Swizzle<3, 3, 3, 0> wwwx;
		Swizzle<0, 3, 3, 1> xwwy; Swizzle<1, 3, 3, 1> ywwy;	Swizzle<2, 3, 3, 1> zwwy; Swizzle<3, 3, 3, 1> wwwy;
		Swizzle<0, 3, 3, 2> xwwz; Swizzle<1, 3, 3, 2> ywwz;	Swizzle<2, 3, 3, 2> zwwz; Swizzle<3, 3, 3, 2> wwwz;
		Swizzle<0, 3, 3, 3> xwww; Swizzle<1, 3, 3, 3> ywww;	Swizzle<2, 3, 3, 3> zwww; Swizzle<3, 3, 3, 3> wwww;

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

	inline Vector4 _CALL operator - (const Vector4, const Vector4);
	inline Vector4 _CALL operator + (const Vector4, const Vector4);
	inline Vector4 _CALL operator * (const Vector4, const Vector4);
	inline Vector4 _CALL operator * (const float, const Vector4);
	inline Vector4 _CALL operator * (const Vector4, const float);
	inline Vector4 _CALL operator / (const Vector4, const float);

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

//inline Math::Vector4::Vector4(const Vector4& v) noexcept 
//	: mData( v.mData )
//{
//
//}

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
inline Math::Vector4::Swizzle<N>& _CALL Math::Vector4::Swizzle<N>::operator= (float _rhs)
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
inline Math::Vector4::Swizzle<0>& _CALL Math::Vector4::Swizzle<0>::operator= (float _rhs)
{
	mData = _mm_move_ss(mData, _mm_set_ss(_rhs));

	return *this;
}

template <unsigned N>
inline _CALL Math::Vector4::Swizzle<N>::operator float(void) const
{
	// Copy the wanted value everywhere, and return the first 32 bits as a float
	return _mm_cvtss_f32(_mm_shuffle_ps(mData, mData, _MM_SHUFFLE(N, N, N, N)));
}

template<>
inline _CALL Math::Vector4::Swizzle<0>::operator float(void) const
{
	// We happen to want the value that's already the first
	// so we can skip the shuffle and just return it
	// - Compiler seems to do this special case optimisation even without this
	//   But I'll leave it here anyway
	return _mm_cvtss_f32(mData);
}



template<unsigned X, unsigned Y, unsigned Z, unsigned W> template<bool>
inline Math::Vector4::Swizzle<X, Y, Z, W>& _CALL Math::Vector4::Swizzle<X, Y, Z, W>::operator = (Math::Vector4 _rhs) noexcept
{
	mData = _mm_shuffle_ps(_rhs.mData, _rhs.mData, shuffleWrite);
	return *this;
}

template<> template<>
inline Math::Vector4::Swizzle<0, 1, 2, 3>& _CALL Math::Vector4::Swizzle<0, 1, 2, 3>::operator = <true> (Math::Vector4 _rhs) noexcept
{
	mData = _rhs.mData;
	return *this;
}

namespace
{
	template <typename, typename>
	struct SwizzleCompare;

	template <template <unsigned ...> class Set, unsigned lhs, unsigned rhs, unsigned ... ls, unsigned ... rs>
	struct SwizzleCompare<Set<lhs, ls...>, Set<rhs, rs...>>
	{
		static constexpr bool value = lhs < rhs;
	};
}

template<unsigned X, unsigned Y, unsigned Z, unsigned W> 
template<unsigned Q, unsigned R, unsigned S, unsigned T, typename ret_t>
inline ret_t _CALL Math::Vector4::Swizzle<X, Y, Z, W>::operator = (Swizzle<Q, R, S, T> _rhs) noexcept
{
	using ReSwizzle_t = Ut::MetaSortT_t <SwizzleCompare, 
		Ut::Collection<
			Swizzle<X, Q>, 
			Swizzle<Y, R>,
			Swizzle<Z, S>,
			Swizzle<W, T>
	>>;

	using Result_t = Ut::MetaExtract_t< 1,
		Ut::MetaExtract_t<0, ReSwizzle_t>,
		Ut::MetaExtract_t<1, ReSwizzle_t>,
		Ut::MetaExtract_t<2, ReSwizzle_t>,
		Ut::MetaExtract_t<3, ReSwizzle_t>
	>;

	mData = Result_t { _rhs._GetRawUnshuf() }.GetRaw();
	return *this;
}

template<unsigned X, unsigned Y, unsigned Z, unsigned W>
inline _CALL Math::Vector4::Swizzle<X, Y, Z, W>::operator Math::Vector4 (void) const noexcept
{
	return Vector4{ GetRaw() };
}

template <unsigned X, unsigned Y, unsigned Z, unsigned W>
inline __m128 _CALL Math::Vector4::Swizzle<X, Y, Z, W>::GetRaw(void) const noexcept
{
	return _mm_shuffle_ps(mData, mData, shuffleRead);
}

template<unsigned X, unsigned Y, unsigned Z, unsigned W>
inline __m128 _CALL Math::Vector4::Swizzle<X, Y, Z, W>::_GetRawUnshuf(void) const noexcept
{
	return mData;
}

template<>
inline __m128 _CALL Math::Vector4::Swizzle<0, 1, 2, 3>::GetRaw(void) const noexcept
{
	return mData;
}

// No automatic compiler instruction change for the following special shuffles
// I guess since intel guide says it's the same
// Although there seems to be a consensus that these are faster than shuffle
// Perhaps on older CPUs and moot on newer ones

template<>
inline __m128 _CALL Math::Vector4::Swizzle<0, 0, 1, 1>::GetRaw(void) const noexcept
{
	return _mm_unpacklo_ps(mData, mData);
}

template<>
inline __m128 _CALL Math::Vector4::Swizzle<2, 2, 3, 3>::GetRaw(void) const noexcept
{
	return _mm_unpackhi_ps(mData, mData);
}

template<>
inline __m128 _CALL Math::Vector4::Swizzle<0, 1, 0, 1>::GetRaw(void) const noexcept
{
	return _mm_movelh_ps(mData, mData);
}

template<>
inline __m128 _CALL Math::Vector4::Swizzle<2, 3, 2, 3>::GetRaw(void) const noexcept
{
	return _mm_movelh_ps(mData, mData);
}

#if defined(_INCLUDED_PMM)		// SSE 3

template<>
inline __m128 _CALL Math::Vector4::Swizzle<0, 0, 2, 2>::GetRaw(void) const noexcept
{
	return _mm_moveldup_ps(mData);
}

template<>
inline __m128 _CALL Math::Vector4::Swizzle<1, 1, 3, 3>::GetRaw(void) const noexcept
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

