#include "Component\Collider.h"

#include "Object\GameObject.h"
#include "Behaviour\Behaviour.h"

namespace Dystopia
{
	Collider::Collider()
		:mv3Offset{0,0,0,0}
	{
		CollisionSystem::GetInstance()->InsertCollider(this);
	}
	Collider::Collider(const Math::Point3D & _offset)
		: mv3Offset{ _offset }
	{
		CollisionSystem::GetInstance()->InsertCollider(this);
	}
	void Collider::Load(void)
	{
	}
	void Collider::Init(void)
	{

	}
	void Collider::OnDestroy(void)
	{
	}
	void Collider::Unload(void)
	{
	}
	Collider * Collider::Duplicate() const
	{
		return nullptr;
	}
	Math::Vec3D Collider::GetOffSet() const
	{
		return this->mv3Offset;
	}
	void Collider::Serialise(TextSerialiser&)
	{
	}
	void Collider::Unserialise(TextSerialiser&)
	{
	}
	Collider::~Collider()
	{

	}
}