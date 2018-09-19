#include "Component\Collider.h"

#include "Object\GameObject.h"

namespace Dystopia
{
	Collider::Collider()
		: mv3Offset{0,0,0,0}
		, m_IsBouncy(false)
	{
		//EngineCore::GetInstance()->GetSystem<CollisionSystem>()->InsertCollider(this);
	}
	Collider::Collider(const Math::Point3D & _offset)
		: mv3Offset{ _offset }
		, m_IsBouncy(false)
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
	Math::Vec3D Collider::GetOffSet() const
	{
		return this->mv3Offset;
	}
	bool Collider::Get_IsBouncy() const
	{
		return m_IsBouncy;
	}
	bool Collider::Set_IsBouncy(const bool state)
	{
		return (m_IsBouncy=state);
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