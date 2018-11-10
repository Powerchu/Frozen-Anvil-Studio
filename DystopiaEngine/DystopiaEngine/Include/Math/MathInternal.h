/* HEADER *********************************************************************************/
/*!
\file	MathInternal.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Internal data structures and stuff for Math Library.
	Seperate file to hide this from public interface

	All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
	Reproduction or disclosure of this file or its contents without the
	prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATH_INTERNAL_H_
#define _MATH_INTERNAL_H_

#include <cstdint>
#include <emmintrin.h>		// SSE 2 [__m128i]

namespace Math
{
	namespace Internal
	{
		namespace
		{
			union __declspec(align(alignof(__m128i))) VectorConverter
			{
				int32_t _val[4];
				__m128i vec;
			};
		}

		template <int32_t _X, int32_t _Y, int32_t _Z, int32_t _W>
		inline constexpr __m128i ConstVec4()
		{
			return VectorConverter{{_X, _Y, _Z, _W}}.vec;
		}

		// Hurray for code bloat!
		template<class T, const unsigned index>
		struct AbsHelper
		{
			inline static void CalculateAbs(T& vector)
			{
				vector[index] = vector[index] < 0 ? -vector[index] : vector[index];
				AbsHelper<T, index - 1>::CalculateAbs(vector);
			}
		};

		template<class T>
		struct AbsHelper<T, 0>
		{
			inline static void CalculateAbs(T& vector)
			{
				vector[0] = vector[0] < 0 ? -vector[0] : vector[0];
			}
		};

		template <typename T, auto E>
		struct PowerCalcHelper
		{
			static constexpr int half = E >> 1;

			inline constexpr static T Power(T _obj)
			{
				return PowerCalcHelper<T, half>::Power(_obj) * PowerCalcHelper<T, E - half>::Power(_obj);
			}
		};
		template <typename T>
		struct PowerCalcHelper<T, 1>
		{
			inline constexpr static T Power(T _obj)
			{
				return _obj;
			}
		};

		template<typename T, bool IsNeg, auto E>
		struct PowerCalc
		{
			inline constexpr static auto Power(T _obj)
			{
				auto result = PowerCalcHelper<T, E>::Power(_obj);
				return IsNeg ? T(1) / result : result;
			}
		};

		template<typename T, bool IsNeg>
		struct PowerCalc<T, IsNeg, 0>
		{
			inline constexpr static T Power(T)
			{
				return T(1);
			}
		};
	}
}



#endif		// INCLUDE GUARD

