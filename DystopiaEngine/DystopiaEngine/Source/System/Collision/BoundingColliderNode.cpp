#include "System/Collision/BoundingColliderNode.h"

namespace Dystopia
{
	BoundingColliderNode::BoundingColliderNode()
		:mBoundingCircle{}, mCollider{nullptr}, mChildrenNode{{nullptr}}
	{
	}

	unsigned BoundingColliderNode::GetNumPotentialContact(unsigned _limit, PotentialContacts* _pPotentialContacts) const
	{
		
		return (isLeaf() || !_limit) ? 0 : mChildrenNode[0]->GetPotentialContactWith(mChildrenNode[1], _pPotentialContacts, _limit);
	}

	unsigned BoundingColliderNode::GetPotentialContactWith(BoundingColliderNode* _other,
		                                                   PotentialContacts*    _pPotentialContacts,
		                                                   unsigned              _limit) const
	{
		/*If current Node is not overlapping the other node, there is no collision*/
		if (isOverlapping(*_other) || _limit == 0) return 0;
		/*If Both Nodes are Leaf, there is a potential collision*/
		else if (isLeaf() && _other->isLeaf())
		{
			_pPotentialContacts->mContacts[0] = mCollider;
			_pPotentialContacts->mContacts[1] = _other->mCollider;
			return 1;
		}
		/*
		 * Use the Bounding Circle with the largest Size as there are more colliders inside the Circle
		 */
		else if(_other->isLeaf() || (!isLeaf() && mBoundingCircle.GetRadius() > _other->mBoundingCircle.GetRadius()))
		{
			/*Recurse into self*/
			unsigned count = mChildrenNode[0]->GetPotentialContactWith(_other, _pPotentialContacts, _limit);

			if (_limit > count)
				return count + mChildrenNode[1]->GetPotentialContactWith(_other, _pPotentialContacts + count, _limit - count);
			else
				return count;
		}
		else
		{
			/*Recurse into other*/
			unsigned count = GetPotentialContactWith(_other->mChildrenNode[0], _pPotentialContacts, _limit);

			if (_limit > count)
				return count + _other->mChildrenNode[1]->GetPotentialContactWith(_other, _pPotentialContacts + count, _limit - count);
			else
				return count;
		}
	}

	bool BoundingColliderNode::isOverlapping(const BoundingColliderNode& _rhs) const
	{
		return mBoundingCircle.isOverlapping(_rhs.mBoundingCircle);
	}

	bool BoundingColliderNode::isLeaf() const
	{
		return mCollider;
	}

	void BoundingColliderNode::Insert(Collider* _pCollider, BroadPhaseCircle const& _circle)
	{

	}
}
