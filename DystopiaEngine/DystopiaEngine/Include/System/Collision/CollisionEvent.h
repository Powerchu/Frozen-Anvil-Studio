#pragma once
#ifndef COLLISION_EVENT_H
#define COLLISION_EVENT_H

#include "Math/MathLib.h"


namespace Dystopia
{
	class GameObject;

	enum class CollisionType : unsigned
	{
		eEnvelop,    /*Collider is completely inside another collider*/
		eIntersect,  /*Collider edge is inside another collider*/
	};

	struct CollisionEvent
	{
		using mpcGobj = GameObject const *;
		using u64int  = unsigned long long;
		CollisionEvent(GameObject* _Self, GameObject * _Target);

		Math::Point3D		mCollisionPoint;	//Contact Point

		Math::Vector3D		mEdgeVector;		//Contact Vector
		Math::Vector3D		mEdgeNormal;		//Contact Normal
		double				mdPeneDepth;		//Previous Frame penetration
		mpcGobj				mThisCollider;		//Gameobject Collision Who?
		mpcGobj				mCollidedWith;		//Gameobject Collision Who?
		float				mfRestitution;		//Total summed (min) restitution
		float				mfStaticFrictionCof;		//Total friction averaged (sqrt)
		float				mfDynamicFrictionCof;
		u64int              mOtherID;

		void ApplyImpulse(void);
		void ApplyPenetrationCorrection(void);

		bool operator==(CollisionEvent const & _rhs)         const;
		bool operator==(unsigned long long _mCollidedWithID) const;
		bool operator==(mpcGobj _pCollidedWith)              const;
	private:
		Math::Vec3D			mCumulativeImpulse;
	};
}



#endif