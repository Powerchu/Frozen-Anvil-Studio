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
		for ( auto & rigid_elem : m_rigidBodies_arr)
		{
			if (rigid_elem != nullptr && rigid_elem->GetOwner())
			{
				rigid_elem->Update(_dt);
			}
		}
	}

	void PhysicsSystem::Shutdown(void)
	{

	}

	void PhysicsSystem::Insert(RigidBody * const rigid_body)
	{
		m_rigidBodies_arr.Insert(rigid_body);
	}

	void PhysicsSystem::Remove(RigidBody * const rigid_body)
	{
		m_rigidBodies_arr.Remove(rigid_body);
	}

	void PhysicsSystem::LoadDefaults(void)
	{

	}

	void PhysicsSystem::LoadSettings(TextSerialiser& serial)
	{

	}

}
