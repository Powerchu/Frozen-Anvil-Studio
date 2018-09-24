#include "System/Collision/CollisionSystem.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"

#include "Component/ColliderList.h"
#include "Component/RigidBody.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

#include <utility>
#include <map>

namespace Dystopia
{
	void CollisionSystem::PreInit(void)
	{
		// empty
	}
	bool CollisionSystem::Init()
	{
		auto * pMeshSys = EngineCore::GetInstance()->Get<MeshSystem>();
		if (pMeshSys)
		{
			for (auto & elem : ComponentDonor<Convex>::mComponents)
			{
				pMeshSys->StartMesh();

				auto const & arr = elem.GetVertexBuffer();
				for (auto i : arr)
				{
					pMeshSys->AddVertex(i.x, i.y, i.z);
				}

				elem.SetMesh(pMeshSys->AddIndices("Collider Mesh", elem.GetIndexBuffer()));
				pMeshSys->EndMesh();
			}

			/*for (auto & elem : ComponentDonor<AABB>::mComponents)
			{

			}*/
		}
		return true;
	}

	void CollisionSystem::PostInit(void)
	{
		//empty
	}

	void CollisionSystem::Update(float)
	{
		// empty
	}

	void CollisionSystem::FixedUpdate(float)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Collsion System", "Update" };

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
			{ CollisionTable{ eColliderType::CIRCLE, eColliderType::CIRCLE}    ,&CollisionSystem::CircleVsCircle },
			{ CollisionTable{ eColliderType::CIRCLE, eColliderType::AABB}      ,&CollisionSystem::CircleVsAABB   },
			{ CollisionTable{ eColliderType::AABB, eColliderType::CIRCLE}      ,&CollisionSystem::AABBvsCircle   },
			{ CollisionTable{ eColliderType::CIRCLE, eColliderType::CONVEX}    ,&CollisionSystem::CircleVsConvex}
			};
			return i;
		}();

		AutoArray<Collider *> mColliders;

		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
			elem.ClearCollisionEvent(); //clear collision table
		}

		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
			if (elem.GetOwner())
			{
				elem.SetPosition(elem.GetOwner()->GetComponent<Transform>()->GetGlobalPosition());
				elem.SetColliding((false));
				mColliders.push_back(&elem);
			}
		}
		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
			if (elem.GetOwner())
			{
				elem.SetPosition(elem.GetOwner()->GetComponent<Transform>()->GetGlobalPosition());
				elem.SetColliding((false));
				mColliders.push_back(&elem);
			}
		}

		for (auto & bodyA : mColliders)
		{	
			for (auto & bodyB : mColliders)
			{
				if (static_cast<Collider *>(bodyA) != static_cast<Collider *>(bodyB) &&
					(!bodyA->GetOwner()->GetComponent<RigidBody>()->Get_IsStaticState() ||
					!bodyB->GetOwner()->GetComponent<RigidBody>()->Get_IsStaticState()))
				{
					const auto pair_key = std::make_pair(bodyA->GetColliderType(), (bodyB)->GetColliderType());
					for (auto & key : CollisionFuncTable)
					{
						if (key.first == pair_key)
						{
							(this->*key.second)(bodyA, bodyB);
							bodyB->SetColliding(bodyB->Collider::hasCollision());
							bodyA->SetColliding(bodyA->Collider::hasCollision());
							break;
						}
					}
				}				
			}
		}
	}

	void CollisionSystem::Shutdown()
	{
		/*
		for (Collider * const & elem : mArrOfCollider)
		{
		delete elem;
		}
		*/

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

		return Utility::Move(ToRet);
	}

	bool CollisionSystem::CircleVsCircle(Collider* const& _ColA, Collider* const& _Colb) const
	{
		return false;
	}

	bool CollisionSystem::CircleVsAABB(Collider * const & _ColA, Collider * const & _ColB) const
	{
		return false;
	}

	bool CollisionSystem::AABBvsCircle(Collider * const & _ColA, Collider * const & _ColB) const
	{
		return false;
	}

	bool CollisionSystem::CircleVsConvex(Collider * const & _ColA, Collider * const & _ColB) const
	{
		return false;
	}

	CollisionSystem::CollisionSystem()
	{

	}

	CollisionSystem::~CollisionSystem()
	{

	}


}
