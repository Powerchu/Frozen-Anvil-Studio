#include "System/Collision/CollisionSystem.h"
#include "System/Graphics/MeshSystem.h"

#include "Component/Collider.h"
#include "Component/RigidBody.h"
#include "Object/GameObject.h"
#include "Component/Transform.h"

#include <map>
#include <utility>

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
			};
			return i;
		}();

		AutoArray<Collider *> mColliders;

		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
			elem.ClearCollisionEvent(); //clear collision table
			break;
		}

		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
			elem.SetPosition(elem.GetOwner()->GetComponent<Transform>()->GetGlobalPosition());
			elem.SetColliding((false));
			mColliders.push_back(&elem);
		}
		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
			elem.SetPosition(elem.GetOwner()->GetComponent<Transform>()->GetGlobalPosition());
			elem.SetColliding((false));
			mColliders.push_back(&elem);
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

	void CollisionSystem::InsertCollider(Collider * const &)
	{
		//this->mArrOfCollider.push_back(_Col);
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

	CollisionSystem::CollisionSystem()
	{

	}

	CollisionSystem::~CollisionSystem()
	{

	}


}
