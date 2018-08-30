#include "Component\Collider.h"
#include <algorithm>
#include <limits>
#include "Math\Vector4.h"

#include "Object\GameObject.h"

namespace Dystopia
{
	Convex::Convex(Math::Point3D const & _v3Offset)
		:Collider{ _v3Offset }
	{
		
	}

	void Convex::Load()
	{

	}

	void Convex::Init()
	{

	}

	void Convex::OnDestroy()
	{

	}

	void Convex::Unload()
	{

	}

	void Convex::Serialise(TextSerialiser&) const
	{
	}

	void Convex::Unserialise(TextSerialiser&)
	{
	}

	Convex * Convex::Duplicate() const
	{
		return new Convex{ *this };
	}

	Convex::~Convex()
	{

	}

	bool Convex::isColliding(const Convex & _ColB) const
	{
		static Math::Vec3D InitialSearchDir{ 1,0,0,0 };
		return isColliding(_ColB, InitialSearchDir);
	}

	bool Convex::isColliding(const Convex * const & _pColB) const
	{
		return isColliding(*_pColB);
	}


	bool Convex::isColliding(const Convex & _pColB, const Math::Vec3D & _v3Dir) const
	{
		/*Only need one simplex to check*/
		static AutoArray<Vertice> Simplex{ 3 };
		static Math::Vec3D vDir;
		/*Insert the first Point of the Miwoski difference*/
		vDir = _v3Dir;
		Simplex.Insert(Vertice{ Support(_pColB, vDir)});
		/*Negate the Direction*/
		vDir = -vDir;
		/*Continue to loop until the return statement stops it*/
		while (true)
		{
			Simplex.Insert(Vertice{ Support(_pColB, vDir) });
			if (Math::Dot(Simplex.back().mPosition, vDir) <= 0)
			{
				return false;
			}
			else
			{
				/*Check if Simplex contains Origin*/
				if (ContainOrigin(Simplex, vDir))
				{
					Simplex.clear();
					return true;
				}
			}
		}
	}

	Vertice Convex::GetFarthestPoint(const Math::Vec3D & _Dir) const
	{
		return Convex::GetFarthestPoint(*this, _Dir);
	}

	/*Support Function for getting the farthest point with relation to a Vector*/
	Vertice Convex::GetFarthestPoint(const Convex & _ColA, const Math::Vec3D & _Dir)
	{
		/*Convert the points to global*/
		Transform & _ColATrans = *(_ColA.GetOwner()->GetComponent<Transform>());
		Math::Vec3D const & OffSet = _ColA.GetOffSet();
		Math::Matrix3D WorldSpace = Math::Translate(_ColATrans.GetPosition().x + OffSet.x, _ColATrans.GetPosition().y + OffSet.y, 0);

		Vertice * pFirst = _ColA.mVertices.begin();
		Vertice FarthestPoint = *pFirst;
		/*Get the dot product of first Vertice's position for comparision*/
		float FarthestVal = pFirst->mPosition.Dot(_Dir);
		/*Loop through the array of Vertices*/
		for (Vertice const & elem : _ColA.mVertices)
		{
			float val = (WorldSpace * elem.mPosition).Dot(_Dir);
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
			unsigned j = (i + 1) == _Simplex.size() ? 0 : i + 1;
			/*Get the vertice of the _Simplex*/
			Vertice & const a = _Simplex[i];
			Vertice & const b = _Simplex[j];
			/*Get the vector of the edge*/
			Math::Vec3D EdgeVec    = b.mPosition - a.mPosition;
			Math::Vec3D EdgeNorm;
			EdgeNorm.xyzw = EdgeVec.yxzw;
#ifdef CLOCKWISE
			EdgeNorm.Negate<Math::NegateFlag::Y>();
#else
			EdgeNorm.Negate<Math::NegateFlag::X>();
#endif
			EdgeNorm.Normalise();
			double distance = EdgeNorm.Dot(a.mPosition);
			if (distance < ClosestDistance)
			{
				ClosestDistance = distance;
				ClosestEdge.mNorm3 = EdgeNorm;
				ClosestEdge.mVec3  = EdgeVec;
				ClosestEdge.mPos   = a.mPosition;
				ClosestEdge.SimplexIndex = i;
			}
		}
		return ClosestEdge;
	}

	/*Support Function for getting the Minkowski Difference*/
	Math::Vec3D Convex::Support(const Convex & _ColA,
		                        const Convex & _ColB,
		                        const Math::Vec3D & _Dir)
	{
		Vertice Farthest_In_ColA = _ColA.GetFarthestPoint(_Dir);
		Vertice Farthest_In_ColB = _ColB.GetFarthestPoint(_Dir * -1);

		return Farthest_In_ColA.mPosition - Farthest_In_ColB.mPosition;
	}
	Math::Vec3D Convex::Support(const Convex & _ColB, const Math::Vec3D & _Dir) const
	{
		return Convex::Support(*this, _ColB, _Dir);
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
		v     = -vLast;

		switch (_Simplex.size())
		{
			/*Triangle Case: There is enough vertices to form a shape that may
			                 contain the origin.
							 Check if the vector from Last to origin is within 
				             LastToFirst & LastToSecond*/
			case TriangleCase:
			{
				static Math::Vec3D LastToFirst;
				static Math::Vec3D LastToSecond;

				LastToFirst  = _Simplex[0].mPosition - vLast;
				LastToSecond = _Simplex[1].mPosition - vLast;
				
				/*Check against LastToFirst*/

				/*Get the Left Hand Normal of LastToFirst*/
				_v3Dir = Math::Vec3D{ -LastToFirst.y, LastToFirst.x,0,0 };

				/*Ensure that the normal is pointing away from the inside
				  of the triangle. If it is not, inverse it*/
				_v3Dir = Dot(_v3Dir, _Simplex[1].mPosition) > 0 ? -_v3Dir : _v3Dir;

				if (Dot(_v3Dir, v) > 0)
				{
					/*
					  Remove the First vertex from the shape so that we can
					  find a new one
					*/
					_Simplex.Remove(static_cast<unsigned>(0));
					/*The origin is outside of the shape*/
					return false;

				}

				/*Check against LastToSecond*/

				/*Get the Left Hand Normal of LastToFirst*/
				_v3Dir = Math::Vec3D{ -LastToSecond.y, LastToSecond.x,0,0 };
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
				static Math::Point3D vFirst;
				/*Vector from last vertex to first Vertex*/
				static Math::Vec3D FirstToLast;
				
				vFirst = _Simplex.begin()->mPosition;
				/*Get the Left-Hand Normal of First to Last*/
				_v3Dir = Math::Vec3D{ -(vLast - vFirst).y,(vLast - vFirst).x, 0, 0};
				
				if (Math::Dot(_v3Dir, v) < 0)
				{
					_v3Dir = -_v3Dir;
				}
			}
		}

		return false;
	}
}
