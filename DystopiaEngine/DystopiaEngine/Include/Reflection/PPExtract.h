/* HEADER *********************************************************************************/
/*!
\file	PPExtract.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Extracts the nth element of a sequence

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PP_EXTRACTFRONT_H_
#define _PP_EXTRACTFRONT_H_

#include "Reflection/PPRemove.h"
#include "Reflection/PPConcat.h"


#define PP_EXTRACTFRONT(_SEQ_) PP_EXTRACT(0, _SEQ_)

#define PP_EXTRACT(_INDEX_, _SEQ_) PP_EXTRACT_AUX_1((PP_EXTRACT ## _INDEX_ _SEQ_))

#define PP_EXTRACT_AUX_1(_AUX_)         PP_EXTRACT_AUX_3 (PP_EXTRACT_AUX_2 _AUX_)
#define PP_EXTRACT_AUX_2(_SEQ_, _X_)   _SEQ_
#define PP_EXTRACT_AUX_3(_SEQ_)        _SEQ_


#define PP_EXTRACT0(_X_) _X_, 0
#define PP_EXTRACT1(_X_) PP_EXTRACT0
#define PP_EXTRACT2(_X_) PP_EXTRACT1
#define PP_EXTRACT3(_X_) PP_EXTRACT2
#define PP_EXTRACT4(_X_) PP_EXTRACT3
#define PP_EXTRACT5(_X_) PP_EXTRACT4
#define PP_EXTRACT6(_X_) PP_EXTRACT5
#define PP_EXTRACT7(_X_) PP_EXTRACT6



#endif		// INCLUDE GUARD

