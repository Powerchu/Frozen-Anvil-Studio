#include "Component\Collider.h"

#include "Object\GameObject.h"
#include "System/Graphics/VertexDefs.h"
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
	AutoArray<Vertex> Collider::GetVertexBuffer() const
	{
		return mDebugVertices;
	}
	AutoArray<short> Collider::GetIndexBuffer() const
	{
		return mIndexBuffer;
	}
	void Collider::SetMesh(Mesh * _ptr)
	{
		mpMesh = _ptr;
	}
	Mesh * Collider::GetMesh() const
	{
		return mpMesh;
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
	void Collider::Triangulate()
	{
		if (mDebugVertices.size() < 3)
			return;
		
		
		auto const start = mDebugVertices.begin();

		auto  first   = mDebugVertices.begin();
		auto  second  = first+1;
		auto  third   = second+1;
		do
		{
		  mIndexBuffer.push_back(first  - start);
		  mIndexBuffer.push_back(second - start);
		  mIndexBuffer.push_back(third - start);

		  auto copy = third++;
		  second    = copy;

		} while (third != mDebugVertices.end());


	}
}