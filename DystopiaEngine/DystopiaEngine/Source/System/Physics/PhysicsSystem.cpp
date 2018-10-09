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
		, mGravity(-930.665F)
		, mMaxVelocityConstant(800.0F)
		, mMaxVelSquared(mMaxVelocityConstant*mMaxVelocityConstant)
		, mPenetrationEpsilon(0.1F)
		, mResolutionIterations(8)
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
#if EDITOR
			if (bodies.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (bodies.GetOwner())
			{
				if(!bodies.Get_IsStaticState())
					bodies.CheckSleeping(_dt);
			}
		}
	}

	void PhysicsSystem::IntegrateRigidBodies(float _dt)
	{
		for (auto& body : mComponents)
		{
#if EDITOR
			if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (!body.Get_IsStaticState() && body.GetIsAwake())
			{
				body.Integrate(_dt);
			}
		}
	}

	void PhysicsSystem::ResolveCollision(float)
	{
		for (unsigned i = 0; i < mResolutionIterations; ++i)
		{
			for (auto& body : mComponents)
			{
#if EDITOR
				if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
				if (!body.Get_IsStaticState() && body.GetIsAwake())
				{
					const auto col = body.GetOwner()->GetComponent<Collider>();
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
								{
									worstContact->ApplyPenetrationCorrection();
								}
							}
						}
					}
				}
			}
		}

	}

	void PhysicsSystem::UpdateResults(float _dt)
	{
		for (auto& body : mComponents)
		{
#if EDITOR
			if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (body.GetOwner())
			{
				body.UpdateResult(_dt);
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
		
	
		// Integrate RigidBodies
		IntegrateRigidBodies(_dt);

		/* Collision Resolution (Response) Logic */
		ResolveCollision(_dt);

		/*Update positions and rotation as result*/
		UpdateResults(_dt);

		// Set all objects at rest to sleeping
		//CheckSleepingBodies(_dt);

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
