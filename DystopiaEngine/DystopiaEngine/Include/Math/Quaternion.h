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

#include "Math\Vector4.h"	// Vector4
#include "Math\Matrix4.h"	// Matrix4

#include <cmath>			// sin, cos, asin, acos


namespace Math
{
	#define _CALL	__vectorcall
	
	/*!
	\struct Quaternion
	\brief
		Representing rotations
	*/
	union Quaternion
	{
	public:
		// ====================================== CONSTRUCTORS ======================================= // 

		inline Quaternion(void) noexcept;
		inline Quaternion(const Quaternion&) noexcept;
		inline Quaternion(float x, float y = 0, float z = 0, float w = 0) noexcept;
		inline explicit Quaternion(__m128) noexcept;


		// ==================================== VECTOR OPERATIONS ==================================== // 

		// Converts the Quaternion to unit length
		inline Quaternion& _CALL Normalise(void);

		// TODO
		// Computes the dot product of this Quaternion and a given Quaternion
		inline float _CALL Dot(const Quaternion) const;

		// TODO
		// Computes the cross product of this Quaternion and a given Quaternion 
		inline Quaternion& _CALL Cross(const Quaternion);

		// TODO
		// Projects the Quaternion onto the given Quaternion
		inline Quaternion& _CALL Project(const Quaternion);

		inline float _CALL Magnitude(void) const;
		inline float _CALL MagnitudeSqr(void) const;

		inline Quaternion& _CALL Conjugate(void) noexcept;

		template <NegateFlag FLAGS>
		inline Quaternion& _CALL Negate(void) noexcept;

		// TODO
		Matrix4 _CALL Matrix(void) const noexcept;



		// ======================================== OPERATORS ======================================= // 

		//inline float& _CALL operator[](const unsigned _nIndex);
		//inline const float _CALL operator[](const unsigned _nIndex) const;

		inline Quaternion  _CALL operator-(void) const;
		inline Quaternion& _CALL operator*=(const float);
		inline Quaternion& _CALL operator*=(const Quaternion);
		inline Quaternion& _CALL operator/=(const float);
		inline Quaternion& _CALL operator+=(const Quaternion);
		inline Quaternion& _CALL operator-=(const Quaternion);

		
		// ================================= QUATERNION GENERATORS ================================= // 

		static inline Quaternion _CALL FromEuler   (float _x, float _y, float _z);
		static inline Quaternion _CALL FromEulerDeg(float _x, float _y, float _z);


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

	inline Matrix4 _CALL Matrix(const Quaternion) noexcept;

	// ====================================== MATH UTILITY ======================================= // 
	// Manually overload the math utility functions which cannot be called for type Quaternion

	//inline Quaternion _CALL Abs(const Quaternion);

	//inline Quaternion _CALL Min(const Quaternion, const Quaternion);
	//inline Quaternion _CALL Max(const Quaternion, const Quaternion);


	// ======================================== OPERATORS ======================================== // 

	inline Quaternion _CALL operator-(const Quaternion, const Quaternion);
	inline Quaternion _CALL operator+(const Quaternion, const Quaternion);
	inline Quaternion _CALL operator*(const Quaternion, const Quaternion);
	inline Quaternion _CALL operator*(const float, const Quaternion);
	inline Quaternion _CALL operator*(const Quaternion, const float);
	inline Quaternion _CALL operator/(const Quaternion, const float);
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline Math::Quaternion::Quaternion(void) noexcept
	: mData{ }
{

}

inline Math::Quaternion::Quaternion(const Quaternion& _Q) noexcept
	: mData{ _Q.mData }
{

}

inline Math::Quaternion::Quaternion(float x, float y, float z, float w) noexcept
	: mData{ x,y,z,w }
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

inline float _CALL Math::Quaternion::Dot(const Quaternion _rhs) const
{
	return .0f;
}

inline float _CALL Math::Dot(const Quaternion _lhs, const Quaternion _rhs)
{
	return _lhs.Dot(_rhs);
}

inline Math::Quaternion& _CALL Math::Quaternion::Cross(const Quaternion _rhs)
{
	return *this;
}

inline Math::Quaternion _CALL Math::Cross(Quaternion _lhs, Quaternion _rhs)
{
	return _lhs.Cross(_rhs);
}

inline Math::Quaternion& _CALL Math::Quaternion::Project(const Quaternion _rhs)
{
	return *this;
}

// Projects lhs onto rhs
inline Math::Quaternion _CALL Math::Project(Quaternion _lhs, Quaternion _rhs)
{
	return _lhs.Project(_rhs);
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

Math::Matrix4 _CALL Math::Quaternion::Matrix(void) const noexcept
{
	float scale = 2.f / mData.MagnitudeSqr();

	Vec4 t3 = mData.ywyw;
	Vec4 t1 = mData.xxyy * mData.xywz; // xx, xy, yw, yz
	Vec4 t2 = mData.zzzw * mData.zwxy; // zz, zw, xz, yw
	t3 *= t3;						   // yy, ww, yy, ww

	// TODO : Use ~ for alternating operation? alternatives ++ --
	Vec4 t4 = AddSub(t2, t1);				 // Add-Sub -> xx + zz, xy - zw, yw + xz, yz - xw
	Vec4 t1 = AddSub(t1.yxwz, t2.yxwz);      // Add-Sub -> xy + zw, xx - zz, yz + xw, yw - xz


	// Goal :
	// ww + xx - yy - zz,    2 (xy - zw)   ,    2 (xz + cw)   , 0
	//    2 (xy + zw)   , ww - xx + yy - zz,    2 (yz - xw)   , 0
	//    2 (xz - cw)   ,    2 (yz + xw)   , ww - xx - yy + zz, 0
	//          0       ,          0       ,          0       , 1


	return Matrix4{};
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
	return *this;
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

inline Math::Quaternion _CALL Math::operator*(Quaternion _lhs, const Quaternion _rhs)
{
	return _lhs *= _rhs;
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





// Remove all our defines
#ifdef _CALL
#undef _CALL
#endif



#endif		// INCLUDE GUARD

