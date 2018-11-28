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
#include "Component/Collider.h"
#include "System/Physics/PhysicsSystem.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "System/Logger/LoggerSystem.h"
//#include "System/Scene/SceneSystem.h"
//#include "System/Scene/Scene.h"
#include "IO/TextSerialiser.h"

#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#endif 

#include <cmath>

#define P_TX mpOwnerTransform

namespace Dystopia
{
	RigidBody::RigidBody(void)
		: mpOwnerTransform(nullptr)
		  , mpPhysSys(nullptr)
		  , mfAngleDegZ(0.0F)
		  , mLinearDamping(0.5, 0.5)
		  , mfAngularDrag(0.5F)
		  , mfStaticFriction(0.5F)
		  , mfDynamicFriction(0.7F)
		  , mfRestitution(0.5F)
		  , mfGravityScale(1.0F)
		  , mfMass(100.0F)
		  , mfInvMass(0.01F)
		  , mfInertia(mfMass)
		  , mfInvInertia(0.01F)
		  , mfWeightedMotion(0.0F)
		  , mbHasGravity(true)
		  , mbIsStatic(false)
		  , mbIsAwake(true)
		  , mbCanSleep(true)
		  , mbFixedRot(false)
		  , mSleepTime(0.00F)
		  , mContactsNo(0)
		  , mPhysicsType(eDynamicBody)
	{
	}

	RigidBody::RigidBody(float _linearDrag, float _angularDrag,
		float _friction, float _elasticity,
		float _gravityScale, float _mass,
		bool _gravityState, bool _staticState)
		: mpOwnerTransform(nullptr)
		  , mpPhysSys(nullptr)
		  , mfAngleDegZ(0.0F)
		  , mLinearDamping(_linearDrag, _linearDrag)
		  , mfAngularDrag(_angularDrag)
		  , mfStaticFriction(_friction)
		  , mfDynamicFriction(_friction)
		  , mfRestitution(_elasticity)
		  , mfGravityScale(_gravityScale)
		  , mfMass(_mass)
		  , mfInvMass(1 / _mass)
		  , mfInertia(mfMass)
		  , mfInvInertia(0.01F)
		  , mfWeightedMotion(0.0F)
		  , mbHasGravity(_gravityState)
		  , mbIsStatic(_staticState)
		  , mbIsAwake(true)
		  , mbCanSleep(true)
		  , mbFixedRot(false)
		  , mSleepTime(0.00F)
		  , mContactsNo(0)
	      , mPhysicsType(eDynamicBody)
	{
	}

	void RigidBody::Awake(void)
	{
		// Get Owner's Transform Component as pointer
		if (nullptr != GetOwner())
		{
			if (GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) return;

			mpPhysSys = EngineCore::GetInstance()->GetSystem<PhysicsSystem>();
			mpOwnerTransform = GetOwner()->GetComponent<Transform>();
			mPrevPosition = mPosition = mpOwnerTransform->GetGlobalPosition();

			const auto ColComponents = GetOwner()->GetComponents<Collider>();

			AutoArray<Collider*> ToRet;

			for (auto elem : ColComponents)
			{
				ToRet.push_back(elem);
			}

			mparrCol = Ut::Move(ToRet);

			mOrientation = Math::RotateZ(Math::Degrees{ P_TX->GetGlobalRotation().ToEuler().z });
			mInverseOrientation = Math::AffineInverse(mOrientation);
			mfAngleDegZ = Math::Degrees{ P_TX->GetGlobalRotation().ToEuler().z }.Radians();
		}
	}

	void RigidBody::Init(void)
	{
		// If mass is zero, object is interpreted to be static
		if (mfMass > 0.0F)
		{
			mfInvMass = 1.0F / mfMass;
			mfInertia = mfMass;
			mfInvInertia = mfInvMass;

			mbIsStatic = false;
			mPhysicsType = eDynamicBody;
		}
		else //if mfMass <= 0
		{
			if (!mbIsStatic)
			{
				mbIsStatic = true;
				mPhysicsType = eStaticBody;
			}
			mfInvMass = 0.0F;
			mfInvInertia = 0.0F;
		}

		// compute final local centroid
		//mLocalCentroid *= mfInvMass;

		// If Static, then is Sleeping
		if (mbIsStatic) {
			mbIsAwake = false;
			mPhysicsType = eStaticBody;
		}
		else
		{
			mbIsAwake = true;
			mPhysicsType = eDynamicBody;
		}

		mLocalCentroid = mPosition * mfInvMass;
		//mLocalCentroid *= mfInvMass;

		//// compute local inertia tensor
		Mat3D localInertiaTensor{ { 1, 0, 0, 0 },{ 0, 1, 0, 0 },{ 0, 0, 1, 0 },{ 0, 0, 0, 1 } };

		mLocalInvInertiaTensor = localInertiaTensor.AffineInverse();
	}

