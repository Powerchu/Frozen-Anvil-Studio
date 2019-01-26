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
//#include "Math/Vector/Vector3.h"
//#include "Math/Vector/Vector2.h"
#include "Math/Vectors.h"

#include <cmath>		// sqrtf


Math::Vector3::Vector3(Vector2 const& _v, float z) noexcept
	: mData{ _v[0], _v[1], z }
{

}


float __vectorcall Math::Vector3::Magnitude(void) const noexcept
{
	return std::sqrtf(MagnitudeSqr());
}


