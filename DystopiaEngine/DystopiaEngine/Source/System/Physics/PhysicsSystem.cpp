#include "System/Physics/PhysicsSystem.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"

#include "Object/GameObject.h"
#include "Component/RigidBody.h"
#include "Component/Collider.h"

namespace Dystopia
{
	PhysicsSystem::PhysicsSystem()
		: mbIsDebugActive(true)
		, mInterpolation_mode(none)
		, mGravity(-920.665F)
		, mMaxVelocityConstant(2048.0F)
		, mMaxVelSquared(mMaxVelocityConstant*mMaxVelocityConstant)
		, mPenetrationEpsilon(0.2F)
		, mPenetrationResolutionPercentage(0.8F)
		, mpColSys{nullptr}
	{
	}

	void PhysicsSystem::PreInit(void)
	{
	}

	bool PhysicsSystem::Init(void)
	{
		mpColSys = EngineCore::GetInstance()->GetSystem<CollisionSystem>();
		return true;
	}

	void PhysicsSystem::PostInit(void)
	{

	}

	void PhysicsSystem::CheckSleepingBodies(float _dt)
	{
		for (auto& bodies : mComponents)
		{
			if (bodies.GetOwner())
			{
				bodies.CheckSleeping(_dt);
			}
		}
	}

	void PhysicsSystem::IntegrateRigidBodies(float _dt)
	{
		for (auto& bodies : mComponents)
		{
			if (bodies.GetOwner())
			{
				bodies.Integrate(_dt);
			}
		}
	}

	void PhysicsSystem::ResolveCollision(float _dt)
	{
		UNUSED_PARAMETER(_dt);
		const GameObject* owner;
		for (auto& body : mComponents)
		{
			owner = body.GetOwner();
			if (nullptr != owner && !body.Get_IsStaticState())
			{
				const auto col = owner->GetComponent<Collider>();
				if (nullptr != col)
				{
					if (true == col->hasCollision())
					{
						//LoggerSystem::ConsoleLog(eLog::MESSAGE, "Collided!");
						
						for (auto& manifold : col->GetCollisionEvents())
						{
							manifold.ApplyImpulse();
							manifold.ApplyPenetrationCorrection();
						};
					}
				}
			}
		}
	}

	void PhysicsSystem::UpdateResults()
	{
		for (auto& body : mComponents)
		{
			if (body.GetOwner())
			{
				body.UpdateResult();
			}
		}

		// If Event System is running: this is where to Broadcast Collision Messages
	}

	void PhysicsSystem::DebugPrint()
	{
		for (auto& body : mComponents)
		{
			if (body.GetOwner())
			{
				body.DebugPrint();
			}
		}
	}

	void PhysicsSystem::DebugDraw()
	{
	}

	void PhysicsSystem::Step(float _dt)
	{
		// Integrate Rigidbodies
		IntegrateRigidBodies(_dt);

		/* Collision Detection*/
		mpColSys->FixedUpdate(_dt);
		
		/* Collision Resolution (Response) Logic */
		ResolveCollision(_dt);

		/*Update positions and rotation as result*/
		UpdateResults();

		// Set all objects at rest to sleeping
		CheckSleepingBodies(_dt);

		/* Debug Velocity*/
		DebugPrint();
	}

	

	void PhysicsSystem::FixedUpdate(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Physics System", "Update" };

		Step(_dt);

		if (mbIsDebugActive)
		{
			//DebugPrint();
			DebugDraw();
		}
	}

	void PhysicsSystem::Update(float)
	{
		
	}

	void PhysicsSystem::Shutdown(void)
	{

	}

	void PhysicsSystem::LoadDefaults(void)
	{

	}

	void PhysicsSystem::LoadSettings(TextSerialiser& serial)
	{
		UNUSED_PARAMETER(serial);
	}
}
