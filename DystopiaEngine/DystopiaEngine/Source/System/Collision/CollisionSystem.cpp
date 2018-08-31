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
		using fpCollisionResolution = bool(CollisionSystem::*)(Collider const * const &, Collider const * const &)const;
		using CollisionTableMap = std::map < CollisionTable, fpCollisionResolution>;
		static CollisionTableMap CollisionFuncTable = []()->CollisionTableMap
		{
			CollisionTableMap i
			{
				{CollisionTable{eColliderType::AABB    ,eColliderType::AABB}     ,&CollisionSystem::AABBToAABBCollision},
				{CollisionTable{eColliderType::AABB    ,eColliderType::CONVEX }  ,&CollisionSystem::ConvexToConvexCollision },
				{CollisionTable{eColliderType::CONVEX  ,eColliderType::AABB }    ,&CollisionSystem::ConvexToConvexCollision },
				{CollisionTable{eColliderType::CONVEX  ,eColliderType::CONVEX }  ,&CollisionSystem::ConvexToConvexCollision },
			};
			return i;
		}();
		bool isColliding;

		for (Collider * const & elem : mArrOfCollider)
		{
			for (Collider * const * i = &elem+1; i <= &mArrOfCollider.back(); i++)
			{
				elem->GetColliderType();
				auto pair_key = std::make_pair(elem->GetColliderType(), (*i)->GetColliderType());
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
	}

	void CollisionSystem::FixedUpdate(float _dt)
	{
		_dt;
	}

	void CollisionSystem::Shutdown()
	{
		for (Collider * const & elem : mArrOfCollider)
		{
			delete elem;
		}
	}

	void CollisionSystem::InsertCollider(Collider * const & _Col)
	{
		this->mArrOfCollider.push_back(_Col);
	}

	bool CollisionSystem::AABBToAABBCollision(Collider const * const & _ColA, Collider const * const & _ColB) const
	{
		const AABB * const ColA = dynamic_cast<const AABB * const>(_ColA);
		const AABB * const ColB = dynamic_cast<const AABB * const>(_ColB);
		//return ColA->isColliding(ColB);
		return true;
	}

	bool CollisionSystem::ConvexToConvexCollision(Collider const * const & _ColA, Collider const * const & _ColB) const
	{
		const Convex * const ColA = dynamic_cast<const Convex * const>(_ColA);
		const Convex * const ColB = dynamic_cast<const Convex * const>(_ColB);
		return ColA->isColliding(ColB);
	}

	CollisionSystem::~CollisionSystem()
	{

	}

	CollisionSystem::CollisionSystem()
	{

	}
}
