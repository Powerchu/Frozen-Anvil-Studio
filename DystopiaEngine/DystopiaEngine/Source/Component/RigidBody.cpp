#include "Component/RigidBody.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"
#include <Editor/ConsoleDebugger.h>

#define GRAVITY_CONSTANT	-9.81F //TODO: change to physics systems
#define TX					mOwnerTransform

namespace Dystopia
{ 
	RigidBody::RigidBody(void)
		: mPrimitiveShape(nullptr)
		, mOwnerTransform(nullptr)
		, mPosition(Vec3D{})
		, mPrevPosition(Vec3D{})
		, mLinearVelocity(Vec3D{})
		, mAngularVelocity(Vec3D{})
		, mCumulativeForce(Vec3D{})
		, mAcceleration(Vec3D{})
		, mfAngle(0.0F)
		, mfTorque(0.0F)
		, mfDamping(0.9F)
		, mfFriction(0.0F)
		, mfCustom_GravityScale(1.0F)
		, mfGravity{GRAVITY_CONSTANT*mfCustom_GravityScale}
		, mfMass(100.0F)
		, mfInvMass(0.0F)
		, mfElasticCoeff(0.0F)
		, mbIsGrounded(false)
		, mbHasGravity(true)
		, mbIsBouncy()
		, mbIsStatic()
	{
		
	}

	RigidBody::~RigidBody(void)
	{
		delete mPrimitiveShape;
	}

	void RigidBody::Load(void)
	{
	}

	void RigidBody::Init(void)
	{
		// Get Owner's Transform Component as pointer
		mOwnerTransform = GetOwner()->GetComponent<Transform>();
		mPosition = TX->GetGlobalPosition();
		mfCustom_GravityScale = mbHasGravity ? mfCustom_GravityScale : 0.0F;
		mfGravity = GRAVITY_CONSTANT * mfCustom_GravityScale;

		//If mass is zero object is interpreted
		//to be static
		if (mfMass > 0.0f)
		{
			mbIsStatic = false;
			mfInvMass = 1.0f / mfMass;
		}
		else
		{
			mbIsStatic = true;
			mfInvMass = 0.0f;
		}
	}

	void RigidBody::Update(float _dt)
	{
		if (mbIsStatic) return;

		//Store previous Position
		mPrevPosition = mPosition;

		// Update Position
		mPosition += mLinearVelocity * _dt;

		//TODO: Incorporate and move Gravity var to Physics System
		// Calculate Acceleration
		mAcceleration = Vec3D{0,mfGravity,0};
		Vec3D newAccel = mCumulativeForce * mfInvMass + mAcceleration;

		// Calculate Velocity
		mLinearVelocity+= newAccel * _dt;

		// Damping (Drag)
		//mLinearVelocity *= Math::Power<static_cast<int>(_dt), float>(mfDamping); //TODO fix this, need Pow for double/floats

		//TODO
		/*
		 * //Clamp to velocity max for numerical stability
		if ( Dot(Velocity, Velocity) > PHYSICS->MaxVelocitySq )
		{
			Normalize(Velocity);
			Velocity = Velocity * PHYSICS->MaxVelocity;
		}
		 */

		TX->SetGlobalPosition(mPosition);
	}

	void RigidBody::LateUpdate(float _dt)
	{
		/*Reset Cumulative Force*/
		ResetCumulative();
	}

	void RigidBody::OnDestroy(void)
	{
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

	void RigidBody::PrintRigidBodies()
	{
		ConsolePrint("transform: x(" + std::to_string(TX->GetGlobalPosition().x) + ')' +
									"y(" + std::to_string(TX->GetGlobalPosition().y) + ')'
					+ "angle: " + std::to_string(mfAngle));
	}

	void RigidBody::DebugDraw()
	{
		//TODO: Jacky - some sort of debug renderer independent from graphics 
	}

	void RigidBody::Set_CustomGravityScale(float _scale)
	{
		if(mbHasGravity)
		{		
			mfCustom_GravityScale = _scale;
		}
		else
		{
			ConsolePrint("Rigidbody has no gravity enabled.");
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
		mAngularVelocity  += angularVel;
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

}
