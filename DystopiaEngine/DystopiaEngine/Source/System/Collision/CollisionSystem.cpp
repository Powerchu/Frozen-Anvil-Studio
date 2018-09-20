#include "System\Collision\CollisionSystem.h"
#include "Component\Collider.h"

#include <map>
#include <utility>

namespace Dystopia
{
	void CollisionSystem::PreInit(void)
	{

	}
	bool CollisionSystem::Init()
	{
		return true;
	}

	void CollisionSystem::PostInit(void)
	{
	}

	void CollisionSystem::Update(float _dt)
	{
		_dt;
		using CollisionTable = std::pair<eColliderType, eColliderType>;
		using fpCollisionResolution = bool(CollisionSystem::*)(Collider  * const &, Collider  * const &)const;
		using CollisionTableMap = std::map < CollisionTable, fpCollisionResolution>;

		bool isColliding;
		
		static CollisionTableMap CollisionFuncTable = []()->CollisionTableMap
		{
			CollisionTableMap i
			{
				{CollisionTable{eColliderType::AABB    ,eColliderType::AABB}     ,&CollisionSystem::AABBvsAABB},
				{CollisionTable{eColliderType::AABB    ,eColliderType::CONVEX }  ,&CollisionSystem::ConvexVsConvex },
				{CollisionTable{eColliderType::CONVEX  ,eColliderType::AABB }    ,&CollisionSystem::ConvexVsConvex },
				{CollisionTable{eColliderType::CONVEX  ,eColliderType::CONVEX }  ,&CollisionSystem::ConvexVsConvex },
			};
			return i;
		}();

		/*
		for (Collider * const & elem : mArrOfCollider)
		{
			for (Collider * const * i = &elem+1; i <= &mArrOfCollider.back(); i++)
			{
				elem->GetColliderType();

				const auto pair_key = std::make_pair(elem->GetColliderType(), (*i)->GetColliderType());
				for (auto & key : CollisionFuncTable)
				{
					if (key.first == pair_key)
					{
						isColliding = (this->*key.second)(elem, *i);
						break;
					}
				}
			}
		}
		*/
	}

	void CollisionSystem::FixedUpdate(float _dt)
	{
		_dt;
	}

	void CollisionSystem::Shutdown()
	{
	}

	void CollisionSystem::InsertCollider(Collider * const & _Col)
	{
		//this->mArrOfCollider.push_back(_Col);
	}

	bool CollisionSystem::AABBvsAABB(Collider * const & _ColA, Collider * const & _ColB) const
	{
		const auto col_a = dynamic_cast<const AABB * const>(_ColA);
		const auto col_b = dynamic_cast<const AABB * const>(_ColB);

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
