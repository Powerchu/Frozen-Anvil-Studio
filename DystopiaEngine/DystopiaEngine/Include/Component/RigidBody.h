#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "Component/Component.h"
#include "ComponentList.h"

#include "Math/Quaternion.h"
#include "Math/Angles.h"
#include "Math/Vector2.h"

#include <Component/Primitive.h>

namespace Dystopia
{
	class Transform;

	class RigidBody : public Component
	{
	public:
		using TAG = ComponentTag;

		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllComponents>::value;
		};


		// ====================================== CONSTRUCTORS ======================================= // 

		RigidBody(void);
		virtual ~RigidBody(void);


		// ================================VIRTUAL MEMBER FUNCTIONS ================================== // 
		virtual void Load(void);
		virtual void Init(void);
		virtual void OnDestroy(void);
		virtual void Unload(void);
		virtual RigidBody* Duplicate() const;
		virtual void Serialise(TextSerialiser&) const;
		virtual void Unserialise(TextSerialiser&);
		// ===================================== MEMBER FUNCTIONS ==================================== // 
		//TODO: Delete this once graphics is up
		void PrintRigidBodies(); // FOR TESTING

		void Set_CustomGravityScale(const float scale);
		bool ToggleGravity(); //returns gravity state

		void Update(float _dt);
		void LateUpdate(float _dt);

		/*Add a force at the origin of the body*/
		void AddForce(Math::Vec3D const & _force);

		// Gettors


		/*************************************************************************************************
		\brief
		Add a force at a point. If point is not origin, this will generate a
		Angular velocity and Torgue with respect to GameObject Origin

		Make sure _point is in global coordinates

		**************************************************************************************************/
		void AddForce(Math::Vec3D const & _force, Math::Point3D const & _point);

		/**************************************************************************************************
		\brief
		Add a force at a point. If point is not origin, this will generate a
		Angular velocity and Torgue with respect to _origin

		Make sure _point & _origin is in global coordinates
		**************************************************************************************************/
		void AddForce(Math::Vec3D   const & _force,
			          Math::Point3D const & _point, 
			          Math::Point3D const & _origin);

		/**************************************************************************************************
		\brief
		Resets the Cumulative Force for the current RigidBody
		**************************************************************************************************/
		void ResetCumulative();

	private:
		Primitive*			m_PrimitiveShape;    /*The underlying primitive of the RigidBody*/
		Transform*			m_OwnerTransform;    /*Used for accessing position and GameObject World orientation*/
		Math::Vec3D			m_LinearVelocity;    /*Linear Velocity of the RigidBody with respect to World Space*/

		Math::Vec3D			m_Force;
		Math::Vec3D			m_AngularVelocity;   /*Angular Velocity/Rotation with respect to World Space*/
		Math::Vec3D			m_CumulativeVector;  /*The sum of all the force acting on the body*/
		float				m_angle;
		float				m_torque;
		float				m_drag;
		float				m_friction;
		
		float				m_custom_gravityScale;
		float				m_gravity;
		float				m_Mass;
		float				m_InverseMass;       /*The inverse of mass, (1/Mass)*/

		bool				m_IsGrounded;
		bool				m_HasGravity;


		/*Quaternion if needed*/

	};
}




#endif
