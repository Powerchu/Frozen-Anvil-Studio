/* HEADER *********************************************************************************/
/*!
\file	Quaternion.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation of Quaternion Rotations

	References:
	Wikipedia

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


Math::Quaternion _CALL Math::Quaternion::FromEuler(Math::Angle _x, Math::Angle _y, Math::Angle _z)
{
	// The double versions of std trig functions are apparently faster
	const double sx = std::sin(_x.Radians() * 0.5);
	const double cx = std::cos(_x.Radians() * 0.5);
	const double sy = std::sin(_y.Radians() * 0.5);
	const double cy = std::cos(_y.Radians() * 0.5);
	const double sz = std::sin(_z.Radians() * 0.5);
	const double cz = std::cos(_z.Radians() * 0.5);

	// pitch = y
	// roll = x
	// yaw = z
	return Math::Quaternion{
		static_cast<float>(sx * cy * cz - cx * sy * sz), //x
		static_cast<float>(cx * sy * cz + sx * cy * sz), //y 
		static_cast<float>(cx * cy * sz - sx * sy * cz), //z 
		static_cast<float>(cx * cy * cz + sx * sy * sz)  //w
	};
}

Math::Vector4 _CALL Math::Quaternion::ToEuler(void) const
{
	const double ySq = mData[1] * mData[1];
	const double t0 = 1.0 - 2.0 * (ySq + mData[0] * mData[0]);
	const double t1 = 1.0 - 2.0 * (ySq + mData[2] * mData[2]);
	const double t2 = 2.0 * (mData[0] * mData[3] + mData[1] * mData[2]);
	const double t3 = 2.0 * (mData[0] * mData[1] + mData[2] * mData[3]);
	      double t4 = 2.0 * (mData[1] * mData[3] - mData[0] * mData[2]);

	t4 = Math::Clamp(t4, -1.0, 1.0);

	return Math::Vector4{
		Math::Radians{static_cast<float>(std::atan2(t2, t0))}.Degrees(),  
		Math::Radians{static_cast<float>(std::asin (t4)    )}.Degrees(),
		Math::Radians{static_cast<float>(std::atan2(t3, t1))}.Degrees(), 
		0
	};
}

Math::Quaternion __vectorcall Math::Slerp(Quaternion _q1, const Quaternion _q2, float _fRatio)
{
	float c = Math::Dot(_q1, _q2);

	if (c < .0f)
	{
		c = -c;
		_q1 = Math::Negate<Math::NegateFlag::XYZW>(_q1);
		// Ensure shortest path by negating the quaternion
	}

	// Too close
	if (c > .9995f)
		return Math::Lerp(_q1, _q2, _fRatio).Normalise();

	float const angle = std::acosf(c);
	float const s1 = std::sinf(angle);
	float const s2 = 1.f / std::sinf(angle * _fRatio);

	return ((std::cosf(angle * _fRatio) - c * s1 * s2) * _q1 + (s1 * s2) * _q2).Normalise();
}

Math::Quaternion __vectorcall Math::nLerp(Quaternion _q1, const Quaternion _q2, float _fRatio)
{
	// Do spline interpolation to correct speed
	// Reference https://www.gamedevs.org/uploads/orientation-representation.pdf slide 46
	// Reference http://number-none.com/product/Hacking%20Quaternions/
	float dp = Math::Dot(_q1, _q2);

	if (dp < .0f)
	{
		dp = -dp;
		_q1 = Math::Negate<Math::NegateFlag::XYZW>(_q1);
	}

	float factor = 1.f - .7878088f * dp;
	factor *= factor;

	float const C = .5069269f * factor;
	float const t = (_fRatio * C) * (2 * _fRatio - 3.f) + (1.f + C);

	return Math::Lerp(_q1, _q2, t).Normalise();
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
	Vec4 t2 = _rhs.mData.yxyx;
	Vec4 t3 = _rhs.mData.wzwz;

	Vec4 t4 = HorizontalSub(t2 * mData, t3 * t1);
	Vec4 t5 = HorizontalAdd(t3 * mData, t2 * t1);

	t2 = Vec4{ _mm_shuffle_ps(t4.GetRaw(), t5.GetRaw(), _MM_SHUFFLE(2, 3, 0, 1)) };
	t3 = Vec4{ _mm_shuffle_ps(t5.GetRaw(), t4.GetRaw(), _MM_SHUFFLE(3, 2, 1, 0)) };
	t1 = AddSub(t3, t2);

	return Quaternion{ t1.xwyz.GetRaw() };
}


#undef _CALL


