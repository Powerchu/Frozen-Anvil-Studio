#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "Math/Vector4.h"
#include "Component/RigidBody.h"
#include "Component/Circle.h"
#include "IO/TextSerialiser.h"
#include "Component/Convex.h"

namespace Dystopia
{
	using Math::Vec3D;
	Circle::Circle()
		: m_radius(0.5F)
		, Collider{Vec3D{0,0,0} }
	{

	}

	Circle::Circle(float const & _radius, Vec3D const & _v3Offset)
		: m_radius(_radius), Collider{ _v3Offset }
	{

	}

	/*Load the Component*/
	void Circle::Load(void)
	{
		
	}
	/*Initialise the Component*/
	void Circle::Init(void)
	{
		if (mv3Offset != Vec3D{0,0,0,0})
		{
			mPosition = mv3Offset;
		}

		if (mDebugVertices.size() == 0)
		{
			const unsigned numberOfSegments = 16;
			const float increment = 2.0f * Math::pi / float(numberOfSegments);

			const float theta = 0.0f;

			for (unsigned i = 0; i < numberOfSegments; ++i)
			{
				Vec3D vertex = 0.5F * Vec3D{ cosf(increment*i), sinf(increment*i), 0 };
				Collider::mDebugVertices.push_back(Vertex{ vertex.x, vertex.y, 0 });
			}

			Collider::Triangulate();
			Collider::Init();
		}

		if (nullptr != GetOwner())
		{
			mPosition += GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
			const float _xScale = GetOwner()->GetComponent<Transform>()->GetScale().x;
			m_radius *= _xScale;
		}
		
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
		return static_cast<ComponentDonor<Circle> *>(EngineCore::GetInstance()->GetSystem<Circle::SYSTEM>())->RequestComponent();
	}

	float Circle::GetRadius() const
	{
		return m_radius;
	}

	/*Serialise and Unserialise*/
	void Circle::Serialise(TextSerialiser& _out) const
	{
		_out.InsertStartBlock("Circle_Collider2D");
		_out << mID;					    // gObj ID
		_out << float(mv3Offset[0]);		// offset for colliders
		_out << float(mv3Offset[1]);
		_out << float(mv3Offset[2]);

		_out << float(m_radius);
		
		_out.InsertEndBlock("Circle_Collider2D");
	}
	void Circle::Unserialise(TextSerialiser& _in)
	{
		_in >> mID;					// gObj ID
		_in >> mv3Offset[0];
		_in >> mv3Offset[1];
		_in >> mv3Offset[2];
		_in >> m_radius;

		if (GameObject* owner =
			EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().FindGameObject(mID))
		{
			owner->AddComponent(this, Circle::TAG{});
			Init();
		}
	}

	/*Collision Check Functions*/
	bool Circle::isColliding(Circle & other_col, Vec3D other_pos)
	{
		const auto this_body = *GetOwner()->GetComponent<RigidBody>();
		const auto other_body = *other_col.GetOwner()->GetComponent<RigidBody>();
		const auto this_pos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
		const auto positionDelta =  other_pos - this_pos;
		const float combinedRadius = this->m_radius + other_col.m_radius;
		

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
				
			}
			else //dis == 0
			{
				//Exact Overlap of each other
				penetration = this->m_radius;
				normal = Vec3D{ 0,1,0 };
				contactPoint = this->mPosition;
			}

			// Add to Collision Events
			mbColliding = true;
			other_col.mbColliding = true;

			CollisionEvent col_info(GetOwner(), other_col.GetOwner());
			col_info.mEdgeNormal = normal;
			//col_info.mEdgeVector = Math::Normalise(positionDelta);
			col_info.mCollisionPoint = contactPoint;
			col_info.mdPeneDepth = static_cast<double>(penetration);
			col_info.mfRestitution = DetermineRestitution(other_body);
			col_info.mfStaticFrictionCof = DetermineStaticFriction(other_body);
			col_info.mfDynamicFrictionCof = DetermineKineticFriction(other_body);

			marr_ContactSets.Insert(col_info);
	
			/*Return true for collision*/
			return true;
		}
			return false;
	}

	bool Circle::isColliding(Circle * const & other_col)
	{
		return this->isColliding(*other_col, other_col->GetOwner()->GetComponent<Transform>()->GetGlobalPosition());
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
	bool Circle::isColliding(const Convex & other_col)
	{
		RigidBody* other_body{ nullptr };
		if (other_col.GetOwner()->GetComponent<RigidBody>())
			other_body = other_col.GetOwner()->GetComponent<RigidBody>();

		AutoArray<Edge> const & ConvexEdges = other_col.GetConvexEdges();
		bool isInside = true;
		/*Check for Circle inside Convex*/
		for (auto & elem : ConvexEdges)
		{
			if (elem.mNorm3.Dot(elem.mPos - GetPosition()) < 0)
			{
				isInside = false;
				break;
			}
		}

		if (!isInside)
		{
			float distance = 0;
			for (auto & elem : ConvexEdges)
			{
				Vec3D v = elem.mVec3;
				Vec3D w = GetPosition() - elem.mPos;
				float c1 = v.Dot((w));
				float c2 = v.Dot(v);
				float ratio = 0.f;
				Point3D PointOfImpact;
				if (c1 < 0)
				{
					distance = w.Magnitude();
				}
				else if (c1 > c2)
				{
					distance = (GetPosition() - (elem.mPos + elem.mVec3)).Magnitude();
				}
				else
				{
					ratio = c1 / c2;
					PointOfImpact = elem.mPos + ratio * elem.mVec3;
					distance = (GetPosition() - PointOfImpact).Magnitude();
				}

				if (distance < GetRadius())
				{
					isInside = true;
					CollisionEvent newEvent(this->GetOwner(), other_col.GetOwner());
					newEvent.mdPeneDepth     = GetRadius() - distance;
					newEvent.mEdgeNormal     = -elem.mNorm3.Normalise();
					newEvent.mEdgeVector     = elem.mVec3;
					newEvent.mCollisionPoint = PointOfImpact;
					if (nullptr != other_body)
					{
						newEvent.mfRestitution = DetermineRestitution(*other_body);
						newEvent.mfDynamicFrictionCof = DetermineKineticFriction(*other_body);
						newEvent.mfStaticFrictionCof = DetermineStaticFriction(*other_body);
					}
					marr_ContactSets.push_back(newEvent);
					mbColliding = isInside  = true;
				}
			}

		}

		return isInside;
	}
	bool Circle::isColliding(Convex * const & other_col)
	{
		UNUSED_PARAMETER(other_col);
		return this->isColliding(*other_col);
	}
	
}
