#ifndef PRIMITIVE_H
#define PRIMITIVE_H


namespace Dystopia
{
	struct PrimitiveShape
	{
		float m_mass;
		float m_momentOfInertia;

		PrimitiveShape()
			: m_mass(0)
			, m_momentOfInertia(0)
		{
			
		}

		PrimitiveShape(float m, float iner)
			: m_mass(m)
			, m_momentOfInertia(iner)
		{
			
		}

		// Suppress warnings
		// Unnecessary Copy Constructor
		PrimitiveShape(const PrimitiveShape&) = delete;
		// Unnecessary move constructor
		PrimitiveShape(PrimitiveShape&&) = delete;
		// Unnecessary Copy Operato
		auto operator=(const PrimitiveShape&) = delete;
		// Unnecessary move operator
		auto operator=(PrimitiveShape&&) = delete;

		virtual ~PrimitiveShape() = default;

		virtual float CalculateInertia() = 0;
	};

	struct BoxShape : PrimitiveShape
	{
		float m_width;
		float m_height;

		BoxShape()
			: m_width(0)
			, m_height(0)
		{
			
		}

		BoxShape(float mass, float inertia, float width, float height)
			: PrimitiveShape{mass, inertia}
			, m_width(width)
			, m_height(height)
		{
			
		}

		float CalculateInertia() override;
	};
}

#endif
