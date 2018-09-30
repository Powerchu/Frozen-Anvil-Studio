#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Scene/SceneSystem.h"
#include "Component/Convex.h"
#include "Component/RigidBody.h"
#include "Object/GameObject.h"
#include "IO/TextSerialiser.h"

namespace Dystopia
{
	Convex::Convex(Math::Point3D const & _v3Offset)
		:Collider{ _v3Offset },
		mVertices{
		Vertice{ Math::MakePoint3D(.5f,.5f,0) },
		Vertice{ Math::MakePoint3D(-.5f,.5f,0) },
		Vertice{ Math::MakePoint3D(-.5f,-.5f,0) },
		Vertice{ Math::MakePoint3D(.5f,-.5f,0) }
	}
	{

	}

	void Convex::Load()
	{

	}

	void Convex::Init()
	{
		for (auto & elem : mVertices)
		{
			auto offset = GetOffSet();
			Collider::mDebugVertices.push_back(Vertex{ elem.mPosition.x + offset.x, elem.mPosition.y + offset.y , elem.mPosition.z + offset.z });
		}

		if (nullptr != GetOwner())
		{
			const float _xScale = GetOwner()->GetComponent<Transform>()->GetScale().x;
			const float _yScale = GetOwner()->GetComponent<Transform>()->GetScale().y;

			if (mVertices.IsEmpty())
			{
				mVertices = {
					Vertice{ Math::MakePoint3D(.5f,.5f,0) },
					Vertice{ Math::MakePoint3D(-.5f,.5f,0) },
					Vertice{ Math::MakePoint3D(-.5f,-.5f,0) },
					Vertice{ Math::MakePoint3D(.5f,-.5f,0) }
				};
			}

			for (auto & elem : mVertices)
			{
				elem.mPosition.x = elem.mPosition.x * Math::Abs(_xScale);
				elem.mPosition.y = elem.mPosition.y * Math::Abs(_yScale);
			}
		}



		Collider::Triangulate();
		Collider::Init();

	}

	void Convex::OnDestroy()
	{

	}

	void Convex::Unload()
	{

	}

	void Convex::Serialise(TextSerialiser& _out) const
	{
		const float _xScale = GetOwner()->GetComponent<Transform>()->GetScale().x;
		const float _yScale = GetOwner()->GetComponent<Transform>()->GetScale().y;

		_out.InsertStartBlock("Convex Collider");
		_out << mID;						// gObj ID
		_out << float(mv3Offset[0]);		// offset for colliders
		_out << float(mv3Offset[1]);
		_out << float(mv3Offset[2]);

		_out << int(mVertices.size());

		for (const auto& vertex : mVertices)
		{
			_out << float(vertex.mPosition[0]) / _xScale;
			_out << float(vertex.mPosition[1]) / _yScale;
			_out << float(vertex.mPosition[2]);
		}

		_out.InsertEndBlock("Convex Collider");
	}

	void Convex::Unserialise(TextSerialiser& _in)
	{
		int arr_vert_size = 0;
		float tmp_x{ 0.0F }, tmp_y{ 0.0F }, tmp_z{ 0.0F };

		mVertices.clear();
		mDebugVertices.clear();

		_in.ConsumeStartBlock();
		_in >> mID;				// gObj ID
		_in >> mv3Offset[0];		// offset for colliders
		_in >> mv3Offset[1];
		_in >> mv3Offset[2];
		_in >> arr_vert_size;

		for (int i = 0; i< arr_vert_size; ++i)
		{
			_in >> tmp_x;
			_in >> tmp_y;
			_in >> tmp_z;
			mVertices.Insert(Vertice{ Math::MakePoint3D(tmp_x,tmp_y,tmp_z) });
		}
		_in.ConsumeEndBlock();

		if (GameObject* owner =
			EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().FindGameObject(mID))
		{
			owner->AddComponent(this, Convex::TAG{});
			Init();
		}
	}

	Convex * Convex::Duplicate() const
	{
		return new Convex{ *this };
	}

	Convex::~Convex()
	{

	}

	bool Convex::isColliding(Convex & _ColB)
	{
		static Math::Vec3D InitialSearchDir{ 1,0,0,0 };
		return isColliding(_ColB, InitialSearchDir);
	}

