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

	class CollisionSystem : 
	public Systems, 
	public ComponentDonor<AABB>,
	public ComponentDonor<Convex>
	{
	public:

		~CollisionSystem();

		CollisionSystem();

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

	private:
		//AutoArray<Collider*> mArrOfCollider;
	};

}


#endif