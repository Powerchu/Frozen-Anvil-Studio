#include "Component/RigidBody.h"
#include "Component/Transform.h"
#include "Object/GameObject.h"

#define GRAVITY_CONSTANT	-9.81F

namespace Dystopia
{
	RigidBody::RigidBody(void)
		: m_PrimitiveShape(new BoxShape())
		, m_OwnerTransform(GetOwner()->GetComponent<Transform>())
		, m_angle(0.0F)
		, m_torque(0.0F)
		, m_drag(0.0F)
		, m_friction(0.1F)
		, m_custom_gravityScale(1.0F)
		, m_gravity{GRAVITY_CONSTANT*m_custom_gravityScale}
		, m_Mass(100.0F)
		, m_InverseMass(1 / m_Mass)
		, m_IsGrounded(false)
		, m_HasGravity(true)
	{
		
	}

	RigidBody::~RigidBody(void)
	{
		delete m_PrimitiveShape;
	}

	void RigidBody::Load(void)
	{
	}

	void RigidBody::Init(void)
	{
		m_custom_gravityScale = m_HasGravity ? m_custom_gravityScale : 0.0F;
		m_gravity = GRAVITY_CONSTANT * m_custom_gravityScale;
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
		ConsolePrint("transform: x(" + std::to_string(m_OwnerTransform->GetGlobalPosition().x) + ')' +
									"y(" + std::to_string(m_OwnerTransform->GetGlobalPosition().y) + ')'
					+ "angle: " + std::to_string(m_angle));
	}

	void RigidBody::Update(float _dt)
	{
		// Downward force due to gravity
		const Math::Vec3D force_gravity = { 0, m_Mass*m_gravity, 0 }; // due to gravity
		AddForce(force_gravity);
		
		const Math::Vec3D acceleration = { m_Force.x / m_Mass, m_Force.y / m_Mass, 0};
		m_LinearVelocity += acceleration * _dt;
		Math::Point3D position{};
		position += m_LinearVelocity * _dt;

		m_OwnerTransform->SetGlobalPosition(position);
	}

	void RigidBody::LateUpdate(float _dt)
	{
		/*Reset Cumulative Force*/
		ResetCumulative();
	}

	void RigidBody::AddForce(Math::Vec3D const & _force)
	{
		m_CumulativeVector += _force;
	}

	void RigidBody::AddForce(Math::Vec3D const & _force, Math::Point3D const & _point)
	{
		AddForce(_force, _point, m_OwnerTransform->GetGlobalPosition());
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
		m_AngularVelocity  += angularVel;
		/*Add the the total Linear Veloctiy of the object in this Update Frame*/
		m_CumulativeVector += _force;
	}

	void RigidBody::ResetCumulative()
	{
		m_CumulativeVector = { 0,0,0,0 };
	}

}