	bool Convex::isColliding(Convex * const & _pColB)
	{
		return isColliding(*_pColB);
	}


	bool Convex::isColliding(Convex & _pColB, const Math::Vec3D & _v3Dir)
	{
		/*Only need one simplex to check*/
		static AutoArray<Vertice> Simplex{ 3 };
		static Math::Vec3D vDir;
		/*Insert the first Miwoski difference point*/
		vDir = _v3Dir;
		Simplex.Insert(Vertice{ Support(_pColB, vDir) });
		/*Negate the Direction*/
		vDir = -vDir;
		/*Continue to loop until the return statement stops it*/
		while (true)
		{
			/*Add the Second Miwoski difference Point */
			Simplex.Insert(Vertice{ Support(_pColB, vDir) });
			/*If the Second Miwoski difference point does not go pass the origin,
			That means that the Shape of the Miwoski difference does not contain origin*/
			if (Math::Dot(Simplex.back().mPosition, vDir) <= 0)
			{
				/*Clear the simplex for the next function call*/
				Simplex.clear();
				/*Return no collision*/
				return false;
			}
			else
			{
				/*Check if Simplex contains Origin*/
				if (ContainOrigin(Simplex, vDir))
				{
					mbColliding = true;
					_pColB.mbColliding = true;
					/*Use EPA to get collision information*/
					marr_ContactSets.Insert(GetCollisionEvent(Simplex, _pColB));
					/*Clear the simplex for the next function call*/
					Simplex.clear();
					/*Return true for collision*/
					return true;
				}
			}
		}
	}

	bool Convex::isColliding(Circle & _ColB)
	{
		UNUSED_PARAMETER(_ColB);
		return false;
	}

	bool Convex::isColliding(Circle * const & _pColB)
	{
		UNUSED_PARAMETER(_pColB);
		return false;
	}

	bool Convex::isColliding(Circle & _pColB, const Math::Vec3D & _v3Dir)
	{
		UNUSED_PARAMETER(_pColB);
		UNUSED_PARAMETER(_v3Dir);
		return false;
	}

	Vertice Convex::GetFarthestPoint(const Math::Vec3D & _Dir) const
	{
		return Convex::GetFarthestPoint(*this, _Dir);

	}

	/*Support Function for getting the farthest point with relation to a Vector*/
	Vertice Convex::GetFarthestPoint(const Convex & _ColA, const Math::Vec3D & _Dir)
	{
		/*Convert the points to global*/
		/*Offset of the collider from Object Local Coordinate System*/
		Math::Vec3D const & OffSet = _ColA.GetOffSet();

		/*Construct the Matrix for Global Coordinate Conversion*/
		Math::Matrix3D WorldSpace = Math::Translate(_ColA.mPosition.x + OffSet.x, _ColA.mPosition.y + OffSet.y, _ColA.mPosition.z + OffSet.z);

		Vertice * pFirst = _ColA.mVertices.begin();
		Vertice FarthestPoint = *pFirst;
		FarthestPoint.mPosition = (WorldSpace * pFirst->mPosition);
		/*Get the dot product of first Vertice's position for comparision*/
		auto p = WorldSpace * (pFirst->mPosition);
		float FarthestVal = (p).Dot(_Dir);
		/*Loop through the array of Vertices*/
		for (Vertice const & elem : _ColA.mVertices)
		{
			const float val = (WorldSpace * elem.mPosition).Dot(_Dir);
			/*
			If the Dot product is more than the current max
			The current vertice take over as the FarthestVertice
			*/
			if (val > FarthestVal)
			{
				FarthestPoint = elem;
				/*Store the FarthestPoint in terms of global space*/
				FarthestPoint.mPosition = (WorldSpace * elem.mPosition);
				FarthestVal = val;
			}
		}
		return FarthestPoint;
	}

