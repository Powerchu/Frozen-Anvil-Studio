#include "System/Collision/BoundingColliderNode.h"

namespace Dystopia
{
	BoundingColliderNode::BoundingColliderNode()
		:mBoundingCircle{}, mCollider{nullptr}, mChildrenNode{{nullptr}}, mParent{nullptr}
	{
	}

	BoundingColliderNode::BoundingColliderNode(Collider * _MyCollider, BroadPhaseCircle _Circle, BoundingColliderNode * _parent)
		: mBoundingCircle{ _Circle }, mCollider{ _MyCollider }, mChildrenNode{ {nullptr} }, mParent{ _parent }
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
		/*If current node is leaf, spawn 2 child and assign the new collider to child 2*/
		if (isLeaf())
		{
			/*First Leaf will be current Node*/
			mChildrenNode[0] = new BoundingColliderNode{ mCollider, mBoundingCircle, this };
			/*Second Leaf will be new Node*/
			mChildrenNode[1] = new BoundingColliderNode{ _pCollider, _circle, this};
			/*Remove Collider because this node is now a branch*/
			mCollider        = nullptr;
			/*Recalculate this Node Bounding Circle*/
			mBoundingCircle  = BroadPhaseCircle{_circle, mBoundingCircle};
		}
		else
		{
			if (mChildrenNode[0]->mBoundingCircle.GetRadiusGrowth(_circle) < mChildrenNode[1]->mBoundingCircle.GetRadiusGrowth(_circle))
			{
				mChildrenNode[0]->Insert(_pCollider, _circle);
			}
			else
			{
				mChildrenNode[1]->Insert(_pCollider, _circle);
			}
		}
	}
	BoundingColliderNode::~BoundingColliderNode()
	{
		if (mParent)
		{
			/*Make Parent be the same as sibling since this Node and all children will be dead*/
			auto * Sibling = mParent->mChildrenNode[0] == this ? mParent->mChildrenNode[1] : mParent->mChildrenNode[0];
			/*Assign parent Bounding Circle to sibling circle*/
			mParent->mBoundingCircle = Sibling->mBoundingCircle;
			/*Assign parent's children to sibling children*/
			for (unsigned i = 0; i < 2; ++i)
				mParent->mChildrenNode[i] = Sibling->mChildrenNode[i];
			/*Assign parent collider to Sibling collider*/
			mParent->mCollider = Sibling->mCollider;

			/*Remove all pointers from Sibling copy to prevent the pointers from being deleted*/
			Sibling->mChildrenNode[0] = Sibling->mChildrenNode[1] = nullptr;
			Sibling->mParent          = nullptr;
			Sibling->mCollider        = nullptr;

			delete Sibling;
		}
		for (auto ptr : mChildrenNode)
		{
			ptr->mParent = nullptr;
			delete ptr;
		}
	}
}
