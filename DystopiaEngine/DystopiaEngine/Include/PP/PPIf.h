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
#ifndef _PP_IF_ELSE_H_
#define _PP_IF_ELSE_H_

#include "PP/PPUtils.h"


#define PP_IF(_COND_, _T, _F)  PP_EVAL(PP_IF_AUX1 PP_OBSTRUCT(PP_NOTHING)()(_COND_, _T, _F))
#define PP_IF_AUX1(_COND_, _T, _F) PP_IF_AUX2 ## _COND_((_T),(_F))

#define PP_IF_AUX20(_T, _F) PP_REMOVEBRACE _F
#define PP_IF_AUX21(_T, _F) PP_REMOVEBRACE _T



#endif		// INCLUDE GUARD

