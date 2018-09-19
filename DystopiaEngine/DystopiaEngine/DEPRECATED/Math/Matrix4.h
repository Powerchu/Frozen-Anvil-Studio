/* HEADER *********************************************************************************/
/*!
\file	Matrix4.h
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
#ifndef _MATRIX_4_H_
#define _MATRIX_4_H_

namespace Math
{
	struct Vector4;

	struct Matrix4
	{
	public:
		Matrix4(void);
		Matrix4(
			const float a, const float b, const float c, const float d,
			const float e, const float f, const float g, const float h,
			const float i, const float j, const float k, const float l,
			const float m, const float n, const float o, const float p
		);
		explicit Matrix4(const float(&_pMatrix)[16]);

		// Not Implemented!
		float Determinant(void) const;

		Matrix4& Identity(void);
		// Warning! : Not implemented!
		Matrix4& Inverse(void);
		Matrix4& Transpose(void);

		Vector4 GetRow(const unsigned _nRow) const;
		Vector4 GetColumn(const unsigned _nCol) const;

		float& operator[](const unsigned _nIndex);
		const float& operator[](const unsigned _nIndex) const;

		Matrix4& operator+= (const Matrix4&);
		Matrix4& operator-= (const Matrix4&);
		Matrix4& operator*= (const Matrix4&);
		Matrix4& operator*= (const float);
		Matrix4& operator/= (const float);

	protected:
		//| [00] [01] [02] [03] |
		//| [04] [05] [06] [07] |
		//| [08] [09] [10] [11] |
		//| [12] [13] [14] [15] |
		float mfMatrix[16];

	};

	Matrix4 Identity(const Matrix4&);
	// Warning! : Not implemented!
	Matrix4 Inverse(const Matrix4&);
	Matrix4 Transpose(const Matrix4&);

	Matrix4 operator+ (const Matrix4&, const Matrix4&);
	Matrix4 operator- (const Matrix4&, const Matrix4&);
	Matrix4 operator* (const Matrix4&, const Matrix4&);
	Vector4 operator* (const Matrix4&, const Vector4&);
	Matrix4 operator* (const Matrix4&, const float);
	Matrix4 operator* (const float, const Matrix4&);
	Matrix4 operator/ (const Matrix4&, const float);

	using Mat4 = Matrix4;
}



#endif


