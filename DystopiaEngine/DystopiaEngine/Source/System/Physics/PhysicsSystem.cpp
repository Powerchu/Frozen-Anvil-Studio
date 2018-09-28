#include "Component/Collider.h"
#include "Component/RigidBody.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

namespace Dystopia
{
	PhysicsSystem::PhysicsSystem()
		: mbIsDebugActive(false)
		, mInterpolation_mode(none)
		, mGravity(-910.665F)
		, mMaxVelocityConstant(1024.0F)
		, mMaxVelSquared(mMaxVelocityConstant*mMaxVelocityConstant)
		, mPenetrationEpsilon(0.1F)
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
					if (col->HasCollision())
					{
						CollisionEvent* worstContact = nullptr;
						double worstPene = mPenetrationEpsilon;

						for (auto& manifold : col->GetCollisionEvents())
						{
							manifold.ApplyImpulse();
							if (manifold.mdPeneDepth > worstPene)
							{
								worstContact = &manifold;
								worstPene = manifold.mdPeneDepth;
							}
							if (nullptr != worstContact)
								worstContact->ApplyPenetrationCorrection();
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

	void PhysicsSystem::Step(float _dt)
	{
		/* Broad Phase Collision Detection*/

		/* Narrow Phase Collision Detection*/
		
		/* Collision Resolution (Response) Logic */
		ResolveCollision(_dt);

		// Integrate RigidBodies
		IntegrateRigidBodies(_dt);

		/*Update positions and rotation as result*/
		UpdateResults();

		// Set all objects at rest to sleeping
		CheckSleepingBodies(_dt);

		/* Debug Velocity*/
		//DebugPrint();
	}

	

	void PhysicsSystem::FixedUpdate(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Physics System", "Update" };

		Step(_dt);

		if (mbIsDebugActive)
		{
			DebugPrint();
		}
	}

	void PhysicsSystem::Update(float)
	{
		
	}

	void PhysicsSystem::PostUpdate()
	{
		for (auto& body : mComponents)
		{
			if (body.GetFlags() & FLAG_REMOVE)
			{
				mComponents.Remove(&body);
			}
		}
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
