/* HEADER *********************************************************************************/
/*!
\file	Quaternion.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation of Quaternion Rotations

	References:
	Wikipedia

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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


Math::Quaternion _CALL Math::Quaternion::FromEuler(Math::Angle _x, Math::Angle _y, Math::Angle _z)
{
	const float sx = std::sinf(_x.Radians() * 0.5f);
	const float cx = std::cosf(_x.Radians() * 0.5f);
	const float sy = std::sinf(_y.Radians() * 0.5f);
	const float cy = std::cosf(_y.Radians() * 0.5f);
	const float sz = std::sinf(_z.Radians() * 0.5f);
	const float cz = std::cosf(_z.Radians() * 0.5f);

	// pitch = y
	// roll = x
	// yaw = z
	return Math::Quaternion{
		sx * cy * cz - cx * sy * sz, //x
		cx * sy * cz + sx * cy * sz, //y 
		cx * cy * sz - sx * sy * cz, //z 
		cx * cy * cz + sx * sy * sz  //w
	};
}

Math::Vector4 _CALL Math::Quaternion::ToEuler(void) const
{
	const float ySq = mData[1] * mData[1];
	const float t0 = 1.f - 2.0f * (ySq + mData[0] * mData[0]);
	const float t1 = 1.f - 2.0f * (ySq + mData[2] * mData[2]);
	const float t2 = 2.0f * (mData[0] * mData[3] + mData[1] * mData[2]);
	const float t3 = 2.0f * (mData[0] * mData[1] + mData[2] * mData[3]);
	      float t4 = 2.0f * (mData[1] * mData[3] - mData[0] * mData[2]);

	t4 = Math::Clamp(t4, -1.f, 1.f);

	return Math::Vector4{ 
		Math::Radians{std::atan2(t2, t0)}.Degrees(),  
		Math::Radians{std::asin(t4)     }.Degrees(),
		Math::Radians{std::atan2(t3, t1)}.Degrees(), 
		0
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
		 mData[3], -mData[2], -mData[1], mData[0],
		 mData[2],  mData[3],  mData[0], mData[1],
		-mData[1],  mData[0], -mData[3], mData[2],
		-mData[0], -mData[1],  mData[2], mData[3]
	},
	m2{
		 mData[3], -mData[2],  mData[1], -mData[0],
		 mData[2],  mData[3], -mData[0], -mData[1],
		 mData[1], -mData[0], -mData[3],  mData[2],
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

	t1 = AddSub(
		// No blending function exists... 
		Vec4{ _mm_shuffle_ps(t3.GetRaw(), t2.GetRaw(), _MM_SHUFFLE(3,2,1,0)) },
		Vec4{ _mm_shuffle_ps(t2.GetRaw(), t3.GetRaw(), _MM_SHUFFLE(2,3,0,1)) }
	);

	return Quaternion{ t1.xwyz.GetRaw() };
}


#undef _CALL


