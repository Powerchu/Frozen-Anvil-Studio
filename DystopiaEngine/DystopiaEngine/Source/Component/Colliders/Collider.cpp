#include "Component\Collider.h"

#include "Object\GameObject.h"

namespace Dystopia
{
	Collider::Collider()
		: mv3Offset{0,0,0,0}
	{
		//EngineCore::GetInstance()->GetSystem<CollisionSystem>()->InsertCollider(this);
	}
	Collider::Collider(const Math::Point3D & _offset)
		: mv3Offset{ _offset }
	{
		//EngineCore::GetInstance()->GetSystem<CollisionSystem>()->InsertCollider(this);
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
	AutoArray<CollisionEvent> const & Collider::GetCollisionEvents() const
	{
		// TODO: insert return statement here
		return mCollisionEvent;
	}
	bool Collider::hasCollision() const
	{
		return Colliding;
	}
	void Collider::SetColliding(bool _b)
	{
		Colliding = _b;
	}
	void Collider::SetPosition(Math::Point3D const & _point)
	{
		mPosition = _point;
	}
	Math::Point3D Collider::GetPosition() const
	{
		return mPosition;
	}
	Math::Vec3D Collider::GetOffSet() const
	{
		return this->mv3Offset;
	}
	void Collider::Serialise(TextSerialiser&) const
	{

	}
	void Collider::Unserialise(TextSerialiser&)
	{

	}
	Collider::~Collider()
	{

	}
}