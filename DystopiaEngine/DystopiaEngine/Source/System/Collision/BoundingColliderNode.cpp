#include "System/Collision/BoundingColliderNode.h"
#include "Component/Collider.h"
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
		unsigned Total = 0;

		if (!isLeaf() && _limit > 0)
			Total += mChildrenNode[0]->GetPotentialContactWith(mChildrenNode[1], _pPotentialContacts, _limit);
		if (!isLeaf() && _limit - Total > 0)
			Total += mChildrenNode[0]->GetNumPotentialContact(_limit - Total, _pPotentialContacts + Total);
		if (!isLeaf() && _limit - Total > 0)
			Total += mChildrenNode[1]->GetNumPotentialContact(_limit - Total, _pPotentialContacts + Total);

		return Total;
	}

	unsigned BoundingColliderNode::GetPotentialContactWith(BoundingColliderNode* _other,
		                                                   PotentialContacts*    _pPotentialContacts,
		                                                   unsigned              _limit) const
	{
		if (nullptr != _other)
		{
			/*If current Node is not overlapping the other node, there is no collision*/
			if (!isOverlapping(*_other) || _limit == 0) return 0;
			/*If Both Nodes are Leaf, there is a potential collision*/
			else if (isLeaf() && _other->isLeaf())
			{
				_pPotentialContacts->mContacts[0] = mCollider;
				_pPotentialContacts->mContacts[1] = _other->mCollider;
				return 1;
			}
			else if (_other->isLeaf())
			{
				/*Recurse into self*/
				unsigned count = mChildrenNode[0]->GetPotentialContactWith(_other, _pPotentialContacts, _limit);

				if (_limit > count)
					return count + mChildrenNode[1]->GetPotentialContactWith(_other, _pPotentialContacts + count, _limit - count);
				else
					return count;
			}
			else if (isLeaf())
			{
				/*Recurse into other*/
				unsigned count = GetPotentialContactWith(_other->mChildrenNode[0], _pPotentialContacts, _limit);

				if (_limit > count)
					return count + GetPotentialContactWith(_other->mChildrenNode[1], _pPotentialContacts + count, _limit - count);
				else
					return count;
			}
			else
			{
				unsigned count = 0;
				unsigned Total = count = mChildrenNode[0]->GetPotentialContactWith(_other->mChildrenNode[0], _pPotentialContacts, _limit);

				if (_limit - Total)
					Total += mChildrenNode[0]->GetPotentialContactWith(_other->mChildrenNode[1], _pPotentialContacts + Total, _limit - Total);
				if (_limit - Total)
					Total += mChildrenNode[1]->GetPotentialContactWith(_other->mChildrenNode[0], _pPotentialContacts + Total, _limit - Total);
				if (_limit - Total)
					Total += mChildrenNode[1]->GetPotentialContactWith(_other->mChildrenNode[1], _pPotentialContacts + Total, _limit - Total);
				return Total;
			}
		}
		
		/*Leave this in. Dont delete yet*/
		/*
		 * Use the Bounding Circle with the largest Size as there are more colliders inside the Circle
		 */
		//else if(_other->isLeaf() || (!isLeaf() && mBoundingCircle.GetRadius() > _other->mBoundingCircle.GetRadius()))
		//{
		//	/*Recurse into self*/
		//	unsigned count = mChildrenNode[0]->GetPotentialContactWith(_other, _pPotentialContacts, _limit);

		//	if (_limit > count)
		//		return count + mChildrenNode[1]->GetPotentialContactWith(_other, _pPotentialContacts + count, _limit - count);
		//	else
		//		return count;
		//}
		//else
		//{
		//	/*Recurse into other*/
		//	unsigned count = GetPotentialContactWith(_other->mChildrenNode[0], _pPotentialContacts, _limit);

		//	if (_limit > count)
		//		return count + GetPotentialContactWith(_other->mChildrenNode[1], _pPotentialContacts + count, _limit - count);
		//	else
		//		return count;
		//}
	}

	unsigned BoundingColliderNode::GetChildrenPotentialContact(unsigned _limit, PotentialContacts * _pPotentialContacts) const
	{
		if (isLeaf())
			return 0;

		unsigned count = 0;
		unsigned total = count = mChildrenNode[0]->GetPotentialContactWith(mChildrenNode[1], _pPotentialContacts, _limit);
		for (auto & elem : mChildrenNode)
		{
			total += count = elem->GetChildrenPotentialContact(_limit - count, _pPotentialContacts);
		}
		return total;
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
		else if (!mCollider && (mChildrenNode[0] == nullptr && mChildrenNode[1] == nullptr))
		{
			mCollider       = _pCollider;
			mBoundingCircle = _circle;
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
	void BoundingColliderNode::UpdateNodes()
	{
		if (isLeaf())
		{
			mBoundingCircle = mCollider->GetBroadPhaseCircle();
			if (mParent)
				mParent->RecalculateBoundingCircle();
		}
		else
		{
			for (auto & elem : mChildrenNode)
				elem->UpdateNodes();
		}
	}
	void BoundingColliderNode::RecalculateBoundingCircle()
	{
		if (!isLeaf())
		{
			if (mChildrenNode[0] == nullptr || mChildrenNode[1] == nullptr)
			{
				auto * ValidChildren = mChildrenNode[0] == nullptr ? mChildrenNode[1] : mChildrenNode[0];
				mBoundingCircle = BroadPhaseCircle{ValidChildren->mBoundingCircle.GetRadius(),ValidChildren->mBoundingCircle.GetOrigin()};
			}
			else
			{
				mBoundingCircle = BroadPhaseCircle{ mChildrenNode[0]->mBoundingCircle, mChildrenNode[1]->mBoundingCircle };
			}
		}

		if (mParent)
			mParent->RecalculateBoundingCircle();
	}

	bool BoundingColliderNode::isEmpty() const
	{
		return (mChildrenNode[0] == nullptr && mChildrenNode[1] == nullptr);
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
			if(ptr == nullptr)
				continue;
			ptr->mParent = nullptr;
			delete ptr;
		}
	}
}
