#include "System/Physics/PhysicsSystem.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"

#include "Object/GameObject.h"
#include "Component/RigidBody.h"
#include "Component/Collider.h"

#define G_CONSTANT 9.80665F

namespace Dystopia
{
	PhysicsSystem::PhysicsSystem()
		: mbIsDebugActive(true)
		, mTimeAccumulator(0.0F)
		, mInterpolation_mode(none)
		, mGravity(-G_CONSTANT)
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
			bodies.Integrate(_dt);
		}
	}

	void PhysicsSystem::ResolveCollision(float _dt)
	{
		UNUSED_PARAMETER(_dt);
		for (auto& rigid_elem : mComponents)
		{
			auto ptr = rigid_elem.GetOwner()->GetComponent<Collider>();
			if (ptr)
			{
				if (ptr->hasCollision() && !rigid_elem.Get_IsStaticState())
				{
					LoggerSystem::ConsoleLog(eLog::MESSAGE, "Collided!");
					rigid_elem.SetVelocity({0,0,0});
					ptr->GetCollisionEvents();
				}
			}
			
		}
	}

	void PhysicsSystem::UpdateResults(float _dt)
	{
		const float alpha = mTimeAccumulator / _dt;

		for (auto& body : mComponents)
		{
			switch (mInterpolation_mode)
			{
			case none:
				body.UpdateResult(1.0f);
				break;
			case interpolate:
				body.UpdateResult(alpha);
				break;
			case extrapolate: // define extrapolate
				break;
			default:
				break;
			}
		}

		// If Event System is running: this is where to Broadcast Collision Messages
	}

	void PhysicsSystem::DebugPrint()
	{
		for (auto& rigid_elem : mComponents)
		{
			rigid_elem.DebugPrint();
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
		UpdateResults(_dt);
	}

	void PhysicsSystem::FixedUpdate(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Physics System", "Update" };

		const float timeStep = 1.0f / 60.0f;

		mTimeAccumulator += _dt;
		mTimeAccumulator = Math::Min(mTimeAccumulator, timeStep * 5);
		if (mTimeAccumulator > timeStep)
		{
			mTimeAccumulator -= timeStep;
			Step(_dt);
		}

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
