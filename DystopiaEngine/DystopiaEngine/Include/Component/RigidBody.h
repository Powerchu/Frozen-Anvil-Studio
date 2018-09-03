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
		virtual void Serialise(TextSerialiser&) const    override;
		virtual void Unserialise(TextSerialiser&)        override;
		// ===================================== MEMBER FUNCTIONS ==================================== // 
		void Update(float _dt);

		void LateUpdate(float _dt);
		/*Add a force at the origin of the body*/
		void AddForce(Math::Vec3D const & _force);

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
		Transform*        mOwnerTransform;    /*Used for accessing position and GameObject World orientation*/
		Math::Vec3D       mVelocity;          /*Velocity of the RigidBody with respect to World Space*/
		Math::Vec3D       mAngularVelocity;   /*Angular Velocity/Rotation with respect to World Space*/
		Math::Vec3D       mCumulativeVector;  /*The sum of all the force acting on the body*/
		float             mInverseMass;       /*The inverse of mass, (1/Mass)*/


		/*Quaternion if needed*/

	};
}




#endif
