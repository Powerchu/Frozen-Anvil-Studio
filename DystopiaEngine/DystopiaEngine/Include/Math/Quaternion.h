/* HEADER *********************************************************************************/
/*!
\file	Quaternion.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation of Quaternion Rotations

	NOTE: Cannot inherit Vector4 due to it and this being unions

Reference: https://software.intel.com/sites/landingpage/IntrinsicsGuide

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _QUARTERNION_H_
#define _QUARTERNION_H_

#include <cmath>			// sqrtf
#include <xmmintrin.h>		// SSE
#include <emmintrin.h>		// SSE 2
#include <tmmintrin.h>		// SSE 3
#include <smmintrin.h>		// SSE 4.1

namespace Math
{
	#define _CALL	__vectorcall
	#define ALLIGN	16
	#define USE_DP	0
	
	/*!
	\struct Quaternion
	\brief
		Representing 3D rotations
	*/
	union Quaternion
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		inline Quaternion(void) noexcept;
		inline Quaternion(const Quaternion&) noexcept;
		inline Quaternion(float x, float y = 0, float z = 0, float w = 0) noexcept;
		inline explicit Quaternion(__m128) noexcept;


		// ==================================== VECTOR OPERATIONS ==================================== // 

		// Converts the vector to unit length
		inline Quaternion& _CALL Normalise(void);

		// Computes the dot product of this vector and a given vector
		inline float _CALL Dot(const Quaternion) const;

		// Computes the cross product of this vector and a given vector 
		inline Quaternion& _CALL Cross(const Quaternion);

		// Projects the Vector4 onto the given vector
		inline Quaternion& _CALL Project(const Quaternion);

		inline float _CALL Magnitude(void) const;
		inline float _CALL MagnitudeSqr(void) const;

		inline Quaternion& _CALL Reciprocal(void);

		template <unsigned FLAGS>
		inline Quaternion& _CALL Negate(void) noexcept;

		// ======================================== OPERATORS ======================================= // 

		// Potentially Slow?, please try to avoid!
		inline float& _CALL operator[](const unsigned _nIndex);
		inline const float _CALL operator[](const unsigned _nIndex) const;

		inline Quaternion  _CALL operator-(void) const;
		inline Quaternion& _CALL operator*=(const float);
		inline Quaternion& _CALL operator*=(const Quaternion);
		inline Quaternion& _CALL operator/=(const float);
		inline Quaternion& _CALL operator+=(const Quaternion);
		inline Quaternion& _CALL operator-=(const Quaternion);


	private:

		template <unsigned N>
		struct DataMember
		{
			inline DataMember<N>& _CALL operator= (float _rhs);
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
		// DataMember  -> 1 shuffle (read)
		// SwizzleMask -> 1 shuffle (read & write)
		template <unsigned X, unsigned Y, unsigned Z, unsigned W>
		struct Swizzle
		{
			Swizzle() = default;
			Swizzle(__m128 _data) : mData{ _data } {}

			template<bool = IsLvalueSwizzle<X, Y, Z, W>::value>
			inline Swizzle& _CALL operator = (Quaternion _rhs);

			template <>
			inline Swizzle& _CALL operator = <false> (Quaternion)
			{
				static_assert(false, "Quaternion Error: lvalue cannot be duplicate.");
				return *this;
			}

			template<unsigned Q, unsigned R, unsigned S, unsigned T, typename ret_t = Utility::EnableIf_t<IsLvalueSwizzle<X, Y, Z, W>::value, Swizzle>>
			inline ret_t& _CALL operator = (Swizzle<Q, R, S, T> _rhs);

			inline __m128 _CALL GetRaw(void) const noexcept;
			inline __m128 _CALL _GetRawUnshuf(void) const noexcept;

			inline _CALL operator Math::Quaternion(void) const;

		private:
			__m128 mData;

			static constexpr unsigned shuffleRead = _MM_SHUFFLE(W, Z, Y, X);
			static constexpr unsigned shuffleWrite = (3 << (2 * W)) | (2 << (2 * Z)) | (1 << (2 * Y)) | (0 << (2 * X));
		};

		static inline __m128 _CALL InvSqrt(__m128);
		static inline __m128 _CALL Dot(__m128, __m128);

		__m128 mData;

	public:
		DataMember<0> x;
		DataMember<1> y;
		DataMember<2> z;
		DataMember<3> w;

		Swizzle<0, 0, 0, 0> xxxx;
		Swizzle<0, 0, 1, 1> xxyy;
		Swizzle<0, 0, 2, 2> xxzz;
		Swizzle<0, 1, 0, 1> xyxy;
		Swizzle<0, 1, 2, 3> xyzw;
		Swizzle<0, 2, 1, 3> xzyw;
		Swizzle<0, 3, 0, 3> xwxw;
		Swizzle<1, 0, 2, 3> yxzw;
		Swizzle<1, 0, 3, 2> yxwz;
		Swizzle<1, 1, 1, 1> yyyy;
		Swizzle<1, 3, 0, 2> ywxz;
		Swizzle<1, 3, 2, 0> ywzx;
		Swizzle<2, 0, 1, 3> zxyw; // Used by cross product 
		Swizzle<2, 1, 2, 1> zyzy;
		Swizzle<2, 2, 2, 2> zzzz;
		Swizzle<2, 3, 0, 1> zwxy;
		Swizzle<3, 1, 2, 0> wyzx;
		Swizzle<3, 3, 3, 3> wwww;
	};
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

