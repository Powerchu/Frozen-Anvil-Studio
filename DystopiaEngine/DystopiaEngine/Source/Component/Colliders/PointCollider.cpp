#include "Component/PointCollider.h"

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
#include "Component/Convex.h"
#include "Component/AABB.h"

#include "IO/TextSerialiser.h"
namespace Dystopia
{
	PointCollider::PointCollider(void)
		:Collider{}
	{
	}
	PointCollider::PointCollider(Math::Point3D const & _Pos, Math::Vec3D const & _Offset)
		:Collider{ _Offset,_Pos }
	{
	}
	void PointCollider::Awake(void)
	{
	}
	void PointCollider::Load(void)
	{
	}
	void PointCollider::Init(void)
	{
	}
	void PointCollider::Update(float)
	{
	}
	void PointCollider::OnDestroy(void)
	{
	}
	void PointCollider::Unload(void)
	{
	}
	PointCollider * PointCollider::Duplicate() const
	{
		return static_cast<ComponentDonor<PointCollider> *>(EngineCore::GetInstance()->GetSystem<PointCollider::SYSTEM>())->RequestComponent(*this);
	}
	BroadPhaseCircle PointCollider::GenerateBoardPhaseCircle() const
	{
		return BroadPhaseCircle(0.f, GetGlobalPosition());
	}
	void PointCollider::Serialise(TextSerialiser & _out) const
	{
		_out.InsertStartBlock("Circle_Collider2D");
		Component::Serialise(_out);
		_out << float(mv3Offset[0]);		// offset for colliders
		_out << float(mv3Offset[1]);
		_out << float(mv3Offset[2]);

		_out << static_cast<float>(mPosition[0]);
		_out << static_cast<float>(mPosition[1]);
		_out << static_cast<float>(mPosition[2]);
		_out << mbIsTrigger;
		_out << static_cast<unsigned>(mColLayer);
		_out.InsertEndBlock("Circle_Collider2D");

	}
	void PointCollider::Unserialise(TextSerialiser & _in)
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
		_in >> mbIsTrigger;
		_in >> collayer_temp;
		_in.ConsumeEndBlock();

		mDebugVertices.clear();
		mColLayer = static_cast<eColLayer>(collayer_temp);
	}
	bool PointCollider::isColliding(Circle & other_col)
	{
		/*Distance Squared from Point to Circle*/
		float dist = (GetGlobalPosition() - other_col.GetGlobalPosition()).MagnitudeSqr();
		/*Radius squared of Circle*/
		float r2 = other_col.GetRadius() * other_col.GetRadius();
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
	bool PointCollider::isColliding(Circle * const & other_col)
	{
		return  isColliding(*other_col);
	}
	bool PointCollider::isColliding(AABB & other_col)
	{
		return isColliding(&other_col);
	}
	bool PointCollider::isColliding(AABB * const & other_col)
	{
		return isColliding(static_cast<Convex * const &>(other_col));
	}
	bool PointCollider::isColliding(Convex & other_col)
	{
		return isColliding(&other_col);
	}
	bool PointCollider::isColliding(Convex * const & other_col)
	{
		auto && AllEdge = other_col->GetConvexEdges();
		Edge * ClosestEdge = nullptr;
		float  ClosestDistance = 0.f;
		float dist = 0.f;

		CollisionEvent ColEvent{ GetOwner(), other_col->GetOwner() };
		/*Check against all the edges*/
		for (auto & edge : AllEdge)
		{


			/*Ceck if the point is within the Edge's Inside Halfspaces*/
			if (edge.mNorm3.Dot(GetGlobalPosition() - edge.mPos) > 0.f)
			{
				Vec3D v = edge.mVec3;
				Vec3D w = GetGlobalPosition() - edge.mPos;
				Vec3D norm;
				float c1 = v.Dot((w));
				float c2 = v.Dot(v);
				float ratio = 0.f;
				Point3D PointOfImpact;
				/*
				Determine which part of the edge does the point lies on

				  Point(.)
							--------------

						OR
							   Point(.)
				 ------------
						OR

					Point(.)
				-----------------
				*/

				/*Case One*/
				if (c1 < 0)
				{
					ClosestDistance = w.Magnitude();
					norm = w;
				}
				/*Case Two*/
				else if (c1 > c2)
				{
					/*Get the distance from the end point of the line to the Point collider*/
					ClosestDistance = (GetGlobalPosition() - (edge.mPos + edge.mVec3)).Magnitude();
					/*The normal is from the other position to the line end point. (This is because physics side will negate again)
					  Originally should be line end point - other position*/
					norm = (GetGlobalPosition() - (v + edge.mPos));
				}
				/*Case Three*/
				else
				{
					/*Calculate the barycentric ratio*/
					ratio = c1 / c2;
					/*Get the point directly under the point collider*/
					PointOfImpact = edge.mPos + ratio * edge.mVec3;
					/*Get the closest distance which is the point directly under to _ColB*/
					ClosestDistance = (GetGlobalPosition() - PointOfImpact).Magnitude();
					/*The normal is from PointDirectly below to the Collider position (This is negated because physics side negates)
					  Original(PointOfImpact - _ColB's Position)*/
					norm = (GetGlobalPosition() - PointOfImpact);
				}

				/*If there is no closest edge, that means it is the first iteration*/
				if (!ClosestEdge)
				{
					/*Store the Collision Event*/
					ColEvent.mfPeneDepth = (PointOfImpact - GetGlobalPosition()).Magnitude();
					ColEvent.mEdgeNormal = -norm;
					ColEvent.mEdgeVector = Math::Vec3D{ ColEvent.mEdgeNormal.yxzw }.Negate< Math::NegateFlag::X>();
					ColEvent.mCollisionPoint = PointOfImpact;
					ClosestEdge = &edge;
				}
				else
				{
					/*Get the distance from the point to the edge origin*/
					/*If this edge is closer to the Point, assign it as the collided edge*/
					if (dist < ClosestDistance)
					{
						/*Store the Collision Event*/
						ColEvent.mfPeneDepth = (PointOfImpact - GetGlobalPosition()).Magnitude();
						ColEvent.mEdgeNormal = -norm;
						ColEvent.mEdgeVector = Math::Vec3D{ ColEvent.mEdgeNormal.yxzw }.Negate< Math::NegateFlag::X>();
						ColEvent.mCollisionPoint = PointOfImpact;
						ClosestEdge = &edge;
					}
				}
			}
			else
				return false;
		}
		/*Add in a new contact/collision event*/
		marr_ContactSets.push_back(Ut::Move(ColEvent));
		return true;
	}
	bool PointCollider::isColliding(PointCollider & other_col)
	{
		if (GetGlobalPosition() == other_col.GetGlobalPosition())
		{
			CollisionEvent ColEvent{ GetOwner(), other_col.GetOwner() };
			ColEvent.mEdgeNormal = GetGlobalPosition() - other_col.GetGlobalPosition();
			ColEvent.mfPeneDepth = 0.f;

			return true;
		}
		return false;
	}
	bool PointCollider::isColliding(PointCollider * const & other_col)
	{
		return isColliding(*other_col);
	}
}