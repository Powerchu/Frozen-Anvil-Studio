#include "Component\Collider.h"
#include "Object\GameObject.h"
namespace Dystopia
{
	Circle::Circle()
		: m_radius(1.0F)
		, m_originCentre{ GetOwner()->GetComponent<Transform>()->GetGlobalPosition() }
	{

	}

	Circle::Circle(float const & _radius, Math::Vec3D const & _v3Offset)
		: m_radius(_radius)
		, m_originCentre{ GetOwner()->GetComponent<Transform>()->GetGlobalPosition() + _v3Offset }
	{

	}

	/*Load the Component*/
	void Circle::Load(void)
	{
		
	}
	/*Initialise the Component*/
	void Circle::Init(void)
	{
		
	}

	/*OnDestroy*/
	void Circle::OnDestroy(void)
	{
		
	}

	/*Unload the Component*/
	void Circle::Unload(void)
	{
		
	}
	/*Duplicate the Component*/
	Circle* Circle::Duplicate() const
	{
		//TODO: not correct
		return new Circle{};
	}

	/*Serialise and Unserialise*/
	void Circle::Serialise(TextSerialiser&) const
	{
		
	}
	void Circle::Unserialise(TextSerialiser&)
	{
		
	}

	//TODO: WIP (aaron)
	/*Collision Check Functions*/
	bool Circle::isColliding(const Circle & other_col) const
	{
		UNUSED_PARAMETER(other_col);
		return false;
	}

	bool Circle::isColliding(const Circle * const & other_col) const
	{
		UNUSED_PARAMETER(other_col);
		return false;
	}
}