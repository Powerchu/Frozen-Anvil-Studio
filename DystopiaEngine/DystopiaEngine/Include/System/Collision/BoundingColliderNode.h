#pragma once
#ifndef BOUNDING_COLLIDER_NODE_H
#define BOUNDING_COLLIDER_NODE_H

#include "System/Collision/BroadPhaseCircle.h"


namespace Dystopia
{
	class Collider;

	struct PotentialContacts
	{
		PotentialContacts() : mContacts{ nullptr } {}
		Collider * mContacts[2];
	};

	struct BoundingColliderNode
	{
		BoundingColliderNode();
		BoundingColliderNode(Collider * _MyCollider, BroadPhaseCircle _Circle, BoundingColliderNode * _parent);
		unsigned GetNumPotentialContact(unsigned _limit, PotentialContacts * _pPotentialContacts)     const;
		unsigned GetPotentialContactWith(BoundingColliderNode * _other,
			                             PotentialContacts    * _pPotentialContacts,
			                             unsigned               _limit) const;
		unsigned GetChildrenPotentialContact(unsigned _limit, PotentialContacts * _pPotentialContacts) const;

		bool     isOverlapping(const BoundingColliderNode & _rhs) const;

		bool     isLeaf() const;

		void     Insert(Collider * _pCollider, BroadPhaseCircle const & _circle);

		void     UpdateNodes();

		void     RecalculateBoundingCircle();
		~BoundingColliderNode();

	private:
		BoundingColliderNode * mChildrenNode[2];
		BoundingColliderNode * mParent;
		BroadPhaseCircle       mBoundingCircle;
		Collider             * mCollider;
	};
}



#endif