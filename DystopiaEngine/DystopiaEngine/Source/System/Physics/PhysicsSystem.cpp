#include "System/Physics/PhysicsSystem.h"
#include "Component/RigidBody.h"
#include "Object/GameObject.h"
#include <System/Collision/CollisionSystem.h>

namespace Dystopia
{
	PhysicsSystem::PhysicsSystem()
	{
	}

	void PhysicsSystem::PreInit(void)
	{
	}

	bool PhysicsSystem::Init(void)
	{
		return false;
	}

	void PhysicsSystem::PostInit(void)
	{

	}

	void PhysicsSystem::FixedUpdate(float)
	{

	}

	void PhysicsSystem::Update(float _dt)
	{
		for ( auto rigid_elem : mComponents)
		{
			if (rigid_elem.GetOwner()->IsActive())
			{
				rigid_elem.Update(_dt);
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

	}

	void PhysicsSystem::ResolveCollision(float)
	{

	}

}
