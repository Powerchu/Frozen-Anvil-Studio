/* HEADER *********************************************************************************/
/*!
\file	Matrix4.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Experimental, using the experimental Vector4
	All the inlining means that everything has to be in the header file

	TODO: General Matrix Inverse

	Reference: https://software.intel.com/sites/landingpage/IntrinsicsGuide

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Math/Matrix4.h"		// File Header

#include <cmath>				// sinf, cosf
#include <xmmintrin.h>			// SSE
#include <emmintrin.h>			// SSE 2
#include <tmmintrin.h>			// SSE 3
#include <smmintrin.h>			// SSE 4.1

#define _CALL __vectorcall


float _CALL Math::Matrix4::Determinant(void) const
{
	return .0f;
}

Math::Matrix4& _CALL Math::Matrix4::Inverse(void)
{
	/* Block matrix method

	// 0 1 2 3  ->  0 2 8 A
	// 4 5 6 7  ->  5 7 D F
	// 8 9 A B  ->  4 6 C E
	// C D E F  ->  1 3 9 B

	__m128 row1 = _mm_shuffle_ps(mData[0].mData, mData[3].mData, _MM_SHUFFLE(2, 0, 2, 0));
	__m128 row4 = _mm_shuffle_ps(mData[0].mData, mData[3].mData, _MM_SHUFFLE(3, 1, 3, 1));
	__m128 row3 = _mm_shuffle_ps(mData[1].mData, mData[2].mData, _MM_SHUFFLE(2, 0, 2, 0));
	__m128 row2 = _mm_shuffle_ps(mData[1].mData, mData[2].mData, _MM_SHUFFLE(3, 1, 3, 1));

	// AD - BC of sub matrices
	row1 = _mm_mul_ps(row1, row4); // AD
	row2 = _mm_mul_ps(row2, row3); // BC
	row1 = _mm_sub_ps(row1, row2); // AD - BC

	row4 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 3, 3, 3));
	row3 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(2, 2, 2, 2));
	row2 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 1, 1, 1));
	row1 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(0, 0, 0, 0));
	*/

	return *this;
}

Math::Matrix4& _CALL Math::Matrix4::AffineInverse(void)
{
	Vector4 mag{ .0f, .0f, .0f, 1.f };
	mag.MultiplyAdd(mData[0], mData[0]);
	mag.MultiplyAdd(mData[1], mData[1]);
	mag.MultiplyAdd(mData[2], mData[2]);
	mag.Reciprocal();
	mag.w = .0f;

	Matrix4 inverse = *this;

	inverse.mData[0] *= mag;
	inverse.mData[1] *= mag;
	inverse.mData[2] *= mag;

	inverse.mData[3] = Vector4{ .0f, .0f, .0f, 1.f };
	inverse.mData[3].MultiplySub(inverse.mData[0], mData[0].wwww);
	inverse.mData[3].MultiplySub(inverse.mData[1], mData[1].wwww);
	inverse.mData[3].MultiplySub(inverse.mData[2], mData[2].wwww);

	return *this = inverse.Transpose();
}


Math::Matrix4 _CALL Math::RotateX(Angle _fAngle)
{
	float s = sinf(_fAngle.Radians()), c = cosf(_fAngle.Radians());
	return Matrix4{ 1.f, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1 };
}

Math::Matrix4 _CALL Math::RotateY(Angle _fAngle)
{
	float s = sinf(_fAngle.Radians()), c = cosf(_fAngle.Radians());
	return Matrix4{ c, 0, s, 0, 0, 1.f, 0, 0, -s, 0, c, 0, 0, 0, 0, 1.f };
}

Math::Matrix4 _CALL Math::RotateZ(Angle _fAngle)
{
	float s = sinf(_fAngle.Radians()), c = cosf(_fAngle.Radians());
	return Matrix4{ c, -s, 0, 0, s, c, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f };
}


Math::Matrix4 _CALL Math::RotXTrans(Angle _fAngle, Vector4 _vTrans)
{
	return RotXTrans(_fAngle, _vTrans.x, _vTrans.y, _vTrans.z);
}

Math::Matrix4 _CALL Math::RotXTrans(Angle _fAngle, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	float s = sinf(_fAngle.Radians()), c = cosf(_fAngle.Radians());
	return Matrix4{ 1.f, 0, 0, _fTranslateX, 0, c, -s, _fTranslateY, 0, s, c, _fTranslateZ, 0, 0, 0, 1 };
}

Math::Matrix4 _CALL Math::RotYTrans(Angle _fAngle, Vector4 _vTrans)
{
	return RotYTrans(_fAngle, _vTrans.x, _vTrans.y, _vTrans.z);
}

Math::Matrix4 _CALL Math::RotYTrans(Angle _fAngle, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	float s = sinf(_fAngle.Radians()), c = cosf(_fAngle.Radians());
	return Matrix4{ c, 0, s, _fTranslateX, 0, 1.f, 0, _fTranslateY, -s, 0, c, _fTranslateZ, 0, 0, 0, 1.f };
}

Math::Matrix4 _CALL Math::RotZTrans(Angle _fAngle, Vector4 _vTrans)
{
	return RotZTrans(_fAngle, _vTrans.x, _vTrans.y, _vTrans.z);
}

Math::Matrix4 _CALL Math::RotZTrans(Angle _fAngle, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	float s = sinf(_fAngle.Radians()), c = cosf(_fAngle.Radians());
	return Matrix4{ c, -s, 0, _fTranslateX, s, c, 0, _fTranslateY, 0, 0, 1.f, _fTranslateZ, 0, 0, 0, 1.f };
}


#undef _CALL


