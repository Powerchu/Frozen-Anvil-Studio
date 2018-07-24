#pragma once
#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include "DataStructure\AutoArray.h"

namespace Dystopia
{
	class Collider;

	class CollisionSystem
	{
	public:

		static CollisionSystem * GetInstance();

		void Init();

		void Update(const float & _dt);

		void FixedUpdate(const float & _dt);

		void Terminate();

		void InsertCollider(Collider * const & _Col);

		bool AABBToAABBCollision(Collider const * const & _ColA,
			                     Collider const * const & _ColB)const;

		bool ConvexToConvexCollision(Collider const * const & _ColA,
			                         Collider const * const & _ColB)const;
	private:

		static CollisionSystem * mpManager;

		~CollisionSystem();

		CollisionSystem();

		AutoArray<Collider * > mArrOfCollider;
	};
}


#endif