	void RigidBody::Integrate(float _dt)
	{
		constexpr const auto VEL_EPSILON = 0.001F;

		if (mbIsStatic || !mbIsAwake)
		{
			return; // don't integrate when body is static
		}

		if (GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) return;


		/*********************************************************************
		 *  Physics 2.0
		 *  Verlet/Leapfrog method, 2nd order integration
		 ********************************************************************/
		 //Store previous Position
		mPrevPosition = mPosition = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
		//mfAngleDegZ = Math::Degrees{ P_TX->GetGlobalRotation().ToEuler().z }.Radians();

		// Store Rotational Tensors
		mGlobalInvInertiaTensor =
			mOrientation * mLocalInvInertiaTensor.AffineInverse() * (mInverseOrientation);


		if (!GetOwner()->IsActive())
		{
			return;
		}


		if (mbHasGravity)
		{
			mLinearAcceleration = Vec3D{ 0, -mpPhysSys->mGravity*mfGravityScale,0 };
		}

		//Determine the acceleration
		mLinearAcceleration += mCumulativeForce * mfInvMass;
		mAngularAcceleration += mGlobalInvInertiaTensor * mCumulativeTorque;

		//Integrate the velocity
		mLinearVelocity += mLinearAcceleration * _dt;

		if (!mbFixedRot)
			mAngularVelocity += mAngularAcceleration * _dt;

		const Vec3D new_accel = mCumulativeForce * mfInvMass + mLinearAcceleration;
		const Vec3D new_ang_accel = mGlobalInvInertiaTensor * mCumulativeTorque + mAngularAcceleration;

		//Integrate the velocity
		mLinearVelocity += (new_accel - mLinearAcceleration) * 0.5f * _dt;

		//Integrate angular velocity
		if (!mbFixedRot)
			mAngularVelocity += (new_ang_accel - mAngularAcceleration) * 0.5f * _dt;

		// Linear Damping (Drag)
		mLinearVelocity.x = mLinearVelocity.x * std::pow(1.0F - mLinearDamping.x, _dt); // x vector
		mLinearVelocity.y = mLinearVelocity.y * std::pow(1.0F - mLinearDamping.y, _dt); // y vector


		mAngularVelocity *= std::pow(1.0F - mfAngularDrag, _dt);

		// Clamp to zero
		if (std::abs(mLinearVelocity.x) < VEL_EPSILON)
		{
			mLinearVelocity.x = 0;
		}

		if (std::abs(mLinearVelocity.y) < VEL_EPSILON)
		{
			mLinearVelocity.y = 0;
		}

		if (std::abs(mAngularVelocity.z) < VEL_EPSILON)
		{
			mAngularVelocity.z = 0;
		}

		//Clamp to velocity max for numerical stability
		if (Dot(mLinearVelocity, mLinearVelocity) > mpPhysSys->mMaxVelSquared)
		{
			mLinearVelocity = Math::Normalise(mLinearVelocity);
			mLinearVelocity *= mpPhysSys->mMaxVelocityConstant;
		}

		//Clamp to velocity max for numerical stability
		if (Dot(mAngularVelocity, mAngularVelocity) > mpPhysSys->mMaxVelSquared * 0.5F)
		{
			mAngularVelocity = Math::Normalise(mAngularVelocity);
			mAngularVelocity *= mpPhysSys->mMaxVelocityConstant * 0.5F;
		}

		//*Reset Cumulative Force*/
		ResetCumulative();
	}

