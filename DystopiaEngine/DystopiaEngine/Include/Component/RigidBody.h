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

#include "Component/Component.h"
#include "Component/ComponentList.h"
#include "Math/Vectors.h"
#include "Math/Matrix4.h"
#include "DataStructure/AutoArray.h"

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
	class Collider;

	// Rigidbody physics component for 2D sprites.
	class _DLL_EXPORT RigidBody : public Component
	{
	public:
		using SYSTEM = PhysicsSystem;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
		};

		static const std::string GetCompileName(void) { return "RigidBody 2D"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }


		// ====================================== CONSTRUCTORS ======================================= // 
		// Default Constructor - Quad shape, Mass = 100.0f, Friction & Elasticity = 0.5F
		RigidBody(void);
		RigidBody(float _linearDrag, float _angularDrag,
				  float _friction, float _elasticity,
				  float _gravityScale, float _mass,
				  bool _gravityState, bool _staticState);

		// ================================VIRTUAL MEMBER FUNCTIONS ================================== // 
		//void Load(void);
		void Awake(void) override;
		void Init(void) override;
		// Unload(void);
		RigidBody* Duplicate() const override;
		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;

	protected:
		enum PhysicsType
		{
			eDynamicBody = 0,		// fixed timestep frame checking for collision
			eKinematicBody,			// dynamic objects that will not be pushed around
			eStaticBody				// continuous checking for collision
		};
		//~RigidBody(void);

	public:
		// ===================================== MEMBER FUNCTIONS ==================================== // 
		void Integrate(float _dt);
		void CheckSleeping(float _dt);
		void PreUpdatePosition(float _dt);
		void UpdateResult(float _dt) const;

		// void Update(float _dt);
		// void LateUpdate(float _dt);

		// To print out properties of the Rigidbodies
		void DebugPrint();

		// Editor UI
#if EDITOR
		void EditorUI(void) noexcept override;
