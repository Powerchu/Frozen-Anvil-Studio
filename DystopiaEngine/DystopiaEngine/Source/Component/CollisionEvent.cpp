#include "System/Collision/CollisionEvent.h"
#include "Object/GameObject.h"
#include "Component/RigidBody.h"
#include <algorithm>

Dystopia::CollisionEvent::CollisionEvent(GameObject* _Self, GameObject * _Target)
	: mdPeneDepth(0.0F)
	, mThisCollider{ _Self }
	, mCollidedWith{ _Target }
	, mfRestitution(0.0F)
	, mfStaticFrictionCof(0.5F)
	, mfDynamicFrictionCof(0.7F)
{

}

void Dystopia::CollisionEvent::ApplyImpulse(void)
{
	const auto bodyA = mThisCollider->GetComponent<RigidBody>();
	const auto bodyB = mCollidedWith->GetComponent<RigidBody>();
	const auto a_invmass = bodyA->GetInverseMass();
	const auto b_invmass = bodyB->GetInverseMass();
	const auto a_oldVel = bodyA->GetLinearVelocity();
	const auto b_oldVel = bodyB->GetLinearVelocity();

	Math::Vec3D rv = b_oldVel - a_oldVel;
					 
	const float contactVel = rv.Dot(mEdgeNormal);

	// Do not resolve if velocities do not meet up
	if (contactVel > 0) return;

	for (int i = 0; i < 8; ++i)
	{
		// Calculate Impulse Scalar
		float tmpJ = -(2.0F + mfRestitution) * contactVel;
		tmpJ /= bodyA->GetInverseMass() + bodyB->GetInverseMass();

		// Apply Impulse
		Vec3D impulse = tmpJ * mEdgeNormal;

		const auto a_newVel = a_oldVel - impulse * a_invmass;
		const auto b_newVel = b_oldVel + impulse * b_invmass;

		if (!bodyA->Get_IsStaticState() && (a_newVel - a_oldVel).MagnitudeSqr() > 0.01F)
			bodyA->SetVelocity(a_newVel);

		if (!bodyB->Get_IsStaticState() && (b_newVel - b_oldVel).MagnitudeSqr() > 0.01F)
			bodyB->SetVelocity(b_newVel);

		// Calculate Frictional Velocity (vec3D) after normal impulse
		rv = b_newVel - a_newVel;

		// Solving for tangent vector
		Vec3D tangent = rv - rv.Dot(mEdgeNormal) * mEdgeNormal;

		if (tangent.MagnitudeSqr() > FLT_EPSILON)
		{
			tangent = Normalise(tangent);
		}

		// Solve for magnitude to apply along the friction vector
		float jt = -rv.Dot(tangent);
		jt /= (a_invmass + b_invmass);

		// Clamp magnitude of friction and create final Friction Impulse
		Vec3D frictionImpulse;

		if (Math::Abs(jt) < tmpJ * mfStaticFrictionCof)
		{
			frictionImpulse = jt * tangent;
		}
		else
		{
			frictionImpulse = -tmpJ * tangent * mfDynamicFrictionCof;
		}

		if (!bodyA->Get_IsStaticState())
			bodyA->SetVelocity(bodyA->GetLinearVelocity() - frictionImpulse * a_invmass);

		if (!bodyB->Get_IsStaticState())
			bodyB->SetVelocity(bodyB->GetLinearVelocity() + frictionImpulse * b_invmass);
	}
}

void Dystopia::CollisionEvent::ApplyPenetrationCorrection()
{
	const auto bodyA = mThisCollider->GetComponent<RigidBody>();
	const auto bodyB = mCollidedWith->GetComponent<RigidBody>();
	const auto a_invmass = bodyA->GetInverseMass();
	const auto b_invmass = bodyB->GetInverseMass();

	const float perc = 0.3F;
	const float slop = 0.01F;
	const Vec3D correction = std::max(float(mdPeneDepth) - slop, 0.0F) / (a_invmass + b_invmass) * perc * mEdgeNormal;

	if (!bodyA->Get_IsStaticState())
		bodyA->SetPosition(bodyA->GetPosition() - correction * a_invmass);
	if (!bodyB->Get_IsStaticState())
		bodyB->SetPosition(bodyA->GetPosition() + correction * b_invmass);
}
