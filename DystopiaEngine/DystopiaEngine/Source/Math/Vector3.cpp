/* HEADER *********************************************************************************/
/*!
\file	Vector3.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Function definitions for functions dealing with other Math objects

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

#include <cmath>		// sqrtf
#include <xmmintrin.h>	// SSE


float __vectorcall Math::Vector3::Magnitude(void) const noexcept
{
	return std::sqrtf(MagnitudeSqr());
}


