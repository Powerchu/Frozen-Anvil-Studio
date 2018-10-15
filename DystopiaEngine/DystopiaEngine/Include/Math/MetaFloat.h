/* HEADER *********************************************************************************/
/*!
\file	MetaFloat.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Compile Time Float

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _METAFLOAT_H_
#define _METAFLOAT_H_

namespace Math
{
	template <char SIGN, int EXP, unsigned long long MANTISA>
	struct MetaFloat
	{
		static constexpr int sign = int(SIGN / SIGN);


		static constexpr long double LDouble = sign * .0l;
		static constexpr double      Double  = double(LDouble);
		static constexpr float       Float   = float(LDouble);
	};
}



#endif		// INCLUDE GUARD