	Edge Convex::GetClosestEdge(AutoArray<Vertice>& _Simplex)
	{
		Edge   ClosestEdge;
		double ClosestDistance = std::numeric_limits<double>::max();

		for (unsigned i = 0; i < _Simplex.size(); ++i)
		{
			unsigned j = (i + 1) >= _Simplex.size() ? 0 : i + 1;

			/*Get the vertice of the _Simplex*/
			Vertice const &  a = _Simplex[i];
			Vertice const &  b = _Simplex[j];
			/*Get the vector of the edge*/
			Math::Vec3D EdgeVec = b.mPosition - a.mPosition;

#if CLOCKWISE
			Math::Vec3D Check = { EdgeVec.y, -EdgeVec.x,EdgeVec.z };
			if (Check.Dot(a.mPosition) < 0)
				EdgeVec = -EdgeVec;
#else
			Math::Vec3D Check = { -EdgeVec.y, EdgeVec.x,EdgeVec.z };
			if (Check.Dot(a.mPosition) < 0)
				EdgeVec = -EdgeVec;
#endif
			Math::Vec3D EdgeNorm;
			EdgeNorm.xyzw = EdgeVec.yxzw;

#if CLOCKWISE
			EdgeNorm.Negate<Math::NegateFlag::Y>();
#else
			EdgeNorm.Negate<Math::NegateFlag::X>();
#endif
			if (EdgeNorm.MagnitudeSqr() > FLT_EPSILON)
			{
				EdgeNorm.Normalise();
			}


			const double distance = EdgeNorm.Dot(a.mPosition);
			if (Math::Abs(distance) < Math::Abs(ClosestDistance))
			{
				ClosestDistance = Math::Abs(distance);
				ClosestEdge.mNorm3 = EdgeNorm;
				ClosestEdge.mVec3 = EdgeVec;
				ClosestEdge.mPos = a.mPosition;
				ClosestEdge.mOrthogonalDistance = distance;
				ClosestEdge.mSimplexIndex = j;
			}
		}
		return ClosestEdge;
	}

	/*Support Function for getting the Minkowski Difference*/
	Math::Point3D Convex::Support(const Convex & _ColA,
		const Convex & _ColB,
		const Math::Vec3D & _Dir)
	{
		const Vertice Farthest_In_ColA = _ColA.GetFarthestPoint(_Dir);
		const Vertice Farthest_In_ColB = _ColB.GetFarthestPoint(_Dir * -1);

		const auto MikwoskiPoint = Farthest_In_ColA.mPosition - Farthest_In_ColB.mPosition;
		return Math::MakePoint3D(MikwoskiPoint.x, MikwoskiPoint.y, MikwoskiPoint.z);
	}

	Math::Point3D Convex::Support(const Convex & _ColB, const Math::Vec3D & _Dir) const
	{
		return Convex::Support(*this, _ColB, _Dir);
	}

	CollisionEvent Convex::GetCollisionEvent(AutoArray<Vertice> _Simplex, const Convex & _ColB)
	{
		RigidBody* other_body {nullptr};
		if (_ColB.GetOwner()->GetComponent<RigidBody>())
			other_body = _ColB.GetOwner()->GetComponent<RigidBody>();

		CollisionEvent col_info(GetOwner(), _ColB.GetOwner());
		Vec3D prevSearchDir{ 0,0,0,0 };
		while (true)
		{
			/*Get the closest edge of our simplex(Made by the minkowski difference to the origin*/
			Edge ClosestEdge = GetClosestEdge(_Simplex);
			Vertice Point{ 0,0 };
			/*Search for a point in the Normal direction of the ClosestEdge*/
			if ((ClosestEdge.mNorm3 - prevSearchDir).MagnitudeSqr() != 0)
			{
				Point = Support(_ColB, ClosestEdge.mNorm3);
				prevSearchDir = ClosestEdge.mNorm3;
			}
			else
			{
				Point = Support(_ColB, -ClosestEdge.mNorm3);
				prevSearchDir = -ClosestEdge.mNorm3;
			}


			/*
			If closest edge is already on the minkowski sum edge,
			The projection distance from the point to the ClosestEdge normal will be
			the same as the orthogonal distance from the origin to the ClosestEdge
			*/
			const double ProjectDis = ClosestEdge.mNorm3.Dot(Point.mPosition);
			const double result = ProjectDis - ClosestEdge.mOrthogonalDistance;

			/*If fail the test, expand the simplex and run the test again*/
			if (Math::Abs(result) <= FLT_EPSILON)
			{
				/*This Position belongs to either ColA or B*/
				col_info.mCollisionPoint = ClosestEdge.mPos;
				col_info.mEdgeVector = ClosestEdge.mVec3;
				col_info.mEdgeNormal = ClosestEdge.mNorm3;
				col_info.mdPeneDepth = ProjectDis;
				if( nullptr != other_body)
				{
					col_info.mfRestitution = DetermineRestitution(*other_body);
					col_info.mfDynamicFrictionCof = DetermineKineticFriction(*other_body);
					col_info.mfStaticFrictionCof = DetermineStaticFriction(*other_body);
				}
				return col_info;
			}
			else
			{
				_Simplex.Insert(Point, ClosestEdge.mSimplexIndex);
			}
		}
		//return col_info;

	}

