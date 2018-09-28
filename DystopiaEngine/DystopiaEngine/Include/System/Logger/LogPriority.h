/* HEADER *********************************************************************************/
/*!
\file	LogPriority.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Labels the seriousness of the logged message

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LOGPRIORITY_H_
#define _LOGPRIORITY_H_

#include "Globals.h"

enum class eLog
{
	NONE       = 0,
	ERROR      = 1 << 0,
	WARNING    = 1 << 1,
	MESSAGE    = 1 << 2,
	SYSINFO    = 1 << 3,

	DEFAULT    = ERROR | WARNING | MESSAGE | SYSINFO
};



#endif

