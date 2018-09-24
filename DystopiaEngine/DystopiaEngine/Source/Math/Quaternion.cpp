/* HEADER *********************************************************************************/
/*!
\file	Quaternion.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation of Quaternion Rotations

	TODO:
	 Convert to matrix

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Math/Quaternion.h"


Math::Matrix4 __vectorcall Math::Quaternion::Matrix(void) const noexcept
{
	Math::Matrix4 m1{
		 mData[3],  mData[2], -mData[1], mData[0],
		-mData[2],  mData[3],  mData[0], mData[1],
		 mData[1], -mData[0],  mData[3], mData[2],
		-mData[0], -mData[1], -mData[2], mData[3]
	},
	m2{
		 mData[3],  mData[2], -mData[1], -mData[0],
		-mData[2],  mData[3],  mData[0], -mData[1],
		 mData[1], -mData[0],  mData[3], -mData[2],
		 mData[0],  mData[1],  mData[2],  mData[3]
	};

	return m1 * m2;
}


