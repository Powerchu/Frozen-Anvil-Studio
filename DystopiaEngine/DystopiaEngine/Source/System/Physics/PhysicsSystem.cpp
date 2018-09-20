#include "System/Physics/PhysicsSystem.h"
#include "Component/RigidBody.h"
//#include "Object/GameObject.h"
#include <System/Collision/CollisionSystem.h>

namespace Dystopia
{
	PhysicsSystem::PhysicsSystem()
		: mbIsDebugActive(false)
		, mTimeAccumulator(0.0F)
		, mGravity(-9.81F)
		, mMaxVelocityConstant(1000)
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
		for (auto rigid_elem : mComponents)
		{
			rigid_elem.Integrate(_dt);
		}
	}

	void PhysicsSystem::ResolveCollision(float _dt)
	{
		UNUSED_PARAMETER(_dt);
	}

	void PhysicsSystem::PostResults()
	{
		for (auto rigid_elem : mComponents)
		{
			rigid_elem.PostResult();
		}

		// If Event System is running: this is where to Broadcast Collision Messages
	}

	void PhysicsSystem::DebugPrint()
	{
		for (auto rigid_elem : mComponents)
		{
			rigid_elem.DebugPrint();
		}
	}

	void PhysicsSystem::DebugDraw()
	{
	}

	void PhysicsSystem::Step(float _dt)
	{
		/*
		 * Physics Logic
		 */
		IntegrateRigidBodies(_dt);

		/*
		 * Collision System Detection
		 */
		// Contacts.Reset();
		// DetectContacts(dt);

		/*
		 * Resolution
		 */
		ResolveCollision(_dt);

		/*
		 * Updating the position
		 */
		PostResults();

		/*
		 * Debug the positions
		 */
		DebugPrint();
	}

	void PhysicsSystem::FixedUpdate(float)
	{

	}

	void PhysicsSystem::Update(float _dt)
	{
		const float TimeStep = 1.0f / 60.0f;


		mTimeAccumulator += _dt;
		mTimeAccumulator = Math::Min(mTimeAccumulator, TimeStep * 5);
		
		if (mTimeAccumulator > TimeStep)
		{
			mTimeAccumulator -= TimeStep;
			Step(TimeStep);
		}

		if (mbIsDebugActive)
		{
			DebugDraw();
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
