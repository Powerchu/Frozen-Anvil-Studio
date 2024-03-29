/* HEADER *********************************************************************************/
/*!
\file	MathLib.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Convenient header file that helps you to include all Math Library Headers!

	All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
	Reproduction or disclosure of this file or its contents without the
	prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATH_LIB_H_
#define _MATH_LIB_H_

#include "Math/MathUtility.h"

#include "Math/Angles.h"
#include "Math/Vectors.h"

#if _WIN64

#include "Math/Vectors.h"
#include "Math/Quaternion.h"
#include "Math/Matrix2.h"
#include "Math/Matrix4.h"
#include "Math/PerlinNoise.h"

#else		// Fallback for x86 Mode

#endif



#endif		// INCLUDE GUARD

