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
		using mpcGobj = GameObject * ;
		using u64int  = unsigned long long;
		CollisionEvent(GameObject* _Self, GameObject * _Target);

		mpcGobj				mThisCollider;		//Gameobject Collision This
		mpcGobj				mCollidedWith;		//Gameobject Collision That

		Math::Point3D		mCollisionPoint;	//Contact Point
		Math::Vector4		mEdgeVector;		//Contact Vector
		Math::Vector4		mEdgeNormal;		//Contact Normal

		float				mfPeneDepth;		//Previous Frame penetration
		float				mfRestitution;		//Total summed (min) restitution
		float				mfStaticFrictionCof;		//Total friction averaged (sqrt)
		float				mfDynamicFrictionCof;
		u64int              mOtherID;

		void ApplyImpulse();
		void ApplyPenetrationCorrection(int iter = 1) const;

		bool operator==(CollisionEvent const & _rhs)         const;
		bool operator==(unsigned long long _mCollidedWithID) const;
		bool operator==(mpcGobj _pCollidedWith)              const;
		bool operator==(const GameObject * const _pCollidedWith) const;

	private:
		Math::Vec3D			mCumulativeImpulse;
	};
}



#endif