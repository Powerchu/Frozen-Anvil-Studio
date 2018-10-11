/* HEADER *********************************************************************************/
/*!
\file	PPForceEval.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Force a macro to evalute

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_FORCEEVAL_H_
#define _PP_FORCEEVAL_H_


#define PP_EVAL(...)  PP_EVAL_AUX1(__VA_ARGS__)

#define PP_EVAL_AUX1(_X_) PP_EVAL_AUX2(_X_)
#define PP_EVAL_AUX2(_X_) _X_



#endif		// INCLUDE GUARD