	/*
	 * Formula from http://www.xbdev.net/physics/RigidBodyImpulseCubes/
	 */
	void RigidBody::CheckSleeping(const float _dt)
	{
		if (GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) return;

		UNUSED_PARAMETER(_dt); // dt is fixed, so it doesnt matter anyway

		size_t tempContacts = 0;
		bool hasChanged = false;

		if (!GetOwner()->IsActive())
		{
			mbIsAwake = true;
			return;
		}

		for (auto elem : mparrCol)
		{
			for (const auto& count : elem->GetCollisionEvents())
			{
				UNUSED_PARAMETER(count);
				tempContacts += 1;
			}
		}

		// Colliders have lost a contact
		if (tempContacts != mContactsNo)
		{
			mbIsAwake = true;
			hasChanged = true;
		}

		mContactsNo = 0;

		for (auto elem : mparrCol)
		{
			for (const auto& count : elem->GetCollisionEvents())
			{
				UNUSED_PARAMETER(count);
				mContactsNo += 1;
			}
		}

		if (hasChanged) return;
		
		//mfWeightedMotion is the average kinetic energy over a given set of frames
		const auto currentMotion = mLinearVelocity.MagnitudeSqr() + mAngularVelocity.MagnitudeSqr();
		mfWeightedMotion = mpPhysSys->mfSleepBias * mfWeightedMotion + (1 - mpPhysSys->mfSleepBias)*currentMotion;

		if (mfWeightedMotion < mpPhysSys->mfSleepVelEpsilon)
		{
			mbIsAwake = false;
			mLinearAcceleration = { 0,0,0 };
			mLinearVelocity = { 0,0,0 };
			mAngularVelocity = { 0,0,0 };
			mCumulativeTorque = { 0,0,0 };
			mCumulativeForce = { 0,0,0 };
		}

		else if (mfWeightedMotion > 10 * mpPhysSys->mfSleepVelEpsilon)
		{
			mfWeightedMotion = 10 * mpPhysSys->mfSleepVelEpsilon;
			mbIsAwake = true;
		}

		else
		{
			mbIsAwake = true;
		}
	}

	void RigidBody::PreUpdatePosition(float _dt)
	{
		if (GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) return;

		mPosition += mLinearVelocity * _dt;

		if (!mbFixedRot)
			mfAngleDegZ += Math::Radians{ mAngularVelocity.Magnitude() }.Degrees() * _dt;

		if (mfAngleDegZ < 0.0F) mfAngleDegZ = 360.0F;
		if (mfAngleDegZ > 360.0F) mfAngleDegZ = 0.0F;
	}

	void RigidBody::UpdateResult(float) const
	{
		// Update Position
		GetOwner()->GetComponent<Transform>()->SetGlobalPosition(mPosition);
		GetOwner()->GetComponent<Transform>()->SetRotation(Math::Radians{ 0 }, Math::Radians{ 0 }, Math::Degrees{ mfAngleDegZ });
	}

	/*void RigidBody::Update(float _dt)
	{

	}*/

	/*void RigidBody::LateUpdate(float _dt)
	{

	}*/

	/*void RigidBody::Unload(void)
	{
		for (auto& elem : mparrCol)
		{
			UNUSED_PARAMETER(elem);
			mparrCol.pop_back();
		}

		mpOwnerTransform = nullptr;
		mpPhysSys = nullptr;
	}*/

	RigidBody * RigidBody::Duplicate() const
	{
		const auto cc = EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent(*this);
		//cc->SetOwner(GetOwner());
		//cc->Init();
		return cc;
	}

	void RigidBody::Serialise(TextSerialiser & _out) const
	{
		_out.InsertStartBlock("RigidBody");
		Component::Serialise(_out);
		_out << mfAngleDegZ;				// Angle in degrees
		_out << static_cast<float>(mLinearDamping.x);		// Linear Drag X
		_out << static_cast<float>(mLinearDamping.y);		// Linear Drag y
		_out << mfAngularDrag;			// Angular Drag
		_out << mfStaticFriction;
		_out << mfDynamicFriction;
		_out << mfRestitution;
		_out << mfGravityScale;
		_out << mfMass;
		_out << mbHasGravity;			// Gravity State
		_out << mbIsStatic;				// Static State
		_out << mbIsAwake;				// Awake State
		_out << mbCanSleep;				// Can Sleep Boolean
		_out << static_cast<int>(mPhysicsType);
		_out.InsertEndBlock("RigidBody");
	}

