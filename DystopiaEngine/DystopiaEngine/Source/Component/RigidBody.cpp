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
#include "System/Scene/SceneSystem.h"
#include "System/Scene/Scene.h"
#if EDITOR
#include "IO/TextSerialiser.h"
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#endif 

#include <cmath>

#define P_TX					mpOwnerTransform

namespace Dystopia
{ 
	RigidBody::RigidBody(void)
		: mpPrimitiveShape(nullptr)
		, mpOwnerTransform(nullptr)
		, mpPhysSys(nullptr)
		, mfAngleDeg(0.0F)
		, mfLinearDamping(0.5F)
		, mfAngularDrag(0.5F)
		, mfStaticFriction(0.5F)
		, mfDynamicFriction(0.7F)
		, mfRestitution(0.5F)
		, mfGravityScale(1.0F)
		, mfMass(100.0F)
		, mfInvMass(0.01F)
		, mfWeightedMotion(0.0F)
		, mOwnerIsActive(true)
		, mbHasGravity(true)
		, mbIsStatic(false)
		, mbIsAwake(true)
		, mbCanSleep(true)
		, mPhysicsType(t_dynamic)
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
		, mfStaticFriction(_friction)
		, mfRestitution(_elasticity)
		, mfGravityScale(_gravityScale)
		, mfMass(_mass)
		, mfInvMass(1/_mass)
		, mfWeightedMotion(0.0F)
		, mOwnerIsActive(true)
		, mbHasGravity(_gravityState)
		, mbIsStatic(_staticState)
		, mbIsAwake(true)
		, mbCanSleep(true)
		, mPhysicsType(t_dynamic)
	{

	}

	void RigidBody::Load(void)
	{
	}

	void RigidBody::Init(void)
	{
		// Get Owner's Transform Component as pointer
		if (nullptr == mpOwnerTransform && nullptr != GetOwner())
		{
			mpPhysSys = EngineCore::GetInstance()->GetSystem<PhysicsSystem>();
			mpOwnerTransform = GetOwner()->GetComponent<Transform>();
			mOwnerIsActive = GetOwner()->IsActive();
			mPosition = mpOwnerTransform->GetGlobalPosition();

			mInverseOrientation = Math::Inverse(mOrientation);
		}

		// If mass is zero, object is interpreted to be static
		if (mfMass > 0.0F)
		{
			mfInvMass = 1.0F / mfMass;
		}
		else //if mfMass <= 0
		{
			if (!mbIsStatic)
			{
				mbIsStatic = true;
				mPhysicsType = t_static;
			}
			mfInvMass = 0.0F;
		}

		// compute final local centroid
		//mLocalCentroid *= mfInvMass;

		// If Static, then is Sleeping
		if (mbIsStatic) {
			mbIsAwake = false;
			mPhysicsType = t_static;
		}
		else
		{
			mbIsAwake = true;
			mPhysicsType = t_dynamic;
		}

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
		constexpr const auto VEL_EPSILON = 0.001F;

		if (!GetOwner()->IsActive() || mbIsStatic || !mbIsAwake)
		{
			return; // don't integrate when body is static
		}

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

		//Integrate the velocity
		mLinearVelocity += mAcceleration * _dt;

		const Vec3D new_accel = mCumulativeForce * mfInvMass + mAcceleration;

		//Integrate the velocity
		mLinearVelocity += (new_accel - mAcceleration) * 0.5f * _dt;

		// Linear Damping (Drag)
		mLinearVelocity *= std::pow(mfLinearDamping, _dt);

		// Clamp to zero
		if (std::abs(mLinearVelocity.x) < VEL_EPSILON)
		{
			mLinearVelocity.x = 0;
		}

		if (std::abs(mLinearVelocity.y) < VEL_EPSILON)
		{
			mLinearVelocity.y = 0;
		}

		//Clamp to velocity max for numerical stability
		if (Dot(mLinearVelocity, mLinearVelocity) > mpPhysSys->mMaxVelSquared)
		{
			mLinearVelocity = Math::Normalise(mLinearVelocity);
			mLinearVelocity *= mpPhysSys->mMaxVelocityConstant;
		}

		//*Reset Cumulative Force*/
		ResetCumulative();
		
		
	}

