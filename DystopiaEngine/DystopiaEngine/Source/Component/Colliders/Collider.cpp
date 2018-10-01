/* HEADER *********************************************************************************/
/*!
\file	Collider.cpp
\author Keith (70%)
		Aaron (30%)
\par    email: keith.goh\@digipen.edu
		email: m.chu\@digipen.edu
\brief
Collider2D for 2D Sprites.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Collider.h"
#include "Component/RigidBody.h"
#include "Object/GameObject.h"
#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/MeshSystem.h"

namespace Dystopia
{
	Collider::Collider()
		: mv3Offset{0,0,0,0}, mpMesh{nullptr}, mbColliding{false}, mPosition{ Math::MakePoint3D(0.f,0.f,0.f) }
	{
		
	}
	Collider::Collider(const Math::Point3D & _offset, const Math::Point3D & _origin)
		: mv3Offset{ _offset }, mpMesh{ nullptr }, mbColliding{ false }, mPosition{_origin}
	{

	}

	void Collider::Load(void)
	{

	}

	void Collider::Init(void)
	{
		if (mpMesh != nullptr || this->mDebugVertices.size() == 0 || this->mIndexBuffer.size() == 0)
			return;

		auto * pMeshSys = EngineCore::GetInstance()->Get<MeshSystem>();
		/*Create Mesh*/
		pMeshSys->StartMesh();

		auto const & arr = GetVertexBuffer();
		for (auto i : arr)
		{
			pMeshSys->AddVertex(i.x, i.y, i.z);
		}

		SetMesh(pMeshSys->AddIndices("Collider Mesh", GetIndexBuffer()));
		pMeshSys->EndMesh();
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

	float Collider::DetermineRestitution(RigidBody const & b) const
	{
		const float a_rest = GetOwner()->GetComponent<RigidBody>()->GetRestitution();
		return Math::Min(a_rest, b.GetRestitution());
	}

	float Collider::DetermineStaticFriction(RigidBody const & b) const
	{
		const float a_fric = GetOwner()->GetComponent<RigidBody>()->GetStaticFriction();
		return sqrt(a_fric*b.GetStaticFriction());
	}

	float Collider::DetermineKineticFriction(RigidBody const & b) const
	{
		const float a_fric = GetOwner()->GetComponent<RigidBody>()->GetKineticFriction();
		return sqrt(a_fric*b.GetKineticFriction());
	}

	AutoArray<CollisionEvent> const & Collider::GetCollisionEvents() const
	{
		return marr_ContactSets;
	}

	void Collider::ClearCollisionEvent()
	{
		marr_ContactSets.clear();
	}

	bool Collider::HasCollision() const
	{
		return mbColliding;
	}

	void Collider::SetColliding(bool _b)
	{
		mbColliding = _b;
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

	void Collider::SetRotation(Math::Degrees _deg)
	{
		mTransformation = Math::RotateZ(_deg);
	}

	void Collider::SetRotation(Math::Radians _rad)
	{
		mTransformation = Math::RotateZ(_rad);
	}

	void Collider::AddRotation(Math::Radians _rad)
	{
		Math::Matrix3D Rotation = Math::RotateZ(_rad);
		mTransformation = Rotation * mTransformation;
	}

	void Collider::AddRotation(Math::Degrees _deg)
	{
		Math::Matrix3D Rotation = Math::RotateZ(_deg);
		mTransformation = Rotation * mTransformation;
	}

	Math::Matrix3D Collider::GetTransformationMatrix() const
	{
		return mTransformation;
	}

	Collider::~Collider()
	{
		mpMesh = nullptr;
	}

	void Collider::Triangulate()
	{
		if (mDebugVertices.size() < 3)
			return;
		
		mIndexBuffer.clear();

		auto const start = mDebugVertices.begin();

		const auto  first   = mDebugVertices.begin();
		auto  second  = first+1;
		auto  third   = second+1;
		do
		{
		
		  mIndexBuffer.push_back(static_cast<const short>(first  - start));
		  mIndexBuffer.push_back(static_cast<const short>(second - start));
		  mIndexBuffer.push_back(static_cast<const short>(third - start));

		  const auto copy = third++;
		  second    = copy;

		} while (third != mDebugVertices.end());
	}
}