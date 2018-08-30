#include "Component\RigidBody.h"
#include "Component\Transform.h"

namespace Dystopia
{
	Dystopia::RigidBody::RigidBody(void)
	{
	}

	Dystopia::RigidBody::~RigidBody(void)
	{
	}

	void Dystopia::RigidBody::Load(void)
	{
	}

	void Dystopia::RigidBody::Init(void)
	{
	}

	void Dystopia::RigidBody::OnDestroy(void)
	{
	}

	void Dystopia::RigidBody::Unload(void)
	{
	}

	RigidBody * Dystopia::RigidBody::Duplicate() const
	{
		return new RigidBody{ *this };
	}

	void Dystopia::RigidBody::Serialise(TextSerialiser &) const
	{
	}

	void Dystopia::RigidBody::Unserialise(TextSerialiser &)
	{
	}

	void Dystopia::RigidBody::Update(float _dt)
	{

	}
	void RigidBody::LateUpdate(float _dt)
	{

		/*Reset Cumulative Force*/
		ResetCumulative();
	}

	void RigidBody::AddForce(Math::Vec3D const & _force)
	{
		mCumulativeVector += _force;
	}

	void RigidBody::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point)
	{
		//AddForce(_force, _point, mOwnerTransform->GetGlobalPosition());
	}

	void RigidBody::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point, Math::Point3D const & _origin)
	{
		Math::Vec3D && displacement_v = _point - _origin;
		Math::Vector3D Angular = Cross(_force, displacement_v) / displacement_v.MagnitudeSqr();
		mAngularVelocity  += Angular;
		mCumulativeVector += _force;
	}

	void RigidBody::ResetCumulative()
	{
		mCumulativeVector = { 0,0,0,0 };
	}

}
