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
	using ui64_t = unsigned long long;

	template <char SIGN, int EXP, ui64_t MANTISA1, ui64_t MANTISA2>
	struct MetaFloat
	{
		static constexpr int sign = int(SIGN / SIGN);

		template <typename T>
		static constexpr T As = T(sign * .0lL);
	};
}



#endif		// INCLUDE GUARD

