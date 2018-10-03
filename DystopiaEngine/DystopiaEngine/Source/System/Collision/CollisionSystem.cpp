#include "Component/ColliderList.h"
#include "Component/RigidBody.h"
#include "Component/Transform.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

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
		auto * pMeshSys = EngineCore::GetInstance()->Get<MeshSystem>();
		if (pMeshSys)
		{
			for (auto & elem : ComponentDonor<Convex>::mComponents)
			{
				/*If Collider already has a mesh, do not make a new one*/
				if (elem.GetMesh() == nullptr)
					continue;

				pMeshSys->StartMesh();

				auto const & arr = elem.GetVertexBuffer();
				for (auto i : arr)
				{
					pMeshSys->AddVertex(i.x, i.y, i.z);
				}

				elem.SetMesh(pMeshSys->AddIndices("Collider Mesh", elem.GetIndexBuffer()));
				pMeshSys->EndMesh();
			}
		}
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
	}

	void CollisionSystem::FixedUpdate(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Collision System", "Update" };

		for (auto& conv : ComponentDonor<Convex>::mComponents)
		{
			conv.Update(_dt);
		}

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
			{ CollisionTable{ eColliderType::CIRCLE  ,eColliderType::CIRCLE }   ,&CollisionSystem::CircleVsCircle },
			{ CollisionTable{ eColliderType::CIRCLE, eColliderType::AABB }      ,&CollisionSystem::CircleVsAABB },
			{ CollisionTable{ eColliderType::AABB,   eColliderType::CIRCLE }    ,&CollisionSystem::AABBvsCircle },
			{ CollisionTable{ eColliderType::CIRCLE, eColliderType::CONVEX }    ,&CollisionSystem::CircleVsConvex },
			{ CollisionTable{ eColliderType::CONVEX, eColliderType::CIRCLE }   ,&CollisionSystem::ConvexVsCircle }
			};
			return i;
		}();

		AutoArray<Collider *> mColliders;

		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
			if (elem.GetOwner())
			{
				elem.ClearCollisionEvent(); //clear collision table
				//auto const & GobjPoint = elem.GetOwner()->GetComponent<Transform>()->GetPosition();
			    //Math::Matrix3D gobjMatrix = Math::Translate(GobjPoint.x, GobjPoint.y, GobjPoint.z) * elem.GetOwner()->GetComponent<Transform>()->GetRotation().Matrix();
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetLocalTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding((false));
				mColliders.push_back(&elem);
			}
		}

		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
			if (elem.GetOwner())
			{
				elem.ClearCollisionEvent(); //clear collision table
				//auto const & GobjPoint =  elem.GetOwner()->GetComponent<Transform>()->GetPosition();
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetLocalTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding((false));
				mColliders.push_back(&elem);
			}
		}

		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
			if (elem.GetOwner())
			{
				elem.ClearCollisionEvent(); //clear collision table
				//auto const   & GobjPoint = elem.GetOwner()->GetComponent<Transform>()->GetPosition();
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetLocalTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding((false));
				mColliders.push_back(&elem);
			}
		}

		for (auto & bodyA : mColliders)
		{
			if (nullptr == bodyA->GetOwner()) continue;
			for (auto & bodyB : mColliders)
			{
				if (nullptr == bodyB->GetOwner()) continue;

				if (static_cast<Collider *>(bodyA) != static_cast<Collider *>(bodyB))
				{
					if (bodyA->GetOwner()->GetComponent<RigidBody>() && bodyB->GetOwner()->GetComponent<RigidBody>())
					{
						if (!bodyA->GetOwner()->GetComponent<RigidBody>()->Get_IsStaticState() ||
							!bodyB->GetOwner()->GetComponent<RigidBody>()->Get_IsStaticState())
						{
							const auto pair_key = std::make_pair(bodyA->GetColliderType(), (bodyB)->GetColliderType());
							for (auto & key : CollisionFuncTable)
							{
								if (key.first == pair_key)
								{
									(this->*key.second)(bodyA, bodyB);
									bodyB->SetColliding(bodyB->Collider::HasCollision());
									bodyA->SetColliding(bodyA->Collider::HasCollision());
									break;
								}
							}
						}
					}
					else
					{
						const auto pair_key = std::make_pair(bodyA->GetColliderType(), (bodyB)->GetColliderType());
						for (auto & key : CollisionFuncTable)
						{
							if (key.first == pair_key)
							{
								(this->*key.second)(bodyA, bodyB);
								bodyB->SetColliding(bodyB->Collider::HasCollision());
								bodyA->SetColliding(bodyA->Collider::HasCollision());
								break;
							}
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
		UNUSED_PARAMETER(_ColA);
		UNUSED_PARAMETER(_ColB);

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
		bool isColliding = pCircle->isColliding((*pConvex));

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
		bool isColliding = pConvex->isColliding((*pCircle));

		return isColliding;
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
		return Utility::Move(ToRet);
	}

	CollisionSystem::CollisionSystem()
	{

	}

	CollisionSystem::~CollisionSystem()
	{

	}


}
