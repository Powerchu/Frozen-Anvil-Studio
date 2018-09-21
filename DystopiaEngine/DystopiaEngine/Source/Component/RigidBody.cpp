#include "Component/RigidBody.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"
#include <System/Logger/LoggerSystem.h>

//TODO: change to physics systems
#define GRAVITY_CONSTANT	-9.81F 
#define TX					mpOwnerTransform

namespace Dystopia
{ 
	RigidBody::RigidBody(void)
		: mpPrimitiveShape(nullptr)
		, mpOwnerTransform(nullptr)
		, mOwnerIsActive(true)
		, mPosition(Vec3D{})
		, mPrevPosition(Vec3D{})
		, mLinearVelocity(Vec3D{})
		, mAngularVelocity(Vec3D{})
		, mCumulativeForce(Vec3D{})
		, mAcceleration(Vec3D{})
		, mfAngle(0.0F)
		, mfTorque(0.0F)
		, mfLinearDamping(0.9F)
		, mfFriction(0.0F)
		, mfCustom_GravityScale(0.25F)
		, mfGravity{GRAVITY_CONSTANT*mfCustom_GravityScale}
		, mfMass(100.0F)
		, mfInvMass(0.0F)
		, mfRestitution(0.0F)
		, mbIsGrounded(false)
		, mbHasGravity(true)
		, mbIsStatic(false)
	{
		
	}

	RigidBody::~RigidBody(void)
	{
		//delete mpPrimitiveShape;
	}

	void RigidBody::Load(void)
	{
	}

	void RigidBody::Init(void)
	{
		// Get Owner's Transform Component as pointer
		if (nullptr == mpOwnerTransform)
		{
			if (GetOwner())
			{
				mpOwnerTransform = GetOwner()->GetComponent<Transform>();
				mPosition = TX->GetGlobalPosition();

				mOwnerIsActive = GetOwner()->IsActive();
			}
		}
		mfCustom_GravityScale = mbHasGravity ? mfCustom_GravityScale : 0.0F;
		mfGravity = GRAVITY_CONSTANT * mfCustom_GravityScale;

		//If mass is zero object is interpreted
		//to be static
		if (mfMass > 0.0F)
		{
			mbIsStatic = false;
			mfInvMass = 1.0F / mfMass;
		}
		else
		{
			if (!mbIsStatic)
			{
				mbIsStatic = true;
			}
			mfInvMass = 0.0F;
		}

		if (!mbHasGravity)
		{
			mfCustom_GravityScale = 0.0F;
		}
	}

	void RigidBody::Integrate(float _dt)
	{
		if (!GetOwner()->IsActive() || mbIsStatic) // when editor / play is separated, change GetOwner to own bool
		{
			return;
		}

		//Store previous Position
		mPrevPosition = mPosition;

		// Update Position
		mPosition += mLinearVelocity * _dt;

		//TODO: Incorporate and move Gravity var to Physics System
		// Calculate Acceleration
		mAcceleration = Vec3D{ 0,mfGravity*40*mfCustom_GravityScale,0 };
		const Vec3D newAccel = mCumulativeForce * mfInvMass + mAcceleration;

		// Calculate Velocity
		mLinearVelocity += newAccel * _dt;

		// Damping (Drag)
		//mLinearVelocity *= Math::Power<_dt, float>(mfLinearDamping); //TODO fix this, need Pow for double/floats

		//TODO
		/*
		 * //Clamp to velocity max for numerical stability
		if ( Dot(Velocity, Velocity) > PHYSICS->MaxVelocitySq )
		{
			Normalize(Velocity);
			Velocity = Velocity * PHYSICS->MaxVelocity;
		}
		 */

		 //*Reset Cumulative Force*/
		ResetCumulative();
	}

	void RigidBody::PostResult()
	{
		if (!mbIsStatic)
		{
			TX->SetGlobalPosition(mPosition);
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
		LoggerSystem::ConsoleLog(eLog::MESSAGE, "transform: (%f,%f) \n angle: %f", mPosition.x, mPosition.y, mfAngle);
	}

	void RigidBody::DebugDraw()
	{
		//TODO: Jacky - some sort of debug renderer independent from graphics 
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
		/*Add to the total Anuglar velocity of the object in this Update Frame*/
		mAngularVelocity += angularVel;
		/*Add the the total Linear Veloctiy of the object in this Update Frame*/
		mCumulativeForce += _force;
	}

	void RigidBody::AddForce(Math::Vec3D const & _force)
	{
		mCumulativeForce += _force;
	}

	void RigidBody::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point)
	{
		AddForce(_force, _point, TX->GetGlobalPosition());
	}

	void RigidBody::ResetCumulative()
	{
		mCumulativeForce = { 0,0,0,0 };
	}

	/****************************************************************
	 * Settors
	 ****************************************************************/
	void RigidBody::Set_CustomGravityScale(float _scale)
	{
		if(mbHasGravity)
		{		
			mfCustom_GravityScale = _scale;
		}
		else
		{
			LoggerSystem::ConsoleLog(eLog::WARNING, "Rigidbody has no gravity enabled.");
			mfCustom_GravityScale = 0;
		}
	}

	bool RigidBody::Set_ToggleGravity()
	{
		return (mbHasGravity = mbHasGravity ? false : true);
	}

	void RigidBody::Set_ToggleGravity(bool _state)
	{
		mfGravity = _state;
	}

	void RigidBody::SetPosition(const Vec3D& _pos)
	{
		mPosition = _pos;
		TX->SetGlobalPosition(mPosition);
	}

	void RigidBody::SetVelocity(const Vec3D& _vel)
	{
		mLinearVelocity = _vel;
	}

	void RigidBody::Set_IsStatic(bool _state)
	{
		mbIsStatic = _state;
	}


	/****************************************************************
	 * Gettors
	 ****************************************************************/
	Vec3D RigidBody::GetPosition()
	{
		return mPosition;
	}

	Vec3D RigidBody::GetPrevPosition()
	{
		return mPrevPosition;
	}

	Vec3D RigidBody::GetLinearVelocity()
	{
		return mLinearVelocity;
	}

	Vec3D RigidBody::GetAngularVelocity()
	{
		return mAngularVelocity;
	}
	Vec3D RigidBody::GetAcceleration()
	{
		return mAcceleration;
	}

	float RigidBody::GetAngle()
	{
		return mfAngle;
	}
	float RigidBody::GetFrictionForce()
	{
		return mfFriction;
	}

	float RigidBody::GetGravityScalar()
	{
		return mfCustom_GravityScale;
	}
	float RigidBody::GetMass()
	{
		return mfMass;
	}

	float RigidBody::GetInverseMass()
	{
		return mfInvMass;
	}

	bool RigidBody::Get_IsGroundedState()
	{
		return mbIsGrounded;
	}
	bool RigidBody::Get_HasGravityState()
	{
		return mbHasGravity;
	}
	bool RigidBody::Get_IsStaticState()
	{
		return mbIsStatic;
	}
}
