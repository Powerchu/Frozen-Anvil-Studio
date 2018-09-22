#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include "Math/MathUtility.h"


namespace Dystopia
{
	enum class eRigidBodyType
	{
		TRIANGLE,
		RECTANGLE,
		CIRCLE,
		POLYGON,

		TOTAL
	};

	struct Primitive
	{
		Primitive()
			: m_mass(0)
			, m_momentOfInertia(0)
		{
			
		}

		Primitive(float m)
			: m_mass(m)
			, m_momentOfInertia(0)
		{
			
		}

		// Suppress warnings
		// Unnecessary Copy Constructor
		Primitive(const Primitive&) = delete;
		// Unnecessary move constructor
		Primitive(Primitive&&) = delete;
		// Unnecessary Copy Operato
		auto operator=(const Primitive&) = delete;
		// Unnecessary move operator
		auto operator=(Primitive&&) = delete;

		virtual ~Primitive() = default;

		virtual float GetInertia() = 0;

	protected:
		float m_mass;
		float m_momentOfInertia;
	};

	struct BoxShape : Primitive
	{
		float m_width;
		float m_height;

		BoxShape()
			: m_width(0)
			, m_height(0)
		{
			
		}

		BoxShape(float mass, float width, float height)
			: Primitive{mass}
			, m_width(width)
			, m_height(height)
		{
			m_momentOfInertia = m_mass * (Math::Power<2, float>(m_width) +
								Math::Power<2, float>(m_height)) / 12;
		}

		float GetInertia() override
		{
			return m_momentOfInertia;
		}
	};
}

#endif
