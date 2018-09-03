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
#include "Math\Quaternion.h"


Math::Matrix4 __vectorcall Math::Quaternion::Matrix(void) const noexcept
{
	float scale = 2.f / mData.MagnitudeSqr();

	Vec4 t3 = mData.ywyw;
	Vec4 t1 = mData.xxyy * mData.xywz; // xx, xy, yw, yz
	Vec4 t2 = mData.zzzw * mData.zwxy; // zz, zw, xz, yw
	t3 *= t3;						   // yy, ww, yy, ww

	Vec4 t4 = AddSub(t2, t1);			// Add-Sub -> xx + zz, xy - zw, yw + xz, yz - xw
	t1 = AddSub(t1.yxwz, t2.yxwz);      // Add-Sub -> xy + zw, xx - zz, yz + xw, yw - xz


	// Goal :
	// ww + xx - yy - zz,    2 (xy - zw)   ,    2 (xz + cw)   , 0
	//    2 (xy + zw)   , ww - xx + yy - zz,    2 (yz - xw)   , 0
	//    2 (xz - cw)   ,    2 (yz + xw)   , ww - xx - yy + zz, 0
	//          0       ,          0       ,          0       , 1


	return Matrix4{};
}


