/* HEADER *********************************************************************************/
/*!
\file	Rigidbody.cpp
\author Aaron Chu Ming San (100%)
\par    email: m.chu\@digipen.edu
\brief
Component to store the information about the object's transformations in space.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/RigidBody.h"
#include "System/Physics/PhysicsSystem.h"
#include "Object/GameObject.h"
#include "System/Logger/LoggerSystem.h"
#include "Math/Matrix4.h"

#include <cmath>

#define P_TX					mpOwnerTransform

namespace Dystopia
{ 
	RigidBody::RigidBody(void)
		: mpPrimitiveShape(nullptr)
		, mpOwnerTransform(nullptr)
		, mpPhysSys(nullptr)
		, mfAngleDeg(0.0F)
		, mfLinearDamping(0.6F)
		, mfAngularDrag(0.6F)
		, mfFriction(0.5F)
		, mfRestitution(0.5F)
		, mfGravityScale(1.0F)
		, mfMass(100.0F)
		, mfInvMass(0.01F)
		, mOwnerIsActive(true)
		, mbHasGravity(true)
		, mbIsStatic(false)
		, mPhysicsType(discrete)
	{
		
	}

	RigidBody::RigidBody(float _linearDrag, float _angularDrag, 
						float _friction, float _elasticity, 
						float _gravityScale, float _mass, 
						bool _gravityState, bool _staticState)
		: mpPrimitiveShape(nullptr)
		, mpOwnerTransform(nullptr)
		, mpPhysSys(nullptr)
		, mfAngleDeg(0.0F)
		, mfLinearDamping(_linearDrag)
		, mfAngularDrag(_angularDrag)
		, mfFriction(_friction)
		, mfRestitution(_elasticity)
		, mfGravityScale(_gravityScale)
		, mfMass(_mass)
		, mfInvMass(1/_mass)
		, mOwnerIsActive(true)
		, mbHasGravity(_gravityState)
		, mbIsStatic(_staticState)
	{

	}

	void RigidBody::Load(void)
	{
	}

	void RigidBody::Init(void)
	{
		// Get Owner's Transform Component as pointer
		if (nullptr == mpOwnerTransform && GetOwner())
		{
			mpPhysSys = EngineCore::GetInstance()->GetSystem<PhysicsSystem>();
			mpOwnerTransform = GetOwner()->GetComponent<Transform>();
			mOwnerIsActive = GetOwner()->IsActive();
			mPosition = P_TX->GetGlobalPosition();

			mInverseOrientation = Math::Inverse(mOrientation);
		}

		// If mass is zero, object is interpreted to be static
		if (mfMass > 0.0F)
		{
			mbIsStatic = false;
			mfInvMass = 1.0F / mfMass;
		}
		else //if mfMass <= 0
		{
			if (!mbIsStatic)
			{
				mbIsStatic = true;
			}
			mfInvMass = 0.0F;
		}

		// compute final local centroid
		mLocalCentroid *= mfInvMass;

		//// compute local inertia tensor
		//Mat3D localInertiaTensor{ { 1, 0, 0, 0 },{ 0, 1, 0, 0 },{ 0, 0, 1, 0 },{ 0, 0, 0, 1 } };

		//for (Collider &collider : m_colliders)
		//{
		//	const Vec3D r = mLocalCentroid - collider.m_localCentroid;
		//	const float rDotR = r.Dot(r);
		//	const Mat3D rOutR = r.Dot()

		//	// accumulate local inertia tensor contribution, 
		//	// using Parallel Axis Theorem
		//	localInertiaTensor +=
		//		collider.localInertiaTensor
		//		+ collider.m_mass * (rDotR * Mat3::Identity() - rOutR);
		//}
		//// using Parallel Axis Theorem
		//localInertiaTensor += mfMass * (rDotR * Mat3::Identity() - rOutR);

		//// compute inverse inertia tensor
		//m_localInverseInertiaTensor = localInertiaTensor.Inverted();
	}

	void RigidBody::Integrate(float _dt)
	{
		if (!GetOwner()->IsActive() || mbIsStatic)
		{
			return; // don't integrate when body is static
		}

		/* Physics 1.0 - Euler Integration
			Update Position
				mPosition += mLinearVelocity * _dt;

			Calculate Acceleration
				mAcceleration = Vec3D{ 0, mpPhysSys->mGravity*mfGravityScale,0 };
				const Vec3D newAccel = mCumulativeForce * mfInvMass + mAcceleration;

			Calculate Velocity
				mLinearVelocity += newAccel * _dt;
		*/

		/*********************************************************************
		 *  Physics 2.0
		 *  Verlet/Leapfrog method, 2nd order integration
		 ********************************************************************/

		//Store previous Position
		mPrevPosition = mPosition = mpOwnerTransform->GetGlobalPosition();

		//Determine the acceleration
		mAcceleration = mCumulativeForce * mfInvMass;

		if (mbHasGravity)
		{
			mAcceleration += Vec3D{0, mpPhysSys->mGravity*mfGravityScale,0};
		}

		// Update Position
		mPosition += (mLinearVelocity + mAcceleration * _dt * 0.5F) * _dt;

		//Integrate the velocity
		mLinearVelocity += mAcceleration * _dt;
		const Vec3D new_accel = mCumulativeForce * mfInvMass + mAcceleration;

		//Integrate the velocity
		mLinearVelocity += (new_accel - mAcceleration) * 0.5f * _dt;

		// Linear Damping (Drag)
		mLinearVelocity *= std::pow(mfLinearDamping, _dt);

		//Clamp to velocity max for numerical stability
		if (Dot(mLinearVelocity, mLinearVelocity) > mpPhysSys->mMaxVelSquared)
		{
			mLinearVelocity = Math::Normalise(mLinearVelocity);
			mLinearVelocity *= mpPhysSys->mMaxVelocityConstant;
		}

		 //*Reset Cumulative Force*/
		ResetCumulative();
	}

	void RigidBody::UpdateResult() const
	{
		if (!mbIsStatic) // only update when body is not static
		{
			P_TX->SetGlobalPosition(mPosition);
		}
	}

	/*void RigidBody::Update(float _dt)
	{
		
	}*/

	/*void RigidBody::LateUpdate(float _dt)
	{
		
	}*/

	void RigidBody::OnDestroy(void)
	{
		// stuff happens on component destroy
	}

	void RigidBody::Unload(void)
	{

	}

	RigidBody * RigidBody::Duplicate() const
	{
		return new RigidBody{ *this };
	}

	void RigidBody::Serialise(TextSerialiser & _serial) const
	{
		_serial;
	}

	void RigidBody::Unserialise(TextSerialiser & _serial)
	{
		_serial;
	}

	void RigidBody::DebugPrint()
	{
		if (!mbIsStatic)
			LoggerSystem::ConsoleLog(eLog::MESSAGE, "transform: (%f,%f) \n angle: %f", float(mPosition.x), float(mPosition.y), mfAngleDeg);
	}

	void RigidBody::DebugDraw()
	{
		//TODO: Jacky - some sort of debug renderer independent from graphics 
	}

	void RigidBody::GlobalCentroidFromPosition(void)
	{
		mGlobalCentroid = (mOrientation * mLocalCentroid) + mPosition;
	}

	void RigidBody::PositionFromGlobalCentroid(void)
	{
		mPosition = (mOrientation * -mLocalCentroid) + mGlobalCentroid;
	}

	Vec3D RigidBody::LocalToGlobal(const Vec3D &p) const
	{
		return mOrientation * p + mPosition;
	}

	Vec3D RigidBody::GlobalToLocal(const Vec3D &p) const
	{
		return mInverseOrientation * (p - mPosition);
	}

	Vec3D RigidBody::LocalToGlobalVec(const Vec3D &v) const
	{
		return mOrientation * v;
	}

	Vec3D RigidBody::GlobalToLocalVec(const Vec3D &v) const
	{
		return mInverseOrientation * v;
	}

	/*
	/brief
		Add a force at a certain position of the object.
		This will create a angular velocity that will cause the object to rotate
		& also move the object in the direction the force is applied

	/param _force
		The Amount of force to be applied to the object

	/param _point
		The point at which the force is applied to.
		_point should be in global coordinate

	/param _origin
		The pivot/origin position of the object
		_origin should be in global coordinate

	*/
	void RigidBody::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point, Math::Point3D const & _origin)
	{
		/*Vector from origin/pivot of the object to the _point where the force is applied*/
		Math::Vec3D && displacement_v = _point - _origin;
		/*Generate the angular velocity as a result of the force being applied at _point*/
		const Math::Vector3D angularVel = Cross(_force, displacement_v) / displacement_v.MagnitudeSqr();
		/*Add to the total Angular velocity of the object in this Update Frame*/
		mAngularVelocity += angularVel;
		/*Add the the total Linear Veloctiy of the object in this Update Frame*/
		mCumulativeForce += _force * 10000.0F;
	}

	void RigidBody::AddForce(Math::Vec3D const & _force)
	{
		mCumulativeForce += _force * 10000.0F;
	}

	void RigidBody::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point)
	{
		AddForce(_force, _point, P_TX->GetGlobalPosition());
	}

	void RigidBody::ResetCumulative()
	{
		mCumulativeForce = { 0,0,0,0 };
		mCumulativeTorque = { 0,0,0,0 };
	}

	/****************************************************************
	 * Settors
	 ****************************************************************/
	void RigidBody::SetGravityScale(float _scale)
	{
		if(!mbHasGravity)
		{		
			LoggerSystem::ConsoleLog(eLog::MESSAGE, "Note: Rigidbody has no gravity enabled.");
		}
		mfGravityScale = _scale;
	}

	bool RigidBody::Set_ToggleGravity()
	{
		return (mbHasGravity = mbHasGravity ? false : true);
	}

	void RigidBody::Set_ToggleGravity(bool _state)
	{
		mbHasGravity = _state;
	}

	void RigidBody::SetPosition(const Vec3D& _pos)
	{
		mPosition = _pos;
		P_TX->SetGlobalPosition(mPosition);
	}

	void RigidBody::SetVelocity(const Vec3D& _vel)
	{
		mLinearVelocity = _vel;
	}

	void RigidBody::SetMass(const float _mass)
	{
		mfMass = _mass;
		mfInvMass = 1 / mfInvMass;
	}

	void RigidBody::Set_IsStatic(bool _state)
	{
		mbIsStatic = _state;
	}


	/****************************************************************
	 * Gettors
	 ****************************************************************/
	Vec3D RigidBody::GetPosition() const
	{
		return mPosition;
	}

	Vec3D RigidBody::GetPrevPosition() const
	{
		return mPrevPosition;
	}

	Vec3D RigidBody::GetLinearVelocity() const
	{
		return mLinearVelocity;
	}

	Vec3D RigidBody::GetAngularVelocity() const
	{
		return mAngularVelocity;
	}
	Vec3D RigidBody::GetAcceleration() const
	{
		return mAcceleration;
	}

	Transform* RigidBody::GetOwnerTransform() const
	{
		return mpOwnerTransform;
	}

	float RigidBody::GetAngle() const
	{
		return mfAngleDeg;
	}
	float RigidBody::GetFrictionForce() const
	{
		return mfFriction;
	}

	float RigidBody::GetRestitution() const
	{
		return mfRestitution;
	}

	float RigidBody::GetGravityScalar() const
	{
		return mfGravityScale;
	}
	float RigidBody::GetMass() const
	{
		return mfMass;
	}

	float RigidBody::GetInverseMass() const
	{
		return mfInvMass;
	}

	bool RigidBody::Get_HasGravityState() const
	{
		return mbHasGravity;
	}
	bool RigidBody::Get_IsStaticState() const
	{
		return mbIsStatic;
	}
}
