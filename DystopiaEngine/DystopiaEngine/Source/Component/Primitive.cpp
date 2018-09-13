#include <Component/Primitive.h>
#include <Math/MathUtility.h>

namespace Dystopia
{
	float BoxShape::CalculateInertia()
	{
		// To calculate Moment Of Inertia (Rectangular)
		/*
		 * I = m(h^2 + w^2)/12
		 */
		const auto h_sq = Math::Power<2, float>(m_width);
		const auto w_sq = Math::Power<2, float>(m_height);
		m_momentOfInertia = m_mass * (w_sq + h_sq) / 12;
		return m_momentOfInertia;
	}
}
