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
	{
	}

	void PhysicsSystem::PreInit(void)
	{
	}

	bool PhysicsSystem::Init(void)
	{
		return true;
	}

	void PhysicsSystem::PostInit(void)
	{

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
		for (auto& body : mComponents)
		{
			if (body.GetOwner())
			{
				auto ptr = body.GetOwner()->GetComponent<Collider>();
				if (ptr)
				{
					if (ptr->hasCollision() && !body.Get_IsStaticState())
					{
						LoggerSystem::ConsoleLog(eLog::MESSAGE, "Collided!");
						body.SetVelocity({ 0,0,0 });
						ptr->GetCollisionEvents();
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

		/* Collision Detection*/

		/* Collision Resolution (Response) Logic */
		ResolveCollision(_dt);

		// Integrate Rigidbodies
		IntegrateRigidBodies(_dt);

		/*Update positions and rotation as result*/
		UpdateResults();
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
