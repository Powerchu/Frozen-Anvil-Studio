/* HEADER *********************************************************************************/
/*!
\file	PPUtils.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Force a macro to evalute

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_UTILITY_H_
#define _PP_UTILITY_H_


#define PP_EVAL(...)        PP_EVAL_AUX(__VA_ARGS__)
#define PP_OBSTRUCT(_X_)    _X_ PP_EVAL(PP_NOTHING)()
#define PP_NOTHING()
#define PP_REMOVEBRACE(_X_) _X_


#define PP_EVAL_AUX(_X_)    _X_



#endif		// INCLUDE GUARD

