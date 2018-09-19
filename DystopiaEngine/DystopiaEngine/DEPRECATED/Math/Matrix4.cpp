/* HEADER *********************************************************************************/
/*!
\file	Matrix4.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	4x4 Matrix.
	The elements of the matrix are laid out like this:

	| [ 0] [ 1] [ 2] [ 3] |
	| [ 4] [ 5] [ 6] [ 7] |
	| [ 8] [ 9] [10] [11] |
	| [12] [13] [14] [15] |

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Matrix4_old.h"	// File Header
#include "Vector4_old.h"
#include "Utility.h"

namespace Math
{
	Matrix4::Matrix4(void) :
		mfMatrix { 1.f, .0f, .0f, .0f, .0f, 1.f, .0f, .0f, .0f, .0f, 1.f, .0f, .0f, .0f, .0f, 1.f }
	{

	}

	Matrix4::Matrix4(
		const float a, const float b, const float c, const float d,
		const float e, const float f, const float g, const float h,
		const float i, const float j, const float k, const float l,
		const float m, const float n, const float o, const float p
	) : mfMatrix {a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p}
	{

	}

	Matrix4::Matrix4(const float(&_pMatrix)[16]) : mfMatrix {
		_pMatrix[ 0], _pMatrix[ 1], _pMatrix[ 2], _pMatrix[ 3], 
		_pMatrix[ 4], _pMatrix[ 5], _pMatrix[ 6], _pMatrix[ 7], 
		_pMatrix[ 8], _pMatrix[ 9], _pMatrix[10], _pMatrix[11], 
		_pMatrix[12], _pMatrix[13], _pMatrix[14], _pMatrix[15] }
	{

	}

	float Matrix4::Determinant(void) const
	{
		return .0f;
	}

	Matrix4& Matrix4::Identity(void)
	{
		return *this = Mat4{};
	}

	Matrix4& Matrix4::Inverse(void)
	{
		return *this;
	}

	Matrix4& Matrix4::Transpose(void)
	{
		Utility::Swap(mfMatrix[1], mfMatrix[4]);
		Utility::Swap(mfMatrix[2], mfMatrix[8]);
		Utility::Swap(mfMatrix[3], mfMatrix[12]);
		Utility::Swap(mfMatrix[6], mfMatrix[9]);
		Utility::Swap(mfMatrix[7], mfMatrix[13]);
		Utility::Swap(mfMatrix[11], mfMatrix[14]);

		return *this;
	}

	Vec4 Matrix4::GetRow(const unsigned _nRow) const
	{
		unsigned row = _nRow * 4;
		return Vec4{ mfMatrix[row], mfMatrix[row + 1], mfMatrix[row + 2], mfMatrix[row + 3] };
	}

	Vec4 Matrix4::GetColumn(const unsigned _nCol) const
	{
		return Vec4{ mfMatrix[_nCol], mfMatrix[_nCol + 4], mfMatrix[_nCol + 8], mfMatrix[_nCol + 12] };
	}

	float& Matrix4::operator[](const unsigned _nIndex)
	{
		return mfMatrix[_nIndex];
	}

	const float& Matrix4::operator[](const unsigned _nIndex) const
	{
		return mfMatrix[_nIndex];
	}

// ==========================  NON MEMBER FUNCTIONS  ============================= //

	Matrix4 Identity(void)
	{
		return Mat4{};
	}

	Matrix4 Inverse(const Matrix4& _Matrix)
	{
		return Mat4(_Matrix).Transpose();
	}

	Matrix4 Transpose(const Matrix4& _Matrix)
	{
		return Mat4(_Matrix).Transpose();
	}

// ==========================  OPERATOR OVERLOADING  ============================= //

	Matrix4& Matrix4::operator+= (const Matrix4& _rhs)
	{
		for (unsigned n = 0; n < 16; ++n)
			mfMatrix[n] += _rhs[n], ++n;

		return *this;
	}

	Matrix4& Matrix4::operator-= (const Matrix4& _rhs)
	{
		for (unsigned n = 0; n < 16; ++n)
			mfMatrix[n] -= _rhs[n], ++n;

		return *this;
	}

	Matrix4& Matrix4::operator*= (const Matrix4& _rhs)
	{
		Mat4 temp;

		for (unsigned row = 0; row < 4; ++row)
			for (unsigned col = 0; col < 4; ++col)
				temp[4 * row + col] = GetRow(row).Dot(_rhs.GetColumn(col));

		return *this = temp;
	}

	Matrix4& Matrix4::operator*= (const float _rhs)
	{
		for (unsigned n = 0; n < 16; ++n)
			mfMatrix[n] *= _rhs;

		return *this;
	}

	Matrix4& Matrix4::operator/= (const float _rhs)
	{
		return *this *= (1.f / _rhs);
	}

// =================  NON MEMBER OPERATOR OVERLOADING  =========================== //
	Matrix4 operator+ (const Matrix4& _lhs, const Matrix4& _rhs)
	{
		return Mat4( _lhs ) += _rhs;
	}

	Matrix4 operator- (const Matrix4& _lhs, const Matrix4& _rhs)
	{
		return Mat4( _lhs ) -= _rhs;
	}

	Matrix4 operator* (const Matrix4& _lhs, const Matrix4& _rhs)
	{
		return Mat4( _lhs ) *= _rhs;
	}

	Vector4 operator* (const Matrix4& _lhs, const Vector4& _rhs)
	{
		return Vec4{
			_rhs.Dot(_lhs.GetRow(0)),
			_rhs.Dot(_lhs.GetRow(1)),
			_rhs.Dot(_lhs.GetRow(2)),
			_rhs.Dot(_lhs.GetRow(3))
		};
	}

	Matrix4 operator* (const Matrix4& _lhs, const float _rhs)
	{
		return Mat4( _lhs ) *= _rhs;
	}

	Matrix4 operator* (const float _lhs, const Matrix4& _rhs)
	{
		return Mat4( _rhs ) *= _lhs;
	}

	Matrix4 operator/ (const Matrix4& _lhs, const float _rhs)
	{
		return Mat4( _lhs ) /= _rhs;
	}

}


