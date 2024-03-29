/* HEADER *********************************************************************************/
/*!
\file	CollisionEvent.cpp
\author Goh Kai Jie Keith (50%)
        Aaron Chu (50%)
\par    email: keith.goh\@digipen.edu
               m.chu\@digipen.edu
\brief


All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Collision/CollisionEvent.h"
#include "Object/GameObject.h"
#include "Component/RigidBody.h"
#include "Component/Collider.h"

namespace Dystopia
{
	CollisionEvent::CollisionEvent(GameObject* _Self, GameObject * _Target)
		: mThisCollider{ _Self }
		, mCollidedWith{ _Target }
		, mfPeneDepth(0.0F)
		, mfRestitution(0.0F)
		, mfStaticFrictionCof(0.5F)
		, mfDynamicFrictionCof(0.7F)
		, mOtherID{ _Target->GetID() }
		, isATrigger{ false }
		, isBTrigger{ false }
	{

	}
	CollisionEvent::CollisionEvent()
		: mThisCollider{ nullptr }
		, mCollidedWith{ nullptr }
		, mfPeneDepth(0.0F)
		, mfRestitution(0.0F)
		, mfStaticFrictionCof(0.5F)
		, mfDynamicFrictionCof(0.7F)
		, mOtherID{ 0 }
		, isATrigger{ false }
		, isBTrigger{ false }
	{

	}

	void CollisionEvent::ApplyImpulse()
	{
		//if (mCollidedWith->GetName() == "Game Object_clone_clone_clone") __debugbreak();
		if (isATrigger || isBTrigger) return;

		constexpr auto velLimit = 50.0F;
		const auto bodyA = mThisCollider->GetComponent<RigidBody>();
		const auto bodyB = mCollidedWith->GetComponent<RigidBody>();

		if (nullptr == bodyA || nullptr == bodyB) return;


		const auto a_invmass = bodyA->GetInverseMass();
		const auto b_invmass = bodyB->GetInverseMass();
		const auto a_oldVel = bodyA->GetLinearVelocity();// -bodyA->GetAcceleration() * 0.5F * _dt * _dt;
		const auto b_oldVel = bodyB->GetLinearVelocity();// -bodyB->GetAcceleration() * 0.5F * _dt * _dt;

		mEdgeNormal.z = 0;
		mfPeneDepth = Math::Abs(mfPeneDepth);

		Math::Vec3D rv = b_oldVel - a_oldVel;

		const float contactVel = rv.Dot(mEdgeNormal);

		// Do not resolve if velocities do not meet up
		if (contactVel > 0) return;

		auto thisRestitution = mfRestitution;
		if (Math::Abs(contactVel) < velLimit)
		{
			thisRestitution = 0.0F;
		}

		// Calculate Impulse Scalar
		float tmpJ = -(1.0F + thisRestitution) * contactVel;
		//float tmpJ = -(bias / _dt * mfPeneDepth + mfRestitution) * contactVel;
		tmpJ /= a_invmass + b_invmass;

		if (Math::Abs(tmpJ) < 0.0005F) tmpJ = 0.0F;

		// Apply Impulse
		const Vec3D impulse = tmpJ * mEdgeNormal;

		if (!bodyA->Get_IsStaticState() && !bodyA->Get_IsKinematic())
			bodyA->AddLinearImpulse(-impulse);
			//bodyA->AddLinearImpulseWithOrigin(-impulse, mCollisionPoint);

		if (!bodyB->Get_IsStaticState() && mCollidedWith->IsActive() && !bodyB->Get_IsKinematic())
			bodyB->AddLinearImpulse(impulse);
			//bodyB->AddLinearImpulseWithOrigin(-impulse, mCollisionPoint);

		// Calculate Frictional Velocity (vec3D) after normal impulse
		rv = bodyB->GetLinearVelocity() - bodyA->GetLinearVelocity();

		// Solving for tangent vector
		Vec3D tangent = rv - rv.Dot(mEdgeNormal) * mEdgeNormal;

		if (tangent.MagnitudeSqr() < FLT_EPSILON)
			return;

		tangent = Normalise(tangent);

		// Solve for magnitude to apply along the friction vector
		float jt = -rv.Dot(tangent);
		jt /= a_invmass + b_invmass;

		if (Math::Abs(jt) < 0.0005F) jt = 0.0F;

		// Clamp magnitude of friction and create final Friction Impulse
		Vec3D frictionImpulse;

		if (Math::Abs(jt) < tmpJ * mfStaticFrictionCof)
			frictionImpulse = jt * tangent;
		else
			frictionImpulse = -tmpJ * tangent * mfDynamicFrictionCof;

		if (!bodyA->Get_IsStaticState() && !bodyA->Get_IsKinematic())
			bodyA->AddLinearImpulse(-frictionImpulse);

		if (!bodyB->Get_IsStaticState() && mCollidedWith->IsActive() && !bodyB->Get_IsKinematic())
			bodyB->AddLinearImpulse(frictionImpulse);

		/*if (!bodyA->Get_IsStaticState())
			bodyA->AddLinearImpulseWithOrigin(-frictionImpulse, mCollisionPoint);

		if (!bodyB->Get_IsStaticState() && !colB->IsTrigger())
			bodyB->AddLinearImpulseWithOrigin(frictionImpulse, mCollisionPoint);*/
	}

	void CollisionEvent::ApplyPenetrationCorrection(const int iter) const
	{
	//	if (mCollidedWith->GetName() == "Game Object_clone_clone_clone") __debugbreak();
		if (isATrigger || isBTrigger) return;

		const auto bodyA = mThisCollider->GetComponent<RigidBody>();
		const auto bodyB = mCollidedWith->GetComponent<RigidBody>();

		if (nullptr == bodyA || nullptr == bodyB) return;


		const auto a_invmass = bodyA->GetInverseMass();
		const auto b_invmass = bodyB->GetInverseMass();

		const float perc = 0.48F / iter;
		const float slop = 0.01F;

		const Vec3D correction = Math::Max(Math::Abs(mfPeneDepth) - slop, 0.0F) / (a_invmass + b_invmass) * perc * mEdgeNormal;

		if (!bodyA->Get_IsStaticState() && bodyA->GetIsAwake() && !bodyA->Get_IsKinematic())
			bodyA->SetPosition(bodyA->GetPosition() - correction * a_invmass);
		if (!bodyB->Get_IsStaticState() && bodyB->GetIsAwake() && mCollidedWith->IsActive() && !bodyB->Get_IsKinematic())
			bodyB->SetPosition(bodyB->GetPosition() + correction * b_invmass);

		//if (bodyA->GetPosition().x == 0.f || bodyB->GetPosition().x == 0.f) __debugbreak();
	}

	bool CollisionEvent::operator==(CollisionEvent const & _rhs) const
	{
		return (operator==(_rhs.mCollidedWith) && operator==(_rhs.mOtherID));
	}

	bool CollisionEvent::operator==(unsigned long long _mCollidedWithID) const
	{
		return mOtherID == _mCollidedWithID;
	}

	bool CollisionEvent::operator==(mpcGobj _pCollidedWith) const
	{
		return mCollidedWith == _pCollidedWith;
	}

	bool CollisionEvent::operator==(const GameObject * const _pCollidedWith) const
	{
		return mCollidedWith == _pCollidedWith;
	}

}