	void RigidBody::CheckSleeping(float _dt)
	{
		constexpr const auto SLEEP_EPSILON = 0.05F;

		const float bias = std::pow(0.1F, _dt);
		const auto currentMotion = mLinearVelocity.MagnitudeSqr() + mAngularVelocity.MagnitudeSqr();
		mfWeightedMotion = bias * mfWeightedMotion + (1 - bias)*currentMotion;

		// TODO change to global sleep epsilon
		if (mfWeightedMotion > 10 * SLEEP_EPSILON) 
			mfWeightedMotion = 10 * SLEEP_EPSILON;

		if (mfWeightedMotion < SLEEP_EPSILON)
		{
			const auto t = Math::Abs(mPosition.Magnitude() - mPrevPosition.Magnitude());
			if (t < 0.05f)
			{
				mbIsAwake = false;
			}
		}
	}

	void RigidBody::UpdateResult(float _dt)
	{
		if (!mbIsStatic && mbIsAwake) // only update when body is not static
		{		
			// Update Position
			mPosition += (mLinearVelocity + mAcceleration * _dt * 0.5F) * _dt;
			P_TX->SetGlobalPosition(mPosition);
		}
	}

	/*void RigidBody::Update(float _dt)
	{
		
	}*/

	/*void RigidBody::LateUpdate(float _dt)
	{
		
	}*/

	void RigidBody::Unload(void)
	{
		mpPrimitiveShape = nullptr;
		mpOwnerTransform = nullptr;
		mpPhysSys		 = nullptr;
	}

	RigidBody * RigidBody::Duplicate() const
	{
		return EngineCore::GetInstance()->GetSystem<PhysicsSystem>()->RequestComponent(*this);
	}

	void RigidBody::Serialise(TextSerialiser & _out) const
	{
		_out.InsertStartBlock("RigidBody");
		_out << mID;					// gObjID
		_out << mfAngleDeg;				// Angle in degrees
		_out << mfLinearDamping;		// Linear Drag
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
		//_out << int(mPhysicsType);		// enum for physicstype
		_out.InsertEndBlock("RigidBody");
	}

	void RigidBody::Unserialise(TextSerialiser & _in)
	{
		//int physicsType;
		_in.ConsumeStartBlock();
		_in >> mID;						// gObjID
		_in >> mfAngleDeg;				// Angle in degrees
		_in >> mfLinearDamping;			// Linear Drag
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
		//_in >> physicsType;				// enum for physicstype
		_in.ConsumeEndBlock();

		//mPhysicsType = static_cast<PhysicsType>(physicsType);

		if (GameObject* owner =
			EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().FindGameObject(mID))
		{
			owner->AddComponent(this, RigidBody::TAG{});
			Init();
		}
	}

	void RigidBody::DebugPrint()
	{
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
		/*Vector from origin/pivot of the object to the _point where the force is applied*/
		Math::Vec3D && displacement_v = _point - _origin;
		/*Generate the angular velocity as a result of the force being applied at _point*/
		const Math::Vector3D angularVel = Cross(_force, displacement_v) / displacement_v.MagnitudeSqr();
		/*Add to the total Angular velocity of the object in this Update Frame*/
		mAngularVelocity += angularVel;
		/*Add the the total Linear Veloctiy of the object in this Update Frame*/
		mCumulativeForce += _force * 5000.0F;
		mCumulativeTorque += (_point - mGlobalCentroid).Cross(_force);
		mbIsAwake = true;
	}

	void RigidBody::AddForce(Math::Vec3D const & _force)
	{
		mCumulativeForce += _force * 5000.0F;
		mbIsAwake = true;
	}

