/* HEADER *********************************************************************************/
/*!
\file	Collider.h
\author Keith (70%)
		Aaron (30%)
\par    email: keith.goh\@digipen.edu
		email: m.chu\@digipen.edu
\brief
Collider2D for 2D Sprites.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
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
#include "System/Collision/CollisionEvent.h"

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
		BASE = 0,
		AABB,
		CIRCLE,			/*Status : Not Done*/
		TRIANGLE,       /*Status : Not Done*/
		CONVEX,
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

	struct _DLL_EXPORT Edge
	{
		Math::Point3D  mPos;
		Math::Vec3D    mVec3;
		Math::Vec3D    mNorm3;
		double         mOrthogonalDistance;
		int            mSimplexIndex;
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
		explicit Collider(const Math::Point3D & _offset);
		
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

		float DetermineRestitution(RigidBody const & b) const;

		float DetermineFriction(RigidBody const & b) const;

		 /*Get Array of collision event*/
		AutoArray<CollisionEvent> const & GetCollisionEvents() const;

		// Settors
		void SetColliding(bool _b);
		void SetPosition(Math::Point3D const & _point);
		void ClearCollisionEvent();

		// Gettors
		Math::Point3D GetPosition() const;
		Math::Vec3D GetOffSet()   const;
		bool hasCollision() const;

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

		bool mbColliding = false;

		Math::Point3D mPosition;
		
		AutoArray<Vertex> mDebugVertices;
		AutoArray<short>  mIndexBuffer;

		void Triangulate();


	private:
		 //Status mStatus;

		/*Offset of the collider with respect to GameObject Transform position*/
		Math::Vec3D mv3Offset;

		// Collider Mesh for debug drawing
		Mesh * mpMesh;


	};
}
#endif /*COLLIDER_H*/
