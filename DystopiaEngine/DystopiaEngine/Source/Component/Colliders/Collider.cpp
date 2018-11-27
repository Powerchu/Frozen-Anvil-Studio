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
#include "Behaviour/Behaviour.h"
#include "Object/GameObject.h"
#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/MeshSystem.h"


namespace Dystopia
{
	Collider::Collider()
		: mv3Offset{0, 0, 0, 0}, mpMesh{nullptr}, mbColliding{false}, mPosition{Math::MakePoint3D(0.f, 0.f, 0.f)},
		  mbIsTrigger(false), mbIsSleeping(false), mScale{1, 1, 1}, mBoundingCircle{GenerateBoardPhaseCircle()}
	{
	}

	Collider::Collider(const Math::Point3D & _offset, const Math::Point3D & _origin)
		: mv3Offset{_offset}, mpMesh{nullptr}, mbColliding{false}, mPosition{_origin}, mbIsTrigger(false),
		  mbIsSleeping(false), mScale{1, 1, 1}, mBoundingCircle{GenerateBoardPhaseCircle()}
	{
	}

	void Collider::Awake(void)
	{
		Triangulate();

		if (mpMesh != nullptr || this->mDebugVertices.size() == 0 || this->mIndexBuffer.size() == 0)
			return;

		auto * pMeshSys = EngineCore::GetInstance()->Get<MeshSystem>();
		if (pMeshSys)
		{
			/*Create Mesh*/
			pMeshSys->StartMesh();

			auto const & arr = GetVertexBuffer();
			for (const auto& i : arr)
			{
				pMeshSys->AddVertex(i.x, i.y, i.z);
			}

			SetMesh(pMeshSys->AddIndices("Collider Mesh", GetIndexBuffer()));
			pMeshSys->EndMesh();
		}
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

	float Collider::DetermineRestitution(RigidBody const & b) const
	{
		if (nullptr != this->GetOwner()->GetComponent<RigidBody>())
		{
			const float a_rest = GetOwner()->GetComponent<RigidBody>()->GetRestitution();
			return Math::Min(a_rest, b.GetRestitution());
		}
		return 0;
	}

	float Collider::DetermineStaticFriction(RigidBody const & b) const
	{
		if (nullptr != this->GetOwner()->GetComponent<RigidBody>())
		{
			const float a_fric = GetOwner()->GetComponent<RigidBody>()->GetStaticFriction();
			return sqrt(a_fric*b.GetStaticFriction());
		}
		return 0;
	}

	float Collider::DetermineKineticFriction(RigidBody const & b) const
	{
		if (nullptr != &b && nullptr != this->GetOwner()->GetComponent<RigidBody>())
		{
			const float a_fric = GetOwner()->GetComponent<RigidBody>()->GetKineticFriction();
			return sqrt(a_fric*b.GetKineticFriction());
		}
		return 0;
	}

	AutoArray<CollisionEvent> const & Collider::GetCollisionEvents() const
	{
		return marr_ContactSets;
	}

	CollisionEvent * Collider::FindCollisionEvent(unsigned long long _ID) const
	{
		for (auto & elem : marr_ContactSets)
			if (elem == _ID)
				return &elem;

		return nullptr;
	}

	void Collider::ClearCollisionEvent()
	{
		marr_ContactSets.clear();
	}

	void Collider::ClearCurrentCollisionEvent()
	{
		marr_CurrentContactSets.clear();
	}



	void Collider::RemoveCollisionEvent(unsigned long long _OtherID)
	{
		auto start = marr_ContactSets.begin();
		const auto end   = marr_ContactSets.end();
		while (start != end)
		{
			if (*start == _OtherID)
			{
				marr_ContactSets.Remove(start);
				return;
			}
			++start;
		}
	}
	void Collider::InformOtherComponents()
	{
		const auto _owner = GetOwner();
		const auto _body = _owner->GetComponent<RigidBody>();

		for (auto & elem : marr_CurrentContactSets)
		{
			if (auto ptr = FindCollisionEvent(elem.mOtherID))
			{
				if (!mbIsTrigger)
					_owner->OnCollisionStay(elem);
				else
					_owner->OnTriggerStay(elem.mCollidedWith);
				RemoveCollisionEvent(elem.mOtherID);
			}
			else
			{
				if (!mbIsTrigger)
					_owner->OnCollisionEnter(elem);
				else
					_owner->OnTriggerEnter(elem.mCollidedWith);
			}
		}

		for (auto & elem : marr_ContactSets)
		{
			if (!mbIsTrigger)
				_owner->OnCollisionExit(elem);
			else
				_owner->OnTriggerExit(elem.mCollidedWith);
		}

		/*I am not sure why i need to clear it before assigning. else will have stuff inside*/
		marr_ContactSets.clear();
		marr_ContactSets = marr_CurrentContactSets;
		//marr_CurrentContactSets.clear();
	}
	void Collider::InformOtherComponents(const bool _isColliding, CollisionEvent const & _Event)
	{
		const auto _owner = GetOwner();
		const auto _body = _owner->GetComponent<RigidBody>();

		if (_isColliding)
		{
			if (auto * ptr = FindCollisionEvent(_Event.mOtherID))
			{
				if (!mbIsTrigger)
					_owner->OnCollisionStay(_Event);
				else
					_owner->OnTriggerStay(_Event.mCollidedWith);
				
				*ptr = _Event;
			}
			else
			{
				if (!mbIsTrigger)
					_owner->OnCollisionEnter(_Event);
				else
					_owner->OnTriggerEnter(_Event.mCollidedWith);
				
				marr_ContactSets.push_back(_Event);
			}
		}
		else
		{
			if (FindCollisionEvent(_Event.mOtherID))
			{
				if (nullptr != _body)
					_body->SetSleeping(false);

				if (!mbIsTrigger)
					_owner->OnCollisionExit(_Event);
				else
					_owner->OnTriggerExit(_Event.mCollidedWith);
				
				RemoveCollisionEvent(_Event.mOtherID);
				
			}
		}
	}

	bool Collider::HasCollision() const
	{
		return mbColliding;
	}

	bool Collider::HasCollisionWith(unsigned long long _ID) const
	{
		for (auto const & elem : marr_ContactSets)
			if (elem == _ID)
				return true;
		return false;
	}

	bool Collider::HasCollisionWith(GameObject const * const _pointer) const
	{
		for (auto const & elem : marr_ContactSets)
			if (elem == _pointer)
				return true;
		return false;
	}

	BroadPhaseCircle Collider::GetBroadPhaseCircle() const
	{
		return mBoundingCircle;
	}

	bool Collider::IsTrigger() const
	{
		return mbIsTrigger;
	}

	bool Collider::IsSleeping() const
	{
		const auto body = GetOwner()->GetComponent<RigidBody>();
		if (body)
		{
			if (body->Get_IsStaticState()) return false;
		}
		return mbIsSleeping;
	}

	void Collider::SetColliding(bool _b)
	{
		mbColliding = _b;
	}

	void Collider::SetSleeping(bool _b)
	{
		mbIsSleeping = _b;
	}

	void Collider::SetTrigger(bool _b)
	{
		mbIsTrigger = _b;
	}

	void Collider::SetLocalPosition(Math::Point3D const & _point)
	{
		mPosition = _point;
		mBoundingCircle = GenerateBoardPhaseCircle();
	}

	Math::Point3D Collider::GetGlobalPosition() const
	{
		auto point =  mOwnerTransformation * Math::Translate(mv3Offset.x, mv3Offset.y , mv3Offset.z) * GetTransformationMatrix() *  mPosition;
		point.z = 0;
		return point;
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

	AutoArray<GameObject*> Collider::GetCollidingObjects() const
	{
		AutoArray<GameObject*> ret;

		for (const auto& elem : marr_ContactSets)
		{
			ret.push_back(const_cast<GameObject*>(elem.mCollidedWith));
		}

		return Ut::Move(ret);
	}

	void Collider::SetMesh(Mesh * _ptr)
	{
		mpMesh = _ptr;
	}

	Mesh * Collider::GetMesh() const
	{
		return mpMesh;
	}

	Math::Matrix3D Collider::GetTransformationMatrix() const
	{
		auto rot = Math::Normalise(mRotation);
		return rot.Matrix() * Math::Scale(mScale.x,mScale.y,mScale.z);
	}

	Math::Matrix3D Collider::GetOwnerTransform() const
	{
		return mOwnerTransformation;
	}

	Math::Matrix3D Collider::GetWorldMatrix() const
	{
		return mOwnerTransformation * Math::Translate(mv3Offset.x, mv3Offset.y, mv3Offset.z) * GetTransformationMatrix();
	}

	void Collider::SetOwnerTransform(Math::Matrix3D const& _ownerMatrix)
	{
		mOwnerTransformation = _ownerMatrix;
		mBoundingCircle = GenerateBoardPhaseCircle();
	}


	BroadPhaseCircle Collider::GenerateBoardPhaseCircle() const
	{
		return BroadPhaseCircle();
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