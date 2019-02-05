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
#include "Math/Vectors.h"                      /*Vector*/
#include "Math/Matrix4.h"
#include "DataStructure/AutoArray.h"	       /*AutoArray Data Structure*/
#include "Utility/MetaAlgorithms.h"		       // MetaFind
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/BroadPhaseCircle.h"
#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/Shader.h"
#include "Math/Quaternion.h"


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
		LINE,
		AABB,
		CIRCLE,			/*Status : Not Done*/
		TRIANGLE,       /*Status : Not Done*/
		CONVEX,
		CONCAVE,		/*This is a dream*/
		POINT,
		RAY,

		TOTAL
	};

	enum eColLayer : unsigned
	{
		LAYER_NONE   = 0x00u,
		LAYER_1      = 0x01u << 0,
		LAYER_2      = 0x01u << 1,
		LAYER_3      = 0x01u << 2,
		LAYER_4      = 0x01u << 3,
		LAYER_5      = 0x01u << 4,
		LAYER_6      = 0x01u << 5,
		LAYER_7      = 0x01u << 6,
		LAYER_8      = 0x01u << 7,
		LAYER_9      = 0x01u << 8,
		LAYER_10     = 0x01u << 9,
		LAYER_11     = 0x01u << 10,
		LAYER_12     = 0x01u << 11,
		LAYER_13     = 0x01u << 12,
		LAYER_14     = 0x01u << 13,
		LAYER_15     = 0x01u << 14,
		LAYER_16     = 0x01u << 15,
		LAYER_17     = 0x01u << 16,
		LAYER_18     = 0x01u << 17,
		LAYER_19     = 0x01u << 18,
		LAYER_20     = 0x01u << 19,
		LAYER_21     = 0x01u << 20,
		LAYER_22     = 0x01u << 21,
		LAYER_23     = 0x01u << 22,
		LAYER_24     = 0x01u << 23,
		LAYER_25     = 0x01u << 24,
		LAYER_26     = 0x01u << 25,
		LAYER_27     = 0x01u << 26,
		LAYER_28     = 0x01u << 27,
		LAYER_29     = 0x01u << 28,
		LAYER_30     = 0x01u << 29,
		LAYER_31     = 0x01u << 30,
		LAYER_32     = 0x01u << 31,
		LAYER_GLOBAL = 0xFFFFFFFFu,
	};

	struct _DLL_EXPORT Vertice
	{
		/*Position of the vectice*/
		Math::Point3D mPosition;

		Vertice(Math::Point3D const & p)
			:mPosition{ p }
		{

		}
		Vertice(float const & x, float const & y)
			:mPosition{ x,y,0,1 }
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
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
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
		explicit Collider(const Math::Point3D & _offset, const Math::Point3D & _origin = Math::MakePoint3D(0.f, 0.f, 0.f));

		void Awake(void);
		/*Load the Component*/
		virtual void Load(void);
		/*Initialise the Component*/
		virtual void Init(void);

		virtual void Update(float) = 0;
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
		float DetermineStaticFriction(RigidBody const & b) const;
		float DetermineKineticFriction(RigidBody const & b) const;

		/*Get Array of collision event*/
		AutoArray<CollisionEvent> const & GetCollisionEvents() const;
		CollisionEvent *  FindCollisionEvent(unsigned long long _ID) const;

		// Settors
		void SetColliding(bool _b);
		void SetSleeping(bool _b);
		void SetTrigger(bool _b);
		void SetLocalPosition(Math::Point3D const & _point);
		void SetRotation(Math::Quaternion const & _rot);
		void SetColLayer(eColLayer _Layer);
		void ResetColLayer();
		void RemoveColLayer(eColLayer _Layer);
		void ClearCollisionEvent();
		void ClearCurrentCollisionEvent();
		void RemoveCollisionEvent(unsigned long long _OtherID);
		void InformOtherComponents(bool _isColliding, CollisionEvent const & _Event);
		void InformOtherComponents();

		// Gettors
		virtual Math::Point3D GetGlobalPosition() const;
		virtual Math::Quaternion GetGlobalRotation() const;
		Math::Vec3D           GetOffSet()   const;
		void				  SetOffSet(Math::Vec3D _offset);
		Math::Vec3D			  GetScale() const;
		void				  SetScale(Math::Vec3D _scale);
		bool                  HasCollision() const;
		bool				  IsTrigger() const;
		bool				  IsSleeping() const;
		bool				  HasCollisionWith(unsigned long long _ID) const;
		bool				  HasCollisionWith(GameObject const * const _pointer) const;
		eColLayer             GetColLayer() const;

		BroadPhaseCircle      GetBroadPhaseCircle() const;

		AutoArray<Gfx::Vertex> GetVertexBuffer() const;
		AutoArray<short>  GetIndexBuffer()  const;
		AutoArray<GameObject*> GetCollidingObjects() const;

		void  SetMesh(Mesh * _ptr);
		Mesh* GetMesh() const;


		virtual Math::Matrix3D GetTransformationMatrix() const;
		Math::Matrix3D GetOwnerTransform()       const;
		Math::Matrix3D GetWorldMatrix()          const;
		void SetOwnerTransform(Math::Matrix3D const & _ownerMatrix);

		/*Serialise and Unserialise*/
		virtual void Serialise(TextSerialiser&) const = 0;
		virtual void Unserialise(TextSerialiser&) = 0;

		virtual BroadPhaseCircle GenerateBoardPhaseCircle() const;
		virtual ~Collider();

		bool     mbColliding;
		bool     mbIsTrigger;
		bool     mbIsSleeping;
		/*Offset of the collider with respect to GameObject Transform position*/
		Math::Vec3D mv3Offset;
		Math::Vec3D mScale;
#if EDITOR

#endif

	protected:

		// Collider Mesh for debug drawing
		Mesh *   mpMesh;
		/*Collision Layer*/
		eColLayer mColLayer;
		/*AutoArray of collision event*/
		AutoArray<CollisionEvent>  marr_ContactSets;
		AutoArray<CollisionEvent>  marr_CurrentContactSets;
		Math::Point3D mPosition;

		AutoArray<Gfx::Vertex> mDebugVertices;
		AutoArray<short>  mIndexBuffer;
		BroadPhaseCircle  mBoundingCircle;
		void Triangulate();
#if EDITOR
		virtual void EditorUI(void) noexcept;
		virtual void AddLayerUI(void) noexcept;
#endif
		
		/*Matrix*/
		Math::Matrix3D		mOwnerTransformation;
		Math::Quaternion    mRotation;
#if EDITOR
		static std::string arrColLayer[33];
#endif

	private:
		int indexDropDown = 0;


	};
}
#endif /*COLLIDER_H*/
