/* HEADER *********************************************************************************/
/*!
\file	Profiler.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Profiler System

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Profiler\Profiler.h"


const std::map<std::string, Dystopia::ProfileInfo>& Dystopia::Profiler::GetInfo(void) const
{
	return mData;
}


