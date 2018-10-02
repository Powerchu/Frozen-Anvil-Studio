#include "System/Collision/CollisionEvent.h"
#include "System/Logger/LoggerSystem.h"
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
	/*TODO: Change to actual Apply Impulse, setVelocity should not be here, add to Rigidbody's Cumulative Force
	*/
 	const auto bodyA = mThisCollider->GetComponent<RigidBody>();
	const auto bodyB = mCollidedWith->GetComponent<RigidBody>();
	const auto a_invmass = bodyA->GetInverseMass();
	const auto b_invmass = bodyB->GetInverseMass();
	const auto a_oldVel = bodyA->GetLinearVelocity();
	const auto b_oldVel = bodyB->GetLinearVelocity();
	const float slop = 0.1F;

	//if (mThisCollider->GetName() == "Another box" && mCollidedWith->GetName() == "Box Object")
	//{
	//	if (mEdgeNormal.MagnitudeSqr() == 0)
	//		__debugbreak();
	//}

	//Debug
	//LoggerSystem::ConsoleLog(eLog::MESSAGE, "%s Collided with %s", mThisCollider->GetName().c_str(), mCollidedWith->GetName().c_str());


	Math::Vec3D rv = b_oldVel - a_oldVel;
					 
	const float contactVel = rv.Dot(mEdgeNormal);

	// Do not resolve if velocities do not meet up
	if (contactVel > 0) return;

	if (contactVel < -10.0F)
	{
		if (!bodyA->GetIsAwake() && !bodyA->Get_IsStaticState())
			bodyA->SetSleeping(false);
		if (!bodyB->GetIsAwake() && !bodyB->Get_IsStaticState())
			bodyB->SetSleeping(false);
	}

	// Calculate Impulse Scalar
	float tmpJ = -(1.0F + mfRestitution) * contactVel  + (slop * (float)mdPeneDepth);
	tmpJ /= a_invmass + b_invmass;

	// Apply Impulse
	Vec3D impulse = tmpJ * mEdgeNormal;

	//// Clamping
	//const Vec3D temp = mCumulativeImpulse;
	//if ((mCumulativeImpulse + impulse).MagnitudeSqr() != 0)
	//	mCumulativeImpulse = mCumulativeImpulse + impulse;
	//else
	//	mCumulativeImpulse = { 0,0,0,0 };

	//impulse = mCumulativeImpulse - temp;

	auto a_newVel = a_oldVel - impulse * a_invmass;
	auto b_newVel = b_oldVel + impulse * b_invmass;

	if (bodyA->GetIsAwake() && !bodyA->Get_IsStaticState())
		bodyA->SetVelocity(a_newVel);

	if (!bodyB->Get_IsStaticState() && !bodyB->GetIsAwake())
		__debugbreak();
	if (bodyB->GetIsAwake() && !bodyB->Get_IsStaticState())
		bodyB->SetVelocity(b_newVel);

	// Calculate Frictional Velocity (vec3D) after normal impulse
	rv = b_newVel - a_newVel;

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
	{
		frictionImpulse = jt * tangent;
	}
	else
	{
		frictionImpulse = -tmpJ * tangent * mfDynamicFrictionCof;
	}

	a_newVel = bodyA->GetLinearVelocity() - frictionImpulse * a_invmass;
	b_newVel = bodyB->GetLinearVelocity() + frictionImpulse * b_invmass;

	if (bodyA->GetIsAwake() && !bodyA->Get_IsStaticState())
		bodyA->SetVelocity(a_newVel);

	if (bodyB->GetIsAwake() && !bodyB->Get_IsStaticState())
		bodyB->SetVelocity(b_newVel);

}

void Dystopia::CollisionEvent::ApplyPenetrationCorrection()
{
	const auto bodyA = mThisCollider->GetComponent<RigidBody>();
	const auto bodyB = mCollidedWith->GetComponent<RigidBody>();
	const double a_invmass = (double)bodyA->GetInverseMass();
	const double b_invmass = (double)bodyB->GetInverseMass();

	const double perc = 0.01F;
	const double slop = 0.01F;

	const Vec3D correction = float(Math::Max((mdPeneDepth) - slop, 0.0) / (a_invmass + b_invmass) * perc) * mEdgeNormal;

	if (bodyA->GetIsAwake() && !bodyA->Get_IsStaticState())
		bodyA->SetPosition(bodyA->GetPosition() - correction * (float)a_invmass);
	/*if (!bodyB->Get_IsStaticState())
		bodyB->SetPosition(bodyB->GetPosition() + correction * b_invmass);*/

}