	void RigidBody::Unserialise(TextSerialiser & _in)
	{
		int type;
		//int physicsType;
		_in.ConsumeStartBlock();
		Component::Unserialise(_in);
		_in >> mfAngleDegZ;				// Angle in degrees
		_in >> mLinearDamping.x;			// Linear Drag
		_in >> mLinearDamping.y;			// Linear Drag
		_in >> mfAngularDrag;			// Angular Drag
		_in >> mfStaticFriction;
		_in >> mfDynamicFriction;
		_in >> mfRestitution;
		_in >> mfGravityScale;
		_in >> mfMass;
		_in >> mbHasGravity;			// Gravity State
		_in >> mbIsStatic;				// Static State
		_in >> mbIsAwake;				// Awake State
		_in >> mbCanSleep;				// Can Sleep Boolean
		_in >> type;
		_in.ConsumeEndBlock();

		mPhysicsType = static_cast<PhysicsType>(type);
	}

	void RigidBody::DebugPrint()
	{
		if (GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) return;

		if (!mbIsStatic)
			LoggerSystem::ConsoleLog(eLog::MESSAGE, "transform: (%f,%f) \n X vel: %f, Y vel: %f \n Awake: %i", float(mPosition.x), float(mPosition.y), float(mLinearVelocity.x), float(mLinearVelocity.y), mbIsAwake);
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
		if (mbIsStatic)
		{
			return;
		}

		if (!mbIsAwake)
		{
			SetSleeping(false);
		}
		mCumulativeForce += _force;
		mCumulativeTorque += (_point - _origin).Cross(_force);
	}

	void RigidBody::AddForce(Math::Vec3D const & _force)
	{
		if (mbIsStatic)
		{
			return;
		}

		if (!mbIsAwake)
		{
			SetSleeping(false);
		}
		mCumulativeForce += _force;
	}

	void RigidBody::AddForceWithOrigin(Math::Vec3D const & _force, Math::Point3D const & _point)
	{
		AddForce(_force, _point, mPosition);
	}

	void RigidBody::AddTorque(Math::Vec3D const& _torque)
	{
		if (mbIsStatic)
		{
			return;
		}

		if (!mbIsAwake)
		{
			SetSleeping(false);
		}
		mCumulativeTorque += _torque;
	}

	void RigidBody::AddLinearImpulse(Vec3D const& _impul)
	{
		if (mbIsStatic)
		{
			return;
		}

		if (!mbIsAwake)
		{
			//SetSleeping(false);
		}
		mLinearVelocity += _impul * mfInvMass;
	}

	void RigidBody::AddLinearImpulse(Vec3D const & _impul, Point3D const & _point, Point3D const & _origin)
	{
		if (mbIsStatic)
		{
			return;
		}

		if (!mbIsAwake)
		{
			//SetSleeping(false);
		}
		mLinearVelocity += _impul * mfInvMass;

		if (!mbFixedRot)
			mAngularVelocity += mfInvInertia * (_point - _origin).Cross(_impul);
	}

	void RigidBody::AddLinearImpulseWithOrigin(Vec3D const & _impul, Point3D const & _point)
	{
		AddLinearImpulse(_impul, _point, mPosition);
	}

	void RigidBody::ApplyAngularImpulse(Vec3D const & _impul)
	{
		if (mbIsStatic)
		{
			return;
		}

		if (!mbIsAwake)
		{
			//SetSleeping(false);
		}
		mAngularVelocity += mfInvInertia * _impul;
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
		if (!mbHasGravity)
		{
			LoggerSystem::ConsoleLog(eLog::MESSAGE, "Note: Rigidbody has no gravity enabled.");
		}
		mfGravityScale = _scale;
	}

	bool RigidBody::Set_ToggleGravity()
	{
		return (mbHasGravity = !mbHasGravity);
	}

	void RigidBody::Set_ToggleGravity(bool _state)
	{
		mbHasGravity = _state;
	}

	void RigidBody::SetPosition(const Vec3D& _pos)
	{
		mPosition = _pos;
		//P_TX->SetGlobalPosition(mPosition);
	}

	void RigidBody::SetLinearVel(const Vec3D& _vel)
	{
		if (mbIsStatic)
		{
			return;
		}

		mLinearVelocity = _vel;

	}

	void RigidBody::SetAngularVel(const Vec3D& _vel)
	{
		if (mbIsStatic)
		{
			return;
		}

		mAngularVelocity = _vel;

	}

	void RigidBody::SetMass(const float _mass)
	{
		mfMass = _mass;
		mfInvMass = 1 / mfInvMass;
	}

