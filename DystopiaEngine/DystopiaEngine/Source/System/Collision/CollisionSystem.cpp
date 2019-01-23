/* HEADER *********************************************************************************/
/*!
\file	CollisionSystem.cpp
\author keith.goh (100%)
\par    email: keith.goh\@digipen.edu
\brief

	Collision System Implementation


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/ColliderList.h"
#include "Component/RigidBody.h"
#include "Component/Transform.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "Behaviour/Behaviour.h"
#include <utility>
#include <map>
#include "System/Graphics/Shader.h"

namespace Dystopia
{
	void CollisionSystem::PreInit(void)
	{
		// empty
	}
	bool CollisionSystem::Init()
	{
		return true;
	}

	void CollisionSystem::PostInit(void)
	{
		//empty
	}

	void CollisionSystem::Update(float)
	{

	}

	void CollisionSystem::PostUpdate()
	{
		for (auto & col : ComponentDonor<Convex>::mComponents)
		{
			if (col.GetFlags() & FLAG_REMOVE)
				ComponentDonor<Convex>::mComponents.Remove(&col);
		}
		for (auto & col : ComponentDonor<AABB>::mComponents)
		{
			if (col.GetFlags() & FLAG_REMOVE)
				ComponentDonor<AABB>::mComponents.Remove(&col);
		}
		for (auto & col : ComponentDonor<Circle>::mComponents)
		{
			if (col.GetFlags() & FLAG_REMOVE)
				ComponentDonor<Circle>::mComponents.Remove(&col);
		}
		for (auto & col : ComponentDonor<PointCollider>::mComponents)
		{
			if (col.GetFlags() & FLAG_REMOVE)
				ComponentDonor<PointCollider>::mComponents.Remove(&col);
		}
	}

	void CollisionSystem::FixedUpdate(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Collision System", "Update" };

		BoundingColliderNode		mCollisionTree;
		PotentialContacts	        ArrayContacts[1024];
		unsigned				    ContactCount = 0;

		using CollisionTable = std::pair<eColliderType, eColliderType>;
		using fpCollisionResolution = bool(CollisionSystem::*)(Collider  * const &, Collider  * const &)const;
		using CollisionTableMap = std::map < CollisionTable, fpCollisionResolution>;

		static CollisionTableMap CollisionFuncTable = []()->CollisionTableMap
		{
			CollisionTableMap i
			{
			{ CollisionTable{ eColliderType::AABB    ,eColliderType::AABB }    ,&CollisionSystem::AABBvsAABB },
			{ CollisionTable{ eColliderType::AABB    ,eColliderType::CONVEX }  ,&CollisionSystem::ConvexVsConvex },
			{ CollisionTable{ eColliderType::CONVEX  ,eColliderType::AABB }    ,&CollisionSystem::ConvexVsConvex },
			{ CollisionTable{ eColliderType::CONVEX  ,eColliderType::CONVEX }  ,&CollisionSystem::ConvexVsConvex },
			{ CollisionTable{ eColliderType::CIRCLE  ,eColliderType::CIRCLE }  ,&CollisionSystem::CircleVsCircle },
			{ CollisionTable{ eColliderType::CIRCLE,  eColliderType::AABB }    ,&CollisionSystem::CircleVsAABB },
			{ CollisionTable{ eColliderType::AABB,    eColliderType::CIRCLE }  ,&CollisionSystem::AABBvsCircle },
			{ CollisionTable{ eColliderType::CIRCLE,  eColliderType::CONVEX }  ,&CollisionSystem::CircleVsConvex },
			{ CollisionTable{ eColliderType::CONVEX,  eColliderType::CIRCLE }  ,&CollisionSystem::ConvexVsCircle },
			{ CollisionTable{ eColliderType::POINT,   eColliderType::POINT }   ,&CollisionSystem::PointVsPoint },
			{ CollisionTable{ eColliderType::POINT,   eColliderType::CONVEX }  ,&CollisionSystem::PointVsConvex },
			{ CollisionTable{ eColliderType::POINT,   eColliderType::CIRCLE }  ,&CollisionSystem::PointVsCircle },
			{ CollisionTable{ eColliderType::CONVEX,  eColliderType::POINT }   ,&CollisionSystem::ConvexVsPoint },
			{ CollisionTable{ eColliderType::CIRCLE,  eColliderType::POINT }   ,&CollisionSystem::CircleVsPoint }
			};
			return i;
		}();

		AutoArray<Collider *> mColliders;

		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				elem.Update(_dt);
				elem.ClearCurrentCollisionEvent(); //clear collision table
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding(false);
				mColliders.push_back(&elem);
				mCollisionTree.Insert(&elem, elem.GetBroadPhaseCircle());
			}
		}

		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				//elem.Recompute();
				elem.ClearCurrentCollisionEvent(); //clear collision table
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding(false);
				mColliders.push_back(&elem);
				mCollisionTree.Insert(&elem, elem.GetBroadPhaseCircle());
			}

		}

		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				elem.ClearCurrentCollisionEvent(); //clear collision table
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding(false);
				mColliders.push_back(&elem);
				mCollisionTree.Insert(&elem, elem.GetBroadPhaseCircle());
			}
		}

		for (auto & elem : ComponentDonor<PointCollider>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				elem.ClearCurrentCollisionEvent(); //clear collision table
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding(false);
				mColliders.push_back(&elem);
				mCollisionTree.Insert(&elem, elem.GetBroadPhaseCircle());
			}
		}

		if (!mCollisionTree.isEmpty())
			ContactCount = mCollisionTree.GetNumPotentialContact(1024, ArrayContacts);

		for (unsigned i = 0; i < ContactCount; ++i)
		{
			if (nullptr == ArrayContacts[i].mContacts[0] || nullptr == ArrayContacts[i].mContacts[1]) continue;
			Collider * bodyA = ArrayContacts[i].mContacts[0];
			Collider * bodyB = ArrayContacts[i].mContacts[1];
			const auto ownerA = bodyA->GetOwner();
			const auto ownerB = bodyB->GetOwner();
			const auto rigidA = ownerA->GetComponent<RigidBody>();
			const auto rigidB = ownerB->GetComponent<RigidBody>();

			if (static_cast<Collider *>(bodyA) != static_cast<Collider *>(bodyB))
			{
				if (rigidA && rigidB)
				{
					// if both bodies are static, continue
					if (rigidA->Get_IsStaticState() && rigidB->Get_IsStaticState()) 
						continue;
					// If the colliders belong to the same owner, continue
					if (ownerA == ownerB) 
						continue;
				}

				const auto pair_key1 = std::make_pair(bodyA->GetColliderType(), (bodyB)->GetColliderType());
				const auto pair_key2 = std::make_pair(bodyB->GetColliderType(), (bodyA)->GetColliderType());
				for (auto & key : CollisionFuncTable)
				{
					if (key.first == pair_key1)
					{
						(this->*key.second)(bodyA, bodyB);
						bodyA->SetColliding(bodyA->Collider::HasCollision());

						break;
					}
				}
				for (auto & key : CollisionFuncTable)
				{
					if (key.first == pair_key2)
					{
						(this->*key.second)(bodyB, bodyA);
						bodyB->SetColliding(bodyB->Collider::HasCollision());

						break;
					}
				}
			}
		}




		//for (auto & bodyA : mColliders)
		//{
		//	const auto ownerA = bodyA->GetOwner();
		//	for (auto & bodyB : mColliders)
		//	{
		//		const auto ownerB = bodyB->GetOwner();
		//		const auto rigidA = ownerA->GetComponent<RigidBody>();
		//		const auto rigidB = ownerB->GetComponent<RigidBody>();

		//		if (rigidA == nullptr)
		//			bodyA->SetTrigger(true);
		//		if (rigidB == nullptr)
		//			bodyB->SetTrigger(true);

		//		if (static_cast<Collider *>(bodyA) != static_cast<Collider *>(bodyB))
		//		{
		//			if (rigidA && rigidB)
		//			{
		//				if (rigidA->Get_IsStaticState() && rigidB->Get_IsStaticState())
		//					continue;
		//				if (ownerA == ownerB)
		//					continue;
		//			}
		//			const auto pair_key = std::make_pair(bodyA->GetColliderType(), (bodyB)->GetColliderType());
		//			for (auto & key : CollisionFuncTable)
		//			{
		//				if (key.first == pair_key)
		//				{
		//					(this->*key.second)(bodyA, bodyB);
		//					bodyB->SetColliding(bodyB->Collider::HasCollision());
		//					bodyA->SetColliding(bodyA->Collider::HasCollision());
		//					break;
		//				}
		//			}
		//		}
		//	}
		//}

		for (auto & elem : mColliders)
		{
			const auto rigid = elem->GetOwner()->GetComponent<RigidBody>();
			if (rigid == nullptr)
				elem->SetTrigger(true);

			elem->InformOtherComponents();
		}
	}

	void CollisionSystem::Shutdown()
	{

	}

	bool CollisionSystem::AABBvsAABB(Collider * const & _ColA, Collider * const & _ColB) const
	{
		const auto col_a = dynamic_cast<AABB * const>(_ColA);
		const auto col_b = dynamic_cast<AABB * const>(_ColB);

		return col_a->isColliding(col_b);
	}

	bool CollisionSystem::ConvexVsConvex(Collider * const & _ColA, Collider * const & _ColB) const
	{
		const auto col_a = dynamic_cast<Convex * const>(_ColA);
		const auto col_b = dynamic_cast<Convex * const>(_ColB);

		return col_a->isColliding(col_b);
	}

	bool CollisionSystem::CircleVsCircle(Collider* const& _ColA, Collider* const& _ColB) const
	{
		const auto col_a = dynamic_cast<Circle * const>(_ColA);
		const auto col_b = dynamic_cast<Circle * const>(_ColB);

		return col_a->isColliding(col_b);
	}

	bool CollisionSystem::CircleVsAABB(Collider * const & _ColA, Collider * const & _ColB) const
	{
		UNUSED_PARAMETER(_ColA);
		UNUSED_PARAMETER(_ColB);
		return false;
	}

	bool CollisionSystem::AABBvsCircle(Collider * const & _ColA, Collider * const & _ColB) const
	{
		UNUSED_PARAMETER(_ColA);
		UNUSED_PARAMETER(_ColB);
		return false;
	}

	bool CollisionSystem::CircleVsConvex(Collider * const & _ColA, Collider * const & _ColB) const
	{
		Circle   * pCircle;
		Convex   * pConvex;

		if (_ColA->GetColliderType() == eColliderType::CIRCLE)
		{
			pCircle = dynamic_cast<Circle *>(_ColA);
			pConvex = dynamic_cast<Convex *>(_ColB);
		}
		else
		{
			pCircle = dynamic_cast<Circle *>(_ColB);
			pConvex = dynamic_cast<Convex *>(_ColA);
		}
		const bool isColliding = pCircle->isColliding((*pConvex));

		return isColliding;
	}

	bool CollisionSystem::ConvexVsCircle(Collider* const& _ColA, Collider* const& _ColB) const
	{
		Circle   * pCircle;
		Convex   * pConvex;

		if (_ColA->GetColliderType() == eColliderType::CIRCLE)
		{
			pCircle = dynamic_cast<Circle *>(_ColA);
			pConvex = dynamic_cast<Convex *>(_ColB);
		}
		else
		{
			pCircle = dynamic_cast<Circle *>(_ColB);
			pConvex = dynamic_cast<Convex *>(_ColA);

		}
		const bool isColliding = pConvex->isColliding((*pCircle));

		return isColliding;
	}

	bool CollisionSystem::PointVsPoint(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * a, * b;
		a = dynamic_cast<PointCollider *>(_ColA);
		b = dynamic_cast<PointCollider *>(_ColB);
		
		return (a && b) || a->isColliding(b);
	}

	bool CollisionSystem::PointVsConvex(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * a;
		Convex *b;
		a = dynamic_cast<PointCollider *>(_ColA);
		b = dynamic_cast<Convex *>(_ColB);

		return (a && b) || a->isColliding(b);
	}

	bool CollisionSystem::ConvexVsPoint(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * b;
		Convex *a;
		a = dynamic_cast<Convex * const>(_ColA);
		b = dynamic_cast<PointCollider * const>(_ColB);
		
		return (!a && !b) && a->isColliding(b);
	}

	bool CollisionSystem::PointVsCircle(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * a;
		Circle *b;
		a = dynamic_cast<PointCollider *>(_ColA);
		b = dynamic_cast<Circle *>(_ColB);

		return (a && b) || a->isColliding(b);
	}

	bool CollisionSystem::CircleVsPoint(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * b;
		Circle *a;
		a = dynamic_cast<Circle * const>(_ColA);
		b = dynamic_cast<PointCollider * const>(_ColB);

		return (!a && !b) && a->isColliding(b);
	}

	bool CollisionSystem::PointVsAABB(Collider * const & _ColA, Collider * const & _ColB) const
	{
		_ColA;
		_ColB;
		return false;
	}

	bool CollisionSystem::AABBVsPoint(Collider * const & _ColA, Collider * const & _ColB) const
	{
		_ColA;
		_ColB;
		return false;
	}

	AutoArray<Collider*> CollisionSystem::GetAllColliders() const
	{
		AutoArray<Collider*> ToRet;
		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
			ToRet.push_back(&elem);
		}
		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
			ToRet.push_back(&elem);
		}
		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
			ToRet.push_back(&elem);
		}
		return Ut::Move(ToRet);
	}

	bool CollisionSystem::RaycastFirstHit(Math::Vec3D const & _Dir, Math::Point3D const & _mPos,CollisionEvent * _Output,GameObject ** _IgnoreList, unsigned _count, float _MaxLength) const
	{
		bool isColliding = false;
		_Output->mTimeIntersection = 999999.f;
		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			CollisionEvent Col;
			Col.mTimeIntersection = 99999.f;
			if (elem.GetOwner())
			{
				if (RayCollider::Raycast(_Dir, _mPos, &elem, &Col, _MaxLength))
				{
					bool ignore = false;
					if (_IgnoreList)
					{
						for (unsigned i = 0; i < _count; ++i)
						{
							if (_IgnoreList[i] == Col.mCollidedWith)
							{
								ignore = true;
							}
						}
					}
					if (ignore) continue;
					isColliding |= true;
					if(_Output->mTimeIntersection > Col.mTimeIntersection ? Col.mTimeIntersection : _Output->mTimeIntersection)
					*_Output = Col;
				}
			}
		}

		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				//isColliding = RayCollider::Raycast(_Dir, _mPos, &elem, _Output, _MaxLength);
			}

		}

		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			CollisionEvent Col;
			Col.mTimeIntersection = 99999.f;
			if (elem.GetOwner())
			{
				if (RayCollider::Raycast(_Dir, _mPos, &elem, &Col, _MaxLength))
				{
					bool ignore = false;
					if (_IgnoreList)
					{
						for (unsigned i = 0; i < _count; ++i)
						{
							if (_IgnoreList[i] == Col.mCollidedWith)
							{
								ignore = true;
							}
						}
					}
					if (ignore) continue;
					isColliding |= true;
					if (_Output->mTimeIntersection > Col.mTimeIntersection ? Col.mTimeIntersection : _Output->mTimeIntersection)
						*_Output = Col;
				}
			}
		}

		return isColliding;
	}

	bool CollisionSystem::RaycastAllHits(Math::Vec3D const & _Dir, Math::Point3D const & _mPos, AutoArray<CollisionEvent>& _Output, float _MaxLength) const
	{
		bool isColliding = false;
		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				CollisionEvent ColEvent{ nullptr, elem.GetOwner() };
				bool result = RayCollider::Raycast(_Dir, _mPos, &elem, &ColEvent, _MaxLength);
				if (result)
					_Output.push_back(Ut::Move(ColEvent));
				isColliding |= result;
			}
		}

		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				//isColliding = RayCollider::Raycast(_Dir, _mPos, &elem, _Output, _MaxLength);
			}

		}

		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				CollisionEvent ColEvent{ nullptr, elem.GetOwner() };
				bool result = RayCollider::Raycast(_Dir, _mPos, &elem, &ColEvent, _MaxLength);
				if (result)
					_Output.push_back(Ut::Move(ColEvent));
				isColliding |= result;
			}
		}

		return isColliding;
	}

	CollisionSystem::CollisionSystem()
	{

	}

	CollisionSystem::~CollisionSystem()
	{

	}


}
