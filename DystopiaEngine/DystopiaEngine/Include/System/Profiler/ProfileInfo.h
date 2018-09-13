/* HEADER *********************************************************************************/
/*!
\file	ProfileInfo.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Information stored by the profiler for each module

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PROFILE_INFO_H_
#define _PROFILE_INFO_H_

#include <map>


namespace Dystopia
{
	struct ProfileInfo
	{
		std::map<std::string, size_t> mTimes;
		size_t mTotal;

		void Clear(void) noexcept;
	};
}



#endif