	void RigidBody::SetStaticFriction(const float _f)
	{
		if (_f > 2.0F) mfStaticFriction = 2.0F;
		else if (_f < 0.0) mfStaticFriction = 0.0F;
		else mfStaticFriction = _f;
	}

	void RigidBody::SetKineticFriction(const float _f)
	{
		if (_f > 2.0F) mfDynamicFriction = 2.0F;
		else if (_f < 0.0) mfDynamicFriction = 0.0F;
		else mfDynamicFriction = _f;
	}

	void RigidBody::SetRestitution(const float _f)
	{
		if (_f > 2.0F) mfRestitution = 2.0F;
		else if (_f < 0.0) mfRestitution = 0.0F;
		else mfRestitution = _f;
	}

	void RigidBody::SetSleeping(const bool flag)
	{
		if (!flag) // awake
		{
			//m_flags |= e_awakeFlag;
			mbIsAwake = !flag;
			mSleepTime = 0.0f;
		}
		else // sleeping
		{
			//m_flags &= ~e_awakeFlag;
			mbIsAwake = flag;
			mSleepTime = 0.0f;
			mLinearVelocity = { 0,0,0,0 };
			mAngularVelocity = { 0,0,0,0 };
			mCumulativeForce = { 0,0,0,0 };
			mCumulativeTorque = { 0,0,0,0 };
		}

	}

	void RigidBody::SetFixedRotation(bool flag)
	{
		mbFixedRot = flag;
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
		return mLinearAcceleration;
	}

	Transform* RigidBody::GetOwnerTransform() const
	{
		return mpOwnerTransform;
	}

	float RigidBody::GetAngle() const
	{
		return mfAngleDegZ;
	}
	float RigidBody::GetStaticFriction() const
	{
		return mfStaticFriction;
	}

