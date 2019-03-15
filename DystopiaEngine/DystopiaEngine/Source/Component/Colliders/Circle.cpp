/* HEADER *********************************************************************************/
/*!
\file	Circle.cpp
\author Goh Kai Jie Keith (70%)
        Aaron Chu         (30%)
\par    email: keith.goh\@digipen.edu
               m.chu\@digipen.edu
\brief


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Scene/SceneSystem.h"
#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/MeshSystem.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "Math/Vectors.h"
#include "Component/RigidBody.h"
#include "Component/Circle.h"
#include "IO/TextSerialiser.h"
#include "Component/Convex.h"
#include "Component/PointCollider.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif

namespace Dystopia
{
	using Math::Vec3D;
	Circle::Circle()
		: Collider{ Vec3D{ 0,0,0 },Math::MakePoint3D(0,0,0) }
		, m_radius(1.0F)
	{

	}

	Circle::Circle(float const & _radius, Vec3D const & _v3Offset)
		: Collider{ _v3Offset,Math::MakePoint3D(0,0,0) }
		, m_radius(_radius)
	{

	}

	void Circle::Awake(void)
	{
		static Mesh * Mesh = nullptr;

		mScale[0] = mScale[1] = m_radius;

		if(!Mesh)
		{
			mDebugVertices.clear();

			const unsigned numberOfSegments = 31;
			const float increment = 2.0f * Math::pi / float(numberOfSegments);

			for (unsigned i = 0; i < numberOfSegments; ++i)
			{
				Vec3D vertex = 0.5F * Vec3D{ cosf(increment*i), sinf(increment*i), 0 };
				mDebugVertices.EmplaceBack(vertex.x, vertex.y, 0.f);
			}

			Triangulate();

			if (this->mDebugVertices.size() == 0 || this->mIndexBuffer.size() == 0)
				throw;

			auto * pMeshSys = EngineCore::GetInstance()->Get<MeshSystem>();
			if (pMeshSys)
			{
				/*Create Mesh*/
				pMeshSys->StartMesh();
				
				auto const & arr = GetVertexBuffer();

				pMeshSys->AddVertex(0,0,0);
				pMeshSys->AddNormal(0,0,1);

				for (auto i : arr)
				{
					pMeshSys->AddVertex(i.x, i.y, i.z);
					pMeshSys->AddNormal(i.x, i.y, i.z);
				}

				SetMesh(pMeshSys->AddIndices("Collider Mesh", GetIndexBuffer()));
				pMeshSys->EndMesh();
			}
			Mesh = GetMesh();
		}
		mpMesh = Mesh;
	}

	/*Load the Component*/
	void Circle::Load(void)
	{
		
	}
	/*Initialise the Component*/
	void Circle::Init(void)
	{
	}

	void Circle::Update(float)
	{

	}

	/*OnDestroy*/
	void Circle::OnDestroy(void)
	{
		
	}

	/*Unload the Component*/
	void Circle::Unload(void)
	{
		
	}
	/*Duplicate the Component*/
	Circle* Circle::Duplicate() const
	{
		return static_cast<ComponentDonor<Circle> *>(EngineCore::GetInstance()->GetSystem<Circle::SYSTEM>())->RequestComponent(*this);
	}

	BroadPhaseCircle Circle::GenerateBoardPhaseCircle() const
	{
		return BroadPhaseCircle(GetRadius(), GetGlobalPosition());
	}

	float Circle::GetRadius() const
	{
		const float Scale = Math::Abs(mOwnerTransformation[0]) > Math::Abs(mOwnerTransformation[5]) ? mOwnerTransformation[0] : mOwnerTransformation[5];
		return Math::Abs(m_radius * Scale * 0.5f);
	}

	float Circle::GetRadiusRaw() const
	{
		return m_radius;
	}

	void Circle::SetRadius(float _radius)
	{
		m_radius = _radius;
	}

	/*Serialise and Unserialise*/
	void Circle::Serialise(TextSerialiser& _out) const
	{
		_out.InsertStartBlock("Circle_Collider2D");
		Component::Serialise(_out);
		_out << float(mv3Offset[0]);		// offset for colliders
		_out << float(mv3Offset[1]);
		_out << float(mv3Offset[2]);

		_out << static_cast<float>(mPosition[0]);
		_out << static_cast<float>(mPosition[1]);
		_out << static_cast<float>(mPosition[2]);

		/*THIS IS THE REAL VERSION*/
		_out << float(m_radius);
		_out << mbIsTrigger;
		_out << static_cast<unsigned>(mColLayer);
		_out.InsertEndBlock("Circle_Collider2D");
	}
	void Circle::Unserialise(TextSerialiser& _in)
	{
		unsigned collayer_temp = 0;
		_in.ConsumeStartBlock();
		Component::Unserialise(_in);
		_in >> mv3Offset[0];
		_in >> mv3Offset[1];
		_in >> mv3Offset[2];

		_in >> (mPosition[0]);
		_in >> (mPosition[1]);
		_in >> (mPosition[2]);
		_in >> m_radius;
		_in >> mbIsTrigger;
		_in >> collayer_temp;
		_in.ConsumeEndBlock();

		mDebugVertices.clear();
		mScale[0] = m_radius;
		mScale[1] = m_radius;
		mColLayer = static_cast<eColLayer>(collayer_temp);
	}

	/*Collision Check Functions*/
	bool Circle::isColliding(Circle & other_col, Math::Point3D other_pos)
	{
		//const auto this_body       = *GetOwner()->GetComponent<RigidBody>();
		const auto other_body      = other_col.GetOwner()->GetComponent<RigidBody>();
		const auto this_pos        = this->GetGlobalPosition();
		const auto positionDelta   =  other_pos - this_pos;
		const float combinedRadius = this->GetRadius() + other_col.GetRadius();
		CollisionEvent col_info(GetOwner(), other_col.GetOwner());
		col_info.isATrigger = this->mbIsTrigger;
		col_info.isBTrigger = other_col.mbIsTrigger;

		// If the position delta is < combined radius, it is colliding
		if (positionDelta.MagnitudeSqr() < combinedRadius*combinedRadius) // collided, getCollisionEvent
		{
			Vec3D contactPoint;
			Vec3D normal;
			float penetration;
			const float dis = positionDelta.Magnitude();

			if (dis > FLT_EPSILON) // dis != 0
			{
				penetration = combinedRadius - dis;
				normal = positionDelta/dis;
				contactPoint = (GetGlobalPosition() * other_col.GetRadius()) + (other_col.GetGlobalPosition() * GetRadius()) / combinedRadius;
			}
			else //dis == 0
			{
				//Exact Overlap of each other
				penetration = GetRadius();
				normal = Vec3D{ 0,1,0 };
				contactPoint = GetGlobalPosition();
			}

			normal.z = 0;

			// Add to Collision Events
			mbColliding = true;
			other_col.mbColliding = true;

			//CollisionEvent col_info(GetOwner(), other_col.GetOwner());
			col_info.mEdgeNormal			= normal;
			col_info.mEdgeVector			= Math::Normalise(positionDelta);
			col_info.mCollisionPoint		= contactPoint;
			col_info.mfPeneDepth			= penetration;
			if (nullptr != other_body)
			{
				col_info.mfRestitution = DetermineRestitution(*other_body);
				col_info.mfStaticFrictionCof = DetermineStaticFriction(*other_body);
				col_info.mfDynamicFrictionCof = DetermineKineticFriction(*other_body);
			}
			//InformOtherComponents(true, col_info);
			marr_CurrentContactSets.push_back(col_info);
			return true;
		}
			//InformOtherComponents(false, col_info);
			return false;
	}

	bool Circle::isColliding(Circle * const & other_col)
	{
		return this->isColliding(*other_col, other_col->GetGlobalPosition());
	}
	bool Circle::isColliding(const AABB & other_col)
	{
		UNUSED_PARAMETER(other_col);
		return false;
	}
	bool Circle::isColliding(const AABB * const & other_col)
	{
		UNUSED_PARAMETER(other_col);
		return this->isColliding(*other_col);

	}
	bool Circle::isColliding(Convex& other_col)
	{
		RigidBody* other_body{ nullptr };
		if (other_col.GetOwner()->GetComponent<RigidBody>())
			other_body = other_col.GetOwner()->GetComponent<RigidBody>();

		CollisionEvent newEvent(this->GetOwner(), other_col.GetOwner());
		newEvent.isATrigger = this->mbIsTrigger;
		newEvent.isBTrigger = other_col.mbIsTrigger;
		newEvent.mEdgeNormal = Math::MakeVector3D(0, 0, 0);
		AutoArray<Edge> const & ConvexEdges = other_col.GetConvexEdges();

		bool isInside = true;
		/*Check for Circle inside Convex*/
		for (auto & elem : ConvexEdges)
		{
			if (elem.mNorm3.Dot(elem.mPos - GetGlobalPosition()) < 0)
			{
				isInside = false;
			}
		}

		if (!isInside)
		{
			float distance = 0;
			for (auto & elem : ConvexEdges)
			{
				Vec3D v = elem.mVec3;
				Vec3D w = GetGlobalPosition() - elem.mPos;
				Vec3D norm;
				const float c1 = v.Dot(w);
				const float c2 = v.Dot(v);
				float ratio = 0.f;
				Point3D PointOfImpact;
				if (c1 < 0)
				{
					distance = w.Magnitude();
					norm = -w;
				}
				else if (c1 > c2)
				{
					distance = (GetGlobalPosition() - (v + elem.mPos)).Magnitude();
					norm = -(GetGlobalPosition() - (v + elem.mPos));
				}
				else
				{
					ratio = c1 / c2;
					PointOfImpact = elem.mPos + ratio * elem.mVec3;
					distance = (GetGlobalPosition() - PointOfImpact).Magnitude();
					norm = -(GetGlobalPosition() - PointOfImpact);
				}

				if (distance <= GetRadius())
				{
					newEvent.mfPeneDepth     = GetRadius() - distance;
					newEvent.mEdgeNormal     += norm;
					newEvent.mEdgeVector     = Math::Vec3D{ newEvent.mEdgeNormal.yxzw }.Negate< Math::NegateFlag::X>();
					newEvent.mCollisionPoint = PointOfImpact;
					if (nullptr != other_body)
					{
						newEvent.mfRestitution        = DetermineRestitution(*other_body);
						newEvent.mfDynamicFrictionCof = DetermineKineticFriction(*other_body);
						newEvent.mfStaticFrictionCof  = DetermineStaticFriction(*other_body);
					}
					mbColliding = isInside  = true;
					break;
				}
			}
			if (isInside)
			{
				newEvent.mEdgeNormal = newEvent.mEdgeNormal.Normalise();
				newEvent.mEdgeNormal.z = 0;
				marr_CurrentContactSets.push_back(newEvent);
				//InformOtherComponents(true, newEvent);
				return true;
			}
			return false;
		}
		else
		{
			/*Circle completely inside*/
			newEvent.mfPeneDepth = FLT_MAX;

			for (auto & elem : ConvexEdges)
			{
				Vec3D v = elem.mVec3;
				Vec3D w = GetGlobalPosition() - elem.mPos;

				if (Math::Abs(w.Dot(elem.mNorm3.Normalise())) < newEvent.mfPeneDepth)
				{
					//currPene = (GetGlobalPosition() - PointOfImpact).Magnitude();
					newEvent.mEdgeNormal = -elem.mNorm3;
					newEvent.mfPeneDepth = Math::Abs(w.Dot(elem.mNorm3.Normalise())) + GetRadius();

				}
			}

			marr_CurrentContactSets.push_back(newEvent);
		}

		mbColliding = isInside;
		return isInside;
	}
	bool Circle::isColliding(Convex * const & other_col)
	{
		return this->isColliding(*other_col);
	}

	bool Circle::isColliding(PointCollider & other_col)
	{
		/*Distance Squared from Point to Circle*/
		float dist = (GetGlobalPosition() - other_col.GetGlobalPosition()).MagnitudeSqr();
		/*Radius squared of Circle*/
		float r2 = GetRadius() * GetRadius();
		/*If the distance squared is less than the radius squared, there is collision*/
		if (dist < r2)
		{
			/*New Collision Event*/
			CollisionEvent ColEvent{ GetOwner(), other_col.GetOwner() };
			/*
			  The collision event is other - me because physics will negate the normal on its side
			  which will become me - other (which is the correct version)
			 */
			ColEvent.mEdgeNormal = other_col.GetGlobalPosition() - GetGlobalPosition();
			ColEvent.mfPeneDepth = sqrtf(r2 - dist);
			/*Add the collision Event*/
			marr_ContactSets.push_back(Ut::Move(ColEvent));
			mbColliding = true;
			return true;
		}
		return false;
	}

	bool Circle::isColliding(PointCollider * const & other_col)
	{
		return isColliding(*other_col);
	}

	
