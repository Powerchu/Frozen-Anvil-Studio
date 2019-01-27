/* HEADER *********************************************************************************/
/*!
\file	PPIsExpandable.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Checks if the given macro argument is wrapped in "()"
	ie. can be expanded by another macro

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_IS_EXPANDABLE_H_
#define _PP_IS_EXPANDABLE_H_

#include "PP/PPUtils.h"
#include "PP/PPConcat.h"
#include "PP/PPRemove.h"


#define PP_IS_EXPANDABLE(_X_) PP_OBSTRUCT(PP_CONCAT)(PP_IS_EXPANDABLE_AUX1,PP_IS_EXPANDABLE_AUX2 _X_))

#define PP_IS_EXPANDABLE_AUX1(...) 1
#define PP_IS_EXPANDABLE_AUX2(...) (
#define PP_IS_EXPANDABLE_AUX1PP_IS_EXPANDABLE_AUX2(...) 0 PP_REMOVE(

#endif


