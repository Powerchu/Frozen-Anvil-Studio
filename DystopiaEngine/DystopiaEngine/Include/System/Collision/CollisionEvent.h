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


		using mpcGobj = GameObject const * const;

		CollisionEvent(GameObject * _Target);


		Math::Point3D  mCollisionPoint;

		Math::Vector3D mEdgeVector;
		Math::Vector3D mEdgeNormal;
		double         mPeneDepth;
		mpcGobj		   mCollidedWith;

	};
}



#endif