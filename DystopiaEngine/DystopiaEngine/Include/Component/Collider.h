/* HEADER *********************************************************************************/
/*!
\file	Collider.h
\author Keith (70%)
		Aaron (30%)
\par    email: keith.goh\@digipen.edu
		email: m.chu\@digipen.edu
\brief
Collider2D for 2D Sprites.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef COLLIDER_H
#define COLLIDER_H

#include "Component/Component.h"		       /*Component Base Class*/
#include "Component/ComponentList.h"           /*Component List*/
#include "Math/Vector4.h"                      /*Vector*/
#include "DataStructure/AutoArray.h"	       /*AutoArray Data Structure*/
#include "Utility/MetaAlgorithms.h"		       // MetaFind
#include "Component/CollisionEvent.h"

#include "System/Graphics/VertexDefs.h"



#define CLOCKWISE 0
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

	struct CollisionEvent;

	class CollisionSystem;

	class Mesh;

	enum class eColliderType
	{
		BASE,
		AABB,
		CIRCLE,			/*Status : Not Done*/
		TRIANGLE,       /*Status : Not Done*/
		CONVEX,         /*Status : Not Done*/

		CONCAVE,		/*This is a dream*/

		TOTAL
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
			:mPosition{x,y,1,1}
		{

		}
	};

	struct Edge
	{
		Math::Point3D  mPos;
		Math::Vec3D    mVec3;
		Math::Vec3D    mNorm3;
		double         OrthogonalDistance;
		int            SimplexIndex;
	};

	class _DLL_EXPORT Collider : public Component
	{
	public:

		using SYSTEM = CollisionSystem;

		
		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		

#if EDITOR
		static const std::string GetCompileName(void) { return "Collider"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
#endif

		static  const eColliderType ColliderType = eColliderType::BASE;
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
		

		/************************************************************************
		 * Member Functions
		 ***********************************************************************/
		 /*Get Array of collision event*/
		AutoArray<CollisionEvent> const & GetCollisionEvents() const;

		void ClearCollisionEvent();

		bool hasCollision() const;
		void SetColliding(bool _b);

		void SetPosition(Math::Point3D const & _point);
		Math::Point3D GetPosition() const;

		// Gettors
		Math::Vec3D GetOffSet()   const;

		AutoArray<Vertex> GetVertexBuffer() const;
		AutoArray<short>  GetIndexBuffer()  const;

		void  SetMesh(Mesh * _ptr);
		Mesh* GetMesh() const;

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		virtual ~Collider();

	protected:

		/*AutoArray of collision event*/
		AutoArray<CollisionEvent>  mCollisionEvent;

		bool Colliding = false;

		Math::Point3D mPosition;
		
		AutoArray<Vertex> mDebugVertices;
		AutoArray<short>  mIndexBuffer;

		void Triangulate();


	private:

		 //Status mStatus;

		/*Offset of the collider with respect to GameObject Transform position*/
		Math::Vec3D mv3Offset;

		Mesh * mpMesh;


	};

	class _DLL_EXPORT Convex : public Collider
	{
	public:

		using SYSTEM = CollisionSystem;

		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<Collider>, AllComponents>::value;
		};

		static const std::string GetCompileName(void) { return "Convex"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }

		static const eColliderType ColliderType = eColliderType::CONVEX;
		virtual const eColliderType GetColliderType(void) const override { return ColliderType; }
		
		/*Constructors*/

		/*Convex Default Constructor*/
		Convex(Math::Point3D const & _v3Offset = { 0,0,0,1 });

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

		bool isColliding(Convex & _ColB);
		bool isColliding(Convex * const & _pColB);
		bool isColliding(Convex & _pColB, const Math::Vec3D & _v3Dir);

		/*Static Member Functions*/

		/*Support Function for getting the farthest point with relation to a Vector*/
		static Vertice GetFarthestPoint(const Convex & _ColA, const Math::Vec3D & _Dir);

		static Edge	   GetClosestEdge(AutoArray<Vertice> & _Simplex);

		static Math::Point3D Support(const Convex & _ColA,
			                       const Convex & _ColB,
			                       const Math::Vec3D & _Dir);

		static Math::Point3D Support(const Convex & _ColA,
			                       const Convex & _ColB,
			                       const Math::Vec3D & _Dir,
			                       bool & hasPoint);

		static bool ContainOrigin(AutoArray<Vertice> & _Simplex, Math::Vec3D & _v3Dir);

		Math::Point3D Support(const Convex & _ColB,
			                  const Math::Vec3D & _Dir)const;
		


	protected:

		CollisionEvent GetCollisionEvent(AutoArray<Vertice> & _Simplex,
			                             const Convex & _ColB);

		/*The vertices of the collider in the Collider Local Coordinate System*/
		AutoArray<Vertice>         mVertices;
	};


	class _DLL_EXPORT AABB : public Convex
	{
	public:

		using SYSTEM = Collider::SYSTEM;

		static const eColliderType ColliderType = eColliderType::AABB;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
		static const std::string GetCompileName(void) { return "AABB"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
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
		bool isColliding(AABB & other_col);
		bool isColliding(AABB * const & other_col);

		/* Gettors */
		float GetWidth() const;
		float GetHeight() const;
		float GetHalfWidth() const;
		float GetHalfHeight() const;

	private:
		float mfWidth;
		float mfHeight;

		Vertice * mMin;
		Vertice * mMax;
	};

	class _DLL_EXPORT Circle : public Collider
	{
	public:

		using SYSTEM = Collider::SYSTEM;

		static const eColliderType ColliderType = eColliderType::CIRCLE;
		virtual const eColliderType GetColliderType(void) const { return ColliderType; }
		static const std::string GetCompileName(void) { return "Circle"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
		/*Constructors*/

		/*Default - (Box Collider)*/
		Circle();
		/*Constructor*/
		Circle(float const & _radius, Math::Vec3D const & _v3Offset = { 0,0,0,0 });

		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);
		/*OnDestroy*/
		virtual void OnDestroy(void);
		/*Unload the Component*/
		virtual void Unload(void);
		/*Duplicate the Component*/
		virtual Circle* Duplicate() const;

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);

		/*Collision Check Functions*/
		bool isColliding(const Circle & other_col) const;
		bool isColliding(const Circle * const & other_col) const;

	private:
		float m_radius;
		Math::Vec3D m_originCentre; // GLOBAL COORDINATES

	};


	class Triangle : public Convex
	{
	public:

		using SYSTEM = Collider::SYSTEM;

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