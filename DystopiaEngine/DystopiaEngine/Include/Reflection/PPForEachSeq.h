/* HEADER *********************************************************************************/
/*!
\file	PPForEachSeq.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Applies a macro to a "null" terminated sequence

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_FOREACH_SEQ_H_
#define _PP_FOREACH_SEQ_H_

#include "Reflection/PPRemove.h"
#include "Reflection/PPConcat.h"
#include "Reflection/PPExtract.h"
#include "Reflection/PPPopFront.h"


#define PP_FOREACH_SEQ(_ACTION_, _SEQ_) /*PP_CONCAT(_ACTION_, PP_EXTRACT(0, _SEQ_))*/ PP_FOREACH_SEQ_AUX1(PP_EVAL(PP_POPFRONT(_SEQ_))) (_ACTION_)

#define PP_FOREACH_SEQ_AUX(_X_) PP_FOREACH_SEQ_AUX2(
#define PP_FOREACH_SEQ_AUX0 PP_REMOVE PP_REMOVE(0
#define PP_FOREACH_SEQ_AUX1(_SEQ_) PP_CONCAT(PP_FOREACH_SEQ_AUX, _SEQ_))
#define PP_FOREACH_SEQ_AUX2(_SEQ_) PP_FOREACH_SEQ_AUX4 (_SEQ_, PP_FOREACH_SEQ_AUX3
#define PP_FOREACH_SEQ_AUX3(_X_) _X_ )
#define PP_FOREACH_SEQ_AUX4(_SEQ_, _ACTION_) PP_FOREACH_SEQ(_ACTION_, _SEQ_)



#endif		// INCLUDE GUARD

