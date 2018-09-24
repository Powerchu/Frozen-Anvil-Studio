#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "Component/Circle.h"
#include "Object/GameObject.h"
#include "Math/Vector4.h"
#include "Component/RigidBody.h"

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
			m_radius *= _xScale * 0.5f;
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
	void Circle::Serialise(TextSerialiser&) const
	{
		
	}
	void Circle::Unserialise(TextSerialiser&)
	{
		
	}

	//CollisionEvent Circle::GetCollisionEvent(const Circle & other_col)
	//{
	//	static constexpr double EPSILON = 0.0001f;
	//	

	//	return col_info;
	//}

	/*Collision Check Functions*/
	bool Circle::isColliding(Circle & other_col, Vec3D other_pos)
	{
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
			if (disSquared < FLT_EPSILON || disSquared > FLT_EPSILON)
			{
				//Right on top of each other
				penetration = this->m_radius;
				normal = Vec3D{ 1,0,1,1 };
				contactPoint = this->m_originCentre;
			}
			else
			{
				const Vec3D norm_delta = Math::Normalise(positionDelta);
				float dis = norm_delta.Magnitude();
				penetration = combinedRadius - dis;
				normal = norm_delta;

				contactPoint = (m_originCentre * other_col.m_radius) + (other_col.m_originCentre * m_radius) / combinedRadius;
			}

			// Add to Collision Events
			mbColliding = true;
			other_col.mbColliding = true;

			CollisionEvent col_info(other_col.GetOwner());
			col_info.mEdgeNormal = normal;
			col_info.mCollisionPoint = contactPoint;
			col_info.mdPeneDepth = static_cast<double>(penetration);
			col_info.mfRestitution = DetermineRestitution(other_body);
			col_info.mfFrictionCof = DetermineFriction(other_body);

			mCollisionEvent.Insert(col_info);
	
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
