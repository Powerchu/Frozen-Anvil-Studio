#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "Math/Vector4.h"
#include "Component/RigidBody.h"
#include "Component/Circle.h"
#include "IO/TextSerialiser.h"

namespace Dystopia
{
	using Math::Vec3D;
	Circle::Circle()
		: m_radius(0.5F)
		, m_originCentre{Vec3D{0,0,0} }
	{

	}

	Circle::Circle(float const & _radius, Vec3D const & _v3Offset)
		: m_radius(_radius)
		, m_originCentre{ _v3Offset }
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
			m_originCentre = mv3Offset;
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
			m_originCentre += GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
			const float _xScale = GetOwner()->GetComponent<Transform>()->GetScale().x;
			m_radius *= _xScale;
		}
		
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
		return new Circle{*this};
	}

	/*Serialise and Unserialise*/
	void Circle::Serialise(TextSerialiser& _out) const
	{
		_out.InsertStartBlock("Circle Collider2D");
		_out << mID;					// gObj ID
		_out << float(mv3Offset[0]);		// offset for colliders
		_out << float(mv3Offset[1]);
		_out << float(mv3Offset[2]);

		_out << float(m_radius);
		
		_out.InsertEndBlock("Circle Collider2D");
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
		const auto positionDelta = this_pos - other_pos;
		const auto disSquared = positionDelta.Dot(positionDelta);
		const float combinedRadius = this->m_radius + other_col.m_radius;
		

		// If the position delta is < combined radius, it is colliding
		if (disSquared < combinedRadius*combinedRadius) // collided, getCollisionEvent
		{
			Vec3D contactPoint;
			Vec3D normal;
			float penetration;
			const float dis = positionDelta.Magnitude();

			if (dis > FLT_EPSILON) // dis != 0
			{
				penetration = combinedRadius - dis;
				contactPoint = (m_originCentre * other_col.m_radius) + (other_col.m_originCentre * m_radius) / combinedRadius;
				const auto normalVec = other_col.m_originCentre - contactPoint;
				if (normalVec.MagnitudeSqr() > FLT_EPSILON)
				{
					normal = Normalise(normalVec);
				}
				const auto rv = this_body.GetLinearVelocity() - other_body.GetLinearVelocity();
				Vec3D tangent = { normal.y, -normal.x,0 };
				float length = rv.Dot(tangent);
				const auto velCompOnTangent = tangent * length;
				const auto reflectedVec = rv - velCompOnTangent;

				if (reflectedVec.MagnitudeSqr() > FLT_EPSILON)
				{
					normal = Normalise(reflectedVec);
				}

				/*contactPoint = (m_originCentre * other_col.m_radius) + (other_col.m_originCentre * m_radius) / combinedRadius;
				normal = Math::Normalise(other_col.m_originCentre - contactPoint);*/
				
			}
			else //dis == 0
			{
				//Right on top of each other
				penetration = this->m_radius;
				normal = Vec3D{ 0,1,0 };
				contactPoint = this->m_originCentre;
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
		return false;
	}
	bool Circle::isColliding(const AABB * const & other_col)
	{
		return this->isColliding(*other_col);

	}
	bool Circle::isColliding(const Convex & other_col)
	{
		return false;
	}
	bool Circle::isColliding(const Convex * const & other_col)
	{
		return this->isColliding(*other_col);
	}
	
}