#if EDITOR
	void Circle::ePositionOffsetVectorFields()
	{
		EGUI::Display::Label("Offset");
		auto arrResult = EGUI::Display::VectorFields("    ", &mv3Offset, 0.01f, -FLT_MAX, FLT_MAX);
		for (auto &e : arrResult)
		{
			switch (e)
			{
			case EGUI::eDragStatus::eSTART_DRAG:
				//EGUI::GetCommandHND()->StartRecording<Collider>(mnOwner, &Circle::mv3Offset);
				break;
			case EGUI::eDragStatus::eEND_DRAG:
			case EGUI::eDragStatus::eENTER:
			case EGUI::eDragStatus::eDRAGGING:
			case EGUI::eDragStatus::eTABBED:
			case EGUI::eDragStatus::eDEACTIVATED:
				Init();
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eNO_CHANGE:
			default:
				break;
			}
		}
	}

	void Circle::eScaleField()
	{
		EGUI::Display::Label("Scale");
		const auto e = EGUI::Display::DragFloat("   ", &m_radius, 0.01f, -FLT_MAX, FLT_MAX);
		switch (e)
		{
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<Circle>(mnOwner, &Circle::m_radius);
			break;
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eDRAGGING:
		case EGUI::eDragStatus::eTABBED:
		case EGUI::eDragStatus::eDEACTIVATED:
			Init();
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eNO_CHANGE:
		default:
			break;
		}
		mScale.x = m_radius;
		mScale.y = m_radius;
	}

	void Circle::eAttachedBodyEmptyBox()
	{
		HashString bodyAttached;
		if (GetOwner()->GetComponent<RigidBody>())
			bodyAttached = GetOwner()->GetName();
		else
			bodyAttached = "None";

		EGUI::Display::Label("Attached Body");
		EGUI::Display::EmptyBox("		", 180.f, bodyAttached.c_str(), false, true);
	}

	void Circle::eNumberOfContactsLabel()
	{
		if (EGUI::Display::CollapsingHeader("Contacts"))
		{
			for (unsigned int i = 0; i < marr_ContactSets.size(); ++i)
			{
				EGUI::PushID(i);
				auto& c = marr_ContactSets[i];
				HashString name = c.mCollidedWith->GetName();
				EGUI::Display::EmptyBox("		", 180.f, name.c_str(), false, true);
				EGUI::PopID();
			}

			if (marr_ContactSets.IsEmpty())
			{
				EGUI::Display::EmptyBox("		", 180.f, "No Contacts", false, true);
			}
		}
	}

	void Circle::eIsTriggerCheckBox()
	{
		//bool tempBool = mbIsTrigger;

		if (EGUI::Display::CheckBox("Is Trigger		  ", &mbIsTrigger))
		{
			//mbIsTrigger = tempBool;
			//EGUI::GetCommandHND()->InvokeCommand<Collider>(mnOwner, &Collider::mbIsTrigger, tempBool);
		}
	}

	void Circle::EditorUI() noexcept
	{
		eAttachedBodyEmptyBox();
		eIsTriggerCheckBox();
		ePositionOffsetVectorFields();
		eScaleField();
		eNumberOfContactsLabel();
		Collider::EditorUI();
	}
#endif
}
