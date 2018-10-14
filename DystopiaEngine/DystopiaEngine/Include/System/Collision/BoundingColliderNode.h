#pragma once
#ifndef BOUNDING_COLLIDER_NODE_H
#define BOUNDING_COLLIDER_NODE_H

#include "System/Collision/BroadPhaseCircle.h"


namespace Dystopia
{
	struct Collider;

	struct PotentialContacts
	{
		Collider * mContacts[2];
	};

	struct BoundingColliderNode
	{
		BoundingColliderNode();
		BoundingColliderNode(Collider * _MyCollider, BroadPhaseCircle _Circle, BoundingColliderNode * _parent);
		unsigned GetNumPotentialContact(unsigned _limit, PotentialContacts * _pPotentialContacts) const;
		unsigned GetPotentialContactWith(BoundingColliderNode * _other,
			                             PotentialContacts    * _pPotentialContacts,
			                             unsigned               _limit) const;

		bool     isOverlapping(const BoundingColliderNode & _rhs) const;

		bool     isLeaf() const;

		void     Insert(Collider * _pCollider, BroadPhaseCircle const & _circle);


		~BoundingColliderNode();

	private:
		BoundingColliderNode * mChildrenNode[2];
		BoundingColliderNode * mParent;
		BroadPhaseCircle       mBoundingCircle;
		Collider             * mCollider;
	};
}



#endif