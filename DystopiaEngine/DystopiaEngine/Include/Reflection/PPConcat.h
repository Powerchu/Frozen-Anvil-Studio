/* HEADER *********************************************************************************/
/*!
\file	PPConcat.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Concatenate assist

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_CONCAT_H_
#define _PP_CONCAT_H_


#define PP_CONCAT(_LHS_, _RHS_) PP_CONCAT_AUX1(_LHS_, _RHS_)
#define PP_CONCAT_AUX1(_LHS_, _RHS_) PP_CONCAT_AUX2(0, _LHS_ ## _RHS_)
#define PP_CONCAT_AUX2(_UNUSED_, _RESULT) _RESULT



#endif		// INCLUDE GUARD

