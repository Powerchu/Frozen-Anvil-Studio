/* HEADER *********************************************************************************/
/*!
\file	MathConstants.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains frequently used magic numbers!
	Constants taken from wikipedia

	All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
	Reproduction or disclosure of this file or its contents without the
	prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATHCONSTANTS_H_
#define _MATHCONSTANTS_H_

#include "Math/MathInternal.h"


namespace Math
{
	namespace C
	{
		template <typename Ty>
		inline constexpr Ty pi    = Ty(3.1415926535897932384626433832795L);
															
		template <typename Ty>								
		inline constexpr Ty phi   = Ty(1.6180339887498948482045868343656L);

		template <typename Ty>
		inline constexpr Ty sqrt2 = Ty(1.4142135623730950488016887242096L);

		template <typename Ty>
		inline constexpr Ty sqrt3 = Ty(1.7320508075688772935274463415058L);

		template <typename Ty>
		inline constexpr Ty sqrt5 = Ty(2.2360679774997896964091736687312L);

		template <typename Ty>
		inline constexpr Ty euler = Ty(2.7182818284590452353602874713526L);

		template <typename Ty>
		inline constexpr Ty epsilon = Internal::PowerCalc<Ty, true, sizeof(Ty) * 2 - 1>::Power(10);
	}
	

	// Mathematical PI
	inline constexpr float pi = C::pi<float>;

	// 2 * PI
	inline constexpr float tau = 2.f * pi;
	inline constexpr float pi2 = tau;
	
	// Golden ratio
	inline constexpr float phi = C::phi<float>;

	inline constexpr float sqrt2 = C::sqrt2<float>;
	inline constexpr float sqrt3 = C::sqrt3<float>;
	inline constexpr float sqrt5 = C::sqrt5<float>;

	// Exponential Constant
	inline constexpr float euler = C::euler<float>;

	// Approx Floating point error tolerance
	inline constexpr float epsilon = C::epsilon<float>;
}



#endif		// INCLUDE GUARD


