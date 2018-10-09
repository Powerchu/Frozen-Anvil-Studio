/* HEADER *********************************************************************************/
/*!
\file	PPForEachSeq.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Applies a macro to a "null" terminated sequence

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_FOREACH_SEQ_H_
#define _PP_FOREACH_SEQ_H_

#include "Reflection/PPConcat.h"
#include "Reflection/PPExtract.h"
#include "Reflection/PPPopFront.h"


#define PP_FOREACH_SEQ(ACTION_, _SEQ_) ACTION_ PP_EXTRACT(0, _SEQ_)

//, PP_FOREACH_SEQ(ACTION_, PP_POPFRONT(SEQ_))

#define PP_FOREACH_SEQ0



#endif		// INCLUDE GUARD

