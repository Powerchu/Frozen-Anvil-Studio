/* HEADER *********************************************************************************/
/*!
\file	Quaternion.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Planned implementation of Quaternion Rotations

Reference: https://software.intel.com/sites/landingpage/IntrinsicsGuide

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _QUARTERNION_H_
#define _QUARTERNION_H_


#include <new>				// nothrow_t
#include <cmath>			// sqrtf
#include <exception>		// bad_alloc
#include <xmmintrin.h>		// SSE
#include <emmintrin.h>		// SSE 2
#include <tmmintrin.h>		// SSE 3
#include <smmintrin.h>		// SSE 4.1

namespace Math
{
	#define _CALL	__vectorcall
	#define ALLIGN	16
	#define USE_DP	0

	class Quaternion
	{
	public:


	private:

		__m128 mData;
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

