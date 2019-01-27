/* HEADER *********************************************************************************/
/*!
\file	Vector2.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Function definitions for functions dealing with other Math objects

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Math/Vector/Vector2.h"

#include <cmath> // sqrtf


float __vectorcall Math::Vector2::Magnitude(void) const noexcept
{
	return std::sqrtf(MagnitudeSqr());
}

void func(void)
{
	Math::Vector2 const v1{ 2.f, 3.f };
	Math::Vector2 v2{ 2.f, 3.f };

	static_cast<float&>(v2.y) = 16;
}


