/* HEADER *********************************************************************************/
/*!
\file	Rigidbody.h
\author Aaron Chu Ming San (100%)
\par    email: m.chu\@digipen.edu
\brief
Rigidbody physics component for 2D sprites.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "Component/Primitive.h"
#include "Component/Component.h"
#include "Component/ComponentList.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"

//#include "Math/Quaternion.h"
//#include "Math/Angles.h"

namespace Dystopia
{
	using Math::Vec3D;
	using Math::Mat3D;
	using Math::Point3D;

	// Forward Declarations
	class Transform;
	class PhysicsSystem;

	// Rigidbody physics component for 2D sprites.
	class _DLL_EXPORT RigidBody : public Component
	{
	public:
		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		static const std::string GetCompileName(void) { return "RigidBody"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 
		// Default Constructor - Quad shape, Mass = 100.0f, Friction & Elasticity = 0.5F
		RigidBody(void);
		RigidBody(float _linearDrag, float _angularDrag,
				  float _friction, float _elasticity,
				  float _gravityScale, float _mass,
				  bool _gravityState, bool _staticState);

		// ================================VIRTUAL MEMBER FUNCTIONS ================================== // 
		void Load(void) override;
		void Init(void) override;
		virtual void OnDestroy(void);
		void Unload(void) override;
		RigidBody* Duplicate() const override;
		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;

	protected:
		enum PhysicsType
		{
			discrete,		// fixed timestep frame checking for collision
			continuous,		// continuous checking for collision
			kinematic		// dynamic objects that will not be pushed around
		};
		//~RigidBody(void);

	public:
		// ===================================== MEMBER FUNCTIONS ==================================== // 
		void Integrate(float _dt);
		void UpdateResult() const;

		// void Update(float _dt);
		// void LateUpdate(float _dt);

		// To print out properties of the Rigidbodies
		void DebugPrint(); 

		//TODO: Draw using renderer (lines)
		void DebugDraw();

		void GlobalCentroidFromPosition(void);
		void PositionFromGlobalCentroid(void);

		/*************************************************************************************************
		\brief
		Add a force at a point. If point is not origin, this will generate a
		Angular velocity and Torgue with respect to GameObject Origin

		Make sure _point is in global coordinates

		**************************************************************************************************/
		void AddForce(Vec3D const & _force, Point3D const & _point);

		/**************************************************************************************************
		\brief
		Add a force at a point. If point is not origin, this will generate a
		Angular velocity and Torgue with respect to _origin

		Make sure _point & _origin is in global coordinates
		**************************************************************************************************/
		void AddForce(Vec3D   const & _force,
			          Point3D const & _point, 
			          Point3D const & _origin);

		/*Add a force at the origin of the body*/
		void AddForce(Vec3D const & _force);

		/**************************************************************************************************
		\brief
		Resets the Cumulative Force for the current RigidBody
		**************************************************************************************************/
		void ResetCumulative();

		// Settors
		void SetGravityScale(float);
		bool Set_ToggleGravity(); // toggles 
		void Set_ToggleGravity(bool);
		void Set_IsStatic(bool);
		void SetPosition(const Vec3D&);
		void SetVelocity(const Vec3D&);
		void SetMass(const float);

		// Gettors
		Vec3D GetPosition() const;
		Vec3D GetPrevPosition() const;
		Vec3D GetLinearVelocity() const;
		Vec3D GetAngularVelocity() const;
		Vec3D GetAcceleration() const;
		Transform* GetOwnerTransform() const;
		float GetAngle() const;
		float GetFrictionForce() const;
		float GetGravityScalar() const;
		float GetMass() const;
		float GetInverseMass() const;
		bool Get_HasGravityState() const;
		bool Get_IsStaticState() const;

	private:
		Primitive*		mpPrimitiveShape;			/* The underlying primitive of the RigidBody*/
		Transform*		mpOwnerTransform;			/* Used for accessing position and GameObject World orientation*/
		PhysicsSystem*	mpPhysSys;					/* A pointer to Physics System to get global constants (i.e. Gravity)*/

		Mat3D			mOrientation;				/* The orientation in local transformation space*/
		Vec3D			mPosition;					/* For Calculation purposes, set by Gameobject's Transform.GetGlobalPosition()*/
		Vec3D			mPrevPosition;				/* For Collision purposes - the position in the last frame*/

		Vec3D			mLinearVelocity;			/* Linear Velocity of the RigidBody with respect to World Space*/
		Vec3D			mAngularVelocity;			/* Angular Velocity/Rotation with respect to World Space*/
		Vec3D			mAcceleration;				/* For integration purpose when updating position.*/

		Vec3D			mCumulativeForce;			/* The sum of all the force acting on the body*/
		Vec3D			mCumulativeTorque;			/* Sum of all angular forces acting on the body*/

		Mat3D			mLocalInvInertiaTensor;
		Mat3D			mGlobalInvInertiaTensor;
		
		Vec3D			mGlobalCentroid;
		Vec3D			mLocalCentroid;
			
		float			mfAngleDeg;					/* Anticlockwise Direction: Angles in Degrees*/
		float			mfLinearDamping;			/* Linear Drag, slows down motion dynamics over time*/

		float			mfAngularDrag;				/* Coefficient of angular drag. */
		float			mfFriction;					/* Like LinearDamping, but only drag the forces when 2 or more objects are colliding*/
		float			mfRestitution;				/* Elasticity Coefficient: 'Bounciness' of the body*/

		float			mfCustom_GravityScale;		/* A scalar multiple (in float) of the Physics Engine's gravity*/
	
		float			mfMass;						/* Defines how difficult it is to move an object*/
		float			mfInvMass;					/* For calculation purposes, since integration uses mass inverse*/

		bool			mOwnerIsActive;				/* Check if owner is active*/
		bool			mbHasGravity;				/* If it has no gravity, do not provide acceleration due to gravity*/
		bool			mbIsStatic;					/* Static bodies do not need to be integrated/updated*/
		PhysicsType		mPhysicsType;


		/*Quaternion if needed*/

	};
}
#endif