#endif
		void GlobalCentroidFromPosition(void);
		void PositionFromGlobalCentroid(void);

		Vec3D LocalToGlobal(const Vec3D & p) const;
		Vec3D GlobalToLocal(const Vec3D & p) const;
		Vec3D LocalToGlobalVec(const Vec3D & v) const;
		Vec3D GlobalToLocalVec(const Vec3D & v) const;

		/*************************************************************************************************
		\brief
		Add a force at a point. If point is not origin, this will generate a
		Angular velocity and Torgue with respect to GameObject Origin

		Make sure _point is in global coordinates

		**************************************************************************************************/
		void AddForceWithOrigin(Vec3D const & _force, Point3D const & _point);

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

		/* Add torque based on COM*/
		void AddTorque(Math::Vec3D const& _torque);


		// ADD IMPULSE
		void AddLinearImpulse(Vec3D const & _impul);
		void AddLinearImpulseWithOrigin(Vec3D const & _impul, Point3D const & _point);
		void AddLinearImpulse(Vec3D const & _impul, Point3D const & _point, Point3D const & _origin);

		// Angular Impulse
		void ApplyAngularImpulse(Vec3D const & _impul);

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
		void SetLinearVel(const Vec3D&);
		void SetAngularVel(const Vec3D&);
		void SetMass(const float);
		void SetStaticFriction(const float);
		void SetKineticFriction(const float);
		void SetRestitution(const float);
		void SetSleeping(const bool);
		void SetFixedRotation(bool flag);

		// Gettors
		Vec3D GetPosition() const;
		Vec3D GetPrevPosition() const;
		Vec3D GetLinearVelocity() const;
		Vec3D GetAngularVelocity() const;
		Vec3D GetAcceleration() const;
		Transform* GetOwnerTransform() const;
		float GetAngle() const; //degrees
		float GetStaticFriction() const;
		float GetKineticFriction() const;
		float GetRestitution() const;
		float GetGravityScalar() const;
		float GetMass() const;
		float GetInverseMass() const;
		float GetInertia() const;
		float GetInverseInertia() const;
		bool  Get_HasGravityState() const;
		bool  Get_IsStaticState() const;
		bool  Get_IsKinematic() const;
		bool GetIsAwake() const;

		// Does this body have fixed rotation?
		bool Get_IsFixedRotation() const;
		/// Get the world position of the center of mass.
		const Vec3D& GetWorldCenter() const;
		/// Get the local position of the center of mass.
		const Vec3D& GetLocalCenter() const;

		AutoArray<Collider*>    mparrCol;
	private:
		Transform*				mpOwnerTransform;			/* Used for accessing position and GameObject World orientation*/
		PhysicsSystem*			mpPhysSys;					/* A pointer to Physics System to get global constants (i.e. Gravity)*/

		Mat3D					mOrientation;				/* The orientation in local transformation space*/
		Mat3D					mInverseOrientation;
		Vec3D					mPosition;					/* For Calculation purposes, set by Gameobject's Transform.GetGlobalPosition()*/
		Vec3D					mPrevPosition;				/* For Collision purposes - the position in the last frame*/

		Vec3D					mLinearVelocity;			/* Linear Velocity of the RigidBody with respect to World Space*/
		Vec3D					mAngularVelocity;			/* Angular Velocity/Rotation with respect to World Space*/
		Vec3D					mLinearAcceleration;		/* For integration purpose when updating position.*/
		Vec3D					mAngularAcceleration;		/* For integration purpose when updating position.*/

		Vec3D					mCumulativeForce;			/* The sum of all the force acting on the body*/
		Vec3D					mCumulativeTorque;			/* Sum of all angular forces acting on the body*/

		Mat3D					mLocalInvInertiaTensor;
		Mat3D					mGlobalInvInertiaTensor;

		Vec3D					mGlobalCentroid;
		Vec3D					mLocalCentroid;

		float					mfZAngleDeg;				/* Anticlockwise Direction: Angles in Degrees*/

		Math::Vector2			mLinearDamping;				/* Linear Damping in the X and Y axis */
		float					mfAngularDrag;				/* Coefficient of angular drag. */
		float					mfStaticFriction;			/* Like LinearDamping, but only drag the forces when 2 or more objects are colliding*/
		float					mfDynamicFriction;
		float					mfRestitution;				/* Elasticity Coefficient: 'Bounciness' of the body*/

		float					mfGravityScale;				/* A scalar multiple (in float) of the Physics Engine's gravity*/

		float					mfMass;						/* Defines how difficult it is to move an object*/
		float					mfInvMass;					/* For calculation purposes, since integration uses mass inverse*/

		// Rotational inertia about the center of mass.
		float					mfInertia;
		float					mfInvInertia;

		float					mfWeightedMotion;

		bool					mbHasGravity;				/* If it has no gravity, do not provide acceleration due to gravity*/
		bool					mbIsStatic;					/* Static bodies do not need to be integrated/updated*/

		bool					mbIsAwake;
		bool					mbCanSleep;
		bool					mbFixedRot;

		int						EditorCanSleepInt = 1;		/*Start Awake*/

		float					mSleepTime;
		PhysicsType				mPhysicsType;

		size_t					mContactsNo = 0;


		//// m_flags
		//enum
		//{
		//	eIslandFlag = 0x0001,
		//	eAwakeFlag = 0x0002,
		//	eAutoSleepFlag = 0x0004,
		//	eBulletFlag = 0x0008,
		//	eFixedRotationFlag = 0x0010,
		//	eActiveFlag = 0x0020,
		//};

		/*=================Editor Stuff=====================*/
#if EDITOR
		void eBodyTypeDropDown();
		void CheckMass();
		void eMassField();
		void eLinearDragField();
		void eAngularDragField();
		void eGravityScaleField();
		void eStaticFrictionDragField();
		void eDynamicFrictionDragField();
		void eRestitutionDragField();

		void eCollisionDetectionField();
		void eSleepingModeDropdown();
		void eRotGravConstraintCheckBox();

		void eExtraInfoHeader();

#endif // EDITOR

	};
}
#endif
