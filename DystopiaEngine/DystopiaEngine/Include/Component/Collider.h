#ifndef COLLIDER_H
#define COLLIDER_H

#include "Component\Component.h"		       /*Component Base Class*/
#include "Component\ComponentList.h"           /*Component List*/
#include "Math\Vector4.h"                      /*Vector*/
#include "DataStructure\AutoArray.h"	       /*AutoArray Data Structure*/
#include "System\Collision\CollisionSystem.h"            /*Collision System*/
#include "Utility\MetaAlgorithms.h"		// MetaFind

#define CLOCKWISE 1
/*
                                 -----------AABB
 Collider[BASE]<--- CONVEX <-----|
                                 -----------TRIANGLE

	 There is 3 Coordinate system at play here.
	 Global Coordinate System
	 - The Global Position of the object
	 Local Object Position
	 - The Local Coordinate System of the Object
	 Collider Coordinate System
	 - The Local Coordinate System of the Collider

	 EG. Object Global Coordinates is (10 , 10)
	 Offset is (4,4)
	 One of the Collider Vertice Position is (1,2)

	 The Global Position of the Collider Vertice is
	 (10,10) + (4,4) + (1,2) = (15,16)

	 The Global Position of the Collider is
	 (10,10) + (4,4)         = (14,14)

*/
namespace Dystopia
{
	typedef char Status;

	enum class eColliderType
	{
		BASE,
		AABB,           /*Status : Not Done*/
		TRIANGLE,       /*Status : Not Done*/
		CONVEX,         /*Status : Not Done*/

		CONCAVE,		/*This is a dream*/

		TOTAL,
	};



	struct _DLL_EXPORT Vertice
	{
		/*Position of the vectice*/
		Math::Point3D mPosition;
		Vertice(Math::Point3D const & p)
			:mPosition{p}
		{

		}
		Vertice(float const & x, float const & y)
			:mPosition{x,y,1,0}
		{

		}
	};

	struct Edge
	{
		Math::Point3D mPos;
		Math::Vec3D   mVec3;
		Math::Vec3D   mNorm3;
		int SimplexIndex;
	};

	class _DLL_EXPORT Collider : public Dystopia::Component
	{
	public:

		unsigned GetComponentType(void) const override
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllComponents>::value;
		};

		static const eColliderType ColliderType = eColliderType::BASE;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; } // returns const
		/*Constructors*/

		/*Default - (Box Collider)*/
		Collider();
		Collider(const Math::Point3D & _offset);
		
		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual Collider* Duplicate() const;

		Math::Vec3D GetOffSet() const;
		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		virtual ~Collider();

	private:
		 //Status mStatus;

		/*Offset of the collider with respect to GameObject Transform position*/
		Math::Vec3D mv3Offset;
	};

	class _DLL_EXPORT Convex : public virtual Collider
	{
	public:

		static const eColliderType ColliderType = eColliderType::CONVEX;
		virtual const eColliderType GetColliderType(void) const override { return ColliderType; }
		
		/*Constructors*/

		/*Convex Default Constructor*/
		Convex(Math::Point3D const & _v3Offset = { 0,0,0,0 });

		/*Convex Constructor. Takes in a Container of Vertice*/
		template<typename Container = AutoArray<Vertice>>
		Convex(Container & _Container)
		{
			for (Vertice const & elem : _Container)
				mVertices.push_back(elem);
		}

		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual Convex* Duplicate() const;

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		virtual ~Convex();

		Vertice GetFarthestPoint(const Math::Vec3D & _Dir)const;

		bool isColliding(const Convex & _ColB) const;
		bool isColliding(const Convex * const & _pColB) const;
		bool isColliding(const Convex & _pColB, const Math::Vec3D & _v3Dir) const;

		/*Static Member Functions*/

		/*Support Function for getting the farthest point with relation to a Vector*/
		static Vertice GetFarthestPoint(const Convex & _ColA, const Math::Vec3D & _Dir);
		static Edge	   GetClosestEdge(AutoArray<Vertice> & _Simplex);

		static Math::Vec3D Support(const Convex & _ColA,
			                       const Convex & _ColB,
			                       const Math::Vec3D & _Dir);

		Math::Vec3D Support(const Convex & _ColB,
			                const Math::Vec3D & _Dir)const;
		
		static bool ContainOrigin(AutoArray<Vertice> & _Simplex, Math::Vec3D & _v3Dir);

	protected:
		/*The vertices of the collider in the Collider Local Coordinate System*/
		AutoArray<Vertice> mVertices;
	};


	class _DLL_EXPORT AABB : public Convex
	{
	public:

		static const eColliderType ColliderType = eColliderType::AABB;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
		/*Constructors*/

		/*Default - (Box Collider)*/
		AABB();
		/*Constructor*/
		AABB(float const & _width, float const & _height, Math::Vec3D const & _v3Offset = { 0,0,0,0 });

		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual AABB* Duplicate() const;

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		/*Collision Check Functions*/
		bool isColliding(const AABB & other_col) const;
		bool isColliding(const AABB * const & other_col) const;

	private:
		float mfWidth;
		float mfHeight;

		Vertice * mMin;
		Vertice * mMax;
		
	};


	class Triangle : public Convex
	{
	public:

		static const eColliderType ColliderType = eColliderType::TRIANGLE;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
		/*Constructors*/

		/*Default - (Box Collider)*/
		Triangle();
		/**/

		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual Triangle* Duplicate() const;

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);
	private:
	};
}
#endif /*COLLIDER_H*/