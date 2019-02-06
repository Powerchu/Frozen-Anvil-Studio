/* HEADER *********************************************************************************/
/*!
\file	MathFwd.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	The "I only want the forward declarations for everything" convenience header!

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATHFWD_H_
#define _MATHFWD_H_

namespace Math
{
	union Vector2;
	union Vector3;
	union alignas(16) Vector4;
	struct alignas(16) Quaternion;

	struct alignas(16) Matrix2;
	struct alignas(16) Matrix4;

	class Radians;
	class Degrees;

	using Vec2 = Vector2;
	using Vec3 = Vector3;
	using Vec4 = Vector4;
	using Pt3D = Vector4;
	using Mat2 = Matrix2;
	using Mat4 = Matrix4;

	enum class NegateFlag : unsigned char
	{
		X = 1 << 0,
		Y = 1 << 1,
		Z = 1 << 2,
		W = 1 << 3,

		XY = X | Y,
		XZ = X | Z,
		XW = X | W,
		YZ = Y | Z,
		YW = Y | W,
		ZW = Z | W,

		XYZ = XY | Z,
		XYW = XY | W,
		XZW = XZ | W,
		YZW = YZ | Z,

		XYZW = XY | ZW
	};
}


#endif		// INCLUDE GUARD

