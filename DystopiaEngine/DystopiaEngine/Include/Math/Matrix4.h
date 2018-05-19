/* HEADER *********************************************************************************/
/*!
\file	Matrix4.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Experimental, using the experimental Vector4
	All the inlining means that almost everything has to be in the header file

	Reference: https://software.intel.com/sites/landingpage/IntrinsicsGuide

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MATRIX_4_H_
#define _MATRIX_4_H_

#if defined(DEBUG) | defined(_DEBUG)
#include "DebugAssert.h"
#endif // Debug only includes

#include "Math\Vector4.h"		// Vector4
#include "Math\MathUtility.h"	// DegToRad

#include <new>					// nothrow_t
#include <cmath>				// sqrtf
#include <exception>			// bad_alloc
#include <xmmintrin.h>			// SSE
#include <emmintrin.h>			// SSE 2
#include <tmmintrin.h>			// SSE 3
#include <smmintrin.h>			// SSE 4.1

namespace Math
{
	#define _CALL	__vectorcall
	#define ALLIGN	16
	#define USE_DP	0

	struct __declspec(align (ALLIGN)) Matrix4
	{
		// ====================================== CONSTRUCTORS ======================================= // 

		inline Matrix4(void) noexcept;
		inline Matrix4(
			const float a, const float b, const float c, const float d,
			const float e, const float f, const float g, const float h,
			const float i, const float j, const float k, const float l,
			const float m, const float n, const float o, const float p
		) noexcept;
		inline explicit Matrix4(const Vector4(&)[4]) noexcept;
		inline explicit Matrix4(const float (&)[16]) noexcept;


		// ==================================== MATRIX OPERATIONS ==================================== // 

		float _CALL Determinant(void) const;

		inline Matrix4& _CALL Identity(void) noexcept;

		// Not implemented!
		Matrix4& _CALL Inverse(void);
		// Not implemented!
		Matrix4& _CALL AffineInverse(void);

		inline Matrix4& _CALL Transpose(void) noexcept;

		inline Vector4 _CALL GetRow(const unsigned _nRow) const;
		inline Vector4 _CALL GetColumn(const unsigned _nCol) const;
		

		// ======================================== OPERATORS ======================================== // 

		// Potentially slow?, please try to avoid!
		// | [00] [01] [02] [03] |
		// | [04] [05] [06] [07] |
		// | [08] [09] [10] [11] |
		// | [12] [13] [14] [15] |
		inline float& _CALL      operator[](const unsigned _nIndex);
		inline const float _CALL operator[](const unsigned _nIndex) const;

		inline Matrix4& _CALL operator+= (const Matrix4);
		inline Matrix4& _CALL operator-= (const Matrix4);
		inline Matrix4& _CALL operator*= (const Matrix4);
		inline Matrix4& _CALL operator*= (const float);
		inline Matrix4& _CALL operator/= (const float);

		inline Vector4 _CALL operator* (const Vector4) const;

#if !defined(_WIN64)	// We need these for win32 - pending fix in auto array
		//inline void* operator new (std::size_t);
		//inline void* operator new (std::size_t, const std::nothrow_t&) noexcept;
		//inline void* operator new (std::size_t, void*) noexcept;

		//inline void* operator new[](std::size_t);
		//inline void* operator new[](std::size_t, const std::nothrow_t&) noexcept;
		//inline void* operator new[](std::size_t, void*) noexcept;

		//inline void operator delete (void*) noexcept;
		//inline void operator delete (void*, const std::nothrow_t&) noexcept;
		//inline void operator delete (void*, void*) noexcept;

		//inline void operator delete[](void*) noexcept;
		//inline void operator delete[](void*, const std::nothrow_t&) noexcept;
		//inline void operator delete[](void*, void*) noexcept;
#endif					// ! WIN64

	private:
		Vector4 mData[4];
	};

	inline Matrix4 _CALL       Inverse(const Matrix4);
	inline Matrix4 _CALL AffineInverse(const Matrix4);

	inline Matrix4 _CALL Transpose(const Matrix4) noexcept;


	// ==================================== MATRIX GENERATORS ==================================== // 

	inline Matrix4 _CALL Scale(const Vector4);
	inline Matrix4 _CALL Scale(float _fScaleX, float _fScaleY, float _fScaleZ);
	inline Matrix4 _CALL Translate(const Vector4);
	inline Matrix4 _CALL Translate(float _fTranslateX, float _fTranslateY, float _fTranslateZ);
	inline Matrix4 _CALL RotateX(float _fRadians);
	inline Matrix4 _CALL RotateY(float _fRadians);
	inline Matrix4 _CALL RotateZ(float _fRadians);
	inline Matrix4 _CALL RotateXDeg(float _fDegrees);
	inline Matrix4 _CALL RotateYDeg(float _fDegrees);
	inline Matrix4 _CALL RotateZDeg(float _fDegrees);

	inline Matrix4 _CALL RotYTrans(float _fRadians, Vector4);
	inline Matrix4 _CALL RotYTrans(float _fRadians, float _fTranslateX, float _fTranslateY, float _fTranslateZ);
	inline Matrix4 _CALL RotXTrans(float _fRadians, Vector4);
	inline Matrix4 _CALL RotXTrans(float _fRadians, float _fTranslateX, float _fTranslateY, float _fTranslateZ);
	inline Matrix4 _CALL RotZTrans(float _fRadians, Vector4);
	inline Matrix4 _CALL RotZTrans(float _fRadians, float _fTranslateX, float _fTranslateY, float _fTranslateZ);
	inline Matrix4 _CALL RotYTransDeg(float _fDegrees, Vector4);
	inline Matrix4 _CALL RotYTransDeg(float _fDegrees, float _fTranslateX, float _fTranslateY, float _fTranslateZ);
	inline Matrix4 _CALL RotXTransDeg(float _fDegrees, Vector4);
	inline Matrix4 _CALL RotXTransDeg(float _fDegrees, float _fTranslateX, float _fTranslateY, float _fTranslateZ);
	inline Matrix4 _CALL RotZTransDeg(float _fDegrees, Vector4);
	inline Matrix4 _CALL RotZTransDeg(float _fDegrees, float _fTranslateX, float _fTranslateY, float _fTranslateZ);


	// ======================================== OPERATORS ======================================== // 

	inline Matrix4 _CALL operator+ (const Matrix4, const Matrix4);
	inline Matrix4 _CALL operator- (const Matrix4, const Matrix4);
	inline Matrix4 _CALL operator* (const Matrix4, const Matrix4);
	inline Matrix4 _CALL operator* (const Matrix4, const float);
	inline Matrix4 _CALL operator* (const float, const Matrix4);
	inline Matrix4 _CALL operator/ (const Matrix4, const float);

	using Matrix3D	= Matrix4;
	using Mat4		= Matrix4;
	using Mat3D		= Matrix4;
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


inline Math::Matrix4::Matrix4(void) noexcept :
	mData{ {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} }
{

}

inline Math::Matrix4::Matrix4(
	const float a, const float b, const float c, const float d,
	const float e, const float f, const float g, const float h,
	const float i, const float j, const float k, const float l,
	const float m, const float n, const float o, const float p
) noexcept : mData{ {a, b, c, d}, {e, f, g, h}, {i, j, k, l}, {m, n, o, p} }
{

}

inline Math::Matrix4::Matrix4(const Vector4(&_arr)[4]) noexcept :
	mData{ _arr[0], _arr[1], _arr[2], _arr[3] }
{

}

inline Math::Matrix4::Matrix4(const float(&_arr)[16]) noexcept :
	mData{
		{ _arr[ 0], _arr[ 1], _arr[ 2], _arr[ 3] },
		{ _arr[ 4], _arr[ 5], _arr[ 6], _arr[ 7] },
		{ _arr[ 8], _arr[ 9], _arr[10], _arr[11] },
		{ _arr[12], _arr[13], _arr[14], _arr[15] }
	}
{

}

inline Math::Matrix4& _CALL Math::Matrix4::Identity(void) noexcept
{
	return *this = Matrix4{};
}

inline Math::Matrix4& _CALL Math::Matrix4::Transpose(void) noexcept
{
	// It's hard to tell what this is doing so here's a diagram
	// 0 1 2 3  ->  0 4 1 5  ->  0 4 8 C
	// 4 5 6 7  ->  8 C 9 D  ->  1 5 9 D
	// 8 9 A B  ->  2 6 3 7  ->  2 6 A E
	// C D E F  ->  A E B F  ->  3 7 B F

	// Transpose by moving around the internal floats. Note the row order
	__m128 row0 = _mm_unpacklo_ps(mData[0].mData, mData[1].mData);
	__m128 row2 = _mm_unpackhi_ps(mData[0].mData, mData[1].mData);
	__m128 row1 = _mm_unpacklo_ps(mData[2].mData, mData[3].mData);
	__m128 row3 = _mm_unpackhi_ps(mData[2].mData, mData[3].mData);

	// movehl flips the order of assignment
	// so we flip to get back the right order   // ie.
	mData[0].mData = _mm_movelh_ps(row0, row1); // movelh : a1 a2 b1 b2
	mData[1].mData = _mm_movehl_ps(row1, row0); // movehl : b3 b4 a3 a4
	mData[2].mData = _mm_movelh_ps(row2, row3);
	mData[3].mData = _mm_movehl_ps(row3, row2);

	return *this;
}

inline Math::Vector4 _CALL Math::Matrix4::GetRow(const unsigned _nRow) const
{
#if defined(DEBUG) | (_DEBUG)
	DEBUG_ASSERT(_nRow > 3, "Matrix4 Error: GetRow row index out of bounds.");
#endif

	return mData[_nRow];
}

inline Math::Vector4 _CALL Math::Matrix4::GetColumn(const unsigned _nCol) const
{
#if defined(DEBUG) | (_DEBUG)
	DEBUG_ASSERT(_nCol > 3, "Matrix4 Error: GetColumn column index out of bounds.");
#endif

	return Math::Transpose(*this).mData[_nCol];
}

inline Math::Matrix4 _CALL Math::Inverse(Matrix4 _mat)
{
	return _mat.Inverse();
}

inline Math::Matrix4 _CALL Math::AffineInverse(Matrix4 _mat)
{
	return _mat.AffineInverse();
}

inline Math::Matrix4 _CALL Math::Transpose(Matrix4 _mat) noexcept
{
	return _mat.Transpose();
}



// ============================================= MATRIX GENERATORS ============================================== // 


inline Math::Matrix4 _CALL Math::Scale(const Vector4 _vScale)
{
	return Scale(_vScale.x, _vScale.y, _vScale.z);
}

inline Math::Matrix4 _CALL Math::Scale(float _fScaleX, float _fScaleY, float _fScaleZ)
{
	return Matrix4{ _fScaleX, 0, 0, 0, 0, _fScaleY, 0, 0, 0, 0, _fScaleZ, 0, 0, 0, 0, 1.f };
}

inline Math::Matrix4 _CALL Math::Translate(const Vector4 _vTranslate)
{
	return Translate(_vTranslate.x, _vTranslate.y, _vTranslate.z);
}

inline Math::Matrix4 _CALL Math::Translate(float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	return Matrix4{ 1.f, 0, 0, _fTranslateX, 0, 1.f, 0, _fTranslateY, 0, 0, 1.f, _fTranslateZ, 0, 0, 0, 1.f };
}

inline Math::Matrix4 _CALL Math::RotateX(float _fRadians)
{
	float s = sinf(_fRadians), c = cosf(_fRadians);
	return Matrix4{ 1.f, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1 };
}

inline Math::Matrix4 _CALL Math::RotateY(float _fRadians)
{
	float s = sinf(_fRadians), c = cosf(_fRadians);
	return Matrix4{ c, 0, s, 0, 0, 1.f, 0, 0, -s, 0, c, 0, 0, 0, 0, 1.f };
}

inline Math::Matrix4 _CALL Math::RotateZ(float _fRadians)
{
	float s = sinf(_fRadians), c = cosf(_fRadians);
	return Matrix4{ c, -s, 0, 0, s, c, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f };
}

inline Math::Matrix4 _CALL Math::RotateXDeg(float _fDegrees)
{
	return RotateX(DegToRad(_fDegrees));
}

inline Math::Matrix4 _CALL Math::RotateYDeg(float _fDegrees)
{
	return RotateY(DegToRad(_fDegrees));
}

inline Math::Matrix4 _CALL Math::RotateZDeg(float _fDegrees)
{
	return RotateZ(DegToRad(_fDegrees));
}


inline Math::Matrix4 _CALL Math::RotXTrans(float _fRadians, Vector4 _vTrans)
{
	return RotXTrans(_fRadians, _vTrans.x, _vTrans.y, _vTrans.z);
}

inline Math::Matrix4 _CALL Math::RotXTrans(float _fRadians, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	float s = sinf(_fRadians), c = cosf(_fRadians);
	return Matrix4{ 1.f, 0, 0, _fTranslateX, 0, c, -s, _fTranslateY, 0, s, c, _fTranslateZ, 0, 0, 0, 1 };
}

inline Math::Matrix4 _CALL Math::RotYTrans(float _fRadians, Vector4 _vTrans)
{
	return RotYTrans(_fRadians, _vTrans.x, _vTrans.y, _vTrans.z);
}

inline Math::Matrix4 _CALL Math::RotYTrans(float _fRadians, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	float s = sinf(_fRadians), c = cosf(_fRadians);
	return Matrix4{ c, 0, s, _fTranslateX, 0, 1.f, 0, _fTranslateY, -s, 0, c, _fTranslateZ, 0, 0, 0, 1.f };
}

inline Math::Matrix4 _CALL Math::RotZTrans(float _fRadians, Vector4 _vTrans)
{
	return RotZTrans(_fRadians, _vTrans.x, _vTrans.y, _vTrans.z);
}

inline Math::Matrix4 _CALL Math::RotZTrans(float _fRadians, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	float s = sinf(_fRadians), c = cosf(_fRadians);
	return Matrix4{ c, -s, 0, _fTranslateX, s, c, 0, _fTranslateY, 0, 0, 1.f, _fTranslateZ, 0, 0, 0, 1.f };
}

inline Math::Matrix4 _CALL Math::RotXTransDeg(float _fDegrees, Vector4 _vTrans)
{
	return RotXTrans(DegToRad(_fDegrees), _vTrans.x, _vTrans.y, _vTrans.z);
}

inline Math::Matrix4 _CALL Math::RotXTransDeg(float _fDegrees, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	return RotXTrans(DegToRad(_fDegrees), _fTranslateX, _fTranslateY, _fTranslateZ);
}

inline Math::Matrix4 _CALL Math::RotYTransDeg(float _fDegrees, Vector4 _vTrans)
{
	return RotYTrans(DegToRad(_fDegrees), _vTrans.x, _vTrans.y, _vTrans.z);
}

inline Math::Matrix4 _CALL Math::RotYTransDeg(float _fDegrees, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	return RotYTrans(DegToRad(_fDegrees), _fTranslateX, _fTranslateY, _fTranslateZ);
}

inline Math::Matrix4 _CALL Math::RotZTransDeg(float _fDegrees, Vector4 _vTrans)
{
	return RotZTrans(DegToRad(_fDegrees), _vTrans.x, _vTrans.y, _vTrans.z);
}

inline Math::Matrix4 _CALL Math::RotZTransDeg(float _fDegrees, float _fTranslateX, float _fTranslateY, float _fTranslateZ)
{
	return RotZTrans(DegToRad(_fDegrees), _fTranslateX, _fTranslateY, _fTranslateZ);
}



// ============================================ OPERATOR OVERLOADING ============================================ // 


inline float& _CALL      Math::Matrix4::operator[] (const unsigned _nIndex)
{
	return mData[_nIndex >> 2][_nIndex % 4];
}

inline const float _CALL Math::Matrix4::operator[] (const unsigned _nIndex) const
{
	return mData[_nIndex >> 2][_nIndex % 4];
}


inline Math::Matrix4& _CALL Math::Matrix4::operator+= (const Matrix4 _rhs)
{
	for (unsigned n = 0; n < 4; ++n)
		mData[n] += _rhs.mData[n];

	return *this;
}

inline Math::Matrix4& _CALL Math::Matrix4::operator-= (const Matrix4 _rhs)
{
	for (unsigned n = 0; n < 4; ++n)
		mData[n] -= _rhs.mData[n];

	return *this;
}

inline Math::Matrix4& _CALL Math::Matrix4::operator*= (const Matrix4 _rhs)
{
	for (unsigned n = 0; n < 4; ++n)
	{
		Vector4 a = mData[n].xxxx;
		Vector4 b = mData[n].yyyy;
		Vector4 c = mData[n].zzzz;
		Vector4 d = mData[n].wwww;

		a *= _rhs.mData[0];
		b *= _rhs.mData[1];
		c *= _rhs.mData[2];
		d *= _rhs.mData[3];

		a += b;
		c += d;

		mData[n] = a + c;
	}

	return *this;
}

inline Math::Matrix4& _CALL Math::Matrix4::operator*= (const float _rhs)
{
	for (unsigned n = 0; n < 4; ++n)
		mData[n] *= _rhs;

	return *this;
}

inline Math::Matrix4& _CALL Math::Matrix4::operator/= (const float _rhs)
{
	for (unsigned n = 0; n < 4; ++n)
		mData[n] /= _rhs;

	return *this;
}

inline Math::Matrix4 _CALL Math::operator+ (Matrix4 _lhs, const Matrix4 _rhs)
{
	return _lhs += _rhs;
}

inline Math::Matrix4 _CALL Math::operator- (Matrix4 _lhs, const Matrix4 _rhs)
{
	return _lhs -= _rhs;
}

inline Math::Matrix4 _CALL Math::operator* (Matrix4 _lhs, const Matrix4 _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Vector4 _CALL Math::Matrix4::operator* (const Vector4 _rhs) const
{
	// No idea if any of these provides any speed up against the plain old version
	Vector4 dot1, dot2, dot3, dot4;

#if USE_DP & defined(_INCLUDED_SMM)	// SSE 4.1
	
	dot1.mData = _mm_dp_ps(mData[0].mData, _rhs.mData, 0xF1); // Store into 1st component
	dot2.mData = _mm_dp_ps(mData[1].mData, _rhs.mData, 0xF2); // Store into 2nd ...
	dot3.mData = _mm_dp_ps(mData[2].mData, _rhs.mData, 0xF4); // Store into 3rd ...
	dot4.mData = _mm_dp_ps(mData[3].mData, _rhs.mData, 0xF8); // Store into 4th ...

	dot1 = dot1 + dot2;
	dot3 = dot3 + dot4;

	return dot1 + dot3;

#elif defined(_INCLUDED_PMM)		// SSE 3

	dot1 = mData[0] * _rhs;
	dot2 = mData[1] * _rhs;
	dot3 = mData[2] * _rhs;
	dot4 = mData[3] * _rhs;

	dot1.mData = _mm_hadd_ps(dot1.mData, dot2.mData);
	dot3.mData = _mm_hadd_ps(dot3.mData, dot4.mData);

	return Vector4{ _mm_hadd_ps(dot1.mData, dot3.mData) };

#else								// Fallback (SSE 2)
	
	dot1 = mData[0] * _rhs;
	dot2 = mData[1] * _rhs;
	dot3 = mData[2] * _rhs;
	dot4 = mData[3] * _rhs;

	// Cause registers to flush to memory? - Apparently not
	// Looks like the compiler changes it to shuffles for us

	return Vector4{
		dot1.x + dot1.y + dot1.z + dot1.w,
		dot2.x + dot2.y + dot2.z + dot2.w,
		dot3.x + dot3.y + dot3.z + dot3.w,
		dot4.x + dot4.y + dot4.z + dot4.w
	};

#endif
}

inline Math::Matrix4 _CALL Math::operator* (Matrix4 _lhs, const float _rhs)
{
	return _lhs *= _rhs;
}

inline Math::Matrix4 _CALL Math::operator* (const float  _lhs, Matrix4 _rhs)
{
	return _rhs *= _lhs;
}

inline Math::Matrix4 _CALL Math::operator/ (Matrix4 _lhs, const float _rhs)
{
	return _lhs /= _rhs;
}

//inline void* Math::Matrix4::operator new (std::size_t _sz)
//{
//	void* ret = _aligned_malloc(_sz, ALLIGN);
//
//	if (ret)
//		return ret;
//
//	throw std::bad_alloc{};
//}
//
//inline void* Math::Matrix4::operator new (std::size_t _sz, const std::nothrow_t&) noexcept
//{
//	return _aligned_malloc(_sz, ALLIGN);
//}
//
//inline void* Math::Matrix4::operator new (std::size_t _sz, void* _ptr) noexcept
//{
//	return ::operator new(_sz, _ptr);
//}
//
//inline void* Math::Matrix4::operator new[](std::size_t _sz)
//{
//	void* ret = _aligned_malloc(_sz, ALLIGN);
//
//	if (ret)
//		return ret;
//
//	throw std::bad_alloc{};
//}
//
//inline void* Math::Matrix4::operator new[](std::size_t _sz, const std::nothrow_t&) noexcept
//{
//	return _aligned_malloc(_sz, ALLIGN);
//}
//
//inline void* Math::Matrix4::operator new[](std::size_t _sz, void* _ptr) noexcept
//{
//	return ::operator new[](_sz, _ptr);
//}
//
//inline void Math::Matrix4::operator delete (void* _ptr) noexcept
//{
//	_aligned_free(_ptr);
//}
//
//inline void Math::Matrix4::operator delete (void* _ptr, const std::nothrow_t&) noexcept
//{
//	delete static_cast<Matrix4*>(_ptr);
//}
//
//inline void Math::Matrix4::operator delete (void*, void*) noexcept
//{
//	return;
//}
//
//inline void Math::Matrix4::operator delete[](void* _ptr) noexcept
//{
//	_aligned_free(_ptr);
//}
//
//inline void Math::Matrix4::operator delete[](void* _ptr, const std::nothrow_t&) noexcept
//{
//	delete[] static_cast<Matrix4*>(_ptr);
//}
//
//inline void Math::Matrix4::operator delete[](void*, void*) noexcept
//{
//	return;
//}



// Remove all our defines
#ifdef USE_DP
#undef USE_DP
#endif
#ifdef ALLIGN
#undef ALLIGN
#endif
#ifdef _CALL
#undef _CALL
#endif



#endif		// INCLUDE GUARD