	float RigidBody::GetKineticFriction() const
	{
		return mfDynamicFriction;
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

	float RigidBody::GetInertia() const
	{
		return mfInertia + mfMass * Math::Dot(mPosition, mPosition);
	}

	float RigidBody::GetInverseInertia() const
	{
		return mfInvInertia;
	}

	bool RigidBody::Get_HasGravityState() const
	{
		return mbHasGravity;
	}
	bool RigidBody::Get_IsStaticState() const
	{
		return mbIsStatic;
	}
	bool RigidBody::GetIsAwake() const
	{
		return mbIsAwake;
	}

	bool RigidBody::Get_IsFixedRotation() const
	{
		return mbFixedRot;
	}

	const Vec3D & RigidBody::GetWorldCenter() const
	{
		// TODO: insert return statement here
		return mPosition;
	}

	const Vec3D & RigidBody::GetLocalCenter() const
	{
		// TODO: insert return statement here
		return mPosition;
	}

	/****************************************************************
	* EDITOR GUI STUFF
	****************************************************************/
#if EDITOR
	void RigidBody::CheckMass()
	{
		if (mfMass)
		{
			mfInvMass = 1 / mfMass;
		}
		else
		{
			mfInvMass = 0;
			mbIsStatic = true;
			mPhysicsType = eStaticBody;
		}
	}


	void RigidBody::EditorUI() noexcept
	{
		EGUI::PushLeftAlign(140.f);

		eBodyTypeDropDown();
		eMassField();

		EGUI::PopLeftAlign();

		eLinearDragField();

		EGUI::PushLeftAlign(140.f);

		eAngularDragField();
		eGravityScaleField();
		eStaticFrictionDragField();
		eDynamicFrictionDragField();
		eRestitutionDragField();

		eCollisionDetectionField();
		eSleepingModeDropdown();
		eRotationConstraintCheckBox();

		eExtraInfoHeader();

		EGUI::PopLeftAlign();
	}

	void RigidBody::eBodyTypeDropDown()
	{
		int i_type = mPhysicsType;
		AutoArray<std::string> arr{ std::string{" Dynamic"},
									std::string{" Kinematic"},
									std::string{" Static"} };
		if (EGUI::Display::DropDownSelection("Body Type", i_type, arr))
		{
			switch (i_type)
			{
			case 0: // Dynamic
				mPhysicsType = eDynamicBody;
				mbIsAwake = true;
				mbIsStatic = false;
				if (mfMass <= FLT_EPSILON) mfMass = 10.0f;
				LoggerSystem::ConsoleLog(eLog::MESSAGE, "Body is now Dynamic, Awake and not Static.");
				break;
			case 1: // Kinematic
				LoggerSystem::ConsoleLog(eLog::WARNING, "Kinematic is not implemented yet. Switching to Dynamic");
				i_type = 0;
				mPhysicsType = eDynamicBody;
				mbIsAwake = true;
				mbIsStatic = false;
				if (mfMass <= FLT_EPSILON) mfMass = 10.0f;
				LoggerSystem::ConsoleLog(eLog::MESSAGE, "Body is now Dynamic, Awake and not Static.");
				break;
			case 2: // Static
				mPhysicsType = eStaticBody;
				mbIsAwake = false;
				mbIsStatic = true;
				mfMass = 0.0f;
				CheckMass();
				LoggerSystem::ConsoleLog(eLog::MESSAGE, "Body is now Static, and not Awake.");
				break;
			default:
				break;
			}
		}
	}

	void RigidBody::eMassField()
	{
		if (mPhysicsType != eStaticBody)
		{
			switch (EGUI::Display::DragFloat("Mass", &mfMass, 0.05f, 0.01F, FLT_MAX))
			{
			case EGUI::eDragStatus::eSTART_DRAG:
				//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &RigidBody::mfMass);
				break;
			case EGUI::eDragStatus::eTABBED:
			case EGUI::eDragStatus::eDEACTIVATED:
			case EGUI::eDragStatus::eEND_DRAG:
			case EGUI::eDragStatus::eENTER:
				//EGUI::GetCommandHND()->EndRecording();
				CheckMass();
				break;
			default:
			case EGUI::eDragStatus::eNO_CHANGE:
			case EGUI::eDragStatus::eDRAGGING:
				break;
			}
		}

	}

	void RigidBody::eLinearDragField()
	{
		if (mPhysicsType != eStaticBody)
		{
			auto arrResult = EGUI::Display::VectorFields("Linear Drag", &mLinearDamping, 0.01f, 0.0F, 0.99999F);
			for (auto &e : arrResult)
			{
				switch (e)
				{
				case EGUI::eDragStatus::eDRAGGING:
				case EGUI::eDragStatus::eNO_CHANGE:
					break;
				case EGUI::eDragStatus::eSTART_DRAG:
					//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &RigidBody::mLinearDamping);
					break;
				case EGUI::eDragStatus::eEND_DRAG:
				case EGUI::eDragStatus::eENTER:
				case EGUI::eDragStatus::eDEACTIVATED:
				case EGUI::eDragStatus::eTABBED:
					//EGUI::GetCommandHND()->EndRecording();
					break;
				}
			}
		}

	}

	void RigidBody::eAngularDragField()
	{
		if (mPhysicsType != eStaticBody)
		{
			switch (EGUI::Display::DragFloat("Angular Drag", &mfAngularDrag, 0.01f, 0.0F, 1.0F))
			{
			case EGUI::eDragStatus::eTABBED:
			case EGUI::eDragStatus::eEND_DRAG:
			case EGUI::eDragStatus::eENTER:
			case EGUI::eDragStatus::eDEACTIVATED:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &RigidBody::mfAngularDrag);
				break;
			default:
			case EGUI::eDragStatus::eDRAGGING:
			case EGUI::eDragStatus::eNO_CHANGE:
				break;
			}
		}

	}

