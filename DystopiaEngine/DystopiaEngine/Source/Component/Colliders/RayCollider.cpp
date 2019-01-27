#include "Component/RayCollider.h"

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
		if (GetGlobalRay().Dot(v) < 0.f)
			return false;
		auto l = v.Dot(GetGlobalRay());
		auto && w = GetGlobalRay() * l;
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
			ColEvent.mEdgeNormal = other_col.GetGlobalPosition() - GetGlobalPosition() + (GetGlobalRay()*t);
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

	bool Dystopia::RayCollider::isColliding(AABB & )
	{


		return false;
	}

	bool Dystopia::RayCollider::isColliding(AABB * const & )
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(Convex & other_col)
	{
		/*Check if ray is travelling toward object*/
		Math::Vec3D && v = other_col.GetGlobalPosition() - GetGlobalPosition();
		if (v.Dot(GetGlobalRay()) < 0.f)
			return false;
		auto && ListOfEdge   = other_col.GetConvexEdges();
		bool    isColliding  = false;
		CollisionEvent ColEvent{ GetOwner(), other_col.GetOwner() };
		ColEvent.mTimeIntersection = mMaxLength ? mMaxLength : 9999.f;
		for (auto const & elem : ListOfEdge)
		{
			/*Check if the edge normal is facing the ray*/
			if (elem.mNorm3.Dot(GetGlobalRay()) >= 0.f)
				continue;
			/*Check if the ray lies within the edge*/
			auto && v1 = GetGlobalPosition() - elem.mPos;
			auto && v2 = GetGlobalPosition() -(elem.mPos + elem.mVec3);
#if CLOCKWISE
			
#else
			v1.Negate<Math::NegateFlag::Y>();
			v2.Negate<Math::NegateFlag::Y>();
#endif
			/*If either results are negative, the ray lies outside of the cone*/
			if (v1.Dot(GetGlobalRay()) < 0.f || v2.Dot(GetGlobalRay()) < 0.f)
				return false;
			/*Get the intersection time*/
			float cosTheta = GetGlobalRay().Dot(elem.mNorm3);
			float adj      = Math::Abs(v1.Dot(elem.mNorm3));
			float time     = adj / cosTheta;
			/*If the time of intersection to the edge is less than the current time of intersection, update it*/
			if (time < ColEvent.mTimeIntersection)
			{
				ColEvent.mTimeIntersection = ColEvent.mTimeIntersection > time ? time : ColEvent.mTimeIntersection;
				ColEvent.mEdgeNormal       = elem.mNorm3;
				ColEvent.mfPeneDepth       = 0;
				ColEvent.mCollisionPoint   = GetGlobalPosition() + ColEvent.mTimeIntersection * GetGlobalRay();
				isColliding = true;
			}
		}
		if (isColliding)
		{
			/*Insert Collsion Event*/
			marr_ContactSets.push_back(ColEvent);
		}
		return isColliding;
	}

	bool Dystopia::RayCollider::isColliding(Convex * const & )
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(PointCollider & )
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(PointCollider * const & )
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(RayCollider & )
	{
		return false;
	}

	bool Dystopia::RayCollider::isColliding(RayCollider * const & )
	{
		return false;
	}
	bool RayCollider::isFinite() const
	{
		return mMaxLength;
	}
	Math::Vec3D RayCollider::GetGlobalRay() const
	{
		return (mOwnerTransformation * Math::Translate(mv3Offset.x, mv3Offset.y, mv3Offset.z) * GetTransformationMatrix() * mRayDir).Normalise();
	}
	CollisionEvent RayCollider::GetFirstHitEvent() const
	{
		return CollisionEvent(nullptr, nullptr);
	}
	Collider * RayCollider::GetFirstHit() const
	{
		return nullptr;
	}
	bool RayCollider::Raycast(Math::Vec3D const & _RayDir, Math::Point3D const & _Pos, Collider * _Collider, CollisionEvent * _OutputResult, float _MaxLength)
	{
		if (auto * ptr = dynamic_cast<Convex*>(_Collider))
		{
			return Raycast_Convex(_RayDir, _Pos, ptr, _OutputResult, _MaxLength);
		}
		else if (auto * ptr2 = dynamic_cast<Circle*>(_Collider))
		{
			return Raycast_Circle(_RayDir, _Pos, ptr2, _OutputResult, _MaxLength);
		}
		else
		{
			/*Default Collider*/
			return false;
		}
	}
	bool RayCollider::Raycast_Circle(Math::Vec3D const & _RayDir, Math::Point3D const & _Pos, Circle * _Collider, CollisionEvent * _OutputResult, float _MaxLength)
	{
		/*Check if it is infinite ray*/
        /*Ray going in opposite direction of Collider*/
		Math::Vec3D && v = _Collider->GetGlobalPosition() - _Pos;
		bool isFinite    = _MaxLength;
		if (_RayDir.Dot(v) < 0.f)
			return false;
		auto l = v.Dot(_RayDir);
		auto && w = (_RayDir) * l;
		auto && u = w - v;
		if (u.MagnitudeSqr() > _Collider->GetRadius() * _Collider->GetRadius())
			return false;



		auto && n = sqrtf(_Collider->GetRadius() * _Collider->GetRadius() - u.MagnitudeSqr());
		auto && t = l - n;

		switch (isFinite)
		{
		case true:
			if (t > _MaxLength)
				return false;
			/*No break here on purpose*/
		case false:

			/*Insert Collision Info*/
			_OutputResult->mEdgeNormal       = _Collider->GetGlobalPosition() - _Pos + (_RayDir*t);
			_OutputResult->mTimeIntersection = t;
			_OutputResult->mCollisionPoint   = _Pos + (_RayDir*t);
			_OutputResult->mCollidedWith     = _Collider->GetOwner();
			return true;
			break;
		}
		return false;
	}
	bool RayCollider::Raycast_Convex(Math::Vec3D const & _RayDir, Math::Point3D const & _Pos, Convex * _Collider, CollisionEvent * _OutputResult, float _MaxLength)
	{
		/*Check if ray is travelling toward object*/
		Math::Vec3D && v = _Collider->GetGlobalPosition() - _Pos;
		if (v.Dot(_RayDir) < 0.f)
			return false;
		auto && ListOfEdge = _Collider->GetConvexEdges();
		bool    isColliding = false;
		_OutputResult->mTimeIntersection =  99999.f;
		for (auto const & elem : ListOfEdge)
		{
			/*Check if the edge normal is facing the ray*/
			if (elem.mNorm3.Dot(_RayDir) >= 0.f || (_Pos - elem.mPos).Dot(_RayDir) > 0.f)
				continue;
			/*Check if the ray lies within the edge*/
			auto && v1      = _Pos - elem.mPos;
			auto  v1_copy   = v1;
			auto && v2 = _Pos - (elem.mPos + elem.mVec3);
			if (v1.Dot(elem.mVec3) < 0.f)
			{
#if CLOCKWISE

#else
				v1.xyzw = v1.yxzw;
				v2.xyzw = v2.yxzw;
				v1.Negate<Math::NegateFlag::Y>();
				v2.Negate<Math::NegateFlag::X>();
#endif

		}
			else if (v1.Dot(elem.mVec3) > elem.mVec3.Dot(elem.mVec3))
			{
#if CLOCKWISE

#else
				v1.xyzw = v1.yxzw;
				v2.xyzw = v2.yxzw;
				v1.Negate<Math::NegateFlag::Y>();
				v2.Negate<Math::NegateFlag::X>();
#endif
			}
			else
			{
#if CLOCKWISE

#else
				v1.xyzw = v1.yxzw;
				v2.xyzw = v2.yxzw;
				v1.Negate<Math::NegateFlag::Y>();
				v2.Negate<Math::NegateFlag::X>();
#endif
			}
			if (v1.Dot(_RayDir) > 0.f || v2.Dot(_RayDir) > 0.f)
				return false;
			/*Get the intersection time*/
			float cosTheta = _RayDir.Dot(-elem.mNorm3);
			float adj = Math::Abs(v1_copy.Dot(elem.mNorm3));
			float time = adj / cosTheta;
			/*If the time of intersection to the edge is less than the current time of intersection, update it*/

			if (time < _OutputResult->mTimeIntersection)
			{
				if(_MaxLength)
					if ((time * _RayDir).MagnitudeSqr() > _MaxLength * _MaxLength)
						continue;
				DEBUG_PRINT(eLog::ERROR, "My Pos      %f %f %f \n", static_cast<float>(_Pos.x), static_cast<float>(_Pos.y), static_cast<float>(_Pos.z));
				DEBUG_PRINT(eLog::ERROR, "Edge origin %f %f %f \n", static_cast<float>(elem.mPos.x), static_cast<float>(elem.mPos.y), static_cast<float>(elem.mPos.z));
				DEBUG_PRINT(eLog::ERROR, "Edge Normal %f %f %f \n", static_cast<float>(elem.mNorm3.x), static_cast<float>(elem.mNorm3.y), static_cast<float>(elem.mNorm3.z));
				DEBUG_PRINT(eLog::ERROR, "CosTheta    %f", cosTheta);
				DEBUG_PRINT(eLog::ERROR, "Adj         %f", adj);
				DEBUG_PRINT(eLog::ERROR, "Time         %f",time);
				_OutputResult->mTimeIntersection = _OutputResult->mTimeIntersection > time ? time : _OutputResult->mTimeIntersection;
				_OutputResult->mEdgeNormal = elem.mNorm3;
				_OutputResult->mfPeneDepth = 0;
				_OutputResult->mCollisionPoint = _Pos + _OutputResult->mTimeIntersection * _RayDir;
				_OutputResult->mCollidedWith = _Collider->GetOwner();
				isColliding = true;
			}
		}
		return isColliding;
	}
}

