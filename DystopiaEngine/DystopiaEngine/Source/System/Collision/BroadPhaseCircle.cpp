#include "System\Collision\BroadPhaseCircle.h"

Dystopia::BroadPhaseCircle::BroadPhaseCircle(float _Radius, Math::Point3D _Origin)
	:mRadius{_Radius} , mOrigin{_Origin}
{

}

Dystopia::BroadPhaseCircle::BroadPhaseCircle(BroadPhaseCircle _c1, BroadPhaseCircle _c2)
	:mOrigin{Math::MakePoint3D(0,0,0)}, mRadius{0.f}
{
	Math::Vec3D vCentreOffset = _c2.mOrigin - _c1.mOrigin;
	float       fRadiusDiff = _c1.mRadius - _c2.mRadius;

	if(fRadiusDiff * fRadiusDiff >= vCentreOffset.MagnitudeSqr())
	{
		if(_c1.mRadius > _c2.mRadius)
		{
			mOrigin = _c1.mOrigin;
			mRadius = _c1.mRadius;
		}
		else
		{
			mOrigin = _c2.mOrigin;
			mRadius = _c2.mRadius;
		}
	}
	else
	{
		float distance = vCentreOffset.Magnitude();
		/*Find a radius big enough that it enclose both bounding circle*/
		mRadius        = (_c1.mRadius + _c2.mRadius + distance) * (0.5f);
		mOrigin        = _c1.mOrigin;
		if(distance)
		{
			mOrigin += vCentreOffset * ((mRadius - _c1.mRadius) / distance);
		}
		
	}
}

bool Dystopia::BroadPhaseCircle::isOverlapping(BroadPhaseCircle const& _rhs) const
{
	return (_rhs.mOrigin - mOrigin).MagnitudeSqr() < (_rhs.mRadius + mRadius) * (_rhs.mRadius + mRadius);
}

float Dystopia::BroadPhaseCircle::GetRadius() const
{
	return mRadius;
}
