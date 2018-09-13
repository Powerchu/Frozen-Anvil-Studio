#include "Component\Particle.h"
#include "Component\Transform.h"

namespace Dystopia
{
	Particle::Particle(void)
	{
	}

	Particle::~Particle(void)
	{
	}

	void Particle::Load(void)
	{
	}

	void Particle::Init(void)
	{
	}

	void Particle::OnDestroy(void)
	{
	}

	void Particle::Unload(void)
	{
	}

	Particle * Particle::Duplicate() const
	{
		return new Particle{ *this };
	}

	void Particle::Serialise(TextSerialiser &) const
	{
	}

	void Particle::Unserialise(TextSerialiser &)
	{
	}

	void Particle::Update(float _dt)
	{

	}
	void Particle::LateUpdate(float _dt)
	{

		/*Reset Cumulative Force*/
		ResetCumulative();
	}

	void Particle::AddForce(Math::Vec3D const & _force)
	{
		mCumulativeVector += _force;
	}

	void Particle::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point)
	{
		//AddForce(_force, _point, mOwnerTransform->GetGlobalPosition());
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
	void Particle::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point, Math::Point3D const & _origin)
	{
		/*Vector from origin/pivot of the object to the _point where the force is applied*/
		Math::Vec3D && displacement_v = _point - _origin;
		/*Generate the angular velocity as a result of the force being applied at _point*/
		Math::Vector3D Angular = Cross(_force, displacement_v) / displacement_v.MagnitudeSqr();
		/*Add to the total Anuglar velocity of the object in this Update Frame*/
		mAngularVelocity += Angular;
		/*Add the the total Linear Veloctiy of the object in this Update Frame*/
		mCumulativeVector += _force;
	}

	void Particle::ResetCumulative()
	{
		mCumulativeVector = { 0,0,0,0 };
	}

}
