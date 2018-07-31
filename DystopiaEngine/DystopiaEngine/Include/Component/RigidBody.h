#pragma once
#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "Component\Component.h"
#include "ComponentList.h"

#include "Math\Quaternion.h"
#include "Math\Vector2.h"

class Transform;

namespace Dystopia
{
	class RigidBody : public Component
	{
	public:
		using TAG = ComponentTag;
		static constexpr eComponents TYPE = eComponents::RIGID_BODY;
		virtual const eComponents GetComponentType(void) const { return TYPE; };


		// ====================================== CONSTRUCTORS ======================================= // 

		RigidBody(void);
		virtual ~RigidBody(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 
		virtual void Load(void);
		virtual void Init(void);
		virtual void OnDestroy(void);
		virtual void Unload(void);
		virtual RigidBody* Duplicate() const;
		virtual void Serialise(TextSerialiser&);
		virtual void Unserialise(TextSerialiser&);

		void Update(float _dt);

		void AddForce(float const & _force);


	private:
		Transform*        mOwnerTransform;    /*Used for accessing position and GameObject World orientation*/
		Math::Vec3D       mVelocity;          /*Velocity of the RigidBody with respect to World Space*/
		Math::Vec3D       mAngularVelocity;   /*Angular Velocity/Rotation with respect to World Space*/
		Math::Vec3D       mCumulativeVector;  /*The sum of all the force acting on the body*/
		float             mInverseMass;       /*The inverse of mass, (1/Mass)*/


		/*Quaternion if needed*/

	};
}




#endif
