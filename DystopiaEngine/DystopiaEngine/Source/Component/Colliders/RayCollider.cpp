#include "Component/RayCollider.h"

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
#include "Component/PointCollider.h"


#include "IO/TextSerialiser.h"
#if EDITOR
#include "Editor/EGUI.h"
#endif

namespace Dystopia
{
	Dystopia::RayCollider::RayCollider(void)
		:Collider{}, mRayDir{0.f,0.f,0.f}, mMaxLength{0}
	{
	}

	Dystopia::RayCollider::RayCollider(Math::Point3D const & _Pos, Math::Vec3D const & _Dir, float const & _MaxLength, Math::Vec3D const & _Offset)
		:Collider{_Offset,_Pos},mRayDir{ Math::Normalise(_Dir) }, mMaxLength{ _MaxLength }
	{
	}

	void Dystopia::RayCollider::Awake(void)
	{
	}

	void Dystopia::RayCollider::Load(void)
	{
	}

	void Dystopia::RayCollider::Init(void)
	{
	}

	void Dystopia::RayCollider::Update(float)
	{
	}

	void Dystopia::RayCollider::OnDestroy(void)
	{
	}

	void Dystopia::RayCollider::Unload(void)
	{
	}

	RayCollider * Dystopia::RayCollider::Duplicate() const
	{
		return static_cast<ComponentDonor<RayCollider> *>(EngineCore::GetInstance()->GetSystem<RayCollider::SYSTEM>())->RequestComponent(*this);
	}

	BroadPhaseCircle Dystopia::RayCollider::GenerateBoardPhaseCircle() const
	{
		return BroadPhaseCircle{ mMaxLength, GetGlobalPosition() };
	}

	void Dystopia::RayCollider::Serialise(TextSerialiser & _out) const
	{
		_out.InsertStartBlock("Ray_Collider2D");
		Component::Serialise(_out);
		_out << float(mv3Offset[0]);		// offset for colliders
		_out << float(mv3Offset[1]);
		_out << float(mv3Offset[2]);

		_out << static_cast<float>(mPosition[0]);
		_out << static_cast<float>(mPosition[1]);
		_out << static_cast<float>(mPosition[2]);
		_out << mbIsTrigger;

		_out << static_cast<float>(mRayDir[0]);
		_out << static_cast<float>(mRayDir[1]);
		_out << static_cast<float>(mRayDir[2]);
		_out << mMaxLength;
		_out.InsertEndBlock("Ray_Collider2D");
	}

	void Dystopia::RayCollider::Unserialise(TextSerialiser & _in)
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

		_in >> mRayDir[0];
		_in >> mRayDir[1];
		_in >> mRayDir[2];
		_in >> mMaxLength;

		_in.ConsumeEndBlock();

		mDebugVertices.clear();
	}

	bool Dystopia::RayCollider::isColliding(Circle & other_col)
	{
		/*Check if it is infinite ray*/
	    /*Ray going in opposite direction of Collider*/
		Math::Vec3D && v = other_col.GetGlobalPosition() - GetGlobalPosition();
		if (mRayDir.Dot(v) < 0.f)
			return false;
		auto l = v.Dot(mRayDir);
		auto && w = mRayDir * l;
		auto && u = w - v;
		if (u.MagnitudeSqr() > other_col.GetRadius() * other_col.GetRadius())
			return false;
		auto && n = sqrtf(other_col.GetRadius() * other_col.GetRadius() - u.MagnitudeSqr());
		auto && t = l - n;

		switch (isFinite())
		{
		case true:
			if (t > mMaxLength)
				return false;
			/*No break here on purpose*/
		case false:

			mbColliding = true;
			/*Insert Collision Info*/
			CollisionEvent ColEvent{ GetOwner(), other_col.GetOwner() };
			ColEvent.mEdgeNormal = other_col.GetGlobalPosition() - GetGlobalPosition() + (mRayDir*t);
			ColEvent.mTimeIntersection = t;
			marr_ContactSets.push_back(Ut::Move(ColEvent));

			return true;
			break;
		}
		return false;
	}

	bool Dystopia::RayCollider::isColliding(Circle * const & other_col)
	{
		return isColliding(*other_col);
	}

	bool Dystopia::RayCollider::isColliding(AABB & other_col)
	{


		return false;
	}

	bool Dystopia::RayCollider::isColliding(AABB * const & other_col)
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(Convex & other_col)
	{
		/*Check if ray is travelling toward object*/
		Math::Vec3D && v = other_col.GetGlobalPosition() - GetGlobalPosition();
		if (v.Dot(mRayDir) < 0.f)
			return false;
		auto && ListOfEdge = other_col.GetConvexEdges();
		for (auto const & elem : ListOfEdge)
		{
			/*Check if the edge normal is facing the ray*/
			if (elem.mNorm3.Dot(mRayDir) >= 0.f)
				continue;
			/*Check if the ray lies within the edge*/
		}
		return false;
	}

	bool Dystopia::RayCollider::isColliding(Convex * const & other_col)
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(PointCollider & other_col)
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(PointCollider * const & other_col)
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(RayCollider & other_col)
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(RayCollider * const & other_col)
	{
		return false;
	}
	bool RayCollider::isFinite() const
	{
		return mMaxLength;
	}
	CollisionEvent RayCollider::GetFirstHitEvent() const
	{
		return CollisionEvent(nullptr, nullptr);
	}
	Collider * RayCollider::GetFirstHit() const
	{
		return nullptr;
	}
}

