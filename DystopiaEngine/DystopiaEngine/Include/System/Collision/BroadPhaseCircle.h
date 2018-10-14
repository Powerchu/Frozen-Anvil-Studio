#pragma once
#ifndef BOARD_PHASE_CIRCLE_H
#define BOARD_PHASE_CIRLCE_H

#include "Math/MathLib.h"
#include "Globals.h"

namespace Dystopia
{
	struct _DLL_EXPORT BroadPhaseCircle
	{
		/*Default Constructor*/
		BroadPhaseCircle() {}
		/*Create a Circle of with Radius = _Radius and origin at _Origin*/
		BroadPhaseCircle(float _Radius, Math::Point3D _Origin);

		/*Generate a BroadPhaseCircle with 2 existing BroadPhaseCircle*/
		BroadPhaseCircle(BroadPhaseCircle _c1, BroadPhaseCircle _c2);

		/*Check if 2 BoardPhase is overlapping*/
		bool isOverlapping(BroadPhaseCircle const &_rhs) const;

		float GetRadius() const;
		float GetRadiusGrowth(BroadPhaseCircle) const;
		Math::Point3D GetOrigin() const;

	private:
		float mRadius;
		Math::Point3D mOrigin;
	};

}

#endif