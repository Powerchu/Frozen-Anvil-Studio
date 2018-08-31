#pragma once
#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "DataStructure\AutoArray.h"
#include "System\Base\Systems.h"

namespace Dystopia
{
	class Collider;

	class CollisionSystem : public Systems
	{
	public:

		~CollisionSystem();

		CollisionSystem();

		virtual void PreInit(void);
		virtual bool Init(void);
		virtual void PostInit(void);

		virtual void FixedUpdate(float);
		virtual void Update(float);
		virtual void Shutdown(void);

		virtual void LoadDefaults(void) { };
		virtual void LoadSettings(TextSerialiser&) { };

		void InsertCollider(Collider * const & _Col);

		bool AABBToAABBCollision(Collider const * const & _ColA,
			                     Collider const * const & _ColB)const;

		bool ConvexToConvexCollision(Collider const * const & _ColA,
			                         Collider const * const & _ColB)const;
	private:

		AutoArray<Collider * > mArrOfCollider;
	};
}


#endif