	void RigidBody::eGravityScaleField()
	{
		if (mPhysicsType != eStaticBody)
		{
			switch (EGUI::Display::DragFloat("Gravity Scale", &mfGravityScale, 0.01f, -FLT_MAX, FLT_MAX))
			{
			case EGUI::eDragStatus::eTABBED:
			case EGUI::eDragStatus::eDEACTIVATED:
			case EGUI::eDragStatus::eEND_DRAG:
			case EGUI::eDragStatus::eENTER:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &RigidBody::mfGravityScale);
				break;
			default:
			case EGUI::eDragStatus::eDRAGGING:
			case EGUI::eDragStatus::eNO_CHANGE:
				break;
			}
		}

	}

	void RigidBody::eStaticFrictionDragField()
	{
		switch (EGUI::Display::DragFloat("Static Friction", &mfStaticFriction, 0.01f, 0.0f, 2.0f))
		{
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eTABBED:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &RigidBody::mfStaticFriction);
			break;
		default:
		case EGUI::eDragStatus::eNO_CHANGE:
		case EGUI::eDragStatus::eDRAGGING:
			break;
		}
	}

	void RigidBody::eDynamicFrictionDragField()
	{
		switch (EGUI::Display::DragFloat("Dynamic Friction", &mfDynamicFriction, 0.01f, 0.0f, 2.0f))
		{
		case EGUI::eDragStatus::eENTER:
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eTABBED:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &RigidBody::mfDynamicFriction);
			break;
		default:
		case EGUI::eDragStatus::eNO_CHANGE:
		case EGUI::eDragStatus::eDRAGGING:
			break;
		}
	}

	void RigidBody::eRestitutionDragField()
	{
		switch (EGUI::Display::DragFloat("Bounciness", &mfRestitution, 0.01f, 0.0f, 2.0f))
		{
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eTABBED:
		case EGUI::eDragStatus::eDEACTIVATED:
		case EGUI::eDragStatus::eENTER:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &RigidBody::mfRestitution);
			break;
		default:
		case EGUI::eDragStatus::eNO_CHANGE:
		case EGUI::eDragStatus::eDRAGGING:
			break;
		}
	}

	void RigidBody::eCollisionDetectionField()
	{
		int i_type = 0;
		AutoArray<std::string> arr{ std::string{ " Discrete" },
			std::string{ " Continuous" } };
		if (EGUI::Display::DropDownSelection("Collision Detection", i_type, arr))
		{
			switch (i_type)
			{
			case 0: // Dynamic
				LoggerSystem::ConsoleLog(eLog::MESSAGE, "Body will now check Discrete Collision.");
				break;
			case 1: // Kinematic
				LoggerSystem::ConsoleLog(eLog::WARNING, "Continuous is not implemented yet. Switching to Discrete");
				i_type = 0;
				LoggerSystem::ConsoleLog(eLog::MESSAGE, "Body will now check Discrete Collision.");
				break;
			case 2: // Static

				break;
			default:
				break;
			}
		}
	}

	void RigidBody::eSleepingModeDropdown()
	{
		if (mPhysicsType != eStaticBody)
		{
			int i_type = 0;
			AutoArray<std::string> arr{ std::string{ " Never Sleep" },
										std::string{ " Start Awake" },
										std::string{ " Start Asleep" } 
									};
			if (EGUI::Display::DropDownSelection("Sleeping Mode", i_type, arr))
			{
				switch (i_type)
				{
				case 0: // Dynamic
					LoggerSystem::ConsoleLog(eLog::WARNING, "Not implemented yet.");
					break;
				case 1: // Kinematic
					LoggerSystem::ConsoleLog(eLog::WARNING, "Not implemented yet.");
					break;
				case 2: // Static
					LoggerSystem::ConsoleLog(eLog::WARNING, "Not implemented yet.");
					break;
				default:
					break;
				}
			}
		}
	}

	void RigidBody::eRotationConstraintCheckBox()
	{
		static bool toggleState = mbFixedRot;
		if (EGUI::Display::CheckBox("Freeze Rotation", &toggleState))
		{
			mbFixedRot = toggleState;
			mfAngleDegZ = 0;
		}
	}

	void RigidBody::eExtraInfoHeader()
	{
		if (EGUI::Display::CollapsingHeader("Info"))
		{
			EGUI::Display::Label("Position     : x[%.2f], y[%.2f], z[%.2f]",
				float(mPosition.x),
				float(mPosition.y),
				float(mPosition.z));
			EGUI::Display::Label("Prev Position: x[%.2f], y[%.2f], z[%.2f]",
				float(mPrevPosition.x),
				float(mPrevPosition.y),
				float(mPrevPosition.z));
			EGUI::Display::Label("Angle Degree : %.2f", mfAngleDegZ);
			EGUI::Display::Label("Linear Vel.  : x[%.2f], y[%.2f], z[%.2f]",
				float(mLinearVelocity.x),
				float(mLinearVelocity.y),
				float(mLinearVelocity.z));
			EGUI::Display::Label("Angular Vel. : x[%.2f], y[%.2f], z[%.2f]",
				float(mAngularVelocity.x),
				float(mAngularVelocity.y),
				float(mAngularVelocity.z));
			std::string show = "Is Awake ";
			show += (mbIsAwake) ? "true" : "false";
			EGUI::Display::Label(show.c_str());
		}

	}
#endif
}
