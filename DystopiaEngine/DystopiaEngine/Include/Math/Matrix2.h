/* HEADER *********************************************************************************/
/*!
\file	Matrix2.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Class for the 2x2 Matrix

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATRIX2_H_
#define _MATRIX2_H_

#include "Math\Vector4.h"

#include <new>				// nothrow_t
#include <cmath>			// sqrtf
#include <exception>		// bad_alloc

namespace Math
{
	#define _CALL	__vectorcall

	struct Matrix2
	{
		// ====================================== CONSTRUCTORS ======================================= // 

		inline Matrix2(void) noexcept;
		inline Matrix2(const float a, const float b, const float c, const float d) noexcept;
		inline explicit Matrix2(const Vector4) noexcept;
		inline explicit Matrix2(const float(&)[4]) noexcept;

		inline float _CALL Determinant(void) const noexcept;

		inline Matrix2& _CALL Identity(void) noexcept;

		inline Matrix2& _CALL Inverse(void);
		inline Matrix2& _CALL Transpose(void) noexcept;


		// ======================================== OPERATORS ======================================== // 

		inline Matrix2& _CALL operator+= (const Matrix2);
		inline Matrix2& _CALL operator-= (const Matrix2);
		inline Matrix2& _CALL operator*= (const Matrix2);
		inline Matrix2& _CALL operator*= (const float);
		inline Matrix2& _CALL operator/= (const float);

	private:

		Vector4 mData;
	};

	inline Matrix2 _CALL operator+ (const Matrix2, const Matrix2);
	inline Matrix2 _CALL operator- (const Matrix2, const Matrix2);
	inline Matrix2 _CALL operator* (const Matrix2, const Matrix2);
	inline Matrix2 _CALL operator* (const Matrix2, const float);
	inline Matrix2 _CALL operator* (const float, const Matrix2);
	inline Matrix2 _CALL operator/ (const Matrix2, const float);

	using Mat2 = Matrix2;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline Math::Matrix2::Matrix2(void) noexcept :
	mData{ { 1, 0, 0, 1 } }
{

}

inline Math::Matrix2::Matrix2(const float a, const float b, const float c, const float d) noexcept :
	mData{ { a, b, c, d } }
{

}

inline Math::Matrix2::Matrix2(const Vector4 _v) noexcept :
	mData{ _v }
{

}

inline Math::Matrix2::Matrix2(const float(&_fArr)[4]) noexcept :
	mData{ _fArr[0], _fArr[1], _fArr[2], _fArr[3] }
{

}

inline float _CALL Math::Matrix2::Determinant(void) const noexcept
{
	return mData[0] * mData[3] - mData[1] * mData[2];
}

inline Math::Matrix2& _CALL Math::Matrix2::Identity(void) noexcept
{
	return *this = Matrix2{};
}

inline Math::Matrix2& _CALL Math::Matrix2::Inverse(void)
{
	float det = 1.f / Determinant();

	mData = mData.wyzx * Vector4{ det, -det, -det, det };

	return *this;
}

inline Math::Matrix2& _CALL Math::Matrix2::Transpose(void)
{
	mData = mData.xzyw;

	return *this;
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


inline Math::Matrix2& _CALL Math::Matrix2::operator+= (const Matrix2 _rhs)
{
	mData += _rhs.mData;

	return *this;
}

inline Math::Matrix2& _CALL Math::Matrix2::operator-= (const Matrix2 _rhs)
{
	mData -= _rhs.mData;

	return *this;
}

inline Math::Matrix2& _CALL Math::Matrix2::operator*= (const Matrix2 _rhs)
{
	// a b   e f   ae+bg af+bh
	// c d * g h = ce+dg cf+dh
	Vector4 tmp = mData * _rhs.mData.xwxw;
	mData  = mData.yxwz * _rhs.mData.zyzy;
	mData += tmp;

	return *this;
}

inline Math::Matrix2& _CALL Math::Matrix2::operator*= (const float _rhs)
{
	mData *= _rhs;

	return *this;
}

inline Math::Matrix2& _CALL Math::Matrix2::operator/= (const float _rhs)
{
	return *this *= (1.f / _rhs);
}

inline Math::Matrix2 _CALL Math::operator+ (Matrix2 _lhs, const Matrix2 _rhs)
{
	return _lhs += _rhs;
}

inline Math::Matrix2 _CALL Math::operator- (Matrix2 _lhs, const Matrix2 _rhs)
{
	return _lhs -= _rhs;
}

inline Math::Matrix2 _CALL Math::operator* (Matrix2 _lhs, const Matrix2 _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Matrix2 _CALL Math::operator* (Matrix2 _lhs, const float _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Matrix2 _CALL Math::operator* (const float _lhs, Matrix2 _rhs)
{
	return _rhs *= _lhs;
}

inline Math::Matrix2 _CALL Math::operator/ (Matrix2 _lhs, const float _rhs)
{
	return _lhs /= _rhs;
}




// Remove all our defines
#ifdef _CALL
#undef _CALL
#endif



#endif		// INCLUDE GUARD

