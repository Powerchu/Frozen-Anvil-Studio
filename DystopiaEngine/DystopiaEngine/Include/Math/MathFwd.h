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
	union __declspec(align(16)) Vector4;
	union __declspec(align(16)) Quaternion;

	struct __declspec(align(16)) Matrix2;
	struct __declspec(align(16)) Matrix4;

	class Radians;
	class Degrees;

	using Vec2 = Vector2;
	using Vec4 = Vector4;
	using Pt3D = Vector4;
	using Mat2 = Matrix2;
	using Mat4 = Matrix4;
}


#endif		// INCLUDE GUARD

