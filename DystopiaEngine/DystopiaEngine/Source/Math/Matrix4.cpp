/* HEADER *********************************************************************************/
/*!
\file	Matrix4.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Experimental, using the experimental Vector4
	All the inlining means that everything has to be in the header file

	Reference: https://software.intel.com/sites/landingpage/IntrinsicsGuide

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Math\Matrix4.h"		// File Header

float _vectorcall Math::Matrix4::Determinant(void) const
{
	return .0f;
}

Math::Matrix4& _vectorcall Math::Matrix4::Inverse(void)
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

Math::Matrix4& _vectorcall Math::Matrix4::AffineInverse(void)
{
	return *this;
}

