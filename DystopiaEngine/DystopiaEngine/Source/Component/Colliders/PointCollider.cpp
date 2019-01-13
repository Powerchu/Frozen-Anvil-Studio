#include "Component/PointCollider.h"

#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Scene/SceneSystem.h"
#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/MeshSystem.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "Math/Vector4.h"
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

		_out.InsertEndBlock("Circle_Collider2D");

	}
	void PointCollider::Unserialise(TextSerialiser & _in)
	{
		_in.ConsumeStartBlock();
		Component::Unserialise(_in);
		_in >> mv3Offset[0];
		_in >> mv3Offset[1];
		_in >> mv3Offset[2];

		_in >> (mPosition[0]);
		_in >> (mPosition[1]);
		_in >> (mPosition[2]);
		_in >> mbIsTrigger;
		_in.ConsumeEndBlock();

		mDebugVertices.clear();
	}
	bool PointCollider::isColliding(Circle & other_col)
	{
		return  (GetGlobalPosition() - other_col.GetGlobalPosition()).MagnitudeSqr() < other_col.GetRadius() * other_col.GetRadius();
	}
	bool PointCollider::isColliding(Circle * const & other_col)
	{
		return  (GetGlobalPosition() - other_col->GetGlobalPosition()).MagnitudeSqr() < other_col->GetRadius() * other_col->GetRadius();
	}
	bool PointCollider::isColliding(const AABB & other_col)
	{
		auto && AllEdge = other_col.GetConvexEdges();
		for (auto & edge : AllEdge)
		{
			if (edge.mNorm3.Dot(GetGlobalPosition() - edge.mPos) > 0.f)
				return false;
		}
		return true;
	}
	bool PointCollider::isColliding(const AABB * const & other_col)
	{
		auto && AllEdge = other_col->GetConvexEdges();
		for (auto & edge : AllEdge)
		{
			if (edge.mNorm3.Dot(GetGlobalPosition() - edge.mPos) > 0.f)
				return false;
		}
		return true;
	}
	bool PointCollider::isColliding(Convex & other_col)
	{
		auto && AllEdge = other_col.GetConvexEdges();
		for (auto & edge : AllEdge)
		{
			if (edge.mNorm3.Dot(GetGlobalPosition() - edge.mPos) > 0.f)
				return false;
		}
		return true;
	}
	bool PointCollider::isColliding(Convex * const & other_col)
	{
		auto && AllEdge = other_col->GetConvexEdges();
		for (auto & edge : AllEdge)
		{
			if (edge.mNorm3.Dot(GetGlobalPosition() - edge.mPos) > 0.f)
				return false;
		}
		return true;
	}
	bool PointCollider::isColliding(PointCollider & other_col)
	{
		return GetGlobalPosition() == other_col.GetGlobalPosition();
	}
	bool PointCollider::isColliding(PointCollider * const & other_col)
	{
		return GetGlobalPosition() == other_col->GetGlobalPosition();
	}
}