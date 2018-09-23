#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "Component/Primitive.h"
#include "Component/Component.h"
#include "Component/ComponentList.h"
#include "Math/Vector4.h"

//#include "Math/Quaternion.h"
//#include "Math/Angles.h"

namespace Dystopia
{
	using Math::Vec3D;
	using Math::Point3D;

	class Transform;
	class PhysicsSystem;

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
		RigidBody(void);
		virtual ~RigidBody(void);

		// ================================VIRTUAL MEMBER FUNCTIONS ================================== // 
		void Load(void) override;
		void Init(void) override;
		virtual void OnDestroy(void);
		void Unload(void) override;
		RigidBody* Duplicate() const override;
		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;

		// ===================================== MEMBER FUNCTIONS ==================================== // 
		void Integrate(float _dt);
		void PostResult() const;

		// void Update(float _dt);
		// void LateUpdate(float _dt);

		//TODO: Delete this once graphics is up
		void DebugPrint(); // FOR TESTING

		//TODO: Draw using renderer (lines)
		void DebugDraw();

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
		void Set_CustomGravityScale(float);
		bool Set_ToggleGravity(); // toggles 
		void Set_ToggleGravity(bool);
		void Set_IsStatic(bool);
		void SetPosition(const Vec3D&);
		void SetVelocity(const Vec3D&);

		// Gettors
		Vec3D GetPosition();
		Vec3D GetPrevPosition();

		Vec3D GetLinearVelocity();
		Vec3D GetAngularVelocity();
		Vec3D GetAcceleration();
		
		float GetAngle();
		float GetFrictionForce();

		float GetGravityScalar();
		float GetMass();
		float GetInverseMass();

		bool Get_IsGroundedState();
		bool Get_HasGravityState();
		bool Get_IsStaticState();

	private:
		Primitive*			mpPrimitiveShape;    /*The underlying primitive of the RigidBody*/
		Transform*			mpOwnerTransform;    /*Used for accessing position and GameObject World orientation*/
		PhysicsSystem*		mpPhysSys;

		bool				mOwnerIsActive;	 /* check if owner is active*/

		Vec3D				mPosition;
		Vec3D				mPrevPosition;

		Vec3D				mLinearVelocity;    /*Linear Velocity of the RigidBody with respect to World Space*/
		Vec3D				mAngularVelocity;   /*Angular Velocity/Rotation with respect to World Space*/
		Vec3D				mCumulativeForce;  /*The sum of all the force acting on the body*/
		Vec3D				mAcceleration;
				
		float				mfAngle;
		float				mfTorque;
		float				mfLinearDamping;
		float				mfFriction;
		
		float				mfCustom_GravityScale;
		float				mfMass;

		/*The inverse of mass, 1/Mass)*/
		float				mfInvMass;       

		/*Elasticity Coefficient: 'Bounciness' of the body*/
		float				mfRestitution; 

		bool				mbIsGrounded;
		bool				mbHasGravity;
		bool				mbIsStatic;


		/*Quaternion if needed*/

	};
}




#endif
