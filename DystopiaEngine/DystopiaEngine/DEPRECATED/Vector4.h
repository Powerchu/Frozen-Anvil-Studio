/* HEADER *********************************************************************************/
/*!
\file	Vector4_old.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Plain old struct for generic math vectors in 4 space

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _VECTOR4_H_
#define _VECTOR4_H_

namespace Math
{
	/*!
	\struct Vector4
	\brief
	Generic Math 4 Dimensional Space Vector
	*/
	struct Vector4
	{
		// Converts the vector to unit length
		Vector4& Normalise(void);
		float Dot(const Vector4&) const;
		// Projects the Vector4 onto the input vector
		Vector4& Project(const Vector4&);

		float Magnitude(void) const;
		float MagnitudeSqr(void) const;

		float& operator[](const unsigned _nIndex);
		const float& operator[](const unsigned _nIndex) const;

		Vector4& operator*=(const float);
		Vector4& operator/=(const float);
		Vector4& operator+=(const Vector4&);
		Vector4& operator-=(const Vector4&);

		union { float x, r; };
		union { float y, g; };
		union { float z, b; };
		union { float w, a; };
	};

	Vector4 Normalise(const Vector4&);
	// Computes the dot product of two vectors
	float Dot(const Vector4&, const Vector4&);
	Vector4 Cross(const Vector4&, const Vector4&);
	// Projects lhs onto rhs
	Vector4 Project(const Vector4&, const Vector4&);

	Vector4 operator-(const Vector4&);
	Vector4 operator-(const Vector4&, const Vector4&);
	Vector4 operator+(const Vector4&, const Vector4&);
	Vector4 operator*(const float, const Vector4&);
	Vector4 operator*(const Vector4&, const float);
	Vector4 operator/(const Vector4&, const float);

	using Vec4 = Vector4;
}

#endif

