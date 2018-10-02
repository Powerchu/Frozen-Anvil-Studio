/* HEADER *********************************************************************************/
/*!
\file	Quaternion.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Implementation of Quaternion Rotations

	NOTE: Cannot inherit Vector4 due to it and this being unions

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _QUARTERNION_H_
#define _QUARTERNION_H_

#include "Math/Vector4.h"	// Vector4
#include "Math/Matrix4.h"	// Matrix4
#include "System/Graphics/Shader.h"


namespace Math
{
	#define _CALL	__vectorcall
	
	class Angle;

	/*!
	\struct Quaternion
	\brief
		Representing rotations
	*/
	union __declspec(align (alignof(Vector4))) _DLL_EXPORT Quaternion
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		inline Quaternion(void) noexcept;
		inline Quaternion(const Quaternion&) noexcept;
		inline explicit Quaternion(float x, float y, float z, float w) noexcept;
		inline explicit Quaternion(__m128) noexcept;


		// ==================================== VECTOR OPERATIONS ==================================== // 

		// Converts the Quaternion to unit length
		inline Quaternion& _CALL Normalise(void);

		inline Quaternion& _CALL Identity(void);

		inline float _CALL Magnitude(void) const;
		inline float _CALL MagnitudeSqr(void) const;

		inline Quaternion& _CALL Conjugate(void) noexcept;

		template <NegateFlag FLAGS>
		inline Quaternion& _CALL Negate(void) noexcept;

		Vector4 _CALL Rotate(Vector4) const noexcept;

		Matrix4 _CALL Matrix(void) const noexcept;

		Vector4 _CALL ToEuler(void) const;



		// ======================================== OPERATORS ======================================= // 

		inline float& _CALL operator[](const size_t _nIndex);
		inline float  _CALL operator[](const size_t _nIndex) const;

		inline Quaternion  _CALL operator-(void) const;
		inline Quaternion  _CALL operator* (const Quaternion);
		inline Quaternion& _CALL operator*=(const float);
		inline Quaternion& _CALL operator*=(const Quaternion);
		inline Quaternion& _CALL operator/=(const float);
		inline Quaternion& _CALL operator+=(const Quaternion);
		inline Quaternion& _CALL operator-=(const Quaternion);

		
		// ================================= QUATERNION GENERATORS ================================= // 

		static Quaternion _CALL FromEuler(Angle _x, Angle _y, Angle _z);


	private:

		Math::Vector4 mData;
	};

	// Converts a vector into unit length
	inline Quaternion _CALL Normalise(const Quaternion);

	// Computes the dot product of two Quaternions
	inline float _CALL Dot(const Quaternion, const Quaternion);

	// Computes the cross product of two Quaternions
	inline Quaternion _CALL Cross(const Quaternion, const Quaternion);

	// Projects lhs onto rhs
	inline Quaternion _CALL Project(const Quaternion, const Quaternion);

	inline Quaternion _CALL Conjugate(const Quaternion);

	inline Vector4 _CALL Reflect(const Vector4, const Quaternion);

	inline Matrix4 _CALL Matrix(const Quaternion) noexcept;


	// ======================================== OPERATORS ======================================== // 

	inline Quaternion _CALL operator-(const Quaternion, const Quaternion);
	inline Quaternion _CALL operator+(const Quaternion, const Quaternion);
	inline Quaternion _CALL operator*(const float, const Quaternion);
	inline Quaternion _CALL operator*(const Quaternion, const float);
	inline Quaternion _CALL operator/(const Quaternion, const float);
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline Math::Quaternion::Quaternion(void) noexcept
	: mData{ 0, 0, 0, 1.f }
{

}

inline Math::Quaternion::Quaternion(const Quaternion& _Q) noexcept
	: mData{ _Q.mData }
{

}

inline Math::Quaternion::Quaternion(float x, float y, float z, float w) noexcept
	: mData{ x, y, z, w }
{

}

inline Math::Quaternion::Quaternion(__m128 _Q) noexcept
	: mData{ _Q }
{

}

inline Math::Quaternion& _CALL Math::Quaternion::Normalise(void)
{
	mData.Normalise();
	return *this;
}

inline Math::Quaternion& _CALL Math::Quaternion::Identity(void)
{
	return *this = Quaternion{};
}

inline Math::Quaternion _CALL Math::Normalise(Quaternion _Q)
{
	return _Q.Normalise();
}

inline Math::Quaternion& _CALL Math::Quaternion::Conjugate(void) noexcept
{
	mData.Negate<NegateFlag::XYZ>();
	return *this;
}

inline Math::Quaternion _CALL Math::Conjugate(Quaternion _Q)
{
	return _Q.Conjugate();
}


inline float _CALL Math::Quaternion::Magnitude(void) const
{
	return mData.Magnitude();
}

inline float _CALL Math::Quaternion::MagnitudeSqr(void) const
{
	return mData.MagnitudeSqr();
}

template <Math::NegateFlag FLAGS>
inline Math::Quaternion& _CALL Math::Quaternion::Negate(void) noexcept
{
	mData.Negate<FLAGS>();
	return *this;
}

inline Math::Matrix4 _CALL Math::Matrix(const Quaternion _Q) noexcept
{
	return _Q.Matrix();
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


inline Math::Quaternion& _CALL Math::Quaternion::operator*=(const float _fScalar)
{
	mData *= _fScalar;
	return *this;
}

inline Math::Quaternion&_CALL Math::Quaternion::operator*=(const Quaternion _rhs)
{
	return *this = *this * _rhs;
}

inline Math::Quaternion& _CALL Math::Quaternion::operator/=(const float _fScalar)
{
	return *this *= (1/_fScalar);
}

inline Math::Quaternion& _CALL Math::Quaternion::operator+=(const Quaternion _rhs)
{
	mData += _rhs.mData;
	return *this;
}

inline Math::Quaternion& _CALL Math::Quaternion::operator-=(const Quaternion _rhs)
{
	mData -= _rhs.mData;
	return *this;
}

inline Math::Quaternion _CALL Math::Quaternion::operator-(void) const
{
	return Quaternion{ *this }.Negate<NegateFlag::XYZW>();
}

inline Math::Quaternion _CALL Math::operator-(Quaternion _lhs, const Quaternion _rhs)
{
	return _lhs -= _rhs;
}

inline Math::Quaternion _CALL Math::operator+(Quaternion _lhs, const Quaternion _rhs)
{
	return _lhs += _rhs;
}

inline Math::Quaternion _CALL Math::operator*(const float _lhs, Quaternion _rhs)
{
	return _rhs *= _lhs;
}

inline Math::Quaternion _CALL Math::operator*(Quaternion _lhs, const float _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Quaternion _CALL Math::operator/(Quaternion _lhs, const float _rhs)
{
	return _lhs /= _rhs;
}

inline float& _CALL Math::Quaternion::operator[](size_t _nIndex)
{
	return mData[_nIndex];
}

inline float _CALL Math::Quaternion::operator[](size_t _nIndex) const
{
	return mData[_nIndex];
}





// Remove all our defines
#ifdef _CALL
#undef _CALL
#endif



#endif		// INCLUDE GUARD

