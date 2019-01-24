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
#include "System/Graphics/MeshSystem.h"
#include "Editor/EGUI.h"


namespace Dystopia
{
	std::string Collider::arrColLayer[33] = 
	{
				"Layer 1","Layer 2", "Layer 3", "Layer 4",
				"Layer 5","Layer 6", "Layer 7", "Layer 8",
				"Layer 9","Layer 10", "Layer 11", "Layer 12",
				"Layer 13","Layer 14", "Layer 15", "Layer 16",
				"Layer 17","Layer 18", "Layer 19", "Layer 20",
				"Layer 21","Layer 22", "Layer 23", "Layer 24",
				"Layer 25","Layer 26", "Layer 27", "Layer 28",
				"Layer 29","Layer 30", "Layer 31", "Layer 32","Layer_Global"
	};


	Collider::Collider()
		: mv3Offset{0, 0, 0, 0}, mpMesh{nullptr}, mbColliding{false}, mPosition{Math::MakePoint3D(0.f, 0.f, 0.f)},
		  mbIsTrigger(false), mbIsSleeping(false), mScale{1, 1, 1}, mBoundingCircle{GenerateBoardPhaseCircle()}, mColLayer{ eColLayer::LAYER_NONE }
	{
	}

	Collider::Collider(const Math::Point3D & _offset, const Math::Point3D & _origin)
		: mv3Offset{_offset}, mpMesh{nullptr}, mbColliding{false}, mPosition{_origin}, mbIsTrigger(false),
		  mbIsSleeping(false), mScale{1, 1, 1}, mBoundingCircle{GenerateBoardPhaseCircle()}, mColLayer{ eColLayer::LAYER_NONE }
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

			/*Add centroid vertex and {0,0,1} normal*/
			pMeshSys->AddVertex(0, 0, 0);
			pMeshSys->AddNormal(0, 0, 1);

			for (const auto& i : arr)
			{
				pMeshSys->AddVertex(i.x, i.y, i.z);
				pMeshSys->AddNormal(i.x, i.y, i.z);
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

	void Collider::ResetColLayer()
	{
		mColLayer = eColLayer::LAYER_NONE;
	}

	void Collider::RemoveColLayer(eColLayer _Layer)
	{
		mColLayer = static_cast<eColLayer>(mColLayer & (_Layer ^ 0xFFFFFFFFu));
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
		//const auto _body = _owner->GetComponent<RigidBody>();

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

	eColLayer Collider::GetColLayer() const
	{
		return mColLayer;
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

	void Collider::SetRotation(Math::Quaternion const& _rot)
	{
		mRotation = _rot;
	}

	void Collider::SetColLayer(eColLayer _Layer)
	{
		mColLayer  = static_cast<eColLayer>(mColLayer | _Layer);
	}

	Math::Point3D Collider::GetGlobalPosition() const
	{
		auto point =  mOwnerTransformation * Math::Translate(mv3Offset.x, mv3Offset.y , mv3Offset.z) * GetTransformationMatrix() *  mPosition;
		point.z = 0;
		return point;
	}

	Math::Quaternion Collider::GetGlobalRotation() const
	{
		return mRotation;
	}

	Math::Vec3D Collider::GetOffSet() const
	{
		return this->mv3Offset;
	}

	AutoArray<Gfx::Vertex> Collider::GetVertexBuffer() const
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

		const auto  first   = mDebugVertices.begin() - 1;
		auto		second  = first + 1;
		auto		third   = second + 1;
		do
		{
		  mIndexBuffer.push_back(static_cast<const short>(0));
		  mIndexBuffer.push_back(static_cast<const short>(second - first));
		  mIndexBuffer.push_back(static_cast<const short>(third - first));

		  second = third++;

		} while (third != mDebugVertices.end());

		mIndexBuffer.push_back(static_cast<const short>(0));
		mIndexBuffer.push_back(static_cast<const short>(second - first));
		mIndexBuffer.push_back(static_cast<const short>(1));



	}
	void Collider::EditorUI(void) noexcept
	{
		const int totalLayers = 32;


		if (EGUI::Display::CollapsingHeader("Collision Layer", false))
		{
			if (EGUI::Display::Button("Add Layer", {128,24}))
			{
				EGUI::Display::OpenPopup("List of layers", false);
			}
			AddLayerUI();
			EGUI::SameLine();
			if (EGUI::Display::Button("Remove All", { 128,24 }))
			{
				ResetColLayer();
			};
			if(mColLayer)
			{
				if(mColLayer == eColLayer::LAYER_GLOBAL)
				{
					if (EGUI::Display::IconCross("Layer_Global"))
					{
						RemoveColLayer(static_cast<eColLayer>((0xFFFFFFFF)));
					}
					EGUI::SameLine();
					EGUI::Display::Label("Layer_Global");
				}
				else
				{
					for (unsigned i = 0; i < sizeof(eColLayer) * 8; ++i)
					{
						if (mColLayer & (0x01u << i))
						{
							if (EGUI::Display::IconCross(arrColLayer[i].c_str()))
							{
								RemoveColLayer(static_cast<eColLayer>((0x01u << i)));
							}
							EGUI::SameLine();
							EGUI::Display::Label(arrColLayer[i].c_str());
						}
					}
				}
			}
			//for (auto i = 0; i<totalLayers;++i)
			//{
			//	EGUI::PushID(i);
			//	EGUI::PushLeftAlign(50);
			//	if (EGUI::Display::IconCross("Remove"))
			//	{
			//		// Remove from flag
			//	};
			//	EGUI::SameLine();
			//	EGUI::Display::DropDownSelection(std::to_string(i).c_str(), indexDropDown, arr, 200);
			//	EGUI::PopLeftAlign();
			//	EGUI::PopID();
			//}
		}
	}

	void Collider::AddLayerUI(void) noexcept
	{
		if (EGUI::Display::StartPopup("List of layers"))
		{
			for (auto i = 0; i < sizeof(arrColLayer)/sizeof(arrColLayer[0]); ++i)
			{
				EGUI::PushID(i);
				EGUI::PushLeftAlign(50);

				if (EGUI::Display::SelectableTxt(arrColLayer[i].c_str(), false))
				{
					if(i == sizeof(arrColLayer) / sizeof(arrColLayer[0])-1)
					{
						SetColLayer(eColLayer::LAYER_GLOBAL);
					}
					else
						SetColLayer(static_cast<eColLayer>((0x01u << i)));
				}
				EGUI::PopLeftAlign();
				EGUI::PopID();
			}
			EGUI::Display::EndPopup();
		}
	}

}