	void RigidBody::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point)
	{
		AddForce(_force, _point, P_TX->GetGlobalPosition());
	}

	void RigidBody::AddImpulse(Vec3D const& _impul)
	{
		mCumulativeForce = _impul * 5000.0F;
		mbIsAwake = true;
	}

	void RigidBody::AddImpulse(Vec3D const & _impul, Point3D const & _point)
	{
		AddForce(_impul, _point, P_TX->GetGlobalPosition());

	}

	void RigidBody::AddImpulse(Vec3D const & _impul, Point3D const & _point, Point3D const &)
	{
		/*Add the the total Linear Veloctiy of the object in this Update Frame*/
		mCumulativeForce = _impul * 5000.0F;
		mCumulativeTorque = (_point - mGlobalCentroid).Cross(_impul);
		mbIsAwake = true;
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

	void RigidBody::SetStaticFriction(const float _f)
	{
		if (_f > 1.0F) mfStaticFriction = 1.0F;
		else if (_f < 0.0) mfStaticFriction = 0.0F;
		else mfStaticFriction = _f;
	}

	void RigidBody::SetKineticFriction(const float _f)
	{
		if (_f > 1.0F) mfDynamicFriction = 1.0F;
		else if (_f < 0.0) mfDynamicFriction = 0.0F;
		else mfDynamicFriction = _f;
	}

	void RigidBody::SetRestitution(const float _f)
	{
		if (_f > 1.0F) mfRestitution = 1.0F;
		else if (_f < 0.0) mfRestitution = 0.0F;
		else mfRestitution = _f;
	}

	void RigidBody::SetSleeping(const bool _b)
	{
		mbIsAwake = !_b;
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

	/****************************************************************
	* EDITOR GUI STUFF
	****************************************************************/
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
			mPhysicsType = t_static;
		}
	}

	void RigidBody::EditorUI() noexcept
	{
		eBodyTypeDropDown();
		eMassField();
		eLinearDragField();
		eAngularDragField();
		eGravityScaleField();
		eStaticFrictionDragField();
		eDynamicFrictionDragField();
		eRestitutionDragField();

		eCollisionDetectionField();
		eSleepingModeDropdown();
		eRotationConstraintCheckBox();
		
		eExtraInfoHeader();
	}

	void RigidBody::eBodyTypeDropDown()
	{
		int i_type = mPhysicsType;
		AutoArray<std::string> arr{ std::string{" Dynamic"}, 
									std::string{" Kinematic"}, 
									std::string{" Static"} };
		if (EGUI::Display::DropDownSelection("Body Type				", i_type, arr))
		{
			switch (i_type)
			{
				case 0: // Dynamic
					mPhysicsType = t_dynamic;
					mbIsAwake = true;
					mbIsStatic = false;
					if (mfMass <= FLT_EPSILON) mfMass = 10.0f;
					LoggerSystem::ConsoleLog(eLog::MESSAGE, "Body is now Dynamic, Awake and not Static.");
					break;
				case 1: // Kinematic
					LoggerSystem::ConsoleLog(eLog::WARNING, "Kinematic is not implemented yet. Switching to Dynamic");
					i_type = 0;
					mPhysicsType = t_dynamic;
					mbIsAwake = true;
					mbIsStatic = false;
					if (mfMass <= FLT_EPSILON) mfMass = 10.0f;
					LoggerSystem::ConsoleLog(eLog::MESSAGE, "Body is now Dynamic, Awake and not Static.");
					break;
				case 2: // Static
					mPhysicsType = t_static;
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
		if (mPhysicsType != t_static)
		{
			switch (EGUI::Display::DragFloat("Mass					", &mfMass, 0.05f, 0.01F, FLT_MAX))
			{
			case EGUI::eDragStatus::eEND_DRAG:
				EGUI::GetCommandHND()->EndRecording();
				CheckMass();
				break;
			case EGUI::eDragStatus::eENTER:
				EGUI::GetCommandHND()->EndRecording();
				CheckMass();
				break;
			case EGUI::eDragStatus::eDRAGGING:
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				EGUI::GetCommandHND()->StartRecording<RigidBody>(GetOwner()->GetID(), &mfMass);
				break;
			case EGUI::eDragStatus::eDEACTIVATED:
				EGUI::GetCommandHND()->EndRecording();
				CheckMass();
				break;
			case EGUI::eDragStatus::eNO_CHANGE:
				break;
			case EGUI::eDragStatus::eTABBED:
				EGUI::GetCommandHND()->EndRecording();
				CheckMass();
				break;
			default:
				break;
			}
		}
		
	}

	void RigidBody::eLinearDragField()
	{
		switch (EGUI::Display::DragFloat("Linear Drag			 ", &mfLinearDamping, 0.01f, FLT_EPSILON, 1.0F))
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eDRAGGING:
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<RigidBody>(GetOwner()->GetID(), &mfLinearDamping);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eNO_CHANGE:
			break;
		case EGUI::eDragStatus::eTABBED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default:
			break;
		}
	}

	void RigidBody::eAngularDragField()
	{
		switch (EGUI::Display::DragFloat("Angular Drag			", &mfAngularDrag, 0.01f, FLT_EPSILON, 1.0F))
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eDRAGGING:
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<RigidBody>(GetOwner()->GetID(), &mfAngularDrag);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eNO_CHANGE:
			break;
		case EGUI::eDragStatus::eTABBED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default:
			break;
		}
	}

	void RigidBody::eGravityScaleField()
	{
		switch (EGUI::Display::DragFloat("Gravity Scale		   ", &mfGravityScale, 0.01f, -FLT_MAX, FLT_MAX))
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eDRAGGING:
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<RigidBody>(GetOwner()->GetID(), &mfGravityScale);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eNO_CHANGE:
			break;
		case EGUI::eDragStatus::eTABBED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default:
			break;
		}
	}

	void RigidBody::eStaticFrictionDragField()
	{
		switch (EGUI::Display::DragFloat("Static Friction		 ", &mfStaticFriction, 0.01f, 0.0f, 1.0f))
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eDRAGGING:
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<RigidBody>(GetOwner()->GetID(), &mfStaticFriction);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eNO_CHANGE:
			break;
		case EGUI::eDragStatus::eTABBED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default:
			break;
		}
	}

	void RigidBody::eDynamicFrictionDragField()
	{
		switch (EGUI::Display::DragFloat("Dynamic Friction		", &mfDynamicFriction, 0.01f, 0.0f, 1.0f))
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eDRAGGING:
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<RigidBody>(GetOwner()->GetID(), &mfDynamicFriction);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eNO_CHANGE:
			break;
		case EGUI::eDragStatus::eTABBED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default:
			break;
		}
	}

	void RigidBody::eRestitutionDragField()
	{
		switch (EGUI::Display::DragFloat("Bounciness			  ", &mfRestitution, 0.01f, 0.0f, 1.0f))
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eDRAGGING:
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<RigidBody>(GetOwner()->GetID(), &mfRestitution);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eNO_CHANGE:
			break;
		case EGUI::eDragStatus::eTABBED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default:
			break;
		}
	}

	void RigidBody::eCollisionDetectionField()
	{
		int i_type = 0;
		AutoArray<std::string> arr{ std::string{ " Discrete" },
			std::string{ " Continuous" } };    
		if (EGUI::Display::DropDownSelection("Collision Detection	  ", i_type, arr))
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
		int i_type = 0;
		AutoArray<std::string> arr{ std::string{ " Never Sleep" },
									std::string{ " Start Awake" },
									std::string{ " Start Asleep"}};
		if (EGUI::Display::DropDownSelection("Sleeping Mode			", i_type, arr))
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

	void RigidBody::eRotationConstraintCheckBox()
	{
		static bool toggleState = false;
		if (EGUI::Display::CheckBox("Freeze Rotation		  ", &toggleState))
		{
			// Freeze Z Rotation Function here
			if (toggleState)
			{
				//EGUI::Display::IconTick();
			}
			else
			{
				
			}
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
			EGUI::Display::Label("Angle Degree : %.2f",	mfAngleDeg);
			EGUI::Display::Label("Linear Vel.  : x[%.2f], y[%.2f], z[%.2f]",
				float(mLinearVelocity.x),
				float(mLinearVelocity.y),
				float(mLinearVelocity.z));
			EGUI::Display::Label("Angular Vel. : x[%.2f], y[%.2f], z[%.2f]",
				float(mAngularVelocity.x),
				float(mAngularVelocity.y),
				float(mAngularVelocity.z));
			std::string show = "Is Awake     :  ";
			show += (mbIsAwake) ? "true" : "false";
			EGUI::Display::Label(show.c_str());
		}
		
	}

}
