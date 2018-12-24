/* HEADER *********************************************************************************/
/*!
\file	PPIf.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Applies a macro to every element

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_IS_EMPTY_H_
#define _PP_IS_EMPTY_H_


#define PP_IS_EMPTY(...) PP_IS_EMPTY_AUX1(PP_IS_EMPTY_AUX2 __VA_ARGS__ ())

#define PP_IS_EMPTY_AUX1(_X_) PP_CONCAT(PP_IS_EMPTY_AUX3_,PP_EXTRACT_VA(0, _X_)),0)
#define PP_IS_EMPTY_AUX2() 1
#define PP_IS_EMPTY_AUX3_PP_IS_EMPTY_AUX2 0 PP_REMOVE(
#define PP_IS_EMPTY_AUX3_1 1 PP_REMOVE(


#endif