	bool Convex::ContainOrigin(AutoArray<Vertice> & _Simplex,
		Math::Vec3D & _v3Dir)
	{

		static Math::Point3D vLast;
		static Math::Vec3D v;
		static constexpr unsigned TriangleCase = 3;
		/*Get the last element*/
		vLast = _Simplex.back().mPosition;
		/*v will the opposite vector of vLast [origin - vLast]*/
		v = -vLast;

		switch (_Simplex.size())
		{
			/*Triangle Case: There is enough vertices to form a shape that may
			contain the origin.
			Check if the vector from Last to origin is within
			LastToFirst & LastToSecond
			*/
		case TriangleCase:
		{
			static Math::Vec3D LastToFirst;
			static Math::Vec3D LastToSecond;

			LastToFirst = _Simplex[0].mPosition - vLast;
			LastToSecond = _Simplex[1].mPosition - vLast;

			/*Check against LastToFirst*/

			/*Get the Left Hand Normal of LastToFirst*/
#if CLOCKWISE
			_v3Dir = Math::Vec3D{ LastToFirst.y, -LastToFirst.x,0,0 };
#else
			_v3Dir = Math::Vec3D{ -LastToFirst.y, LastToFirst.x,0,0 };
#endif
			/*Ensure that the normal is pointing away from the inside
			of the triangle. If it is not, inverse it*/
			_v3Dir = Dot(_v3Dir, _Simplex[1].mPosition) > 0 ? -_v3Dir : _v3Dir;

			if (Dot(_v3Dir, v) > 0)
			{
				/*
				Remove the First vertex from the shape so that we can
				find a new one
				*/
				_Simplex.Remove(0ull);
				/*The origin is outside of the shape*/
				return false;

			}

			/*Check against LastToSecond*/

			/*Get the Left Hand Normal of LastToFirst*/
#if CLOCKWISE
			_v3Dir = Math::Vec3D{ LastToSecond.y, -LastToSecond.x,0,0 };
#else
			_v3Dir = Math::Vec3D{ -LastToSecond.y,  LastToSecond.x,0,0 };
#endif
			/*Ensure that the normal is pointing away from the inside
			of the triangle. If it is not, inverse it*/
			_v3Dir = Dot(_v3Dir, _Simplex[0].mPosition) > 0 ? -_v3Dir : _v3Dir;
			if (Dot(_v3Dir, v) > 0)
			{
				/*
				Remove the Second vertex from the shape so that we can
				find a new vertex
				*/
				_Simplex.Remove(1);
				/*The origin is outside of the shape*/
				return false;
			}

			/*If the above 2 rejection test fails, Means that
			origin is within the shape*/
			return true;
		}

		/*Line Case: There is not enough vertices to form a convex shape,
		so we will modify _v3Dir to search the Simplex for a
		vertex in a new direction.
		*/
		default:
		{
			/*Line Case*/
			/*Point of the first vertex of the Simplex*/
			Math::Point3D vFirst;
			/*Vector from last vertex to first Vertex*/

			vFirst = _Simplex.begin()->mPosition;
			/*Get the Left-Hand Normal of First to Last*/
			_v3Dir = Math::Vec3D{ -(vLast - vFirst).y,(vLast - vFirst).x, 0, 0 };

			if (Math::Dot(_v3Dir, v) < 0)
			{
				_v3Dir = -_v3Dir;
			}
		}
		}

		return false;
	}
}
