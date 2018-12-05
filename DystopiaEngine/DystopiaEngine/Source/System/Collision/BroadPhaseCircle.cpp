/* HEADER *********************************************************************************/
/*!
\file	BroadPhaseCircle.cpp
\author keith.goh (100%)
\par    email: keith.goh\@digipen.edu
\brief


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Collision\BroadPhaseCircle.h"
#include <utility>

Dystopia::BroadPhaseCircle::BroadPhaseCircle(float _Radius, const Math::Point3D& _Origin)
	:mRadius{_Radius} , mOrigin{_Origin}
{

}

Dystopia::BroadPhaseCircle::BroadPhaseCircle(BroadPhaseCircle _c1, BroadPhaseCircle _c2)
	:mOrigin{Math::MakePoint3D(0,0,0)}, mRadius{0.f}
{
	Math::Vec3D vCentreOffset = _c2.mOrigin - _c1.mOrigin;
	float       fRadiusDiff   = _c1.mRadius - _c2.mRadius;

	if(fRadiusDiff * fRadiusDiff >= vCentreOffset.MagnitudeSqr())
	{
		if(_c1.mRadius > _c2.mRadius)
		{
			mOrigin = _c1.mOrigin;
			mRadius = _c1.GetRadius();
		}
		else
		{
			mOrigin = _c2.mOrigin;
			mRadius = _c2.GetRadius();
		}
	}
	else
	{
		float distance = vCentreOffset.Magnitude();
		/*Find a radius big enough that it enclose both bounding circle*/
		mRadius        = (_c1.GetRadius() + _c2.GetRadius() + Math::Abs(distance)) * (0.5f);
		mOrigin        = _c1.mOrigin;
		if(distance)
		{
			mOrigin += vCentreOffset * ((mRadius - _c1.mRadius) / distance);
		}
		
	}
}

bool Dystopia::BroadPhaseCircle::isOverlapping(BroadPhaseCircle const& _rhs) const
{
	return (_rhs.mOrigin - mOrigin).MagnitudeSqr() <= (_rhs.GetRadius() + GetRadius()) * (_rhs.GetRadius() + GetRadius());
}

float Dystopia::BroadPhaseCircle::GetRadius() const
{
	return Math::Abs(mRadius);
}

float Dystopia::BroadPhaseCircle::GetRadiusGrowth(BroadPhaseCircle _other) const
{
	return BroadPhaseCircle{ *this, Ut::Move(_other)}.GetRadius() - GetRadius();
}

Math::Point3D Dystopia::BroadPhaseCircle::GetOrigin() const
{
	return mOrigin;
}
