/* HEADER *********************************************************************************/
/*!
\file	Angles.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
Contains commonly used Math functions to make life easier.
Tell me if more is needed and I'll add them! (or you can DIY)

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATH_ANGLE_H_
#define _MATH_ANGLE_H_

#include "MathUtility.h"

#define NO_UNDERSCORE 1

namespace Math
{
	class Angle
	{
	public:
		float constexpr Radians(void) const
		{
			return mfAngle;
		}

		float constexpr Degrees(void) const
		{
			return RadToDeg(mfAngle);
		}

		explicit constexpr Angle(float _angle) :
			mfAngle{ _angle }
		{}

	private:
		float mfAngle;
	};

	class Radians : public Angle
	{
	public:
		explicit constexpr Radians(float _angle) :
			Angle{ _angle }
		{}
	};

	class Degrees : public Angle
	{
	public:
		explicit constexpr Degrees(float _angle) :
			Angle{ DegToRad(_angle) }
		{}
	};
}

// For some reason Visual Studios don't like these when they are not in global namespace

#if NO_UNDERSCORE
#pragma warning(push)
#pragma warning(disable : 4455)

constexpr Math::Radians operator ""rad(long double _angle)
{
	return Math::Radians(static_cast<float>(_angle));
}

constexpr Math::Degrees operator ""deg(long double _angle)
{
	return Math::Degrees(static_cast<float>(_angle));
}

constexpr Math::Radians operator ""rad(unsigned long long _angle)
{
	return Math::Radians(static_cast<float>(_angle));
}

constexpr Math::Degrees operator ""deg(unsigned long long _angle)
{
	return Math::Degrees(static_cast<float>(_angle));
}

#pragma warning(pop)
#else

constexpr Math::Radians operator ""_rad(long double _angle)
{
	return Math::Radians(static_cast<float>(_angle));
}

constexpr Math::Degrees operator ""_deg(long double _angle)
{
	return Math::Degrees(static_cast<float>(_angle));
}

constexpr Math::Radians operator ""_rad(unsigned long long _angle)
{
	return Math::Radians(static_cast<float>(_angle));
}

constexpr Math::Degrees operator ""_deg(unsigned long long _angle)
{
	return Math::Degrees(static_cast<float>(_angle));
}

#endif


#undef NO_UNDERSCORE

#endif		// INCLUDE GUARD

