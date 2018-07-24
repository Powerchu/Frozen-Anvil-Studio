#pragma once
#ifndef COLLIDER_H
#define COLLIDER_H

#include "Component\Component.h"		       /*Component Base Class*/
#include "Component\ComponentList.h"           /*Component List*/
#include "Math\Vector4.h"                      /*Vector*/
#include "DataStructure\AutoArray.h"	       /*AutoArray Data Structure*/
#include "System/Collider/CollisionSystem.h"   /*Collision System*/

#define CLOCKWISE 1
/*
                                 -----------AABB
 Collider[BASE]<--- CONVEX <-----|
                                 -----------TRIANGLE
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



	struct Vertice
	{
		/*Position of the vectice*/
		Math::Point3D mPosition;
		Vertice(Math::Point3D const & _p)
			:mPosition{_p}
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

	class Collider : public Dystopia::Component
	{
	public:

		static const eComponents TYPE = eComponents::COLLIDER;
		const eComponents GetComponentType(void) const { return TYPE; };

		static const eColliderType ColliderType = eColliderType::BASE;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
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
		virtual void Serialise(TextSerialiser&);
		virtual void Unserialise(TextSerialiser&);

		virtual ~Collider();

	private:
		 //Status mStatus;

		/*Offset of the collider with respect to GameObject Transform position*/
		Math::Vec3D mv3Offset;
	};

	class Convex : public virtual Collider
	{
	public:
		static const eComponents TYPE = eComponents::COLLIDER;
		const eComponents GetComponentType(void) const { return TYPE; };

		static const eColliderType ColliderType = eColliderType::CONVEX;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
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
		virtual void Serialise(TextSerialiser&);

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
		AutoArray<Vertice> mVertices;
	};


	class AABB : public Convex
	{
	public:
		static const eComponents TYPE = eComponents::COLLIDER;
		const eComponents GetComponentType(void) const { return TYPE; };

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
		virtual void Serialise(TextSerialiser&);
		virtual void Unserialise(TextSerialiser&);

		/*Collision Check Functions*/
		bool isColliding(const AABB & _ColB);
		bool isColliding(const AABB * const & _ColB);

	private:
		float mfWidth;
		float mfHeight;

		Vertice * mMin;
		Vertice * mMax;
		
	};

	class Triangle : public Convex
	{
	public:
		static const eComponents TYPE = eComponents::COLLIDER;
		const eComponents GetComponentType(void) const { return TYPE; };

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
		virtual void Serialise(TextSerialiser&);
		virtual void Unserialise(TextSerialiser&);
	private:
	};
}
#endif /*COLLIDER_H*/