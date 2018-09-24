#pragma once
#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "DataStructure/AutoArray.h"
#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"

namespace Dystopia
{

	class Collider;
	class AABB;
	class Convex;
	class Circle;

	class CollisionSystem : public Systems, public ComponentDonor<AABB>,
											public ComponentDonor<Convex>,	  
											public ComponentDonor<Circle>
	{
	public:

		CollisionSystem();
		~CollisionSystem();


		virtual void PreInit(void) override;
		virtual bool Init(void) override;
		virtual void PostInit(void) override;

		virtual void FixedUpdate(float) override;
		virtual void Update(float) override;
		virtual void Shutdown(void) override;

		virtual void LoadDefaults(void) override { };
		virtual void LoadSettings(TextSerialiser&) override { };

		void InsertCollider(Collider * const & _Col);

		bool AABBvsAABB(Collider  * const & _ColA,
			            Collider  * const & _ColB) const;

		bool ConvexVsConvex(Collider  * const & _ColA,
			                Collider  * const & _ColB) const;

		bool CircleVsCircle(Collider * const & _ColA,
							Collider * const & _Colb) const;

		bool CircleVsAABB(Collider * const & _ColA,
							Collider * const & _ColB) const;

		bool CircleVsConvex(Collider * const & _ColA,
							Collider * const & _ColB) const;
	};
}

#endif