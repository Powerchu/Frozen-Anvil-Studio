/* HEADER *********************************************************************************/
/*!
\file	PPToSequence.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Conversion of variadic arguments to sequence

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_TOSEQENCE_H_
#define _PP_TOSEQENCE_H_

#include "Reflection/PPVariadicSize.h"

#define PP_TOSEQ(...) PP_TOSEQ ## PP_VARIADIC_SIZE(__VA_ARGS__) (__VA_ARGS__)

#define PP_TOSEQ0(...)
#define PP_TOSEQ1(_1) (_1) 0
#define PP_TOSEQ2(_1, _2) (_1) (_2) 0
#define PP_TOSEQ3(_1, _2, _3) (_1) (_2) (_3) 0
#define PP_TOSEQ4(_1, _2, _3, _4) (_1) (_2) (_3) (_4) 0
#define PP_TOSEQ5(_1, _2, _3, _4, _5) (_1) (_2) (_3) (_4) (_5) 0
#define PP_TOSEQ6(_1, _2, _3, _4, _5, _6) (_1) (_2) (_3) (_4) (_5) (_6) 0
#define PP_TOSEQ7(_1, _2, _3, _4, _5, _6, _7) (_1) (_2) (_3) (_4) (_5) (_6) (_7) 0
#define PP_TOSEQ8(_1, _2, _3, _4, _5, _6, _7, _8) (_1) (_2) (_3) (_4) (_5) (_6) (_7) (_8) 0
#define PP_TOSEQ9(_1, _2, _3, _4, _5, _6, _7, _8, _9) (_1) (_2) (_3) (_4) (_5) (_6) (_7) (_8) (_9) 0



#endif		// INCLUDE GUARD

