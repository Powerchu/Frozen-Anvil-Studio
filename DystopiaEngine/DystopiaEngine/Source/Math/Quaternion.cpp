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
#include "Math/Angles.h"

#include <xmmintrin.h>			// SSE
#include <emmintrin.h>			// SSE 2
#include <tmmintrin.h>			// SSE 3
#include <smmintrin.h>			// SSE 4.1
#include <cmath>  //sinf, cosf

#define _CALL __vectorcall


static Math::Quaternion _CALL Math::Quaternion::FromEuler(Math::Angle _x, Math::Angle _y, Math::Angle _z)
{
	float sx = sinf(_x.Radians()),
		cx = cosf(_x.Radians()),
		sy = sinf(_y.Radians()),
		cy = cosf(_y.Radians()),
		sz = sinf(_z.Radians()),
		cz = cosf(_z.Radians());

	return Math::Quaternion{
		cx * cy * sz - sx * sy * cz,
		sx * cy * cz + cx * sy * sz,
		cx * sy * cz - sx * cy * sz,
		cx * cy * cz + sx * sy * sz
	};
}


Math::Vector4 _CALL Math::Quaternion::Rotate(Vector4 _v) const noexcept
{
	Vec4 temp = 2 * Math::Cross(mData, _v);
	return _v + mData.wwww * temp + Math::Cross(mData, temp);
}

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



// ============================================ OPERATOR OVERLOADING ============================================ // 


Math::Quaternion _CALL Math::Quaternion::operator*(Math::Quaternion _rhs)
{
	Vec4 t1 = mData.wzyx;
	Vec4 t2 = _rhs.mData.yxyx * t1;
	Vec4 t3 = _rhs.mData.wzwz * t1;

	// No blending function exists... 
	t1 = AddSub(
		Vec4{ _mm_shuffle_ps(t3.GetRaw(), t2.GetRaw(), _MM_SHUFFLE(3,2,1,0)) },
		Vec4{ _mm_shuffle_ps(t2.GetRaw(), t3.GetRaw(), _MM_SHUFFLE(2,3,0,1)) }
	);

	return Quaternion{ t1.xwyz.GetRaw() };
}


#undef _CALL


