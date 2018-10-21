#include "System/Collision/CollisionEvent.h"
#include "Object/GameObject.h"
#include "Component/RigidBody.h"

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
	{

	}

	void CollisionEvent::ApplyImpulse(void)
	{
		const auto bodyA = mThisCollider->GetComponent<RigidBody>();
		const auto bodyB = mCollidedWith->GetComponent<RigidBody>();

		if (nullptr == bodyA || nullptr == bodyB) return;

		const auto a_invmass = bodyA->GetInverseMass();
		const auto b_invmass = bodyB->GetInverseMass();
		const auto a_oldVel = bodyA->GetLinearVelocity();
		const auto b_oldVel = bodyB->GetLinearVelocity();
		const auto perc = 2.0F;

		mEdgeNormal.z = 0;
		mfPeneDepth = Math::Abs(mfPeneDepth);

		Math::Vec3D rv = b_oldVel - a_oldVel;

		const float contactVel = rv.Dot(mEdgeNormal);

		// Do not resolve if velocities do not meet up
		if (contactVel > 0) return;

		// Calculate Impulse Scalar
		float tmpJ = -(1.0F + mfRestitution) * contactVel + perc*mfPeneDepth;
		tmpJ /= a_invmass + b_invmass;

		// Apply Impulse
		const Vec3D impulse = tmpJ * mEdgeNormal;

		if (bodyA->GetIsAwake() && !bodyA->Get_IsStaticState())
			bodyA->AddLinearImpulse(-impulse);

		if (bodyB->GetIsAwake() && !bodyB->Get_IsStaticState())
			bodyB->AddLinearImpulse(impulse);

		// Calculate Frictional Velocity (vec3D) after normal impulse
		rv = bodyB->GetLinearVelocity() - bodyA->GetLinearVelocity();

		// Solving for tangent vector
		Vec3D tangent = rv - rv.Dot(mEdgeNormal) * mEdgeNormal;

		if (tangent.MagnitudeSqr() < FLT_EPSILON)
			return;

		tangent = Normalise(tangent);

		// Solve for magnitude to apply along the friction vector
		float jt = -rv.Dot(tangent);
		jt /= (a_invmass + b_invmass);

		// Clamp magnitude of friction and create final Friction Impulse
		Vec3D frictionImpulse;

		if (Math::Abs(jt) < tmpJ * mfStaticFrictionCof)
			frictionImpulse = jt * tangent;
		else
			frictionImpulse = -tmpJ * tangent * mfDynamicFrictionCof;

	if (bodyA->GetIsAwake() && !bodyA->Get_IsStaticState())
		bodyA->AddLinearImpulse(-frictionImpulse);
		

	if (bodyB->GetIsAwake() && !bodyB->Get_IsStaticState())
		bodyB->AddLinearImpulse(frictionImpulse);
	}

	void CollisionEvent::ApplyPenetrationCorrection(const int iter) const
	{
		const auto bodyA = mThisCollider->GetComponent<RigidBody>();
		const auto bodyB = mCollidedWith->GetComponent<RigidBody>();

		if (nullptr == bodyA || nullptr == bodyB) return;

		const auto a_invmass = bodyA->GetInverseMass();
		const auto b_invmass = bodyB->GetInverseMass();

		const float perc = 0.40F/iter;
		const float slop = 0.01F;

		const Vec3D correction = Math::Max((mfPeneDepth)+slop, 0.0F) / (a_invmass + b_invmass) * perc * mEdgeNormal;

		if (bodyA->GetIsAwake() && !bodyA->Get_IsStaticState())
			bodyA->SetPosition(bodyA->GetPosition() - correction * a_invmass);
		if (bodyB->GetIsAwake() && !bodyB->Get_IsStaticState())
			bodyB->SetPosition(bodyB->GetPosition() + correction * b_invmass);
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

}

